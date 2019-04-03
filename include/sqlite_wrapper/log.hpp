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

#include <sqlite_wrapper/config.hpp>
#include <sqlite_wrapper/db.hpp>

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

namespace db {
namespace log {

  /// @brief Logging level definitions.
  enum level {
    log_undef = 0,  //!< undefined level
    log_critical,   //!< critical conditions
    log_error,      //!< error conditions
    log_warning,    //!< warning conditions
    log_notice,     //!< normal, but significant, condition
    log_info,       //!< informational
    log_debug       //!< debug level messages
  };

  level global_level();
  void global_level(level lvl);

  namespace buffer {
    /// Simplest buffer.
    class SQLITE_WRAPPER_DLLAPI basic : private boost::noncopyable {
    public:
      db::ostream & output_stream() { return db::cout; }

      using ptr = boost::shared_ptr< basic >;
    };

    /// Simplest file buffer.
    ///
    /// @tparam T normally as db::ostream
    ///
    template< typename T = db::ofstream >
    class file : private boost::noncopyable {
      T of_;

    public:
      file(const db::string & name)
        :
#ifdef SQLITE_WRAPPER_NARROW_STRING
        of_(name.c_str())
#else
        of_(static_cast< const char * >(db::detail::w2a(name.c_str())))
#endif  // SQLITE_WRAPPER_NARROW_STRING
      {
      }

      db::ostream & output_stream() { return of_; }

      using ptr = boost::shared_ptr< file >;
    };

    /// Simplest database buffer.
    ///
    /// @tparam T normally as db::base
    ///
    template< typename T = db::base >
    class database : private boost::noncopyable {
      bool is_connected_;

      T db_;

      inline void init() {
        db::ostringstream s_cr;
        s_cr << DB_TEXT("create table if not exists ")
             << DB_TEXT("log(id integer primary key autoincrement") << DB_TEXT(", date_real real")
             << DB_TEXT(", date_text text") << DB_TEXT(", level integer")
             << DB_TEXT(", msg text);");
        db_.execute_ptr(s_cr.str());

        is_connected_ = true;
      }

    public:
      database()
        : is_connected_(false) {}

      bool is_connected() const { return is_connected_; }

      void connect(const db::string & name) {
        try {
#ifdef SQLITE_WRAPPER_NARROW_STRING
          db_.connect(name);
#else
          db_.connect(static_cast< const char * >(db::detail::w2a(name.c_str())));
#endif  // SQLITE_WRAPPER_NARROW_STRING
          init();
        } catch (const db::exception::base & e) { is_connected_ = false; }
      }

      database(const db::string & name) { connect(name); }

#ifdef SQLITE_HAS_CODEC
      void connect(const db::string & name, const std::string & key) {
        try {
#ifdef SQLITE_WRAPPER_NARROW_STRING
          db_.connect(name, key);
#else
          db_.connect(static_cast< const char * >(db::detail::w2a(name.c_str())), key);
#endif  // SQLITE_WRAPPER_NARROW_STRING
          init();
        } catch (const db::exception::base & e) { is_connected_ = false; }
      }

      database(const db::string & name, const std::string & key) { connect(name, key); }
#endif  // SQLITE_HAS_CODEC

      T & output_stream() { return db_; }

      using ptr = boost::shared_ptr< database >;
    };
  }  // namespace buffer

  namespace provider {
    /// Simplest universal provider.
    ///
    /// @tparam Buffer should be buffer from db::log::buffer
    ///
    template< typename Buffer >
    class basic : private boost::noncopyable {
      basic()
        : boost::noncopyable() {}

    public:
      using buffer_type = Buffer;

      basic(typename buffer_type::ptr buffer_ptr)
        : boost::noncopyable()
        , indent_(0)
        , buffer_(buffer_ptr) {}

      typename buffer_type::ptr get_buffer() const { return buffer_; }

      void increase_indent() { ++indent_; }

      void decrease_indent() { --indent_; }

      size_t indent() const { return indent_; }

    private:
      /// Current indentation level.
      size_t indent_;
      ///
      typename buffer_type::ptr buffer_;
    };

  }  // namespace provider

  namespace format {
    /// Simplest formatter.
    class SQLITE_WRAPPER_DLLAPI basic {
    private:
      static db::string get_akt_time() {
#ifndef BOOST_NO_STD_LOCALE
        return db::detail::to_sql_string(boost::posix_time::second_clock::local_time());
#else
        return db::detail::to_sql_string(db::time::time_ce(0));
#endif
      }

      static db::string to_string(db::log::level lvl) {
        switch (lvl) {
          case log_critical: return DB_TEXT("critical"); break;
          case log_error: return DB_TEXT("error"); break;
          case log_warning: return DB_TEXT("warning"); break;
          case log_notice: return DB_TEXT("notice"); break;
          case log_info: return DB_TEXT("info"); break;
          case log_debug: return DB_TEXT("debug"); break;
          case log_undef: return DB_TEXT("undef"); break;
          default: return DB_TEXT("undef");
        }
      }

    public:
      basic() {}
      ~basic() {}

      template< typename Buffer >
      void do_format(
        Buffer & b,
        size_t indent,
        const db::string & data,
        db::log::level lvl = db::log::log_undef) {
        for (size_t i(0); i < indent; ++i) b.output_stream() << DB_TEXT(' ');

        b.output_stream() << get_akt_time();
        b.output_stream() << DB_TEXT(" - ");
        b.output_stream() << to_string(lvl);
        b.output_stream() << DB_TEXT(" - ");
        // Push data.
        b.output_stream() << data.c_str();
        // Push delimiter (end of line).
        b.output_stream() << std::endl;
      }

      template< typename Buffer, typename T >
      void do_format(
        Buffer & b,
        size_t indent,
        const T & data,
        db::log::level lvl = db::log::log_undef) {
        for (size_t i(0); i < indent; ++i) b.output_stream() << DB_TEXT(' ');

        b.output_stream() << get_akt_time();
        b.output_stream() << DB_TEXT(" - ");
        b.output_stream() << to_string(lvl);
        b.output_stream() << DB_TEXT(" - ");
        // Push data.
        b.output_stream() << data;
        // Push delimiter (end of line).
        b.output_stream() << DB_TEXT('\n');
      }

      static db::string decorate_message(const db::string & msg) {
        return msg;
        // return db::string(DB_TEXT("[")) +
        //	msg +
        //	db::string(DB_TEXT("]"));
      }

      static db::string decorate_scope_open(const db::string & msg) { return msg; }

      static db::string decorate_scope_close(const db::string & msg) {
        return db::string(DB_TEXT("\\")) + msg;
      }
    };

#ifndef BOOST_NO_STD_LOCALE
    static const boost::posix_time::ptime time_t_epoch(boost::gregorian::date(1970, 1, 1));
#endif

    /// Simplest formatter.
    class SQLITE_WRAPPER_DLLAPI database {
      struct save {
        double local_time_d;
        string local_time_s;
        unsigned lvl;
        string data;
      };

      std::vector< save > save_;

    public:
      database() {}
      ~database() {}

      template< typename Buffer, typename T >
      void do_format(
        Buffer & b,
        size_t /*indent*/
        ,
        const T & data,
        db::log::level lvl = db::log::log_undef) {
#ifndef BOOST_NO_STD_LOCALE
        // boost::posix_time::ptime time_t_epoch(
        //	boost::gregorian::date(1970, 1, 1));
        boost::posix_time::ptime local_time(boost::posix_time::second_clock::local_time());
        string local_time_string(db::detail::to_sql_string(local_time));
        double local_time_double(
          boost::posix_time::time_duration(local_time - time_t_epoch).total_seconds());
#else
        time_t_ce local_time(db::time::time_ce(0));
        string local_time_string(db::detail::to_sql_string(local_time));
        double local_time_double(local_time);
#endif
        if (!b.is_connected()) {
          save s = {local_time_double,
                    local_time_string,
                    static_cast< unsigned >(lvl),
                    db::detail::to_string(data)};

          save_.push_back(s);

          return;
        }

        if (!save_.empty()) {
          b.output_stream().begin();

          for (std::vector< save >::const_iterator itb(save_.begin()), ite(save_.end()); itb != ite;
               ++itb) {
            b.output_stream().execute_ptr(
              (db::ins(DB_TEXT("log")) % db::field(DB_TEXT("date_real"), itb->local_time_d) %
               db::field(DB_TEXT("date_text"), itb->local_time_s) %
               db::field(DB_TEXT("level"), itb->lvl) % db::field(DB_TEXT("msg"), itb->data)));
          }

          b.output_stream().execute_ptr(
            (db::ins(DB_TEXT("log")) % db::field(DB_TEXT("date_real"), local_time_double) %
             db::field(DB_TEXT("date_text"), local_time_string) %
             db::field(DB_TEXT("level"), static_cast< unsigned >(lvl)) %
             db::field(DB_TEXT("msg"), db::detail::to_string(data))));

          b.output_stream().commit();
        } else
          b.output_stream().execute_ptr(
            (db::ins(DB_TEXT("log")) % db::field(DB_TEXT("date_real"), local_time_double) %
             db::field(DB_TEXT("date_text"), local_time_string) %
             db::field(DB_TEXT("level"), static_cast< unsigned >(lvl)) %
             db::field(DB_TEXT("msg"), db::detail::to_string(data))));
      }

      static db::string decorate_message(const db::string & msg) { return msg; }

      static db::string decorate_scope_open(const db::string & msg) { return msg; }

      static db::string decorate_scope_close(const db::string & msg) {
        return db::string(DB_TEXT("\\")) + msg;
      }
    };

  }  // namespace format

  /// @brief Provides logging functionality.
  ///
  /// @tparam Provider should be a provider like  db::log::provider::basic
  /// @tparam Format should be a format like  db::log::format::basic
  ///
  template<
    typename Provider = provider::basic< db::log::buffer::basic >,
    typename Format = format::basic >
  class base : private boost::noncopyable {
    /// Construct a log.
    base()
      : boost::noncopyable()
      , last_level_(db::log::log_undef) {}

  public:
    /// The I/O provider type.
    using provider_type = Provider;
    /// The I/O provider pointer type.
    using provider_ptr_type = boost::shared_ptr< provider_type >;
    /// The formatter type.
    using formatter_type = Format;
    /// The log type.
    typedef base< provider_type, formatter_type > log_type;

    /// Construct a log.
    base(const provider_ptr_type & io_provider)
      : boost::noncopyable()
#if defined(SQLITE_WRAPPER_USE) && defined(SQLITE_WRAPPER_USE) == 1
      , provider_(io_provider)
#endif
      , last_level_(db::log::log_undef) {
    }

    /// @brief        Destructor
    ///
    /// <BR>qualifier
    /// <BR>access    public
    /// @return
    ///
    /// @date         15.3.2010 10:25
    ///
    ~base() {}

    void increase_indent() {
#if defined(SQLITE_WRAPPER_USE) && defined(SQLITE_WRAPPER_USE) == 1
      if (provider_) provider_->increase_indent();
#endif
    }

    void decrease_indent() {
#if defined(SQLITE_WRAPPER_USE) && defined(SQLITE_WRAPPER_USE) == 1
      if (provider_) provider_->decrease_indent();
#endif
    }

    base & operator<<(const level & data) {
#if defined(SQLITE_WRAPPER_USE) && defined(SQLITE_WRAPPER_USE) == 1
      last_level_ = data;
#endif
      return *this;
    }

    template< typename T >
    base & operator<<(const T & data) {
      if (!provider_) return *this;
      if (!provider_->get_buffer()) return *this;

#if defined(SQLITE_WRAPPER_USE) && defined(SQLITE_WRAPPER_USE) == 1
      f_.do_format(*provider_->get_buffer(), provider_->indent(), data, last_level_);
#endif
      return *this;
    }

    provider_ptr_type get_provider() const { return provider_; }

  private:
    /// The provider of output streams utilized by the log.
    provider_ptr_type provider_;

    level last_level_;

    formatter_type f_;
  };

  /// constructs a message
  ///
  /// @tparam Log should be a db::log::base
  template< typename Log >
  class message : private boost::noncopyable {
    Log & base_;
    level last_level_;

  public:
    /// The log type.
    using log_type = Log;

    explicit message(log_type & _base, const db::string & msg, const level lvl = db::log::log_error)
      : boost::noncopyable()
      , base_(_base)
      , last_level_(lvl) {
#if defined(SQLITE_WRAPPER_USE) && defined(SQLITE_WRAPPER_USE) == 1
      if (last_level_ <= global_level())
        base_ << last_level_ << log_type::formatter_type::decorate_message(msg);
#endif
    }

    bool operator()(const db::string & msg) const {
#if defined(SQLITE_WRAPPER_USE) && defined(SQLITE_WRAPPER_USE) == 1
      if (last_level_ <= global_level())
        base_ << last_level_ << log_type::formatter_type::decorate_message(msg);
#endif
      return true;
    }

    bool operator()(const level & lvl, const db::string & msg) const {
#if defined(SQLITE_WRAPPER_USE) && defined(SQLITE_WRAPPER_USE) == 1
      if (lvl <= global_level()) base_ << lvl << log_type::formatter_type::decorate_message(msg);
#endif
      return true;
    }
  };

  /// @brief Wraps scope into a pair of log statements.
  /// The scope class wraps the scope of it's declaration into the pair of log statements.
  /// This tool automatically increases indentation level for all inner log messages.
  template< typename Log >
  class scope : private boost::noncopyable {
  public:
    /// The log type.
    using log_type = Log;

    /// @brief        This constructor creates a scope statement
    ///
    /// <BR>qualifier : boost::noncopyable() , log_(scope_log) , message(scope_message)
    /// <BR>access    public
    /// @return
    /// @param        scope_log as log_type & - The log object that the
    ///								log_scope will use to display its content
    /// @param        scope_message as const db::string & - The scope's
    ///								message. It usually includes scope name.
    /// @param        lvl as const level
    ///
    /// @date         15.3.2010 10:51
    ///
    explicit scope(
      log_type & scope_log,
      const db::string & scope_message,
      const level lvl = db::log::log_debug)
      : boost::noncopyable()
#if defined(SQLITE_WRAPPER_USE) && defined(SQLITE_WRAPPER_USE) == 1
      , log_(scope_log)
      , message_(scope_message)
      , last_level_(lvl)
#endif
    {
#if defined(SQLITE_WRAPPER_USE) && defined(SQLITE_WRAPPER_USE) == 1
      if (last_level_ <= global_level()) {
        log_ << last_level_ << log_type::formatter_type::decorate_scope_open(message_);
        log_.increase_indent();
      }
#endif
    }

    /// @brief Puts closing statement and decreases indentation level
    ///
    /// <BR>qualifier
    /// <BR>access    public
    /// @return
    ///
    /// @date         15.3.2010 10:50
    ///
    ~scope() {
#if defined(SQLITE_WRAPPER_USE) && defined(SQLITE_WRAPPER_USE) == 1
      if (last_level_ <= global_level()) {
        log_.decrease_indent();
        log_ << last_level_ << log_type::formatter_type::decorate_scope_close(message_);
      }
#endif
    }

  private:
#if defined(SQLITE_WRAPPER_USE) && defined(SQLITE_WRAPPER_USE) == 1
    /// The log associated with the scope.
    log_type & log_;

    /// The message associated with the scope.
    db::string message_;

    level last_level_;
#endif
  };

  namespace singleton {
    namespace db {
      using buffer_type = ::db::log::buffer::database<::db::base >;
      using provider_type = ::db::log::provider::basic< buffer_type >;
      using format_type = ::db::log::format::database;

      using log_type = ::db::log::base< provider_type, format_type >;

      using log_scope = ::db::log::scope< log_type >;
      using log_msg = ::db::log::message< log_type >;

      /// @brief        get a log singleton for logging into a sqlite database
      ///
      /// <BR>qualifier
      /// <BR>access    public
      ///
      /// @return       log_type&
      /// @param        name as const string &
      ///
      /// @author       T. Schroeder (explodus@gmx.de)
      /// @date         27.3.2010 8:56
      ///
      log_type & get_log(const string & name = DB_TEXT("log.db"));
    }  // namespace db

    namespace basic {
      using buffer_type = ::db::log::buffer::basic;
      using provider_type = ::db::log::provider::basic< buffer_type >;
      using format_type = ::db::log::format::basic;

      using log_type = ::db::log::base< provider_type, format_type >;

      using log_scope = ::db::log::scope< log_type >;
      using log_msg = ::db::log::message< log_type >;

      /// @brief        get a log singleton for logging in the db::cout ofstream
      ///
      /// <BR>qualifier
      /// <BR>access    public
      /// @return       log_type&
      ///
      /// @author       T. Schroeder (explodus@gmx.de)
      /// @date         29.3.2010 11:37
      ///
      log_type & get_log();
    }  // namespace basic

    namespace io {
      using buffer_type = ::db::log::buffer::file<::db::ofstream >;
      using provider_type = ::db::log::provider::basic< buffer_type >;
      using format_type = ::db::log::format::basic;

      using log_type = ::db::log::base< provider_type, format_type >;

      using log_scope = ::db::log::scope< log_type >;
      using log_msg = ::db::log::message< log_type >;

      /// @brief        get a log singleton for logging into a file
      ///
      /// <BR>qualifier
      /// <BR>access    public
      /// @return       log_type&
      /// @param        name as const string &
      ///
      /// @author       T. Schroeder (explodus@gmx.de)
      /// @date         29.3.2010 11:45
      ///
      log_type & get_log(const string & name = DB_TEXT("log.txt"));
    }  // namespace io
  }    // namespace singleton

}  // namespace log
}  // namespace db

#include <sqlite_wrapper/impl/log_level.ipp>

#endif  // DB_LOG_HPP_INCLUDED
