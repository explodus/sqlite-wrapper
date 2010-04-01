///	@file		include/sqlite_wrapper/detail/param.hpp
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

#ifndef DB_PARAM_HPP_INCLUDED
#define DB_PARAM_HPP_INCLUDED

#include <sqlite_wrapper/config.hpp>
#include <sqlite_wrapper/detail/tools.hpp>
#include <boost/date_time.hpp>

struct sqlite3;
struct sqlite3_stmt;

namespace db 
{

	/// @enum  db::param_types
	/// @brief Parameter Typen 
	enum param_types
	{
		e_null,       //!< \a null type
		e_bool,       //!< \a boolean type
		e_int,        //!< \a integer type
		e_unsigned,   //!< \a unsigned integer type
		e_long,       //!< \a long type
		e_float,      //!< \a float type
		e_double,     //!< \a double type
		e_char,       //!< \a char type
		e_string,     //!< \a std::string type
		e_blob,       //!< \a blob type (big string)
		e_date_time   //!< \a boost::gregorian::date type
	}; 

	inline param_types tp(const bool&)
	{ return e_bool; }
	inline param_types tp(const int&)
	{ return e_int; }
	inline param_types tp(const unsigned&)
	{ return e_unsigned; }
	inline param_types tp(const long&)
	{ return e_long; }
	inline param_types tp(const float&)
	{ return e_float; }
	inline param_types tp(const double&)
	{ return e_double; }
	inline param_types tp(const char&)
	{ return e_char; }
	inline param_types tp(const std::string&, bool blob=false)
	{ return blob?e_blob:e_string; }
	inline param_types tp(const std::wstring&, bool blob=false)
	{ return blob?e_blob:e_string; }
#ifndef BOOST_NO_STD_LOCALE
	inline param_types tp(const boost::gregorian::date &)
	{ return e_date_time; } 
#else
	inline param_types tp(const time_t_ce &)
	{ return e_date_time; } 
#endif

	/// @brief Parameter Extras
	enum field_extra
	{
		e_nullExtra         = 0,  //!< null extra
		eFixedField         = 2,  //!< Feld mit fester Größe (numerischer Typ)
		eVariableField      = 4,  //!< variables Feld (textlicher Typ)
		eAutoIncrField      = 8,  //!< automatisch erhöhendes Feld
		eUpdatableField     = 16, //!< veränderbares Feld
		eRequiredField      = 32, //!< benötigtes Feld
		eZeroLengthAllowed  = 64  //!< Nulllänge erlaubt
	};

  ///dynamic parameter class
  class SQLITE_WRAPPER_DLLAPI param 
//#ifndef BOOST_NO_STD_LOCALE
//    : sigb::trackable
//#endif // _DEBUG
  {
    string _data;
    param_types _type;
    bool _is_changed;
    unsigned _col; 

  public:
		/// @todo correct this...
		param();

    /// @brief     param
    ///
    /// <BR>qualifier : _col(col), _type(e_null), _is_changed(false)
    /// <BR>access    public  
    /// @return    
    /// @param     col as unsigned
    ///
    /// @date      20:2:2009   14:36
    ///
    param(unsigned col);

    /// @brief     ~param
    ///
    /// <BR>qualifier
    /// <BR>access    virtual public  
    /// @return    
    ///
    /// @date      20:2:2009   14:35
    ///
    ~param();

    /// @brief     copy constructor
    ///
    /// <BR>qualifier : _col(Param._col), _type(Param._type), _is_changed(false), _data(Param._data)
    /// <BR>access    public  
    /// @return    
    /// @param     Param as const param &
    ///
    /// @date      20:2:2009   14:35
    ///
    param(const param& Param);               

    ///
    /// <BR>qualifier const 
    /// <BR>access    public  
    /// @return    const param_types&
    ///
    /// @date      20:2:2009   14:35
    ///
    param_types get_type() const;

    /// @brief     set_type
    ///
    /// <BR>qualifier 
    /// <BR>access    public  
    /// @return    void
    /// @param     Type as param_types
    ///
    /// @date      20:2:2009   14:35
    ///
    void set_type(param_types Type);

    /// @brief     is param changed?
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    bool&
    ///
    /// @date      20:2:2009   14:35
    ///
    bool& is_changed();

    ///
    /// <BR>qualifier
    /// <BR>access    virtual public  
    /// @return    void
    /// @param     stm as sqlite3_stmt *
    ///
    /// @date      20:2:2009   14:34
    ///
    void set(sqlite3_stmt* stm);

    ///gesetzt als bool
    void set(bool dat); 
    ///gesetzt als int
    void set(int dat); 
    ///gesetzt als unsigned
    void set(unsigned dat); 
    ///gesetzt als long
    void set(long dat); 
    ///gesetzt als float
    void set(float dat); 
    ///gesetzt als double
    void set(double dat); 
    ///gesetzt als char
    void set(char dat); 
    ///gesetzt als const db::char_type*
    void set(const db::char_type* dat); 
    ///gesetzt als const string&
    void set(const string& dat, bool isBlob=false); 

#ifndef BOOST_NO_STD_LOCALE
    ///gesetzt als boost::gregorian::date
		void set(const boost::gregorian::date& dat); 
#else
    ///gesetzt als time_t_ce
    void set(const time_t_ce& dat); 
#endif

    /// @brief     set_null
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    void
    ///
    /// @date      20:2:2009   14:39
    ///
    void set_null();

    /// @brief     set_with_old_type
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    void
    /// @param     data as const string &
    ///
    /// @date      20:2:2009   14:40
    ///
    void set_with_old_type(const string& data);

    /// @brief     set_with_old_type
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    void
    /// @param     data as const param &
    ///
    /// @date      20:2:2009   14:40
    ///
    void set_with_old_type(const param& data);

    /// @brief     set_with_old_type
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    void
    /// @param     data as const param *
    ///
    /// @date      20:2:2009   14:40
    ///
    void set_with_old_type(const param* data);

    /// @brief     is_null
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    bool
    ///
    /// @date      20:2:2009   14:33
    ///
    bool is_null() const;

    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    bool
    ///
    /// @date      20:2:2009   14:40
    ///
    bool get_bool() const; 


    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    int
    ///
    /// @date      20:2:2009   14:40
    ///
    int get_int() const;

    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    unsigned
    ///
    /// @date      20:2:2009   14:42
    ///
    unsigned get_unsigned() const;

    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    long
    ///
    /// @date      11:3:2009   13:56
    ///
    long get_long() const;

    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    float
    ///
    /// @date      11:3:2009   13:56
    ///
    float get_float() const;

    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    double
    ///
    /// @date      11:3:2009   13:57
    ///
    double get_double() const;

    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    char
    ///
    /// @date      11:3:2009   13:57
    ///
    db::char_type get_char() const;

    operator string() const;

    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    db::string
    ///
    /// @date      18:2:2009   11:33
    ///
    string& str();

    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    const string&
    ///
    /// @date      18:2:2009   11:33
    ///
    const string& str() const;

#ifdef _UNICODE
    const wchar_t* c_str() const;
#else
    const char* c_str() const;
#endif // _UNICODE

#ifndef BOOST_NO_STD_LOCALE
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    boost::gregorian::date
    ///
    /// @date      18:2:2009   11:34
    ///
    boost::gregorian::date get_date_time() const;
#else
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    db::time_t_ce
    /// @param     tmp as const time_t_ce *
    ///
    /// @date      19:3:2009   10:36
    ///
    time_t_ce get_date_time(const time_t_ce* tmp = 0) const;
#endif

#ifdef _MFC_VER
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    COleDateTime
    /// @param     tmp as const COleDateTime *
    ///
    /// @date      19:3:2009   10:36
    ///
    COleDateTime get_date_time(const COleDateTime* tmp = 0) const;
#endif // _MFCVER

    /// @brief     operator=
    ///
    /// <BR>qualifier
    /// <BR>access    public  
    /// @return    const param&
    /// @param     v as const param &
    ///
    /// @date      20:2:2009   9:24
    ///
    const param& operator=( const param &v );

    /// @brief     operator!
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    bool
    ///
    /// @date      20:2:2009   9:27
    ///
    bool operator!() const;

    /// @brief     operator==
    ///
    /// <BR>qualifier const
    /// <BR>access    public  
    /// @return    bool
    /// @param     v as const param &
    ///
    /// @date      20:2:2009   9:24
    ///
    bool operator== (const param& v) const;

//#ifndef BOOST_NO_STD_LOCALE
//    typedef boost::signal2<void, const string&, bool&> type_signal_external_check; 
//    type_signal_external_check signal_external_check;
//#endif // BOOST_NO_STD_LOCALE
  };
	//typedef boost::detail::allocator::partial_std_allocator_wrapper<
	//	boost::detail::quick_allocator<param > > alloc_param; 
	typedef std::allocator<param > alloc_param; 

  namespace detail
  {
    template<> 
    inline string to_string(const db::param& a) 
    { return a.str(); } 
	}

}

#endif