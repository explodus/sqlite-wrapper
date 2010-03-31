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
    typedef field_type::iterator fiterator;
    typedef field_type::const_iterator const_fiterator;
		
		query() : _base(0), _stm(0) {}

    query(base& base_);

    query(const query& q);

    ~query();

    void execute(const string& cmd);

    ///get a row by number
    const row& getRow(size_type row_num) const;
    row& getRow(size_type row_num);

    ///get a row by number
    const_reference operator[] (size_type row_num) const;
    reference operator[] (size_type row_num);

    ///get id from field title
    int getIdOf(const string& sField) const;
    string getTitleOf(unsigned iField) const;
    const field* getFieldInfo(unsigned iField) const;

    ///begin iterator
    const_iterator begin() const;
    ///end iterator
    const_iterator end() const;
    ///begin iterator
    iterator begin();
    ///end iterator
    iterator end();
    ///begin iterator
    const_fiterator fbegin() const;
    ///end iterator
    const_fiterator fend() const;
    ///begin iterator
    fiterator fbegin();
    ///end iterator
    fiterator fend();
    ///container size
    size_type size() const;
    ///is container empty?
    bool empty() const;
    bool f_empty() const;

    bool operator==(const query& t) const;

    ///push a row to the row container
    void push_back(const value_type& v);
    ///push a field title to the field container
    void push_back(const fvalue_type& v);
  };

	typedef boost::shared_ptr<query> query_ptr;
}

#endif