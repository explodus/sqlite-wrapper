///	@file test/log.cpp
///  
///	@date 16.3.2010 19:51
///  
/// @author T. Schroeder (explodus@gmx.de)
///  
/// @brief holds the testcase to an example of basic log
///  
/// This file is part of the sqlite-wrapper project
///

#include <sqlite_wrapper/config.hpp>
#include <sqlite_wrapper/log.hpp>

#include <boost/make_shared.hpp>

void sub_sub_scope()
{
	db::log::singleton::basic::log_scope scope_(
		  db::log::singleton::basic::get_log()
		, DB_TEXT("sub_sub_scope()"));

	throw std::exception("test");
}

void sub_scope()
{
	db::log::singleton::basic::log_scope scope_(
		  db::log::singleton::basic::get_log()
		, DB_TEXT("sub_scope()"));

	db::log::singleton::basic::log_msg msg(
		  db::log::singleton::basic::get_log()
		, DB_TEXT("some usefull information")
		, db::log::log_info);

	sub_sub_scope();
}

int main( int argc, char **argv )
{
	// set to debug level
	db::log::global_level = db::log::log_debug;
	
	db::log::singleton::basic::log_scope scope_(
		  db::log::singleton::basic::get_log()
		, DB_TEXT("main()"));

	try
	{
		sub_scope();
	}
	catch (std::exception &e)
	{
		db::log::singleton::basic::log_msg msg(
			  db::log::singleton::basic::get_log()
			, db::string(DB_TEXT("exception: "))
			+ static_cast<const wchar_t*>(db::detail::a2w(e.what()))
			, db::log::log_error);
	}
}

