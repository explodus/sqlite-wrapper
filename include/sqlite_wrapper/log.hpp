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

namespace db { namespace log {

	namespace buffer
	{
		/// Simplest buffer.
		class basic : private boost::noncopyable
		{
		public:
			db::ostream& output_stream()
			{ return db::cout; }
		};
		typedef boost::shared_ptr< basic > basic_ptr;

	}

	namespace provider
	{
		/// Simplest provider.
		class basic : private boost::noncopyable
		{
		public:

			basic()
				: boost::noncopyable()
				, indent_(0)
				, buffer_( new buffer::basic() )
			{
			}

			buffer::basic_ptr operator() const
			{ return buffer_; }

			void increase_indent()
			{ ++indent_; }

			void decrease_indent()
			{ --indent_; }

			size_t get_indent() const
			{ return indent_; }

		private:
			/// Current indentation level.
			size_t indent_;
			/// 
			buffer::basic_ptr buffer_;
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

		public:
			template <typename OutputStream, typename T>
			static void do_format(
				  OutputStream& stream
				, size_t indent
				, const T& data)
			{
				// Push indent.
				std::fill_n(
					std::ostream_iterator< db::char_type >(stream), 
						indent, DB_TEXT(' ') );
				// Push data.
				stream << data;
				// Push delimiter (end of line).
				stream << std::endl;
			}

			static db::string decorate_message(const db::string& message)
			{ return message; }

			static db::string decorate_scope_open(const db::string& message)
			{ return message; }

			static db::string decorate_scope_close(const db::string& message)
			{ return db::string(DB_TEXT("\\")) + message; }
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

	/// Provides logging functionality.
	/**
	* The log class template provides logging functionality.
	*/
	template 
	<
		  typename Provider = provider::basic
		, typename Format = format::basic
	>
	class base : private boost::noncopyable
	{
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
		base() : boost::noncopyable(), provider_( new provider_type() )
		{ }

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
				  provider_()->output_stream()
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
	public:
		/// The log type.
		typedef Log log_type;

		explicit message(
			  log_type& base_
			, const db::string& msg) : boost::noncopyable()
		{
			// Put message.
			base_ << log_type::formatter_type::decorate_message(msg);
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

		/// Construct a log_scope.
		/**
		* This constructor creates scope statement.
		*
		* @param scope_log The log object that the log_scope will use to display
		* its content.
		*
		* @param scope_message The scope's message. It usually includes scope name.
		*/
		explicit log_scope(
			  log_type& scope_log
			, const db::string& scope_message)
			: boost::noncopyable()
			, log_(scope_log)
			, message(scope_message)
		{
			// Put opening message.
			log_ << log_type::formatter_type::decorate_scope_open(message);
			// Increase indent.
			log_.increase_indent();
		}

		/// Destructor.
		/**
		* Puts closing statement and decreases indentation level.
		*/
		~log_scope()
		{
			// Decrease indent.
			log_.decrease_indent();
			// Put closing message.
			log_ << log_type::formatter_type::decorate_scope_close(message);
		}

	private:
		/// The log associated with the scope.
		log_type& log_;

		/// The message associated with the scope.
		db::string message;
	};

} }

#endif // DB_LOG_HPP_INCLUDED