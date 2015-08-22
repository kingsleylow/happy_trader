#ifndef _EVENT_BASED_MATHDEFS_INCLUDED
#define _EVENT_BASED_MATHDEFS_INCLUDED

#	if defined (WIN32) 
#		if defined (SERVICE_BACKEND_DLLS)
#			if defined (EVENT_BASED_MATH_BCK_EXPORTS)
#				define EVENT_BASED_MATH_EXP __declspec(dllexport)
#			else
#				define EVENT_BASED_MATH_EXP __declspec(dllimport)
#			endif
#		else // WIN32
#			define EVENT_BASED_MATH_EXP
#		endif		
#	endif


// Include standard stuff
//
#include "shared/cpputils/src/cpputils.hpp"
#include "../alglib/alglib.hpp"

// redefined assert
 #include "shared/cpputils/src/assertdef.hpp"

// algorithm holder structure
#include "../inqueue/inqueue.hpp"
#include "../inqueue/commontypes.hpp"

#include "../math/math.hpp"


namespace AlgLib {

/** Creator function  */
extern "C" EVENT_BASED_MATH_EXP AlgorithmBase::CreatorAlgorithmFun createAlgorithm;

/** Terminator function function  */
extern "C" EVENT_BASED_MATH_EXP AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm;


};


#endif // _EVENT_BASED_MATHDEFS_INCLUDED
