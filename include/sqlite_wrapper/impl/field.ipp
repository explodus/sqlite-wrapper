///	@file		include/sqlite_wrapper/impl/field.ipp
///	@date		15.3.2010 14:23
///	file base	field
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

#ifndef SQLITE_WRAPPER_FIELD_IPP_INCLUDED
#define SQLITE_WRAPPER_FIELD_IPP_INCLUDED

SQLITE_WRAPPER_INLINE db::field::field(const string & n, const param_types & t /*=e_long*/)
  : _name(n)
  , _type(t)
  , _extra(e_nullExtra)
  , _length(-1) {
}

SQLITE_WRAPPER_INLINE
  db::field::field(const string_pair & n_tbl, const param_types & t, const Values & vals)
  : _name(n_tbl.first)
  , _table(n_tbl.second)
  , _type(t)
  , _extra(e_nullExtra)
  , _values(vals)
  , _length(-1) {
}

SQLITE_WRAPPER_INLINE db::field::field(const field & f)
  : _name(f._name)
  , _table(f._table)
  , _type(f._type)
  , _extra(f._extra)
  , _values(f._values)
  , _length(f._length) {
}

SQLITE_WRAPPER_INLINE db::field::field()
  : _type(e_long)
  , _extra(e_nullExtra)
  , _length(-1) {
}

SQLITE_WRAPPER_INLINE db::string db::field::fullName() const {
  if (table().length() > 0)
    return table() + DB_TEXT(".") + name();
  else
    return name();
}

SQLITE_WRAPPER_INLINE db::string db::field::name() const {
  return _name;
}

SQLITE_WRAPPER_INLINE db::string db::field::sourcename() const {
  return _sourcename;
}

SQLITE_WRAPPER_INLINE db::param_types db::field::type() const {
  return _type;
}

SQLITE_WRAPPER_INLINE db::string db::field::table() const {
  return _table;
}

SQLITE_WRAPPER_INLINE const long & db::field::length() const {
  return _length;
}

SQLITE_WRAPPER_INLINE db::field & db::field::set_type(param_types type) {
  _type = type;
  return *this;
}

SQLITE_WRAPPER_INLINE void db::field::set_table(const string & table) {
  _table = table;
}

SQLITE_WRAPPER_INLINE void db::field::set_sourcename(const string & sourcename) {
  _sourcename = sourcename;
}

SQLITE_WRAPPER_INLINE void db::field::set_length(const long & len) {
  _length = len;
}

SQLITE_WRAPPER_INLINE bool db::field::has_extra(field_extra extra) const {
  return ((_extra & extra) == 1);
}

SQLITE_WRAPPER_INLINE void db::field::add_extra(field_extra extra) {
  if (!(_extra & extra)) _extra |= extra;
}

SQLITE_WRAPPER_INLINE void db::field::remove_extra(field_extra extra) {
  if (_extra & extra) _extra &= ~extra;
}

SQLITE_WRAPPER_INLINE db::field::Values & db::field::values() {
  return _values;
}

SQLITE_WRAPPER_INLINE const db::field::Values & db::field::values() const {
  return _values;
}

SQLITE_WRAPPER_INLINE bool db::field::operator==(const field & fd) const {
  return fd.fullName() == fullName();
}

SQLITE_WRAPPER_INLINE bool db::field::operator!=(const field & fd) const {
  return !(*this == fd);
}

SQLITE_WRAPPER_INLINE db::expr::in db::field::In(const string & set) const {
  return expr::in(*this, set);
}

SQLITE_WRAPPER_INLINE db::expr::in db::field::In(const sel & sel) const {
  return expr::in(*this, sel);
}

SQLITE_WRAPPER_INLINE db::expr::like db::field::Like(const string & s) {
  return expr::like(*this, s);
}

SQLITE_WRAPPER_INLINE db::field::~field() {
}

#endif
