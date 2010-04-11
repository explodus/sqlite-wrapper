///	@file doc/sqlite-wrapper.hpp
///  
///	@date 10.3.2010 20:12
///  
/// @author T. Schroeder (explodus@gmx.de)
///  
/// This file is part of the sqlite_wrapper project
///

/// @defgroup sqlite_wrapper Db Wrapper around Sqlite
///
/// @mainpage Db Wrapper around Sqlite
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
/// @section main_todo Todo
///
/// @copydoc todo
///
/// @}

/// @page update usage example of the wrapper
/// @ingroup sqlite_wrapper
///
/// only a simple example
///
/// @code
///try
///{
///	db::base_ptr _db(get_db());
///	
///	if (_db==0)
///		return;
///
///	double anl_id(db::detail::to_type<double>(db::string(_code)));
///	db::string kenst(DB_TEXT("Blub"));
///
///	_db->execute_ptr(
///		(db::upd(L"TermChange") % 
///			db::field(L"KENST", kenst) %
///			(
///				db::field(L"ANL_ID", 0.0) == anl_id && 
///				db::field(L"NR", 0) == _nr
///			)
///		)
///	);
///}
///catch (db::exception::base& e)
///{
///		db::log::singleton::basic::log_msg msg(
///			db::log::singleton::basic::get_log()
///		, e.what() 
///		, db::log::log_error);
///	return;
///}
/// @endcode
///
/// @ref sqlite_wrapper

#include <config.hpp>
namespace db {}
