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

#ifndef DB_EXPR_HPP_INCLUDED
#define DB_EXPR_HPP_INCLUDED

#include <sqlite_wrapper/config.hpp>
#include <sqlite_wrapper/detail/tools.hpp>
#include <sqlite_wrapper/detail/field.hpp>

namespace db
{
	class field;

	namespace expr
	{
		///expression base class
		class base 
		{
		protected:
			split extraTables; //!< extra tables, to be joined
		public:

			/// @brief     default expression is true
			///
			/// <BR>qualifier const
			/// <BR>access    virtual public  
			/// @return    db::string
			///
			/// @date      18:2:2009   11:16
			///
			virtual string str() const;

			/// @brief     string operator 
			///
			/// <BR>qualifier const
			/// <BR>access    virtual public  
			/// @return    
			///
			///
			virtual operator string() const;

			/// <BR>qualifier const
			/// <BR>access    public  
			/// @return    db::split
			///
			/// @date      18:2:2009   11:16
			///
			split getExtraTables() const;

			/// @brief     default virtual destructor
			///
			/// <BR>qualifier
			/// <BR>access    virtual public  
			/// @return    
			///
			/// @date      18:2:2009   11:17
			///
			virtual ~base();
		}; 

		class raw : public base 
		{
			string expr;
		public:
			/// @brief     usable for special expressions in WHERE terms
			///
			/// <BR>qualifier : expr(e)
			/// <BR>access    public  
			/// @return    
			/// @param     e as const string &
			///
			/// @date      18:2:2009   11:17
			///
			explicit raw(const string& e);

			virtual string str() const;
		};

		class connective : public base 
		{
		private:
			string op;
		protected:
			const base &e1, &e2;

			/// @brief     base class to connect 2 expressions
			///
			/// <BR>qualifier : op(o), e1(e1_), e2(e2_)
			/// <BR>access    protected  
			/// @return    
			/// @param     string o
			/// @param     e1_ as const base &
			/// @param     e2_ as const base &
			///
			/// @date      18:2:2009   11:20
			///
			explicit connective(string o, const base & e1_, const base & e2_);

		public:        

			/// @brief     str
			///
			/// <BR>qualifier const
			/// <BR>access    virtual public  
			/// @return    db::string
			///
			/// @date      20:2:2009   12:55
			///
			virtual string str() const;
		};  

		class and_ : public connective 
		{
		public:
			/// @brief     connects 2 expressions with the AND operator
			///
			/// <BR>qualifier : connective(DB_TEXT("and"), e1_, e2_)
			/// <BR>access    public  
			/// @return    
			/// @param     e1_ as const base &
			/// @param     e2_ as const base &
			///
			/// @date      18:2:2009   11:22
			///
			explicit and_(const base & e1_, const base & e2_);

			/// @brief     str
			///
			/// <BR>qualifier const
			/// <BR>access    public  
			/// @return    db::string
			///
			/// @date      20:2:2009   12:56
			///
			virtual string str() const;
		};

		class or_ : public connective 
		{
		public:
			/// @brief     connects 2 expressions with the OR operator
			///
			/// <BR>qualifier : connective(DB_TEXT("or"), e1_, e2_)
			/// <BR>access    public  
			/// @return    
			/// @param     e1_ as const base &
			/// @param     e2_ as const base &
			///
			/// @date      18:2:2009   11:22
			///
			explicit or_(const base & e1_, const base & e2_);

			/// <BR>qualifier const
			/// <BR>access    public  
			/// @return    db::string
			///
			/// @date      20:2:2009   12:56
			///
			virtual string str() const;
		};

		class not_ : public base 
		{
		private:
			const base & exp;
		public:
			/// @brief     negates expression
			///
			/// <BR>qualifier : exp(_exp)
			/// <BR>access    public  
			/// @return    
			/// @param     _exp as const base &
			///
			/// @date      18:2:2009   11:23
			///
			explicit not_(const base & _exp);

			/// <BR>qualifier const
			/// <BR>access    virtual public  
			/// @return    db::string
			///
			/// @date      20:2:2009   12:57
			///
			virtual string str() const;

		};

		///base class for operators in sql terms
		class oper : public base 
		{
		protected:
			const field & _field;
			string op;
			string data;
			bool escape;

			/// @brief as an example, e_string got escaped
			///
			/// <BR>qualifier
			/// <BR>access    protected  
			/// @return    bool
			/// @param     type_ as const param_types &
			///
			/// @date      18:2:2009   11:26
			///
			bool check_escape(const param_types& type_);

			/// @brief     string overload
			///
			/// <BR>qualifier : _field(fld), op(o), data(d), escape(check_escape(_field.type()))
			/// <BR>access    protected  
			/// @return    
			/// @param     fld as const field & 
			/// @param     o as const string &
			/// @param     d as const string &
			///
			/// @date      18:2:2009   11:28
			///
			explicit oper(const field & fld, const string& o, const string& d);

			/// <BR>qualifier : _field(fld)*/, op(o)*/, data(DB_TEXT("0"))*/, escape(check_escape(_field.type()))
			/// <BR>access    protected 
			/// 
			/// @return       
			/// @param        fld as const field &
			/// @param        o as const string &
			/// @param        d as const T &
			///
			/// @date         4.4.2010 21:16
			/// 
			template<typename T>
			explicit oper(const field & fld, const string& o, const T& d) 
				: _field(fld), op(o), data(DB_TEXT("0")), 
				escape(check_escape(_field.type())) 
			{
				data = detail::to_string(d);
				extraTables.push_back(fld.table());
			}

			/// @brief     db::field overload
			///
			/// <BR>qualifier : _field(fld), op(o), data(f2.fullName()), escape(false)
			/// <BR>access    protected  
			/// @return    
			/// @param     fld as const field &
			/// @param     o as const string &
			/// @param     f2 as const field &
			///
			/// @date      18:2:2009   11:30
			///
			explicit oper(const field & fld, const string& o, const field &f2);

		public:
			/// <BR>qualifier const
			/// <BR>access    virtual public  
			/// @return    db::string
			///
			/// @date      20:2:2009   13:03
			///
			virtual string str() const;
		};

		///==, is equal operator
		class eq : public oper {
		public:
			/// @brief     ==, is equal operator
			///
			/// <BR>qualifier : oper<typename T>(fld, DB_TEXT("="), d)
			/// <BR>access    public  
			/// @return    
			/// @param     fld as const field &
			/// @param     d as const T &
			///
			/// @date      20:2:2009   11:52
			///
			template<typename T>
			explicit eq(const field & fld, const T& d)
				: oper(fld, DB_TEXT("="), d) 
			{}
		};

		///<> not equal operator
		class not_eq_ : public oper {
		public:
			/// @brief     !=, not equal constructor
			///
			/// <BR>qualifier : oper(fld, DB_TEXT("<>"), d)
			/// <BR>access    public  
			/// @return    
			/// @param     fld as const field &
			/// @param     d as const T &
			///
			/// @date      20:2:2009   13:03
			///
			template<typename T>
			explicit not_eq_(const field & fld, const T& d)
				: oper(fld, DB_TEXT("<>"), d) 
			{}
		};

		///> greater then operator
		class gt : public oper {
		public:
			/// @brief     gt
			///
			/// <BR>qualifier : oper(fld, DB_TEXT(">"), d)
			/// <BR>access    public  
			/// @return    
			/// @param     fld as const field &
			/// @param     d as const T &
			///
			/// @date      20:2:2009   13:02
			///
			template<typename T>
			explicit gt(const field & fld, const T& d)
				: oper(fld, DB_TEXT(">"), d) 
			{}
		};

		///greater then or equal operator
		class gt_eq : public oper {
		public:
			/// @brief     gt_eq
			///
			/// <BR>qualifier : oper(fld, DB_TEXT(">="), d)
			/// <BR>access    public  
			/// @return    
			/// @param     fld as const field &
			/// @param     d as const T &
			///
			/// @date      20:2:2009   13:01
			///
			template<typename T>
			explicit gt_eq(const field & fld, const T& d)
				: oper(fld, DB_TEXT(">="), d) 
			{}
		};

		///lower then operator
		class lt : public oper {
		public:
			/// @brief     <, lower then constructor
			///
			/// <BR>qualifier : oper(fld, DB_TEXT("<"), d)
			/// <BR>access    public  
			/// @return    
			/// @param     fld as const field &
			/// @param     d as const T &
			///
			/// @date      20:2:2009   13:01
			///
			template<typename T>
			explicit lt(const field & fld, const T& d)
				: oper(fld, DB_TEXT("<"), d) 
			{}
		};

		///lower then or equal operator
		class lt_eq : public oper {
		public:
			/// @brief     <=, lower then or equal constructor
			///
			/// <BR>qualifier : oper(fld, DB_TEXT("<="), d)
			/// <BR>access    public  
			/// @return    
			/// @param     fld as const field &
			/// @param     d as const T &
			///
			/// @date      20:2:2009   13:01
			///
			template<typename T>
			explicit lt_eq(const field & fld, const T& d)
				: oper(fld, DB_TEXT("<="), d) 
			{}
		};

		///like operator
		class like : public oper {
		public:
			/// @brief     like constructor
			///
			/// <BR>qualifier : oper(fld, DB_TEXT("LIKE"), d)
			/// <BR>access    public  
			/// @return    
			/// @param     fld as const field &
			/// @param     d as const string &
			///
			/// @date      20:2:2009   13:00
			///
			explicit like(const field & fld, const string& d);
		};

		///in Operator
		class in : public oper {
		public:
			/// @brief     in constructor
			///
			/// <BR>qualifier : oper(fld, DB_TEXT("IN"), DB_TEXT("(")+set+DB_TEXT(")"))
			/// <BR>access    public  
			/// @return    
			/// @param     fld as const field &
			/// @param     set as const string &
			///
			/// @date      20:2:2009   13:01
			///
			explicit in(const field & fld, const string& set);

			/// @brief     in constructor
			///
			/// <BR>qualifier
			/// <BR>access    public  
			/// @return    
			/// @param     fld as const field &
			/// @param     s as const sel &
			///
			/// @date      20:2:2009   13:01
			///
			explicit in(const field & fld, const sel& s);

			/// <BR>qualifier const
			/// <BR>access    virtual public  
			/// @return    db::string
			///
			/// @date      20:2:2009   13:01
			///
			virtual string str() const;

		};
	}
}

/// @brief     overload from the global == operator
///
/// <BR>qualifier
/// <BR>access    public  
/// @return    db::expr::eq
/// @param     fld as const db::field &
/// @param     f2 as const T &
///
/// @date      20:2:2009   14:17
///
template<typename T>
inline db::expr::eq operator==(const db::field& fld, const T& f2)
{ return db::expr::eq(fld, f2); }

/// @brief     overload from the global > operator
///
/// <BR>qualifier
/// <BR>access    public  
/// @return    db::expr::gt
/// @param     fld as const db::field &
/// @param     o2 as const T &
///
/// @date      20:2:2009   14:17
///
template<typename T>
inline db::expr::gt operator>(const db::field& fld, const T& o2)
{ return db::expr::gt(fld, o2); }

/// @brief     overload from the global >= operator
///
/// <BR>qualifier
/// <BR>access    public  
/// @return    db::expr::gt_eq
/// @param     fld as const db::field &
/// @param     o2 as const T &
///
/// @date      20:2:2009   14:17
///
template<typename T>
inline db::expr::gt_eq operator>=(const db::field& fld, const T& o2)
{ return db::expr::gt_eq(fld, o2); }

/// @brief     overload from the global < operator
///
/// <BR>qualifier
/// <BR>access    public  
/// @return    db::expr::lt
/// @param     fld as const db::field &
/// @param     o2 as const T &
///
/// @date      20:2:2009   14:21
///
template<typename T>
inline db::expr::lt operator<(const db::field& fld, const T& o2)
{ return db::expr::lt(fld, o2); }

/// @brief     overload from the global <= operator
///
/// <BR>qualifier
/// <BR>access    public  
/// @return    db::expr::lt_eq
/// @param     fld as const db::field &
/// @param     o2 as const T &
///
/// @date      20:2:2009   14:22
///
template<typename T>
inline db::expr::lt_eq operator<=(const db::field& fld, const T& o2)
{ return db::expr::lt_eq(fld, o2); }

/// @brief     overload from the global != operator
///
/// <BR>qualifier
/// <BR>access    public  
/// @return    db::expr::not_eq_
/// @param     fld as const db::field &
/// @param     f2 as const T &
///
/// @date      20:2:2009   14:22
///
template<typename T>
inline db::expr::not_eq_ operator!=(const db::field& fld, const T& f2)
{ return db::expr::not_eq_(fld, f2); }

#endif