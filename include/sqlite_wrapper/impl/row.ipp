///	@file		include/sqlite_wrapper/impl/row.ipp
///	@date		15.3.2010 14:23
///	file base	row
///	file ext	ipp
///
/// @author		T. Schroeder (explodus@gmx.de)
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
/// @brief 
///
/// This file is part of the sqlite-wrapper project

#ifndef SQLITE_WRAPPER_ROW_IPP_INCLUDED
#define SQLITE_WRAPPER_ROW_IPP_INCLUDED

inline db::row::row( db::query* query_, unsigned row_ ) 
	: _query(query_), _row(row_)
{

}

inline db::row::row( const row& r ) : _query(r._query), _row(r._row)
{
	_data.reserve(r.size());
	std::copy(r.begin(), r.end(), std::back_inserter(_data));
}

inline db::row::~row()
{

}

inline const db::param& db::row::value( size_type field_num ) const
{
	const value_type& pValue(*(begin()+field_num));
	return pValue;
}

inline db::param& db::row::value( size_type field_num )
{
	value_type& pValue(*(begin()+field_num));
	return pValue;
}

inline db::row::const_reference db::row::operator[]( size_type field_num ) const
{
	return *(begin()+field_num);
}

inline db::row::reference db::row::operator[]( size_type field_num )
{
	return *(begin()+field_num);
}

inline db::param* db::row::operator[]( const string& )
{
	return (0);
}

inline db::row::const_iterator db::row::begin() const
{
	return _data.begin();
}

inline db::row::iterator db::row::begin()
{
	return _data.begin();
}

inline db::row::const_iterator db::row::end() const
{
	return _data.end();
}

inline db::row::iterator db::row::end()
{
	return _data.end();
}

inline db::row::reverse_iterator db::row::rbegin()
{
	return _data.rbegin();
}

inline db::row::reverse_iterator db::row::rend()
{
	return _data.rend();
}

inline db::row::size_type db::row::size() const
{
	return _data.size();
}

inline bool db::row::empty() const
{
	return _data.empty();
}

inline bool db::row::operator==( const row& t ) const
{
	return _data == t._data;
}

inline void db::row::push_back( const value_type& v )
{
	_data.push_back(v);
}

inline void db::row::fill(sqlite3_stmt* stm)
{
	if (!stm)
		return;
	if (!_query)
		return;

	_data.reserve(sqlite3_column_count(stm));
	for (int i(0), sz(sqlite3_column_count(stm)); i < sz; ++i)
	{
		_data.push_back(param(i));
		_data.back().set(stm);
	} 
}

#endif