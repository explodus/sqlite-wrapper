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
		
		inline void operator()( int operand )
		{ _expr % db::field(_title, operand); }
		
		inline void operator()( const db::string & operand )
		{ _expr % db::field(_title, operand); }
		
		inline void operator()( const double & operand )
		{ _expr % db::field(_title, operand); }

		inline void operator()( const db::table::map_type::value_type & operand )
		{ _title = operand.first; boost::apply_visitor(*this, operand.second); }
	};

	inline db::table::value_type get_variant(const db::param& p)
	{
		db::table::value_type ret;

		switch (p.get_type())
		{
		case db::e_null:
			break;
		case db::e_bool:
		case db::e_int:
		case db::e_unsigned:
		case db::e_long:
			ret = db::detail::to_type<int>(p.str());
			break;
		case db::e_float:
		case db::e_double:
		case db::e_date_time:
			ret = db::detail::to_type<double>(p.str());
			break;
		default:
			ret = p.str();
			break;
		}

		return ret;
	}

} }

db::sel db::table::get_sel()
{
	db::sel ret = get_sel_complete();

	ret % (db::field(DB_TEXT("id"), 0) == id<int>());

	return ret;
}

db::sel db::table::get_sel_complete()
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
	detail::get_field_visitor<db::ins> visitor(ret);

	for (db::table::map_type::const_iterator 
		  itb(_members.begin())
		, ite(_members.end())
		; itb!=ite
		; ++itb)
		visitor(*itb);

	return ret;
}

db::upd db::table::get_upd()
{
	db::upd ret(table_name());
	detail::get_field_visitor<db::upd> visitor(ret);

	ret % (db::field(DB_TEXT("id"), 0) == id<int>());

	for (db::table::map_type::const_iterator 
		  itb(boost::next(_members.begin()))
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

void db::table::get( db::base& b )
{
	db::query_ptr q(b.execute_ptr(get_sel()));
	if (q && q->size())
	{
		const db::row& r = *q->begin();
		db::row::const_iterator itb(++r.begin()), ite(r.end());
		map_type::iterator mtb(++_members.begin());

		for (; itb!=ite; ++itb)
			mtb->second = detail::get_variant(*itb);
	}
}

void db::table::get( db::base& b, vec_type& v )
{
	db::query_ptr q(b.execute_ptr(get_sel_complete()));
	if (q && q->size())
	{
		v.resize(q->size(), *this);
		vec_type::iterator vtp(v.begin());
		for (
			  db::query::const_iterator qtb(q->begin())
			, qte(q->end())
			; qtb!=qte
			; ++qtb
			, ++vtp)
		{
			map_type::iterator mtb(vtp->_members.begin());

			for (
				  db::row::const_iterator rtb(qtb->begin())
				, rte(qtb->end())
				; rtb!=rte
				; ++rtb)
				mtb->second = detail::get_variant(*rtb);
		}
	}
}

void db::table::set( db::base& b )
{
	/// todo: insert or update or both?
	b.execute_ptr(get_upd());
}

void db::table::set( db::base& b, vec_type& v )
{
	try
	{
		b.begin();

		for (
			  vec_type::const_iterator itb(v.begin())
			, ite(v.end())
			; itb!=ite
			; ++itb)
			b.execute_ptr(itb->get_upd());

		b.commit();
	}
	catch (db::exception& e)
	{	
		/// todo: do somethin with this exception, rethrow or anything
		b.rollback();
	}

}

void db::table::erase( db::base& b )
{
	b.execute_ptr(get_del());
}

void db::table::create( db::base& b )
{

}