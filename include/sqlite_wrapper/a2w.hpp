/// @file a2w.hpp
///
/// @date 10.03.2010 09:36
///
/// @author T. Schröder (explodus@gmx.de)
///
/// @brief 
///
/// This file is part of the sqlite-wrapper project
 
#ifndef SQLITE_WRAPPER_A2W_HPP_INCLUDED
#define SQLITE_WRAPPER_A2W_HPP_INCLUDED

#include <sqlite_wrapper/config.hpp>

namespace db { namespace detail 
{
	/// char to wchar_t conversion class
	///
	/// an example:
	/// @code
	///
	/// std::string blob( reinterpret_cast<const char*>( sqlite3_column_blob( stm, _col ) ) );
	///
	/// std::wstring wblob = db::detail::a2w( blob.c_str() );
	///
	/// @endcode
	class a2w
	{
	public:
		/// @brief        constructor
		///
		/// <BR>qualifier throw() : empty_(0)
		/// <BR>access    private  
		/// @return       
		/// @param        psz as _In_opt_ LPCWSTR
		/// @param        nCodePage as unsigned
		///
		/// @date         12.3.2010 14:16
		///
		a2w(const char* psz, unsigned nCodePage = 65001 /*CP_UTF8*/ ) : empty_(0)
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
		/// <BR>qualifier throw()
		/// <BR>access    private  
		/// @return       void
		/// @param        psz as _In_opt_ const char *
		/// @param        nConvertCodePage as unsigned
		///
		/// @date         12.3.2010 14:39
		///
		void init(const char* psz, unsigned nConvertCodePage);

	public:
		const wchar_t* empty_;
		std::vector<wchar_t> sz_;

	private:
		a2w( const a2w& ) throw();
		a2w& operator=( const a2w& ) throw();
	};

} }
 
#endif
