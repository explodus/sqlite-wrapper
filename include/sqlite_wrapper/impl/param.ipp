///	@file		include/sqlite_wrapper/impl/param.ipp
///	@date		15.3.2010 14:23
///	file base	param
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

#ifndef SQLITE_WRAPPER_PARAM_IPP_INCLUDED
#define SQLITE_WRAPPER_PARAM_IPP_INCLUDED

SQLITE_WRAPPER_INLINE db::param::param()
  : _type(e_null)
  , _is_changed(false) {
  _data = DB_TEXT("");
}

SQLITE_WRAPPER_INLINE db::param::param(unsigned col)
  : _col(col)
  , _type(e_null)
  , _is_changed(false) {
  _data = DB_TEXT("");
}

SQLITE_WRAPPER_INLINE db::param::param(const param & Param)
  : _col(Param._col)
  , _type(Param._type)
  , _is_changed(false)
  , _data(Param._data) {
}

SQLITE_WRAPPER_INLINE db::param::~param() {
}

SQLITE_WRAPPER_INLINE db::param_types db::param::get_type() const {
  return _type;
}

SQLITE_WRAPPER_INLINE void db::param::set_type(param_types Type) {
  _type = Type;
}

SQLITE_WRAPPER_INLINE bool & db::param::is_changed() {
  return _is_changed;
}

SQLITE_WRAPPER_INLINE void db::param::set(sqlite3_stmt * stm) {
  if (!stm) {
    set_null();
    return;
  }

  switch (sqlite3_column_type(stm, _col)) {
    case SQLITE_INTEGER: set(sqlite3_column_int(stm, _col)); break;
    case SQLITE_FLOAT: set(sqlite3_column_double(stm, _col)); break;
    case SQLITE_TEXT:
#ifdef _UNICODE
      set(reinterpret_cast< const wchar_t * >(sqlite3_column_text16(stm, _col)));
#else
      set(reinterpret_cast< const char * >(sqlite3_column_text(stm, _col)));
#endif  // _UNICODE
      break;
    case SQLITE_BLOB: {
#ifdef _UNICODE
      std::string blob(reinterpret_cast< const char * >(sqlite3_column_blob(stm, _col)));
      std::wstring wblob = static_cast< const wchar_t * >(detail::a2w(blob.c_str()));
      set(wblob);
#else
      set(reinterpret_cast< const char * >(sqlite3_column_blob(stm, _col)));
#endif  // _UNICODE
    } break;
    case SQLITE_NULL: set_null(); break;
    default: set_null();
  }
}

SQLITE_WRAPPER_INLINE void db::param::set(bool dat) {
  _type = e_bool;
  _data = dat ? DB_TEXT("TRUE") : DB_TEXT("FALSE");
}

SQLITE_WRAPPER_INLINE void db::param::set(int dat) {
  _type = e_int;
  _data = boost::str(format(DB_TEXT("%d")) % dat);
}

SQLITE_WRAPPER_INLINE void db::param::set(unsigned dat) {
  _type = e_unsigned;
  _data = boost::str(format(DB_TEXT("%d")) % dat);
}

SQLITE_WRAPPER_INLINE void db::param::set(long dat) {
  _type = e_long;
  _data = boost::str(format(DB_TEXT("%ld")) % dat);
}

SQLITE_WRAPPER_INLINE void db::param::set(float dat) {
  _type = e_float;
  _data = boost::str(format(DB_TEXT("%f")) % dat);
}

SQLITE_WRAPPER_INLINE void db::param::set(double dat) {
  _type = e_double;
  _data = boost::str(format(DB_TEXT("%f")) % dat);
}

SQLITE_WRAPPER_INLINE void db::param::set(char dat) {
  _type = e_char;
  _data = dat;
}

SQLITE_WRAPPER_INLINE void db::param::set(const db::char_type * dat) {
  _type = e_string;
  _data = dat;
}

SQLITE_WRAPPER_INLINE void db::param::set(const string & dat, bool isBlob /*=false*/) {
  _type = isBlob ? e_blob : e_string;
  _data = dat;
}

#ifndef BOOST_NO_STD_LOCALE
SQLITE_WRAPPER_INLINE void db::param::set(const boost::gregorian::date & dat) {
  _type = e_date_time;
  _data = detail::to_sql_string(dat);
}
#else
SQLITE_WRAPPER_INLINE void db::param::set(const time_t_ce & dat) {
  _type = e_date_time;
  _data = detail::to_sql_string(dat);
}
#endif

SQLITE_WRAPPER_INLINE void db::param::set_null() {
  _type = e_null;
  _data = DB_TEXT("");
}

SQLITE_WRAPPER_INLINE void db::param::set_with_old_type(const string & data) {
  param_types ty(get_type());
  set(data);
  set_type(ty);
}

SQLITE_WRAPPER_INLINE void db::param::set_with_old_type(const param & data) {
  param_types ty(get_type());
  set(data.str());
  set_type(ty);
}

SQLITE_WRAPPER_INLINE void db::param::set_with_old_type(const param * data) {
  if (!data) return;
  param_types ty(get_type());
  set(data->str());
  set_type(ty);
}

SQLITE_WRAPPER_INLINE bool db::param::is_null() const {
  return _type == e_null ? true : false;
}

SQLITE_WRAPPER_INLINE bool db::param::get_bool() const {
  if (_type == e_bool)
    return _data == DB_TEXT("TRUE") ? true : false;
  else
    return false;
}

SQLITE_WRAPPER_INLINE int db::param::get_int() const {
  return detail::to_type< int >(_data);
}

SQLITE_WRAPPER_INLINE unsigned db::param::get_unsigned() const {
  return detail::to_type< unsigned >(_data);
}

SQLITE_WRAPPER_INLINE long db::param::get_long() const {
  return detail::to_type< long >(_data);
}

SQLITE_WRAPPER_INLINE float db::param::get_float() const {
  return detail::to_type< float >(_data);
}

SQLITE_WRAPPER_INLINE double db::param::get_double() const {
  return detail::to_type< double >(_data);
}

SQLITE_WRAPPER_INLINE db::char_type db::param::get_char() const {
  if (_data.length() == 0)
    return DB_TEXT(' ');
  else
    return _data[0];
}

SQLITE_WRAPPER_INLINE db::param::operator db::string() const {
  return _data;
}

SQLITE_WRAPPER_INLINE db::string & db::param::str() {
  return _data;
}

#ifdef _UNICODE
SQLITE_WRAPPER_INLINE const wchar_t * db::param::c_str() const {
  return _data.c_str();
}
#else
SQLITE_WRAPPER_INLINE const char * db::param::c_str() const {
  return _data.c_str();
}
#endif  // _UNICODE

#ifndef BOOST_NO_STD_LOCALE
SQLITE_WRAPPER_INLINE boost::gregorian::date db::param::get_date_time() const {
#ifdef _UNICODE
  std::string data = static_cast< const char * >(detail::w2a(_data.c_str()));
  return boost::date_time::parse_date< boost::gregorian::date >(data);
#else
  return boost::date_time::parse_date< boost::gregorian::date >(_data);
#endif  // _UNICODE
}
#else
SQLITE_WRAPPER_INLINE db::time_t_ce db::param::get_date_time(const time_t_ce * tmp /*= 0*/) const {
  return detail::from_sql_string(_data, tmp);
}
#endif

#ifdef _MFC_VER
SQLITE_WRAPPER_INLINE COleDateTime
  db::param::get_date_time(const COleDateTime * tmp /*= 0*/) const {
  return detail::from_sql_string(_data, tmp);
}
#endif  // _MFC_VER

SQLITE_WRAPPER_INLINE const db::string & db::param::str() const {
  return _data;
}

SQLITE_WRAPPER_INLINE const db::param & db::param::operator=(const param & v) {
  _data = v._data;
  _type = v._type;
  _is_changed = v._is_changed;
  return *this;
}

SQLITE_WRAPPER_INLINE bool db::param::operator!() const {
  return !this->is_null();
}

SQLITE_WRAPPER_INLINE bool db::param::operator==(const param & v) const {
  return (!(*this) && (!v) && _data == v._data);
}

#endif
