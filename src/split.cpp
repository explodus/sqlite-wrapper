///	@file		src/query.cpp
///	@date		15.3.2010 14:23
///	file base	log_level
///	file ext	cpp
///
/// @author		T. Schroeder (explodus@gmx.de)
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
/// @brief 
///
/// This file is part of the sqlite-wrapper project

#include <sqlite_wrapper/config.hpp>
#include <sqlite_wrapper/db.hpp>

db::split_map::split_map() 
{ 
}

db::split_map::split_map(std::map<db::string, db::string>& data) 
	: std::map<db::string, db::string>(data) 
{

}

db::string db::split_map::join_fields(db::string delim) const
{
	string res;
	for (const_iterator i = begin(); i != end(); i++)
	{
		if (i != begin())
			res += delim;
		res += i->first;
	}
	return res; 
}

db::string db::split_map::join_values(db::string delim) const
{
	string res;
	for (const_iterator i = begin(); i != end(); i++)
	{
		if (i != begin())
			res += delim;
		if(i->second.length()!=0)
			res += i->second;
		else
			res += DB_TEXT("NULL");
	}
	return res; 
}

db::split::split() 
{

}

db::split::split(std::vector<db::string> data) 
	: std::vector<db::string>(data) 
{

}

db::split::split(db::string s, db::string delim/*=DB_TEXT(" ")*/)
{
	if (s.length()==0)
		return;
	db::char_type *ptr(&*s.begin());
	int len(delim.length());
	std::vector<db::char_type*> pointers;
	pointers.push_back(ptr);
	
	ptr = _tcsstr(ptr, delim.c_str());
	
	while(ptr) 
	{
		*ptr = DB_TEXT('\0');
		ptr += len;
		pointers.push_back(ptr);
		(ptr = _tcsstr(ptr, delim.c_str()));
	}
	for (std::vector<db::char_type*>::iterator i(pointers.begin()), 
		e(pointers.end()); i != e; ++i)
		push_back(string(*i));
}

db::split db::split::slice(int start, int end) const
{
	std::vector<string> data;
	if (start < 0)
		start = start+size();
	if (end < 0)
		end = end+size();
	if (start >= static_cast<int>(size()))
		start = size()-1;
	if (end >= static_cast<int>(size()))
		end = size()-1;
	if (start >= end)
		return data;
	for (int i = start; i < end; i++)
		data.push_back(this->operator[](i));
	return data; 
}

db::string db::split::join(db::string delim) const
{
	string res;
	for (const_iterator i = begin(); i != end(); i++)
	{
		if (i != begin())
			res += delim;
		res += *i;
	}
	return res; 
}

db::split & db::split::extend(const db::split & s)
{
	for (size_t i = 0; i < s.size(); i++)
		push_back(s[i]);
	return *this; 
}
