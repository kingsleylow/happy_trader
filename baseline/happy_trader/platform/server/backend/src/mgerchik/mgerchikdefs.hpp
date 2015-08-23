#ifndef _MGERCHIKDEFS_INCLUDED
#define _MGERCHIKDEFS_INCLUDED
//
#	if defined (WIN32) 
#		if defined (SERVICE_BACKEND_DLLS)
#			if defined (MGERCHIK_BCK_EXPORTS)
#				define MGERCHIK_EXP __declspec(dllexport)
#			else
#				define MGERCHIK_EXP __declspec(dllimport)
#			endif
#		else // WIN32
#			define MGERCHIK_EXP
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
  extern "C" MGERCHIK_EXP AlgorithmBase::CreatorAlgorithmFun createAlgorithm;

	/** Terminator function function  */
	extern "C" MGERCHIK_EXP AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm;


};

#endif // _MGERCHIKDEFS_INCLUDED
