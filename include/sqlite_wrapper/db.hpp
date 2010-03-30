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

#include <sqlite_wrapper/detail/time.hpp>
#include <sqlite_wrapper/detail/tools.hpp>
#include <sqlite_wrapper/detail/exception.hpp>
#include <sqlite_wrapper/detail/field.hpp>
#include <sqlite_wrapper/detail/expr.hpp>

namespace db
{
  class row;
  class query;
  class base;

  ///dynamic parameter class*/
  class SQLITE_WRAPPER_DLLAPI param 
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
    param();
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
    param(unsigned col);

    /// @brief     ~param
    ///
    /// <BR>qualifier
    /// <BR>access    virtual public  
    /// @return    
    ///
    /// @date      20:2:2009   14:35
    ///
    ~param();

    /// @brief     copy constructor
    ///
    /// <BR>qualifier : _col(Param._col), _type(Param._type), _is_changed(false), _data(Param._data)
    /// <BR>access    public  
    /// @return    
    /// @param     Param as const param &
    ///
    /// @date      20:2:2009   14:35
    ///
    param(const param& Param);               

    ///
    /// <BR>qualifier const 
    /// <BR>access    public  
    /// @return    const param_types&
    ///
    /// @date      20:2:2009   14:35
    ///
    param_types get_type() const;

    /// @brief     set_type
    ///
    /// <BR>qualifier 
    /// <BR>access    public  
    /// @return    void
    /// @param     Type as param_types
    ///
    /// @date      20:2:2009   14:35
    ///
    void set_type(param_types Type);

    /// @brief     is param changed?
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    bool&
    ///
    /// @date      20:2:2009   14:35
    ///
    bool& is_changed();

    ///
    /// <BR>qualifier
    /// <BR>access    virtual public  
    /// @return    void
    /// @param     stm as sqlite3_stmt *
    ///
    /// @date      20:2:2009   14:34
    ///
    void set(sqlite3_stmt* stm);

    ///gesetzt als bool
    void set(bool dat); 
    ///gesetzt als int
    void set(int dat); 
    ///gesetzt als unsigned
    void set(unsigned dat); 
    ///gesetzt als long
    void set(long dat); 
    ///gesetzt als float
    void set(float dat); 
    ///gesetzt als double
    void set(double dat); 
    ///gesetzt als char
    void set(char dat); 
    ///gesetzt als const db::char_type*
    void set(const db::char_type* dat); 
    ///gesetzt als const string&
    void set(const string& dat, bool isBlob=false); 

#ifndef BOOST_NO_STD_LOCALE
    ///gesetzt als boost::gregorian::date
		void set(const boost::gregorian::date& dat); 
#else
    ///gesetzt als time_t_ce
    void set(const time_t_ce& dat); 
#endif

    /// @brief     set_null
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    void
    ///
    /// @date      20:2:2009   14:39
    ///
    void set_null();

    /// @brief     set_with_old_type
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    void
    /// @param     data as const string &
    ///
    /// @date      20:2:2009   14:40
    ///
    void set_with_old_type(const string& data);

    /// @brief     set_with_old_type
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    void
    /// @param     data as const param &
    ///
    /// @date      20:2:2009   14:40
    ///
    void set_with_old_type(const param& data);

    /// @brief     set_with_old_type
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    void
    /// @param     data as const param *
    ///
    /// @date      20:2:2009   14:40
    ///
    void set_with_old_type(const param* data);

    /// @brief     is_null
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    bool
    ///
    /// @date      20:2:2009   14:33
    ///
    bool is_null() const;

    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    bool
    ///
    /// @date      20:2:2009   14:40
    ///
    bool get_bool() const; 


    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    int
    ///
    /// @date      20:2:2009   14:40
    ///
    int get_int() const;

    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    unsigned
    ///
    /// @date      20:2:2009   14:42
    ///
    unsigned get_unsigned() const;

    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    long
    ///
    /// @date      11:3:2009   13:56
    ///
    long get_long() const;

    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    float
    ///
    /// @date      11:3:2009   13:56
    ///
    float get_float() const;

    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    double
    ///
    /// @date      11:3:2009   13:57
    ///
    double get_double() const;

    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    char
    ///
    /// @date      11:3:2009   13:57
    ///
    db::char_type get_char() const;

    operator string() const;

    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    db::string
    ///
    /// @date      18:2:2009   11:33
    ///
    string& str();

    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    const string&
    ///
    /// @date      18:2:2009   11:33
    ///
    const string& str() const;

#ifdef _UNICODE
    const wchar_t* c_str() const;
#else
    const char* c_str() const;
#endif // _UNICODE

#ifndef BOOST_NO_STD_LOCALE
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    boost::gregorian::date
    ///
    /// @date      18:2:2009   11:34
    ///
    boost::gregorian::date get_date_time() const;
#else
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    db::time_t_ce
    /// @param     tmp as const time_t_ce *
    ///
    /// @date      19:3:2009   10:36
    ///
    time_t_ce get_date_time(const time_t_ce* tmp = 0) const;
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
    COleDateTime get_date_time(const COleDateTime* tmp = 0) const;
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
    const param& operator=( const param &v );

    /// @brief     operator!
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    bool
    ///
    /// @date      20:2:2009   9:27
    ///
    bool operator!() const;

    /// @brief     operator==
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    bool
    /// @param     v as const param &
    ///
    /// @date      20:2:2009   9:24
    ///
    bool operator== (const param& v) const;

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
  class SQLITE_WRAPPER_DLLAPI row
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
    row(query* query_, unsigned row_);

    /// @brief     row
    ///
    /// <BR>qualifier : _query(r._query), _row(r._row)
    /// <BR>access    public  
    /// @return    
    /// @param     r as const row &
    ///
    /// @date      20:2:2009   9:23
    ///
    row(const row& r);

    /// @brief     ~row
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    
    ///
    /// @date      20:2:2009   9:23
    ///
    ~row();


    /// @brief     get a value by number
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    const param&
    /// @param     field_num as size_type 
    ///
    /// @date      20:2:2009   9:23
    ///
    const param& value(size_type field_num) const;

    /// @brief     get a value by number
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    param&
    /// @param     field_num as size_type
    ///
    /// @date      20:2:2009   9:23
    ///
    param& value(size_type field_num);

    /// @brief     get a value by number
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    db::row::const_reference
    /// @param     field_num as size_type
    ///
    /// @date      20:2:2009   9:22
    ///
    const_reference operator[] (size_type field_num) const;

    /// @brief     get a value by number
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    db::row::reference
    /// @param     field_num as size_type
    ///
    /// @date      20:2:2009   9:21
    ///
    reference operator[] (size_type field_num);

    /// @brief     operator[]
		/// @todo			 need an implementation
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    param*
    /// @param     field_title as const string &
    ///
    /// @date      20:2:2009   9:21
    ///
    param* operator[] (const string& field_title);

    /// @brief     const begin iterator
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    db::row::const_iterator
    ///
    /// @date      20:2:2009   9:21
    ///
    const_iterator begin() const;

    /// @brief     const end iterator
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    db::row::const_iterator
    ///
    /// @date      20:2:2009   9:21
    ///
    const_iterator end() const;

    /// @brief     begin iterator
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    db::row::iterator
    ///
    /// @date      20:2:2009   9:21
    ///
    iterator begin();

    /// @brief     end iterator
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    db::row::iterator
    ///
    /// @date      20:2:2009   9:20
    ///
    iterator end();

    /// @brief     begin reverse iterator
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    db::row::reverse_iterator
    ///
    /// @date      20:2:2009   9:20
    ///
    reverse_iterator rbegin();

    /// @brief     end reverse iterator
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    db::row::reverse_iterator
    ///
    /// @date      20:2:2009   9:20
    ///
    reverse_iterator rend();

    /// @brief     container size
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    db::row::size_type
    ///
    /// @date      20:2:2009   9:20
    ///
    size_type size() const;

    /// @brief     returns true if the container is empty
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    bool
    ///
    /// @date      20:2:2009   9:19
    ///
    bool empty() const;

    /// @brief     operator==
    ///
    /// <BR>qualifier const 
    /// <BR>access    public  
    /// @return    bool
    /// @param     t as const row &
    ///
    /// @date      20:2:2009   9:19
    ///
    bool operator==(const row& t) const;

    /// @brief     push a param to the container
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    void
    /// @param     v as const value_type &
    ///
    /// @date      20:2:2009   9:18
    ///
    void push_back(const value_type& v);

    /// @brief     fill
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    void
    /// @param     stm as sqlite3_stmt *
    ///
    /// @date      20:2:2009   9:18
    ///
    void fill(sqlite3_stmt* stm);

  };
  typedef boost::detail::allocator::partial_std_allocator_wrapper<
    boost::detail::quick_allocator<row > > alloc_row; 

  ///cursor interface
  class SQLITE_WRAPPER_DLLAPI query
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

    query(base& base_);

    query(const query& q);

    ~query() {}

    void execute(const string& cmd);

    ///get a row by number
    const row& getRow(size_type row_num) const;
    row& getRow(size_type row_num);

    ///get a row by number
    const_reference operator[] (size_type row_num) const;
    reference operator[] (size_type row_num);

    ///get id from field title
    int getIdOf(const string& sField) const;
    string getTitleOf(unsigned iField) const;
    const field* getFieldInfo(unsigned iField) const;

    ///begin iterator
    const_iterator begin() const;
    ///end iterator
    const_iterator end() const;
    ///begin iterator
    iterator begin();
    ///end iterator
    iterator end();
    ///begin iterator
    const_fiterator fbegin() const;
    ///end iterator
    const_fiterator fend() const;
    ///begin iterator
    fiterator fbegin();
    ///end iterator
    fiterator fend();
    ///container size
    size_type size() const;
    ///is container empty?
    bool empty() const;
    bool f_empty() const;

    bool operator==(const query& t) const;

    ///push a row to the row container
    void push_back(const value_type& v);
    ///push a field title to the field container
    void push_back(const fvalue_type& v);
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
  class SQLITE_WRAPPER_DLLAPI base
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
      static int xProgressCallback(void* db_); 

      sqlite3* _db;
    public:
      progress_handler(sqlite3* db_, base* base_);
    	~progress_handler();
    };
    
    typedef std::map<string, query> type_last_queries; 
    type_last_queries _last_queries; 

	protected:
		friend class query;

		/// @brief     throw_error
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return    void
		/// @param     status as int
		///
		/// @date      20:2:2009   14:13
		///
		void throw_error(int status); 

  public:
    /// @brief     base
    ///
    /// <BR>qualifier : _db(0)
    /// <BR>access    public  
    /// @return    
    ///
    /// @date      20:2:2009   14:14
    ///
    base();

    /// @brief     ~base
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    
    ///
    /// @date      20:2:2009   14:14
    ///
    ~base();

    enum feature
    {
      e_transact,
      e_blob
    };

    /// @brief     open the specified db
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    void
    /// @param     file as const std::string &
    ///
    /// @date      20:2:2009   14:14
    ///
    void connect(const std::string& file);

    /// @brief     begins SQL transaction
    ///
    /// <BR>qualifier 
    /// <BR>access    public  
    /// @return    void
    ///
    /// @date      20:2:2009   14:14
    ///
    void begin();

    /// @brief     commits SQL transaction
    ///
    /// <BR>qualifier 
    /// <BR>access    public  
    /// @return    void
    ///
    /// @date      20:2:2009   14:15
    ///
    void commit();

    /// @brief     cancels active SQL transaction
    ///
    /// <BR>qualifier 
    /// <BR>access    public  
    /// @return    void
    ///
    /// @date      20:2:2009   14:15
    ///
    void rollback();

    /// @brief     execute SQL statement
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    query&
    /// @param     cmd as const string &
    ///
    /// @date      20:2:2009   14:15
    ///
    query& execute(const string& cmd);

    /// @brief     execute
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    db::query_ptr
    /// @param     cmd as const string &
    ///
    /// @date      13:7:2009   13:14
    ///
    query_ptr execute_ptr(const string& cmd);

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

void db::detail::w2a::init(
	  const wchar_t* psz
	, unsigned nConvertCodePage)
{
	if (psz == 0)
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
		, 0
		, 0));

	if (failed)
	{
		if (GetLastError()==ERROR_INSUFFICIENT_BUFFER)
		{
			lenA = ::WideCharToMultiByte( 
				nConvertCodePage
				, 0
				, psz
				, lenW
				, 0
				, 0
				, 0
				, 0);

			sz_.resize(lenA);

			failed=(0 == ::WideCharToMultiByte(
				nConvertCodePage
				, 0
				, psz
				, lenW
				, &*sz_.begin()
				, lenA
				, 0
				, 0));
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
	if (psz == 0)
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
				, 0
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
