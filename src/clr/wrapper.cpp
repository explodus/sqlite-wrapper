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
	/// @brief Parameter Typen 
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

	/// @brief Parameter Extras
	public enum class field_extra : unsigned short
	{
		e_nullExtra         = 0,  //!< null extra
		eFixedField         = 2,  //!< Feld mit fester Größe (numerischer Typ)
		eVariableField      = 4,  //!< variables Feld (textlicher Typ)
		eAutoIncrField      = 8,  //!< automatisch erhöhendes Feld
		eUpdatableField     = 16, //!< veränderbares Feld
		eRequiredField      = 32, //!< benötigtes Feld
		eZeroLengthAllowed  = 64  //!< Nulllänge erlaubt
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

	namespace detail
	{
		template<class T> 
		inline T front_back_delim(T sText, T delim1, T delim2)
		{ 
			sText = delim1 + sText + delim2; 
			return sText;
		}

		inline String^ escape_sql(String^ const str)
		{ 
			if (str == "NULL")
				return gcnew String(str);
			
			String^ ret = gcnew String(str);
			ret->Replace("\'NULL\'", "NULL");
			ret->Replace("'", "''");

			ret = "'" + ret + "'";
			return ret;
		}
	}

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

	ref class sel;

	namespace expr
	{
		ref class in_;
		ref class like;
	}

	/// fieldtype interface
	public ref class field 
	{
	public:

	protected:
		String^ _name;
		String^ _table;
		String^ _sourcename;
		param_types _type;
		unsigned _extra;
		Values^ _values;
		long _length;

	public:
		/// @brief        field
		///
		/// <BR>qualifier : _name(n) , _table(gcnew String("")) , _sourcename(gcnew String("")) , _type(t) , _extra(e_nullExtra) , _values(gcnew Values()) , _length(-1)
		/// <BR>access    public   
		/// @return       
		/// @param        String ^ n
		/// @param        const param_types & t
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         22.10.2013 7:39
		/// 
		field(String^ n, param_types t/*=e_long*/)
			: _name(n)
			, _table(gcnew String(""))
			, _sourcename(gcnew String(""))
			, _type(t)
			, _extra(static_cast<unsigned>(field_extra::e_nullExtra))
			, _values(gcnew Values())
			, _length(-1)
		{

		}

		///
		/// <BR>qualifier : _name(n) , _table(gcnew String("")) , _sourcename(gcnew String("")) , _type(tp(val)) , _extra(e_nullExtra) , _values(gcnew Values()) , _length(-1)
		/// <BR>access    public   
		/// @return       
		/// @param        String ^ n
		/// @param        T val
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         22.10.2013 7:39
		/// 
		template<class T>
		field(String^ n, T val)
			: _name(n)
			, _table(gcnew String(""))
			, _sourcename(gcnew String(""))
			, _type(tp(val))
			, _extra(static_cast<unsigned>(field_extra::e_nullExtra))
			, _values(gcnew Values())
			, _length(-1) 
		{
			_values.Add(gcnew StringPair(_name, System::Convert::ToString(val))); 
		}

		/// @brief        field
		///
		/// <BR>qualifier : _name(n_tbl->Key) , _table(n_tbl->Value) , _sourcename(gcnew String("")) , _type(t) , _extra(e_nullExtra) , _values(vals) , _length(-1)
		/// <BR>access    public   
		/// @return       
		/// @param        StringPair ^ n_tbl
		/// @param        param_types t
		/// @param        Values ^ vals
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         22.10.2013 7:41
		/// 
		field(
			  StringPair^ n_tbl
			, param_types t/*=e_long*/
			, Values^ vals/* = Values()*/)
			: _name(n_tbl->Key)
			, _table(n_tbl->Value)
			, _sourcename(gcnew String(""))
			, _type(t)
			, _extra(static_cast<unsigned>(field_extra::e_nullExtra))
			, _values(vals)
			, _length(-1) 
		{

		}

		/// @brief        field
		///
		/// <BR>qualifier : _name(f->_name) , _table(f->_table) , _sourcename(f->_sourcename) , _type(f->_type) , _extra(f->_extra) , _values(f->_values) , _length(f->_length)
		/// <BR>access    public   
		/// @return       
		/// @param        field ^ f
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         22.10.2013 7:43
		/// 
		field(field^ f)
			: _name(f->_name)
			, _table(f->_table)
			, _sourcename(f->_sourcename)
			, _type(f->_type)
			, _extra(f->_extra)
			, _values(f->_values)
			, _length(f->_length) 
		{

		}

		///
		/// <BR>qualifier : _name(gcnew String("")) , _table(gcnew String("")) , _sourcename(gcnew String("")) , _type(e_int) , _extra(e_nullExtra) , _values(gcnew Values()) , _length(-1)
		/// <BR>access    public   
		/// @return       
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         22.10.2013 7:53
		/// 
		field()
			: _name(gcnew String(""))
			, _table(gcnew String(""))
			, _sourcename(gcnew String(""))
			, _type(param_types::e_int)
			, _extra(static_cast<unsigned>(field_extra::e_nullExtra))
			, _values(gcnew Values())
			, _length(-1)

		{

		}

		/// @brief        fullName
		///
		/// <BR>qualifier
		/// <BR>access    public   
		/// @return       String^
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         22.10.2013 7:53
		/// 
		String^ fullName()
		{
			if (table()->Length > 0) 
				return  table() + "." + name(); 
			else
				return  name();
		}

		/// @brief        ~field
		///
		/// <BR>qualifier
		/// <BR>access    public   
		/// @return       
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         22.10.2013 7:53
		/// 
		~field()
		{

		}

		/// @brief        name
		///
		/// <BR>qualifier
		/// <BR>access    public   
		/// @return       String^
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         22.10.2013 7:58
		/// 
		inline String^ name()
		{
			return _name;
		}

		/// @brief        sourcename
		///
		/// <BR>qualifier
		/// <BR>access    public   
		/// @return       String^
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         22.10.2013 7:58
		/// 
		inline String^ sourcename()
		{
			return _sourcename;
		}

		/// @brief        type
		///
		/// <BR>qualifier
		/// <BR>access    public   
		/// @return       clr_db::param_types
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         22.10.2013 7:58
		/// 
		inline param_types type()
		{
			return _type;
		}
		
		/// @brief        table
		///
		/// <BR>qualifier
		/// <BR>access    public   
		/// @return       String^
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         22.10.2013 7:58
		/// 
		inline String^ table()
		{
			return _table;
		}

		/// @brief        length
		///
		/// <BR>qualifier
		/// <BR>access    public   
		/// @return       long
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         22.10.2013 7:58
		/// 
		inline long length()
		{
			return _length;
		}

		/// @brief        set_type
		///
		/// <BR>qualifier
		/// <BR>access    public   
		/// @return       field^
		/// @param        param_types type
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         22.10.2013 7:58
		/// 
		inline field^ set_type(param_types type)
		{
			_type = type;
			return this;
		}

		/// @brief        set_table
		///
		/// <BR>qualifier
		/// <BR>access    public   
		/// @return       void
		/// @param        String ^ table
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         22.10.2013 7:58
		/// 
		void set_table(String^ table)
		{
			_table = table;
		}

		/// @brief        set_sourcename
		///
		/// <BR>qualifier
		/// <BR>access    public   
		/// @return       void
		/// @param        String ^ sourcename
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         22.10.2013 7:58
		/// 
		void set_sourcename(String^ sourcename)
		{
			_sourcename = sourcename;
		}

		/// @brief        set_length
		///
		/// <BR>qualifier
		/// <BR>access    public   
		/// @return       void
		/// @param        const long & len
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         22.10.2013 7:58
		/// 
		void set_length(const long& len)
		{
			_length = len;
		}

		/// @brief        has_extra
		///
		/// <BR>qualifier
		/// <BR>access    public   
		/// @return       bool
		/// @param        field_extra extra
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         22.10.2013 7:58
		/// 
		bool has_extra(field_extra extra)
		{
			return ((_extra&static_cast<unsigned>(extra))==1);
		}

		/// @brief        add_extra
		///
		/// <BR>qualifier
		/// <BR>access    public   
		/// @return       void
		/// @param        field_extra extra
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         22.10.2013 7:58
		/// 
		void add_extra(field_extra extra)
		{
			if(!(_extra&static_cast<unsigned>(extra))) 
				_extra |= static_cast<unsigned>(extra);
		}

		/// @brief        remove_extra
		///
		/// <BR>qualifier
		/// <BR>access    public   
		/// @return       void
		/// @param        field_extra extra
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         22.10.2013 7:58
		/// 
		void remove_extra(field_extra extra)
		{
			if(_extra&static_cast<unsigned>(extra)) 
				_extra &= ~static_cast<unsigned>(extra);
		}

		/// @brief        values
		///
		/// <BR>qualifier
		/// <BR>access    public   
		/// @return       Values^
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         22.10.2013 7:59
		/// 
		Values^ values()
		{
			return _values;
		}

		/// @brief     syntactic sugar to expression-api, field.in(sel)
		///
		/// <BR>qualifier
		/// <BR>access    public   
		/// @return       clr_db::expr::in^
		/// @param        String ^ set
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         22.10.2013 8:04
		/// 
		inline clr_db::expr::in_^ In(String^ set);

		/// @brief     syntactic sugar to expression-api, field.in(sel)
		///
		/// <BR>qualifier
		/// <BR>access    public   
		/// @return       clr_db::expr::in^
		/// @param        clr_db::sel ^ sel
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         22.10.2013 8:05
		/// 
		inline clr_db::expr::in_^ In(clr_db::sel^ sel);

		/// @brief     syntactic sugar to expression-api, field.like(string)
		///
		/// <BR>qualifier
		/// <BR>access    public   
		/// @return       clr_db::expr::like^
		/// @param        String ^ const s
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         22.10.2013 8:07
		/// 
		inline clr_db::expr::like^ Like(String^ const s);

		/// @brief        operator==
		///
		/// <BR>qualifier
		/// <BR>access    public   
		/// @return       bool
		/// @param        field ^ const fd
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         22.10.2013 8:07
		/// 
		bool operator==(field^ const fd)
		{
			return fd->fullName() == fullName();
		}

		/// @brief        operator!=
		///
		/// <BR>qualifier
		/// <BR>access    public   
		/// @return       bool
		/// @param        field ^ const fd
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         22.10.2013 8:07
		/// 
		bool operator!=(field^ const fd)
		{
			return ! (*this == fd);
		}
	};

	//typedef boost::detail::allocator::partial_std_allocator_wrapper<
	//	boost::detail::quick_allocator<field > > alloc_field; 
	//typedef std::allocator<field> alloc_field; 
	typedef KeyValuePair<field^, field^> FieldPair;

	namespace expr
	{
		ref class and_;

		///expression base class
		public ref class base_ 
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

			base_()
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
			virtual ~base_()
			{

			}

			virtual and_^ operator&(base_^ o);
		}; 

		public ref class raw : public base_ 
		{
			String^ expr;
		public:
			/// @brief        usable for special expressions in WHERE terms
			///
			/// <BR>qualifier : base_(), expr(e)
			/// <BR>access    public   
			/// @return       
			/// @param        String ^ e
			///
			/// @author       T. Schroeder (explodus@gmx.de)
			/// @date         21.10.2013 15:34
			/// 
			raw(String^ e) : base_(), expr(e)
			{

			}

			virtual String^ str() override 
			{
				return expr;
			}
		};

		public ref class connective : public base_ 
		{
		private:
			String^ op;
		protected:
			base_^ const e1;
			base_^ const e2;

			/// @brief     base class to connect 2 expressions
			///
			/// <BR>qualifier : op(o), e1(e1_), e2(e2_)
			/// <BR>access    protected  
			/// @return    
			/// @param     string o
			/// @param     e1_ as const base_ &
			/// @param     e2_ as const base_ &
			///
			/// @date      18:2:2009   11:20
			///
			connective(String^ o, base_^ const e1_, base_^ const e2_) 
				: base_(), op(o), e1(e1_), e2(e2_)
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
			/// @param     e1_ as const base_ &
			/// @param     e2_ as const base_ &
			///
			/// @date      18:2:2009   11:22
			///
			and_(base_^ const e1_, base_^ const e2_) 
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
			/// @param     e1_ as const base_ &
			/// @param     e2_ as const base_ &
			///
			/// @date      18:2:2009   11:22
			///
			or_(base_^ const e1_, base_^ const e2_) 
				: connective(gcnew String("or"), e1_, e2_)
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

		public ref class not_ : public base_ 
		{
		private:
			base_^ const exp;
		public:
			/// @brief        negates expression
			///
			/// <BR>qualifier : base_(), exp(_exp)
			/// <BR>access    public   
			/// @return       
			/// @param        base_ ^ const _exp
			///
			/// @author       T. Schroeder (explodus@gmx.de)
			/// @date         21.10.2013 15:47
			/// 
			not_(base_^ const _exp)
				: base_(), exp(_exp)
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
		public ref class oper : public base_ 
		{
		protected:
			field^ const _field;
			String^ op;
			String^ data;
			bool escape;

			/// @brief as an example, e_string got escaped
			///
			/// <BR>qualifier
			/// <BR>access    protected  
			/// @return    bool
			/// @param     type_ as param_types
			///
			/// @date      18:2:2009   11:26
			///
			bool check_escape(param_types type_)
			{
				switch(type_) 
				{
				case param_types::e_string:
				case param_types::e_char:
				case param_types::e_blob:
				case param_types::e_date_time:
					return true;
					break;
				case param_types::e_null:
				case param_types::e_bool:
				case param_types::e_int:
				case param_types::e_unsigned:
				case param_types::e_long:
				case param_types::e_float:
				case param_types::e_double:
					return false;
					break;
				}
				return false;
			}

			/// @brief     string overload
			///
			/// <BR>qualifier : base_() , _field(fld) , op(o) , data(d) , escape(check_escape(fld->type()))
			/// <BR>access    protected   
			/// @return       
			/// @param        field ^ const fld
			/// @param        String ^ o
			/// @param        String ^ d
			///
			/// @author       T. Schroeder (explodus@gmx.de)
			/// @date         22.10.2013 8:16
			/// 
			oper(field^ const fld, String^ o, String^ d)
				: base_()
				, _field(fld)
				, op(o)
				, data(d)
				, escape(check_escape(fld->type()))
			{
				extraTables->Add(fld->table());
			}

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
			template<class T>
			oper(field^ const fld, String^ o, T d) 
				: base_()
				, _field(fld)
				, op(o)
				, data(gcnew String("0"))
				, escape(check_escape(_field.type())) 
			{
				data = System::Convert::ToString(d);
				extraTables->Add(fld->table());
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
			oper(field^ const fld, String^ o, field^ f2)
				: base_()
				, _field(fld)
				, op(o)
				, data(f2->fullName())
				, escape(false)
			{
				extraTables->Add(fld->table());
			}

		public:
			///
			/// <BR>qualifier
			/// <BR>access    virtual public   
			/// @return       String^
			///
			/// @author       T. Schroeder (explodus@gmx.de)
			/// @date         22.10.2013 8:20
			/// 
			virtual String^ str() override 
			{
				String^ res = gcnew String("");
				res += _field->fullName() + " " + op + " " + 
					(escape ? detail::escape_sql(data) : data);
				return res;
			}
		};

		///==, is equal operator
		public ref class eq : public oper 
		{
		public:
			/// @brief     ==, is equal operator
			///
			/// <BR>qualifier : oper(fld, gcnew String("="), d)
			/// <BR>access    public   
			/// @return       
			/// @param        field ^ const fld
			/// @param        T d
			///
			/// @author       T. Schroeder (explodus@gmx.de)
			/// @date         22.10.2013 8:27
			/// 
			template<class T>
			eq(field^ const fld, T d) : oper(fld, gcnew String("="), d) 
			{ }
		};

		///<> not equal operator
		public ref class not_eq_ : public oper 
		{
		public:
			/// @brief     !=, not equal constructor
			///
			/// <BR>qualifier : oper(fld, gcnew String("<>"), d)
			/// <BR>access    public   
			/// @return       
			/// @param        field ^ const fld
			/// @param        T d
			///
			/// @author       T. Schroeder (explodus@gmx.de)
			/// @date         22.10.2013 8:28
			/// 
			template<class T>
			not_eq_(field^ const fld, T d) : oper(fld, gcnew String("<>"), d)
			{ }
		};

		///> greater then operator
		public ref class gt : public oper 
		{
		public:
			/// @brief     >, greater then operator
			///
			/// <BR>qualifier : oper(fld, gcnew String(">"), d)
			/// <BR>access    public   
			/// @return       
			/// @param        field ^ const fld
			/// @param        T d
			///
			/// @author       T. Schroeder (explodus@gmx.de)
			/// @date         22.10.2013 8:29
			/// 
			template<class T>
			gt(field^ const fld, T d) : oper(fld, gcnew String(">"), d)
			{ }
		};

		///greater then or equal operator
		public ref class gt_eq : public oper 
		{
		public:
			/// @brief        greater then or equal operator
			///
			/// <BR>qualifier : oper(fld, gcnew String(">="), d)
			/// <BR>access    public   
			/// @return       
			/// @param        field ^ const fld
			/// @param        T d
			///
			/// @author       T. Schroeder (explodus@gmx.de)
			/// @date         22.10.2013 8:29
			/// 
			template<class T>
			gt_eq(field^ const fld, T d) : oper(fld, gcnew String(">="), d)
			{ }
		};

		///lower then operator
		public ref class lt : public oper 
		{
		public:
			/// @brief     <, lower then constructor
			///
			/// <BR>qualifier : oper(fld, gcnew String("<"), d)
			/// <BR>access    public   
			/// @return       
			/// @param        field ^ const fld
			/// @param        T d
			///
			/// @author       T. Schroeder (explodus@gmx.de)
			/// @date         22.10.2013 8:30
			/// 
			template<class T>
			lt(field^ const fld, T d) : oper(fld, gcnew String("<"), d)
			{ }
		};

		///lower then or equal operator
		public ref class lt_eq : public oper 
		{
		public:
			/// @brief     <=, lower then or equal constructor
			///
			/// <BR>qualifier : oper(fld, gcnew String("<="), d)
			/// <BR>access    public   
			/// @return       
			/// @param        field ^ const fld
			/// @param        T d
			///
			/// @author       T. Schroeder (explodus@gmx.de)
			/// @date         22.10.2013 8:31
			/// 
			template<class T>
			lt_eq(field^ const fld, T d) : oper(fld, gcnew String("<="), d)
			{ }
		};

		///like operator
		public ref class like : public oper 
		{
		public:
			/// @brief     like constructor
			///
			/// <BR>qualifier : oper(fld, gcnew String("LIKE"), d)
			/// <BR>access    public   
			/// @return       
			/// @param        field ^ const fld
			/// @param        String ^ d
			///
			/// @author       T. Schroeder (explodus@gmx.de)
			/// @date         22.10.2013 8:32
			/// 
			like(field^ const fld, String^ d) : oper(fld, gcnew String("LIKE"), d)
			{

			}
		};

		///in Operator
		public ref class in_ : public oper 
		{
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
			in_(field^ const fld, String^ set) 
				: oper(fld, gcnew String("IN"), gcnew String("(" + set + ")"))
			{

			}

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
			inline in_(field^ const fld, clr_db::sel^ s);

			///
			/// <BR>qualifier
			/// <BR>access    virtual public   
			/// @return       String^
			///
			/// @author       T. Schroeder (explodus@gmx.de)
			/// @date         22.10.2013 8:36
			/// 
			virtual String^ str() override 
			{
				return gcnew String(_field->fullName() + " " + op + " " + data);
			}
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

	/// @brief helper class, generates SELECT SQL statements
	public ref class sel
	{
	protected:
		bool _distinct;
		int _limit;
		int _offset;
		split^ _results;
		split^ _sources;
		split^ _groupBy;
		split^ _orderBy;

		String^ _where; 
		String^ _having;
		String^ _delim1;
		String^ _delim2;
		FieldPair^ _join;

	public:
		/// @defgroup constructors summary of all constructors
		/// @defgroup operators summary of all operator overloads

		/// @brief        constructor
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return       
		///
		/// @date         20.4.2010 10:38
		/// @ingroup			constructors
		///
		sel() 
			: _distinct(false)
			, _limit(0)
			, _offset(0)
			, _results(gcnew split())
			, _sources(gcnew split())
			, _groupBy(gcnew split())
			, _orderBy(gcnew split())
			, _where(gcnew String("True"))
			, _having(gcnew String(""))
			, _delim1(gcnew String(""))
			, _delim2(gcnew String(""))
		{

		}

		/// @brief        constructor
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return       
		/// @param        tablename as const string &
		///
		/// @date         20.4.2010 10:38
		/// @ingroup			constructors
		///
		sel(String^ tablename)
			: _distinct(false)
			, _limit(0)
			, _offset(0)
			, _results(gcnew split())
			, _sources(gcnew split())
			, _groupBy(gcnew split())
			, _orderBy(gcnew split())
			, _where(gcnew String("True"))
			, _having(gcnew String(""))
			, _delim1(gcnew String(""))
			, _delim2(gcnew String(""))
		{
			source(tablename, nullptr);
		}

		/// @brief        constructor
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return       
		/// @param        tablename as const string &
		/// @param        delim as const string &
		///
		/// @date         20.4.2010 10:39
		/// @ingroup			constructors
		///
		sel(String^ tablename, String^ delim)
			: _distinct(false)
			, _limit(0)
			, _offset(0)
			, _results(gcnew split())
			, _sources(gcnew split())
			, _groupBy(gcnew split())
			, _orderBy(gcnew split())
			, _where(gcnew String("True"))
			, _having(gcnew String(""))
			, _delim1(gcnew String(delim))
			, _delim2(gcnew String(delim))
		{
			source(tablename, nullptr);
		}

		/// @brief        constructor
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return       
		/// @param        tablename as const string &
		/// @param        delim1 as const string &
		/// @param        delim2 as const string &
		///
		/// @date         20.4.2010 10:39
		/// @ingroup			constructors
		///
		sel(String^ tablename, String^ delim1, String^ delim2)
			: _distinct(false)
			, _limit(0)
			, _offset(0)
			, _results(gcnew split())
			, _sources(gcnew split())
			, _groupBy(gcnew split())
			, _orderBy(gcnew split())
			, _where(gcnew String("True"))
			, _having(gcnew String(""))
			, _delim1(delim1)
			, _delim2(delim2)
		{
			source(tablename, nullptr);
		}


		/// @brief        copy constructor
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return       
		/// @param        s as const sel &
		///
		/// @date         20.4.2010 10:39
		/// @ingroup			constructors
		///
		sel(sel^ const s)
			: _distinct(s->_distinct)
			, _limit(s->_limit)
			, _offset(s->_offset)
			, _results(s->_results)
			, _sources(s->_sources)
			, _groupBy(s->_groupBy)
			, _orderBy(s->_orderBy)
			, _where(s->_where)
			, _having(s->_having)
			, _delim1(s->_delim1)
			, _delim2(s->_delim2)
			, _join(s->_join)
		{

		}

		/// @brief        destructor
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return       
		///
		/// @date         20.4.2010 10:41
		///
		virtual ~sel()
		{

		}

		/// @brief        adds a column name
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return       sel & - itself
		/// @param        s as const string & - column name
		///
		/// @date         20.4.2010 10:51
		/// @ingroup			operators
		///
		virtual sel^ operator,(String^ s)
		{
			return result(s, nullptr);
		}

		/// @brief        adds a where expression
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return       sel & - itself
		/// @param        e as const expr::base & - where expression
		///
		/// @date         20.4.2010 10:53
		/// @ingroup			operators
		///
		virtual sel^ operator%(expr::base_^ e)
		{
			return where(e->str());
		}

		/// @brief        joins two fields
		///
		/// <BR>qualifier
		/// <BR>access    virtual public  
		/// @return       sel & - itself
		/// @param        f as const FieldPair &
		///
		/// @date         20.4.2010 10:56
		/// @ingroup			operators
		///
		virtual sel^ operator<(FieldPair^ f)
		{
			return join(f);
		}

		/// @brief     distinct
		///
		/// <BR>qualifier
		/// <BR>access    virtual public  
		/// @return    sel &
		/// @param     d as bool
		///
		/// @date      20:2:2009   14:02
		///
		virtual sel^ distinct(bool d)
		{
			_distinct = d;
			return this;
		}

		/// @brief     limit
		///
		/// <BR>qualifier
		/// <BR>access    virtual public  
		/// @return    sel &
		/// @param     value as int
		///
		/// @date      20:2:2009   14:02
		///
		virtual sel^ limit(int value)
		{
			_limit = value;
			return this;
		}

		/// @brief     join
		///
		/// <BR>qualifier
		/// <BR>access    virtual public  
		/// @return    sel &
		/// @param     f as const FieldPair&
		///
		/// @date      20:2:2009   14:02
		///
		virtual sel^ join(FieldPair^ f)
		{
			_join = f;
			return this;
		}

		/// @brief     offset
		///
		/// <BR>qualifier
		/// <BR>access    virtual public  
		/// @return    sel &
		/// @param     value as int 
		///
		/// @date      20:2:2009   14:02
		///
		virtual sel^ offset(int value)
		{
			_offset = value;
			return this;
		}

		/// @brief     result_no_delim
		///
		/// <BR>qualifier
		/// <BR>access    virtual public  
		/// @return    sel &
		/// @param     r as string
		/// @param     alias as string
		///
		/// @date      20:2:2009   14:02
		///
		virtual sel^ result_no_delim(String^ r, String^ alias/*=DB_TEXT("")*/)
		{
			if (alias && alias->Length)
				r += DB_TEXT(" AS ") + alias;
			_results->Add(r); 
			return this;
		}

		/// @brief     result
		///
		/// <BR>qualifier
		/// <BR>access    virtual public  
		/// @return    sel &
		/// @param     r as string
		/// @param     alias as string
		///
		/// @date      20:2:2009   14:02
		///
		virtual sel^ result(String^ r, String^ alias/*=DB_TEXT("")*/)
		{
			if (r != "*") 
				r = detail::front_back_delim(r, _delim1, _delim2);
			if (alias && alias->Length)
				r += " AS " + detail::front_back_delim(alias, _delim1, _delim2);
			_results->Add(r); 
			return this;
		}

		/// @brief     result_max
		///
		/// <BR>qualifier
		/// <BR>access    virtual public  
		/// @return    sel &
		/// @param     r as string
		/// @param     alias as string
		///
		/// @date      20:2:2009   14:02
		///
		virtual sel^ result_max(String^ r, String^ alias/*=DB_TEXT("")*/)
		{
			if (r != "*") 
				r = detail::front_back_delim(r, _delim1, _delim2);

			r = "MAX(" + r + ")";

			if (alias && alias->Length)
				r += " AS " + detail::front_back_delim(alias, _delim1, _delim2);

			_results->Add(r); 
			return this;
		}

		/// @brief     clear
		///
		/// <BR>qualifier
		/// <BR>access    virtual public  
		/// @return    sel &
		///
		/// @date      20:2:2009   14:01
		///
		virtual void clear()
		{
			_results->Clear();
			_sources->Clear();
		}

		/// @brief     source
		///
		/// <BR>qualifier
		/// <BR>access    virtual public  
		/// @return    sel &
		/// @param     s as string
		/// @param     alias as string
		///
		/// @date      20:2:2009   14:01
		///
		virtual sel^ source(String^ s, String^ alias/*=DB_TEXT("")*/)
		{
			s = detail::front_back_delim(s, _delim1, _delim2);

			if (alias && alias->Length)
				s += " AS " + detail::front_back_delim(alias, _delim1, _delim2);

			_sources->Add(s);
			return this;
		}

		/// @brief     where
		///
		/// <BR>qualifier
		/// <BR>access    virtual public  
		/// @return    sel &
		/// @param     w as const expr::base_ &
		///
		/// @date      20:2:2009   14:01
		///
		virtual sel^ where(expr::base_^ w)
		{
			expr::and_^ a = gcnew expr::and_(gcnew expr::raw(_where), w);
			_where = a->str();	
			return this;
		}

		/// @brief     where
		///
		/// <BR>qualifier
		/// <BR>access    virtual public  
		/// @return    sel &
		/// @param     w as string
		///
		/// @date      20:2:2009   14:01
		///
		virtual sel^ where(String^ w)
		{
			expr::and_^ a = gcnew expr::and_(gcnew expr::raw(_where), gcnew expr::raw(w));
			_where = a->str();
			return this;
		}

		/// @brief     group_by
		///
		/// <BR>qualifier
		/// <BR>access    virtual public  
		/// @return    sel &
		/// @param     gb as string
		///
		/// @date      20:2:2009   14:01
		///
		virtual sel^ group_by(String^ gb)
		{
			gb = detail::front_back_delim(gb, _delim1, _delim2);
			_groupBy->Add(gb);	
			return this;
		}

		/// @brief     having
		///
		/// <BR>qualifier
		/// <BR>access    virtual public  
		/// @return    sel &
		/// @param     h as const expr::base_ &
		///
		/// @date      20:2:2009   14:01
		///
		virtual sel^ having(expr::base_^ h)
		{
			_having = h->str(); 
			return this;
		}

		/// @brief     having
		///
		/// <BR>qualifier
		/// <BR>access    virtual public  
		/// @return    sel &
		/// @param     h as string
		///
		/// @date      20:2:2009   14:01
		///
		virtual sel^ having(String^ h)
		{
			_having = h; 
			return this;
		}

		/// @brief     order_by
		///
		/// <BR>qualifier
		/// <BR>access    virtual public  
		/// @return    sel &
		/// @param     ob as string
		/// @param     ascending as bool 
		///
		/// @date      20:2:2009   14:00
		///
		virtual sel^ order_by(String^ ob, bool ascending/*=true*/)
		{
			ob = detail::front_back_delim(ob, _delim1, _delim2);
			if (!ascending)
				ob += " DESC";
			_orderBy->Add(ob); 
			return this;
		}

		/// @brief        db::string overload
		///
		/// <BR>qualifier const
		/// <BR>access    virtual public  
		/// @return       
		///
		/// @date         20.4.2010 10:57
		/// @ingroup			operators
		///
		virtual operator String^()
		{
			return str();
		}

		/// @brief        same as the string operator overload, more stl like 
		///
		/// <BR>qualifier const
		/// <BR>access    public  
		/// @return       db::string
		///
		/// @date         15.4.2010 13:59
		///
		virtual String^ str()
		{
			String^ res = gcnew String("SELECT ");

			if (_distinct)
				res += "DISTINCT ";
			res += _results->join(",");
			res += " FROM ";
			res += _sources->join(",");

			if (_join 
				&& _join->Key->table()->Length != 0 
				&& _join->Value->table()->Length != 0 
				&& _sources->Count == 1)
			{
				res += " INNER JOIN ";
				res += _join->Key->table();
				res += " ON ";
				res += _join->Key->fullName();
				res += " = ";
				res += _join->Value->fullName();
				res += " ";
			}

			if (_where != "True")
				res += " WHERE " + _where;
			if (_groupBy->Count > 0)
				res += " GROUP BY " + _groupBy->join(",");
			if (_having->Length)
				res += " HAVING " + _having;
			if (_orderBy->Count > 0)
				res += " ORDER BY " + _orderBy->join(",");
			if (_limit) 
				res += " LIMIT " + System::Convert::ToString(_limit);
			if (_offset) 
				res += " OFFSET " + System::Convert::ToString(_offset);

			if (_delim1->Length > 0)
			{
				res->Replace(_delim1+_delim1, _delim1);
				if (_delim1 != _delim2)
					res->Replace(_delim2+_delim2, _delim2);
			}

			return res;
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

		/// @brief        Table
		///
		/// <BR>qualifier
		/// <BR>access    public   
		/// @return       String^ const
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         24.10.2013 11:03
		/// 
		String^ const Table() { return table; }
		
		/// @brief        Table
		///
		/// <BR>qualifier
		/// <BR>access    public   
		/// @return       void
		/// @param        String ^ val
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         24.10.2013 11:03
		/// 
		void Table(String^ val) { table = val; }

		/// @brief        
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return       upd^
		/// @param        e as expr::base_^
		///
		/// @date         15.4.2010 13:55
		///
		virtual upd^ where(expr::base_^ w)
		{
			expr::and_^ a = gcnew expr::and_(gcnew expr::raw(_where), w);
			_where = a->str();	
			return this;
		}

		/// @brief        insert a new field with it name and the value
		///
		/// <BR>qualifier
		/// <BR>access    public   
		/// @return       upd^
		/// @param        field ^ f
		/// @param        String ^ val
		///
		/// @author       T. Schroeder (explodus@gmx.de)
		/// @date         24.10.2013 10:03
		/// 
		upd^ set(field^ f, String^ val)
		{
			fields.Add(f->name());
			if ( f->type()==param_types::e_string
				|| f->type()==param_types::e_char
				|| f->type()==param_types::e_blob
				|| f->type()==param_types::e_date_time)
				values.Add(detail::escape_sql(val));
			else
				values.Add(val);
			return this;
		}

		/// @brief        insert a new field with its name and the value
		///
		/// <BR>qualifier
		/// <BR>access    public  
		/// @return       upd&
		/// @param        f as const field &
		/// @param        val as const T &
		///
		/// @date         15.4.2010 13:58
		///
		template<class T>
		upd^ set(field^ f, T val)
		{ return set(f, System::Convert::ToString(val)); }

		template<>
		upd^ set(field^ f, Int32 val)
		{ return set(f, System::Convert::ToString(val)); }

		template<>
		upd^ set(field^ f, Double val)
		{ return set(f, System::Convert::ToString(val)); }

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

inline clr_db::expr::in_^ clr_db::field::In(String^ set)
{
	return gcnew clr_db::expr::in_(this, set);
}

inline clr_db::expr::in_^ clr_db::field::In(clr_db::sel^ sel)
{
	return gcnew clr_db::expr::in_(this, sel); 
}

inline clr_db::expr::like^ clr_db::field::Like(String^ const s)
{
	return gcnew clr_db::expr::like(this, s); 
}

inline clr_db::expr::in_::in_(clr_db::field^ const fld, clr_db::sel^ s)
	: clr_db::expr::oper(fld, gcnew String("IN")
	, gcnew String("(" + s->str() + ")"))
{

}

clr_db::expr::and_^ clr_db::expr::base_::operator&(clr_db::expr::base_^ o)
{ 
	return gcnew clr_db::expr::and_(this, o); 
}

clr_db::expr::and_^ operator&(clr_db::expr::base_^ o1, clr_db::expr::base_^ o2)
{ 
	return gcnew clr_db::expr::and_(o1,o2); 
}
