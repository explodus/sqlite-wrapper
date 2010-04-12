/// @file private_assemblies.hpp
///
/// @date 10.03.2010 09:36
///
/// @author T. Schröder (explodus@gmx.de)
///
/// @brief 
///
/// This file is part of the sqlite-wrapper project

#ifndef DB_PRIVATE_ASSEMBLIES_HPP_INCLUDED
#define DB_PRIVATE_ASSEMBLIES_HPP_INCLUDED

#if defined (WIN32) || defined (WIN64)

#ifndef RC_INVOKED
// Avoid problems with the resource compiler if included

//never define this again... damn...
//#define _USE_ANSI_CPP

#ifndef _BIND_TO_CURRENT_VCLIBS_VERSION
#	define _BIND_TO_CURRENT_VCLIBS_VERSION 0
#endif
#ifndef _BIND_TO_CURRENT_MFC_VERSION
#	define _BIND_TO_CURRENT_MFC_VERSION 0
#else
#	define _BIND_TO_CURRENT_MFC_VERSION 1
#endif
#ifndef _BIND_TO_CURRENT_CRT_VERSION
#	define _BIND_TO_CURRENT_CRT_VERSION 0
#else
#	define _BIND_TO_CURRENT_CRT_VERSION 1
#endif

#ifndef __OPENMP_ASSEMBLY_VERSION
#	define __OPENMP_ASSEMBLY_VERSION "9.0.21022.8"
#endif

#ifndef __OPENMP_ASSEMBLY_NAME_PREFIX
#	define __OPENMP_ASSEMBLY_NAME_PREFIX "Microsoft.VC90"
#endif

#define _VC_ASSEMBLY_PUBLICKEYTOKEN ""

#ifndef __LIBRARIES_ASSEMBLY_NAME_PREFIX
#	define __LIBRARIES_ASSEMBLY_NAME_PREFIX "Microsoft.VC90"
#endif

#if _MSC_VER>1400
#	ifndef _MFC_ASSEMBLY_VERSION
#		if defined(_BIND_TO_CURRENT_MFC_VERSION) && defined(_BIND_TO_CURRENT_MFC_VERSION) == 1
#			define _MFC_ASSEMBLY_VERSION "9.0.30411.0"
#		elif defined(_BIND_TO_CURRENT_VCLIBS_VERSION) && defined(_BIND_TO_CURRENT_VCLIBS_VERSION) == 1
#			define _MFC_ASSEMBLY_VERSION "9.0.30411.0"
#		else
#			define _MFC_ASSEMBLY_VERSION "9.0.21022.8"
#		endif
#	endif
#	ifndef _CRT_ASSEMBLY_VERSION
#		if defined(_BIND_TO_CURRENT_CRT_VERSION) && defined(_BIND_TO_CURRENT_CRT_VERSION) == 1
#			define _CRT_ASSEMBLY_VERSION "9.0.30411.0"
#		elif defined(_BIND_TO_CURRENT_VCLIBS_VERSION) && defined(_BIND_TO_CURRENT_VCLIBS_VERSION) == 1
#			define _CRT_ASSEMBLY_VERSION "9.0.30411.0"
#		else
#			define _CRT_ASSEMBLY_VERSION "9.0.21022.8"
#		endif
#	endif
#endif

#if defined(_M_IX86)
#	define MANIFEST_PROCESSORARCHITECTURE "x86"
#elif defined(_M_AMD64)
#	define MANIFEST_PROCESSORARCHITECTURE "amd64"
#elif defined(_M_IA64)
#	define MANIFEST_PROCESSORARCHITECTURE "ia64"
#else
#	error Unknown processor architecture.
#endif

//This defines bock the creation in the header files
//#pragma message("Using private assemblies for the MS runtimes")

#define _STL_NOFORCE_MANIFEST
#define _CRT_NOFORCE_MANIFEST
#define _MFC_NOFORCE_MANIFEST
#define _AFX_NOFORCE_MANIFEST
#define _ATL_NOFORCE_MANIFEST

// The next statements block the linker from including object files in the
// CRT and the MFC, that would create manifest pragmas too.
#ifdef __cplusplus
extern "C" 
{ // Assume C declarations for C++
#endif

	__declspec(selectany)	int _forceCRTManifest;
	__declspec(selectany)	int _forceMFCManifest;
	__declspec(selectany)	int _forceAtlDllManifest;

	// The next symbols are used by the several versions of VC 9.0 
	__declspec(selectany)	int _forceCRTManifestRTM; 
	__declspec(selectany)	int _forceMFCManifestRTM; 
	__declspec(selectany)	int _forceMFCManifestCUR;    

#ifdef __cplusplus
}						/* __cplusplus */
#endif

// We use crtassem.h with the defines there. It just gives us the
// versions and name parts for the dependencies.
// Note that there is also a MFCassem.h but this include file has the
// manifest pragma's already in it. So we can't use it
//
// Three files are contrlling this crtassem.h, MFCassem.h and atlassem.h!
// Happily __LIBRARIES_ASSEMBLY_NAME_PREFIX is used in CRT, MFC and ATL!
// Doing it right would need to use _MFC_ASSEMBLY_VERSION for the MFC
// but in fact _CRT_ASSEMBLY_VERSION and _MFC_ASSEMBLY_VERSION and
// _ATL_ASSEMBLY_VERSION arethe same (VC-2005 SP1 8.0.50727.762)

// We don't have a seperate block for the Debug version. We just handle
// this with a extra define here.
#ifdef _DEBUG
#define __LIBRARIES_SUB_VERSION	"Debug"
#else
#define __LIBRARIES_SUB_VERSION	""
#endif

//Manifest for the CRT
#if defined (_BIND_TO_CURRENT_CRT_VERSION) && (_BIND_TO_CURRENT_CRT_VERSION == 1)
#	pragma comment(linker,"/manifestdependency:\"type='win32' name='" __LIBRARIES_ASSEMBLY_NAME_PREFIX "." __LIBRARIES_SUB_VERSION "CRT' version='" _CRT_ASSEMBLY_VERSION "' processorArchitecture='" MANIFEST_PROCESSORARCHITECTURE "' \"")    
#endif

#if defined (_BIND_TO_CURRENT_MFC_VERSION) && (_BIND_TO_CURRENT_MFC_VERSION == 1)
// Manifest for the MFC 
#	pragma comment(linker,"/manifestdependency:\"type='win32' name='" __LIBRARIES_ASSEMBLY_NAME_PREFIX "." __LIBRARIES_SUB_VERSION "MFC' version='" _CRT_ASSEMBLY_VERSION "' processorArchitecture='" MANIFEST_PROCESSORARCHITECTURE "'\"")    
#endif

#endif // RC_INVOKED

#endif

#endif // DB_PRIVATE_ASSEMBLIES_HPP_INCLUDED