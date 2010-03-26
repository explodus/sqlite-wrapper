#if(MSVC)
#	set(CMAKE_CXX_FLAGS_DEBUG_INIT "/D_DEBUG /MTd /Zi /Ob0 /Od /RTC1 /W4")
#	set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT     "/MT /O1 /Ob1 /D NDEBUG")
#	set(CMAKE_CXX_FLAGS_RELEASE_INIT        "/MT /O2 /Ob2 /D NDEBUG")
#	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "/MT /Zi /O2 /Ob1 /D NDEBUG")
#endif()

if(CMAKE_COMPILER_IS_GNUCXX)
       set(CMAKE_CXX_FLAGS_INIT "-pedantic-errors -Wall -Wextra -Wconversion -Wfloat-equal -Wredundant-decls -Wstrict-null-sentinel -Winit-self -Woverloaded-virtual -Wnon-virtual-dtor -Wctor-dtor-privacy -Wsign-promo -Wstrict-aliasing=1")
endif()

#if(MSVC)
#	set(CMAKE_CXX_FLAGS_DEBUG_INIT "/D_DEBUG /MTd /Zi /Ob0 /Od /RTC1")
#	set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT "/MT /O1 /Ob1 /D NDEBUG")
#	set(CMAKE_CXX_FLAGS_RELEASE_INIT "/MT /O2 /Ob2 /D NDEBUG")
#	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "/MT /Zi /O2 /Ob1 /D NDEBUG")
#
#	set(CMAKE_CXX_FLAGS_DEBUG_STATIC CACHE STRING "/D_DEBUG /MTd /Zi /Ob0 /Od /RTC1" FORCE)
#	set(CMAKE_CXX_FLAGS_RELEASE_STATIC CACHE STRING "/MT /O2 /Ob2 /D NDEBUG" FORCE)
#endif()

IF(MSVC) # adds a static build in release and debug 
	SET( CMAKE_CXX_FLAGS_DEBUGSTATIC "/D_DEBUG /MTd /Zi /Ob0 /Od /RTC1" CACHE STRING "" FORCE)
	SET( CMAKE_CXX_FLAGS_RELEASESTATIC "/MT /O2 /Ob2 /D NDEBUG" CACHE STRING "" FORCE)

	SET( CMAKE_EXE_LINKER_FLAGS_DEBUGSTATIC
	    ${CMAKE_EXE_LINKER_FLAGS_DEBUG} CACHE STRING
	    "Flags used for linking binaries during static debug builds."
	    FORCE )
	SET( CMAKE_SHARED_LINKER_FLAGS_DEBUGSTATIC
	    ${CMAKE_SHARED_LINKER_FLAGS_DEBUG} CACHE STRING
	    "Flags used by the shared libraries linker during static debug builds."
	    FORCE )

	MARK_AS_ADVANCED(
	    CMAKE_CXX_FLAGS_DEBUGSTATIC
	    CMAKE_EXE_LINKER_FLAGS_DEBUGSTATIC
	    CMAKE_SHARED_LINKER_FLAGS_DEBUGSTATIC )

	SET( CMAKE_EXE_LINKER_FLAGS_RELEASESTATIC
	    ${CMAKE_EXE_LINKER_FLAGSRELEASE} CACHE STRING
	    "Flags used for linking binaries during static debug builds."
	    FORCE )
	SET( CMAKE_SHARED_LINKER_FLAGS_RELEASESTATIC
	    ${CMAKE_SHARED_LINKER_FLAGS_RELEASE} CACHE STRING
	    "Flags used by the shared libraries linker during static debug builds."
	    FORCE )

	MARK_AS_ADVANCED(
	    CMAKE_CXX_FLAGS_RELEASESTATIC
	    CMAKE_EXE_LINKER_FLAGS_RELEASESTATIC
	    CMAKE_SHARED_LINKER_FLAGS_RELEASESTATIC )

#	SET( CMAKE_BUILD_TYPE "" CACHE STRING
#	    "Choose the type of build, options are: None Debug Release RelWithDebInfo DebugStatic ReleaseStatic." )

	SET( CMAKE_CONFIGURATION_TYPES 
		"Debug;Release;MinSizeRel;RelWithDebInfo;DebugStatic;ReleaseStatic" CACHE STRING 
		"Debug;Release;MinSizeRel;RelWithDebInfo;DebugStatic;ReleaseStatic" FORCE)
ENDIF()