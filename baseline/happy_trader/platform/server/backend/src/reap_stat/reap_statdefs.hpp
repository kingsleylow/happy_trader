#ifndef _REAP_STATDEFS_INCLUDED
#define _REAP_STATDEFS_INCLUDED
//
#	if defined (WIN32) 
#		if defined (SERVICE_BACKEND_DLLS)
#			if defined (REAP_STAT_BCK_EXPORTS)
#				define REAP_STAT_EXP __declspec(dllexport)
#			else
#				define REAP_STAT_EXP __declspec(dllimport)
#			endif
#		else // WIN32
#			define REAP_STAT_EXP
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
  extern "C" REAP_STAT_EXP AlgorithmBase::CreatorAlgorithmFun createAlgorithm;

	/** Terminator function function  */
	extern "C" REAP_STAT_EXP AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm;


};

#endif // _REAP_STATDEFS_INCLUDED
