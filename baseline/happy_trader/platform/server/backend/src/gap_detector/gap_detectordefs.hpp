#ifndef _GAP_DETECTORDEFS_INCLUDED
#define _GAP_DETECTORDEFS_INCLUDED
//
#	if defined (WIN32) 
#		if defined (SERVICE_BACKEND_DLLS)
#			if defined (GAP_DETECTOR_BCK_EXPORTS)
#				define GAP_DETECTOR_EXP __declspec(dllexport)
#			else
#				define GAP_DETECTOR_EXP __declspec(dllimport)
#			endif
#		else // WIN32
#			define GAP_DETECTOR_EXP
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

//
#include "../math/math.hpp"


namespace AlgLib {

  /** Creator function  */
  extern "C" GAP_DETECTOR_EXP AlgorithmBase::CreatorAlgorithmFun createAlgorithm;

	/** Terminator function function  */
	extern "C" GAP_DETECTOR_EXP AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm;


};

#endif // _GAP_DETECTORDEFS_INCLUDED
