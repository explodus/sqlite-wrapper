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
/// @verbatim git clone git://github.com/explodus/sqlite-wrapper.git sqlite-wrapper @endverbatim
///
/// @section compile Compiling
/// 
/// @subsection Prerequesites
/// 	- Cmake
/// 	- Boost
///
/// @section main_todo Todo
///
/// @copydoc todo
///
/// @}

/// @page sqlite_wrapper Db Wrapper around Sqlite
/// @ingroup sqlite_wrapper
///
/// @todo somehow get the content from the mainpage in here
///
/// only a simple example
///
/// @code
///try
///{
///	db::base_ptr _db(theApp.get_db());
///	
///	if (_db==0)
///		return;
///
///	using db::field;
///	using db::upd;
///
///	double anl_id(db::detail::to_type<double>(db::string(_code)));
///
///	if (theApp.Term())
///		theApp.Term()->Kenst(((*_q)[_sel]).value(0).str().c_str());
///
///	_db->execute_ptr(
///		(upd(L"TermChange") % 
///			field(L"KENST", ((*_q)[_sel]).value(0).str()) %
///			(
///				field(L"ANL_ID", 0.0)==anl_id && 
///				field(L"NR", 0)==_nr
///			)
///		)
///	);
///}
///catch (db::exception::base& e)
///{
///	CString err(e.what());
///	tBox(err);
///	return;
///}
/// @endcode
///
/// @ref sqlite_wrapper

#include <config.hpp>
namespace db {}
