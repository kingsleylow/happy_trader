#ifndef _JAVA_SYS_UTILSDEFS_INCLUDED
#define _JAVA_SYS_UTILSDEFS_INCLUDED
//
#	if defined (WIN32) 
#		if defined (SERVICE_BACKEND_DLLS)
#			if defined (JAVA_SYS_UTILS_BCK_EXPORTS)
#				define JAVA_SYS_UTILS_EXP __declspec(dllexport)
#			else
#				define JAVA_SYS_UTILS_EXP __declspec(dllimport)
#			endif
#		else // WIN32
#			define JAVA_SYS_UTILS_EXP
#		endif		
#	endif
//
//#define _USE_32BIT_TIME_T

#include "shared/cpputils/src/cpputils.hpp"

#endif // _JAVA_SYS_UTILSDEFS_INCLUDED
