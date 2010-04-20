///	@file		include/sqlite_wrapper/detail/sel.hpp
///	@date		15.3.2010 8:45
///	file base	tools
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
///	@brief 

#ifndef DB_SEL_HPP_INCLUDED
#define DB_SEL_HPP_INCLUDED

#include <sqlite_wrapper/config.hpp>

#include <sqlite_wrapper/detail/tools.hpp>
#include <sqlite_wrapper/detail/param.hpp>
#include <sqlite_wrapper/detail/expr.hpp>

namespace db
{
  /// @brief helper class, generates SELECT SQL statements
	///
	/// @code
	/// void generate_select_expression()
	/// {
	///	 using db::string;
	///	 using db::sel;
	///	 using db::field;
	///
	///	 string sql(DB_TEXT("SELECT longitude,latitude FROM gps WHERE id = 1234"));
	///
	///	 sel s(((
	/// 		  sel(DB_TEXT("gps")) 
	/// 		, DB_TEXT("longitude")
	/// 		, DB_TEXT("latitude"))
	/// 		% (field(DB_TEXT("id"), 1) == 1234)));
	/// 
	/// 	BOOST_CHECK_MESSAGE( sql == string(s)
	/// 		, "\n sql is: \"" 
	/// 		<< db::detail::w2a(sql.c_str())
	/// 		<< "\",\n sel is: \"" 
	/// 		<< db::detail::w2a(string(s).c_str())
	/// 		<< "\"" );
	/// }
	/// @endcode
	///
  class SQLITE_WRAPPER_DLLAPI sel 
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
		/// @defgroup constructors summary of all constructors
		/// @defgroup operators summary of all operator overloads

    /// @brief        constructor
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return       
    ///
    /// @date         20.4.2010 10:38
		/// @ingroup			constructors
    ///
    sel();

    /// @brief        constructor
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return       
    /// @param        tablename as const string &
    ///
    /// @date         20.4.2010 10:38
		/// @ingroup			constructors
    ///
    sel(const string& tablename);

    /// @brief        constructor
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return       
    /// @param        tablename as const string &
    /// @param        delim as const string &
    ///
    /// @date         20.4.2010 10:39
		/// @ingroup			constructors
    ///
    sel(const string& tablename, const string& delim);

    /// @brief        constructor
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return       
    /// @param        tablename as const string &
    /// @param        delim1 as const string &
    /// @param        delim2 as const string &
    ///
    /// @date         20.4.2010 10:39
		/// @ingroup			constructors
    ///
    sel(const string& tablename, const string& delim1, 
      const string& delim2);

    /// @brief        copy constructor
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return       
    /// @param        s as const sel &
    ///
    /// @date         20.4.2010 10:39
		/// @ingroup			constructors
    ///
    sel(const sel& s);

    /// @brief        destructor
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return       
    ///
    /// @date         20.4.2010 10:41
    ///
    virtual ~sel();

		/// @brief        simple copy operator
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return       sel & - itself
		/// @param        s as const sel &
		///
		/// @date         20.4.2010 10:54
		/// @ingroup			operators
		///
		sel & operator=(const sel& s);

    /// @brief        adds a column name
		///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return       sel & - itself
    /// @param        s as const string & - column name
    ///
    /// @date         20.4.2010 10:51
		/// @ingroup			operators
		///
    virtual sel & operator,(const string& s);

    /// @brief        adds a where expression
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return       sel & - itself
    /// @param        e as const expr::base & - where expression
    ///
    /// @date         20.4.2010 10:53
		/// @ingroup			operators
    ///
    virtual sel & operator%(const expr::base& e);

    /// @brief        joins two fields
    ///
    /// <BR>qualifier
    /// <BR>access    virtual public  
    /// @return       sel & - itself
    /// @param        f as const field_pair &
    ///
    /// @date         20.4.2010 10:56
		/// @ingroup			operators
    ///
    virtual sel & operator<(const field_pair& f);

    /// @brief     distinct
    ///
    /// <BR>qualifier
    /// <BR>access    virtual public  
    /// @return    sel &
    /// @param     d as bool
    ///
    /// @date      20:2:2009   14:02
    ///
    virtual sel & distinct(bool d);

    /// @brief     limit
    ///
    /// <BR>qualifier
    /// <BR>access    virtual public  
    /// @return    sel &
    /// @param     value as int
    ///
    /// @date      20:2:2009   14:02
    ///
    virtual sel & limit(int value);

    /// @brief     join
    ///
    /// <BR>qualifier
    /// <BR>access    virtual public  
    /// @return    sel &
    /// @param     f as const field_pair&
    ///
    /// @date      20:2:2009   14:02
    ///
    virtual sel & join(const field_pair& f);

    /// @brief     offset
    ///
    /// <BR>qualifier
    /// <BR>access    virtual public  
    /// @return    sel &
    /// @param     value as int 
    ///
    /// @date      20:2:2009   14:02
    ///
    virtual sel & offset(int value);

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
    virtual sel & result_no_delim(string r, string alias=DB_TEXT(""));

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
    virtual sel & result(string r, string alias=DB_TEXT(""));

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
    virtual sel & result_max(string r, string alias=DB_TEXT(""));

    /// @brief     clear
    ///
    /// <BR>qualifier
    /// <BR>access    virtual public  
    /// @return    sel &
    ///
    /// @date      20:2:2009   14:01
    ///
    virtual sel & clear();

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
    virtual sel & source(string s, string alias=DB_TEXT(""));

    /// @brief     where
    ///
    /// <BR>qualifier
    /// <BR>access    virtual public  
    /// @return    sel &
    /// @param     w as const expr::base &
    ///
    /// @date      20:2:2009   14:01
    ///
    virtual sel & where(const expr::base & w);

    /// @brief     where
    ///
    /// <BR>qualifier
    /// <BR>access    virtual public  
    /// @return    sel &
    /// @param     w as string
    ///
    /// @date      20:2:2009   14:01
    ///
    virtual sel & where(string w);

    /// @brief     group_by
    ///
    /// <BR>qualifier
    /// <BR>access    virtual public  
    /// @return    sel &
    /// @param     gb as string
    ///
    /// @date      20:2:2009   14:01
    ///
    virtual sel & group_by(string gb);

    /// @brief     having
    ///
    /// <BR>qualifier
    /// <BR>access    virtual public  
    /// @return    sel &
    /// @param     h as const expr::base &
    ///
    /// @date      20:2:2009   14:01
    ///
    virtual sel & having(const expr::base & h);

    /// @brief     having
    ///
    /// <BR>qualifier
    /// <BR>access    virtual public  
    /// @return    sel &
    /// @param     h as string
    ///
    /// @date      20:2:2009   14:01
    ///
    virtual sel & having(string h);

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
    virtual sel & order_by(string ob, bool ascending=true);

		/// @brief        db::string overload
		///
		/// <BR>qualifier const
		/// <BR>access    virtual public  
		/// @return       
		///
		/// @date         20.4.2010 10:57
		/// @ingroup			operators
		///
		virtual operator string() const;

		/// @brief        same as the string operator overload, more stl like 
		///
		/// <BR>qualifier const
		/// <BR>access    public  
		/// @return       db::string
		///
		/// @date         15.4.2010 13:59
		///
    virtual string str() const;
  }; 

}

#endif