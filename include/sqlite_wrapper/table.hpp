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
	
	typedef boost::variant<int, db::string, double> types;

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
		template<typename T> \
		inline const typename T & get_##name() const \
		{ return boost::get<T>(_##name); } \
		inline const db::types & get_##name() const \
		{ return _##name; } \
		inline void set_##name(const db::types& val) \
		{ _##name = val; } \
	private: \
		db::types _##name

}
 
#endif
