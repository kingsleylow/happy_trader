// Version 1.0 by Martin Richter [WWJD]

#ifndef __CPPUTILS_USE_PRIVATE_ASSEMPLIES
#define	__CPPUTILS_USE_PRIVATE_ASSEMPLIES

#ifdef FOO_UNDEFINED_SYMBOL____
#ifndef RC_INVOKED

#ifndef _BIND_TO_CURRENT_VCLIBS_VERSION
#define _BIND_TO_CURRENT_VCLIBS_VERSION 1
#endif



// Avoid problems with the resource compiler if included

// This defines bock the creation in the header files
//#pragma message("Using private assemblies for the MS runtimes")
#define _STL_NOFORCE_MANIFEST
#define _CRT_NOFORCE_MANIFEST
#define _AFX_NOFORCE_MANIFEST
#define _ATL_NOFORCE_MANIFEST

//#define _SXS_ASSEMBLY_VERSION _CRT_ASSEMBLY_VERSION

//#define _CRT_ASSEMBLY_VERSION _SXS_ASSEMBLY_VERSION
//#define _MFC_ASSEMBLY_VERSION _SXS_ASSEMBLY_VERSION
//#define _ATL_ASSEMBLY_VERSION _SXS_ASSEMBLY_VERSION



// The next statements block the linker from including object files in the
// CRT and the MFC, that would create manifest pragmas too.
#ifdef __cplusplus
extern "C" {            // Assume C declarations for C++ 
#endif

//__declspec(selectany)		int _forceCRTManifest;
//__declspec(selectany)		int _forceMFCManifest;
//__declspec(selectany)	int _forceAtlDllManifest;

__declspec(selectany) int _forceCRTManifestRTM;
//__declspec(selectany) int _forceMFCManifestRTM;
//__declspec(selectany) int _forceAtlDllManifestRTM;

//__declspec(selectany)		int _forceCRTManifestCUR;
//__declspec(selectany)		int _forceMFCManifestCUR;
//__declspec(selectany)	int _forceAtlDllManifestCUR;

#ifdef __MFC_PRIVATE_DEP
	__declspec(selectany) int _forceMFCManifestRTM;
#endif


#ifdef __ATL_PRIVATE_DEP
		__declspec(selectany)	int _forceAtlDllManifest;
		__declspec(selectany)	int _forceAtlDllManifestCUR;
		__declspec(selectany) int _forceAtlDllManifestRTM;
#endif

#ifdef __cplusplus
}						// __cplusplus 
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

#include <crtassem.h>

// We don't have a seperate block for the Debug version. We just handle
// this with a extra define here.
#ifdef _DEBUG
#define __LIBRARIES_SUB_VERSION	"Debug"
#else
#define __LIBRARIES_SUB_VERSION	""
#endif

// Manifest for the CRT
#pragma comment(linker,"/manifestdependency:\"type='win32' "						\
	"name='" __LIBRARIES_ASSEMBLY_NAME_PREFIX "." __LIBRARIES_SUB_VERSION "CRT' "   \
	"version='" _CRT_ASSEMBLY_VERSION "' "											\
	"processorArchitecture='x86' \"")


#ifdef __MFC_PRIVATE_DEP
// Manifest for the MFC
#pragma comment(linker,"/manifestdependency:\"type='win32' "						\
	"name='" __LIBRARIES_ASSEMBLY_NAME_PREFIX "." __LIBRARIES_SUB_VERSION "MFC' "   \
	"version='" _CRT_ASSEMBLY_VERSION "' "											\
	"processorArchitecture='x86'\"")
#endif

/*
// #pragma comment(linker,"/manifestdependency:\"type='win32' "						\
// 	"name='" __LIBRARIES_ASSEMBLY_NAME_PREFIX ".MFCLOC' "							\
// 	"version='" _CRT_ASSEMBLY_VERSION "' "											\
// 	"processorArchitecture='x86'\"")

*/

#ifdef __ATL_PRIVATE_DEP
// Manifest for the ATL
 #pragma comment(linker,"/manifestdependency:\"type='win32' "						\
	"name='" __LIBRARIES_ASSEMBLY_NAME_PREFIX ".ATL' "								\
	"version='" _CRT_ASSEMBLY_VERSION "' "											\
	"processorArchitecture='x86' \"")

#endif

#endif // RC_INVOKED

#endif // DISABLE

#endif
