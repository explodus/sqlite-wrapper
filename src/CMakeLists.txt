
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
	${SQLITE_WRAPPER_INCLUDE_DIR}/table.hpp
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
	${SQLITE_WRAPPER_INCLUDE_DIR}/impl/table.ipp
	${SQLITE_WRAPPER_INCLUDE_DIR}/impl/time.ipp
	${SQLITE_WRAPPER_INCLUDE_DIR}/impl/upd.ipp
	${SQLITE_WRAPPER_INCLUDE_DIR}/impl/w2a.ipp
	)

FILE (GLOB H_FILES *.hpp )
FILE (GLOB H_FILES *.ipp )
SOURCE_GROUP( "Header Files\\impl" FILES ${sqlite_wrapper_impl_all} )
SOURCE_GROUP( "Header Files\\detail" FILES ${sqlite_wrapper_header_detail} )
SOURCE_GROUP( "Header Files" FILES ${sqlite_wrapper_header_top} )
SOURCE_GROUP( "Source Files" FILES ${sqlite3_source_top} )

SET( sqlite_wrapper_source_top "" )

SET( sqlite3_source_top
	${CMAKE_SOURCE_DIR}/src/sqlite/sqlite3.c
	${CMAKE_SOURCE_DIR}/src/sqlite/sqlite3.h
	${CMAKE_SOURCE_DIR}/src/sqlite/sqlite3ext.h
	)

SET( sqlite_wrapper_header
	${sqlite_wrapper_header_top}
	)

SET( sqlite_wrapper_source
	${sqlite_wrapper_source_top}
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
	${sqlite3_header_top}
	)

SET( sqlite_wrapper_source
	${sqlite_wrapper_source_top}
	${sqlite3_source_top}
	)

SET (sqlite_wrapper_files ${sqlite_wrapper_source} ${sqlite_wrapper_header} )

SET( sqlite_wrapper_files_string ${files_string} PARENT_SCOPE )

ADD_SUBDIRECTORY (shared)
ADD_SUBDIRECTORY (static)

