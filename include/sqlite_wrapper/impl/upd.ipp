///	@file		include/sqlite_wrapper/impl/upd.ipp
///	@date		15.3.2010 14:23
///	file base	upd
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

#ifndef SQLITE_WRAPPER_UPD_IPP_INCLUDED
#define SQLITE_WRAPPER_UPD_IPP_INCLUDED

SQLITE_WRAPPER_INLINE db::upd::~upd()
{

}

SQLITE_WRAPPER_INLINE db::upd::upd( const string& t ) : 
	  table(t)
	, _where(DB_TEXT("True"))
{

}

SQLITE_WRAPPER_INLINE db::upd::upd( 
	  const string& t
	, const string& d ) : 
	  table(t)
	, _where(DB_TEXT("True"))
	, _delim1(d)
	, _delim2(d)
{

}

SQLITE_WRAPPER_INLINE db::upd::upd( 
	  const string& t
	, const string& d1
	, const string& d2 ) : 
	  table(t)
	, _where(DB_TEXT("True"))
	, _delim1(d1)
	, _delim2(d2)
{

}

SQLITE_WRAPPER_INLINE db::upd& db::upd::where( const expr::base& e )
{
	_where = expr::and_(expr::raw(_where), e).str();
	return *this;
}

SQLITE_WRAPPER_INLINE db::upd & db::upd::operator%( const expr::base& e )
{
	return where(e);
}

SQLITE_WRAPPER_INLINE db::upd & db::upd::operator%( const db::field& f )
{
	if (f.values().size()==1)
		return set(f, f.values().begin()->second);
	else
		return *this;
}

SQLITE_WRAPPER_INLINE db::upd& db::upd::set( const field& f, const string& val )
{
	fields.push_back(f.name());
	if (f.type()==e_string||f.type()==e_char||
		f.type()==e_blob||f.type()==e_date_time)
		values.push_back(detail::escape_sql(val));
	else
		values.push_back(val);
	return *this;
}

SQLITE_WRAPPER_INLINE db::upd::operator db::string() const
{
	return str();
}

SQLITE_WRAPPER_INLINE db::string db::upd::str() const
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

#endif