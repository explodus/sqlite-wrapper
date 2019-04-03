///	@file test/db_log.cpp
///  
///	@date 16.3.2010 19:51
///  
/// @author T. Schroeder (explodus@gmx.de)
///  
/// @brief description what this file is for  
///	@brief interface of the db class 
///  
/// This file is part of the sqlite-wrapper project
///

#include <sqlite_wrapper/config.hpp>
#include <sqlite_wrapper/cout.hpp>
#include <sqlite_wrapper/a2w.hpp>
#include <sqlite_wrapper/db.hpp>
#include <sqlite_wrapper/log.hpp>

#include <boost/make_shared.hpp>

using db::log::singleton::db::log_scope;
using db::log::singleton::db::log_msg;
using db::log::singleton::db::log_type;

log_type* theLog(0);

void sub_sub_scope()
{
	log_scope scope_(*theLog, DB_TEXT("sub_sub_scope()"));

	throw std::runtime_error("test");
}

void sub_scope()
{
	log_scope scope_(*theLog, DB_TEXT("sub_scope()"));

	log_msg msg(
		  *theLog
		, DB_TEXT("some usefull information")
		, db::log::log_info);

	try
	{
		sub_sub_scope();
	}
	catch (std::exception &e)
	{
		log_msg msg(
			  *theLog
			, db::string(DB_TEXT("exception: "))
#ifdef SQLITE_WRAPPER_NARROW_STRING
			+ e.what()
#else
			+ static_cast<const wchar_t*>(db::detail::a2w(e.what()))
#endif
			, db::log::log_error);
	}
}

int main( int /*argc*/, char **argv )
{
#ifdef SQLITE_WRAPPER_NARROW_STRING
	db::string db_name = argv[0];
#else
	db::string db_name = db::detail::a2w(argv[0]);
#endif
	db_name = db_name.substr(0, db_name.rfind(DB_TEXT("\\")));
	db_name += DB_TEXT("\\log.db");

	theLog = &db::log::singleton::db::get_log(db_name);
	
	try
	{
		// set to debug level
		db::log::global_level(db::log::log_debug);

		log_scope scope_(
			  *theLog
			, DB_TEXT("main()"));

		sub_scope();
	}
	catch (std::exception &e)
	{
		db::cout << e.what() << std::endl;
	}
}

