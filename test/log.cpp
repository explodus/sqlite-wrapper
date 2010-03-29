///	@file test/log.cpp
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
#include <sqlite_wrapper/log.hpp>

#include <boost/make_shared.hpp>

using namespace db::log::singleton::basic;

void sub_sub_scope()
{
	log_scope scope_(get_log(), DB_TEXT("sub_sub_scope()"));

}

void sub_scope()
{
	log_scope scope_(get_log(), DB_TEXT("sub_scope()"));

	sub_sub_scope();
}

int main( int argc, char **argv )
{
	// set to debug level
	db::log::global_level = db::log::log_debug;
	
	log_scope scope_(get_log(), DB_TEXT("main()"));

	sub_scope();
}

