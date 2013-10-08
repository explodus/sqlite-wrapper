///	@file		include/sqlite_wrapper/impl/expr.ipp
///	@date		15.3.2010 14:23
///	file base	expr
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

#ifndef SQLITE_WRAPPER_EXPR_IPP_INCLUDED
#define SQLITE_WRAPPER_EXPR_IPP_INCLUDED

SQLITE_WRAPPER_INLINE db::string db::expr::base::str() const
{
	return DB_TEXT("True");
}

SQLITE_WRAPPER_INLINE db::expr::base::operator db::string() const
{
	return str();
}

SQLITE_WRAPPER_INLINE db::split db::expr::base::getExtraTables() const
{
	return extraTables;
}

SQLITE_WRAPPER_INLINE db::expr::base::~base()
{

}

SQLITE_WRAPPER_INLINE db::expr::raw::raw( const string& e ) : expr(e)
{

}

SQLITE_WRAPPER_INLINE db::string db::expr::raw::str() const
{
	return expr;
}

SQLITE_WRAPPER_INLINE db::expr::connective::connective( string o, const base & e1_, const base & e2_ ) : op(o), e1(e1_), e2(e2_)
{

}

SQLITE_WRAPPER_INLINE db::string db::expr::connective::str() const
{
	string res = DB_TEXT("(") + e1.str() + DB_TEXT(") ") + op 
		+ DB_TEXT(" (") + e2.str() + DB_TEXT(")");
	return res;
}

SQLITE_WRAPPER_INLINE db::expr::and_::and_( const base & e1_, const base & e2_ ) : 
connective(DB_TEXT("and"), e1_, e2_)
{

}

SQLITE_WRAPPER_INLINE db::string db::expr::and_::str() const
{
	if (e1.str() == DB_TEXT("True"))
		return e2.str();
	else if (e2.str() == DB_TEXT("True"))
		return e1.str();
	else
		return connective::str();
}

SQLITE_WRAPPER_INLINE db::expr::or_::or_( const base & e1_, const base & e2_ ) : connective(DB_TEXT("or"), e1_, e2_)
{

}

SQLITE_WRAPPER_INLINE db::string db::expr::or_::str() const
{
	if (e1.str() == DB_TEXT("True"))
		return DB_TEXT("True");
	else if (e2.str() == DB_TEXT("True"))
		return DB_TEXT("True");
	else
		return connective::str();
}

SQLITE_WRAPPER_INLINE db::expr::not_::not_( const base & _exp ) : exp(_exp)
{

}

SQLITE_WRAPPER_INLINE db::string db::expr::not_::str() const
{
	return DB_TEXT("not (")+exp.str()+DB_TEXT(")");
}

SQLITE_WRAPPER_INLINE bool db::expr::oper::check_escape( const param_types& type_ )
{
	switch(type_) 
	{
	case e_string:
	case e_char:
	case e_blob:
	case e_date_time:
		return true;
		break;
	default:
		return false;
	}
}

SQLITE_WRAPPER_INLINE db::expr::oper::oper( const field & fld, const string& o, const string& d ) : _field(fld), op(o), data(d), escape(check_escape(_field.type()))
{
	extraTables.push_back(fld.table());
}

SQLITE_WRAPPER_INLINE db::expr::oper::oper( const field & fld, const string& o, const field &f2 ) : _field(fld), op(o), data(f2.fullName()), escape(false)
{
	extraTables.push_back(fld.table());
}

SQLITE_WRAPPER_INLINE db::string db::expr::oper::str() const
{
	string res;
	res += _field.fullName() + DB_TEXT(" ") + op + DB_TEXT(" ") + 
		(escape ? detail::escape_sql(data) : data);
	return res;
}

SQLITE_WRAPPER_INLINE db::expr::like::like( const field & fld, const string& d ) : oper(fld, DB_TEXT("LIKE"), d)
{

}

SQLITE_WRAPPER_INLINE db::expr::in::in( const field & fld, const string& set ) : oper(fld, DB_TEXT("IN"), DB_TEXT("(")+set+DB_TEXT(")"))
{

}

/// @brief     overload from the global ! operator
///
/// <BR>qualifier
/// <BR>access    public  
/// @return    db::expr::not
/// @param     exp as const db::expr::base & 
///
/// @date      20:2:2009   14:23
///
SQLITE_WRAPPER_INLINE db::expr::not_ operator!(const db::expr::base &exp)
{ 
	return db::expr::not_(exp); 
}

/// @brief     overload from the global << operator
///
/// <BR>qualifier
/// <BR>access    public  
/// @return    db::expr::in
/// @param     fld as const db::field &
/// @param     f2 as const db::char_type *
///
/// @date      20:2:2009   14:23
///
SQLITE_WRAPPER_INLINE db::expr::in operator<<(
	  const db::field& fld
	, const db::char_type* f2)
{ 
	return db::expr::in(fld,f2); 
}

/// @brief     overload from the global << operator
///
/// <BR>qualifier
/// <BR>access    public  
/// @return    db::expr::in
/// @param     fld as const db::field &
/// @param     f2 as const db::string &
///
/// @date      20:2:2009   14:23
///
SQLITE_WRAPPER_INLINE db::expr::in operator<<(const db::field& fld, const db::string& f2)
{ 
	return db::expr::in(fld,f2); 
}

SQLITE_WRAPPER_INLINE db::expr::in::in(const db::field & fld, const db::sel& s) 
	: oper(fld, DB_TEXT("in"), DB_TEXT("(") + s.str() + DB_TEXT(")")) 
{
}

SQLITE_WRAPPER_INLINE db::string db::expr::in::str() const
{
	return _field.fullName() + DB_TEXT(" ") + op + DB_TEXT(" ") + data;
}

///overload from the global && operator
SQLITE_WRAPPER_INLINE db::expr::and_ operator&&(
													const db::expr::base& o1
													, const db::expr::base& o2)
{ 
	return db::expr::and_(o1,o2); 
}

///overload from the global || operator
SQLITE_WRAPPER_INLINE db::expr::or_ operator||(const db::expr::base& o1, 
												 const db::expr::base& o2)
{ 
	return db::expr::or_(o1,o2); 
}

#endif