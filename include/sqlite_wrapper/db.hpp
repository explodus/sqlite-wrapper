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
#	pragma once
#endif // _MSC_VER > 1000

#include <sqlite_wrapper/config.hpp>

#if _MSC_VER > 1000
#	pragma warning( push, 1 )
#endif // _MSC_VER > 1000

#include <boost/detail/allocator_utilities.hpp>
#include <boost/detail/quick_allocator.hpp> 
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>

#if _MSC_VER > 1000
#	pragma warning( pop )
#endif // _MSC_VER > 1000

#include <sqlite_wrapper/detail/time.hpp>
#include <sqlite_wrapper/detail/tools.hpp>
#include <sqlite_wrapper/detail/exception.hpp>
#include <sqlite_wrapper/detail/field.hpp>
#include <sqlite_wrapper/detail/expr.hpp>
#include <sqlite_wrapper/detail/row.hpp>
#include <sqlite_wrapper/detail/query.hpp>

namespace db
{
  ///datenbank interface
  class SQLITE_WRAPPER_DLLAPI base
  {
    sqlite3* _db;

    class SQLITE_WRAPPER_DLLAPI progress_handler
    {
			/// @brief        got called from sqlite, and calls the connected signal
      ///
      /// <BR>qualifier
      /// <BR>access    private static  
      /// @return       int
      /// @param        db_ as void *
      ///
      /// @author       T. Schroeder (explodus@gmx.de)
      /// @date         15.4.2010 13:47
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

		/// @brief        got called from db::query if a sqlite error was occured
		///
		/// <BR>qualifier
		/// <BR>access    protected  
		/// @return       void
		/// @param        status as int
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         15.4.2010 13:49
		///
		void throw_error(int status); 

  public:
    base();
    ~base();

    enum feature
    {
      e_transact,
      e_blob
    };

		/// @brief        open the specified db
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return       void
    /// @param        file as const std::string &
    ///
    /// @author       T. Schroeder (explodus@gmx.de)
    /// @date         15.4.2010 13:49
    ///
    void connect(const std::string& file);

    /// @brief				begins SQL transaction
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return       void
    ///
    /// @author       T. Schroeder (explodus@gmx.de)
    /// @date         15.4.2010 13:49
    ///
    void begin();

    /// @brief        commits SQL transaction
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return       void
    ///
    /// @author       T. Schroeder (explodus@gmx.de)
    /// @date         15.4.2010 13:50
    ///
    void commit();

    /// @brief        cancels active SQL transaction
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return       void
    ///
    /// @author       T. Schroeder (explodus@gmx.de)
    /// @date         15.4.2010 13:50
    ///
    void rollback();

    /// @brief        execute SQL statement
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return       query&
    /// @param        cmd as const string &
    ///
    /// @author       T. Schroeder (explodus@gmx.de)
    /// @date         15.4.2010 13:50
    ///
    query& execute(const string& cmd);

    /// @brief        execute SQL statement
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return       db::query_ptr
    /// @param        cmd as const string &
    ///
    /// @author       T. Schroeder (explodus@gmx.de)
    /// @date         15.4.2010 13:51
    ///
    query_ptr execute_ptr(const string& cmd);

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

#endif // db_h__
