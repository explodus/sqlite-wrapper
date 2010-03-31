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

namespace db
{
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

}

#endif