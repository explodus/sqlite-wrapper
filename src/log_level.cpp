///	@file		src/log_level.cpp
///	@date		15.3.2010 14:23
///	file base	log_level
///	file ext	cpp
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

#include <sqlite_wrapper/config.hpp>
#include <sqlite_wrapper/log.hpp>

///
db::log::level db::log::global_level(db::log::log_error);

db::log::singleton::db::log_type& 
db::log::singleton::db::get_log(const string& name/* = DB_TEXT("log.db")*/)
{
	static log_type ret
		( boost::make_shared<log_type::provider_type >
		( boost::make_shared<buffer_type >(name)));
	return ret;
}

db::log::singleton::basic::log_type& 
db::log::singleton::basic::get_log()
{
	static log_type ret
		( boost::make_shared<log_type::provider_type >
		( boost::make_shared<buffer_type >()));
	return ret;
}

db::log::singleton::io::log_type& 
db::log::singleton::io::get_log(const string& name/* = DB_TEXT("log.txt")*/)
{
	static log_type ret
		( boost::make_shared<log_type::provider_type >
		( boost::make_shared<buffer_type >(name)));
	return ret;
}
