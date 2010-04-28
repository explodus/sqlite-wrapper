/// @file table.hpp
///
/// @date 28.04.2010 09:36
///
/// @author T. Schröder (explodus@gmx.de)
///
/// @brief 
///
/// This file is part of the sqlite-wrapper project
 
#ifndef SQLITE_WRAPPER_TABLE_HPP_INCLUDED
#define SQLITE_WRAPPER_TABLE_HPP_INCLUDED

#include <sqlite_wrapper/config.hpp>
#include <boost/variant.hpp>

namespace db
{
	class sel;
	
	typedef boost::variant<db::string, int, double> types;

	///
	class SQLITE_WRAPPER_DLLAPI table
	{
		db::string _table_name;

	public:
		table(const db::string table_name) : _table_name(table_name) {}
		virtual ~table() 
		{ }

		const db::string& table_name() const { return _table_name; }

		virtual db::sel get_sel();
	};

#	define TABLE_MEMBER(name) \
	public: \
		inline const db::types& ##name() const \
		{ return _##name; } \
		inline void ##name(const db::types& val) \
		{ _##name = val; } \
	private: \
		db::types _##name

}
 
#endif
