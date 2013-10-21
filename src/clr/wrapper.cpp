#include <sqlite_wrapper/config.hpp>
#include <sqlite_wrapper/db.hpp>
#include <tchar.h>

#define NATIVEDLL_EXPORTS
#include <vcclr.h>
#include <msclr\marshal.h>
#include <msclr\marshal_cppstd.h>

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Collections::ObjectModel;
using namespace System::Data::SqlTypes;
using namespace Runtime::InteropServices;
using namespace msclr::interop;

namespace clr_db 
{
	public enum class param_types : unsigned short
	{
		e_null = 0/*= db::e_null*/,       //!< \a null type
		e_bool /*= db::e_bool*/,       //!< \a boolean type
		e_int /*= db::e_int*/,        //!< \a integer type
		e_unsigned /*= db::e_unsigned*/,   //!< \a unsigned integer type
		e_long /*= db::e_long*/,       //!< \a long type
		e_float /*= db::e_float*/,      //!< \a float type
		e_double /*= db::e_double*/,     //!< \a double type
		e_char /*= db::e_char*/,       //!< \a char type
		e_string /*= db::e_string*/,     //!< \a std::string type
		e_blob /*= db::e_blob*/,       //!< \a blob type (big string)
		e_date_time /*= db::e_date_time*/   //!< \a boost::gregorian::date type
	};

	inline param_types tp(Boolean)
	{ return param_types::e_bool; }
	inline param_types tp(Int16)
	{ return param_types::e_int; }
	inline param_types tp(Int32)
	{ return param_types::e_int; }
	inline param_types tp(const long&)
	{ return param_types::e_long; }
	inline param_types tp(const float&)
	{ return param_types::e_float; }
	inline param_types tp(Double)
	{ return param_types::e_double; }
	inline param_types tp(const char&)
	{ return param_types::e_char; }
	inline param_types tp(String^, bool blob=false)
	{ return blob?param_types::e_blob:param_types::e_string; }
	inline param_types tp(DateTime^)
	{ return param_types::e_date_time; } 

	typedef KeyValuePair<String^, String^> StringPair;
	typedef List< StringPair > Values;

	/// splitmap basis
	public ref class split_map : public Dictionary<String^, String^>
	{
	public:
		/// @brief        empty splitmap
		///
		/// <BR>qualifier
		/// <BR>access    public 
		/// 
		/// @date         2.5.2010 20:11
		/// 
		split_map() {}

		/// @brief        returns text with added delimiter
		///
		/// <BR>qualifier 
		/// <BR>access    public 
		/// 
		/// @return       String^
		/// @param        delim as String^
		///
		/// @date         2.5.2010 20:10
		/// 
		String^ join_fields(String^ delim)
		{
			String^ res = gcnew String("");
			unsigned cnt(0);
			for each (StringPair^ kv in *this)
			{
				if (cnt != 0)
					res += delim;
				++cnt;
				res += kv->Key;
			}

			return res;
		}

		/// @brief        returns text with added delimiter
		///
		/// <BR>qualifier 
		/// <BR>access    public 
		/// 
		/// @return       String^
		/// @param        delim as String^
		///
		/// @date         2.5.2010 20:10
		/// 
		String^ join_values(String^ delim) 
		{
			String^ res = gcnew String("");
			unsigned cnt(0);
			for each (KeyValuePair<String^,String^>^ kv in *this)
			{
				if (cnt != 0)
					res += delim;
				++cnt;
				res += kv->Value;
			}
			return res;
		}
	};

	///split base
	public ref class split : public List<String^>
	{
	public:
		/// @brief        empty split
		///
		/// <BR>qualifier
		/// <BR>access    public 
		/// 
		/// @date         2.5.2010 19:46
		/// 
		split()
		{

		}

		/// @brief        from string vector
		///
		/// <BR>qualifier
		/// <BR>access    public 
		/// 
		/// @param        data as const std::vector<string>&
		///
		/// @date         2.5.2010 19:46
		/// 
		split(List<String^> data)
		{
			Clear();
			this->Capacity = data.Capacity;
			for each (String^ i in data)
			{
				Add(i);
			}
		}

		/// @brief        from string. split to parts using delimeter
		///
		/// <BR>qualifier
		/// <BR>access    public 
		/// 
		/// @param        s as const string&
		/// @param        delim as const string&
		///
		/// @date         2.5.2010 19:45
		/// @ingroup			constructors
		/// 
		split(String^ s_, String^ delim_/*=DB_TEXT(" ")*/)
		{
			if (s_->Length == 0)
				return;

			std::wstring tmp = marshal_as<std::wstring>(s_);
			std::wstring tmpdelim = marshal_as<std::wstring>(delim_);

			db::char_type *ptr(&*tmp.begin());
			int len(tmpdelim.length());
			std::vector<db::char_type*> pointers;
			std::vector<db::string> data;
			pointers.push_back(ptr);

			ptr = _tcsstr(ptr, tmpdelim.c_str());

			while(ptr) 
			{
				*ptr = DB_TEXT('\0');
				ptr += len;
				pointers.push_back(ptr);
				(ptr = _tcsstr(ptr, tmpdelim.c_str()));
			}

			for (std::vector<db::char_type*>::const_iterator 
				i(pointers.begin())
				, e(pointers.end())
				; i != e
				; ++i)
				data.push_back(db::string(*i));

			for (std::vector<db::string>::const_iterator 
				i(data.begin())
				, e(data.end())
				; i != e
				; ++i)
				Add(gcnew String(i->c_str()));
		}

		/// @brief        returns a subset of terms
		///								the current index calculates from the end of the split
		///
		/// <BR>qualifier const
		/// <BR>access    public  
		/// @return       db::split
		/// @param        start as int - start index - could be negative
		/// @param        end as int - end index - could be negative
		///
		/// @date         12.3.2010 8:30
		///
		split^ slice(Int32 start, Int32 end)
		{
			split^ data = gcnew split();

			Int32 size_ = Count;

			if (start < 0)
				start = start+size_;
			if (end < 0)
				end = end+size_;
			if (start >= size_)
				start = size_-1;
			if (end >= size_)
				end = size_-1;
			if (start >= end)
				return data;

			for (int i = start; i < end; ++i)
				data->Add(this[i]);
			return data; 
		}

		/// @brief        returns text and adds a delimiter
		///
		/// <BR>qualifier const
		/// <BR>access    public 
		/// 
		/// @return       db::string
		/// @param        delim as const string & - the added delimiter
		///
		/// @date         2.5.2010 19:37
		/// 
		String^ join(String^ delim)
		{
			String^ res = gcnew String("");
			unsigned cnt = 0;

			for each (String^ i in this)
			{
				if (cnt != 0)
					res += delim;
				++cnt;
				res += i;
			}
			return res; 
		}

		/// @brief        extend the split with the terms from another one
		///
		/// <BR>qualifier
		/// <BR>access    public 
		/// 
		/// @return       split &
		/// @param        s as const split &
		///
		/// @date         2.5.2010 19:39
		/// 
		split^ extend(split^ s)
		{
			for each (String^ i in s)
			{
				Add(i);
			}
			return this; 
		}
	}; 

	/// fieldtype interface
	public ref class field 
	{
	public:

	protected:
		String^ _name, _table, _sourcename;
		param_types _type;
		unsigned _extra;
		Values^ _values;
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
		field(const string& n, 
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
	//typedef std::allocator<field> alloc_field; 
	typedef std::pair<field, field> field_pair;
	
	namespace expr
	{
		///expression base class
		public ref class base 
		{
		protected:
			split^ extraTables; //!< extra tables, to be joined
		public:

			/// @brief     default expression is true
			///
			/// <BR>qualifier const
			/// <BR>access    virtual public  
			/// @return    String^
			///
			/// @date      18:2:2009   11:16
			///
			virtual String^ str()
			{
				return gcnew String("True");
			}

			/// @brief     string operator 
			///
			/// <BR>qualifier const
			/// <BR>access    virtual public  
			/// @return    String^
			///
			///
			virtual operator String^()
			{
				return str();
			}

			/// <BR>qualifier const
			/// <BR>access    public  
			/// @return    split^
			///
			/// @date      18:2:2009   11:16
			///
			split^ getExtraTables()
			{
				return extraTables;
			}

			base()
				: extraTables(gcnew split())
			{

			}

			/// @brief     default virtual destructor
			///
			/// <BR>qualifier
			/// <BR>access    virtual public  
			/// @return    
			///
			/// @date      18:2:2009   11:17
			///
			virtual ~base()
			{

			}
		}; 

		public ref class raw : public base 
		{
			String^ expr;
		public:
			/// @brief        usable for special expressions in WHERE terms
			///
			/// <BR>qualifier : base(), expr(e)
			/// <BR>access    public   
			/// @return       
			/// @param        String ^ e
			///
			/// @author       T. Schroeder (explodus@gmx.de)
			/// @date         21.10.2013 15:34
			/// 
			raw(String^ e) : base(), expr(e)
			{

			}

			virtual String^ str() override 
			{
				return expr;
			}
		};

		public ref class connective : public base 
		{
		private:
			String^ op;
		protected:
			base^ const e1;
			base^ const e2;

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
			connective(String^ o, base^ const e1_, base^ const e2_) 
				: base(), op(o), e1(e1_), e2(e2_)
			{

			}

		public:        

			/// @brief     str
			///
			/// <BR>qualifier const
			/// <BR>access    virtual public  
			/// @return    String^
			///
			/// @date      20:2:2009   12:55
			///
			virtual String^ str() override 
			{
				String^ res = gcnew 
					String("(" + e1->str() + ") " + op + " (" + e2->str() + ")");
				return res;
			}
		};  

		public ref class and_ : public connective 
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
			and_(base^ const e1_, base^ const e2_) 
				: connective(gcnew String("and"), e1_, e2_)
			{

			}

			/// @brief     str
			///
			/// <BR>qualifier const
			/// <BR>access    public  
			/// @return    db::string
			///
			/// @date      20:2:2009   12:56
			///
			virtual String^ str() override 
			{
				if (e1->str() == "True")
					return e2->str();
				else if (e2->str() == "True")
					return e1->str();
				else
					return connective::str();
			}
		};

		public ref class or_ : public connective 
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
			or_(base^ const e1_, base^ const e2_) 
				: connective(gcnew String("or"), e1_, e2)
			{

			}

			/// <BR>qualifier const
			/// <BR>access    public  
			/// @return    db::string
			///
			/// @date      20:2:2009   12:56
			///
			virtual String^ str() override 
			{
				if (e1->str() == "True")
					return e1->str();
				else if (e2->str() == "True")
					return e2->str();
				else
					return connective::str();
			}
		};

		public ref class not_ : public base 
		{
		private:
			base^ const exp;
		public:
			/// @brief        negates expression
			///
			/// <BR>qualifier : base(), exp(_exp)
			/// <BR>access    public   
			/// @return       
			/// @param        base ^ const _exp
			///
			/// @author       T. Schroeder (explodus@gmx.de)
			/// @date         21.10.2013 15:47
			/// 
			not_(base^ const _exp)
				: base(), exp(_exp)
			{

			}

			/// <BR>qualifier const
			/// <BR>access    virtual public  
			/// @return    db::string
			///
			/// @date      20:2:2009   12:57
			///
			virtual String^ str() override 
			{
				return gcnew String("not (" + exp->str() + ")");
			}

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
		class SQLITE_WRAPPER_DLLAPI eq : public oper {
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
		class SQLITE_WRAPPER_DLLAPI not_eq_ : public oper {
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
		class SQLITE_WRAPPER_DLLAPI gt : public oper {
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
		class SQLITE_WRAPPER_DLLAPI gt_eq : public oper {
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
		class SQLITE_WRAPPER_DLLAPI lt : public oper {
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
		class SQLITE_WRAPPER_DLLAPI lt_eq : public oper {
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
		class SQLITE_WRAPPER_DLLAPI like : public oper {
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
		class SQLITE_WRAPPER_DLLAPI in : public oper {
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

	public ref class param
	{
	private:
		System::Object^ data_;
		bool is_null_;
	public:
		param()
		{

		}

		~param()
		{

		}

		System::Object^ Data() { return data_; }
		void Data(System::Object^ val) { data_ = val; }
		bool IsNull() { return is_null_; }
		void IsNull(bool val) { is_null_ = val; }

	protected:
		
	};

	public ref class row : ObservableCollection<param^>
	{
	private:

	public:
		row()
		{

		}

		virtual ~row()
		{

		}
	};

	public ref class base_wrapper
	{
	private:
		db::base * _base;

	public:
		/// @brief        base
		///
		/// <BR>qualifier
		/// <BR>access    private   
		/// @return       
		/// @param        String ^ db_file_name
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         9.10.2013 8:29
		/// 
		base_wrapper(String^ db_file_name)
		{
			_base = new db::base();
			if (_base)
				_base->connect(marshal_as<std::string>(db_file_name), "");
		}

		/// @brief        base
		///
		/// <BR>qualifier
		/// <BR>access    private   
		/// @return       
		/// @param        String ^ db_file_name
		/// @param        String ^ key
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         9.10.2013 8:29
		/// 
		base_wrapper(String^ db_file_name, String^ key)
		{
			_base = new db::base();
			if (_base)
				_base->connect(marshal_as<std::string>(db_file_name), marshal_as<std::string>(key));
		}

		/// @brief        ~base
		///
		/// <BR>qualifier
		/// <BR>access    virtual private   
		/// @return       
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         9.10.2013 8:29
		/// 
		virtual ~base_wrapper()
		{
			if (_base)
				delete _base;
		}

		/// @brief				begins SQL transaction
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return       void
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         15.4.2010 13:49
		///
		void begin()
		{
			if (_base)
				_base->begin();
		}

		/// @brief        commits SQL transaction
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return       void
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         15.4.2010 13:50
		///
		void commit()
		{
			if (_base)
				_base->commit();
		}

		/// @brief        cancels active SQL transaction
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return       void
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         15.4.2010 13:50
		///
		void rollback()
		{
			if (_base)
				_base->rollback();
		}

		/// @brief        execute
		///
		/// <BR>qualifier
		/// <BR>access    public   
		/// @return       ObservableCollection<row^>^
		/// @param        String ^ sql
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         9.10.2013 9:33
		/// 
		ObservableCollection<row^>^ execute(String^ sql)
		{
			ObservableCollection<row^>^ ret = gcnew ObservableCollection<row^>;
			db::query_ptr q;

			try
			{
				if (_base)
					q = _base->execute_ptr(marshal_as<std::wstring>(sql));
			}
			catch (db::exception::base & e)
			{
				db::string err = e.what();
				String^ serr = gcnew String(err.c_str());

#ifdef _DEBUG
#ifdef _WIN32
				System::Diagnostics::Debug::WriteLine(serr);
#endif // _WIN32
#ifdef _WIN64
				System::Diagnostics::Debug::WriteLine(serr);
#endif // _WIN64
#endif // _DEBUG
				throw gcnew ApplicationException(serr);
			}

			if (!q)
				return ret;

			if (!q->size())
				return ret;

			for (db::query::const_iterator 
				  itb(q->begin())
				, ite(q->end())
				; itb != ite
				; ++itb)
			{
				const db::row & r = *itb;
				row^ ref_r = gcnew row();

				for (db::row::const_iterator 
					  itrb(r.begin())
					, itre(r.end())
					; itrb != itre
					; ++itrb)
				{
					const db::param & p = *itrb;
					param^ ref_p = gcnew param;
					{ // param befüllen

						ref_p->IsNull(p.is_null());

						switch (p.get_type())
						{
						case db::e_int:
						case db::e_unsigned:
							ref_p->Data(gcnew System::Int32(p.get_int()));
							break;
						case db::e_long:
							ref_p->Data(gcnew System::Int64(p.get_long()));
							break;
						case db::e_float:
						case db::e_double:
						case db::e_date_time:
							ref_p->Data(gcnew System::Double(p.get_double()));
							break;
						default:
							ref_p->Data(gcnew String(p.c_str()));
							break;
						}
					}
					ref_r->Add(ref_p);
				}
				ret->Add(ref_r);
			}
			return ret;
		}
	};

	/// @brief helper class, generates INSERT SQL statements
	public ref class ins 
	{
	protected:
		split_map _values;
		String^    _source;
		String^    _delim1;
		String^    _delim2;

	public:
		///
		/// <BR>qualifier
		/// <BR>access    public 
		/// 
		/// @return       
		/// @param        tab as const string &
		///
		/// @date         20.4.2010 20:37
		/// @ingroup			constructors
		/// 
		ins(String^ tab) : _source(tab)
		{

		}

		///
		/// <BR>qualifier
		/// <BR>access    public 
		/// 
		/// @return       
		/// @param        tab as const string &
		/// @param        delim as const string &
		///
		/// @date         20.4.2010 20:37
		/// @ingroup			constructors
		/// 
		ins(String^ tab, String^ delim)
			: _source(tab)
			, _delim1(delim)
			, _delim2(delim)
		{

		}

		///
		/// <BR>qualifier
		/// <BR>access    public 
		/// 
		/// @return       
		/// @param        tab as const string &
		/// @param        delim1 as const string &
		/// @param        delim2 as const string &
		///
		/// @date         20.4.2010 20:38
		/// @ingroup			constructors
		/// 
		ins(String^ tab, String^ delim1, String^ delim2)
			: _source(tab)
			, _delim1(delim1)
			, _delim2(delim2)
		{

		}

		/// @brief        destructor
		///
		/// <BR>qualifier
		/// <BR>access    public 
		/// 
		/// @return       
		///
		/// @date         20.4.2010 20:39
		/// 
		~ins()
		{

		}

		/// @brief     clear_values
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return    ins &
		///
		/// @date      20:2:2009   9:07
		///
		ins^ clear_values()
		{
			_values.Clear();
			return this;
		}

		/// @brief     operator%
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return    ins &
		/// @param     f as const db::field &
		///
		/// @date      20:2:2009   9:07
		///
		//ins & operator%(const db::field& f);

		/// @brief     values
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return    ins &
		/// @param     t as string
		/// @param     v as param *
		///
		/// @date      20:2:2009   9:07
		///
		//ins & values(String^ t, param* v);

		/// @brief     values
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return    ins &
		/// @param     t as string
		/// @param     v as string
		/// @param     p as param_types
		///
		/// @date      20:2:2009   9:07
		///
		ins^ values(String^ t, String^ v, param_types p/*=e_int*/)
		{
			String^ tt = gcnew String(t);
			String^ vv = gcnew String(v);
			switch(p) 
			{
			case param_types::e_char:
			case param_types::e_blob:
			case param_types::e_string:
			case param_types::e_date_time:
				vv->Replace("'", "");
				vv->Replace("`", "");
				vv->Replace("´", "");
				vv = "'" + vv;
				vv += "'";
				break;
			}
			tt = _delim1 + tt + _delim2;
			_values.Add(tt, vv);

			return this;
		}

		/// @brief     values
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return    ins &
		/// @param     t as const string &
		/// @param     v as const T &
		///
		/// @date      20:2:2009   11:07
		///
		template<typename T>
		ins^ values(String^ t, T v)
		{ return values(t, System::Convert::ToString(v), tp(v)); }

		template<>
		ins^ values(String^ t, Boolean v)
		{ return values(t, System::Convert::ToString(v), tp(v)); }
		template<>
		ins^ values(String^ t, Int16 v)
		{ return values(t, System::Convert::ToString(v), tp(v)); }
		template<>
		ins^ values(String^ t, Int32 v)
		{ return values(t, System::Convert::ToString(v), tp(v)); }
		template<>
		ins^ values(String^ t, Double v)
		{ return values(t, System::Convert::ToString(v), tp(v)); }
		template<>
		ins^ values(String^ t, String^ v)
		{ return values(t, v, tp(v, false)); }

		/// @brief     operator string
		///
		/// <BR>qualifier const
		/// <BR>access    public  
		/// @return    
		///
		/// @date      20:2:2009   11:04
		///
		operator String^()
		{
			return str();
		}

		/// @brief     str
		///
		/// <BR>qualifier const
		/// <BR>access    public  
		/// @return    db::string
		///
		/// @date      20:2:2009   11:05
		///
		String^ str()
		{
			String^ res = gcnew String("INSERT INTO ");
			res += _source;

			if (_values.Count > 0)
			{
				res += " (";
				res += _values.join_fields(",");
				res += ") VALUES (";
				res += _values.join_values(",");
				res += ")";
			}
			return res;
		}
	}; 

	///	@brief helper class for generating update sql expressions
	public ref class upd 
	{
		String^ _delim1;
		String^ _delim2;
		String^ table;
		String^ _where;
		split fields;
		split values;
	public:
		///
		/// <BR>qualifier
		/// <BR>access    public   
		/// @return       
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         21.10.2013 11:30
		/// 
		~upd()
		{

		}

		///
		/// <BR>qualifier : table(t) , _where(gcnew String("True")) , _delim1(gcnew String("")) , _delim2(gcnew String(""))
		/// <BR>access    public   
		/// @return       
		/// @param        String ^ t - should contain a table name
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         21.10.2013 11:30
		/// 
		upd(String^ t) 
			: table(t)
			, _where(gcnew String("True"))
			, _delim1(gcnew String(""))
			, _delim2(gcnew String(""))
		{

		}

		///
		/// <BR>qualifier : table(t) , _where(gcnew String("True")) , _delim1(gcnew String(d)) , _delim2(gcnew String(d))
		/// <BR>access    public   
		/// @return       
		/// @param        String ^ t - should contain a table name
		/// @param        String ^ d - holds a front and back delimiter
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         21.10.2013 11:29
		/// 
		upd(String^ t, String^ d)
			: table(t)
			, _where(gcnew String("True"))
			, _delim1(gcnew String(d))
			, _delim2(gcnew String(d))
		{

		}

		///
		/// <BR>qualifier : table(t) , _where(gcnew String("True")) , _delim1(gcnew String(d1)) , _delim2(gcnew String(d2))
		/// <BR>access    public   
		/// @return       
		/// @param        String ^ t - should contain a table name
		/// @param        String ^ d1 - the front delimiter
		/// @param        String ^ d2 - the back delimiter
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         21.10.2013 11:29
		/// 
		upd(String^ t, String^ d1, String^ d2)
			: table(t)
			, _where(gcnew String("True"))
			, _delim1(gcnew String(d1))
			, _delim2(gcnew String(d2))
		{

		}

		/// @brief        
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return       upd&
		/// @param        e as const expr::base &
		///
		/// @date         15.4.2010 13:55
		///
		//upd& where(const expr::base& e);

		/// @brief        insert a sql where expression through the operator % overload 
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return       upd &
		/// @param        e as const expr::base &
		///
		/// @date         15.4.2010 13:55
		///
		//upd & operator%(const expr::base& e);

		/// @brief        insert a new field with it name and the value
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return       upd &
		/// @param        f as const db::field &
		///
		/// @date         15.4.2010 13:58
		///
		//upd & operator%(const db::field& f);

		/// @brief        insert a new field with it name and the value
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return       upd&
		/// @param        f as const field &
		/// @param        val as const string &
		///
		/// @date         15.4.2010 13:56
		///
		//upd& set(const field& f, const string& val);

		/// @brief        insert a new field with it name and the value
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return       upd&
		/// @param        f as const field &
		/// @param        val as const T &
		///
		/// @date         15.4.2010 13:58
		///
		//template<typename T>
		//upd& set(const field& f, const T& val)
		//{ return set(f, detail::to_string(val)); }

		/// @brief        String^ overload
		///
		/// <BR>qualifier
		/// <BR>access    public   
		/// @return       
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         21.10.2013 11:31
		/// 
		operator String^()
		{
			return str();
		}

		/// @brief        same as the string operator overload, more stl like
		///
		/// <BR>qualifier
		/// <BR>access    public   
		/// @return       String^
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         21.10.2013 11:32
		/// 
		String^ str()
		{
			String^ q = gcnew String("UPDATE " + table + " SET ");
			split sets;

			Int32 i = 0;
			for each (String^ var in fields)
			{
				sets.Add(var + "=" + values[i++]);
			}

			q += sets.join(",");

			if (_where->Length)
				q += " WHERE " + _where;

			return q;
		}
	};
}
