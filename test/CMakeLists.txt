IF(MSVC)
	MACRO (ADD_SQLITE_WRAPPER_TEST name)
		ADD_DEFINITIONS( -DSQLITE_WRAPPER_USE=1 -DSQLITE_WRAPPER_SHARED )
		ADD_EXECUTABLE (sqlite_wrapper_${name}_test ${name}.cpp)
		TARGET_LINK_LIBRARIES (sqlite_wrapper_${name}_test sqlite_wrapper)
		ADD_CUSTOM_COMMAND(
			TARGET sqlite_wrapper_${name}_test
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E echo "$(TargetPath) --result_code=no --report_level=no"
			COMMAND ${CMAKE_COMMAND} -E time "$(TargetPath)" --result_code=no --report_level=no
		)
	ENDMACRO ()
ELSE(MSVC)
	MACRO (ADD_SQLITE_WRAPPER_TEST name)
		ADD_EXECUTABLE (sqlite_wrapper_${name}_test ${name}.cpp)
		TARGET_LINK_LIBRARIES (sqlite_wrapper_${name}_test sqlite_wrapper_static)
		ADD_CUSTOM_COMMAND(
			TARGET sqlite_wrapper_${name}_test
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E echo "${CMAKE_BINARY_DIR}/bin/sqlite_wrapper_${name}_test${CMAKE_EXECUTABLE_SUFFIX} --result_code=no --report_level=no"
			COMMAND ${CMAKE_COMMAND} -E time "${CMAKE_BINARY_DIR}/bin/sqlite_wrapper_${name}_test${CMAKE_EXECUTABLE_SUFFIX}"
		)

	ENDMACRO ()
ENDIF()

MACRO( ADD_SQLITE_WRAPPER_QT4_TEST name )
	IF( QT4_FOUND )
		SET( sqlite_wrapper_${name}_test_header
			${CMAKE_SOURCE_DIR}/test/${name}.hpp
		)
		SET( sqlite_wrapper_${name}_test_source
			${CMAKE_SOURCE_DIR}/test/${name}.cpp
		)
		QT4_WRAP_CPP(sqlite_wrapper_${name}_test_moc_source ${sqlite_wrapper_${name}_test_header})

		ADD_DEFINITIONS( -DSQLITE_WRAPPER_USE=1 -DSQLITE_WRAPPER_SHARED )
		ADD_EXECUTABLE (sqlite_wrapper_${name}_test ${sqlite_wrapper_${name}_test_source} ${sqlite_wrapper_${name}_test_moc_source})
		TARGET_LINK_LIBRARIES (sqlite_wrapper_${name}_test sqlite_wrapper ${QT_LIBRARIES})

		GET_TARGET_PROPERTY( TEST_COMMAND sqlite_wrapper_${name}_test LOCATION )

		ADD_CUSTOM_TARGET(
			sqlite_wrapper_test_qt4_${name}
			COMMAND ${CMAKE_COMMAND} -E echo "${TEST_COMMAND}"
			COMMAND ${CMAKE_COMMAND} -E time "${TEST_COMMAND} --result-code=no --report-level=no"
			DEPENDS sqlite_wrapper_${name}_test
			)

		IF( ENABLE_AUTORUN_TEST )
			ADD_CUSTOM_COMMAND(
				TARGET sqlite_wrapper_${name}_test
				COMMAND ${CMAKE_COMMAND} -E echo "${TEST_COMMAND}"
				COMMAND ${CMAKE_COMMAND} -E time "${TEST_COMMAND} --result-code=no --report-level=no"
				DEPENDS sqlite_wrapper_${name}_test
				)
		ENDIF( ENABLE_AUTORUN_TEST )
	ENDIF( QT4_FOUND )
ENDMACRO()


ADD_SQLITE_WRAPPER_TEST(db)
ADD_SQLITE_WRAPPER_TEST(db_log)
ADD_SQLITE_WRAPPER_TEST(log)
ADD_SQLITE_WRAPPER_QT4_TEST(db_log_viewer)
