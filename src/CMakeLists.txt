
###############################################################################
#
# Top Level
#
###############################################################################

SET( sqlite_wrapper_header_top
	${SQLITE_WRAPPER_INCLUDE_DIR}/config.hpp
	${SQLITE_WRAPPER_INCLUDE_DIR}/db.hpp
	)

SOURCE_GROUP( "Header Files" FILES
	${sqlite_wrapper_header_top}
	)

SET( sqlite_wrapper_source_top
	${CMAKE_SOURCE_DIR}/src/cout.cpp
	)

SOURCE_GROUP( "Source Files" FILES
	${sqlite_wrapper_source_top}
	)

SET( sqlite_wrapper_header
	${sqlite_wrapper_header_top}
	)

SET( sqlite_wrapper_source
	${sqlite_wrapper_source_top}
	)


SET (sqlite_wrapper_files ${sqlite_wrapper_source} ${sqlite_wrapper_header} )

SET( files_string "" )

FOREACH( file IN LISTS sqlite_wrapper_files )
	SET( files_string "${files_string} ${file}")
ENDFOREACH()

SET( sqlite_wrapper_files_string ${files_string} PARENT_SCOPE )

ADD_LIBRARY (sqlite_wrapper STATIC ${sqlite_wrapper_files})

IF (NOT MSVC)
	TARGET_LINK_LIBRARIES (
		sqlite_wrapper
		${Boost_LIBRARIES}
	)
ENDIF ()

INSTALL (TARGETS sqlite_wrapper DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
