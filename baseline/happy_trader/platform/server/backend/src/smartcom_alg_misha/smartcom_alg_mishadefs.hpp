#ifndef _SMARTCOM_ALG_MISHADEFS_INCLUDED
#define _SMARTCOM_ALG_MISHADEFS_INCLUDED

#	if defined (WIN32) 
#		if defined (SERVICE_BACKEND_DLLS)
#			if defined (SMARTCOM_ALG_MISHA_BCK_EXPORTS)
#				define SMARTCOM_ALG_MISHA_EXP __declspec(dllexport)
#			else
#				define SMARTCOM_ALG_MISHA_EXP __declspec(dllimport)
#			endif
#		else // WIN32
#			define SMARTCOM_ALG_MISHA_EXP
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
extern "C" SMARTCOM_ALG_MISHA_EXP AlgorithmBase::CreatorAlgorithmFun createAlgorithm;

/** Terminator function function  */
extern "C" SMARTCOM_ALG_MISHA_EXP AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm;


};


#endif // _SMARTCOM_ALG_MISHADEFS_INCLUDED
