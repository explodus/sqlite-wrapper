///	@file		include/sqlite_wrapper/detail/ins.hpp
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

#ifndef DB_INS_HPP_INCLUDED
#define DB_INS_HPP_INCLUDED

#include <sqlite_wrapper/config.hpp>

#include <sqlite_wrapper/detail/tools.hpp>
#include <sqlite_wrapper/detail/param.hpp>
#include <sqlite_wrapper/detail/expr.hpp>

namespace db
{
  /// @brief helper class, generates INSERT SQL statements
  class SQLITE_WRAPPER_DLLAPI ins 
  {
  protected:
    split_map _values;
    string    _source;
    string    _delim1;
    string    _delim2;

  public:
		///
		/// <BR>qualifier
		/// <BR>access    public 
		/// 
		/// @return       
		/// @param        tab as const string &
		///
		/// @date         20.4.2010 20:37
		/// @ingroup			constructors
		/// 
		ins(const string& tab);

    ///
    /// <BR>qualifier
    /// <BR>access    public 
    /// 
    /// @return       
    /// @param        tab as const string &
    /// @param        delim as const string &
    ///
    /// @date         20.4.2010 20:37
		/// @ingroup			constructors
    /// 
    ins(const string& tab, const string& delim);

    ///
    /// <BR>qualifier
    /// <BR>access    public 
    /// 
    /// @return       
    /// @param        tab as const string &
    /// @param        delim1 as const string &
    /// @param        delim2 as const string &
    ///
    /// @date         20.4.2010 20:38
		/// @ingroup			constructors
    /// 
    ins(const string& tab, const string& delim1, const string& delim2);
    
    /// @brief        destructor
    ///
    /// <BR>qualifier
    /// <BR>access    public 
    /// 
    /// @return       
    ///
    /// @date         20.4.2010 20:39
    /// 
    ~ins();
    
    /// @brief     clear_values
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    ins &
    ///
    /// @date      20:2:2009   9:07
    ///
    ins & clear_values();

    /// @brief     operator%
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    ins &
    /// @param     f as const db::field &
    ///
    /// @date      20:2:2009   9:07
    ///
    ins & operator%(const db::field& f);

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
    ins & values(string t, param* v);

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
    ins & values(string t, string v, param_types p=e_int);

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
    operator string() const;

    /// @brief     str
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    db::string
    ///
    /// @date      20:2:2009   11:05
    ///
    string str() const;
  }; 

}

///
/// @page usage examples of the wrapper
/// @ingroup sqlite_wrapper_examples
/// an usage example of the insert expression generator:
///
/// @code
///
/// db::string sql(DB_TEXT("INSERT INTO gps (id,latitude,longitude) VALUES (1234,11.1234,53.1234)"));
///
/// db::ins i((
/// 	  db::ins(DB_TEXT("gps")) 
/// 	% db::field(DB_TEXT("id"), 1234) 
/// 	% db::field(DB_TEXT("longitude"), 53.1234) 
/// 	% db::field(DB_TEXT("latitude"), 11.1234)
/// 	)); 
///
/// BOOST_CHECK_MESSAGE( sql == db::string(i)
/// 	, "\n sql is: \"" 
/// 	<< db::detail::w2a(sql.c_str())
/// 	<< "\",\n ins is: \"" 
/// 	<< db::detail::w2a(db::string(i).c_str())
/// 	<< "\"" );
///
/// @endcode
///

#endif