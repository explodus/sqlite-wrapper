
###############################################################################
#
# Top Level
#
###############################################################################

SET( sqlite_wrapper_header_top
	${SQLITE_WRAPPER_INCLUDE_DIR}/config.hpp
	${SQLITE_WRAPPER_INCLUDE_DIR}/version.hpp
	${SQLITE_WRAPPER_INCLUDE_DIR}/cout.hpp
	${SQLITE_WRAPPER_INCLUDE_DIR}/db.hpp
	${SQLITE_WRAPPER_INCLUDE_DIR}/log.hpp
	${SQLITE_WRAPPER_INCLUDE_DIR}/log.hpp
	${SQLITE_WRAPPER_INCLUDE_DIR}/a2w.hpp
	${SQLITE_WRAPPER_INCLUDE_DIR}/w2a.hpp
	#${SQLITE_WRAPPER_INCLUDE_DIR}/table.hpp
	)

SET( sqlite_wrapper_header_detail
	${SQLITE_WRAPPER_INCLUDE_DIR}/detail/time.hpp
	${SQLITE_WRAPPER_INCLUDE_DIR}/detail/tools.hpp
	${SQLITE_WRAPPER_INCLUDE_DIR}/detail/param.hpp
	${SQLITE_WRAPPER_INCLUDE_DIR}/detail/exception.hpp
	${SQLITE_WRAPPER_INCLUDE_DIR}/detail/field.hpp
	${SQLITE_WRAPPER_INCLUDE_DIR}/detail/expr.hpp
	${SQLITE_WRAPPER_INCLUDE_DIR}/detail/row.hpp
	${SQLITE_WRAPPER_INCLUDE_DIR}/detail/query.hpp
	${SQLITE_WRAPPER_INCLUDE_DIR}/detail/sel.hpp
	${SQLITE_WRAPPER_INCLUDE_DIR}/detail/del.hpp
	${SQLITE_WRAPPER_INCLUDE_DIR}/detail/ins.hpp
	${SQLITE_WRAPPER_INCLUDE_DIR}/detail/upd.hpp
	${SQLITE_WRAPPER_INCLUDE_DIR}/detail/serialize.hpp
	)

SET( sqlite_wrapper_impl_all
	${SQLITE_WRAPPER_INCLUDE_DIR}/impl/a2w.ipp
	${SQLITE_WRAPPER_INCLUDE_DIR}/impl/cout.ipp
	${SQLITE_WRAPPER_INCLUDE_DIR}/impl/db.ipp
	${SQLITE_WRAPPER_INCLUDE_DIR}/impl/del.ipp
	${SQLITE_WRAPPER_INCLUDE_DIR}/impl/exception.ipp
	${SQLITE_WRAPPER_INCLUDE_DIR}/impl/expr.ipp
	${SQLITE_WRAPPER_INCLUDE_DIR}/impl/field.ipp
	${SQLITE_WRAPPER_INCLUDE_DIR}/impl/ins.ipp
	${SQLITE_WRAPPER_INCLUDE_DIR}/impl/log_level.ipp
	${SQLITE_WRAPPER_INCLUDE_DIR}/impl/param.ipp
	${SQLITE_WRAPPER_INCLUDE_DIR}/impl/query.ipp
	${SQLITE_WRAPPER_INCLUDE_DIR}/impl/row.ipp
	${SQLITE_WRAPPER_INCLUDE_DIR}/impl/sel.ipp
	${SQLITE_WRAPPER_INCLUDE_DIR}/impl/split.ipp
	#${SQLITE_WRAPPER_INCLUDE_DIR}/impl/table.ipp
	${SQLITE_WRAPPER_INCLUDE_DIR}/impl/time.ipp
	${SQLITE_WRAPPER_INCLUDE_DIR}/impl/upd.ipp
	${SQLITE_WRAPPER_INCLUDE_DIR}/impl/w2a.ipp
	)
	
SET( sqlite_wrapper_src_all
	${CMAKE_SOURCE_DIR}/src/shared/a2w.cpp
	${CMAKE_SOURCE_DIR}/src/shared/cout.cpp
	${CMAKE_SOURCE_DIR}/src/shared/db.cpp
	${CMAKE_SOURCE_DIR}/src/shared/del.cpp
	${CMAKE_SOURCE_DIR}/src/shared/exception.cpp
	${CMAKE_SOURCE_DIR}/src/shared/expr.cpp
	${CMAKE_SOURCE_DIR}/src/shared/field.cpp
	${CMAKE_SOURCE_DIR}/src/shared/ins.cpp
	${CMAKE_SOURCE_DIR}/src/shared/log_level.cpp
	${CMAKE_SOURCE_DIR}/src/shared/param.cpp
	${CMAKE_SOURCE_DIR}/src/shared/query.cpp
	${CMAKE_SOURCE_DIR}/src/shared/row.cpp
	${CMAKE_SOURCE_DIR}/src/shared/sel.cpp
	${CMAKE_SOURCE_DIR}/src/shared/split.cpp
	#${CMAKE_SOURCE_DIR}/src/shared/table.cpp
	${CMAKE_SOURCE_DIR}/src/shared/time.cpp
	${CMAKE_SOURCE_DIR}/src/shared/upd.cpp
	${CMAKE_SOURCE_DIR}/src/shared/w2a.cpp
	)

FILE (GLOB H_FILES *.hpp )
FILE (GLOB H_FILES *.cpp )
#SOURCE_GROUP( "Header Files\\impl" FILES ${sqlite_wrapper_impl_all} )
#SOURCE_GROUP( "Header Files\\detail" FILES ${sqlite_wrapper_header_detail} )
#SOURCE_GROUP( "Header Files" FILES ${sqlite_wrapper_header_top} )
#SOURCE_GROUP( "Source Files" FILES ${sqlite3_source_top} )

SET( sqlite_wrapper_source_top "" )

IF (ENABLE_ENCRYPTION)
	SET( sqlite3_source_top
		${SQLITE3_SRC_DIR}/sha2.c
		${SQLITE3_SRC_DIR}/sqlite3secure.c
	)
ELSE ()
	SET( sqlite3_source_top
		${SQLITE3_SRC_DIR}/sqlite3.c
	)
ENDIF()

SET( sqlite_wrapper_header
	${sqlite_wrapper_header_top}
	)

SET( sqlite_wrapper_source
	${sqlite_wrapper_source_top}
	${sqlite_wrapper_src_all}
	)

SET (sqlite_wrapper_files ${sqlite_wrapper_header} ${sqlite_wrapper_header_detail} ${sqlite_wrapper_impl_all})

SET( files_string "" )

FOREACH( file IN LISTS sqlite_wrapper_files )
	SET( files_string "${files_string} ${file}")
ENDFOREACH()

SET( sqlite_wrapper_header
	${sqlite_wrapper_header_top}
	${sqlite_wrapper_header_detail}
	${sqlite_wrapper_impl_all}
	${SQLITE3_INCLUDE_DIR}/sqlite3.h
	${SQLITE3_INCLUDE_DIR}/sqlite3ext.h
	)

SET( sqlite_wrapper_source
	${sqlite_wrapper_source_top}
	${sqlite3_source_top}
	${sqlite_wrapper_src_all}
	)

SET (sqlite_wrapper_files ${sqlite_wrapper_source} ${sqlite_wrapper_header} )

SET( sqlite_wrapper_files_string ${files_string} PARENT_SCOPE )

INCLUDE_DIRECTORIES(${SQLITE3_INCLUDE_DIR})

ADD_SUBDIRECTORY (shared)
ADD_SUBDIRECTORY (static)
ADD_SUBDIRECTORY (clr)

