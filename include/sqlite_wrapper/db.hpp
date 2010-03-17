///	@file		include/sqlite_wrapper/db.hpp
///	@date		09.03.2010	14:52
///	file base	db
///	file ext	h
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
///	@brief interface of the db class	
///        purpose: c++ sqlite handling

#ifndef db_h__
#define db_h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning( push, 1 )

#include "config.hpp"

#include <boost/detail/allocator_utilities.hpp>
#include <boost/detail/quick_allocator.hpp> 
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>

#include "sqlite/sqlite3.h"

#pragma warning( pop )

#include "detail/time.hpp"
#include "detail/tools.hpp"
#include "detail/param.hpp"
#include "detail/exception.hpp"
#include "detail/field.hpp"

namespace db
{
	namespace expr
	{
		class base;
		class connective;
		class in;
		class and_;
		class or_;
		class not_;
		class not_eq_;
		class eq;
		class lt;
		class lt_eq;
		class gt;
		class gt_eq;
		class like;
	}

  class row;
  class query;
  class base;
	class sel;
  class del;
  class ins;
  class upd;
  class field;

	/// fieldtype interface
	class field 
	{
	public:
		typedef std::vector< std::pair<string, string> > Values;
	protected:
		string _name, _table, _sourcename;
		param_types _type;
		unsigned _extra;
		Values _values;
		long _length;
	public:
		/// @brief     field
		/// @todo need documentation
		///
		/// <BR>qualifier : _name(n), _type(t), _extra(e_nullExtra), _length(-1)
		/// <BR>access    public  
		/// @return    
		/// @param     n as const string & 
		/// @param     t as const param_types & 
		///
		/// @date      20:2:2009   11:03
		///
		explicit field(const string& n, 
			const param_types& t=e_long)
			: _name(n), _type(t), _extra(e_nullExtra), _length(-1) {}

		/// @brief     field
		///
		/// <BR>qualifier : _name(n), _type(tp(val)), _extra(e_nullExtra), _length(-1)
		/// <BR>access    public  
		/// @return    
		/// @param     n as const string & 
		/// @param     val as const T & 
		///
		/// @date      20:2:2009   11:03
		///
		template<typename T>
		explicit field(const string& n, const T& val)
			: _name(n), _type(tp(val)), _extra(e_nullExtra), _length(-1) 
		{
			_values.push_back(Values::value_type(n, detail::to_string(val))); 
		}

		/// @brief     field
		///
		/// <BR>qualifier : _name(n_tbl.first), _type(t), _table(n_tbl.second), 
		///              _values(vals), _extra(e_nullExtra), _length(-1)
		/// <BR>access    public  
		/// @return    
		/// @param     n_tbl as const string_pair & 
		/// @param     t as const param_types & 
		/// @param     vals as const Values & 
		///
		/// @date      6:8:2009   12:57
		///
		explicit field(const string_pair& n_tbl,
			const param_types& t=e_long, 
			const Values& vals = Values())
			: _name(n_tbl.first), _type(t), _table(n_tbl.second), _values(vals), 
			_extra(e_nullExtra), _length(-1) {}

		/// @brief     field
		///
		/// <BR>qualifier : _name(f._name), _type(f._type), _table(f._table), 
		///              _values(f._values), _extra(f._extra), _length(f._length)
		/// <BR>access    public  
		/// @return    
		/// @param     f as const field &
		///
		/// @date      6:8:2009   10:56
		///
		field(const field& f)
			: _name(f._name), _type(f._type), _table(f._table), _values(f._values), 
			_extra(f._extra), _length(f._length) {}

		/// @brief     field
		///
		/// <BR>qualifier : _type(e_long), _extra(e_nullExtra), _length(-1)
		/// <BR>access    public  
		/// @return    
		///
		/// @date      6:8:2009   11:30
		///
		field() : _type(e_long), _extra(e_nullExtra), _length(-1) {}

		string fullName() const { 
			if (table().length()>0) 
				return  table() + DB_TEXT(".") + name(); 
			else
				return  name(); }

		~field() {}

		string      name()        const { return _name; }
		string      sourcename()  const { return _sourcename; }
		param_types type()        const { return _type; }
		string      table()       const { return _table; }
		const long& length()      const { return _length; }

		field&      set_type(param_types type) { _type = type; return *this; }
		void        set_table(const string& table) { _table = table; }
		void        set_sourcename(const string& sourcename) 
		{_sourcename = sourcename;}
		void        set_length(const long& len) { _length = len; }
		bool        has_extra(field_extra extra) const 
		{ return ((_extra&extra)==1); }
		void        add_extra(field_extra extra) { 
			if(!(_extra&extra)) 
				_extra |= extra; 
		}
		void        remove_extra(field_extra extra) { 
			if(_extra&extra) 
				_extra &= ~extra; 
		}

		/// @brief     values
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return    Values&
		///
		/// @date      20:2:2009   14:26
		///
		Values& values() { return _values; }

		/// @brief     values
		///
		/// <BR>qualifier const
		/// <BR>access    public  
		/// @return    const Values&
		///
		/// @date      20:2:2009   14:26
		///
		const Values& values() const { return _values; }

		expr::in In(const string& set) const;

		expr::in In(const sel& sel) const;

		expr::like Like(const string& s);

		bool operator==(const field & fd) const 
		{
			return fd.fullName() == fullName();
		}
		bool operator!=(const field & fd) const 
		{
			return ! (*this == fd);
		}
	};
	typedef boost::detail::allocator::partial_std_allocator_wrapper<
		boost::detail::quick_allocator<field > > alloc_field; 

	typedef std::pair<field, field> field_pair;

  namespace expr
  {
    ///expression base class
    class base 
    {
    protected:
      split extraTables; //!< extra tables, to be joined
    public:

			/// @brief     default expression is true
      ///
      /// <BR>qualifier const
      /// <BR>access    virtual public  
      /// @return    db::string
      ///
      /// @date      18:2:2009   11:16
      ///
      virtual string str() const { return DB_TEXT("True"); }

      /// @brief     operator string
      ///
      /// <BR>qualifier const
      /// <BR>access    virtual public  
      /// @return    
      ///
      ///
      virtual operator string() const
      { return str(); }

      /// <BR>qualifier const
      /// <BR>access    public  
      /// @return    db::split
      ///
      /// @date      18:2:2009   11:16
      ///
      split getExtraTables() const { 
        return extraTables;
      }

      /// @brief     default virtual destructor
      ///
      /// <BR>qualifier
      /// <BR>access    virtual public  
      /// @return    
      ///
      /// @date      18:2:2009   11:17
      ///
      virtual ~base() {}
    }; 

    class raw : public base 
    {
      string expr;
    public:
      /// @brief     usable for special expressions in WHERE terms
      ///
      /// <BR>qualifier : expr(e)
      /// <BR>access    public  
      /// @return    
      /// @param     e as const string &
      ///
      /// @date      18:2:2009   11:17
      ///
      explicit raw(const string& e) : expr(e) {}

      virtual string str() const { return expr; }
    };

    class connective : public base 
    {
    private:
      string op;
    protected:
      const base &e1, &e2;

      /// @brief     base class to connect 2 expressions
      ///
      /// <BR>qualifier : op(o), e1(e1_), e2(e2_)
      /// <BR>access    protected  
      /// @return    
      /// @param     string o
      /// @param     e1_ as const base &
      /// @param     e2_ as const base &
      ///
      /// @date      18:2:2009   11:20
      ///
      explicit connective(string o, const base & e1_, const base & e2_)
        : op(o), e1(e1_), e2(e2_) { }

    public:        

      /// @brief     str
      ///
      /// <BR>qualifier const
      /// <BR>access    virtual public  
      /// @return    db::string
      ///
      /// @date      20:2:2009   12:55
      ///
      virtual string str() const {
        string res = DB_TEXT("(") + e1.str() + DB_TEXT(") ") + op 
          + DB_TEXT(" (") + e2.str() + DB_TEXT(")");
        return res;
      }
    };  

    class and_ : public connective 
    {
    public:
      /// @brief     connects 2 expressions with the AND operator
      ///
      /// <BR>qualifier : connective(DB_TEXT("and"), e1_, e2_)
      /// <BR>access    public  
      /// @return    
      /// @param     e1_ as const base &
      /// @param     e2_ as const base &
      ///
      /// @date      18:2:2009   11:22
      ///
      explicit and_(const base & e1_, const base & e2_) : 
				connective(DB_TEXT("and"), e1_, e2_) {}

      /// @brief     str
      ///
      /// <BR>qualifier const
      /// <BR>access    public  
      /// @return    db::string
      ///
      /// @date      20:2:2009   12:56
      ///
      virtual string str() const {     
        if (e1.str() == DB_TEXT("True"))
          return e2.str();
        else if (e2.str() == DB_TEXT("True"))
          return e1.str();
        else
          return connective::str();
      }
    };

    class or_ : public connective 
    {
    public:
      /// @brief     connects 2 expressions with the OR operator
      ///
      /// <BR>qualifier : connective(DB_TEXT("or"), e1_, e2_)
      /// <BR>access    public  
      /// @return    
      /// @param     e1_ as const base &
      /// @param     e2_ as const base &
      ///
      /// @date      18:2:2009   11:22
      ///
      explicit or_(const base & e1_, const base & e2_) 
        : connective(DB_TEXT("or"), e1_, e2_) {}

      /// @brief     str
      ///
      /// <BR>qualifier const
      /// <BR>access    public  
      /// @return    db::string
      ///
      /// @date      20:2:2009   12:56
      ///
      virtual string str() const {
        if (e1.str() == DB_TEXT("True"))
          return DB_TEXT("True");
        else if (e2.str() == DB_TEXT("True"))
          return DB_TEXT("True");
        else
          return connective::str();
      }
    };

    class not_ : public base 
    {
    private:
      const base & exp;
    public:
      /// @brief     negates expression
      ///
      /// <BR>qualifier : exp(_exp)
      /// <BR>access    public  
      /// @return    
      /// @param     _exp as const base &
      ///
      /// @date      18:2:2009   11:23
      ///
      explicit not_(const base & _exp) : exp(_exp) {}

      /// @brief     str
      ///
      /// <BR>qualifier const
      /// <BR>access    virtual public  
      /// @return    db::string
      ///
      /// @date      20:2:2009   12:57
      ///
      virtual string str() const {    
        return DB_TEXT("not (")+exp.str()+DB_TEXT(")");
      }

    };

    ///base class for operators in sql terms
    class oper : public base 
    {
    protected:
      const field & _field;
      string op;
      string data;
      bool escape;

			/// @brief 
			/// @todo need documentation
			///
      /// <BR>qualifier
      /// <BR>access    protected  
      /// @return    bool
      /// @param     type_ as const param_types &
      ///
      /// @date      18:2:2009   11:26
      ///
      inline bool check_escape(const param_types& type_)
      {
        switch(type_) 
        {
        case e_string:
        case e_char:
        case e_blob:
        case e_date_time:
          return true;
          break;
        default:
          return false;
        }
      }

      /// @brief     string overload
      ///
      /// <BR>qualifier : _field(fld), op(o), data(d), escape(check_escape(_field.type()))
      /// <BR>access    protected  
      /// @return    
      /// @param     fld as const field & 
      /// @param     o as const string &
      /// @param     d as const string &
      ///
      /// @date      18:2:2009   11:28
      ///
      explicit oper(const field & fld, const string& o, const string& d) 
        : _field(fld), op(o), data(d), escape(check_escape(_field.type())) 
      {
        extraTables.push_back(fld.table());
      }

      template<typename T>
      explicit oper(const field & fld, const string& o, const T& d) 
        : _field(fld), op(o), data(DB_TEXT("0")), 
        escape(check_escape(_field.type())) 
      {
        data = detail::to_string(d);
        extraTables.push_back(fld.table());
      }

      /// @brief     db::field overload
      ///
      /// <BR>qualifier : _field(fld), op(o), data(f2.fullName()), escape(false)
      /// <BR>access    protected  
      /// @return    
      /// @param     fld as const field &
      /// @param     o as const string &
      /// @param     f2 as const field &
      ///
      /// @date      18:2:2009   11:30
      ///
      explicit oper(const field & fld, const string& o, const field &f2) 
        : _field(fld), op(o), data(f2.fullName()), escape(false) 
      {
        extraTables.push_back(fld.table());
      }

    public:
      /// @brief     str
      ///
      /// <BR>qualifier const
      /// <BR>access    virtual public  
      /// @return    db::string
      ///
      /// @date      20:2:2009   13:03
      ///
      virtual string str() const {
        string res;
        res += _field.fullName() + DB_TEXT(" ") + op + DB_TEXT(" ") + 
          (escape ? detail::escape_sql(data) : data);
        return res;
      }
    };

    ///==, is equal operator
    class eq : public oper {
    public:
      /// @brief     ==, is equal operator
      ///
      /// <BR>qualifier : oper<typename T>(fld, DB_TEXT("="), d)
      /// <BR>access    public  
      /// @return    
      /// @param     fld as const field &
      /// @param     d as const T &
      ///
      /// @date      20:2:2009   11:52
      ///
      template<typename T>
      explicit eq(const field & fld, const T& d)
        : oper(fld, DB_TEXT("="), d) 
      {}
    };

    ///<> not equal operator
    class not_eq_ : public oper {
    public:
      /// @brief     not_eq_
      ///
      /// <BR>qualifier : oper(fld, DB_TEXT("<>"), d)
      /// <BR>access    public  
      /// @return    
      /// @param     fld as const field &
      /// @param     d as const T &
      ///
      /// @date      20:2:2009   13:03
      ///
      template<typename T>
      explicit not_eq_(const field & fld, const T& d)
        : oper(fld, DB_TEXT("<>"), d) 
      {}
    };

    ///> greater then operator
    class gt : public oper {
    public:
      /// @brief     gt
      ///
      /// <BR>qualifier : oper(fld, DB_TEXT(">"), d)
      /// <BR>access    public  
      /// @return    
      /// @param     fld as const field &
      /// @param     d as const T &
      ///
      /// @date      20:2:2009   13:02
      ///
      template<typename T>
      explicit gt(const field & fld, const T& d)
        : oper(fld, DB_TEXT(">"), d) 
      {}
    };

    ///greater then or equal operator
    class gt_eq : public oper {
    public:
      /// @brief     gt_eq
      ///
      /// <BR>qualifier : oper(fld, DB_TEXT(">="), d)
      /// <BR>access    public  
      /// @return    
      /// @param     fld as const field &
      /// @param     d as const T &
      ///
      /// @date      20:2:2009   13:01
      ///
      template<typename T>
      explicit gt_eq(const field & fld, const T& d)
        : oper(fld, DB_TEXT(">="), d) 
      {}
    };

    ///lower then operator
    class lt : public oper {
    public:
      /// @brief     lt
      ///
      /// <BR>qualifier : oper(fld, DB_TEXT("<"), d)
      /// <BR>access    public  
      /// @return    
      /// @param     fld as const field &
      /// @param     d as const T &
      ///
      /// @date      20:2:2009   13:01
      ///
      template<typename T>
      explicit lt(const field & fld, const T& d)
        : oper(fld, DB_TEXT("<"), d) 
      {}
    };

    ///lower then or equal operator
    class lt_eq : public oper {
    public:
      /// @brief     lt_eq
      ///
      /// <BR>qualifier : oper(fld, DB_TEXT("<="), d)
      /// <BR>access    public  
      /// @return    
      /// @param     fld as const field &
      /// @param     d as const T &
      ///
      /// @date      20:2:2009   13:01
      ///
      template<typename T>
      explicit lt_eq(const field & fld, const T& d)
        : oper(fld, DB_TEXT("<="), d) 
      {}
    };

    ///like operator
    class like : public oper {
    public:
      /// @brief     like
      ///
      /// <BR>qualifier : oper(fld, DB_TEXT("LIKE"), d)
      /// <BR>access    public  
      /// @return    
      /// @param     fld as const field &
      /// @param     d as const string &
      ///
      /// @date      20:2:2009   13:00
      ///
      explicit like(const field & fld, const string& d)
        : oper(fld, DB_TEXT("LIKE"), d) 
      {}
    };

    ///in Operator
    class in : public oper {
    public:
      /// @brief     in
      ///
      /// <BR>qualifier : oper(fld, DB_TEXT("IN"), DB_TEXT("(")+set+DB_TEXT(")"))
      /// <BR>access    public  
      /// @return    
      /// @param     fld as const field &
      /// @param     set as const string &
      ///
      /// @date      20:2:2009   13:01
      ///
      explicit in(const field & fld, const string& set)
        : oper(fld, DB_TEXT("IN"), DB_TEXT("(")+set+DB_TEXT(")")) 
      {}

      /// @brief     in
      ///
      /// <BR>qualifier
      /// <BR>access    public  
      /// @return    
      /// @param     fld as const field &
      /// @param     s as const sel &
      ///
      /// @date      20:2:2009   13:01
      ///
      explicit inline in(const field & fld, const sel& s);

      /// @brief     str
      ///
      /// <BR>qualifier const
      /// <BR>access    virtual public  
      /// @return    db::string
      ///
      /// @date      20:2:2009   13:01
      ///
      virtual string str() const 
      { return _field.fullName() + DB_TEXT(" ") + op + DB_TEXT(" ") + data; }

    };
  }

  ///dynamic parameter class*/
  class param 
//#ifndef BOOST_NO_STD_LOCALE
//    : sigb::trackable
//#endif // _DEBUG
  {
    string _data;
    param_types _type;
    bool _is_changed;
    unsigned _col; 

    /// @brief     protected constructor
    ///
    /// <BR>qualifier : _type(e_null), _is_changed(false)
    /// <BR>access    public  
    /// @return    
    ///
    /// @date      20:2:2009   14:36
    ///
    param() : _type(e_null), _is_changed(false)
    { _data = DB_TEXT(""); }
  public:
    /// @brief     param
    ///
    /// <BR>qualifier : _col(col), _type(e_null), _is_changed(false)
    /// <BR>access    public  
    /// @return    
    /// @param     col as unsigned
    ///
    /// @date      20:2:2009   14:36
    ///
    param(unsigned col) : _col(col), _type(e_null), _is_changed(false)
    { _data = DB_TEXT(""); }

    /// @brief     ~param
    ///
    /// <BR>qualifier
    /// <BR>access    virtual public  
    /// @return    
    ///
    /// @date      20:2:2009   14:35
    ///
    ~param() 
    {}

    /// @brief     copy constructor
    ///
    /// <BR>qualifier : _col(Param._col), _type(Param._type), _is_changed(false), _data(Param._data)
    /// <BR>access    public  
    /// @return    
    /// @param     Param as const param &
    ///
    /// @date      20:2:2009   14:35
    ///
    param(const param& Param) : _col(Param._col), _type(Param._type), 
      _is_changed(false), _data(Param._data) 
    { }               

    ///
    /// <BR>qualifier const 
    /// <BR>access    public  
    /// @return    const param_types&
    ///
    /// @date      20:2:2009   14:35
    ///
    param_types get_type() const { return _type; }

    /// @brief     set_type
    ///
    /// <BR>qualifier 
    /// <BR>access    public  
    /// @return    void
    /// @param     Type as param_types
    ///
    /// @date      20:2:2009   14:35
    ///
    void set_type(param_types Type) { _type = Type; }

    /// @brief     is param changed?
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    bool&
    ///
    /// @date      20:2:2009   14:35
    ///
    bool& is_changed() { return _is_changed; }

    ///
    /// <BR>qualifier
    /// <BR>access    virtual public  
    /// @return    void
    /// @param     stm as sqlite3_stmt *
    ///
    /// @date      20:2:2009   14:34
    ///
    void set(sqlite3_stmt* stm)
    {
      if (!stm)
      {
        set_null();
        return;
      }

      switch(sqlite3_column_type(stm, _col)) 
      {
      case SQLITE_INTEGER:
        set(sqlite3_column_int(stm, _col));
        break;
      case SQLITE_FLOAT:
        set(sqlite3_column_double(stm, _col));
        break;
      case SQLITE_TEXT:
#ifdef _UNICODE
        set(reinterpret_cast<const wchar_t*>(
          sqlite3_column_text16(stm, _col)));
#else
        set(reinterpret_cast<const char*>(
          sqlite3_column_text(stm, _col)));
#endif // _UNICODE
        break;
      case SQLITE_BLOB:
        {
#ifdef _UNICODE
          std::string blob(reinterpret_cast<const char*>(
            sqlite3_column_blob(stm, _col)));
					std::wstring wblob = detail::a2w(blob.c_str());
          set(wblob);
#else
          set(reinterpret_cast<const char*>(
            sqlite3_column_blob(stm, _col)));
#endif // _UNICODE
        }
        break;
      case SQLITE_NULL:
        set_null();
        break;
      default:
        set_null();
      } 
    }

    ///gesetzt als bool
    void set(bool dat)
    { 
      _type = e_bool;
      _data = dat ? DB_TEXT("TRUE") : DB_TEXT("FALSE");
    } 
    ///gesetzt als int
    void set(int dat)
    { 
      _type = e_int;
      _data = boost::str(format(DB_TEXT("%d")) % dat);
    } 
    ///gesetzt als unsigned
    void set(unsigned dat)
    { 
      _type = e_unsigned;
      _data = boost::str(format(DB_TEXT("%d")) % dat);
    } 
    ///gesetzt als long
    void set(long dat)
    { 
      _type = e_long;
      _data = boost::str(format(DB_TEXT("%ld")) % dat);
    } 
    ///gesetzt als float
    void set(float dat)
    { 
      _type = e_float;
      _data = boost::str(format(DB_TEXT("%f")) % dat);
    } 
    ///gesetzt als double
    void set(double dat)
    { 
      _type = e_double;
      _data = boost::str(format(DB_TEXT("%f")) % dat);
    } 
    ///gesetzt als char
    void set(char dat)
    { 
      _type = e_char;
      _data = dat;
    } 
    ///gesetzt als const db::char_type*
    void set(const db::char_type* dat)
    { 
      _type = e_string;
      _data = dat;
    } 
    ///gesetzt als const string&
    void set(const string& dat, bool isBlob=false)
    { 
      _type = isBlob?e_blob:e_string;
      _data = dat;
    } 

#ifndef BOOST_NO_STD_LOCALE
    ///gesetzt als boost::gregorian::date
    void set(const boost::gregorian::date& dat)
    { 
      _type = e_date_time;
      _data = detail::to_sql_string(dat);
    } 
#else
    ///gesetzt als time_t_ce
    void set(const time_t_ce& dat)
    { 
      _type = e_date_time;
      _data = detail::to_sql_string(dat);
    } 
#endif

    /// @brief     set_null
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    void
    ///
    /// @date      20:2:2009   14:39
    ///
    void set_null() { _type=e_null; _data = DB_TEXT(""); }

    /// @brief     set_with_old_type
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    void
    /// @param     data as const string &
    ///
    /// @date      20:2:2009   14:40
    ///
    void set_with_old_type(const string& data)
    {
      param_types ty(get_type());
      set(data);
      set_type(ty);
    }

    /// @brief     set_with_old_type
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    void
    /// @param     data as const param &
    ///
    /// @date      20:2:2009   14:40
    ///
    void set_with_old_type(const param& data)
    {
      param_types ty(get_type());
      set(data.str());
      set_type(ty);
    }

    /// @brief     set_with_old_type
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    void
    /// @param     data as const param *
    ///
    /// @date      20:2:2009   14:40
    ///
    void set_with_old_type(const param* data)
    {
      if (!data)
        return;
      param_types ty(get_type());
      set(data->str());
      set_type(ty);
    }

    /// @brief     is_null
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    bool
    ///
    /// @date      20:2:2009   14:33
    ///
    bool is_null() const { return _type==e_null ? true : false; }

    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    bool
    ///
    /// @date      20:2:2009   14:40
    ///
    bool get_bool() const
    {
      if (_type == e_bool)
        return _data == DB_TEXT("TRUE") ? true : false;
      else
        return false; 
    } 


    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    int
    ///
    /// @date      20:2:2009   14:40
    ///
    int get_int() const
    {
      return detail::to_type<int>(_data);
    }

    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    unsigned
    ///
    /// @date      20:2:2009   14:42
    ///
    unsigned get_unsigned() const
    {
      return detail::to_type<unsigned>(_data);
    }

    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    long
    ///
    /// @date      11:3:2009   13:56
    ///
    long get_long() const
    {
      return detail::to_type<long>(_data);
    }

    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    float
    ///
    /// @date      11:3:2009   13:56
    ///
    float get_float() const
    {
      return detail::to_type<float>(_data);
    }

    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    double
    ///
    /// @date      11:3:2009   13:57
    ///
    double get_double() const
    {
      return detail::to_type<double>(_data);
    }

    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    char
    ///
    /// @date      11:3:2009   13:57
    ///
    db::char_type get_char() const 
    {
      if (_data.length()==0)
        return DB_TEXT(' ');
      else
        return _data[0];
    }

    operator string() const
    { return _data; }

    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    db::string
    ///
    /// @date      18:2:2009   11:33
    ///
    string& str()
    { return _data; }

    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    const string&
    ///
    /// @date      18:2:2009   11:33
    ///
    const string& str() const
    { return _data; }

#ifdef _UNICODE
    const wchar_t* c_str() const
    { return _data.c_str(); }
#else
    const char* c_str() const
    { return _data.c_str(); }
#endif // _UNICODE

#ifndef BOOST_NO_STD_LOCALE
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    boost::gregorian::date
    ///
    /// @date      18:2:2009   11:34
    ///
    boost::gregorian::date get_date_time() const
    {
#ifdef _UNICODE
			std::string data = detail::w2a(_data.c_str());
      return boost::date_time::parse_date<
        boost::gregorian::date>(data);
#else
      return boost::date_time::parse_date<
        boost::gregorian::date>(_data);
#endif // _UNICODE
    }
#else
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    db::time_t_ce
    /// @param     tmp as const time_t_ce *
    ///
    /// @date      19:3:2009   10:36
    ///
    time_t_ce get_date_time(const time_t_ce* tmp = 0) const
    { return detail::from_sql_string(_data, tmp); }
#endif

#ifdef _MFC_VER
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    COleDateTime
    /// @param     tmp as const COleDateTime *
    ///
    /// @date      19:3:2009   10:36
    ///
    COleDateTime get_date_time(const COleDateTime* tmp = 0) const
    { return detail::from_sql_string(_data, tmp); }
#endif // _MFCVER

    /// @brief     operator=
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    const param&
    /// @param     v as const param &
    ///
    /// @date      20:2:2009   9:24
    ///
    const param& operator=( const param &v )
    {
      _data = v._data;
      _type = v._type;
      _is_changed = v._is_changed;
      return *this; 
    }

    /// @brief     operator!
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    bool
    ///
    /// @date      20:2:2009   9:27
    ///
    bool operator!() const 
    { return !this->is_null(); }

    /// @brief     operator==
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    bool
    /// @param     v as const param &
    ///
    /// @date      20:2:2009   9:24
    ///
    bool operator== (const param& v) const 
    { return (!(*this) && (!v) && _data == v._data); }

//#ifndef BOOST_NO_STD_LOCALE
//    typedef boost::signal2<void, const string&, bool&> type_signal_external_check; 
//    type_signal_external_check signal_external_check;
//#endif // BOOST_NO_STD_LOCALE
  };
  typedef boost::detail::allocator::partial_std_allocator_wrapper<
    boost::detail::quick_allocator<param > > alloc_param; 

  namespace detail
  {
    template<> 
    inline string to_string(const db::param& a) 
    { return a.str(); } 
  }

  ///table rows interface
  class row
  {
  protected:
    typedef std::vector<param, alloc_param > data_type;
    data_type _data;
    unsigned _row;
    query* _query;

  public:
    typedef data_type::value_type value_type;
    typedef data_type::reference reference;
    typedef data_type::const_reference const_reference;
    typedef data_type::size_type size_type;
    typedef data_type::difference_type difference_type;
    typedef data_type::iterator iterator;
    typedef data_type::reverse_iterator reverse_iterator;
    typedef data_type::const_iterator const_iterator;

    /// @brief     row
    ///
    /// <BR>qualifier : _query(query_), _row(row_)
    /// <BR>access    public  
    /// @return    
    /// @param     query_ as query *
    /// @param     row_ as unsigned
    ///
    /// @date      20:2:2009   9:24
    ///
    row(query* query_, unsigned row_) : _query(query_), _row(row_)
    {
    }

    /// @brief     row
    ///
    /// <BR>qualifier : _query(r._query), _row(r._row)
    /// <BR>access    public  
    /// @return    
    /// @param     r as const row &
    ///
    /// @date      20:2:2009   9:23
    ///
    row(const row& r) : _query(r._query), _row(r._row)
    {
      _data.reserve(r.size());
      std::copy(r.begin(), r.end(), std::back_inserter(_data));
    }

    /// @brief     ~row
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    
    ///
    /// @date      20:2:2009   9:23
    ///
    ~row() {}


    /// @brief     get a value by number
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    const param&
    /// @param     field_num as size_type 
    ///
    /// @date      20:2:2009   9:23
    ///
    const param& value(size_type field_num) const
    { 
      const value_type& pValue(*(begin()+field_num));
      return pValue; 
    }

    /// @brief     get a value by number
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    param&
    /// @param     field_num as size_type
    ///
    /// @date      20:2:2009   9:23
    ///
    param& value(size_type field_num)
    { 
      value_type& pValue(*(begin()+field_num));
      return pValue; 
    }

    /// @brief     get a value by number
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    db::row::const_reference
    /// @param     field_num as size_type
    ///
    /// @date      20:2:2009   9:22
    ///
    const_reference operator[] (size_type field_num) const
    { return *(begin()+field_num); }

    /// @brief     get a value by number
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    db::row::reference
    /// @param     field_num as size_type
    ///
    /// @date      20:2:2009   9:21
    ///
    reference operator[] (size_type field_num)
    { return *(begin()+field_num); }

    /// @brief     operator[]
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    param*
    /// @param     field_title as const string &
    ///
    /// @date      20:2:2009   9:21
    ///
    param* operator[] (const string& field_title)
    { return (NULL); }

    /// @brief     const begin iterator
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    db::row::const_iterator
    ///
    /// @date      20:2:2009   9:21
    ///
    const_iterator begin() const          { return _data.begin(); }

    /// @brief     const end iterator
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    db::row::const_iterator
    ///
    /// @date      20:2:2009   9:21
    ///
    const_iterator end() const            { return _data.end(); }

    /// @brief     begin iterator
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    db::row::iterator
    ///
    /// @date      20:2:2009   9:21
    ///
    iterator begin()                      { return _data.begin(); }

    /// @brief     end iterator
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    db::row::iterator
    ///
    /// @date      20:2:2009   9:20
    ///
    iterator end()                        { return _data.end(); }

    /// @brief     begin reverse iterator
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    db::row::reverse_iterator
    ///
    /// @date      20:2:2009   9:20
    ///
    reverse_iterator rbegin()             { return _data.rbegin(); }

    /// @brief     end reverse iterator
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    db::row::reverse_iterator
    ///
    /// @date      20:2:2009   9:20
    ///
    reverse_iterator rend()               { return _data.rend(); }

    /// @brief     container size
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    db::row::size_type
    ///
    /// @date      20:2:2009   9:20
    ///
    size_type size() const                { return _data.size(); }

    /// @brief     returns true if the container is empty
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    bool
    ///
    /// @date      20:2:2009   9:19
    ///
    bool empty() const                    { return _data.empty(); }

    /// @brief     operator==
    ///
    /// <BR>qualifier const 
    /// <BR>access    public  
    /// @return    bool
    /// @param     t as const row &
    ///
    /// @date      20:2:2009   9:19
    ///
    bool operator==(const row& t) const { return _data == t._data; }

    /// @brief     push a param to the container
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    void
    /// @param     v as const value_type &
    ///
    /// @date      20:2:2009   9:18
    ///
    void push_back(const value_type& v)  { _data.push_back(v); }

    /// @brief     fill
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    void
    /// @param     stm as sqlite3_stmt *
    ///
    /// @date      20:2:2009   9:18
    ///
    inline void fill(sqlite3_stmt* stm);

  };
  typedef boost::detail::allocator::partial_std_allocator_wrapper<
    boost::detail::quick_allocator<row > > alloc_row; 

  ///cursor interface
  class query
  {
  protected:
    typedef row _value_type;
    typedef std::vector<_value_type, alloc_row > data_type;
    typedef field _fvalue_type;
    typedef std::vector<_fvalue_type, alloc_field > field_type;

    data_type _data;
    field_type _field;
    base& _base;
    sqlite3_stmt* _stm; 

  public:
    typedef _fvalue_type fvalue_type;
    typedef _value_type value_type;
    typedef data_type::reference reference;
    typedef data_type::const_reference const_reference;
    typedef data_type::size_type size_type;
    typedef data_type::difference_type difference_type;
    typedef data_type::iterator iterator;
    typedef data_type::const_iterator const_iterator;
    typedef field_type::iterator fiterator;
    typedef field_type::const_iterator const_fiterator;

    query(base& base_) : _base(base_), _stm(0)
    {

    }

    query(const query& q) : _base(q._base), _stm(q._stm)
    {
      _data.reserve(q._data.size());
      std::copy(q.begin(), q.end(), std::back_inserter(_data));

      _field.reserve(q._field.size());
      std::copy(q.fbegin(), q.fend(), std::back_inserter(_field));
    }

    ~query() {}

    inline void execute(const string& cmd);

    ///get a row by number
    const row& getRow(size_type row_num) const 
    { 
      const value_type& pValue(*(begin()+row_num));
      return pValue; 
    }
    row& getRow(size_type row_num) 
    { 
      value_type& pValue(*(begin()+row_num));
      return pValue; 
    }

    ///get a row by number
    const_reference operator[] (size_type row_num) const 
    { return *(begin()+row_num); }
    reference operator[] (size_type row_num) 
    { return *(begin()+row_num); }

    ///get id from field title
    int getIdOf(const string& sField) const  
    { 
      field_type::const_iterator iTB(_field.begin());
      field_type::const_iterator iTE(_field.end());
      for (iTB;iTB!=iTE;++iTB)
        if (iTB->name()==sField)
          return (int)(iTB-_field.begin());
      return -1;
    }
    string getTitleOf(unsigned iField) const  
    { 
      if (iField<_field.size())
        return _field[iField].name();
      else
        return DB_TEXT("");
    }
    const field* getFieldInfo(unsigned iField) const  
    { 
      if (iField<_field.size())
        return &_field[iField];
      else
        return NULL;
    }

    ///begin iterator
    const_iterator begin() const { return _data.begin(); }
    ///end iterator
    const_iterator end() const   { return _data.end(); }
    ///begin iterator
    iterator begin()             { return _data.begin(); }
    ///end iterator
    iterator end()               { return _data.end(); }
    ///begin iterator
    const_fiterator fbegin() const { return _field.begin(); }
    ///end iterator
    const_fiterator fend() const   { return _field.end(); }
    ///begin iterator
    fiterator fbegin()             { return _field.begin(); }
    ///end iterator
    fiterator fend()               { return _field.end(); }
    ///container size
    size_type size() const         { return _data.size(); }
    ///is container empty?
    bool empty() const             { return _data.empty(); }
    bool f_empty() const           { return _field.empty(); }

    bool operator==(const query& t) const 
    { return  _data == t._data; }

    ///push a row to the row container
    void push_back(const value_type& v)  { _data.push_back(v); }
    ///push a field title to the field container
    void push_back(const fvalue_type& v)  { _field.push_back(v); }
  };

  typedef boost::shared_ptr<query> query_ptr;

  ///helper class, generates SELECT SQL statements
  class sel 
  {
  protected:
    bool _distinct;
    int _limit;
    int _offset;
    split _results;
    split _sources;
    split _groupBy;
    split _orderBy;

    string _where; 
    string _having;
    string _delim1;
    string _delim2;
    field_pair _join;

  public:
    /// @brief     sel
    ///
    /// <BR>qualifier : _distinct(false), _limit(0), _offset(0), _where(DB_TEXT("True")), 
    ///              _delim1(DB_TEXT("")), _delim2(DB_TEXT(""))
    /// <BR>access    public  
    /// @return    
    ///
    /// @date      20:2:2009   14:03
    ///
    sel() : _distinct(false), _limit(0), _offset(0), 
      _where(DB_TEXT("True")), _delim1(DB_TEXT("")), _delim2(DB_TEXT(""))
    {}

    /// @brief     sel
    ///
    /// <BR>qualifier : _distinct(false), _limit(0), _offset(0), _where(DB_TEXT("True")), 
    ///              _delim1(DB_TEXT("")), _delim2(DB_TEXT(""))
    /// <BR>access    public  
    /// @return    
    /// @param     tablename as const string & 
    ///
    /// @date      20:2:2009   14:03
    ///
    sel(const string& tablename) : _distinct(false), _limit(0), _offset(0), 
      _where(DB_TEXT("True")), _delim1(DB_TEXT("")), _delim2(DB_TEXT(""))
    { source(tablename); }

    /// @brief     sel
    ///
    /// <BR>qualifier : _distinct(false), _limit(0), _offset(0), _where(DB_TEXT("True")), 
    ///              _delim1(delim), _delim2(delim)
    /// <BR>access    public  
    /// @return    
    /// @param     tablename as const string &
    /// @param     delim as const string &
    ///
    /// @date      20:2:2009   14:03
    ///
    sel(const string& tablename, const string& delim) : _distinct(false), 
      _limit(0), _offset(0), _where(DB_TEXT("True")), _delim1(delim), 
      _delim2(delim)
    { source(tablename); }

    /// @brief     sel
    ///
    /// <BR>qualifier : _distinct(false), _limit(0), _offset(0), _where(DB_TEXT("True")), 
    ///              _delim1(delim1), _delim2(delim2)
    /// <BR>access    public  
    /// @return    
    /// @param     tablename as const string &
    /// @param     delim1 as const string &
    /// @param     delim2 as const string &
    ///
    /// @date      20:2:2009   14:03
    ///
    sel(const string& tablename, const string& delim1, 
      const string& delim2) : _distinct(false), 
      _limit(0), _offset(0), _where(DB_TEXT("True")), _delim1(delim1), 
      _delim2(delim2)
    { source(tablename); }

    /// @brief     sel
    ///
    /// <BR>qualifier : _distinct(s._distinct), _limit(s._limit), _offset(s._offset), _where(s._where), _having(s._having), _delim1(s._delim1), _delim2(s._delim2), _results(s._results), _sources(s._sources), _groupBy(s._groupBy), _orderBy(s._orderBy)
    /// <BR>access    public  
    /// @return    
    /// @param     s as const sel &
    ///
    ///
    sel(const sel& s) : _distinct(s._distinct), _limit(s._limit),
      _offset(s._offset), _where(s._where), _having(s._having),
      _delim1(s._delim1), _delim2(s._delim2), _results(s._results),
      _sources(s._sources), _groupBy(s._groupBy), _orderBy(s._orderBy),
      _join(s._join)
    {
    }

    /// @brief     ~sel
    ///
    /// <BR>qualifier
    /// <BR>access    virtualpublic  
    /// @return    
    ///
    /// @date      20:2:2009   14:03
    ///
    virtual ~sel() {}

    /// @brief     operator=
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    sel &
    /// @param     s as const sel &
    ///
    ///
    sel & operator=(const sel& s)
    { 
      _distinct = s._distinct;
      _limit = s._limit;
      _offset = s._offset;
      _results = s._results;
      _sources = s._sources;
      _groupBy = s._groupBy;
      _orderBy = s._orderBy;

      _where = s._where; 
      _having = s._having;
      _delim1 = s._delim1;
      _delim2 = s._delim2;
      _join = s._join;

      return *this; 
    }

		/// @brief     ,
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    sel &
    /// @param     s as const string &
    ///
    /// @date      20:2:2009   14:02
    ///
    virtual sel & operator,(const string& s)
    { return this->result(s); }

    /// @brief     operator%
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    sel &
    /// @param     e as const expr::base &
    ///
    /// @date      20:2:2009   14:02
    ///
    virtual sel & operator%(const expr::base& e)
    { return this->where(e.str()); }

    /// @brief     operator<
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    sel &
    /// @param     f as const field_pair &
    ///
    /// @date      6:8:2009   10:58
    ///
    virtual sel & operator<(const field_pair& f)
    { return this->join(f); }

    /// @brief     distinct
    ///
    /// <BR>qualifier
    /// <BR>access    virtual public  
    /// @return    sel &
    /// @param     d as bool
    ///
    /// @date      20:2:2009   14:02
    ///
    virtual sel & distinct(bool d) 
    { 
      _distinct = d; 
      return *this;
    }

    /// @brief     limit
    ///
    /// <BR>qualifier
    /// <BR>access    virtual public  
    /// @return    sel &
    /// @param     value as int
    ///
    /// @date      20:2:2009   14:02
    ///
    virtual sel & limit(int value)
    { 
      _limit = value;	
      return *this;
    }

    /// @brief     join
    ///
    /// <BR>qualifier
    /// <BR>access    virtual public  
    /// @return    sel &
    /// @param     f as const field_pair&
    ///
    /// @date      20:2:2009   14:02
    ///
    virtual sel & join(const field_pair& f)
    { 
      _join = f;	
      return *this;
    }

    /// @brief     offset
    ///
    /// <BR>qualifier
    /// <BR>access    virtual public  
    /// @return    sel &
    /// @param     value as int 
    ///
    /// @date      20:2:2009   14:02
    ///
    virtual sel & offset(int value)
    { 
      _offset = value; 
      return *this;
    }

    /// @brief     result_no_delim
    ///
    /// <BR>qualifier
    /// <BR>access    virtual public  
    /// @return    sel &
    /// @param     r as string
    /// @param     alias as string
    ///
    /// @date      20:2:2009   14:02
    ///
    virtual sel & result_no_delim(string r, string alias=DB_TEXT(""))
    { 
      if (alias.length())
        r += DB_TEXT(" AS ") + alias;
      _results.push_back(r); 
      return *this;
    }

    /// @brief     result
    ///
    /// <BR>qualifier
    /// <BR>access    virtual public  
    /// @return    sel &
    /// @param     r as string
    /// @param     alias as string
    ///
    /// @date      20:2:2009   14:02
    ///
    virtual sel & result(string r, string alias=DB_TEXT(""))
    { 
      if (alias.length())
        r += DB_TEXT(" AS ") + alias;
      if (r!=DB_TEXT("*")) 
        detail::front_back_delim(r, _delim1, _delim2);
      _results.push_back(r); 
      return *this;
    }

    /// @brief     result_max
    ///
    /// <BR>qualifier
    /// <BR>access    virtual public  
    /// @return    sel &
    /// @param     r as string
    /// @param     alias as string
    ///
    /// @date      20:2:2009   14:02
    ///
    virtual sel & result_max(string r, string alias=DB_TEXT(""))
    { 
      r = DB_TEXT("MAX(")+r;
      r += DB_TEXT(")");
      if (r!=DB_TEXT("*")) 
        detail::front_back_delim(r, _delim1, _delim2);
      if (alias.length())
      {
        detail::front_back_delim(alias, _delim1, _delim2);
        r += DB_TEXT(" AS ") + alias;
      }
      _results.push_back(r); 
      return *this;
    }

    /// @brief     clear
    ///
    /// <BR>qualifier
    /// <BR>access    virtual public  
    /// @return    sel &
    ///
    /// @date      20:2:2009   14:01
    ///
    virtual sel & clear()
    {
      _results.clear();
      _sources.clear();
      return *this;
    }

    /// @brief     source
    ///
    /// <BR>qualifier
    /// <BR>access    virtual public  
    /// @return    sel &
    /// @param     s as string
    /// @param     alias as string
    ///
    /// @date      20:2:2009   14:01
    ///
    virtual sel & source(string s, string alias=DB_TEXT(""))
    {
      detail::front_back_delim(s, _delim1, _delim2);
      if (alias.length())
      {
        detail::front_back_delim(alias, _delim1, _delim2);
        s += DB_TEXT(" AS ") + alias;
      }
      _sources.push_back(s);
      return *this;
    }

    /// @brief     where
    ///
    /// <BR>qualifier
    /// <BR>access    virtual public  
    /// @return    sel &
    /// @param     w as const expr::base &
    ///
    /// @date      20:2:2009   14:01
    ///
    virtual sel & where(const expr::base & w)
    { 
      _where = expr::and_(expr::raw(_where), w).str();	
      return *this;
    }

    /// @brief     where
    ///
    /// <BR>qualifier
    /// <BR>access    virtual public  
    /// @return    sel &
    /// @param     w as string
    ///
    /// @date      20:2:2009   14:01
    ///
    virtual sel & where(string w)
    { 
      _where = expr::and_(expr::raw(_where), expr::raw(w)).str();
      return *this;
    }

    /// @brief     group_by
    ///
    /// <BR>qualifier
    /// <BR>access    virtual public  
    /// @return    sel &
    /// @param     gb as string
    ///
    /// @date      20:2:2009   14:01
    ///
    virtual sel & group_by(string gb)
    { 
      detail::front_back_delim(gb, _delim1, _delim2);
      _groupBy.push_back(gb);	
      return *this;
    }

    /// @brief     having
    ///
    /// <BR>qualifier
    /// <BR>access    virtual public  
    /// @return    sel &
    /// @param     h as const expr::base &
    ///
    /// @date      20:2:2009   14:01
    ///
    virtual sel & having(const expr::base & h)
    { 
      _having = h.str(); 
      return *this;
    }

    /// @brief     having
    ///
    /// <BR>qualifier
    /// <BR>access    virtual public  
    /// @return    sel &
    /// @param     h as string
    ///
    /// @date      20:2:2009   14:01
    ///
    virtual sel & having(string h)
    { 
      _having = h;
      return *this;
    }

    /// @brief     order_by
    ///
    /// <BR>qualifier
    /// <BR>access    virtual public  
    /// @return    sel &
    /// @param     ob as string
    /// @param     ascending as bool 
    ///
    /// @date      20:2:2009   14:00
    ///
    virtual sel & order_by(string ob, bool ascending=true)
    { 
      string value = ob;
      detail::front_back_delim(value, _delim1, _delim2);
      if (!ascending)
        value += DB_TEXT(" DESC");
      _orderBy.push_back(value); 
      return *this;
    }

    /// @brief     operator string
    ///
    /// <BR>qualifier const
    /// <BR>access    virtual public  
    /// @return    
    ///
    /// @date      20:2:2009   14:00
    ///
    virtual operator string() const
    {
      ostringstream res;
      res << DB_TEXT("SELECT ");
      if (_distinct)
        res << DB_TEXT("DISTINCT ");
      res << _results.join(DB_TEXT(","));
      res << DB_TEXT(" FROM ");
      res << _sources.join(DB_TEXT(","));
      
      if (_join.first.table().length()!=0 && 
          _join.second.table().length()!=0 && 
          _sources.size()==1)
        res << DB_TEXT(" INNER JOIN ") << _join.first.table() << DB_TEXT(" ON ") << 
          _join.first.fullName() << DB_TEXT(" = ") << _join.second.fullName() << 
          DB_TEXT(" ");
      
      if (_where != DB_TEXT("True"))
        res << DB_TEXT(" WHERE ") << _where;
      if (_groupBy.size() > 0)
        res << DB_TEXT(" GROUP BY ") << _groupBy.join(DB_TEXT(","));
      if (_having.length())
        res << DB_TEXT(" HAVING ") << _having;
      if (_orderBy.size() > 0)
        res << DB_TEXT(" ORDER BY ") << _orderBy.join(DB_TEXT(","));
      if (_limit) 
        res << DB_TEXT(" LIMIT ") << string(detail::to_string(_limit));
      if (_offset) 
        res << DB_TEXT(" OFFSET ") << string(detail::to_string(_offset));

      string ret(res.str());
      detail::replace_all(ret, _delim1+_delim1, _delim1);
      if (_delim1!=_delim2)
        detail::replace_all(ret, _delim2+_delim2, _delim2);
      return ret;
    }

    /// @brief     str
    ///
    /// <BR>qualifier const
    /// <BR>access    virtual public  
    /// @return    db::string
    ///
    /// @date      20:2:2009   14:12
    ///
    virtual string str() const { return this->operator string(); }
  }; 

  ///helper class, generates DELETE SQL statements
  class del : public sel
  {
  public:
    del(const string& tablename) : sel(tablename) { }

    /// @brief     operator string
    ///
    /// <BR>qualifier const
    /// <BR>access    virtual public  
    /// @return    
    ///
    /// @date      20:2:2009   14:12
    ///
    virtual operator string() const
    {
      string res = DB_TEXT("DELETE FROM ");
      res += _sources.join(DB_TEXT(","));
      if (_where != DB_TEXT("True"))
        res += DB_TEXT(" WHERE ") + _where;
      if (_groupBy.size() > 0)
        res += DB_TEXT(" GROUP BY ") + _groupBy.join(DB_TEXT(","));
      if (_having.length())
        res += DB_TEXT(" HAVING ") + _having;
      if (_orderBy.size() > 0)
        res += DB_TEXT(" ORDER BY ") + _orderBy.join(DB_TEXT(","));
      if (_limit) 
        res += DB_TEXT(" LIMIT ") + string(detail::to_string(_limit));
      if (_offset) 
        res += DB_TEXT(" OFFSET ") + string(detail::to_string(_offset));

      detail::replace_all(res, _delim1+_delim1, _delim1);
      if (_delim1!=_delim2)
        detail::replace_all(res, _delim2+_delim2, _delim2);
      return res;
    }

    /// @brief     ~del
    ///
    /// <BR>qualifier
    /// <BR>access    virtual public  
    /// @return    
    ///
    /// @date      20:2:2009   14:12
    ///
    virtual ~del() {}
  };

  ///helper class, generates INSERT SQL statements
  class ins 
  {
  protected:
    split_map _values;
    string    _source;
    string    _delim1;
    string    _delim2;

  public:
    /// @brief     ins
    ///
    /// <BR>qualifier : _source(tab)
    /// <BR>access    public  
    /// @return    
    /// @param     tab as const string &
    ///
    /// @date      20:2:2009   9:07
    ///
    ins(const string& tab) : _source(tab) 
    {}

    /// @brief     ins
    ///
    /// <BR>qualifier : _source(tab), _delim1(delim), _delim2(delim)
    /// <BR>access    public  
    /// @return    
    /// @param     tab as const string &
    /// @param     delim as const string &
    ///
    /// @date      20:2:2009   9:07
    ///
    ins(const string& tab, const string& delim) : _source(tab), 
      _delim1(delim), _delim2(delim) 
    {}

    /// @brief     ins
    ///
    /// <BR>qualifier : _source(tab), _delim1(delim1), _delim2(delim2)
    /// <BR>access    public  
    /// @return    
    /// @param     tab as const string &
    /// @param     delim1 as const string &
    /// @param     delim2 as const string &
    ///
    /// @date      20:2:2009   9:07
    ///
    ins(const string& tab, const string& delim1, const string& delim2) : 
      _source(tab), _delim1(delim1), _delim2(delim2) 
    {}
    
    /// @brief     ~ins
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    
    ///
    /// @date      20:2:2009   9:07
    ///
    ~ins() {}
    
    /// @brief     clear_values
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    ins &
    ///
    /// @date      20:2:2009   9:07
    ///
    ins & clear_values()
    {
      _values.clear();
      return *this;
    }

    /// @brief     operator%
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    ins &
    /// @param     f as const db::field &
    ///
    /// @date      20:2:2009   9:07
    ///
    ins & operator%(const db::field& f)
    { 
      if (f.values().size()==1)
        return values(f.name(), f.values().begin()->second, f.type());
      else
        return *this;
    }

    /// @brief     values
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    ins &
    /// @param     t as string
    /// @param     v as param *
    ///
    /// @date      20:2:2009   9:07
    ///
    ins & values(string t, param* v)
    {
      string sV(v->str());
      if (sV.length()==0)
        sV = DB_TEXT("NULL");
      else
        switch(v->get_type()) 
      {
        case e_string:
        case e_date_time:
          sV = DB_TEXT("'") + sV;
          sV += DB_TEXT("'");
          break;
      }
      detail::front_back_delim(t, _delim1, _delim2);
      _values.insert(std::pair<string, string>(t, sV));
      return *this;
    }

    /// @brief     values
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    ins &
    /// @param     t as string
    /// @param     v as string
    /// @param     p as param_types
    ///
    /// @date      20:2:2009   9:07
    ///
    ins & values(string t, string v, param_types p=e_int)
    {
      switch(p) 
      {
      case e_char:
      case e_blob:
      case e_string:
      case e_date_time:
        detail::erase_all<string>(v, DB_TEXT("'"));
        detail::erase_all<string>(v, DB_TEXT("`"));
        detail::erase_all<string>(v, DB_TEXT("´"));
        v = DB_TEXT("'") + v;
        v += DB_TEXT("'");
        break;
      }
      detail::front_back_delim(t, _delim1, _delim2);
      _values.insert(std::pair<string, string>(t, v));
      return *this;
    }

    /// @brief     values
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    ins &
    /// @param     t as const string &
    /// @param     v as const T &
    ///
    /// @date      20:2:2009   11:07
    ///
    template<typename T>
    ins & values(const string& t, const T& v)
    { return values(t, detail::to_string(v), tp(v)); }

    /// @brief     operator string
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    
    ///
    /// @date      20:2:2009   11:04
    ///
    operator string() const
    { return str(); }

    /// @brief     str
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    db::string
    ///
    /// @date      20:2:2009   11:05
    ///
    string str() const 
    {       
      string res = DB_TEXT("INSERT INTO ");
      res += _source;

      if (_values.size() > 0)
      {
        res += DB_TEXT(" (");
        res += _values.join_fields(DB_TEXT(","));
        res += DB_TEXT(") VALUES (");
        res += _values.join_values(DB_TEXT(","));
        res += DB_TEXT(")");
      }
      return res;
    }
  }; 

  ///hilft beim anlegen von UPDATE-SQL Ausdrücken.
  class upd 
  {
    string _delim1;
    string _delim2;
    string table;
    string _where;
    split fields;
    split values;
  public:
    /// @brief     ~upd
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    
    ///
    /// @date      20:2:2009   11:14
    ///
    ~upd() 
    { }

    /// @brief     upd
    ///
    /// <BR>qualifier : table(t), _where(DB_TEXT("True"))
    /// <BR>access    public  
    /// @return    
    /// @param     t as const string &
    ///
    /// @date      20:2:2009   11:15
    ///
    upd(const string& t) : table(t), _where(DB_TEXT("True")) 
    { }

    /// @brief     upd
    ///
    /// <BR>qualifier : table(t), _where(DB_TEXT("True")), _delim1(d), _delim2(d)
    /// <BR>access    public  
    /// @return    
    /// @param     t as const string &
    /// @param     d as const string &
    ///
    /// @date      20:2:2009   11:15
    ///
    upd(const string& t, const string& d) : table(t), 
      _where(DB_TEXT("True")), _delim1(d), _delim2(d) 
    { }

    /// @brief     upd
    ///
    /// <BR>qualifier : table(t), _where(DB_TEXT("True")), _delim1(d1), _delim2(d2)
    /// <BR>access    public  
    /// @return    
    /// @param     t as const string &
    /// @param     d1 as const string &
    /// @param     d2 as const string &
    ///
    /// @date      20:2:2009   11:15
    ///
    upd(const string& t, const string& d1, const string& d2) : table(t), 
      _where(DB_TEXT("True")), _delim1(d1), _delim2(d2) 
    { }

    /// @brief     where
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    upd&
    /// @param     e as const expr::base &
    ///
    /// @date      20:2:2009   11:14
    ///
    upd& where(const expr::base& e)
    {
      _where = expr::and_(expr::raw(_where), e).str();
      return *this;
    }

    /// @brief     operator%
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    upd &
    /// @param     e as const expr::base &
    ///
    /// @date      20:2:2009   11:14
    ///
    upd & operator%(const expr::base& e)
    { return where(e); }

    /// @brief     operator%
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    upd &
    /// @param     f as const db::field &
    ///
    /// @date      20:2:2009   11:14
    ///
    upd & operator%(const db::field& f)
    { 
      if (f.values().size()==1)
        return set(f, f.values().begin()->second);
      else
        return *this;
    }

    /// @brief     set
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    upd&
    /// @param     f as const field &
    /// @param     val as const string &
    ///
    /// @date      20:2:2009   11:14
    ///
    upd& set(const field& f, const string& val)
    {
      fields.push_back(f.name());
      if (f.type()==e_string||f.type()==e_char||
          f.type()==e_blob||f.type()==e_date_time)
        values.push_back(detail::escape_sql(val));
      else
        values.push_back(val);
      return *this;
    }

    /// @brief     set
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    upd&
    /// @param     f as const field &
    /// @param     val as const T &
    ///
    /// @date      20:2:2009   11:14
    ///
    template<typename T>
    upd& set(const field& f, const T& val)
    { return set(f, detail::to_string(val)); }

    /// @brief     operator string
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    
    ///
    /// @date      20:2:2009   11:15
    ///
    operator string() const
    { return str(); }

    /// @brief     str
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    db::string
    ///
    /// @date      20:2:2009   11:16
    ///
    string str() const 
    {
      string q = DB_TEXT("UPDATE ") + table + DB_TEXT(" SET ");
      split sets;
      for (size_t i = 0; i < fields.size(); i++)
        sets.push_back(fields[i] + DB_TEXT("=") + values[i]);
      q += sets.join(DB_TEXT(","));
      if (_where.length())
        q += DB_TEXT(" WHERE ") + _where;
      return q;
    }
  };

  ///datenbank interface
  class base
  {
    sqlite3* _db;

    class progress_handler
    {
      /// @brief     xProgressCallback
      ///
      /// <BR>qualifier
      /// <BR>access    private static  
      /// @return    int
      /// @param     db_ as void *
      ///
      /// @date      20:2:2009   14:13
      ///
      static int xProgressCallback(void* db_)
      {
        if (!db_)
          return 1;
        base* base_(reinterpret_cast<base*> (db_));

        if (!base_)
          return 1;

        //if (!base_->Sig_Progress().connected())
        //  return 1;

        //if (base_->Sig_Progress()(0,0))
        //  return 0;
        //else

        return 0;
      } 

      sqlite3* _db;
    public:
      progress_handler(sqlite3* db_, base* base_) : _db(db_)
      {
        sqlite3_progress_handler(_db, 4, 
          progress_handler::xProgressCallback, base_);
      }
    	~progress_handler()
      {
        sqlite3_progress_handler(_db, 0, NULL, NULL); 
      }
    };
    
    typedef std::map<string, query> type_last_queries; 
    type_last_queries _last_queries; 

  public:
    /// @brief     base
    ///
    /// <BR>qualifier : _db(NULL)
    /// <BR>access    public  
    /// @return    
    ///
    /// @date      20:2:2009   14:14
    ///
    base() : _db(NULL)
    {}

    /// @brief     ~base
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    
    ///
    /// @date      20:2:2009   14:14
    ///
    ~base()
    {
      if (_db) 
        sqlite3_close(_db);
    }

    enum feature
    {
      e_transact,
      e_blob
    };

    /// @brief     throw_error
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    void
    /// @param     status as int
    ///
    /// @date      20:2:2009   14:13
    ///
    inline void throw_error(int status) 
    {
      string err(boost::str(
        format(DB_TEXT("%d=status code : %s")) % 
          status % sqlite3_errmsg(_db)));
      switch(status) 
      {
      case SQLITE_ERROR: throw exception::sql_error(err);
      case SQLITE_INTERNAL: throw exception::internal_error(err);
      case SQLITE_NOMEM: throw exception::memory_error(err);
      case SQLITE_FULL: throw exception::insertion_error(err);
      default: throw exception::unknown_error(DB_TEXT("compile failed: ") + err);
      }
    } 

    /// @brief     open the specified db
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    void
    /// @param     file as const std::string &
    ///
    /// @date      20:2:2009   14:14
    ///
    void connect(const std::string& file)
    {
      if (_db) 
        sqlite3_close(_db);
      int rc = sqlite3_open(file.c_str(), &_db); 
      if (rc != SQLITE_OK)
        throw_error(rc);
      rc = sqlite3_errcode(_db);
      if (rc != SQLITE_OK)
        throw_error(rc);
    }

    /// @brief     begins SQL transaction
    ///
    /// <BR>qualifier 
    /// <BR>access    public  
    /// @return    void
    ///
    /// @date      20:2:2009   14:14
    ///
    void begin() 
    {
      int rc(sqlite3_exec(_db, "BEGIN;", NULL, NULL, NULL)); 
      if (rc != SQLITE_OK)
        throw_error(rc);
    }

    /// @brief     commits SQL transaction
    ///
    /// <BR>qualifier 
    /// <BR>access    public  
    /// @return    void
    ///
    /// @date      20:2:2009   14:15
    ///
    void commit() 
    {
      int rc(sqlite3_exec(_db, "COMMIT;", NULL, NULL, NULL)); 
      if (rc != SQLITE_OK)
        throw_error(rc);
    }

    /// @brief     cancels active SQL transaction
    ///
    /// <BR>qualifier 
    /// <BR>access    public  
    /// @return    void
    ///
    /// @date      20:2:2009   14:15
    ///
    void rollback() 
    {
      int rc(sqlite3_exec(_db, "ROLLBACK;", NULL, NULL, NULL)); 
      if (rc != SQLITE_OK)
        throw_error(rc);
    }

    /// @brief     execute SQL statement
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    query&
    /// @param     cmd as const string &
    ///
    /// @date      20:2:2009   14:15
    ///
    inline query& execute(const string& cmd);

    /// @brief     execute
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    db::query_ptr
    /// @param     cmd as const string &
    ///
    /// @date      13:7:2009   13:14
    ///
    inline query_ptr execute_ptr(const string& cmd);

    /// @brief     get_db_ptr
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    sqlite3*
    ///
    /// @date      20:2:2009   14:16
    ///
    sqlite3* get_db_ptr() {return _db;}

//#ifndef BOOST_NO_STD_LOCALE
//    typedef boost::signal2<bool, unsigned, unsigned> type_signal_progress; 
//
//    /// @brief     Sig_Progress
//    ///
//    /// <BR>qualifier
//    /// <BR>access    public  
//    /// @return    type_signal_progress&
//    ///
//    /// @date      20:2:2009   14:16
//    ///
//    type_signal_progress& Sig_Progress() { return sig_progress; }
//
//#endif // BOOST_NO_STD_LOCALE
  protected:
//#ifndef BOOST_NO_STD_LOCALE
//    type_signal_progress sig_progress;
//#endif // BOOST_NO_STD_LOCALE
  };
  typedef boost::shared_ptr<base> base_ptr;

#ifdef _MFC_VER

  namespace serialize
  {
    class field : public CObject
    {
      string _name;
      int    _type;

    public:
      /// @brief     field
      ///
      /// <BR>qualifier : _type(db::e_null)
      /// <BR>access    public  
      /// @return    
      ///
      ///
      field() : CObject(), _type(db::e_null) {}
      
      /// @brief     field
      ///
      /// <BR>qualifier : _type(type_), _name(name_)
      /// <BR>access    public  
      /// @return    
      /// @param     type_ as param_types
      /// @param     name_ as const string &
      ///
      /// @date      14:9:2009   10:00
      ///
      field(param_types type_, const string& name_) : CObject(),
        _type(type_), _name(name_) 
      {}

      /// @brief     field
      ///
      /// <BR>qualifier : CObject(), _type(f._type), _name(f._name)
      /// <BR>access    public  
      /// @return    
      /// @param     f as const field &
      ///
      /// @date      28:9:2009   15:17
      ///
      field(const field& f) : CObject(), _type(f._type), _name(f._name) 
      {}

      /// @brief     operator=
      ///
      /// <BR>qualifier
      /// <BR>access    public  
      /// @return    field&
      /// @param     f as const field &
      ///
      /// @date      28:9:2009   15:21
      ///
      field& operator=(const field& f)
      {
        _type = f._type;
        _name = f._name;
        return *this;
      }

      const db::string& name() const { return _name; }
      void name(const db::string& val) { _name = val; }

      param_types Type() const { return static_cast<param_types>(_type); }
      void Type(param_types val) { _type = val; }

      /// @brief     Serialize
      ///
      /// <BR>qualifier
      /// <BR>access    public  
      /// @return    void
      /// @param     ar as CArchive &
      ///
      /// @date      14:9:2009   10:00
      ///
      virtual void Serialize(CArchive& ar)
      {
        CObject::Serialize(ar);

        if( ar.IsStoring() )
        {
          ar.WriteCount(_type);
          ar << CString(_name.c_str());
#ifdef _BOOST_TEST
#ifdef _DEBUG
          OutputDebugStringW(db::detail::to_string(_type).c_str());
          OutputDebugStringW(L"\n");
          OutputDebugStringW(_name.c_str());
          OutputDebugStringW(L"\n");
#endif // _DEBUG
#endif        
        }
        else
        {
          _type = ar.ReadCount();
          CString tmp;
          ar >> tmp;
          _name = tmp;

#ifdef _BOOST_TEST
#ifdef _DEBUG
          OutputDebugStringW(db::detail::to_string(_type).c_str());
          OutputDebugStringW(L"\n");
          OutputDebugStringW(_name.c_str());
          OutputDebugStringW(L"\n");
#endif // _DEBUG
#endif        
        }
      }

      DECLARE_SERIAL(field);
    };

    typedef std::vector<db::serialize::field> t_ser_field;

    class row : public CObject
    {
      string_vec _data;
      
    public:
      /// @brief     row
      ///
      /// <BR>qualifier
      /// <BR>access    public  
      /// @return    
      ///
      /// @date      14:9:2009   10:15
      ///
      row() : CObject() 
      {}

      /// @brief     row
      ///
      /// <BR>qualifier : CObject(), _data(r._data)
      /// <BR>access    public  
      /// @return    
      /// @param     r as const row &
      ///
      /// @date      28:9:2009   15:18
      ///
      row(const row& r) : CObject(), _data(r._data)
      {}

      /// @brief     ~row
      ///
      /// <BR>qualifier
      /// <BR>access    public  
      /// @return    
      ///
      /// @date      14:9:2009   10:15
      ///
      ~row() {}

      /// @brief     operator=
      ///
      /// <BR>qualifier
      /// <BR>access    public  
      /// @return    row&
      /// @param     r as const row &
      ///
      /// @date      28:9:2009   15:22
      ///
      row& operator=(const row& r)
      {
        _data = r._data;
        return *this;
      }

      db::string_vec& Data() { return _data; }
      const db::string_vec& Data() const { return _data; }

      /// @brief     Serialize
      ///
      /// <BR>qualifier
      /// <BR>access    public  
      /// @return    void
      /// @param     ar as CArchive &
      ///
      /// @date      14:9:2009   10:15
      ///
      virtual void Serialize(CArchive& ar)
      {
        CObject::Serialize(ar);

        if( ar.IsStoring() )
        {
          ar.WriteCount(_data.size());
#ifdef _BOOST_TEST
#ifdef _DEBUG
          OutputDebugStringW(db::detail::to_string(_data.size()).c_str());
          OutputDebugStringW(L"\n");
#endif
#endif          
          for (string_vec::iterator itb(_data.begin()), 
            ite(_data.end()); itb!=ite; ++itb)
          {
            ar << CString(itb->c_str());
#ifdef _BOOST_TEST
#ifdef _DEBUG
            OutputDebugStringW(itb->c_str());
            OutputDebugStringW(L"\n");
#endif // _DEBUG
#endif          
          }
        }
        else
        {
          DWORD_PTR cnts(ar.ReadCount());
#ifdef _BOOST_TEST
#ifdef _DEBUG
          OutputDebugStringW(db::detail::to_string(cnts).c_str());
          OutputDebugStringW(L"\n");
#endif
#endif          
          if (cnts>0)
          {
            _data.resize(cnts);
            
            for (DWORD_PTR cnt(0); cnt<cnts; ++cnt)
            {
              CString tmp;
              ar >> tmp;
              _data[cnt] = tmp;
#ifdef _BOOST_TEST
#ifdef _DEBUG
              OutputDebugStringW(tmp);
              OutputDebugStringW(L"\n");
#endif // _DEBUG
#endif            
            }
          }
        }
      }
      
      DECLARE_SERIAL(row);
    };

    typedef std::vector<db::serialize::row> t_ser_row;


    class table : public CObject
    {
      string _name;
      t_ser_field  _fields;
      t_ser_row _rows;

    public:
      /// @brief     table
      ///
      /// <BR>qualifier
      /// <BR>access    public  
      /// @return    
      ///
      /// @date      14:9:2009   9:46
      ///
      table() : CObject() {}

      /// @brief     table
      ///
      /// <BR>qualifier : CObject(), _name(t._name), _fields(t._fields), _rows(t._rows)
      /// <BR>access    public  
      /// @return    
      /// @param     t as const table &
      ///
      /// @date      28:9:2009   15:19
      ///
      table(const table& t) : CObject(), _name(t._name), _fields(t._fields),
        _rows(t._rows)
      {}

      /// @brief     table
      ///
      /// <BR>qualifier : _name(name_)
      /// <BR>access    public  
      /// @return    
      /// @param     name_ as const db::string &
      ///
      /// @date      14:9:2009   9:46
      ///
      table(const db::string& name_) : CObject(), _name(name_) 
      {}

      /// @brief     operator=
      ///
      /// <BR>qualifier
      /// <BR>access    public  
      /// @return    table&
      /// @param     r as const table &
      ///
      /// @date      28:9:2009   15:23
      ///
      table& operator=(const table& t)
      {
        _name   = t._name; 
        _fields = t._fields;
        _rows   = t._rows;

        return *this;
      }

      const db::string& name() const { return _name; }
      void name(const db::string& val) { _name = val; }

      db::serialize::t_ser_field& fields() { return _fields; }
      const db::serialize::t_ser_field& fields() const { return _fields; }

      db::serialize::t_ser_row& rows() { return _rows; }
      const db::serialize::t_ser_row& rows() const { return _rows; }

      /// @brief     Serialize
      ///
      /// <BR>qualifier
      /// <BR>access    public  
      /// @return    void
      /// @param     ar as CArchive &
      ///
      /// @date      14:9:2009   9:46
      ///
      virtual void Serialize(CArchive& ar)
      {
        CObject::Serialize(ar);

        if( ar.IsStoring() )
        {
          ar.WriteCount(_fields.size());
          ar.WriteCount(_rows.size());
          ar << CString(_name.c_str());

#ifdef _BOOST_TEST
#ifdef _DEBUG
          OutputDebugStringW(db::detail::to_string(_fields.size()).c_str());
          OutputDebugStringW(L"\n");
          OutputDebugStringW(db::detail::to_string(_rows.size()).c_str());
          OutputDebugStringW(L"\n");
          OutputDebugStringW(_name.c_str());
          OutputDebugStringW(L"\n");
#endif
#endif
          for (t_ser_field::iterator itb(_fields.begin()), 
            ite(_fields.end()); itb!=ite; ++itb)
            itb->Serialize(ar);

          for (t_ser_row::iterator itb(_rows.begin()), 
            ite(_rows.end()); itb!=ite; ++itb)
            itb->Serialize(ar);
        }
        else
        {
          DWORD_PTR cnt_fields(ar.ReadCount());
          DWORD_PTR cnt_rows(ar.ReadCount());
          
          CString name; ar >> name;
          _name = name;

#ifdef _BOOST_TEST
#ifdef _DEBUG
          OutputDebugStringW(db::detail::to_string(cnt_fields).c_str());
          OutputDebugStringW(L"\n");
          OutputDebugStringW(db::detail::to_string(cnt_rows).c_str());
          OutputDebugStringW(L"\n");
          OutputDebugStringW(_name.c_str());
          OutputDebugStringW(L"\n");
#endif
#endif          
          _fields.reserve(cnt_fields);
          for (t_ser_field::size_type cnt(0); cnt<cnt_fields; ++cnt)
          {
            _fields.push_back(db::serialize::field());
            _fields.back().Serialize(ar);
          }

          _rows.reserve(cnt_rows);
          for (t_ser_row::size_type cnt(0); cnt<cnt_rows; ++cnt)
          {
            _rows.push_back(db::serialize::row());
            _rows.back().Serialize(ar);
          }
        }
      }

      typedef std::vector<db::serialize::table> t_ser_table;

      DECLARE_SERIAL(table);
    };

    typedef std::vector<table > t_ser_table;
  }

#endif // _MFC_VER

} // namespace db

///overload from the global && operator
inline db::expr::and_ operator&&(const db::expr::base& o1, 
                                const db::expr::base& o2)
{ return db::expr::and_(o1,o2); }
///overload from the global || operator
inline db::expr::or_ operator||(const db::expr::base& o1, 
                               const db::expr::base& o2)
{ return db::expr::or_(o1,o2); }

/// @brief     overload from the global == operator
///
/// <BR>qualifier
/// <BR>access    public  
/// @return    db::expr::eq
/// @param     fld as const db::field &
/// @param     f2 as const T &
///
/// @date      20:2:2009   14:17
///
template<typename T>
inline db::expr::eq operator==(const db::field& fld, const T& f2)
{ return db::expr::eq(fld, f2); }

/// @brief     overload from the global > operator
///
/// <BR>qualifier
/// <BR>access    public  
/// @return    db::expr::gt
/// @param     fld as const db::field &
/// @param     o2 as const T &
///
/// @date      20:2:2009   14:17
///
template<typename T>
inline db::expr::gt operator>(const db::field& fld, const T& o2)
{ return db::expr::gt(fld, o2); }

/// @brief     overload from the global >= operator
///
/// <BR>qualifier
/// <BR>access    public  
/// @return    db::expr::gt_eq
/// @param     fld as const db::field &
/// @param     o2 as const T &
///
/// @date      20:2:2009   14:17
///
template<typename T>
inline db::expr::gt_eq operator>=(const db::field& fld, const T& o2)
{ return db::expr::gt_eq(fld, o2); }

/// @brief     overload from the global < operator
///
/// <BR>qualifier
/// <BR>access    public  
/// @return    db::expr::lt
/// @param     fld as const db::field &
/// @param     o2 as const T &
///
/// @date      20:2:2009   14:21
///
template<typename T>
inline db::expr::lt operator<(const db::field& fld, const T& o2)
{ return db::expr::lt(fld, o2); }

/// @brief     overload from the global <= operator
///
/// <BR>qualifier
/// <BR>access    public  
/// @return    db::expr::lt_eq
/// @param     fld as const db::field &
/// @param     o2 as const T &
///
/// @date      20:2:2009   14:22
///
template<typename T>
inline db::expr::lt_eq operator<=(const db::field& fld, const T& o2)
{ return db::expr::lt_eq(fld, o2); }

/// @brief     overload from the global != operator
///
/// <BR>qualifier
/// <BR>access    public  
/// @return    db::expr::not_eq_
/// @param     fld as const db::field &
/// @param     f2 as const T &
///
/// @date      20:2:2009   14:22
///
template<typename T>
inline db::expr::not_eq_ operator!=(const db::field& fld, const T& f2)
{ return db::expr::not_eq_(fld, f2); }

/// @brief     overload from the global ! operator
///
/// <BR>qualifier
/// <BR>access    public  
/// @return    db::expr::not
/// @param     exp as const db::expr::base & 
///
/// @date      20:2:2009   14:23
///
inline db::expr::not_ operator!(const db::expr::base &exp)
{ return db::expr::not_(exp); }

/// @brief     overload from the global << operator
///
/// <BR>qualifier
/// <BR>access    public  
/// @return    db::expr::in
/// @param     fld as const db::field &
/// @param     f2 as const db::char_type *
///
/// @date      20:2:2009   14:23
///
inline db::expr::in operator<<(
	  const db::field& fld
	, const db::char_type* f2)
{ return db::expr::in(fld,f2); }

/// @brief     overload from the global << operator
///
/// <BR>qualifier
/// <BR>access    public  
/// @return    db::expr::in
/// @param     fld as const db::field &
/// @param     f2 as const db::string &
///
/// @date      20:2:2009   14:23
///
inline db::expr::in operator<<(const db::field& fld, const db::string& f2)
{ return db::expr::in(fld,f2); }

/// @brief     syntactic sugar to expression-api, field.in(sel)
///
/// <BR>qualifier const
/// <BR>access    public  
/// @return    db::expr::in
/// @param     set as const string &
///
/// @date      20:2:2009   14:24
///
inline db::expr::in db::field::In(const string& set) const
{ return expr::in(*this, set); }

/// @brief     syntactic sugar to expression-api, field.in(sel)
///
/// <BR>qualifier const
/// <BR>access    public  
/// @return    db::expr::in
/// @param     sel as const sel &
///
/// @date      20:2:2009   14:23
///
inline db::expr::in db::field::In(const sel& sel) const
{ return expr::in(*this, sel); }

/// @brief     syntactic sugar to expression-api, field.like(string)
///
/// <BR>qualifier
/// <BR>access    public  
/// @return    db::expr::like
/// @param     s as const string &
///
/// @date      20:2:2009   14:24
///
inline db::expr::like db::field::Like(const string& s)
{ return expr::like(*this, s); }

inline db::expr::in::in(const db::field & fld, const db::sel& s) : 
  oper(fld, DB_TEXT("in"), DB_TEXT("(") + s.str() + DB_TEXT(")")) 
{
}

inline void db::row::fill(sqlite3_stmt* stm)
{
  if (!stm)
    return;
  if (!_query)
    return;

  _data.reserve(sqlite3_column_count(stm));
  for (int i(0), sz(sqlite3_column_count(stm)); i < sz; ++i)
  {
    _data.push_back(param(i));
    _data.back().set(stm);
  } 
}

inline db::query& db::base::execute(const string& cmd)
{
  progress_handler progress(_db, this);

  type_last_queries::iterator it(_last_queries.find(cmd));
  if (it==_last_queries.end())
  {
    _last_queries.insert(type_last_queries::value_type(cmd, query(*this)));
    it = _last_queries.find(cmd);
  }

  it->second.execute(cmd);
  return it->second;
}

inline db::query_ptr db::base::execute_ptr(const string& cmd)
{
  progress_handler progress(_db, this);
  db::query_ptr ret(new query(*this));
  if (ret)
    ret->execute(cmd);
  return ret;
}

inline void db::query::execute(const string& cmd)
{
  if (_stm) { sqlite3_finalize(_stm); _stm = 0; }
  sqlite3* _db(_base.get_db_ptr());

  _data.clear();

  int rc;
  while (1) 
  {
#ifdef _UNICODE
    rc = sqlite3_prepare16_v2(_db, 
      reinterpret_cast<const wchar_t*>(cmd.c_str()), 
      cmd.length()*sizeof(wchar_t), 
      &_stm, NULL);
#else
    rc = sqlite3_prepare_v2(_db, 
      reinterpret_cast<const char*>(cmd.c_str()), 
      cmd.length()*sizeof(char), 
      &_stm, NULL);
#endif // _UNICODE

    if (rc != SQLITE_OK || _stm == NULL) 
    {
      switch(rc) 
      {
      case SQLITE_BUSY: 
      case SQLITE_LOCKED: 
				/// @todo portabel machen
#if defined (WIN32) || defined (WIN64)
        Sleep(250); 
#else
				sleep(250); 
#endif
        break;
      default: 
        _base.throw_error(rc);
      }
    }
    else
      break;
  } 

  if (rc != SQLITE_OK || !_stm)
    return;

  unsigned nRowCnt(0);
  while ((rc = sqlite3_step(_stm)) == SQLITE_ROW) 
  {
    if (nRowCnt==0)
    {
      _field.reserve(sqlite3_column_count(_stm)); string tmp;
      for (int i(0), sz(sqlite3_column_count(_stm)); i < sz; ++i)
      {
#ifdef _UNICODE
        tmp = static_cast<const wchar_t*>(
          sqlite3_column_origin_name16(_stm, i));
#else
        tmp = sqlite3_column_origin_name(_stm, i);
#endif // _UNICODE

        _field.push_back(db::field(tmp));

        switch (sqlite3_column_type(_stm, i))
        {
        case SQLITE_NULL:
          _field.back().set_type(db::e_null);
          break;
        case SQLITE_INTEGER:
          _field.back().set_type(db::e_long);
          break;
        case SQLITE_FLOAT:
          _field.back().set_type(db::e_double);
          break;
        case SQLITE_TEXT:
          _field.back().set_type(db::e_string);
          break;
        case SQLITE_BLOB:
          _field.back().set_type(db::e_blob);
          break;
        default:
          _field.back().set_type(db::e_null);
          break;
        }
      }
    }
    
    _data.push_back(row(this, nRowCnt++));
    _data.back().fill(_stm);
  }
  sqlite3_finalize(_stm);
  _stm = NULL; 
}

void db::detail::w2a::init(
	  const wchar_t* psz
	, unsigned nConvertCodePage)
{
	if (psz == NULL)
	{
		sz_.clear();
		return;
	}

#if defined (WIN32) || defined (WIN64)
	int lenW(lstrlenW( psz )+1);		 
	int lenA(lenW*4);

	sz_.resize(lenA);

	bool failed(0 == ::WideCharToMultiByte( 
		  nConvertCodePage
		, 0
		, psz
		, lenW
		, &*sz_.begin()
		, lenA
		, NULL
		, NULL));

	if (failed)
	{
		if (GetLastError()==ERROR_INSUFFICIENT_BUFFER)
		{
			lenA = ::WideCharToMultiByte( 
				nConvertCodePage
				, 0
				, psz
				, lenW
				, NULL
				, 0
				, NULL
				, NULL);

			sz_.resize(lenA);

			failed=(0 == ::WideCharToMultiByte(
				nConvertCodePage
				, 0
				, psz
				, lenW
				, &*sz_.begin()
				, lenA
				, NULL
				, NULL));
		}			
	}

	if (failed)
	{
		db::stringstream err;
		err << DB_TEXT("Lasterror was ") 
			<< GetLastError() 
			<< DB_TEXT("\n");
		throw db::exception::unknown_error(err.str());
	}
#else
	/// @todo linux conversion...
#endif
}

inline void db::detail::a2w::init(
	  const char* psz
	, unsigned nCodePage)
{
	if (psz == NULL)
	{
		sz_.clear();
		return;
	}

#if defined (WIN32) || defined (WIN64)
	int lenA(lstrlenA( psz )+1);		 
	int lenW(lenA);

	sz_.resize(lenW);

	bool failed(0 == ::MultiByteToWideChar(
		  nCodePage
		, 0
		, psz
		, lenA
		, &*sz_.begin()
		, lenW));

	if (failed)
	{
		if (GetLastError()==ERROR_INSUFFICIENT_BUFFER)
		{
			lenW = ::MultiByteToWideChar(
				  nCodePage
				, 0
				, psz
				, lenA
				, NULL
				, 0);

			sz_.resize(lenW);

			failed=(0 == ::MultiByteToWideChar(
				  nCodePage
				, 0
				, psz
				, lenA
				, &*sz_.begin()
				, lenW));
		}			
	}

	if (failed)
	{
		db::stringstream err;
		err << DB_TEXT("Lasterror was ") 
			<< GetLastError() 
			<< DB_TEXT("\n");
		throw db::exception::unknown_error(err.str());
	}
#else
	/// @todo linux conversion...
#endif
}

#endif // db_h__
