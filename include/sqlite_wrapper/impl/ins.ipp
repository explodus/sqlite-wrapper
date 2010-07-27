///	@file		include/sqlite_wrapper/impl/ins.ipp
///	@date		15.3.2010 14:23
///	file base	ins
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

#ifndef SQLITE_WRAPPER_INS_IPP_INCLUDED
#define SQLITE_WRAPPER_INS_IPP_INCLUDED

inline db::ins::ins( const string& tab ) : _source(tab)
{

}

inline db::ins::ins( 
	  const string& tab
	, const string& delim ) : 
	  _source(tab)
	, _delim1(delim)
	, _delim2(delim)
{

}

inline db::ins::ins( 
	  const string& tab
	, const string& delim1
	, const string& delim2 ) : 
	  _source(tab)
	, _delim1(delim1)
	, _delim2(delim2)
{

}

inline db::ins::~ins()
{

}

inline db::ins & db::ins::clear_values()
{
	_values.clear();
	return *this;
}

inline db::ins & db::ins::operator%( const db::field& f )
{
	if (f.values().size()==1)
		return values(f.name(), f.values().begin()->second, f.type());
	else
		return *this;
}

inline db::ins & db::ins::values( string t, param* v )
{
	string sV(v->str());
	if (sV.length()==0)
		sV = DB_TEXT("NULL");
	else
		switch(v->get_type()) 
	{
		case e_string:
		case e_date_time:
			sV = DB_TEXT("'") + sV;
			sV += DB_TEXT("'");
			break;
	}
	detail::front_back_delim(t, _delim1, _delim2);
	_values.insert(std::pair<string, string>(t, sV));
	return *this;
}

inline db::ins & db::ins::values( string t, string v, param_types p/*=e_int*/ )
{
	switch(p) 
	{
	case e_char:
	case e_blob:
	case e_string:
	case e_date_time:
		detail::erase_all<string>(v, DB_TEXT("'"));
		detail::erase_all<string>(v, DB_TEXT("`"));
		detail::erase_all<string>(v, DB_TEXT("´"));
		v = DB_TEXT("'") + v;
		v += DB_TEXT("'");
		break;
	}
	detail::front_back_delim(t, _delim1, _delim2);
	_values.insert(std::pair<string, string>(t, v));
	return *this;
}

inline db::ins::operator db::string() const
{
	return str();
}

inline db::string db::ins::str() const
{
	string res = DB_TEXT("INSERT INTO ");
	res += _source;

	if (_values.size() > 0)
	{
		res += DB_TEXT(" (");
		res += _values.join_fields(DB_TEXT(","));
		res += DB_TEXT(") VALUES (");
		res += _values.join_values(DB_TEXT(","));
		res += DB_TEXT(")");
	}
	return res;
}

#endif