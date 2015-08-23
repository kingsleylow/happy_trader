#ifndef _GAP_CALCDEFS_INCLUDED
#define _GAP_CALCDEFS_INCLUDED
//
#	if defined (WIN32) 
#		if defined (SERVICE_BACKEND_DLLS)
#			if defined (GAP_CALC_BCK_EXPORTS)
#				define GAP_CALC_EXP __declspec(dllexport)
#			else
#				define GAP_CALC_EXP __declspec(dllimport)
#			endif
#		else // WIN32
#			define GAP_CALC_EXP
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
  extern "C" GAP_CALC_EXP AlgorithmBase::CreatorAlgorithmFun createAlgorithm;

	/** Terminator function function  */
	extern "C" GAP_CALC_EXP AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm;


};


#endif // _GAP_CALCDEFS_INCLUDED
