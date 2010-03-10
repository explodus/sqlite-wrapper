/// @file config.hpp
///
/// @date 10.03.2010 09:36
///
/// @author T. Schröder (explodus@gmx.de)
///
/// @brief 
///
/// This file is part of the sqlite-wrapper project
 
#ifndef SQLITE_WRAPPER_CONFIG_HPP_INCLUDED
#define SQLITE_WRAPPER_CONFIG_HPP_INCLUDED
 
/* #undef SQLITE_WRAPPER_HAVE_VARIADIC_TEMPLATES */
/* #undef SQLITE_WRAPPER_HAVE_POSIX */
#define SQLITE_WRAPPER_HAVE_ATLEAST_UINT64
/* #undef SQLITE_WRAPPER_HAVE_DEFAULTED_AND_DELETED_FUNCTIONS */
/* #undef SQLITE_WRAPPER_NARROW_STRING */
/* #undef SQLITE_WRAPPER_HAVE_CLOCK_GETTIME */
/* #undef SQLITE_WRAPPER_STRING_IS_UTF8 */
/* #undef SQLITE_WRAPPER_HAS_GNU_DEMANGLE */
/* #undef SQLITE_WRAPPER_HAVE_GCC_PRAGMA_MESSAGE */
 
#ifdef _WIN32
#define SQLITE_WRAPPER_WINDOWS_PLATFORM 1
#elif defined(SQLITE_WRAPPER_HAVE_POSIX)
#define SQLITE_WRAPPER_POSIX_PLATFORM 1
#ifdef __APPLE__
#define SQLITE_WRAPPER_DARWIN_PLATFORM
#endif
#endif
 
#ifdef __GNUC__
/* #undef SQLITE_WRAPPER_HAVE_GCC_VISIBILITY */
#endif
 
#include <string>
#include <iosfwd>
 
namespace db
{
#ifdef SQLITE_WRAPPER_NARROW_STRING
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
