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
#endif

#include <sqlite_wrapper/w2a.hpp>

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

	string sql(DB_TEXT("UPDATE gps SET longitude=11.12345678,")
						 DB_TEXT("latitude=53.12345678 WHERE id = 1234"));

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
	using db::string;
	using db::del;
	using db::field;

	string sql(DB_TEXT("DELETE FROM gps WHERE id = 1234"));

	del d(DB_TEXT("gps")); 
	d.where((field(DB_TEXT("id"), 1) == 1234));

	BOOST_CHECK_MESSAGE( sql == string(d)
		, "\n sql is: \"" 
		<< sql.c_str()
		<< "\",\n del is: \"" 
		<< string(d).c_str()
		<< "\"" );
}

void generate_insert_expression()
{
	using db::string;
	using db::ins;
	using db::field;

	string sql(DB_TEXT("INSERT INTO gps (id,latitude,longitude) ")
								 DB_TEXT("VALUES (1234,11.1234,53.1234)"));

	ins i((
		  ins(DB_TEXT("gps")) 
		% field(DB_TEXT("id"), 1234) 
		% field(DB_TEXT("longitude"), 53.1234) 
		% field(DB_TEXT("latitude"), 11.1234)
	)); 

	BOOST_CHECK_MESSAGE( sql == string(i)
		, "\n sql is: \"" 
		<< sql.c_str()
		<< "\",\n ins is: \"" 
		<< string(i).c_str()
		<< "\"" );
}

namespace 
{
	class gps : public db::table
	{
	public:
		TABLE_BEGIN(gps)
			TABLE_MEMBER(longitude)
			TABLE_MEMBER(latitude)
		TABLE_END()

		TABLE_MEMBER_GET_SET(longitude)
		TABLE_MEMBER_GET_SET(latitude)
		
		void test()
		{
			db::string sql_sel(DB_TEXT("SELECT id,latitude,longitude ")
												 DB_TEXT("FROM gps WHERE id = 0"));

			BOOST_CHECK_MESSAGE( sql_sel == get_sel().str()
				, "\n sql_sel is: \"" 
				<< static_cast<const char*>(db::detail::w2a(sql_sel.c_str()))
				<< "\",\n sel is: \"" 
				<< static_cast<const char*>(db::detail::w2a(get_sel().str().c_str()))
				<< "\"" );

			db::string sql_del(DB_TEXT("DELETE FROM gps WHERE id = 0"));

			BOOST_CHECK_MESSAGE( sql_del == get_del().str()
				, "\n sql_del is: \"" 
				<< static_cast<const char*>(db::detail::w2a(sql_del.c_str()))
				<< "\",\n del is: \"" 
				<< static_cast<const char*>(db::detail::w2a(get_del().str().c_str()))
				<< "\"" );

			db::string sql_ins(DB_TEXT("INSERT INTO gps (id,latitude,longitude) ")
												 DB_TEXT("VALUES (0,0,0)"));

			BOOST_CHECK_MESSAGE( sql_ins == get_ins().str()
				, "\n sql_ins is: \"" 
				<< static_cast<const char*>(db::detail::w2a(sql_ins.c_str()))
				<< "\",\n ins is: \"" 
				<< static_cast<const char*>(db::detail::w2a(get_ins().str().c_str()))
				<< "\"" );

			db::string sql_upd(DB_TEXT("UPDATE gps SET latitude=0,longitude=0 ")
												 DB_TEXT("WHERE id = 0"));

			BOOST_CHECK_MESSAGE( sql_upd == get_upd().str()
				, "\n sql_upd is: \"" 
				<< static_cast<const char*>(db::detail::w2a(sql_upd.c_str()))
				<< "\",\n upd is: \"" 
				<< static_cast<const char*>(db::detail::w2a(get_upd().str().c_str()))
				<< "\"" );
		}
	};
}

void test_table_class()
{
	gps gps_;
	gps_.test();

	gps_.longitude(53.1234);
	gps_.latitude(11.1234);

	db::base b;

	try
	{
		b.connect(":memory:");
		gps_.create(b);
		gps_.set(b);
		{
			gps gps_compare;
			gps_compare.get(b);
			BOOST_CHECK(gps_ == gps_compare);
		}
	}
	catch (db::exception::base& e)
	{
		
	}
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

