#ifndef _BRKLIBQUIKDEFS_INCLUDED
#define _BRKLIBQUIKDEFS_INCLUDED
//
#	if defined (WIN32) 
#		if defined (SERVICE_BACKEND_DLLS)
#			if defined (BRKLIBQUIK_BCK_EXPORTS)
#				define BRKLIBQUIK_EXP __declspec(dllexport)
#			else
#				define BRKLIBQUIK_EXP __declspec(dllimport)
#			endif
#		else // WIN32
#			define BRKLIBQUIK_EXP
#		endif		
#	endif

// include standard stuff

//#define _USE_32BIT_TIME_T

#include "shared/cpputils/src/cpputils.hpp"
#include "../brklib/brklib.hpp"
#include "../inqueue/inqueue.hpp"

//include quik library
#include "trans2quik_api.h"

// redefined assert
#include "shared/cpputils/src/assertdef.hpp"


namespace BrkLib {
	/** Creator function  */
	extern "C" BRKLIBQUIK_EXP BrokerBase::CreatorBrokerFun createBroker;

	/** Terminator function function  */
	extern "C" BRKLIBQUIK_EXP BrokerBase::DestroyerBrokerFun destroyBroker;

}

//
#endif // _BRKLIBQUIKDEFS_INCLUDED
