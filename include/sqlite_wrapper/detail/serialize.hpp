///	@file		include/sqlite_wrapper/detail/serialize.hpp
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

#ifndef DB_SERIALIZE_HPP_INCLUDED
#define DB_SERIALIZE_HPP_INCLUDED

#include <sqlite_wrapper/config.hpp>

namespace db
{
#ifdef _MFC_VER

  namespace serialize
  {
    class SQLITE_WRAPPER_DLLAPI field : public CObject
    {
      string _name;
      int    _type;

    public:
      /// @brief     field
      ///
      /// <BR>qualifier : _type(db::e_null)
      /// <BR>access    public  
      /// @return    
      ///
      ///
      field() : CObject(), _type(db::e_null) {}
      
      /// @brief     field
      ///
      /// <BR>qualifier : _type(type_), _name(name_)
      /// <BR>access    public  
      /// @return    
      /// @param     type_ as param_types
      /// @param     name_ as const string &
      ///
      /// @date      14:9:2009   10:00
      ///
      field(param_types type_, const string& name_) : CObject(),
        _type(type_), _name(name_) 
      {}

      /// @brief     field
      ///
      /// <BR>qualifier : CObject(), _type(f._type), _name(f._name)
      /// <BR>access    public  
      /// @return    
      /// @param     f as const field &
      ///
      /// @date      28:9:2009   15:17
      ///
      field(const field& f) : CObject(), _type(f._type), _name(f._name) 
      {}

      /// @brief     operator=
      ///
      /// <BR>qualifier
      /// <BR>access    public  
      /// @return    field&
      /// @param     f as const field &
      ///
      /// @date      28:9:2009   15:21
      ///
      field& operator=(const field& f)
      {
        _type = f._type;
        _name = f._name;
        return *this;
      }

      const db::string& name() const { return _name; }
      void name(const db::string& val) { _name = val; }

      param_types Type() const { return static_cast<param_types>(_type); }
      void Type(param_types val) { _type = val; }

      /// @brief     Serialize
      ///
      /// <BR>qualifier
      /// <BR>access    public  
      /// @return    void
      /// @param     ar as CArchive &
      ///
      /// @date      14:9:2009   10:00
      ///
      virtual void Serialize(CArchive& ar)
      {
        CObject::Serialize(ar);

        if( ar.IsStoring() )
        {
          ar.WriteCount(_type);
          ar << CString(_name.c_str());
#ifdef _BOOST_TEST
#ifdef _DEBUG
          OutputDebugStringW(db::detail::to_string(_type).c_str());
          OutputDebugStringW(L"\n");
          OutputDebugStringW(_name.c_str());
          OutputDebugStringW(L"\n");
#endif // _DEBUG
#endif        
        }
        else
        {
          _type = ar.ReadCount();
          CString tmp;
          ar >> tmp;
          _name = tmp;

#ifdef _BOOST_TEST
#ifdef _DEBUG
          OutputDebugStringW(db::detail::to_string(_type).c_str());
          OutputDebugStringW(L"\n");
          OutputDebugStringW(_name.c_str());
          OutputDebugStringW(L"\n");
#endif // _DEBUG
#endif        
        }
      }

      DECLARE_SERIAL(field);
    };

    typedef std::vector<db::serialize::field> t_ser_field;

    class SQLITE_WRAPPER_DLLAPI row : public CObject
    {
      string_vec _data;
      
    public:
      /// @brief     row
      ///
      /// <BR>qualifier
      /// <BR>access    public  
      /// @return    
      ///
      /// @date      14:9:2009   10:15
      ///
      row() : CObject() 
      {}

      /// @brief     row
      ///
      /// <BR>qualifier : CObject(), _data(r._data)
      /// <BR>access    public  
      /// @return    
      /// @param     r as const row &
      ///
      /// @date      28:9:2009   15:18
      ///
      row(const row& r) : CObject(), _data(r._data)
      {}

      /// @brief     ~row
      ///
      /// <BR>qualifier
      /// <BR>access    public  
      /// @return    
      ///
      /// @date      14:9:2009   10:15
      ///
      ~row() {}

      /// @brief     operator=
      ///
      /// <BR>qualifier
      /// <BR>access    public  
      /// @return    row&
      /// @param     r as const row &
      ///
      /// @date      28:9:2009   15:22
      ///
      row& operator=(const row& r)
      {
        _data = r._data;
        return *this;
      }

      db::string_vec& Data() { return _data; }
      const db::string_vec& Data() const { return _data; }

      /// @brief     Serialize
      ///
      /// <BR>qualifier
      /// <BR>access    public  
      /// @return    void
      /// @param     ar as CArchive &
      ///
      /// @date      14:9:2009   10:15
      ///
      virtual void Serialize(CArchive& ar)
      {
        CObject::Serialize(ar);

        if( ar.IsStoring() )
        {
          ar.WriteCount(_data.size());
#ifdef _BOOST_TEST
#ifdef _DEBUG
          OutputDebugStringW(db::detail::to_string(_data.size()).c_str());
          OutputDebugStringW(L"\n");
#endif
#endif          
          for (string_vec::iterator itb(_data.begin()), 
            ite(_data.end()); itb!=ite; ++itb)
          {
            ar << CString(itb->c_str());
#ifdef _BOOST_TEST
#ifdef _DEBUG
            OutputDebugStringW(itb->c_str());
            OutputDebugStringW(L"\n");
#endif // _DEBUG
#endif          
          }
        }
        else
        {
          DWORD_PTR cnts(ar.ReadCount());
#ifdef _BOOST_TEST
#ifdef _DEBUG
          OutputDebugStringW(db::detail::to_string(cnts).c_str());
          OutputDebugStringW(L"\n");
#endif
#endif          
          if (cnts>0)
          {
            _data.resize(cnts);
            
            for (DWORD_PTR cnt(0); cnt<cnts; ++cnt)
            {
              CString tmp;
              ar >> tmp;
              _data[cnt] = tmp;
#ifdef _BOOST_TEST
#ifdef _DEBUG
              OutputDebugStringW(tmp);
              OutputDebugStringW(L"\n");
#endif // _DEBUG
#endif            
            }
          }
        }
      }
      
      DECLARE_SERIAL(row);
    };

    typedef std::vector<db::serialize::row> t_ser_row;


    class SQLITE_WRAPPER_DLLAPI table : public CObject
    {
      string _name;
      t_ser_field  _fields;
      t_ser_row _rows;

    public:
      /// @brief     table
      ///
      /// <BR>qualifier
      /// <BR>access    public  
      /// @return    
      ///
      /// @date      14:9:2009   9:46
      ///
      table() : CObject() {}

      /// @brief     table
      ///
      /// <BR>qualifier : CObject(), _name(t._name), _fields(t._fields), _rows(t._rows)
      /// <BR>access    public  
      /// @return    
      /// @param     t as const table &
      ///
      /// @date      28:9:2009   15:19
      ///
      table(const table& t) : CObject(), _name(t._name), _fields(t._fields),
        _rows(t._rows)
      {}

      /// @brief     table
      ///
      /// <BR>qualifier : _name(name_)
      /// <BR>access    public  
      /// @return    
      /// @param     name_ as const db::string &
      ///
      /// @date      14:9:2009   9:46
      ///
      table(const db::string& name_) : CObject(), _name(name_) 
      {}

      /// @brief     operator=
      ///
      /// <BR>qualifier
      /// <BR>access    public  
      /// @return    table&
      /// @param     r as const table &
      ///
      /// @date      28:9:2009   15:23
      ///
      table& operator=(const table& t)
      {
        _name   = t._name; 
        _fields = t._fields;
        _rows   = t._rows;

        return *this;
      }

      const db::string& name() const { return _name; }
      void name(const db::string& val) { _name = val; }

      db::serialize::t_ser_field& fields() { return _fields; }
      const db::serialize::t_ser_field& fields() const { return _fields; }

      db::serialize::t_ser_row& rows() { return _rows; }
      const db::serialize::t_ser_row& rows() const { return _rows; }

      /// @brief     Serialize
      ///
      /// <BR>qualifier
      /// <BR>access    public  
      /// @return    void
      /// @param     ar as CArchive &
      ///
      /// @date      14:9:2009   9:46
      ///
      virtual void Serialize(CArchive& ar)
      {
        CObject::Serialize(ar);

        if( ar.IsStoring() )
        {
          ar.WriteCount(_fields.size());
          ar.WriteCount(_rows.size());
          ar << CString(_name.c_str());

#ifdef _BOOST_TEST
#ifdef _DEBUG
          OutputDebugStringW(db::detail::to_string(_fields.size()).c_str());
          OutputDebugStringW(L"\n");
          OutputDebugStringW(db::detail::to_string(_rows.size()).c_str());
          OutputDebugStringW(L"\n");
          OutputDebugStringW(_name.c_str());
          OutputDebugStringW(L"\n");
#endif
#endif
          for (t_ser_field::iterator itb(_fields.begin()), 
            ite(_fields.end()); itb!=ite; ++itb)
            itb->Serialize(ar);

          for (t_ser_row::iterator itb(_rows.begin()), 
            ite(_rows.end()); itb!=ite; ++itb)
            itb->Serialize(ar);
        }
        else
        {
          DWORD_PTR cnt_fields(ar.ReadCount());
          DWORD_PTR cnt_rows(ar.ReadCount());
          
          CString name_; 
					ar >> name_;
					if (name_.GetLength())
	          _name = name_;

#ifdef _BOOST_TEST
#ifdef _DEBUG
          OutputDebugStringW(db::detail::to_string(cnt_fields).c_str());
          OutputDebugStringW(L"\n");
          OutputDebugStringW(db::detail::to_string(cnt_rows).c_str());
          OutputDebugStringW(L"\n");
          OutputDebugStringW(_name.c_str());
          OutputDebugStringW(L"\n");
#endif
#endif          
          _fields.reserve(cnt_fields);
          for (t_ser_field::size_type cnt(0); cnt<cnt_fields; ++cnt)
          {
            _fields.push_back(db::serialize::field());
            _fields.back().Serialize(ar);
          }

          _rows.reserve(cnt_rows);
          for (t_ser_row::size_type cnt(0); cnt<cnt_rows; ++cnt)
          {
            _rows.push_back(db::serialize::row());
            _rows.back().Serialize(ar);
          }
        }
      }

      typedef std::vector<db::serialize::table> t_ser_table;

      DECLARE_SERIAL(table);
    };

    typedef std::vector<table > t_ser_table;
  }

#endif // _MFC_VER
}

#endif