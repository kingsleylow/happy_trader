#ifndef _MTQUEUEDEFS_INCLUDED
#define _MTQUEUEDEFS_INCLUDED
//
// This library must be mediator between C++ code and Metatrader
// it will be loadad via MQL script and make communication via pipe
// static linkage to cpputils 


#	if defined (WIN32) 
#		if defined (SERVICE_BACKEND_DLLS)
#			if defined (MTQUEUE_BCK_EXPORTS)
#				define MTQUEUE_EXP __declspec(dllexport)
#			else
#				define MTQUEUE_EXP __declspec(dllimport)
#			endif
#		else // WIN32
#			define MTQUEUE_EXP
#		endif		
#		define PROCCALL _stdcall
#	endif

// Include standard stuff
//


#include "shared/cpputils/src/cpputils.hpp"


// Rapid XML
/*
#include "../hlpstruct/rapidxml/rapidxml.hpp"
#include "../hlpstruct/rapidxml/rapidxml_iterators.hpp"
#include "../hlpstruct/rapidxml/rapidxml_print.hpp"
#include "../hlpstruct/rapidxml/rapidxml_utils.hpp"

#include "../hlpstruct/json/reader.h"
#include "../hlpstruct/json/writer.h"
#include "../hlpstruct/json/elements.h"
*/




//
#endif // _MTPROXYDEFS_INCLUDED
