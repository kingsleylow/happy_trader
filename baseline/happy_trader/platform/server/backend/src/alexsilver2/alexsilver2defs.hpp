#ifndef _ALEXSILVER2DEFS_INCLUDED
#define _ALEXSILVER2DEFS_INCLUDED
//
#	if defined (WIN32) 
#		if defined (SERVICE_BACKEND_DLLS)
#			if defined (ALEXSILVER2_BCK_EXPORTS)
#				define ALEXSILVER2_EXP __declspec(dllexport)
#			else
#				define ALEXSILVER2_EXP __declspec(dllimport)
#			endif
#		else // WIN32
#			define ALEXSILVER2_EXP
#		endif		
#	endif
//

// Include standard stuff
//
#include "shared/cpputils/src/cpputils.hpp"
#include "../alglib/alglib.hpp"

// algorithm holder structure
#include "../inqueue/inqueue.hpp"
#include "../inqueue/commontypes.hpp"

// redefined assert
#include "shared/cpputils/src/assertdef.hpp"


namespace AlgLib {

  /** Creator function  */
  extern "C" ALEXSILVER2_EXP AlgorithmBase::CreatorAlgorithmFun createAlgorithm;

	/** Terminator function function  */
	extern "C" ALEXSILVER2_EXP AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm;


};


#endif // _ALEXSILVER2DEFS_INCLUDED
