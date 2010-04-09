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
#include <sqlite_wrapper/cout.hpp>
#include <sqlite_wrapper/db.hpp>


db::exception::base::base( const string& msg ) : _msg(msg)
{

}

db::exception::base::base( const base& _That ) : _msg(_That._msg)
{

}

db::exception::base::~base()
{

}

const db::char_type* db::exception::base::what()
{
	return _msg.c_str();
}

void db::exception::base::report()
{
	db::cout << _msg << DB_TEXT("\n");
}

db::exception::not_found::not_found( const string& s/*=DB_TEXT("")*/ ) : 
	base(DB_TEXT("NotFound: ")+s)
{

}

db::exception::db_error::db_error( const string& m/*=DB_TEXT("")*/ ) : 
	base(DB_TEXT("Database Error: ")+m)
{

}

db::exception::sql_error::sql_error( const string& m/*=DB_TEXT("")*/ ) : 
	base(DB_TEXT("SQL Error: ")+m)
{

}

db::exception::param_error::param_error( const string& m/*=DB_TEXT("")*/ ) : 
	base(DB_TEXT("Param Error: ")+m)
{

}

db::exception::internal_error::internal_error( const string& m/*=DB_TEXT("")*/ ) : 
	base(DB_TEXT("Internal Error: ")+m)
{

}

db::exception::memory_error::memory_error( const string& m/*=DB_TEXT("")*/ ) : 
	base(DB_TEXT("Allocation failed: ")+m)
{

}

db::exception::insertion_error::insertion_error( const string& m/*=DB_TEXT("")*/ ) : 
	base(DB_TEXT("Database full: ")+m)
{

}

db::exception::unknown_error::unknown_error( const string& m/*=DB_TEXT("")*/ ) : 
	base(DB_TEXT("Unknown Error: ")+m)
{

}