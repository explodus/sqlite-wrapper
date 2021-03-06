MACRO(MAKE_SQLITE_WRAPPER_LIBRARY TARGETNAME VARIANT)
	ADD_DEFINITIONS(-DSQLITE_WRAPPER_SHARED -DSQLITE_WRAPPER_USE=1)
	ADD_LIBRARY (${TARGETNAME} ${VARIANT} ${sqlite_wrapper_files} ${sqlite_wrapper_source})

	SOURCE_GROUP( "Header Files\\impl" FILES ${sqlite_wrapper_impl_all} )
	SOURCE_GROUP( "Header Files\\detail" FILES ${sqlite_wrapper_header_detail} )
	SOURCE_GROUP( "Header Files" FILES ${sqlite_wrapper_header_top} )
	SOURCE_GROUP( "Source Files" FILES ${sqlite3_source_top} )
	SOURCE_GROUP( "Source Files" FILES ${sqlite_wrapper_src_all} )

	#SET_TARGET_PROPERTIES(${TARGETNAME} PROPERTIES VERSION ${SQLITE_WRAPPER_VERSION})

	#IF(${TARGETNAME} STREQUAL sqlite_wrapper)
	#	SET_TARGET_PROPERTIES(
	#		${TARGETNAME} PROPERTIES SOVERSION ${SQLITE_WRAPPER_SHARED_VERSION}
	#	)
	#ENDIF()

	SET_TARGET_PROPERTIES(
#		${TARGETNAME} PROPERTIES OUTPUT_NAME sqlite_wrapper
		${TARGETNAME} PROPERTIES OUTPUT_NAME_RELEASE sqlite_wrapper
		${TARGETNAME} PROPERTIES OUTPUT_NAME_DEBUG sqlite_wrapper_debug
		${TARGETNAME} PROPERTIES OUTPUT_NAME_RELEASESTATIC sqlite_wrapper_st
		${TARGETNAME} PROPERTIES OUTPUT_NAME_DEBUGSTATIC sqlite_wrapper_st_debug
		${TARGETNAME} PROPERTIES OUTPUT_NAME_MINSIZEREL sqlite_wrapper_minsizerel
		${TARGETNAME} PROPERTIES OUTPUT_NAME_RELWITHDEBINFO sqlite_wrapper_relwithdebinfo
		LINK_FLAGS "/MANIFEST:NO"
	)
	
	IF (NOT MSVC)
		TARGET_LINK_LIBRARIES (
			${TARGETNAME}
			${Boost_LIBRARIES}
			dl
		)
	ENDIF ()

	IF(SQLITE_WRAPPER_HAVE_CLOCK_GETTIME)
		TARGET_LINK_LIBRARIES(${TARGETNAME} rt)
	ENDIF()

	INSTALL (TARGETS ${TARGETNAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
ENDMACRO()

MAKE_SQLITE_WRAPPER_LIBRARY(sqlite_wrapper SHARED)
