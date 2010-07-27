/// @file include/sqlite_wrapper/impl/w2a.ipp
///
/// @date 01.03.2010 15:15
///
/// @author T. Schroeder (explodus@gmx.de)
///
/// @brief 
///
/// This file is part of the sqlite-wrapper project

#ifndef SQLITE_WRAPPER_W2A_IPP_INCLUDED
#define SQLITE_WRAPPER_W2A_IPP_INCLUDED

inline void db::detail::w2a::init(const wchar_t* psz, unsigned nConvertCodePage)
{
	if (psz == 0)
	{
		sz_.clear();
		return;
	}

#if defined (WIN32) || defined (WIN64)
	int lenW(lstrlenW( psz )+1);		 
	int lenA(lenW*4);

	sz_.resize(lenA);

	bool failed(0 == ::WideCharToMultiByte( 
		nConvertCodePage
		, 0
		, psz
		, lenW
		, &*sz_.begin()
		, lenA
		, 0
		, 0));

	if (failed)
	{
		if (GetLastError()==ERROR_INSUFFICIENT_BUFFER)
		{
			lenA = ::WideCharToMultiByte( 
				nConvertCodePage
				, 0
				, psz
				, lenW
				, 0
				, 0
				, 0
				, 0);

			sz_.resize(lenA);

			failed=(0 == ::WideCharToMultiByte(
				nConvertCodePage
				, 0
				, psz
				, lenW
				, &*sz_.begin()
				, lenA
				, 0
				, 0));
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

#endif