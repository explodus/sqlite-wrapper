///	@file doc/sqlite-wrapper.hpp
///  
///	@date 10.3.2010 20:12
///  
/// @author T. Schroeder (explodus@gmx.de)
///  
/// This file is part of the sqlite_wrapper project
///

/// @defgroup sqlite_wrapper wrapper
/// @defgroup sqlite_wrapper_examples examples
///
/// @mainpage Wrapper around Sqlite with logging facilities
/// @ingroup sqlite_wrapper
/// @section source Getting the source
///
/// @verbatim git clone git://github.com/explodus/sqlite-wrapper.git sqlite_wrapper @endverbatim
///
/// @section compile Compiling
/// 
/// @subsection Prerequesites
/// 	- CMake 2.6
/// 	- Boost 1.38
///
/// @ref sqlite_wrapper_examples
///
/// @section main_todo Todo
///
/// @copydoc todo
///

/// @page update usage example of the wrapper
/// @ingroup sqlite_wrapper_examples
///
/// only a simple example
///
/// @code
/// try
/// {
/// 	db::base_ptr _db(get_db());
/// 	
/// 	if (_db==0)
/// 		return;
/// 
/// 	double anl_id(db::detail::to_type<double>(db::string(_code)));
/// 	db::string kenst(DB_TEXT("Blub"));
/// 
/// 	_db->execute_ptr(
/// 		(db::upd(L"TermChange") % 
/// 			db::field(L"KENST", kenst) %
/// 			(
/// 				db::field(L"ANL_ID", 0.0) == anl_id && 
/// 				db::field(L"NR", 0) == _nr
/// 			)
/// 		)
/// 	);
/// }
/// catch (db::exception::base& e)
/// {
/// 		db::log::singleton::basic::log_msg msg(
/// 			db::log::singleton::basic::get_log()
/// 		, e.what() 
/// 		, db::log::log_error);
/// 	return;
/// }
/// @endcode
///
/// insert example
///
/// @code
///
/// db::string sql(DB_TEXT("INSERT INTO gps (id,latitude,longitude) VALUES (1234,11.1234,53.1234)"));
///
/// db::ins i((
/// 	db::ins(DB_TEXT("gps")) 
/// 	% db::field(DB_TEXT("id"), 1234) 
/// 	% db::field(DB_TEXT("longitude"), 53.1234) 
/// 	% db::field(DB_TEXT("latitude"), 11.1234)
/// 	)); 
///
/// BOOST_CHECK_MESSAGE( sql == db::string(i)
/// 	, "\n sql is: \"" 
/// 	<< db::detail::w2a(sql.c_str())
/// 	<< "\",\n ins is: \"" 
/// 	<< db::detail::w2a(db::string(i).c_str())
/// 	<< "\"" );
///
/// @endcode
///
/// select example
///
/// @code
///
/// void generate_select_expression()
/// {
///	 using db::string;
///	 using db::sel;
///	 using db::field;
///
///	 string sql(DB_TEXT("SELECT longitude,latitude FROM gps WHERE id = 1234"));
///
///	 sel s(((
/// 		  sel(DB_TEXT("gps")) 
/// 		, DB_TEXT("longitude")
/// 		, DB_TEXT("latitude"))
/// 		% (field(DB_TEXT("id"), 1) == 1234)));
/// 
/// 	BOOST_CHECK_MESSAGE( sql == string(s)
/// 		, "\n sql is: \"" 
/// 		<< db::detail::w2a(sql.c_str())
/// 		<< "\",\n sel is: \"" 
/// 		<< db::detail::w2a(string(s).c_str())
/// 		<< "\"" );
/// }
///
/// @endcode
///
/// update example
///
/// @code
///
///void generate_update_expression()
///{
///	using db::string;
///	using db::upd;
///	using db::field;
///
///	string sql(DB_TEXT("UPDATE gps SET longitude=11.12345678,latitude=53.12345678 WHERE id = 1234"));
///
///	upd u((
///		  upd(DB_TEXT("gps")) 
///		% field(DB_TEXT("longitude"), 11.12345678)
///		% field(DB_TEXT("latitude"), 53.12345678) 
///		% (field(DB_TEXT("id"), 1) == 1234)));
///
///	BOOST_CHECK(sql == string(u));
///}
///
/// @endcode
///
/// delete example
///
/// @code
///
/// db::string sql(DB_TEXT("DELETE FROM gps WHERE id = 1234"));
///
/// db::del d(DB_TEXT("gps")); 
/// d.where((db::field(DB_TEXT("id"), 1) == 1234));
///
/// BOOST_CHECK_MESSAGE( sql == db::string(d)
/// 	, "\n sql is: \"" 
/// 	<< db::detail::w2a(sql.c_str())
/// 	<< "\",\n sel is: \"" 
/// 	<< db::detail::w2a(db::string(d).c_str())
/// 	<< "\"" );
///
/// @endcode

#include <config.hpp>
namespace db {}
