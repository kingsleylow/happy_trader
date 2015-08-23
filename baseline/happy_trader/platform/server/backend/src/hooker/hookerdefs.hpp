#ifndef _HOOKERDEFS_INCLUDED
#define _HOOKERDEFS_INCLUDED
//
#	if defined (WIN32) 
#		if defined (SERVICE_BACKEND_DLLS)
#			if defined (HOOKER_BCK_EXPORTS)
#				define HOOKER_EXP __declspec(dllexport)
#			else
#				define HOOKER_EXP __declspec(dllimport)
#			endif
#		else // WIN32
#			define HOOKER_EXP
#		endif		
#	endif

// Include standard stuff
//
#include "shared/cpputils/src/cpputils.hpp"
#include "../alglib/alglib.hpp"


// algorithm holder structure
#include "../inqueue/inqueue.hpp"
#include "../inqueue/commontypes.hpp"

//
#include "../math/math.hpp"

namespace AlgLib {

  /** Creator function  */
  extern "C" HOOKER_EXP AlgorithmBase::CreatorAlgorithmFun createAlgorithm;

	/** Terminator function function  */
	extern "C" HOOKER_EXP AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm;


};



//
#endif // _HOOKERDEFS_INCLUDED
