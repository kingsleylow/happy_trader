#ifndef _BRKLIBBTESTDEFS_INCLUDED
#define _BRKLIBBTESTDEFS_INCLUDED
//
#	if defined (WIN32) 
#		if defined (SERVICE_BACKEND_DLLS)
#			if defined (BRKLIBBTEST_BCK_EXPORTS)
#				define BRKLIBBTEST_EXP __declspec(dllexport)
#			else
#				define BRKLIBBTEST_EXP __declspec(dllimport)
#			endif
#		else // WIN32
#			define BRKLIBBTEST_EXP
#		endif		
#	endif
//

// include standard stuff

#include "shared/cpputils/src/cpputils.hpp"
#include "../brklib/brklib.hpp"
#include "../inqueue/inqueue.hpp"


// redefined assert
#include "shared/cpputils/src/assertdef.hpp"

namespace BrkLib {
	/** Creator function  */
	extern "C" BRKLIBBTEST_EXP BrokerBase::CreatorBrokerFun createBroker;

	/** Terminator function function  */
	extern "C" BRKLIBBTEST_EXP BrokerBase::DestroyerBrokerFun destroyBroker;

}


#endif // _BRKLIBBTESTDEFS_INCLUDED
