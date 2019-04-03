if(CMAKE_COMPILER_IS_GNUCXX)
       set(CMAKE_CXX_FLAGS_INIT "-pedantic-errors -Wall -Wextra -Wconversion -Wfloat-equal -Wredundant-decls -Wstrict-null-sentinel -Winit-self -Woverloaded-virtual -Wnon-virtual-dtor -Wctor-dtor-privacy -Wsign-promo -Wstrict-aliasing=1")
endif()

IF(MSVC) # adds a static build in release and debug 
	SET( CMAKE_C_FLAGS_DEBUGSTATIC "/DSQLITE_WRAPPER_STATIC /D_DEBUG /MTd /Zi /Ob0 /Od /RTC1" CACHE STRING "" FORCE)
	SET( CMAKE_C_FLAGS_RELEASESTATIC "/DSQLITE_WRAPPER_STATIC /MT /O2 /Ob2 /D NDEBUG" CACHE STRING "" FORCE)
	SET( CMAKE_CXX_FLAGS_DEBUGSTATIC "/DSQLITE_WRAPPER_STATIC /D_DEBUG /MTd /Zi /Ob0 /Od /RTC1" CACHE STRING "" FORCE)
	SET( CMAKE_CXX_FLAGS_RELEASESTATIC "/DSQLITE_WRAPPER_STATIC /MT /O2 /Ob2 /D NDEBUG" CACHE STRING "" FORCE)
	SET( CMAKE_EXE_LINKER_FLAGS_DEBUG "/debug /MANIFEST /INCREMENTAL:NO"  CACHE STRING "" FORCE)
	SET( CMAKE_EXE_LINKER_FLAGS_RELEASE "/MANIFEST /INCREMENTAL:NO"  CACHE STRING "" FORCE)
	SET( CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "/MANIFEST /INCREMENTAL:NO"  CACHE STRING "" FORCE)
	SET( CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "/MANIFEST /INCREMENTAL:NO"  CACHE STRING "" FORCE)
	SET( CMAKE_MODULE_LINKER_FLAGS_DEBUG "/debug /MANIFEST /INCREMENTAL:NO"  CACHE STRING "" FORCE)
	SET( CMAKE_MODULE_LINKER_FLAGS_RELEASE "/MANIFEST /INCREMENTAL:NO"  CACHE STRING "" FORCE)
	SET( CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL "/MANIFEST /INCREMENTAL:NO"  CACHE STRING "" FORCE)
	SET( CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO "/MANIFEST /INCREMENTAL:NO"  CACHE STRING "" FORCE)
	SET( CMAKE_SHARED_LINKER_FLAGS_DEBUG "/debug /MANIFEST /INCREMENTAL:NO"  CACHE STRING "" FORCE)
	SET( CMAKE_SHARED_LINKER_FLAGS_RELEASE "/MANIFEST /INCREMENTAL:NO"  CACHE STRING "" FORCE)
	SET( CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL "/MANIFEST /INCREMENTAL:NO"  CACHE STRING "" FORCE)
	SET( CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO "/MANIFEST /INCREMENTAL:NO"  CACHE STRING "" FORCE)

	SET( CMAKE_EXE_LINKER_FLAGS_DEBUGSTATIC
	    ${CMAKE_EXE_LINKER_FLAGS_DEBUG} CACHE STRING
	    "Flags used for linking binaries during static debug builds."
	    FORCE )
	SET( CMAKE_MODULE_LINKER_FLAGS_DEBUGSTATIC
	    ${CMAKE_MODULE_LINKER_FLAGS_DEBUG} CACHE STRING
	    "Flags used for linking binaries during static debug builds."
	    FORCE )
	SET( CMAKE_SHARED_LINKER_FLAGS_DEBUGSTATIC
	    ${CMAKE_SHARED_LINKER_FLAGS_DEBUG} CACHE STRING
	    "Flags used by the shared libraries linker during static debug builds."
	    FORCE )

	MARK_AS_ADVANCED(
	    CMAKE_CXX_FLAGS_DEBUGSTATIC
	    CMAKE_C_FLAGS_DEBUGSTATIC
	    CMAKE_EXE_LINKER_FLAGS_DEBUGSTATIC
	    CMAKE_MODULE_LINKER_FLAGS_DEBUGSTATIC
	    CMAKE_SHARED_LINKER_FLAGS_DEBUGSTATIC )

	SET( CMAKE_EXE_LINKER_FLAGS_RELEASESTATIC
	    ${CMAKE_EXE_LINKER_FLAGSRELEASE} CACHE STRING
	    "Flags used for linking binaries during static debug builds."
	    FORCE )
	SET( CMAKE_MODULE_LINKER_FLAGS_RELEASESTATIC
	    ${CMAKE_MODULE_LINKER_FLAGSRELEASE} CACHE STRING
	    "Flags used for linking binaries during static debug builds."
	    FORCE )
	SET( CMAKE_SHARED_LINKER_FLAGS_RELEASESTATIC
	    "${CMAKE_SHARED_LINKER_FLAGS_RELEASE}" CACHE STRING
	    "Flags used by the shared libraries linker during static debug builds."
	    FORCE )

	MARK_AS_ADVANCED(
	    CMAKE_CXX_FLAGS_RELEASESTATIC
	    CMAKE_C_FLAGS_RELEASESTATIC
	    CMAKE_EXE_LINKER_FLAGS_RELEASESTATIC
	    CMAKE_MODULE_LINKER_FLAGS_RELEASESTATIC
	    CMAKE_SHARED_LINKER_FLAGS_RELEASESTATIC )

	SET( CMAKE_CONFIGURATION_TYPES 
		"Debug;Release;MinSizeRel;RelWithDebInfo;DebugStatic;ReleaseStatic" CACHE STRING 
		"Debug;Release;MinSizeRel;RelWithDebInfo;DebugStatic;ReleaseStatic" FORCE)
ENDIF()