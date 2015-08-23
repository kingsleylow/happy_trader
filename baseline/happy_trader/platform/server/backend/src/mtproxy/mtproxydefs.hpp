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

/*
#ifndef _SINGLE_LOG_FILE
#define _SINGLE_LOG_FILE 1
#endif
*/

// Include standard stuff
//
#include "shared/cpputils/src/cpputils.hpp"

// Rapid XML
#include "../hlpstruct/rapidxml/rapidxml.hpp"
#include "../hlpstruct/rapidxml/rapidxml_iterators.hpp"
#include "../hlpstruct/rapidxml/rapidxml_print.hpp"
#include "../hlpstruct/rapidxml/rapidxml_utils.hpp"

#include "../hlpstruct/json/reader.h"
#include "../hlpstruct/json/writer.h"
#include "../hlpstruct/json/elements.h"
#include "../alglibmetatrader2/shared.hpp"




//
#endif // _MTPROXYDEFS_INCLUDED
