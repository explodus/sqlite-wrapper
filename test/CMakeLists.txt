IF(MSVC)
	MACRO (ADD_SQLITE_WRAPPER_TEST name)
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
		TARGET_LINK_LIBRARIES (sqlite_wrapper_${name}_test sqlite_wrapper)
		ADD_CUSTOM_COMMAND(
			TARGET sqlite_wrapper_${name}_test
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E echo "${CMAKE_BINARY_DIR}/bin/sqlite_wrapper_${name}_test${CMAKE_EXECUTABLE_SUFFIX} --result_code=no --report_level=no"
			COMMAND ${CMAKE_COMMAND} -E time "${CMAKE_BINARY_DIR}/bin/sqlite_wrapper_${name}_test${CMAKE_EXECUTABLE_SUFFIX}"
		)
	ENDMACRO ()
ENDIF()

ADD_SQLITE_WRAPPER_TEST(db)
ADD_SQLITE_WRAPPER_TEST(db_log)
ADD_SQLITE_WRAPPER_TEST(log)
