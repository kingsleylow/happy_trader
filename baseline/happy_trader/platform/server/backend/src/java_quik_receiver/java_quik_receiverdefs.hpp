#ifndef _JAVA_QUIK_RECEIVERDEFS_INCLUDED
#define _JAVA_QUIK_RECEIVERDEFS_INCLUDED

#	if defined (WIN32) 
#		if defined (SERVICE_BACKEND_DLLS)
#			if defined (JAVA_QUIK_RECEIVER_BCK_EXPORTS)
#				define JAVA_QUIK_RECEIVER_EXP __declspec(dllexport)
#			else
#				define JAVA_QUIK_RECEIVER_EXP __declspec(dllimport)
#			endif
#		else // WIN32
#			define JAVA_QUIK_RECEIVER_EXP
#		endif		
#	endif


// Include standard stuff
//
#include "shared/cpputils/src/cpputils.hpp"
#include "../alglib/alglib.hpp"


namespace AlgLib {



};


#endif // _JAVA_QUIK_RECEIVERDEFS_INCLUDED
