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
#include <map>

namespace db
{
	class sel;
	
	///	@brief a base wrapper class for a database table
	class SQLITE_WRAPPER_DLLAPI table
	{
	public:
		typedef boost::variant<int, db::string, double> value_type;
		typedef std::map<db::string, value_type> map_type;

		table(const db::string& table_name) : _table_name(table_name) {}
		virtual ~table() 
		{ }

		const db::string& table_name() const { return _table_name; }

		virtual db::sel get_sel();
	protected:
		map_type _members;

	private:
		db::string _table_name;
	};

///
#	define TABLE_BEGIN(name) \
	name##() : db::table(DB_TEXT(#name)) \
	{ 

///
#	define TABLE_MEMBER(name) \
		_members.insert(db::table::map_type::value_type( \
			  DB_TEXT(#name) \
			, value_type()));

///
#	define TABLE_END() }

///
#	define TABLE_MEMBER_GET_SET(name) \
	public: \
		template<typename T> \
		inline const typename T & name##() const \
		{ \
			static T empty_; \
			db::table::map_type::const_iterator it(_members.find(DB_TEXT(#name))); \
			return (it != _members.end()) ? boost::get<T>(it->second) : empty_; \
		} \
		inline const db::table::value_type & name##() const \
		{ \
			static db::table::value_type empty_; \
			db::table::map_type::const_iterator it(_members.find(DB_TEXT(#name))); \
			return (it != _members.end()) ? it->second : empty_; \
		} \
		inline void name##(const db::table::value_type & val) \
		{ \
			db::table::map_type::iterator it(_members.find(DB_TEXT(#name))); \
			if (it != _members.end()) \
				it->second = val; \
		}
}

/// @page usage examples of the wrapper
/// @ingroup sqlite_wrapper_examples
///
/// an usage example of the table wrapper:
///
///@code
///
/// class gps : public db::table
/// {
/// public:
///
/// 	TABLE_BEGIN(gps)
/// 		TABLE_MEMBER(id)
/// 		TABLE_MEMBER(longitude)
/// 		TABLE_MEMBER(latitude)
/// 	TABLE_END()
///
/// 	TABLE_MEMBER_GET_SET(id)
/// 	TABLE_MEMBER_GET_SET(longitude)
/// 	TABLE_MEMBER_GET_SET(latitude)
/// };
///
/// @endcode
///

#endif
