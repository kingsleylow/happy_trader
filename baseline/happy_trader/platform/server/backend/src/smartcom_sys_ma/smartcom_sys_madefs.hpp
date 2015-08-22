#ifndef _SMARTCOM_SYS_MADEFS_INCLUDED
#define _SMARTCOM_SYS_MADEFS_INCLUDED

#	if defined (WIN32) 
#		if defined (SERVICE_BACKEND_DLLS)
#			if defined (SMARTCOM_SYS_MA_BCK_EXPORTS)
#				define SMARTCOM_SYS_MA_EXP __declspec(dllexport)
#			else
#				define SMARTCOM_SYS_MA_EXP __declspec(dllimport)
#			endif
#		else // WIN32
#			define SMARTCOM_SYS_MA_EXP
#		endif		
#	endif


// Include standard stuff
//
#include "shared/cpputils/src/cpputils.hpp"
#include "../alglib/alglib.hpp"

// algorithm holder structure
#include "../inqueue/inqueue.hpp"
#include "../inqueue/commontypes.hpp"

#include "../math/math.hpp"


namespace AlgLib {

/** Creator function  */
extern "C" SMARTCOM_SYS_MA_EXP AlgorithmBase::CreatorAlgorithmFun createAlgorithm;

/** Terminator function function  */
extern "C" SMARTCOM_SYS_MA_EXP AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm;


};


#endif // _SMARTCOM_SYS_MADEFS_INCLUDED
