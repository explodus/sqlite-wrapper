/// @file cout.hpp
///
/// @date 10.03.2010 09:36
///
/// @author T. Schröder (explodus@gmx.de)
///
/// @brief
///
/// This file is part of the sqlite-wrapper project

#ifndef SQLITE_WRAPPER_COUT_HPP_INCLUDED
#define SQLITE_WRAPPER_COUT_HPP_INCLUDED

#include <sqlite_wrapper/config.hpp>

namespace db {
/// defintion of db::cout
// extern ostream& cout;
static db::ostream & cout(
#ifdef SQLITE_WRAPPER_NARROW_STRING
  std::cout
#else
  std::wcout
#endif
);

}  // namespace db

#endif
