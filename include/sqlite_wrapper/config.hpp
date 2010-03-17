/// @file config.hpp
///
/// @date 10.03.2010 09:36
///
/// @author T. Schr�der (explodus@gmx.de)
///
/// @brief 
///
/// This file is part of the sqlite-wrapper project
 
#ifndef SQLITE_WRAPPER_CONFIG_HPP_INCLUDED
#define SQLITE_WRAPPER_CONFIG_HPP_INCLUDED
 
/* #undef SQLITE_WRAPPER_HAVE_VARIADIC_TEMPLATES */
/* #undef SQLITE_WRAPPER_HAVE_POSIX */
/* #undef SQLITE_WRAPPER_HAVE_ATLEAST_UINT64 */
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
#include <vector>
#include <map>
#include <iosfwd>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>

#if defined (WIN32) || defined (WIN64)
	#include <windows.h>
	
	#ifdef max	
		#undef max
	#endif

	#ifdef min
		#undef min
	#endif
#endif 

#include <boost/format.hpp>

#ifndef BOOST_NO_STD_LOCALE
	#include <boost/signals.hpp>
	namespace sigb = boost::BOOST_SIGNALS_NAMESPACE;
#endif

/// @todo entfernen
#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(P) { (P) = (P); } 
#endif

#ifndef _tcsstr
	#ifdef _UNICODE
		#define _tcsstr wcsstr
	#else
		#define _tcsstr strstr
	#endif // _UNICODE
#endif

namespace db
{
#ifdef SQLITE_WRAPPER_NARROW_STRING
	typedef char char_type;
	#define DB_TEXT(x) x
	typedef std::string string;
#else
	typedef wchar_t char_type;
	#define DB_TEXT(x) L ## x
	typedef std::wstring string;
#endif
 
	typedef std::basic_ostream<char_type> ostream;
	typedef std::basic_istream<char_type> istream;
	
	typedef std::basic_ofstream
	<
	  char_type
	, std::char_traits<char_type> 
	> 
	ofstream;
	
	typedef std::basic_ifstream
	<
	  char_type
	, std::char_traits<char_type> 
	> 
	ifstream;
	
	typedef std::basic_ostringstream<char_type> ostringstream;
	typedef std::basic_istringstream<char_type> istringstream;
	typedef std::basic_stringstream<char_type>  stringstream;
	typedef boost::basic_format<char_type > format;

	typedef std::pair<string, string> string_pair;
	typedef std::vector<string> string_vec;
 
	/// defintion of db::cout
	extern ostream &cout;
	
	namespace detail
	{
		/// char to wchar_t conversion class
		class a2w
		{
		public:
			/// @brief        constructor
			///
			/// <BR>qualifier throw(...) : empty_(0)
			/// <BR>access    private  
			/// @return       
			/// @param        psz as _In_opt_ LPCWSTR
			/// @param        nCodePage as unsigned
			///
			/// @date         12.3.2010 14:16
			///
			a2w(
				  const char* psz
				, unsigned nCodePage = CP_UTF8 ) throw(...) : 
			empty_(0)
			{
				init( psz, nCodePage );
			}

			/// <BR>qualifier const throw()
			/// <BR>access    public  
			/// @return       const wchar_t*
			///
			/// @date         12.3.2010 14:39
			///
			operator const wchar_t*() const throw()
			{
				if (sz_.size())
					return( &*sz_.begin() );
				else
					return empty_;
			}

		private:
			/// @brief        init
			///
			/// <BR>qualifier throw(...)
			/// <BR>access    private  
			/// @return       void
			/// @param        psz as _In_opt_ const char *
			/// @param        nConvertCodePage as _In_ UINT
			///
			/// @date         12.3.2010 14:39
			///
			inline void init(
				  const char* psz
				, UINT nConvertCodePage) throw(...);

		public:
			const wchar_t* empty_;
			std::vector<wchar_t> sz_;

		private:
			a2w( const a2w& ) throw();
			a2w& operator=( const a2w& ) throw();
		};

		/// wchar_t to char conversion class
		class w2a
		{
		public:
			/// @brief        constructor
			///
			/// <BR>qualifier throw(...) : empty_(0)
			/// <BR>access    private  
			/// @return       
			/// @param        psz as const wchar_t*
			/// @param        nCodePage as unsigned
			///
			/// @date         12.3.2010 14:16
			///
			w2a(
				  const wchar_t* psz
				, unsigned nCodePage = CP_ACP ) throw(...) : 
				empty_(0)
			{
				init( psz, nCodePage );
			}

			/// <BR>qualifier const throw()
			/// <BR>access    public  
			/// @return       const char*
			///
			/// @date         12.3.2010 14:39
			///
			operator const char*() const throw()
			{
				if (sz_.size())
					return( &*sz_.begin() );
				else
					return empty_;
			}

		private:
			/// @brief        init
			///
			/// <BR>qualifier throw(...)
			/// <BR>access    private  
			/// @return       void
			/// @param        psz as _In_opt_ const wchar_t *
			/// @param        nConvertCodePage as _In_ UINT
			///
			/// @date         12.3.2010 14:39
			///
			inline void init(
				  const wchar_t* psz
				, UINT nConvertCodePage) throw(...);

		public:
			const char* empty_;
			std::vector<char> sz_;

		private:
			w2a( const w2a& ) throw();
			w2a& operator=( const w2a& ) throw();
		};
	}
}
 
#endif
