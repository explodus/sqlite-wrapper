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
#include <sqlite_wrapper/cout.hpp>
#include <sqlite_wrapper/a2w.hpp>
#include <sqlite_wrapper/db.hpp>
#include <sqlite_wrapper/log.hpp>

#include <boost/make_shared.hpp>

void sub_sub_scope()
{
	db::log::singleton::basic::log_scope scope_(
		  db::log::singleton::basic::get_log()
		, DB_TEXT("sub_sub_scope()"));

	throw std::runtime_error("test");
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

	try
	{
		sub_sub_scope();
	}
	catch (std::exception &e)
	{
		db::log::singleton::basic::log_msg msg(
			db::log::singleton::basic::get_log()
			, db::string(DB_TEXT("exception: "))
#ifdef SQLITE_WRAPPER_NARROW_STRING
			+ e.what()
#else
			+ static_cast<const wchar_t*>(db::detail::a2w(e.what()))
#endif // SQLITE_WRAPPER_NARROW_STRING
			, db::log::log_error);
	}
}

int main( int /*argc*/, char ** /*argv*/ )
{
	try
	{
		// set to debug level
		db::log::global_level(db::log::log_debug);
		
		db::log::singleton::basic::log_scope scope_(
				db::log::singleton::basic::get_log()
			, DB_TEXT("main()"));

		sub_scope();
	}
	catch (std::exception &e)
	{
		db::cout << e.what() << std::endl;
	}
}

