MACRO( ADD_TARGET_TEST name )
	GET_TARGET_PROPERTY( TEST_COMMAND sqlite_wrapper_test_${name} LOCATION )

	ADD_CUSTOM_TARGET(
		sqlite_wrapper_start_test_${name}
		COMMAND ${CMAKE_COMMAND} -E echo "${TEST_COMMAND}"
		COMMAND ${CMAKE_COMMAND} -E time "${TEST_COMMAND} --result-code=no --report-level=no"
		DEPENDS sqlite_wrapper_test_${name}
		)

	IF( ENABLE_AUTORUN_TEST )
		ADD_CUSTOM_COMMAND(
			TARGET sqlite_wrapper_test_${name}
			COMMAND ${CMAKE_COMMAND} -E echo "${TEST_COMMAND}"
			COMMAND ${CMAKE_COMMAND} -E time "${TEST_COMMAND} --result-code=no --report-level=no"
			DEPENDS sqlite_wrapper_test_${name}
			)
	ENDIF( ENABLE_AUTORUN_TEST )
ENDMACRO ()

IF(MSVC)
	MACRO (ADD_SQLITE_WRAPPER_TEST name)
		ADD_DEFINITIONS( -DSQLITE_WRAPPER_USE=1 -DSQLITE_WRAPPER_SHARED )
		ADD_EXECUTABLE (sqlite_wrapper_test_${name} ${name}.cpp)
		TARGET_LINK_LIBRARIES (sqlite_wrapper_test_${name} sqlite_wrapper)
		ADD_TARGET_TEST(${name})
	ENDMACRO ()
ELSE(MSVC)
	MACRO (ADD_SQLITE_WRAPPER_TEST name)
		ADD_DEFINITIONS( -DSQLITE_WRAPPER_USE=1 )
		ADD_EXECUTABLE (sqlite_wrapper_test_${name} ${name}.cpp)
		TARGET_LINK_LIBRARIES (sqlite_wrapper_test_${name} sqlite_wrapper_static)
		ADD_TARGET_TEST(${name})
	ENDMACRO ()
ENDIF()

MACRO( ADD_SQLITE_WRAPPER_QT4_TEST name )
	IF( QT4_FOUND )
		SET( sqlite_wrapper_test_${name}_header
			${CMAKE_SOURCE_DIR}/test/${name}.hpp
		)
		SET( sqlite_wrapper_test_${name}_source
			${CMAKE_SOURCE_DIR}/test/${name}.cpp
		)
		QT4_WRAP_CPP(sqlite_wrapper_test_${name}_moc_source ${sqlite_wrapper_test_${name}_header})

IF(MSVC)
		ADD_DEFINITIONS( -DSQLITE_WRAPPER_USE=1 )
		ADD_EXECUTABLE (sqlite_wrapper_test_${name} ${sqlite_wrapper_test_${name}_source} ${sqlite_wrapper_test_${name}_moc_source})
		TARGET_LINK_LIBRARIES (sqlite_wrapper_test_${name} libsqlite_wrapper ${QT_LIBRARIES})
ELSE(MSVC)
		ADD_DEFINITIONS( -DSQLITE_WRAPPER_USE=1 )
		ADD_EXECUTABLE (sqlite_wrapper_test_${name} ${sqlite_wrapper_test_${name}_source} ${sqlite_wrapper_test_${name}_moc_source})
		TARGET_LINK_LIBRARIES (sqlite_wrapper_test_${name} libsqlite_wrapper ${QT_LIBRARIES})
ENDIF()
		ADD_TARGET_TEST(${name})

	ENDIF( QT4_FOUND )
ENDMACRO()


ADD_SQLITE_WRAPPER_TEST(db)
ADD_SQLITE_WRAPPER_TEST(db_log)
ADD_SQLITE_WRAPPER_TEST(log)
ADD_SQLITE_WRAPPER_QT4_TEST(db_log_viewer)
