#ifndef _JAVA_QUOTE_SERIALIZERDEFS_INCLUDED
#define _JAVA_QUOTE_SERIALIZERDEFS_INCLUDED

#	if defined (WIN32) 
#		if defined (SERVICE_BACKEND_DLLS)
#			if defined (JAVA_QUOTE_SERIALIZER_BCK_EXPORTS)
#				define JAVA_QUOTE_SERIALIZER_EXP __declspec(dllexport)
#			else
#				define JAVA_QUOTE_SERIALIZER_EXP __declspec(dllimport)
#			endif
#		else // WIN32
#			define JAVA_QUOTE_SERIALIZER_EXP
#		endif		
#	endif


// Include standard stuff
//
#include "shared/cpputils/src/cpputils.hpp"
#include "../alglib/alglib.hpp"



// algorithm holder structure
#include "../inqueue/inqueue.hpp"
#include "../inqueue/commontypes.hpp"





#endif // _JAVA_QUOTE_SERIALIZERDEFS_INCLUDED
