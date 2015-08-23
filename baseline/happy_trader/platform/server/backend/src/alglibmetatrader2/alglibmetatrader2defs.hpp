#ifndef _ALGLIBMETATRADER2DEFS_INCLUDED
#define _ALGLIBMETATRADER2DEFS_INCLUDED
//
#	if defined (WIN32) 
#		if defined (SERVICE_BACKEND_DLLS)
#			if defined (ALGLIBMETATRADER2_BCK_EXPORTS)
#				define ALGLIBMETATRADER2_EXP __declspec(dllexport)
#			else
#				define ALGLIBMETATRADER2_EXP __declspec(dllimport)
#			endif
#		else // WIN32
#			define ALGLIBMETATRADER2_EXP
#		endif		
#	endif


// Include standard stuff
//
#include "shared/cpputils/src/cpputils.hpp"
#include "../alglib/alglib.hpp"
#include "../hlpstruct/hlpstruct.hpp"



// algorithm holder structure
#include "../inqueue/inqueue.hpp"
#include "../inqueue/commontypes.hpp"

#include "xsd/auto/soapH.h"
// to resolve error: unresolved external symbol _namespaces
//#include "xsd/auto/empty.nsmap"




namespace AlgLib {
	/** Creator function  */
	extern "C" ALGLIBMETATRADER2_EXP AlgorithmBase::CreatorAlgorithmFun createAlgorithm;

	/** Terminator function function  */
	extern "C" ALGLIBMETATRADER2_EXP AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm;

}
//
#endif // _ALGLIBMETATRADERDEFS_INCLUDED
