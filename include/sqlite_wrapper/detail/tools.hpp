///	@file		include/sqlite_wrapper/detail/tools.hpp
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

#ifndef DB_DETAIL_HPP_INCLUDED
#define DB_DETAIL_HPP_INCLUDED

#include <sqlite_wrapper/config.hpp>

#ifdef _MFC_VER
#include <ATLComTime.h>
#endif

namespace db { namespace detail 
{
	/// @brief     replace_all
	/// @todo need documentation
	///
	/// <BR>qualifier
	/// <BR>access    public  
	/// @return    typename S::size_type
	/// @param     s as S & 
	/// @param     from as const S & 
	/// @param     to as const S & 
	///
	/// @date      20:2:2009   9:53
	///
	template<typename S>
	inline typename S::size_type replace_all(
		  S& s
		, const S& from
		, const S& to)
	{
		typedef typename S::size_type st;
		st cnt(S::npos);

		if(from != to && !from.empty()) {
			st pos1(0), pos2(0);
			const st from_len(from.size()), to_len(to.size());
			cnt = 0;

			while((pos1 = s.find(from, pos2)) != S::npos) {
				s.replace(pos1, from_len, to);
				pos2 = pos1 + to_len;
				++cnt;
			}
		}

		return cnt;
	}

	/// @brief     erase_all
	/// @todo need documentation
	///
	/// <BR>qualifier
	/// <BR>access    public  
	/// @return    typename S::size_type
	/// @param     s as S & 
	/// @param     from as const S & 
	///
	/// @date      20:2:2009   9:53
	///
	template<typename S>
	inline typename S::size_type erase_all(S& s, const S& from)
	{
		return replace_all<S>(s, from, DB_TEXT(""));
	}

	/// @brief     replace_all_copy
	/// @todo need documentation
	///
	/// <BR>qualifier
	/// <BR>access    public  
	/// @return    S
	/// @param     s as const S & 
	/// @param     from as const S & 
	/// @param     to as const S & 
	///
	/// @date      20:2:2009   9:52
	///
	template<typename S>
	inline S replace_all_copy(const S& s, const S& from, const S& to)
	{
		S ret(s);
		replace_all<S>(ret, from, to);
		return ret;
	}

	/// @brief     erase_all_copy
	/// @todo need documentation
	///
	/// <BR>qualifier
	/// <BR>access    public  
	/// @return    S
	/// @param     s as const S & 
	/// @param     from as const S & 
	///
	/// @date      20:2:2009   9:52
	///
	template<typename S>
	inline S erase_all_copy(const S& s, const S& from)
	{
		S ret(s);
		replace_all<S>(ret, from, DB_TEXT(""));
		return ret;
	}

	/// @brief     to_string
	/// @todo need documentation
	///
	/// <BR>access    public  
	/// @return    db::string
	/// @param     a as T 
	///
	/// @date      20:2:2009   9:52
	///
	template<class T> 
	inline string to_string(const T& a) 
	{ ostringstream ost; ost << a; return ost.str(); }   

	/// @todo need documentation
	/// @brief        to_type
	///
	/// @return       typename T
	/// @param        a as const string &
	///
	/// @date         9.3.2010 9:16
	///
	/// @todo alte definiton mit typename vor der rückgabe
	/// @todo nochmal checken ob das mit vc geht
	template<typename T> 
	inline T to_type(const string& a) 
	{ istringstream ost(a); T ret; ost >> ret; return ret; }   

	/// @brief        to_string
	/// @todo need documentation
	///
	/// @return       db::string
	/// @param        a as const double &
	///
	/// @date         9.3.2010 9:16
	///
	template<> 
	inline string to_string(const double& a) 
	{ 
		ostringstream ost; 
		ost << std::setprecision(16) << a; 
		return ost.str(); 
	}   

	/// @brief        specialise
	///
	/// @return       db::string
	/// @param        a as const bool &
	///
	/// @date         9.3.2010 9:16
	///
	template<> 
	inline string to_string(const bool& a) 
	{ return string(a?DB_TEXT("1"):DB_TEXT("0")); } 

	/// @brief        specialise
	///
	/// <BR>qualifier
	/// <BR>access    public  
	/// @return       db::string
	/// @param        a as const string &
	///
	/// @date         9.3.2010 9:17
	///
	template<> 
	inline string to_string(const string& a) 
	{ return a; }   

#ifdef _MFC_VER
	/// @brief        specialise
	///
	/// <BR>qualifier
	/// <BR>access    public  
	/// @return       db::string
	/// @param        a as const CString &
	///
	/// @date         9.3.2010 9:17
	///
	template<> 
	inline string to_string(const CString& a) 
	{ return string(a); }   

	///
	/// @todo need documentation
	/// <BR>qualifier
	/// <BR>access    public  
	/// @return    db::string
	/// @param     d as const COleDateTime & 
	///
	/// @date      19:3:2009   10:28
	///
	inline string to_sql_string(const COleDateTime& d) 
	{
		ostringstream ss;
		ss << std::setw(2) << std::setfill(DB_TEXT('0'))
			<< d.GetDay() << DB_TEXT(".")
			<< std::setw(2) << std::setfill(DB_TEXT('0')) 
			<< d.GetMonth()
			<< DB_TEXT(".") << d.GetYear() << DB_TEXT(" ")
			<< d.GetHour() << DB_TEXT(":") << d.GetMinute() << DB_TEXT(":")
			<< d.GetSecond();
		return ss.str();
	}

	/// @todo need documentation
	/// <BR>qualifier
	/// <BR>access    public static  
	/// @return    COleDateTime
	/// @param     s as const string & 
	/// @param     tmp as const COleDateTime * 
	///
	/// @date      19:3:2009   10:33
	///
	static COleDateTime from_sql_string(const string& s, 
		const COleDateTime *tmp=0) 
	{
		/// @todo replace UNREFERENCED_PARAMETER
		UNREFERENCED_PARAMETER(tmp);
		COleDateTime ret;
		tm _tm = {0};
		int err = _stscanf(s.c_str(), DB_TEXT("%d.%d.%d %d:%d:%d"), 
			&_tm.tm_mday, &_tm.tm_mon, &_tm.tm_year,
			&_tm.tm_hour, &_tm.tm_min, &_tm.tm_sec);
		/// @todo replace UNREFERENCED_PARAMETER
		UNREFERENCED_PARAMETER(err);
		ret.SetDateTime(_tm.tm_year, _tm.tm_mon, _tm.tm_mday, 
			_tm.tm_hour, _tm.tm_min, _tm.tm_sec);
		if (ret.GetStatus()==COleDateTime::invalid)
			ret = to_type<double>(s);
		return ret;
	}
#endif // _MFCVER

	/// @brief     front_back_delim
	/// @todo need documentation
	///
	/// <BR>qualifier
	/// <BR>access    protected  
	/// @return    void
	/// @param     sText as T & 
	/// @param     delim1 as const T & 
	/// @param     delim2 as const T & 
	///
	/// @date      20:2:2009   9:08
	///
	template<class T> 
	inline void front_back_delim(T& sText, const T& delim1, const T& delim2)
	{ sText = delim1 + sText + delim2; }

	inline string escape_sql(const string& str)
	{ 
		if (str == DB_TEXT("NULL"))
			return str;

		string ret(detail::replace_all_copy<string>(str, 
			DB_TEXT("\'NULL\'"), DB_TEXT("NULL")));
		ret = detail::replace_all_copy<string>(str, DB_TEXT("'"), 
			DB_TEXT("''"));
		ret = DB_TEXT("'") + ret + DB_TEXT("'");
		return ret;
	}  

} }

namespace db 
{
	/// splitmap basis
	class SQLITE_WRAPPER_DLLAPI split_map : public std::map<string, string> 
	{
	public:
		/// empty splitmap
		split_map();
		/// from map
		split_map(std::map<string, string>& data);

		/// gibt Text zurück mit eingefügtem Abgrenzer
		string join_fields(string delim) const;

		string join_values(string delim) const;
	};

	///split basis
	class SQLITE_WRAPPER_DLLAPI split : public std::vector<string> 
	{
	public:
		///empty split
		split();

		///from string vector
		split(std::vector<string> data);

		///from string. split to parts using delimeter
		split(string s, string delim=DB_TEXT(" "));

		/// @todo to englisch
		/// @brief        Gibt eine Teilmenge von Texten zurück
		///								Indexe können negativ sein. 
		///								Aktueller Index is dann berechnet vom Ende von split.
		///
		/// <BR>qualifier const
		/// <BR>access    public  
		/// @return       db::split
		/// @param        start as int - Startindex
		/// @param        end as int - Endindex
		///
		/// @author       Torsten Schroeder
		/// @author       schroeder@ipe-chemnitz.de / explodus@gmx.de
		/// @date         12.3.2010 8:30
		///
		split slice(int start, int end) const;

		///gibt Text zurück mit eingefügtem Abgrenzer
		string join(string delim) const;

		///Fügt Ausdrücke ans Ende eines anderen split an
		split & extend(const split & s);
	}; 
}

#endif // DB_DETAIL_HPP_INCLUDED
