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

}

#endif