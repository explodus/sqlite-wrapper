///	@file		sqlite_wrapper/detail/exception.hpp
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
	class SQLITE_WRAPPER_DLLAPI base
	{
	private:
		string _msg; 
	public:
		base(const string& msg);
		base(const base& _That);
		virtual ~base();

		//virtual base& operator=(const base& _That)
		//{
		//	_msg = _That._msg;
		//	return *this;
		//}

		/// <BR>qualifier
		/// <BR>access    public  
		/// @return       const db::char_type*
		///
		/// @author       T. Schroeder
		/// @author       explodus@gmx.de
		/// @date         16.3.2010 16:24
		///
		const db::char_type* what();

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
		virtual void report();

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
		friend ostream &operator<<(ostream &os, base &e)
		{
			os << e._msg;
			return os;
		}

	};

	/// exception thrown when a record is not found
	class SQLITE_WRAPPER_DLLAPI not_found : public base 
	{
	public:
		explicit not_found(const string& s=DB_TEXT(""));
	};

	/// exception thrown when database cannot be accessed
	class SQLITE_WRAPPER_DLLAPI db_error : public base 
	{
	public:
		explicit db_error(const string& m=DB_TEXT(""));
	};

	/// exception thrown when SQL statement cannot be executed
	class SQLITE_WRAPPER_DLLAPI sql_error : public base 
	{
	public:
		explicit sql_error(const string& m=DB_TEXT(""));
	};

	/// exception thrown when Param handling get failed
	class SQLITE_WRAPPER_DLLAPI param_error : public base 
	{
	public:
		explicit param_error(const string& m=DB_TEXT(""));
	};

	/// exception thrown when backend produces internal error
	class SQLITE_WRAPPER_DLLAPI internal_error : public base 
	{
	public:
		explicit internal_error(const string& m=DB_TEXT(""));
	};

	/// exception thrown when backend cannot allocate memory
	class SQLITE_WRAPPER_DLLAPI memory_error : public base 
	{
	public:
		explicit memory_error(const string& m=DB_TEXT(""));
	};

	/// exception thrown when database (disk) is full
	class SQLITE_WRAPPER_DLLAPI insertion_error : public base 
	{
	public:
		explicit insertion_error(const string& m=DB_TEXT(""));
	};

	/// exception thrown when none of other exceptions match
	class SQLITE_WRAPPER_DLLAPI unknown_error : public base 
	{
		// handles the rest
	public:
		explicit unknown_error(const string& m=DB_TEXT(""));
	};

} }

#endif