/// @file w2a.hpp
///
/// @date 10.03.2010 09:36
///
/// @author T. Schröder (explodus@gmx.de)
///
/// @brief 
///
/// This file is part of the sqlite-wrapper project
 
#ifndef SQLITE_WRAPPER_W2A_HPP_INCLUDED
#define SQLITE_WRAPPER_W2A_HPP_INCLUDED

#include <sqlite_wrapper/config.hpp>

namespace db { namespace detail
{
	/// @brief wchar_t to char conversion class
	///
	/// an example:
	/// @code
	///
	/// std::string data( db::detail::w2a( _data.c_str() ) );
	///
	/// return boost::date_time::parse_date< boost::gregorian::date >( data );
	///
	/// @endcode
	class w2a
	{
	public:
		/// @brief        constructor
		///
		/// <BR>qualifier throw() : empty_(0)
		/// <BR>access    private  
		/// 
		/// @param        psz as const wchar_t*
		/// @param        nCodePage as unsigned
		///
		/// @date         12.3.2010 14:16
		///
		w2a(const wchar_t* psz, unsigned nCodePage = 0 /*CP_ACP*/ ) : empty_(0)
		{
			init( psz, nCodePage );
		}

		/// @brief        constructor
		///
		/// <BR>qualifier : empty_(0)
		/// <BR>access    private 
		/// 
		/// @param        psz as const char *
		///
		/// @date         2.5.2010 17:14
		/// 
		w2a(const char* psz) : empty_(0)
		{
			if (strlen(psz)==0)
				return;
			sz_.resize(strlen(psz)+1);
			memcpy(&*sz_.begin(), psz, strlen(psz));
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
		/// <BR>qualifier throw()
		/// <BR>access    private  
		/// @return       void
		/// @param        psz as _In_opt_ const wchar_t *
		/// @param        nConvertCodePage as unsigned
		///
		/// @date         12.3.2010 14:39
		///
		void init(const wchar_t* psz, unsigned nConvertCodePage);

	public:
		const char* empty_;
		std::vector<char> sz_;

	private:
		w2a( const w2a& ) throw();
		w2a& operator=( const w2a& ) throw();
	};

} }
 
#endif
