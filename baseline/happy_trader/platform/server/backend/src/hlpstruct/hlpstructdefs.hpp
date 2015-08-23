#ifndef _HLPSTRUCTDEFS_INCLUDED
#define _HLPSTRUCTDEFS_INCLUDED
//
#	if defined (WIN32) 
#		if defined (SERVICE_BACKEND_DLLS)
#			if defined (HLPSTRUCT_BCK_EXPORTS)
#				define HLPSTRUCT_EXP __declspec(dllexport)
#			else
#				define HLPSTRUCT_EXP __declspec(dllimport)
#			endif
#		else // WIN32
#			define HLPSTRUCT_EXP
#		endif		
#	endif
//
// Include standard stuff
//
#include "shared/cpputils/src/cpputils.hpp"
#include <deque>
#include <queue>

// max symbol length
#define MAX_SYMB_LENGTH  32

//
// testframe handlers
#ifdef HANDLE_NATIVE_EXCEPTIONS
#include "shared/testframe/src/testsysdep.hpp"
#include "shared/testframe/src/testexcept.hpp"




#endif

#endif // _HLPSTRUCTDEFS_INCLUDED
