///	@file test/log.cpp
///  
///	@date 16.3.2010 19:51
///  
/// @author T. Schroeder (explodus@gmx.de)
///  
/// @brief description what this file is for  
///	@brief interface of the db class 
///  
/// This file is part of the sqlite-wrapper project
///

#include <sqlite_wrapper/config.hpp>
#include <sqlite_wrapper/log.hpp>

#include <boost/make_shared.hpp>

typedef db::log::base
<
  db::log::provider::basic<db::log::buffer::file<db::ofstream> >
, db::log::format::basic 
> 
log_type;

log_type log_instance
(
	boost::make_shared
	<
		log_type::provider_type
	>
	(
		boost::make_shared
		<
			db::log::buffer::basic
		>
		(
			DB_TEXT("log.log")
		)
	)
);

int main( int argc, char **argv )
{

}

