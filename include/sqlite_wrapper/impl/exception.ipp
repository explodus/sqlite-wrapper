///	@file		include/sqlite_wrapper/impl/exception.ipp
///	@date		15.3.2010 14:23
///	file base	exception
///	file ext	ipp
///
/// @author		T. Schroeder (explodus@gmx.de)
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
/// @brief
///
/// This file is part of the sqlite-wrapper project

#ifndef SQLITE_WRAPPER_EXCEPTION_IPP_INCLUDED
#define SQLITE_WRAPPER_EXCEPTION_IPP_INCLUDED

SQLITE_WRAPPER_INLINE db::exception::base::base(const string & msg)
  : _msg(msg) {
}

SQLITE_WRAPPER_INLINE db::exception::base::base(const base & _That)
  : _msg(_That._msg) {
}

SQLITE_WRAPPER_INLINE db::exception::base::~base() {
}

SQLITE_WRAPPER_INLINE const db::char_type * db::exception::base::what() {
  return _msg.c_str();
}

SQLITE_WRAPPER_INLINE void db::exception::base::report() {
  db::cout << _msg << DB_TEXT("\n");
}

SQLITE_WRAPPER_INLINE db::exception::not_found::not_found(const string & s /*=DB_TEXT("")*/)
  : base(DB_TEXT("NotFound: ") + s) {
}

SQLITE_WRAPPER_INLINE db::exception::db_error::db_error(const string & m /*=DB_TEXT("")*/)
  : base(DB_TEXT("Database Error: ") + m) {
}

SQLITE_WRAPPER_INLINE db::exception::sql_error::sql_error(const string & m /*=DB_TEXT("")*/)
  : base(DB_TEXT("SQL Error: ") + m) {
}

SQLITE_WRAPPER_INLINE db::exception::param_error::param_error(const string & m /*=DB_TEXT("")*/)
  : base(DB_TEXT("Param Error: ") + m) {
}

SQLITE_WRAPPER_INLINE
  db::exception::internal_error::internal_error(const string & m /*=DB_TEXT("")*/)
  : base(DB_TEXT("Internal Error: ") + m) {
}

SQLITE_WRAPPER_INLINE db::exception::memory_error::memory_error(const string & m /*=DB_TEXT("")*/)
  : base(DB_TEXT("Allocation failed: ") + m) {
}

SQLITE_WRAPPER_INLINE
  db::exception::insertion_error::insertion_error(const string & m /*=DB_TEXT("")*/)
  : base(DB_TEXT("Database full: ") + m) {
}

SQLITE_WRAPPER_INLINE db::exception::unknown_error::unknown_error(const string & m /*=DB_TEXT("")*/)
  : base(DB_TEXT("Unknown Error: ") + m) {
}

#endif
