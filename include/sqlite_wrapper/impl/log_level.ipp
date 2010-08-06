///	@file		include/sqlite_wrapper/impl/log_level.ipp
///	@date		15.3.2010 14:23
///	file base	log_level
///	file ext	ipp
///
/// @author		T. Schroeder (explodus@gmx.de)
///	
///	Using		:
///		This code may be used in compiled form in any way you desire. This
///		file may be redistributed unmodified by any means PROVIDING it is
///		not sold for profit without the authors written consent, and
///		providing that this notice and the authors name is included.
///		This file is provided 'as is' with no expressed or implied warranty.
///		The author accepts no liability if it causes any damage to your 
///		computer.
///	Expect Bugs		:
///		Please let me know of any bugs/mods/improvements.
///		and I will try to fix/incorporate them into this file.
///		Enjoy!
///
/// @brief 
///
/// This file is part of the sqlite-wrapper project

#ifndef SQLITE_WRAPPER_LOGLEVEL_IPP_INCLUDED
#define SQLITE_WRAPPER_LOGLEVEL_IPP_INCLUDED

#include <boost/make_shared.hpp>

///
namespace db { namespace log 
{
	static db::log::level _global_level(db::log::log_error);
}}

inline db::log::level db::log::global_level()
{
	return db::log::_global_level;
}

inline void db::log::global_level(db::log::level lvl)
{
	db::log::_global_level = lvl;
}

inline db::log::singleton::db::log_type& 
db::log::singleton::db::get_log(const string& name/* = DB_TEXT("log.db")*/)
{
	static log_type ret
		( boost::make_shared<provider_type >
		( boost::make_shared<buffer_type >(name)));
	return ret;
}

inline db::log::singleton::basic::log_type& 
db::log::singleton::basic::get_log()
{
	static log_type ret
		( boost::shared_ptr<provider_type >
			( new provider_type
				( boost::shared_ptr<buffer_type >
					(new buffer_type)
				)
			)
		);
	return ret;
}

inline db::log::singleton::io::log_type& 
db::log::singleton::io::get_log(const string& name/* = DB_TEXT("log.txt")*/)
{
	static log_type ret
		( boost::make_shared<provider_type >
		( boost::make_shared<buffer_type >(name)));
	return ret;
}

#endif