
/// @file src/cout.cpp
///
/// @date 01.03.2010 15:15
///
/// @author T. Schroeder (explodus@gmx.de)
///
/// @brief 
///
/// This file is part of the sqlite-wrapper project

#include <sqlite_wrapper/config.hpp>
#include <sqlite_wrapper/db.hpp>

#include <iostream>

///
db::ostream& db::cout
(
#ifdef SQLITE_WRAPPER_NARROW_STRING
	std::cout
#else
	std::wcout
#endif
);
