///	@file		include/sqlite_wrapper/impl/db.ipp
///	@date		15.3.2010 14:23
///	file base	db
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

#ifndef SQLITE_WRAPPER_BASE_IPP_INCLUDED
#define SQLITE_WRAPPER_BASE_IPP_INCLUDED

namespace db {
SQLITE_WRAPPER_INLINE base::progress_handler::progress_handler(sqlite3 * db_, base * base_)
  : _db(db_) {
  sqlite3_progress_handler(_db, 4, progress_handler::xProgressCallback, base_);
}

SQLITE_WRAPPER_INLINE base::progress_handler::~progress_handler() {
  sqlite3_progress_handler(_db, 0, 0, 0);
}

SQLITE_WRAPPER_INLINE base::base(void)
  : _db(0) {
}

SQLITE_WRAPPER_INLINE base::~base(void) {
  if (_db) sqlite3_close(_db);
}

SQLITE_WRAPPER_INLINE void base::throw_error(int status) {
  stringstream err;
  err << status << DB_TEXT("=status code : ") << sqlite3_errmsg(_db);
  switch (status) {
    case SQLITE_ERROR: throw exception::sql_error(err.str());
    case SQLITE_INTERNAL: throw exception::internal_error(err.str());
    case SQLITE_NOMEM: throw exception::memory_error(err.str());
    case SQLITE_FULL: throw exception::insertion_error(err.str());
    default: throw exception::unknown_error(DB_TEXT("compile failed: ") + err.str());
  }
}

SQLITE_WRAPPER_INLINE void base::connect(const std::string & file) {
  if (_db) sqlite3_close(_db);
  int rc = sqlite3_open(file.c_str(), &_db);
  if (rc != SQLITE_OK) throw_error(rc);
  rc = sqlite3_errcode(_db);
  if (rc != SQLITE_OK) throw_error(rc);
}

#ifdef SQLITE_HAS_CODEC
SQLITE_WRAPPER_INLINE void base::connect(const std::string & file, const std::string & key) {
  if (_db) sqlite3_close(_db);
  int rc = sqlite3_open(file.c_str(), &_db);
  if (rc != SQLITE_OK) throw_error(rc);
  rc = sqlite3_errcode(_db);
  if (rc != SQLITE_OK) throw_error(rc);

  rc = sqlite3_key(_db, reinterpret_cast< const void * >(key.c_str()), key.length());
  if (rc != SQLITE_OK) throw_error(rc);
}
#endif

SQLITE_WRAPPER_INLINE void base::begin() {
  int rc(sqlite3_exec(_db, "BEGIN;", 0, 0, 0));
  if (rc != SQLITE_OK) throw_error(rc);
}

SQLITE_WRAPPER_INLINE void base::commit() {
  int rc(sqlite3_exec(_db, "COMMIT;", 0, 0, 0));
  if (rc != SQLITE_OK) throw_error(rc);
}

SQLITE_WRAPPER_INLINE void base::rollback() {
  int rc(sqlite3_exec(_db, "ROLLBACK;", 0, 0, 0));
  if (rc != SQLITE_OK) throw_error(rc);
}

SQLITE_WRAPPER_INLINE db::query & base::execute(const string & cmd) {
  progress_handler progress(_db, this);

  type_last_queries::iterator it(_last_queries.find(cmd));
  if (it == _last_queries.end()) {
    _last_queries.insert(type_last_queries::value_type(cmd, query(*this)));
    it = _last_queries.find(cmd);
  }

  it->second.execute(cmd);
  return it->second;
}

SQLITE_WRAPPER_INLINE db::query_ptr base::execute_ptr(const db::string & cmd) {
  progress_handler progress(_db, this);
  db::query_ptr ret(new query(*this));
  if (ret) ret->execute(cmd);
  return ret;
}

}  // namespace db

#endif
