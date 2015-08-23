#ifndef _SYSTEM1DEFS_INCLUDED
#define _SYSTEM1DEFS_INCLUDED

#	if defined (WIN32) 
#		if defined (SERVICE_BACKEND_DLLS)
#			if defined (SYSTEM1_BCK_EXPORTS)
#				define SYSTEM1_EXP __declspec(dllexport)
#			else
#				define SYSTEM1_EXP __declspec(dllimport)
#			endif
#		else // WIN32
#			define SYSTEM1_EXP
#		endif		
#	endif


// Include standard stuff
//
#include "shared/cpputils/src/cpputils.hpp"
#include "../alglib/alglib.hpp"

// redefined assert
 #include "shared/cpputils/src/assertdef.hpp"

// algorithm holder structure
#include "../inqueue/inqueue.hpp"
#include "../inqueue/commontypes.hpp"


namespace AlgLib {

/** Creator function  */
extern "C" SYSTEM1_EXP AlgorithmBase::CreatorAlgorithmFun createAlgorithm;

/** Terminator function function  */
extern "C" SYSTEM1_EXP AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm;


};


#endif // _SYSTEM1DEFS_INCLUDED
