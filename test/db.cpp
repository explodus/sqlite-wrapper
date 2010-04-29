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

#ifndef SQLITE_WRAPPER_NARROW_STRING
	#include <sqlite_wrapper/a2w.hpp>
	#include <sqlite_wrapper/w2a.hpp>
#endif

#include <sqlite_wrapper/db.hpp>

#include <sqlite_wrapper/detail/field.hpp>

#include <sqlite_wrapper/detail/sel.hpp>
#include <sqlite_wrapper/detail/del.hpp>
#include <sqlite_wrapper/detail/ins.hpp>
#include <sqlite_wrapper/detail/upd.hpp>

#include <sqlite_wrapper/detail/expr.hpp>

#include <sqlite_wrapper/table.hpp>

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

	BOOST_CHECK_MESSAGE( sql == string(u)
		, "\n sql is: \"" 
		<< sql.c_str()
		<< "\",\n upd is: \"" 
		<< string(u).c_str()
		<< "\"" );
}

void generate_select_expression()
{
	using db::string;
	using db::sel;
	using db::field;

	string sql(DB_TEXT("SELECT longitude,latitude FROM gps WHERE id = 1234"));

	sel s(((
		  sel(DB_TEXT("gps")) 
		, DB_TEXT("longitude")
		, DB_TEXT("latitude"))
		% (field(DB_TEXT("id"), 1) == 1234)));

	BOOST_CHECK_MESSAGE( sql == string(s)
		, "\n sql is: \"" 
		<< sql.c_str()
		<< "\",\n sel is: \"" 
		<< string(s).c_str()
		<< "\"" );
}

void generate_delete_expression()
{
	db::string sql(DB_TEXT("DELETE FROM gps WHERE id = 1234"));

	db::del d(DB_TEXT("gps")); 
	d.where((db::field(DB_TEXT("id"), 1) == 1234));

	BOOST_CHECK_MESSAGE( sql == db::string(d)
		, "\n sql is: \"" 
		<< sql.c_str()
		<< "\",\n del is: \"" 
		<< db::string(d).c_str()
		<< "\"" );
}

void generate_insert_expression()
{
	db::string sql(DB_TEXT("INSERT INTO gps (id,latitude,longitude) VALUES (1234,11.1234,53.1234)"));

	db::ins i((
		  db::ins(DB_TEXT("gps")) 
		% db::field(DB_TEXT("id"), 1234) 
		% db::field(DB_TEXT("longitude"), 53.1234) 
		% db::field(DB_TEXT("latitude"), 11.1234)
	)); 

	BOOST_CHECK_MESSAGE( sql == db::string(i)
		, "\n sql is: \"" 
		<< sql.c_str()
		<< "\",\n ins is: \"" 
		<< db::string(i).c_str()
		<< "\"" );
}

namespace 
{
	class gps : public db::table
	{
	public:
		TABLE_BEGIN(gps)
			TABLE_MEMBER(id)
			TABLE_MEMBER(longitude)
			TABLE_MEMBER(latitude)
		TABLE_END()

		TABLE_MEMBER_GET_SET(id)
		TABLE_MEMBER_GET_SET(longitude)
		TABLE_MEMBER_GET_SET(latitude)
	};
}

void test_table_class()
{
	gps gps_;

}

test_suite* init_unit_test_suite( int /*argc*/, char* argv[] ) 
{
	argv = argv; // only for a warning

	framework::master_test_suite().
		add( BOOST_TEST_CASE( &generate_insert_expression )/*, 30*/ );

	framework::master_test_suite().
		add( BOOST_TEST_CASE( &generate_update_expression )/*, 30*/ );

	framework::master_test_suite().
		add( BOOST_TEST_CASE( &generate_select_expression )/*, 30*/ );

	framework::master_test_suite().
		add( BOOST_TEST_CASE( &generate_delete_expression )/*, 30*/ );
	framework::master_test_suite().
		add( BOOST_TEST_CASE( &test_table_class )/*, 30*/ );

	return 0;
}

int main( int argc, char ** argv )
{
	using db::string;

#ifdef SQLITE_WRAPPER_NARROW_STRING
	string db_name = argv[0];
#else
	using db::detail::a2w;
	string db_name = a2w(argv[0]);
#endif 

	db_name = db_name.substr(0, db_name.rfind(DB_TEXT("\\")));
	db_name += DB_TEXT("\\test.db");

	int ret(unit_test_main( &init_unit_test_suite, argc, argv )); 

	return ret;
}

