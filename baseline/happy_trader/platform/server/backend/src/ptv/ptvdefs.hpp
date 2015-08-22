#ifndef _PTVDEFS_INCLUDED
#define _PTVDEFS_INCLUDED
//
#	if defined (WIN32) 
#		if defined (SERVICE_BACKEND_DLLS)
#			if defined (PTV_BCK_EXPORTS)
#				define PTV_EXP __declspec(dllexport)
#			else
#				define PTV_EXP __declspec(dllimport)
#			endif
#		else // WIN32
#			define PTV_EXP
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
  extern "C" PTV_EXP AlgorithmBase::CreatorAlgorithmFun createAlgorithm;

	/** Terminator function function  */
	extern "C" PTV_EXP AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm;


};

#endif // _PTVDEFS_INCLUDED
