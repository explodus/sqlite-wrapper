///	@file		include/sqlite_wrapper/impl/query.cpp
///	@date		15.3.2010 14:23
///	file base	query
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

#ifndef SQLITE_WRAPPER_QUERY_IPP_INCLUDED
#define SQLITE_WRAPPER_QUERY_IPP_INCLUDED

inline db::query::query() : _base(0), _stm(0)
{

}

inline db::query::query( base& base_ ) : _base(&base_), _stm(0)
{

}

inline db::query::query( const query& q ) : _base(q._base), _stm(q._stm)
{
	_data.reserve(q._data.size());
	std::copy(q.begin(), q.end(), std::back_inserter(_data));

	_field.reserve(q._field.size());
	std::copy(q.fbegin(), q.fend(), std::back_inserter(_field));
}

inline db::query::~query()
{

}

inline void db::query::execute(const string& cmd)
{
	if (_stm) { sqlite3_finalize(_stm); _stm = 0; }
	sqlite3* _db(_base->get_db_ptr());

	_data.clear();

	int rc;
	while (1) 
	{
#ifdef _UNICODE
		rc = sqlite3_prepare16_v2(_db, 
			reinterpret_cast<const wchar_t*>(cmd.c_str()), 
			cmd.length()*sizeof(wchar_t), 
			&_stm, 0);
#else
		rc = sqlite3_prepare_v2(_db, 
			reinterpret_cast<const char*>(cmd.c_str()), 
			cmd.length()*sizeof(char), 
			&_stm, 0);
#endif // _UNICODE

		if (rc != SQLITE_OK || _stm == 0) 
		{
			switch(rc) 
			{
			case SQLITE_BUSY: 
			case SQLITE_LOCKED: 
				/// @todo portabel machen
#if defined (WIN32) || defined (WIN64)
				Sleep(250); 
#else
				sleep(250); 
#endif
				break;
			default: 
				_base->throw_error(rc);
			}
		}
		else
			break;
	} 

	if (rc != SQLITE_OK || !_stm)
		return;

	unsigned nRowCnt(0);
	while ((rc = sqlite3_step(_stm)) == SQLITE_ROW) 
	{
		if (nRowCnt==0)
		{
			_field.reserve(sqlite3_column_count(_stm)); string tmp;
			for (int i(0), sz(sqlite3_column_count(_stm)); i < sz; ++i)
			{
#ifdef _UNICODE
				tmp = static_cast<const wchar_t*>(
					sqlite3_column_origin_name16(_stm, i));
#else
				tmp = sqlite3_column_origin_name(_stm, i);
#endif // _UNICODE

				_field.push_back(db::field(tmp));

				switch (sqlite3_column_type(_stm, i))
				{
				case SQLITE_NULL:
					_field.back().set_type(db::e_null);
					break;
				case SQLITE_INTEGER:
					_field.back().set_type(db::e_long);
					break;
				case SQLITE_FLOAT:
					_field.back().set_type(db::e_double);
					break;
				case SQLITE_TEXT:
					_field.back().set_type(db::e_string);
					break;
				case SQLITE_BLOB:
					_field.back().set_type(db::e_blob);
					break;
				default:
					_field.back().set_type(db::e_null);
					break;
				}
			}
		}

		_data.push_back(row(this, nRowCnt++));
		_data.back().fill(_stm);
	}
	sqlite3_finalize(_stm);
	_stm = 0; 
}

inline const db::row& db::query::getRow( size_type row_num ) const
{
	const value_type& pValue(*(begin()+row_num));
	return pValue;
}

inline db::row& db::query::getRow( size_type row_num )
{
	value_type& pValue(*(begin()+row_num));
	return pValue;
}

inline db::query::const_reference db::query::operator[]( size_type row_num ) const
{
	return *(begin()+row_num);
}

inline db::query::reference db::query::operator[]( size_type row_num )
{
	return *(begin()+row_num);
}

inline int db::query::getIdOf( const string& sField ) const
{
	field_type::const_iterator iTB(_field.begin());
	field_type::const_iterator iTE(_field.end());
	for (iTB;iTB!=iTE;++iTB)
		if (iTB->name()==sField)
			return (int)(iTB-_field.begin());
	return -1;
}

inline db::string db::query::getTitleOf( unsigned iField ) const
{
	if (iField<_field.size())
		return _field[iField].name();
	else
		return DB_TEXT("");
}

inline const db::field* db::query::getFieldInfo( unsigned iField ) const
{
	if (iField<_field.size())
		return &_field[iField];
	else
		return 0;
}

inline db::query::const_iterator db::query::begin() const
{
	return _data.begin();
}

inline db::query::iterator db::query::begin()
{
	return _data.begin();
}

inline db::query::const_iterator db::query::end() const
{
	return _data.end();
}

inline db::query::iterator db::query::end()
{
	return _data.end();
}

inline db::query::const_reverse_iterator db::query::rbegin() const
{
	return _data.rbegin();
}

inline db::query::reverse_iterator db::query::rbegin()
{
	return _data.rbegin();
}

inline db::query::const_reverse_iterator db::query::rend() const
{
	return _data.rend();
}

inline db::query::reverse_iterator db::query::rend()
{
	return _data.rend();
}

inline db::query::const_fiterator db::query::fbegin() const
{
	return _field.begin();
}

inline db::query::fiterator db::query::fbegin()
{
	return _field.begin();
}

inline db::query::const_fiterator db::query::fend() const
{
	return _field.end();
}

inline db::query::fiterator db::query::fend()
{
	return _field.end();
}

inline db::query::size_type db::query::size() const
{
	return _data.size();
}

inline bool db::query::empty() const
{
	return _data.empty();
}

inline bool db::query::f_empty() const
{
	return _field.empty();
}

inline bool db::query::operator==( const query& t ) const
{
	return  _data == t._data;
}

inline void db::query::push_back( const value_type& v )
{
	_data.push_back(v);
}

inline void db::query::push_back( const fvalue_type& v )
{
	_field.push_back(v);
}

inline db::query::iterator db::query::erase(const_iterator _Where)
{
	return _data.erase(_Where);
}

inline db::query::iterator db::query::erase(
	  const_iterator _First_arg
	, const_iterator _Last_arg)
{
	return _data.erase(_First_arg, _Last_arg);
}

#endif