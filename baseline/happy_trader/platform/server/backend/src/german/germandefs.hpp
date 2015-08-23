#ifndef _GERMANDEFS_INCLUDED
#define _GERMANDEFS_INCLUDED
//
#	if defined (WIN32) 
#		if defined (SERVICE_BACKEND_DLLS)
#			if defined (GERMAN_BCK_EXPORTS)
#				define GERMAN_EXP __declspec(dllexport)
#			else
#				define GERMAN_EXP __declspec(dllimport)
#			endif
#		else // WIN32
#			define GERMAN_EXP
#		endif		
#	endif
//

// Include standard stuff
//
#include "shared/cpputils/src/cpputils.hpp"
#include "../alglib/alglib.hpp"

// redefined assert
#include "shared/cpputils/src/assertdef.hpp"

// algorithm holder structure
#include "../inqueue/inqueue.hpp"
#include "../inqueue/commontypes.hpp"

//
#include "../math/math.hpp"


namespace AlgLib {

  /** Creator function  */
  extern "C" GERMAN_EXP AlgorithmBase::CreatorAlgorithmFun createAlgorithm;

	/** Terminator function function  */
	extern "C" GERMAN_EXP AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm;
};


#endif // _GERMANDEFS_INCLUDED
