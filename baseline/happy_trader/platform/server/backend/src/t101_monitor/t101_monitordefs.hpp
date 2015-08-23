#ifndef _T101_MONITORDEFS_INCLUDED
#define _T101_MONITORDEFS_INCLUDED
//
#	if defined (WIN32) 
#		if defined (SERVICE_BACKEND_DLLS)
#			if defined (T101_MONITOR_BCK_EXPORTS)
#				define T101_MONITOR_EXP __declspec(dllexport)
#			else
#				define T101_MONITOR_EXP __declspec(dllimport)
#			endif
#		else // WIN32
#			define T101_MONITOR_EXP
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
  extern "C" T101_MONITOR_EXP AlgorithmBase::CreatorAlgorithmFun createAlgorithm;

	/** Terminator function function  */
	extern "C" T101_MONITOR_EXP AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm;


};


#endif // _T101_MONITORDEFS_INCLUDED
