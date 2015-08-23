#ifndef _GERCHIK_RANGE_BREAKDEFS_INCLUDED
#define _GERCHIK_RANGE_BREAKDEFS_INCLUDED
//
#	if defined (WIN32) 
#		if defined (SERVICE_BACKEND_DLLS)
#			if defined (GERCHIK_RANGE_BREAK_BCK_EXPORTS)
#				define GERCHIK_RANGE_BREAK_EXP __declspec(dllexport)
#			else
#				define GERCHIK_RANGE_BREAK_EXP __declspec(dllimport)
#			endif
#		else // WIN32
#			define GERCHIK_RANGE_BREAK_EXP
#		endif		
#	endif

// Include standard stuff
//
#include "shared/cpputils/src/cpputils.hpp"
#include "../alglib/alglib.hpp"
#include "../brklib/brklib.hpp"

// redefined assert
#include "shared/cpputils/src/assertdef.hpp"

// algorithm holder structure
#include "../inqueue/inqueue.hpp"
#include "../inqueue/commontypes.hpp"

//



namespace AlgLib {

  /** Creator function  */
  extern "C" GERCHIK_RANGE_BREAK_EXP AlgorithmBase::CreatorAlgorithmFun createAlgorithm;

	/** Terminator function function  */
	extern "C" GERCHIK_RANGE_BREAK_EXP AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm;


};
//
#endif // _GERCHIK_RANGE_BREAKDEFS_INCLUDED
