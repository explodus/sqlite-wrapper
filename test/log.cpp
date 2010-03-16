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
  db::log::provider::basic<db::log::buffer::basic >
, db::log::format::basic 
> 
log_type;

log_type instance
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
		( )
	)
);

int main( int argc, char **argv )
{
	// set to debug level
	db::log::global_level = db::log::log_debug;

	db::log::scope<log_type> scope_(instance, DB_TEXT("main()"));


}

