///	@file		include/sqlite_wrapper/detail/del.hpp
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

#ifndef DB_DEL_HPP_INCLUDED
#define DB_DEL_HPP_INCLUDED

#include <sqlite_wrapper/config.hpp>

#include <sqlite_wrapper/detail/sel.hpp>

namespace db
{
  /// @brief helper class, generates DELETE SQL statements
  class SQLITE_WRAPPER_DLLAPI del : public sel
  {
  public:
    /// @brief        constructor
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return       
    /// @param        tablename as const string &
    ///
    /// @date         20.4.2010 10:07
		/// @ingroup			constructors
    ///
    del(const string& tablename);

		/// @brief        db::string overload
    ///
    /// <BR>qualifier const
    /// <BR>access    virtual public  
    /// @return       db::string
    ///
    /// @date         20.4.2010 10:09
		/// @ingroup			operators
    ///
    virtual operator string() const;

    /// @brief        destructor
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return       
    ///
    /// @date         20.4.2010 10:09
    ///
    virtual ~del();
  };

}

///
/// @page usage examples of the wrapper
/// @ingroup sqlite_wrapper_examples
/// an usage example of the delete expression generator:
///
/// @code
///
/// db::string sql(DB_TEXT("DELETE FROM gps WHERE id = 1234"));
///
/// db::del d(DB_TEXT("gps")); 
/// d.where((db::field(DB_TEXT("id"), 1) == 1234));
///
/// BOOST_CHECK_MESSAGE( sql == db::string(d)
/// 	, "\n sql is: \"" 
/// 	<< db::detail::w2a(sql.c_str())
/// 	<< "\",\n sel is: \"" 
/// 	<< db::detail::w2a(db::string(d).c_str())
/// 	<< "\"" );
///
/// @endcode
///

#endif