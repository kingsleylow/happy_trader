#ifndef _DYMMY_QUIKDEFS_INCLUDED
#define _DYMMY_QUIKDEFS_INCLUDED
//
#	if defined (WIN32) 
#		if defined (SERVICE_BACKEND_DLLS)
#			if defined (DYMMY_QUIK_BCK_EXPORTS)
#				define DYMMY_QUIK_EXP __declspec(dllexport)
#			else
#				define DYMMY_QUIK_EXP __declspec(dllimport)
#			endif
#		else // WIN32
#			define DYMMY_QUIK_EXP
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


namespace AlgLib {

  /** Creator function  */
  extern "C" DYMMY_QUIK_EXP AlgorithmBase::CreatorAlgorithmFun createAlgorithm;

	/** Terminator function function  */
	extern "C" DYMMY_QUIK_EXP AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm;


};

#endif // _DYMMY_QUIKDEFS_INCLUDED
