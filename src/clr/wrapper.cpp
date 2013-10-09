#include <sqlite_wrapper/config.hpp>
#include <sqlite_wrapper/db.hpp>

#define NATIVEDLL_EXPORTS
#include <vcclr.h>
#include <msclr\marshal.h>
#include <msclr\marshal_cppstd.h>

using namespace System;
using namespace System::Collections::ObjectModel;
using namespace System::Data::SqlTypes;
using namespace Runtime::InteropServices;
using namespace msclr::interop;

namespace clr_db 
{
	public ref class param
	{
	private:
		System::Object^ data_;
	public:
		param()
		{

		}

		~param()
		{

		}

		System::Object^ Data() { return data_; }
		void Data(System::Object^ val) { data_ = val; }

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
				err = err;
				return ret;
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

						switch (p.get_type())
						{
						case db::e_int:
						case db::e_unsigned:
							ref_p->Data(gcnew System::Int32(p.get_int()));
							//Convert::ChangeType(ref_p, Type::GetType("System.Int32"));
							break;
						case db::e_long:
							ref_p->Data(gcnew System::Int64(p.get_long()));
							//Convert::ChangeType(ref_p, Type::GetType("System.Int64"));
							break;
						case db::e_float:
						case db::e_double:
						case db::e_date_time:
							ref_p->Data(gcnew System::Double(p.get_double()));
							//Convert::ChangeType(ref_p, Type::GetType("System.Double"));
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
}
