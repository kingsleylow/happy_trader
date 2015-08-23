#ifndef _PAIR_TRADEDEFS_INCLUDED
#define _PAIR_TRADEDEFS_INCLUDED

#	if defined (WIN32) 
#		if defined (SERVICE_BACKEND_DLLS)
#			if defined (PAIR_TRADE_BCK_EXPORTS)
#				define PAIR_TRADE_EXP __declspec(dllexport)
#			else
#				define PAIR_TRADE_EXP __declspec(dllimport)
#			endif
#		else // WIN32
#			define PAIR_TRADE_EXP
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

#include "../math/math.hpp"


namespace AlgLib {

/** Creator function  */
extern "C" PAIR_TRADE_EXP AlgorithmBase::CreatorAlgorithmFun createAlgorithm;

/** Terminator function function  */
extern "C" PAIR_TRADE_EXP AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm;


};


#endif // _PAIR_TARDEDEFS_INCLUDED
