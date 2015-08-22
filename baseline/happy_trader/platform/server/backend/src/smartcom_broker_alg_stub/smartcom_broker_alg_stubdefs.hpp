#ifndef _SMARTCOM_BROKER_ALG_STUBDEFS_INCLUDED
#define _SMARTCOM_BROKER_ALG_STUBDEFS_INCLUDED

#	if defined (WIN32) 
#		if defined (SERVICE_BACKEND_DLLS)
#			if defined (SMARTCOM_BROKER_ALG_STUB_BCK_EXPORTS)
#				define SMARTCOM_BROKER_ALG_STUB_EXP __declspec(dllexport)
#			else
#				define SMARTCOM_BROKER_ALG_STUB_EXP __declspec(dllimport)
#			endif
#		else // WIN32
#			define SMARTCOM_BROKER_ALG_STUB_EXP
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


namespace AlgLib {

/** Creator function  */
extern "C" SMARTCOM_BROKER_ALG_STUB_EXP AlgorithmBase::CreatorAlgorithmFun createAlgorithm;

/** Terminator function function  */
extern "C" SMARTCOM_BROKER_ALG_STUB_EXP AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm;


};


#endif // _SMARTCOM_BROKER_ALG_STUBDEFS_INCLUDED
