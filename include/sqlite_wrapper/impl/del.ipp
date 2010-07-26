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

#ifndef SQLITE_WRAPPER_DEL_IPP_INCLUDED
#define SQLITE_WRAPPER_DEL_IPP_INCLUDED

inline db::del::del( const string& tablename ) : sel(tablename)
{

}

inline db::del::operator db::string() const
{
	string res = DB_TEXT("DELETE FROM ");
	res += _sources.join(DB_TEXT(","));
	if (_where != DB_TEXT("True"))
		res += DB_TEXT(" WHERE ") + _where;
	if (_groupBy.size() > 0)
		res += DB_TEXT(" GROUP BY ") + _groupBy.join(DB_TEXT(","));
	if (_having.length())
		res += DB_TEXT(" HAVING ") + _having;
	if (_orderBy.size() > 0)
		res += DB_TEXT(" ORDER BY ") + _orderBy.join(DB_TEXT(","));
	if (_limit) 
		res += DB_TEXT(" LIMIT ") + string(detail::to_string(_limit));
	if (_offset) 
		res += DB_TEXT(" OFFSET ") + string(detail::to_string(_offset));

	detail::replace_all(res, _delim1+_delim1, _delim1);
	if (_delim1!=_delim2)
		detail::replace_all(res, _delim2+_delim2, _delim2);
	return res;
}

inline db::del::~del()
{

}

#endif