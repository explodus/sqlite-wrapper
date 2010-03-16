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

}

#endif