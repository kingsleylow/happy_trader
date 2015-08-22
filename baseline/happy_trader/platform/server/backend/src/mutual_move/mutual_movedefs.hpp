#ifndef _MUTUAL_MOVEDEFS_INCLUDED
#define _MUTUAL_MOVEDEFS_INCLUDED

#	if defined (WIN32) 
#		if defined (SERVICE_BACKEND_DLLS)
#			if defined (MUTUAL_MOVE_BCK_EXPORTS)
#				define MUTUAL_MOVE_EXP __declspec(dllexport)
#			else
#				define MUTUAL_MOVE_EXP __declspec(dllimport)
#			endif
#		else // WIN32
#			define MUTUAL_MOVE_EXP
#		endif		
#	endif


// Include standard stuff
//
#include "shared/cpputils/src/cpputils.hpp"
#include "../alglib/alglib.hpp"


// algorithm holder structure
#include "../inqueue/inqueue.hpp"
#include "../inqueue/commontypes.hpp"


namespace AlgLib {

/** Creator function  */
extern "C" MUTUAL_MOVE_EXP AlgorithmBase::CreatorAlgorithmFun createAlgorithm;

/** Terminator function function  */
extern "C" MUTUAL_MOVE_EXP AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm;


};


#endif // _MUTUAL_MOVEDEFS_INCLUDED
