///	@file		include/sqlite_wrapper/detail/field.hpp
///	@date		15.3.2010 8:45
///	file base	tools
///	file ext	hpp
///
/// @author		T. Schroeder (explodus@gmx.de)
///
/// This software is released under the BSD Licence
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
///	@brief 

#ifndef DB_FIELD_HPP_INCLUDED
#define DB_FIELD_HPP_INCLUDED

#include <sqlite_wrapper/config.hpp>
#include <sqlite_wrapper/detail/param.hpp>

namespace db
{
	namespace expr
	{
		class base;
		class in;
		class like;
	}
	class sel;
	class del;
	class ins;
	class upd;

	/// fieldtype interface
	class SQLITE_WRAPPER_DLLAPI field 
	{
	public:
		typedef std::vector< std::pair<string, string> > Values;

	protected:
		string _name, _table, _sourcename;
		param_types _type;
		unsigned _extra;
		Values _values;
		long _length;

	public:
		/// @brief     field
		/// @todo need documentation
		///
		/// <BR>qualifier : _name(n), _type(t), _extra(e_nullExtra), _length(-1)
		/// <BR>access    public  
		/// @return    
		/// @param     n as const string & 
		/// @param     t as const param_types & 
		///
		/// @date      20:2:2009   11:03
		///
		explicit field(const string& n, 
			const param_types& t=e_long);

		/// @brief     field
		///
		/// <BR>qualifier : _name(n), _type(tp(val)), _extra(e_nullExtra), _length(-1)
		/// <BR>access    public  
		/// @return    
		/// @param     n as const string & 
		/// @param     val as const T & 
		///
		/// @date      20:2:2009   11:03
		///
		template<typename T>
		explicit field(const string& n, const T& val)
			: _name(n), _type(tp(val)), _extra(e_nullExtra), _length(-1) 
		{
			_values.push_back(Values::value_type(n, detail::to_string(val))); 
		}

		/// @brief     field
		///
		/// <BR>qualifier : _name(n_tbl.first), _type(t), _table(n_tbl.second), 
		///              _values(vals), _extra(e_nullExtra), _length(-1)
		/// <BR>access    public  
		/// @return    
		/// @param     n_tbl as const string_pair & 
		/// @param     t as const param_types & 
		/// @param     vals as const Values & 
		///
		/// @date      6:8:2009   12:57
		///
		explicit field(const string_pair& n_tbl,
			const param_types& t=e_long, 
			const Values& vals = Values());

		/// @brief     field
		///
		/// <BR>qualifier : _name(f._name), _type(f._type), _table(f._table), 
		///              _values(f._values), _extra(f._extra), _length(f._length)
		/// <BR>access    public  
		/// @return    
		/// @param     f as const field &
		///
		/// @date      6:8:2009   10:56
		///
		field(const field& f);

		/// @brief     field
		///
		/// <BR>qualifier : _type(e_long), _extra(e_nullExtra), _length(-1)
		/// <BR>access    public  
		/// @return    
		///
		/// @date      6:8:2009   11:30
		///
		field();

		string fullName() const;

		~field();

		string      name()        const;
		string      sourcename()  const;
		param_types type()        const;
		string      table()       const;
		const long& length()      const;

		field&      set_type(param_types type);
		void        set_table(const string& table);
		void        set_sourcename(const string& sourcename);
		void        set_length(const long& len);
		bool        has_extra(field_extra extra) const;
		void        add_extra(field_extra extra);
		void        remove_extra(field_extra extra);

		/// @brief     values
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return    Values&
		///
		/// @date      20:2:2009   14:26
		///
		Values& values();

		/// @brief     values
		///
		/// <BR>qualifier const
		/// <BR>access    public  
		/// @return    const Values&
		///
		/// @date      20:2:2009   14:26
		///
		const Values& values() const;

		/// @brief     syntactic sugar to expression-api, field.in(sel)
		///
		/// <BR>qualifier const
		/// <BR>access    public  
		/// @return    db::expr::in
		/// @param     set as const string &
		///
		/// @date      20:2:2009   14:24
		///
		db::expr::in In(const string& set) const;

		/// @brief     syntactic sugar to expression-api, field.in(sel)
		///
		/// <BR>qualifier const
		/// <BR>access    public  
		/// @return    db::expr::in
		/// @param     sel as const sel &
		///
		/// @date      20:2:2009   14:23
		///
		db::expr::in In(const sel& sel) const;

		/// @brief     syntactic sugar to expression-api, field.like(string)
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return    db::expr::like
		/// @param     s as const string &
		///
		/// @date      20:2:2009   14:24
		///
		db::expr::like Like(const string& s);

		bool operator==(const field & fd) const;
		bool operator!=(const field & fd) const;
	};

	//typedef boost::detail::allocator::partial_std_allocator_wrapper<
	//	boost::detail::quick_allocator<field > > alloc_field; 
	typedef std::allocator<field> alloc_field; 
	typedef std::pair<field, field> field_pair;
}

#endif