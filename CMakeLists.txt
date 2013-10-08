CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
 
SET (CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS ON)
SET (CMAKE_USER_MAKE_RULES_OVERRIDE_CXX ${CMAKE_CURRENT_SOURCE_DIR}/cxx_flag_overrides.cmake)
 
IF(MSVC)   # Do not generate manifests for the plugins - caused issues loading plugins
	SET(CMAKE_EXTRA_LINK_FLAGS "${CMAKE_EXTRA_LINK_FLAGS} /NOMANIFEST /MANIFESTFILE:"" /MANIFEST:NO /incremental:no /pdb:none")
ENDIF()

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules/")

PROJECT (sqlite_wrapper)

set (SQLITE_WRAPPER_VERSION_MAJOR 0)
set (SQLITE_WRAPPER_VERSION_MINOR 2)
 
#SET( CMAKE_BUILD_TYPE Debug )
 
IF(${CMAKE_BINARY_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
	message(FATAL_ERROR "In-source builds are not permitted. Make a separate folder for building:\nmkdir build; cd build; cmake ..\nBefore that, remove the files that cmake just created:\nrm -rf CMakeCache.txt CMakeFiles")
ENDIF()
 
INCLUDE (CheckCXXCompilerFlag)
INCLUDE (CheckCXXSourceCompiles)
INCLUDE (CheckCXXSourceRuns)
INCLUDE (FindPkgConfig)

IF (MSVC)
	OPTION (ENABLE_NARROW_STRING "Build sqlite_wrapper with std::string instead of std::wstring" OFF)
ELSE ()
	OPTION (ENABLE_NARROW_STRING "Build sqlite_wrapper with std::string instead of std::wstring" ON)
ENDIF ()
 
IF( MSVC ) # enable unicode support for Visual Studio
	#ADD_DEFINITIONS( -D_UNICODE -DUNICODE -D_CRT_SECURE_CPP_OVERLOAD_SECURE_NAMES=1 -D_SECURE_SCL=0 -D_SECURE_ATL=0 -D_HAS_ITERATOR_DEBUGGING=0)
	ADD_DEFINITIONS( -D_UNICODE -DUNICODE)
ENDIF( MSVC )

ADD_DEFINITIONS( -DSQLITE_ENABLE_COLUMN_METADATA )

OPTION (ENABLE_HEADER_ONLY "Build sqlite_wrapper header only" OFF)

IF (ENABLE_HEADER_ONLY)
	ADD_DEFINITIONS( -DSQLITE_WRAPPER_HEADER_ONLY )
ENDIF()

OPTION (ENABLE_ENCRYPTION "Build sqlite_wrapper with encryption" ON)

IF (ENABLE_ENCRYPTION)
	ADD_DEFINITIONS(-DSQLITE_ENABLE_COLUMN_METADATA -DSQLITE_HAS_CODEC -DSQLITE_DEPRECATED -DSQLITE_EXPERIMENTAL -DSQLITE_ENABLE_EXTFUNC -DSQLITE_ENABLE_RTREE)
ENDIF()

IF (ENABLE_NARROW_STRING)
       SET (SQLITE_WRAPPER_NARROW_STRING true)
ENDIF()
 
ADD_DEFINITIONS(-D BOOST_FILESYSTEM_NO_DEPRECATED)
 
IF (CMAKE_COMPILER_IS_GNUCXX)
	IF(UNIX)
		SET (SQLITE_WRAPPER_STRING_IS_UTF8 true)
	ENDIF()
 
	CHECK_CXX_COMPILER_FLAG(
		"-fvisibility=hidden"
		SQLITE_WRAPPER_HAVE_GCC_VISIBILITY
	)
 
	OPTION (ENABLE_DEBUG_WARNINGS "Enable -Wpadded and -Weffc++" OFF)
  
	IF(ENABLE_DEBUG_WARNINGS)
		SET (CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG}" -Wpadded -Weffc++")
	ENDIF()
 
	SET (CMAKE_CXX_FLAGS_RELEASE "-O3 -fomit-frame-pointer -DNDEBUG")
 
	IF (SQLITE_WRAPPER_HAVE_GCC_VISIBILITY)
		ADD_DEFINITIONS(-fvisibility=hidden)
	ENDIF ()
 
	CHECK_CXX_SOURCE_COMPILES(
		"#include <cxxabi.h>
		 #include <cstdlib>
		int main() { int status; char *name =
		abi::__cxa_demangle(typeid(int).name(),0,0,&status); std::free(name); }"
		SQLITE_WRAPPER_HAS_GNU_DEMANGLE
	)
 
	SET (CMAKE_REQUIRED_FLAGS "-Wall -Werror -pedantic -Wno-long-long")
 
	CHECK_CXX_SOURCE_COMPILES(
		"
		#define DO_PRAGMA(x) _Pragma(#x)
		#define DO_MESSAGE(x) DO_PRAGMA(message (#x))
		DO_MESSAGE(test)
		int main() {}
		"
		SQLITE_WRAPPER_HAVE_GCC_PRAGMA_MESSAGE
	)
 
	SET (CMAKE_REQUIRED_FLAGS "")
ELSEIF (MSVC)
	ADD_DEFINITIONS ("/W4 /wd4127 /wd4275 /wd4251 /wd4512 /wd4355 /wd4800 /wd4996 /wd4714 /MP2 /EHa /D_BIND_TO_CURRENT_CRT_VERSION=1")
 ENDIF ()
 
INCLUDE_DIRECTORIES (${CMAKE_SOURCE_DIR}/include)
 
SET (LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
SET (EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
 
# Create package file for relocateable package
SET (pkgfile "${CMAKE_BINARY_DIR}/sqlite_wrapper.pc")
FILE(WRITE ${pkgfile}
	"prefix=${CMAKE_INSTALL_PREFIX}\n"
	"exec_prefix=\${prefix}\n"
	"libdir=\${prefix}/lib\n"
	"includedir=\${prefix}/include\n"
	"\n"
	"Name: sqlite_wrapper\n"
	"Description: a sqlite wrapper\n"
	"Version: ${SQLITE_WRAPPER_VERSION_MAJOR}.${SQLITE_WRAPPER_VERSION_MINOR}\n"
	"Requires:\n"
	"Libs: -L\${libdir} -lsqlite_wrapper\n"
	"Cflags: -I\${includedir}/sqlite_wrapper")

INSTALL (FILES ${pkgfile} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig)
 
CHECK_CXX_SOURCE_RUNS(
	"
	#include <unistd.h>
	#include <cstdlib>
	int main()
	{
		return _POSIX_VERSION >= 200100
			? EXIT_SUCCESS
			: EXIT_FAILURE;
	}"
	SQLITE_WRAPPER_HAVE_POSIX
)
 
CHECK_CXX_SOURCE_RUNS(
	"
	#include <unistd.h>
	#include <cstdlib>
	int main()
	{
		return _POSIX_TIMERS > 0
			? EXIT_SUCCESS
			: EXIT_FAILURE;
	}"
	SQLITE_WRAPPER_HAVE_CLOCK_GETTIME
)
 
SET (Boost_ADDITIONAL_VERSIONS "1.38" "1.38.0")
 
IF(MSVC)
	SET(Boost_USE_STATIC_LIBS ON)
ENDIF()
 
FIND_PACKAGE( SQLITE3 REQUIRED )

FIND_PACKAGE(
	Boost 1.38.0 REQUIRED COMPONENTS
	date_time
	system
	thread
)
 
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
 
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
 
SET (CMAKE_REQUIRED_INCLUDES ${Boost_INCLUDE_DIR})

# check for Qt

FIND_PACKAGE( Qt4 COMPONENTS QtCore QtGui OPTIONAL )
INCLUDE( ${QT_USE_FILE} )

INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} )
LINK_DIRECTORIES( ${QT_LIBRARY_DIR} )

CHECK_CXX_SOURCE_COMPILES(
	"#include <boost/cstdint.hpp>
	int main()
	{
	   boost::uint_least64_t test;
	}"
	SQLITE_WRAPPER_HAVE_ATLEAST_UINT64
)
 
CONFIGURE_FILE(
	${CMAKE_SOURCE_DIR}/config.hpp.in
	${CMAKE_SOURCE_DIR}/include/sqlite_wrapper/config.hpp
)

CONFIGURE_FILE(
	${CMAKE_SOURCE_DIR}/version.hpp.in
	${CMAKE_SOURCE_DIR}/include/sqlite_wrapper/version.hpp
)

CONFIGURE_FILE(
	${CMAKE_SOURCE_DIR}/sqlite3.c.in
	${CMAKE_SOURCE_DIR}/src/sqlite/sqlite3.c
)
CONFIGURE_FILE(
	${CMAKE_SOURCE_DIR}/sqlite3.h.in
	${CMAKE_SOURCE_DIR}/src/sqlite/sqlite3.h
)
CONFIGURE_FILE(
	${CMAKE_SOURCE_DIR}/sqlite3ext.h.in
	${CMAKE_SOURCE_DIR}/src/sqlite/sqlite3ext.h
)
 
SET( SQLITE_WRAPPER_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include/sqlite_wrapper )
  
SET( sqlite_wrapper_files_string "" )

IF (MSVC)
	ADD_DEFINITIONS( /W4 )

	SET(CMAKE_CXX_WARNING_LEVEL 4 CACHE TYPE STRING FORCE)

	if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		STRING(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	else(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
	endif(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
	
	if(CMAKE_C_FLAGS MATCHES "/W[0-4]")
		STRING(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
	else(CMAKE_C_FLAGS MATCHES "/W[0-4]")
		SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
	endif(CMAKE_C_FLAGS MATCHES "/W[0-4]")

ENDIF ()

ADD_SUBDIRECTORY (src)
 
OPTION(ENABLE_TEST "Build the tests" ON)
 
IF (ENABLE_TEST)
	ADD_SUBDIRECTORY (test)
ENDIF ()
 
INSTALL (
	DIRECTORY include/
	DESTINATION include/
)
 
FIND_PACKAGE( Doxygen )
 
IF( DOXYGEN_FOUND )
	CONFIGURE_FILE(
		${CMAKE_SOURCE_DIR}/doc/Doxyfile.in
		${CMAKE_BINARY_DIR}/Doxyfile
		)

	SET( doc_files
		${CMAKE_BINARY_DIR}/Doxyfile 
		${CMAKE_SOURCE_DIR}/doc/sqlite_wrapper.hpp
		)
 
	add_custom_target(
		doc ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/Doxyfile
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		DEPENDS ${CMAKE_BINARY_DIR}/Doxyfile 
		COMMENT "Generating documentation"
		)
ENDIF( DOXYGEN_FOUND )