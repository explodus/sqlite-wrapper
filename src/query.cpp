///	@file		src/query.cpp
///	@date		15.3.2010 14:23
///	file base	log_level
///	file ext	cpp
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

#include <sqlite_wrapper/config.hpp>
#include <sqlite_wrapper/db.hpp>

db::query::query( base& base_ ) : _base(base_), _stm(0)
{

}

db::query::query( const query& q ) : _base(q._base), _stm(q._stm)
{
	_data.reserve(q._data.size());
	std::copy(q.begin(), q.end(), std::back_inserter(_data));

	_field.reserve(q._field.size());
	std::copy(q.fbegin(), q.fend(), std::back_inserter(_field));
}

void db::query::execute(const string& cmd)
{
	if (_stm) { sqlite3_finalize(_stm); _stm = 0; }
	sqlite3* _db(_base.get_db_ptr());

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
				_base.throw_error(rc);
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

const db::row& db::query::getRow( size_type row_num ) const
{
	const value_type& pValue(*(begin()+row_num));
	return pValue;
}

db::row& db::query::getRow( size_type row_num )
{
	value_type& pValue(*(begin()+row_num));
	return pValue;
}

db::query::const_reference db::query::operator[]( size_type row_num ) const
{
	return *(begin()+row_num);
}

db::query::reference db::query::operator[]( size_type row_num )
{
	return *(begin()+row_num);
}

int db::query::getIdOf( const string& sField ) const
{
	field_type::const_iterator iTB(_field.begin());
	field_type::const_iterator iTE(_field.end());
	for (iTB;iTB!=iTE;++iTB)
		if (iTB->name()==sField)
			return (int)(iTB-_field.begin());
	return -1;
}

db::string db::query::getTitleOf( unsigned iField ) const
{
	if (iField<_field.size())
		return _field[iField].name();
	else
		return DB_TEXT("");
}

const db::field* db::query::getFieldInfo( unsigned iField ) const
{
	if (iField<_field.size())
		return &_field[iField];
	else
		return 0;
}

db::query::const_iterator db::query::begin() const
{
	return _data.begin();
}

db::query::iterator db::query::begin()
{
	return _data.begin();
}
db::query::const_iterator db::query::end() const
{
	return _data.end();
}

db::query::iterator db::query::end()
{
	return _data.end();
}

db::query::const_fiterator db::query::fbegin() const
{
	return _field.begin();
}

db::query::fiterator db::query::fbegin()
{
	return _field.begin();
}
db::query::const_fiterator db::query::fend() const
{
	return _field.end();
}

db::query::fiterator db::query::fend()
{
	return _field.end();
}

db::query::size_type db::query::size() const
{
	return _data.size();
}

bool db::query::empty() const
{
	return _data.empty();
}

bool db::query::f_empty() const
{
	return _field.empty();
}

bool db::query::operator==( const query& t ) const
{
	return  _data == t._data;
}

void db::query::push_back( const value_type& v )
{
	_data.push_back(v);
}

void db::query::push_back( const fvalue_type& v )
{
	_field.push_back(v);
}
