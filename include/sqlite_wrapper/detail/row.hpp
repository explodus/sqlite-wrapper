///	@file		include/sqlite_wrapper/detail/row.hpp
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

#ifndef DB_ROW_HPP_INCLUDED
#define DB_ROW_HPP_INCLUDED

#include <sqlite_wrapper/config.hpp>
#include <sqlite_wrapper/detail/param.hpp>

namespace db
{
	class query;

  ///table rows interface
  class SQLITE_WRAPPER_DLLAPI row
  {
  protected:
    typedef std::vector<param, alloc_param > data_type;
    data_type _data;
    unsigned _row;
    query* _query;

  public:
    typedef data_type::value_type value_type;
    typedef data_type::reference reference;
    typedef data_type::const_reference const_reference;
    typedef data_type::size_type size_type;
    typedef data_type::difference_type difference_type;
    typedef data_type::iterator iterator;
    typedef data_type::reverse_iterator reverse_iterator;
    typedef data_type::const_iterator const_iterator;

		/// @todo correct this...
		row() {}

    /// @brief     row
    ///
    /// <BR>qualifier : _query(query_), _row(row_)
    /// <BR>access    public  
    /// @return    
    /// @param     query_ as query *
    /// @param     row_ as unsigned
    ///
    /// @date      20:2:2009   9:24
    ///
    row(query* query_, unsigned row_);

    /// @brief     row
    ///
    /// <BR>qualifier : _query(r._query), _row(r._row)
    /// <BR>access    public  
    /// @return    
    /// @param     r as const row &
    ///
    /// @date      20:2:2009   9:23
    ///
    row(const row& r);

    /// @brief     ~row
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    
    ///
    /// @date      20:2:2009   9:23
    ///
    ~row();

    /// @brief     get a value by number
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    const param&
    /// @param     field_num as size_type 
    ///
    /// @date      20:2:2009   9:23
    ///
    const param& value(size_type field_num) const;

    /// @brief     get a value by number
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    param&
    /// @param     field_num as size_type
    ///
    /// @date      20:2:2009   9:23
    ///
    param& value(size_type field_num);

    /// @brief     get a value by number
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    db::row::const_reference
    /// @param     field_num as size_type
    ///
    /// @date      20:2:2009   9:22
    ///
    const_reference operator[] (size_type field_num) const;

    /// @brief     get a value by number
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    db::row::reference
    /// @param     field_num as size_type
    ///
    /// @date      20:2:2009   9:21
    ///
    reference operator[] (size_type field_num);

    /// @brief     operator[]
		/// @todo			 need an implementation
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    param*
    /// @param     field_title as const string &
    ///
    /// @date      20:2:2009   9:21
    ///
    param* operator[] (const string& field_title);

    /// @brief     const begin iterator
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    db::row::const_iterator
    ///
    /// @date      20:2:2009   9:21
    ///
    const_iterator begin() const;

    /// @brief     const end iterator
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    db::row::const_iterator
    ///
    /// @date      20:2:2009   9:21
    ///
    const_iterator end() const;

    /// @brief     begin iterator
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    db::row::iterator
    ///
    /// @date      20:2:2009   9:21
    ///
    iterator begin();

    /// @brief     end iterator
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    db::row::iterator
    ///
    /// @date      20:2:2009   9:20
    ///
    iterator end();

    /// @brief     begin reverse iterator
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    db::row::reverse_iterator
    ///
    /// @date      20:2:2009   9:20
    ///
    reverse_iterator rbegin();

    /// @brief     end reverse iterator
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    db::row::reverse_iterator
    ///
    /// @date      20:2:2009   9:20
    ///
    reverse_iterator rend();

    /// @brief     container size
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    db::row::size_type
    ///
    /// @date      20:2:2009   9:20
    ///
    size_type size() const;

    /// @brief     returns true if the container is empty
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    bool
    ///
    /// @date      20:2:2009   9:19
    ///
    bool empty() const;

    /// @brief     operator==
    ///
    /// <BR>qualifier const 
    /// <BR>access    public  
    /// @return    bool
    /// @param     t as const row &
    ///
    /// @date      20:2:2009   9:19
    ///
    bool operator==(const row& t) const;

    /// @brief     push a param to the container
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    void
    /// @param     v as const value_type &
    ///
    /// @date      20:2:2009   9:18
    ///
    void push_back(const value_type& v);

    /// @brief     fill
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    void
    /// @param     stm as sqlite3_stmt *
    ///
    /// @date      20:2:2009   9:18
    ///
    void fill(sqlite3_stmt* stm);

  };
  
	//typedef boost::detail::allocator::partial_std_allocator_wrapper<
 //   boost::detail::quick_allocator<row > > alloc_row; 
	typedef std::allocator<row> alloc_row;
}

#endif