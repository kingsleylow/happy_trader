#ifndef _MTPROXYDEFS_INCLUDED
#define _MTPROXYDEFS_INCLUDED
//
// This library must be mediator between C++ code and Metatrader
// it will be loadad via MQL script and make communication via pipe
// static linkage to cpputils 


#	if defined (WIN32) 
#		if defined (SERVICE_BACKEND_DLLS)
#			if defined (MTPROXY_BCK_EXPORTS)
#				define MTPROXY_EXP __declspec(dllexport)
#			else
#				define MTPROXY_EXP __declspec(dllimport)
#			endif
#		else // WIN32
#			define MTPROXY_EXP
#		endif		
#		define PROCCALL _stdcall
#	endif

// Include standard stuff
//
#include "shared/cpputils/src/cpputils.hpp"

#define VERSION "1.1.58"
//
#endif // _MTPROXYDEFS_INCLUDED
