#ifndef _JAVA_POWER_LISTENERDEFS_INCLUDED
#define _JAVA_POWER_LISTENERDEFS_INCLUDED
//
#	if defined (WIN32) 
#		if defined (SERVICE_BACKEND_DLLS)
#			if defined (JAVA_POWER_LISTENER_BCK_EXPORTS)
#				define JAVA_POWER_LISTENER_EXP __declspec(dllexport)
#			else
#				define JAVA_POWER_LISTENER_EXP __declspec(dllimport)
#			endif
#		else // WIN32
#			define JAVA_POWER_LISTENER_EXP
#		endif		
#	endif



#include "shared/cpputils/src/cpputils.hpp"
//
#endif // _JAVA_POWER_LISTENERDEFS_INCLUDED
