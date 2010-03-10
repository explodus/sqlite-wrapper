/// @file config.hpp
///
/// @date 10.03.2010 09:36
///
/// @author T. Schröder (explodus@gmx.de)
///
/// @brief 
///
/// This file is part of the sqlite-wrapper project
 
#ifndef SQLITE-WRAPPER_CONFIG_HPP_INCLUDED
#define SQLITE-WRAPPER_CONFIG_HPP_INCLUDED
 
/* #undef SQLITE */
/* #undef SQLITE */
/* #undef SQLITE */
/* #undef SQLITE */
/* #undef SQLITE */
/* #undef SQLITE */
/* #undef SQLITE */
/* #undef SQLITE */
/* #undef SQLITE */
 
#ifdef _WIN32
#define SQLITE-WRAPPER_WINDOWS_PLATFORM 1
#elif defined(SQLITE-WRAPPER_HAVE_POSIX)
#define SQLITE-WRAPPER_POSIX_PLATFORM 1
#ifdef __APPLE__
#define SQLITE-WRAPPER_DARWIN_PLATFORM
#endif
#endif
 
#ifdef __GNUC__
/* #undef SQLITE */
#endif
 
#include <string>
#include <iosfwd>
 
namespace db
{
#ifdef SQLITE-WRAPPER_NARROW_STRING
	typedef char char_type;
	#define DB_TEXT(x) x
#else
	typedef wchar_t char_type;
	#define DB_TEXT(x) L ## x
#endif
 
	typedef std::basic_string<char_type> string;
	typedef std::basic_ostream<char_type> ostream;
	typedef std::basic_istream<char_type> istream;
 
	/// defintion of db::cout
	extern ostream &cout;
}
 
#endif
