#ifndef _JAVA_UIDDEFS_INCLUDED
#define _JAVA_UIDDEFS_INCLUDED
//
#	if defined (WIN32) 
#		if defined (SERVICE_BACKEND_DLLS)
#			if defined (JAVA_UID_BCK_EXPORTS)
#				define JAVA_UID_EXP __declspec(dllexport)
#			else
#				define JAVA_UID_EXP __declspec(dllimport)
#			endif
#		else // WIN32
#			define JAVA_UID_EXP
#		endif		
#	endif
//
//#define _USE_32BIT_TIME_T

#include "shared/cpputils/src/cpputils.hpp"

#endif // _JAVA_UIDDEFS_INCLUDED
