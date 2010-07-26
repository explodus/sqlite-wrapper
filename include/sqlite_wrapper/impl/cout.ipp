
/// @file src/cout.cpp
///
/// @date 01.03.2010 15:15
///
/// @author T. Schroeder (explodus@gmx.de)
///
/// @brief 
///
/// This file is part of the sqlite-wrapper project

#ifndef SQLITE_WRAPPER_COUT_IPP_INCLUDED
#define SQLITE_WRAPPER_COUT_IPP_INCLUDED

///
db::ostream& db::cout
(
#ifdef SQLITE_WRAPPER_NARROW_STRING
	std::cout
#else
	std::wcout
#endif
);

#endif