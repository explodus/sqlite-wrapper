/// @file include/sqlite_wrapper/impl/table.ipp
///
/// @date 28.04.2010 15:15
///
/// @author T. Schroeder (explodus@gmx.de)
///
/// @brief 
///
/// This file is part of the sqlite-wrapper project

#ifndef SQLITE_WRAPPER_TABLE_IPP_INCLUDED
#define SQLITE_WRAPPER_TABLE_IPP_INCLUDED

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
			ret = static_cast<int>(0);
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

	class are_strict_equals
		: public boost::static_visitor<bool>
	{
	public:

		template <typename T, typename U>
		bool operator()( const T &, const U & ) const
		{
			return false; // cannot compare different types
		}

		template <typename T>
		bool operator()( const T & lhs, const T & rhs ) const
		{
			return lhs == rhs;
		}

	};

} }

inline db::sel db::table::get_sel() const
{
	db::sel ret = get_sel_complete();

	ret % (db::field(DB_TEXT("id"), 0) == id<int>());

	return ret;
}

inline db::sel db::table::get_sel_complete() const
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

inline db::ins db::table::get_ins() const
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

inline db::upd db::table::get_upd() const 
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

inline db::del db::table::get_del() const
{
	db::del ret(table_name());

	ret % (db::field(DB_TEXT("id"), 0) == id<int>());

	return ret;
}

inline void db::table::get( db::base& b )
{
	db::query_ptr q(b.execute_ptr(get_sel()));
	if (q && q->size())
	{
		const db::row& r = *q->begin();
		map_type::iterator mtb(++_members.begin());

		for (db::row::const_iterator 
			  itb(++r.begin())
			, ite(r.end())
			; itb!=ite
			; ++itb, ++mtb)
			mtb->second = detail::get_variant(*itb);
	}
}

inline void db::table::get( db::base& b, vec_type& v )
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
				; ++rtb, ++mtb)
				mtb->second = detail::get_variant(*rtb);
		}
	}
}

inline void db::table::set( db::base& b )
{
	try
	{ b.execute_ptr(get_ins()); }
	catch (db::exception::base&)
	{ b.execute_ptr(get_upd()); }
}

inline void db::table::set( db::base& b, vec_type& v )
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
	catch (db::exception::base& /*e*/)
	{	
		/// todo: do somethin with this exception, rethrow or anything
		b.rollback();
	}

}

inline void db::table::erase( db::base& b )
{
	b.execute_ptr(get_del());
}

inline void db::table::create( db::base& b )
{
	db::stringstream ss;

	ss 
		<< DB_TEXT("create table if not exists ") 
		<< _table_name 
		<< DB_TEXT("(id integer primary key");

	for (
		  map_type::const_iterator itb(++_members.begin())
		, ite(_members.end())
		; itb!=ite
		; ++itb)
	{
		ss << DB_TEXT(", ") << itb->first;
		if (itb->second.type() == typeid(int))
			ss << DB_TEXT(" integer");
		else if (itb->second.type() == typeid(double))
			ss << DB_TEXT(" real");
		else if (itb->second.type() == typeid(db::string))
			ss << DB_TEXT(" text");
	}

	ss << DB_TEXT("); ");

	b.execute_ptr(ss.str());
}

inline bool db::table::operator==( const table& t ) const
{
	if (_table_name != t._table_name)
		return false;

	if (_members.size() != t._members.size())
		return false;

	for (
		  map_type::const_iterator itb(_members.begin())
		, ttb(t._members.begin())
		, ite(_members.end())
		; itb!=ite
		; ++itb, ++ttb)
	{
		if (itb->first != ttb->first)
			return false;

		if(!boost::apply_visitor(
			  detail::are_strict_equals()
			, itb->second
			, ttb->second))
			return false;
	}

	return true;
}

#endif