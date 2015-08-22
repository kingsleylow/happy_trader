#ifndef _GERM_2CHANNEL_MMDEFS_INCLUDED
#define _GERM_2CHANNEL_MMDEFS_INCLUDED

#	if defined (WIN32) 
#		if defined (SERVICE_BACKEND_DLLS)
#			if defined (GERM_2CHANNEL_MM_BCK_EXPORTS)
#				define GERM_2CHANNEL_MM_EXP __declspec(dllexport)
#			else
#				define GERM_2CHANNEL_MM_EXP __declspec(dllimport)
#			endif
#		else // WIN32
#			define GERM_2CHANNEL_MM_EXP
#		endif		
#	endif


// Include standard stuff
//
#include "shared/cpputils/src/cpputils.hpp"
#include "../alglib/alglib.hpp"

// algorithm holder structure
#include "../inqueue/inqueue.hpp"
#include "../inqueue/commontypes.hpp"

#include "../math/math.hpp"


namespace AlgLib {

/** Creator function  */
extern "C" GERM_2CHANNEL_MM_EXP AlgorithmBase::CreatorAlgorithmFun createAlgorithm;

/** Terminator function function  */
extern "C" GERM_2CHANNEL_MM_EXP AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm;


};


#endif // _GERM_2CHANNEL_MMDEFS_INCLUDED
