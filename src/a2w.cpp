
/// @file src/a2w.cpp
///
/// @date 01.03.2010 15:15
///
/// @author T. Schroeder (explodus@gmx.de)
///
/// @brief 
///
/// This file is part of the sqlite-wrapper project

#include <sqlite_wrapper/a2w.hpp>
#include <sqlite_wrapper/detail/exception.hpp>

void db::detail::a2w::init(const char* psz, unsigned nCodePage)
{
	if (psz == 0)
	{
		sz_.clear();
		return;
	}

#if defined (WIN32) || defined (WIN64)
	int lenA(lstrlenA( psz )+1);		 
	int lenW(lenA);

	sz_.resize(lenW);

	bool failed(0 == ::MultiByteToWideChar(
		nCodePage
		, 0
		, psz
		, lenA
		, &*sz_.begin()
		, lenW));

	if (failed)
	{
		if (GetLastError()==ERROR_INSUFFICIENT_BUFFER)
		{
			lenW = ::MultiByteToWideChar(
				nCodePage
				, 0
				, psz
				, lenA
				, 0
				, 0);

			sz_.resize(lenW);

			failed=(0 == ::MultiByteToWideChar(
				nCodePage
				, 0
				, psz
				, lenA
				, &*sz_.begin()
				, lenW));
		}			
	}

	if (failed)
	{
		db::stringstream err;
		err	<< DB_TEXT("Lasterror was ") 
				<< GetLastError() 
				<< DB_TEXT("\n");
		throw db::exception::unknown_error(err.str());
	}
#else
	/// @todo linux conversion...
#endif
}
