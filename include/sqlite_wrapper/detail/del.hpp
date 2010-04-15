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
  ///helper class, generates DELETE SQL statements
  class SQLITE_WRAPPER_DLLAPI del : public sel
  {
  public:
    del(const string& tablename);

    /// @brief     operator string
    ///
    /// <BR>qualifier const
    /// <BR>access    virtual public  
    /// @return    
    ///
    /// @date      20:2:2009   14:12
    ///
    virtual operator string() const;

    /// @brief     ~del
    ///
    /// <BR>qualifier
    /// <BR>access    virtual public  
    /// @return    
    ///
    /// @date      20:2:2009   14:12
    ///
    virtual ~del();
  };

}

#endif