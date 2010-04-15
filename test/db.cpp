///	@file test/db.cpp
///  
///	@date 16.3.2010 19:51
///  
/// @author T. Schroeder (explodus@gmx.de)
///  
/// @brief  database test case
///  
/// This file is part of the sqlite-wrapper project
///

#include <sqlite_wrapper/config.hpp>
#include <sqlite_wrapper/a2w.hpp>
#include <sqlite_wrapper/db.hpp>
#include <sqlite_wrapper/detail/expr.hpp>

#define BOOST_TEST_NO_MAIN
#include <boost/test/included/unit_test.hpp>

using namespace boost::unit_test;

void generate_update_expression()
{
	using db::string;
	using db::upd;
	using db::field;

	string sql(DB_TEXT("UPDATE gps SET longitude=11.12345678,latitude=53.12345678 WHERE id = 1234"));

	upd u((
		upd(DB_TEXT("gps")) 
		% field(DB_TEXT("longitude"), 11.12345678)
		% field(DB_TEXT("latitude"), 53.12345678) 
		% (field(DB_TEXT("id"), 1) == 1234)));

	BOOST_CHECK(sql == string(u));
}

test_suite* init_unit_test_suite( int /*argc*/, char* argv[] ) 
{
	argv = argv; // only for a warning

	framework::master_test_suite().
		add( BOOST_TEST_CASE( &generate_update_expression )/*, 30*/ );

	return 0;
}

int main( int argc, char ** argv )
{
	using db::detail::a2w;
	using db::string;

	string db_name = a2w(argv[0]);
	db_name = db_name.substr(0, db_name.rfind(DB_TEXT("\\")));
	db_name += DB_TEXT("\\test.db");

	int ret(unit_test_main( &init_unit_test_suite, argc, argv )); 

	return ret;
}

