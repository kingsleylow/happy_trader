#ifndef _SYSTEM2DEFS_INCLUDED
#define _SYSTEM2DEFS_INCLUDED

#	if defined (WIN32) 
#		if defined (SERVICE_BACKEND_DLLS)
#			if defined (SYSTEM2_BCK_EXPORTS)
#				define SYSTEM2_EXP __declspec(dllexport)
#			else
#				define SYSTEM2_EXP __declspec(dllimport)
#			endif
#		else // WIN32
#			define SYSTEM2_EXP
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

#include "../math/math.hpp"


namespace AlgLib {

/** Creator function  */
extern "C" SYSTEM2_EXP AlgorithmBase::CreatorAlgorithmFun createAlgorithm;

/** Terminator function function  */
extern "C" SYSTEM2_EXP AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm;


};


#endif // _SYSTEM2DEFS_INCLUDED
