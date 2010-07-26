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

#ifndef SQLITE_WRAPPER_SEL_IPP_INCLUDED
#define SQLITE_WRAPPER_SEL_IPP_INCLUDED

inline db::sel::sel() 
	: _distinct(false)
	, _limit(0)
	, _offset(0)
	, _where(DB_TEXT("True"))
	, _delim1(DB_TEXT(""))
	, _delim2(DB_TEXT(""))
{

}

inline db::sel::sel( const string& tablename ) 
	: _distinct(false)
	, _limit(0)
	, _offset(0)
	, _where(DB_TEXT("True"))
	, _delim1(DB_TEXT(""))
	, _delim2(DB_TEXT(""))
{
	source(tablename);
}

inline db::sel::sel( const string& tablename, const string& delim ) 
	: _distinct(false)
	, _limit(0)
	, _offset(0)
	, _where(DB_TEXT("True"))
	, _delim1(delim)
	, _delim2(delim)
{
	source(tablename);
}

inline db::sel::sel( 
	  const string& tablename
	, const string& delim1
	, const string& delim2 ) 
	: _distinct(false)
	, _limit(0)
	, _offset(0)
	, _where(DB_TEXT("True"))
	, _delim1(delim1)
	, _delim2(delim2)
{
	source(tablename);
}

inline db::sel::sel( const sel& s ) 
	: _distinct(s._distinct)
	, _limit(s._limit)
	, _offset(s._offset)
	, _where(s._where)
	, _having(s._having)
	, _delim1(s._delim1)
	, _delim2(s._delim2)
	, _results(s._results)
	, _sources(s._sources)
	, _groupBy(s._groupBy)
	, _orderBy(s._orderBy)
	, _join(s._join)
{

}

inline db::sel::~sel()
{

}

inline db::sel & db::sel::operator=( const sel& s )
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

inline db::sel & db::sel::operator,( const db::string& s )
{
	return this->result(s);
}

inline db::sel & db::sel::operator%( const expr::base& e )
{
	return this->where(e.str());
}

inline db::sel & db::sel::operator<( const field_pair& f )
{
	return this->join(f);
}

inline db::sel & db::sel::distinct( bool d )
{
	_distinct = d; 
	return *this;
}

inline db::sel & db::sel::limit( int value )
{
	_limit = value;	
	return *this;
}

inline db::sel & db::sel::join( const field_pair& f )
{
	_join = f;	
	return *this;
}

inline db::sel & db::sel::offset( int value )
{
	_offset = value; 
	return *this;
}

inline db::sel & db::sel::result_no_delim( string r, string alias/*=DB_TEXT("")*/ )
{
	if (alias.length())
		r += DB_TEXT(" AS ") + alias;
	_results.push_back(r); 
	return *this;
}

inline db::sel & db::sel::result( string r, string alias/*=DB_TEXT("")*/ )
{
	if (alias.length())
		r += DB_TEXT(" AS ") + alias;
	if (r!=DB_TEXT("*")) 
		detail::front_back_delim(r, _delim1, _delim2);
	_results.push_back(r); 
	return *this;
}

inline db::sel & db::sel::result_max( string r, string alias/*=DB_TEXT("")*/ )
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

inline db::sel & db::sel::clear()
{
	_results.clear();
	_sources.clear();
	return *this;
}

inline db::sel & db::sel::source( string s, string alias/*=DB_TEXT("")*/ )
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

inline db::sel & db::sel::where( const expr::base & w )
{
	_where = expr::and_(expr::raw(_where), w).str();	
	return *this;
}

inline db::sel & db::sel::where( string w )
{
	_where = expr::and_(expr::raw(_where), expr::raw(w)).str();
	return *this;
}

inline db::sel & db::sel::group_by( string gb )
{
	detail::front_back_delim(gb, _delim1, _delim2);
	_groupBy.push_back(gb);	
	return *this;
}

inline db::sel & db::sel::having( const expr::base & h )
{
	_having = h.str(); 
	return *this;
}

inline db::sel & db::sel::having( string h )
{
	_having = h;
	return *this;
}

inline db::sel & db::sel::order_by( string ob, bool ascending/*=true*/ )
{
	string value = ob;
	detail::front_back_delim(value, _delim1, _delim2);
	if (!ascending)
		value += DB_TEXT(" DESC");
	_orderBy.push_back(value); 
	return *this;
}

inline db::sel::operator db::string() const
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

inline db::string db::sel::str() const
{
	return this->operator db::string();
}

#endif