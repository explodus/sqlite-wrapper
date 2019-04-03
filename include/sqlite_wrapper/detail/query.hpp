///	@file		include/sqlite_wrapper/detail/query.hpp
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

#ifndef DB_QUERY_HPP_INCLUDED
#define DB_QUERY_HPP_INCLUDED

#include <sqlite_wrapper/config.hpp>
#include <sqlite_wrapper/detail/field.hpp>
#include <sqlite_wrapper/detail/row.hpp>

namespace db
{
	class base;

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

    base* _base;
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
		typedef data_type::reverse_iterator reverse_iterator;
		typedef data_type::const_reverse_iterator const_reverse_iterator;
    typedef field_type::iterator fiterator;
    typedef field_type::const_iterator const_fiterator;
		
		/// @brief        query
		///
		/// @return       
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         31.3.2010 21:54
		query();

    /// @brief        query
    ///
    /// <BR>qualifier
    /// <BR>access    public 
    /// 
    /// @return       
    /// @param        base_ as base &
    ///
		/// @author       T. Schroeder (explodus@gmx.de)
    /// @date         31.3.2010 21:55
    /// 
    query(base& base_);

    /// @brief        query
    ///
    /// <BR>qualifier
    /// <BR>access    public 
    /// 
    /// @return       
    /// @param        q as const query &
    ///
		/// @author       T. Schroeder (explodus@gmx.de)
    /// @date         31.3.2010 21:55
    /// 
    query(const query& q);

    /// @brief        ~query
    ///
    /// <BR>qualifier
    /// <BR>access    public 
    /// 
    /// @return       
    ///
		/// @author       T. Schroeder (explodus@gmx.de)
    /// @date         31.3.2010 21:55
    /// 
    ~query();

    /// @brief        execute
    ///
    /// <BR>qualifier
    /// <BR>access    public 
    /// 
    /// @return       void
    /// @param        cmd as const string &
    ///
		/// @author       T. Schroeder (explodus@gmx.de)
    /// @date         31.3.2010 21:55
    /// 
    void execute(const string& cmd);

    /// @brief        get a row by number
    ///
    /// <BR>qualifier const
    /// <BR>access    public 
    /// 
    /// @return       const row&
    /// @param        row_num as size_type
    ///
		/// @author       T. Schroeder (explodus@gmx.de)
    /// @date         31.3.2010 21:55
    /// 
    const row& getRow(size_type row_num) const;

    /// @brief        get a row by number
    ///
    /// <BR>qualifier
    /// <BR>access    public 
    /// 
    /// @return       row&
    /// @param        row_num as size_type
    ///
		/// @author       T. Schroeder (explodus@gmx.de)
    /// @date         31.3.2010 21:55
    /// 
    row& getRow(size_type row_num);

    /// @brief        get a row by number
    ///
    /// <BR>qualifier const
    /// <BR>access    public 
    /// 
    /// @return       db::query::const_reference
    /// @param        row_num as size_type
    ///
		/// @author       T. Schroeder (explodus@gmx.de)
    /// @date         31.3.2010 21:57
    /// 
    const_reference operator[] (size_type row_num) const;

    /// @brief        get a row by number
    ///
    /// <BR>qualifier
    /// <BR>access    public 
    /// 
    /// @return       db::query::reference
    /// @param        row_num as size_type
    ///
		/// @author       T. Schroeder (explodus@gmx.de)
    /// @date         31.3.2010 21:57
    /// 
    reference operator[] (size_type row_num);

    /// @brief        get id from field title
    ///
    /// <BR>qualifier const
    /// <BR>access    public 
    /// 
    /// @return       int
    /// @param        sField as const string &
    ///
		/// @author       T. Schroeder (explodus@gmx.de)
    /// @date         31.3.2010 21:57
    /// 
    int getIdOf(const string& sField) const;

    /// @brief        get id from field title
    ///
    /// <BR>qualifier const
    /// <BR>access    public 
    /// 
    /// @return       db::string
    /// @param        iField as unsigned
    ///
		/// @author       T. Schroeder (explodus@gmx.de)
    /// @date         31.3.2010 21:57
    /// 
    string getTitleOf(unsigned iField) const;

    /// @brief        getFieldInfo
    ///
    /// <BR>qualifier const
    /// <BR>access    public 
    /// 
    /// @return       const field*
    /// @param        iField as unsigned
    ///
		/// @author       T. Schroeder (explodus@gmx.de)
    /// @date         31.3.2010 21:57
    /// 
    const field* getFieldInfo(unsigned iField) const;

    /// @brief        begin iterator
    ///
    /// <BR>qualifier const
    /// <BR>access    public 
    /// 
    /// @return       db::query::const_iterator
    ///
		/// @author       T. Schroeder (explodus@gmx.de)
    /// @date         31.3.2010 21:57
    /// 
    const_iterator begin() const;

    /// @brief        end iterator
    ///
    /// <BR>qualifier const
    /// <BR>access    public 
    /// 
    /// @return       db::query::const_iterator
    ///
		/// @author       T. Schroeder (explodus@gmx.de)
    /// @date         31.3.2010 21:57
    /// 
    const_iterator end() const;

    /// @brief        begin iterator
    ///
    /// <BR>qualifier
    /// <BR>access    public 
    /// 
    /// @return       db::query::iterator
    ///
		/// @author       T. Schroeder (explodus@gmx.de)
    /// @date         31.3.2010 21:58
    /// 
    iterator begin();

    /// @brief        end iterator
    ///
    /// <BR>qualifier
    /// <BR>access    public 
    /// 
    /// @return       db::query::iterator
    ///
		/// @author       T. Schroeder (explodus@gmx.de)
    /// @date         31.3.2010 21:58
    /// 
    iterator end();

		/// @brief        begin reverse iterator
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return       db::query::reverse_iterator
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         22.9.2010 11:28
		///
		reverse_iterator rbegin();
		
		/// @brief        end reverse iterator
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return       db::query::reverse_iterator
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         22.9.2010 11:28
		///
		reverse_iterator rend();

		/// @brief        const begin reverse iterator
		///
		/// <BR>qualifier const
		/// <BR>access    public  
		/// @return       db::query::const_reverse_iterator
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         22.9.2010 11:29
		///
		const_reverse_iterator rbegin() const;
		
		/// @brief        const end reverse iterator
		///
		/// <BR>qualifier const
		/// <BR>access    public  
		/// @return       db::query::const_reverse_iterator
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         22.9.2010 11:29
		///
		const_reverse_iterator rend() const;

    /// @brief        begin iterator
    ///
    /// <BR>qualifier const
    /// <BR>access    public 
    /// 
    /// @return       db::query::const_fiterator
    ///
		/// @author       T. Schroeder (explodus@gmx.de)
    /// @date         31.3.2010 21:57
    /// 
    const_fiterator fbegin() const;

    /// @brief        end iterator
    ///
    /// <BR>qualifier const
    /// <BR>access    public 
    /// 
    /// @return       db::query::const_fiterator
    ///
		/// @author       T. Schroeder (explodus@gmx.de)
    /// @date         31.3.2010 21:57
    /// 
    const_fiterator fend() const;

    /// @brief        begin iterator
    ///
    /// <BR>qualifier
    /// <BR>access    public 
    /// 
    /// @return       db::query::fiterator
    ///
		/// @author       T. Schroeder (explodus@gmx.de)
    /// @date         31.3.2010 21:57
    /// 
    fiterator fbegin();

    /// @brief        end iterator
    ///
    /// <BR>qualifier
    /// <BR>access    public 
    /// 
    /// @return       db::query::fiterator
    ///
		/// @author       T. Schroeder (explodus@gmx.de)
    /// @date         31.3.2010 21:57
    /// 
    fiterator fend();

		/// @brief        erase
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return       db::query::iterator
		/// @param        _Where as const_iterator
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         22.9.2010 11:35
		///
		iterator erase(const_iterator _Where);

		/// @brief        erase
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return       db::query::iterator
		/// @param        _First_arg as const_iterator
		/// @param        _Last_arg as const_iterator
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         22.9.2010 11:35
		///
		iterator erase(const_iterator _First_arg, const_iterator _Last_arg);

    /// @brief        container size
    ///
    /// <BR>qualifier const
    /// <BR>access    public 
    /// 
    /// @return       db::query::size_type
    ///
		/// @author       T. Schroeder (explodus@gmx.de)
    /// @date         31.3.2010 21:57
    /// 
    size_type size() const;

    /// @brief        is container empty?
    ///
    /// <BR>qualifier const
    /// <BR>access    public 
    /// 
    /// @return       bool
    ///
		/// @author       T. Schroeder (explodus@gmx.de)
    /// @date         31.3.2010 21:57
    /// 
    bool empty() const;

    /// @brief        is container empty?
    ///
    /// <BR>qualifier const
    /// <BR>access    public 
    /// 
    /// @return       bool
    ///
		/// @author       T. Schroeder (explodus@gmx.de)
    /// @date         31.3.2010 21:57
    /// 
    bool f_empty() const;

    /// @brief        operator==
    ///
    /// <BR>qualifier const
    /// <BR>access    public 
    /// 
    /// @return       bool
    /// @param        t as const query &
    ///
    /// @author       Torsten Schroeder
    /// @author       schroeder@ipe-chemnitz.de / explodus@gmx.de
    /// @date         31.3.2010 22:01
    /// 
    bool operator==(const query& t) const;

    /// @brief        push a row to the row container
    ///
    /// <BR>qualifier
    /// <BR>access    public 
    /// 
    /// @return       void
    /// @param        v as const value_type &
    ///
		/// @author       T. Schroeder (explodus@gmx.de)
    /// @date         31.3.2010 21:57
    /// 
    void push_back(const value_type& v);

    /// @brief        push a field title to the field container
    ///
    /// <BR>qualifier
    /// <BR>access    public 
    /// 
    /// @return       void
    /// @param        v as const fvalue_type &
    ///
		/// @author       T. Schroeder (explodus@gmx.de)
    /// @date         31.3.2010 21:57
    /// 
    void push_back(const fvalue_type& v);
  };

	typedef boost::shared_ptr<query> query_ptr;
}

#endif