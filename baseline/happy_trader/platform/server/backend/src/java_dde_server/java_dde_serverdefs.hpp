#ifndef _JAVA_DDE_SERVERDEFS_INCLUDED
#define _JAVA_DDE_SERVERDEFS_INCLUDED

//#define HANDLE_NATIVE_EXCEPTIONS 1
//
#	if defined (WIN32) 
#		if defined (SERVICE_BACKEND_DLLS)
#			if defined (JAVA_DDE_SERVER_BCK_EXPORTS)
#				define JAVA_DDE_SERVER_EXP __declspec(dllexport)
#			else
#				define JAVA_DDE_SERVER_EXP __declspec(dllimport)
#			endif
#		else // WIN32
#			define JAVA_DDE_SERVER_EXP
#		endif		
#	endif

//#define _USE_32BIT_TIME_T

#include "shared/cpputils/src/cpputils.hpp"

// testframe handlers
#ifdef HANDLE_NATIVE_EXCEPTIONS
#include "shared/testframe/src/testsysdep.hpp"
#include "shared/testframe/src/testexcept.hpp"




#endif


//
#endif // _JAVA_DDE_SERVERDEFS_INCLUDED
