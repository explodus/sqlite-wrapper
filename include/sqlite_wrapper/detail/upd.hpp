///	@file		include/sqlite_wrapper/detail/upd.hpp
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

#ifndef DB_UPD_HPP_INCLUDED
#define DB_UPD_HPP_INCLUDED

#include <sqlite_wrapper/config.hpp>

#include <sqlite_wrapper/detail/tools.hpp>
#include <sqlite_wrapper/detail/param.hpp>
#include <sqlite_wrapper/detail/expr.hpp>

namespace db
{
	///	@brief helper class for generating update sql expressions
	class SQLITE_WRAPPER_DLLAPI upd 
  {
    string _delim1;
    string _delim2;
    string table;
    string _where;
    split fields;
    split values;
  public:
    ~upd();

    /// <BR>qualifier
    /// <BR>access    public  
    /// @return       
    /// @param        t as const string & - should contain a table name
    ///
    /// @date         15.4.2010 13:52
		/// @ingroup			constructors
    ///
    upd(const string& t);

    /// <BR>qualifier
    /// <BR>access    public  
    /// @return       
		/// @param        t as const string & - should contain a table name
    /// @param        d as const string & - holds a front and back delimiter
    ///
    /// @date         15.4.2010 13:53
		/// @ingroup			constructors
    ///
    upd(const string& t, const string& d);

    /// @brief        upd
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return       
		/// @param        t as const string & - should contain a table name
    /// @param        d1 as const string & - the front delimiter
    /// @param        d2 as const string & - the back delimiter
    ///
    /// @date         15.4.2010 13:54
		/// @ingroup			constructors
    ///
    upd(const string& t, const string& d1, const string& d2);

    /// @brief        
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return       upd&
    /// @param        e as const expr::base &
    ///
    /// @date         15.4.2010 13:55
    ///
    upd& where(const expr::base& e);

    /// @brief        insert a sql where expression through the operator % overload 
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return       upd &
    /// @param        e as const expr::base &
    ///
    /// @date         15.4.2010 13:55
    ///
    upd & operator%(const expr::base& e);

		/// @brief        insert a new field with it name and the value
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return       upd &
		/// @param        f as const db::field &
		///
		/// @date         15.4.2010 13:58
		///
		upd & operator%(const db::field& f);

    /// @brief        insert a new field with it name and the value
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return       upd&
    /// @param        f as const field &
    /// @param        val as const string &
    ///
    /// @date         15.4.2010 13:56
    ///
    upd& set(const field& f, const string& val);

		/// @brief        insert a new field with it name and the value
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return       upd&
    /// @param        f as const field &
    /// @param        val as const T &
    ///
    /// @date         15.4.2010 13:58
    ///
    template<typename T>
    upd& set(const field& f, const T& val)
    { return set(f, detail::to_string(val)); }

    /// @brief        string overload
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return       db::string
    ///
    /// @date         15.4.2010 13:59
		/// @ingroup			operators
    ///
    operator string() const;

    /// @brief        same as the string operator overload, more stl like 
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return       db::string
    ///
    /// @date         15.4.2010 13:59
    ///
    string str() const;
  };

}

///
/// @page usage examples of the wrapper
/// @ingroup sqlite_wrapper_examples
/// an usage example of the update expression generator:
///
///@code
///
///db::string sql(DB_TEXT("UPDATE gps SET longitude=11.12345678,latitude=53.12345678 WHERE id = 1234"));
///
///db::upd u((
///	  db::upd(DB_TEXT("gps")) 
///	% db::field(DB_TEXT("longitude"), 11.12345678)
///	% db::field(DB_TEXT("latitude"), 53.12345678) 
///	% (db::field(DB_TEXT("id"), 1) == 1234)));
///
///BOOST_CHECK(sql == string(u));
///
///@endcode
///

#endif