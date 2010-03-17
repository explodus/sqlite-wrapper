///	@file		include/sqlite_wrapper/detail/exception.hpp
///	@date		15.3.2010 8:45
///	file base	tools
///	file ext	hpp
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
///	@brief 

#ifndef DB_EXCEPTION_HPP_INCLUDED
#define DB_EXCEPTION_HPP_INCLUDED

#include <sqlite_wrapper/config.hpp>

namespace db { namespace exception
{
	/// exception basis interface
	class base
	{
	private:
		string _msg; 
	public:
		base(const string& msg)  : _msg(msg) 
		{ }
		base(const base& _That)  : _msg(_That._msg) 
		{ }
		virtual ~base() {}

		//virtual base& operator=(const base& _That)
		//{
		//	_msg = _That._msg;
		//	return *this;
		//}

		/// @brief        what
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return       const TCHAR*
		///
		/// @author       T. Schroeder
		/// @author       explodus@gmx.de
		/// @date         16.3.2010 16:24
		///
		const TCHAR* what() 
		{ return _msg.c_str(); }

		/// @brief        report
		///
		/// <BR>qualifier
		/// <BR>access    virtual public  
		/// @return       void
		///
		/// @author       T. Schroeder
		/// @author       explodus@gmx.de
		/// @date         16.3.2010 16:25
		///
		virtual void report() 
		{ db::cout << _msg << DB_TEXT("\n"); }

		/// @brief        operator<<
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return       friend ostream &
		/// @param        os as ostream &
		/// @param        e as base &
		///
		/// @author       T. Schroeder
		/// @author       explodus@gmx.de
		/// @date         16.3.2010 16:25
		///
		friend ostream &operator<<(ostream &os, base &e) {
			os << e._msg;
			return os;
		} 
	};

	/// exception thrown when a record is not found
	class not_found : public base {
	public:
		not_found(const string& s=DB_TEXT("")) : 
				base(DB_TEXT("NotFound: ")+s){}
	};

	/// exception thrown when database cannot be accessed
	class db_error : public base {
	public:
		db_error(const string& m=DB_TEXT("")) : 
				base(DB_TEXT("Database Error: ")+m){}
	};

	/// exception thrown when SQL statement cannot be executed
	class sql_error : public base {
	public:
		sql_error(const string& m=DB_TEXT("")) : 
				base(DB_TEXT("SQL Error: ")+m){}
	};

	/// exception thrown when Param handling get failed
	class param_error : public base {
	public:
		param_error(const string& m=DB_TEXT("")) : 
				base(DB_TEXT("Param Error: ")+m){}
	};

	/// exception thrown when backend produces internal error
	class internal_error : public base {
	public:
		internal_error(const string& m=DB_TEXT("")) : 
				base(DB_TEXT("Internal Error: ")+m){}
	};

	/// exception thrown when backend cannot allocate memory
	class memory_error : public base {
	public:
		memory_error(const string& m=DB_TEXT("")) : 
				base(DB_TEXT("Allocation failed: ")+m){}
	};

	/// exception thrown when database (disk) is full
	class insertion_error : public base {
	public:
		insertion_error(const string& m=DB_TEXT("")) : 
				base(DB_TEXT("Database full: ")+m){}
	};

	/// exception thrown when none of other exceptions match
	class unknown_error : public base {
		// handles the rest
	public:
		unknown_error(const string& m=DB_TEXT("")) : 
				base(DB_TEXT("Unknown Error: ")+m){}
	};

} }

#endif