///	@file		include/sqlite_wrapper/impl/split.ipp
///	@date		15.3.2010 14:23
///	file base	split
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

#ifndef SQLITE_WRAPPER_SPLIT_IPP_INCLUDED
#define SQLITE_WRAPPER_SPLIT_IPP_INCLUDED

SQLITE_WRAPPER_INLINE db::split_map::split_map() {
}

SQLITE_WRAPPER_INLINE db::split_map::split_map(const std::map< db::string, db::string > & data)
  : std::map< db::string, db::string >(data) {
}

SQLITE_WRAPPER_INLINE db::string db::split_map::join_fields(const db::string & delim) const {
  string res;
  for (const_iterator i = begin(); i != end(); i++) {
    if (i != begin()) res += delim;
    res += i->first;
  }
  return res;
}

SQLITE_WRAPPER_INLINE db::string db::split_map::join_values(const db::string & delim) const {
  string res;
  for (const_iterator i = begin(); i != end(); i++) {
    if (i != begin()) res += delim;
    if (i->second.length() != 0)
      res += i->second;
    else
      res += DB_TEXT("NULL");
  }
  return res;
}

SQLITE_WRAPPER_INLINE db::split::split() {
}

SQLITE_WRAPPER_INLINE db::split::split(const std::vector< db::string > & data)
  : std::vector< db::string >(data) {
}

SQLITE_WRAPPER_INLINE db::split::split(const db::string & s, const db::string & delim) {
  if (s.length() == 0) return;
  db::string tmp(s);
  db::char_type * ptr(&*tmp.begin());
  int len(delim.length());
  std::vector< db::char_type * > pointers;
  pointers.push_back(ptr);

  ptr = _tcsstr(ptr, delim.c_str());

  while (ptr) {
    *ptr = DB_TEXT('\0');
    ptr += len;
    pointers.push_back(ptr);
    (ptr = _tcsstr(ptr, delim.c_str()));
  }

  for (std::vector< db::char_type * >::iterator i(pointers.begin()), e(pointers.end()); i != e; ++i)
    push_back(string(*i));
}

SQLITE_WRAPPER_INLINE db::split db::split::slice(int start, int end) const {
  std::vector< string > data;
  int size_ = static_cast< int >(size());

  if (start < 0) start = start + size_;
  if (end < 0) end = end + size_;
  if (start >= size_) start = size_ - 1;
  if (end >= size_) end = size_ - 1;
  if (start >= end) return data;

  for (int i = start; i < end; ++i) data.push_back(this->operator[](i));
  return data;
}

SQLITE_WRAPPER_INLINE db::string db::split::join(const db::string & delim) const {
  string res;
  for (const_iterator i = begin(); i != end(); i++) {
    if (i != begin()) res += delim;
    res += *i;
  }
  return res;
}

SQLITE_WRAPPER_INLINE db::split & db::split::extend(const db::split & s) {
  for (size_t i = 0; i < s.size(); i++) push_back(s[i]);
  return *this;
}

#endif
