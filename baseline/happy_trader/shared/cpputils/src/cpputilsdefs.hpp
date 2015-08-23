#ifndef _CPPUTILSDEFS_INCLUDED
#define _CPPUTILSDEFS_INCLUDED


// Handle DLL decl specifier
//
#if defined (WIN32) && defined (CPPUTILS_DLL)



#if defined (CPPUTILS_EXPORTS)
#define CPPUTILS_EXP __declspec(dllexport)
#else
#define CPPUTILS_EXP __declspec(dllimport)
#endif

#else // WIN32


#define CPPUTILS_EXP

#endif

//#ifndef _BIND_TO_CURRENT_VCLIBS_VERSION
//#define _BIND_TO_CURRENT_VCLIBS_VERSION 1
//#endif

// Version type
#ifdef _DEBUG

//_BIND_TO_CURRENT_VCLIBS_VERSION

#define BUILD_TARGET_STR "debug"

#else

#define BUILD_TARGET_STR "release"

#endif


// some definitions
#define PATH_SEPARATOR_CHAR '\\'
#define PATH_SEPARATOR_STR "\\"

#ifndef HANDLE_NATIVE_EXCEPTIONS
#define HANDLE_NATIVE_EXCEPTIONS 1
#endif

#ifdef HANDLE_NATIVE_EXCEPTIONS
#include "shared/testframe/src/testsysdep.hpp"
#include "shared/testframe/src/testexcept.hpp"

#endif


#ifndef _USE_32BIT_TIME_T
#define _USE_32BIT_TIME_T 1
#endif

// Forward decls
//

/** Namespace holding all definition in cpputils library. */
namespace CppUtils {
	class Exception;
	class ExceptionInternal;
	class Signature;
}



// Include system dependent stuff
//

#include "sysdep/system.hpp"
#include "sysdep/sysdep.hpp"



// Debugging support
//
#include <assert.h>

#ifdef _DEBUG

#include "shared/testframe/src/testexcept.hpp"

#endif



#endif // _CPPUTILSDEFS_INCLUDED
