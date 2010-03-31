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

namespace db
{
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

}

#endif