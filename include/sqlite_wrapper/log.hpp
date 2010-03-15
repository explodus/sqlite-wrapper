///	@file		include/sqlite_wrapper/log.hpp
///	@date		15.3.2010 9:47
///	file base	log
///	file ext	hpp
///
/// @author		T. Schroeder (explodus@gmx.de)
///
/// This software is released under the BSD Licence
///	
///	Using		:
///		This code may be used in compiled form in any way you desire. This
///		file may be redistributed unmodified by any means PROVIDING it is
///		not sold for profit without the authors written consent, and
///		providing that this notice and the authors name is included.
///		This file is provided 'as is' with no expressed or implied warranty.
///		The author accepts no liability if it causes any damage to your 
///		computer.
///	Expect Bugs		:
///		Please let me know of any bugs/mods/improvements.
///		and I will try to fix/incorporate them into this file.
///		Enjoy!
///
///	@brief interface of the log class	

#ifndef DB_LOG_HPP_INCLUDED
#define DB_LOG_HPP_INCLUDED

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include <sqlite_wrapper/config.hpp>
#include <sqlite_wrapper/db.hpp>

namespace db { namespace log {

	namespace buffer
	{
		/// Simplest buffer.
		template<typename T = db::ostream>
		class basic : private boost::noncopyable
		{
		public:
			typename T& output_stream() 
			{ return db::cout; }

			typedef boost::shared_ptr< basic > ptr;
		};

		/// Simplest file buffer.
		template<typename T = db::ostream>
		class file : private boost::noncopyable
		{
			db::ofstream of_;

		public:
			file(const db::string& name) : 
				#ifdef SQLITE_WRAPPER_NARROW_STRING
					of_(name)
				#else
					of_(static_cast<const char*>(
						db::detail::w2a(name.c_str())))
				#endif // SQLITE_WRAPPER_NARROW_STRING
			{
			}

			typename T& output_stream() 
			{ return of_; }

			typedef boost::shared_ptr< file > ptr;
		};

		/// Simplest database buffer.
		template<typename T = db::base>
		class database : private boost::noncopyable
		{
			T db_;

		public:
			database(const db::string& name)
			{
				#ifdef SQLITE_WRAPPER_NARROW_STRING
					db_.connect(name);
				#else
					db_.connect(static_cast<const char*>(
						db::detail::w2a(name.c_str())));
				#endif // SQLITE_WRAPPER_NARROW_STRING

				{
					db::ostringstream s_cr;
					s_cr 
						<< DB_TEXT("CREATE TABLE IF NOT EXISTS ") 
						<< DB_TEXT("log(id as integer autoincrement")
						<< DB_TEXT(", date_time as text")
						<< DB_TEXT(", msg as text);");
					db_.execute_ptr(s_cr.str());
				}
			}

			typename T& output_stream() 
			{ return db_; }

			typedef boost::shared_ptr< database > ptr;
		};
	}

	namespace provider
	{
		/// Simplest universal provider.
		template<typename Buffer>
		class basic : private boost::noncopyable
		{
			basic()
				: boost::noncopyable()
			{
			}
		public:
			typedef Buffer buffer_type;

			basic(typename buffer_type::ptr buffer_ptr)
				: boost::noncopyable()
				, indent_(0)
				, buffer_( buffer_ptr )
			{
			}

			typename buffer_type::ptr get_buffer() const
			{ return buffer_; }

			void increase_indent()
			{ ++indent_; }

			void decrease_indent()
			{ --indent_; }

			size_t indent() const
			{ return indent_; }

		private:
			/// Current indentation level.
			size_t indent_;
			/// 
			typename buffer_type::ptr buffer_;
		};

	}

	namespace format
	{
		/// Simplest formatter.
		class basic
		{
		private:
			basic();
			~basic();

			static db::string get_akt_time()
			{
#ifndef BOOST_NO_STD_LOCALE
				return db::detail::to_sql_string(
					boost::gregorian::day_clock::local_day());
#else
				return db::detail::to_sql_string(db::time::time_ce(0));
#endif
			}

		public:
			template <typename OutputStream>
			static void do_format(
				OutputStream& stream
				, size_t indent
				, const db::string& data)
			{
				for (size_t i(0); i<indent; ++i)
					stream << DB_TEXT(' ');
				
				stream << get_akt_time();
				stream << DB_TEXT(" - ");
				// Push data.
				stream << data.c_str();
				// Push delimiter (end of line).
				stream << std::endl;
			}

			template <typename OutputStream, typename T>
			static void do_format(
				  OutputStream& stream
				, size_t indent
				, const T& data)
			{
				for (size_t i(0); i<indent; ++i)
					stream << DB_TEXT(' ');

				stream << get_akt_time();
				stream << DB_TEXT(" - ");
				// Push data.
				stream << data;
				// Push delimiter (end of line).
				stream << DB_TEXT('\n');
			}

			static db::string decorate_message(const db::string& msg)
			{ 
				return msg; 
				//return db::string(DB_TEXT("[")) + 
				//	msg + 
				//	db::string(DB_TEXT("]")); 
			}

			static db::string decorate_scope_open(const db::string& msg)
			{ return msg; }

			static db::string decorate_scope_close(const db::string& msg)
			{ return db::string(DB_TEXT("\\")) + msg; }
		};

		/// Simplest formatter.
		class database
		{
			database();
			~database();

			static db::string get_akt_time()
			{
#ifndef BOOST_NO_STD_LOCALE
				return db::detail::to_sql_string(
					boost::gregorian::day_clock::local_day());
#else
				return db::detail::to_sql_string(db::time::time_ce(0));
#endif
			}

		public:

			template <typename Database, typename T>
			static void do_format(
				  Database& db_
				, size_t indent
				, const T& data)
			{
				db_.execute_ptr((
					  db::ins(DB_TEXT("log"))
					% db::field(DB_TEXT("date_time"), get_akt_time())
					% db::field(DB_TEXT("msg"), db::detail::to_string(data))));
			}

			static db::string decorate_message(const db::string& msg)
			{ 
				return msg; 
			}

			static db::string decorate_scope_open(const db::string& msg)
			{ return msg; }

			static db::string decorate_scope_close(const db::string& msg)
			{ return db::string(DB_TEXT("\\")) + msg; }
		};

	}

	/*! @enum LogLevel
	Logging level definitions.
	*/
	enum level
	{
		log_undef    =0, //!< undefined level
		log_critical,    //!< critical conditions
		log_error,       //!< error conditions
		log_warning,     //!< warning conditions
		log_notice,      //!< normal, but significant, condition
		log_info,        //!< informational
		log_debug        //!< debug level messages
	};

	extern level global_level;

	/// Provides logging functionality.
	/// The log class template provides logging functionality.
	template 
	<
		  typename Provider = provider::basic
		, typename Format = format::basic
	>
	class base : private boost::noncopyable
	{
		/// Construct a log.
		base() : boost::noncopyable()
		{ }

	public:
		/// The I/O provider type.
		typedef Provider provider_type;
		/// The I/O provider pointer type.
		typedef boost::shared_ptr< provider_type > provider_ptr_type;
		/// The formatter type.
		typedef Format formatter_type;
		/// The log type.
		typedef base<provider_type, formatter_type> log_type;

		/// Construct a log.
		base(const provider_ptr_type& io_provider)
			: boost::noncopyable() , provider_(io_provider)
		{
		}

		/// @brief        Destructor
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return       
		///
		/// @date         15.3.2010 10:25
		///
		~base()
		{
		}

		void increase_indent()
		{
			provider_->increase_indent();
		}

		void decrease_indent()
		{
			provider_->decrease_indent();
		}

		template <typename T>
		base& operator << (const T& data)
		{
			formatter_type::do_format(
				  provider_->get_buffer()->output_stream()
				, provider_->indent()
				, data);
			return *this;
		}

	private:
		/// The provider of output streams utilized by the log.
		provider_ptr_type provider_;
	};

	/// 
	template <typename Log>
	class message : private boost::noncopyable
	{
		Log& base_;
		level last_level_;

	public:
		/// The log type.
		typedef Log log_type;

		explicit message(
		    log_type& _base
			, const db::string& msg
			, const level lvl = db::log::log_error) 
			: boost::noncopyable()
			, base_(_base)
			, last_level_(lvl)
		{
			if (last_level_ <= global_level)
				base_ << 
					log_type::formatter_type::decorate_message(msg); 
		}

		bool operator()(const db::string& msg) const
		{
			if (last_level_ <= global_level)
				base_ << log_type::formatter_type::decorate_message(msg);
			return true;
		}
	};

	/// @brief Wraps scope into a pair of log statements.
	/// The log_routine class wraps the scope of it's declaration
	/// into the pair of log statements. 
	/// This tool automatically increases indentation level for all
	/// inner log messages.
	template <typename Log>
	class scope : private boost::noncopyable
	{
	public:

		/// The log type.
		typedef Log log_type;

		/// @brief        This constructor creates scope statement
		///
		/// <BR>qualifier : boost::noncopyable() , log_(scope_log) , message(scope_message)
		/// <BR>access    public  
		/// @return       
		/// @param        scope_log as log_type & - The log object that the
		///								log_scope will use to display its content
		/// @param        scope_message as const db::string & - The scope's
		///								message. It usually includes scope name.
		///
		/// @date         15.3.2010 10:51
		///
		explicit scope(
			  log_type& scope_log
			, const db::string& scope_message
			, const level lvl = db::log::log_debug)
			: boost::noncopyable()
			, log_(scope_log)
			, message_(scope_message)
			, last_level_(lvl)
		{
			if (last_level_ <= global_level)
			{
				log_ << 
					log_type::formatter_type::decorate_scope_open(message_);
				log_.increase_indent();
			}
		}

		/// @brief Puts closing statement and decreases indentation level
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return       
		///
		/// @date         15.3.2010 10:50
		///
		~scope()
		{
			if (last_level_ <= global_level)
			{
				log_.decrease_indent();
				log_ << 
					log_type::formatter_type::decorate_scope_close(message_);
			}
		}

	private:
		/// The log associated with the scope.
		log_type& log_;

		/// The message associated with the scope.
		db::string message_;

		level last_level_;
	};

} }

#endif // DB_LOG_HPP_INCLUDED