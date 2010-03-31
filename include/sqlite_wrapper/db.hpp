///	@file		include/sqlite_wrapper/db.hpp
///	@date		09.03.2010	14:52
///	file base	db
///	file ext	h
///
/// @author		T. Schroeder (explodus@gmx.de)
///
/// This software is released under the BSD Licence
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
///	@brief interface of the db class	
///        purpose: c++ sqlite handling

#ifndef db_h__
#define db_h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning( push, 1 )

#include "config.hpp"

#include <boost/detail/allocator_utilities.hpp>
#include <boost/detail/quick_allocator.hpp> 
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>

#pragma warning( pop )

#include <sqlite_wrapper/detail/time.hpp>
#include <sqlite_wrapper/detail/tools.hpp>
#include <sqlite_wrapper/detail/exception.hpp>
#include <sqlite_wrapper/detail/field.hpp>
#include <sqlite_wrapper/detail/expr.hpp>
#include <sqlite_wrapper/detail/row.hpp>
#include <sqlite_wrapper/detail/query.hpp>
#include <sqlite_wrapper/detail/del.hpp>
#include <sqlite_wrapper/detail/ins.hpp>
#include <sqlite_wrapper/detail/upd.hpp>

namespace db
{
  ///datenbank interface
  class SQLITE_WRAPPER_DLLAPI base
  {
    sqlite3* _db;

    class SQLITE_WRAPPER_DLLAPI progress_handler
    {
      /// @brief     xProgressCallback
      ///
      /// <BR>qualifier
      /// <BR>access    private static  
      /// @return    int
      /// @param     db_ as void *
      ///
      /// @date      20:2:2009   14:13
      ///
      static int xProgressCallback(void* db_); 

      sqlite3* _db;
    public:
      progress_handler(sqlite3* db_, base* base_);
    	~progress_handler();
    };
    
    typedef std::map<string, query> type_last_queries; 
		type_last_queries _last_queries; 

	protected:
		friend class query;

		/// @brief     throw_error
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return    void
		/// @param     status as int
		///
		/// @date      20:2:2009   14:13
		///
		void throw_error(int status); 

  public:
    /// @brief     base
    ///
    /// <BR>qualifier : _db(0)
    /// <BR>access    public  
    /// @return    
    ///
    /// @date      20:2:2009   14:14
    ///
    base();

    /// @brief     ~base
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    
    ///
    /// @date      20:2:2009   14:14
    ///
    ~base();

    enum feature
    {
      e_transact,
      e_blob
    };

    /// @brief     open the specified db
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    void
    /// @param     file as const std::string &
    ///
    /// @date      20:2:2009   14:14
    ///
    void connect(const std::string& file);

    /// @brief     begins SQL transaction
    ///
    /// <BR>qualifier 
    /// <BR>access    public  
    /// @return    void
    ///
    /// @date      20:2:2009   14:14
    ///
    void begin();

    /// @brief     commits SQL transaction
    ///
    /// <BR>qualifier 
    /// <BR>access    public  
    /// @return    void
    ///
    /// @date      20:2:2009   14:15
    ///
    void commit();

    /// @brief     cancels active SQL transaction
    ///
    /// <BR>qualifier 
    /// <BR>access    public  
    /// @return    void
    ///
    /// @date      20:2:2009   14:15
    ///
    void rollback();

    /// @brief     execute SQL statement
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    query&
    /// @param     cmd as const string &
    ///
    /// @date      20:2:2009   14:15
    ///
    query& execute(const string& cmd);

    /// @brief     execute
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    db::query_ptr
    /// @param     cmd as const string &
    ///
    /// @date      13:7:2009   13:14
    ///
    query_ptr execute_ptr(const string& cmd);

    /// @brief     get_db_ptr
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    sqlite3*
    ///
    /// @date      20:2:2009   14:16
    ///
    sqlite3* get_db_ptr() {return _db;}

//#ifndef BOOST_NO_STD_LOCALE
//    typedef boost::signal2<bool, unsigned, unsigned> type_signal_progress; 
//
//    /// @brief     Sig_Progress
//    ///
//    /// <BR>qualifier
//    /// <BR>access    public  
//    /// @return    type_signal_progress&
//    ///
//    /// @date      20:2:2009   14:16
//    ///
//    type_signal_progress& Sig_Progress() { return sig_progress; }
//
//#endif // BOOST_NO_STD_LOCALE
  protected:
//#ifndef BOOST_NO_STD_LOCALE
//    type_signal_progress sig_progress;
//#endif // BOOST_NO_STD_LOCALE
  };
  typedef boost::shared_ptr<base> base_ptr;


} // namespace db


void db::detail::w2a::init(
	  const wchar_t* psz
	, unsigned nConvertCodePage)
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
		err << DB_TEXT("Lasterror was ") 
			<< GetLastError() 
			<< DB_TEXT("\n");
		throw db::exception::unknown_error(err.str());
	}
#else
	/// @todo linux conversion...
#endif
}

inline void db::detail::a2w::init(
	  const char* psz
	, unsigned nCodePage)
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
		err << DB_TEXT("Lasterror was ") 
			<< GetLastError() 
			<< DB_TEXT("\n");
		throw db::exception::unknown_error(err.str());
	}
#else
	/// @todo linux conversion...
#endif
}

#endif // db_h__
