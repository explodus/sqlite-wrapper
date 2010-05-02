/// @file src/table.cpp
///
/// @date 28.04.2010 15:15
///
/// @author T. Schroeder (explodus@gmx.de)
///
/// @brief 
///
/// This file is part of the sqlite-wrapper project

#include <sqlite_wrapper/table.hpp>

#include <sqlite_wrapper/detail/sel.hpp>
#include <sqlite_wrapper/detail/ins.hpp>
#include <sqlite_wrapper/detail/upd.hpp>
#include <sqlite_wrapper/detail/del.hpp>

#include <sqlite_wrapper/db.hpp>

namespace db { namespace detail 
{
	template<typename T>
	class get_field_visitor : public boost::static_visitor<>
	{
		db::string _title;
		T& _expr;
	public:
		get_field_visitor(T& expr_) : _expr(expr_) 
		{ }
		
		void operator()( int operand )
		{ _expr % db::field(_title, operand); }
		
		void operator()( const db::string & operand )
		{ _expr % db::field(_title, operand); }
		
		void operator()( const double & operand )
		{ _expr % db::field(_title, operand); }

		void operator()( const db::table::map_type::value_type & operand )
		{ _title = operand.first; boost::apply_visitor(*this, operand.second); }
	};

} }

db::sel db::table::get_sel()
{
	db::sel ret(table_name());

	for (
		  map_type::const_iterator itb(_members.begin())
		, ite(_members.end())
		; itb!=ite
		; ++itb)
		ret.result(itb->first);

	return ret;
}

db::ins db::table::get_ins()
{
	db::ins ret(table_name());

	return ret;
}

db::upd db::table::get_upd()
{
	db::upd ret(table_name());

	ret % (db::field(DB_TEXT("id"), 0) == id<int>());

	detail::get_field_visitor<db::upd> visitor(ret);
	for (db::table::map_type::const_iterator 
			itb(_members.begin())
		, ite(_members.end())
		; itb!=ite
		; ++itb)
		visitor(*itb);
	
	return ret;
}

db::del db::table::get_del()
{
	db::del ret(table_name());

	ret % (db::field(DB_TEXT("id"), 0) == id<int>());

	return ret;
}

void db::table::get( db::base& b, table& t )
{

}

void db::table::get( db::base& b, vec_type& v )
{

}

void db::table::set( db::base& b, table& t )
{

}

void db::table::set( db::base& b, vec_type& v )
{

}

void db::table::erase( db::base& b, table& t )
{

}

void db::table::create( db::base& b )
{

}