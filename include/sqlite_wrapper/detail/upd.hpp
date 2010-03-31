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

namespace db
{
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

}

#endif