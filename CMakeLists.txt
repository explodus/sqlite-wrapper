CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
 
SET (CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS ON)
SET (CMAKE_USER_MAKE_RULES_OVERRIDE_CXX ${CMAKE_CURRENT_SOURCE_DIR}/cxx_flag_overrides.cmake)
 
PROJECT (sqlite_wrapper)
 
SET( CMAKE_BUILD_TYPE Debug )
 
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
	ADD_DEFINITIONS( -D_UNICODE -DUNICODE )
ENDIF( MSVC )

ADD_DEFINITIONS( -DSQLITE_ENABLE_COLUMN_METADATA )

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
 
	SET (CMAKE_REQUIRED_FLAGS "-Wall -Werror -pedantic")
 
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
	ADD_DEFINITIONS ("/W3 /wd4512 /wd4355 /wd4800 /wd4996 /wd4714 /EHa /D_BIND_TO_CURRENT_VCLIBS_VERSION=1")
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
	"Version: 1.0\n"
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
 
FIND_PACKAGE(
	Boost 1.38.0 REQUIRED COMPONENTS
	system
	thread
)
 
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
 
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
 
SET (CMAKE_REQUIRED_INCLUDES ${Boost_INCLUDE_DIR})
 
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
 
SET( SQLITE_WRAPPER_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include/sqlite_wrapper )
  
SET( sqlite_wrapper_files_string "" )

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