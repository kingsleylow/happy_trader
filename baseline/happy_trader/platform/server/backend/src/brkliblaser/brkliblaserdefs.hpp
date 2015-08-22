#ifndef _BRKLIBLASERDEFS_INCLUDED
#define _BRKLIBLASERDEFS_INCLUDED
//
#	if defined (WIN32) 
#		if defined (SERVICE_BACKEND_DLLS)
#			if defined (BRKLIBLASER_BCK_EXPORTS)
#				define BRKLIBLASER_EXP __declspec(dllexport)
#			else
#				define BRKLIBLASER_EXP __declspec(dllimport)
#			endif
#		else // WIN32
#			define BRKLIBLASER_EXP
#		endif		
#	endif

// include standard stuff

#include "shared/cpputils/src/cpputils.hpp"
#include "../brklib/brklib.hpp"
#include "../inqueue/inqueue.hpp"

//include laser library
#include "GTAPI32.h"

/// testframe handlers
#ifdef HANDLE_NATIVE_EXCEPTIONS
#include "shared/testframe/src/testsysdep.hpp"
#include "shared/testframe/src/testexcept.hpp"

#endif

// redefined assert
#include "shared/cpputils/src/assertdef.hpp"


// reassign event plugins


namespace BrkLib {
	/** Creator function  */
	extern "C" BRKLIBLASER_EXP BrokerBase::CreatorBrokerFun createBroker;

	/** Terminator function function  */
	extern "C" BRKLIBLASER_EXP BrokerBase::DestroyerBrokerFun destroyBroker;

}


//
#endif // _BRKLIBLASERDEFS_INCLUDED
