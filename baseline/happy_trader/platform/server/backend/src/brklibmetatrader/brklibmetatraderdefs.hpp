/**
This is almost the same as brklibbtest_bck but it is intended to work in RT with preliminary passedc metainfo data in advance
No spped-up caching as this is RT mode
*/

#ifndef _BRKLIBMETATRADERDEFS_INCLUDED
#define _BRKLIBMETATRADERDEFS_INCLUDED
//
#	if defined (WIN32) 
#		if defined (SERVICE_BACKEND_DLLS)
#			if defined (BRKLIBMETATRADER_BCK_EXPORTS)
#				define BRKLIBMETATRADER_EXP __declspec(dllexport)
#			else
#				define BRKLIBMETATRADER_EXP __declspec(dllimport)
#			endif
#		else // WIN32
#			define BRKLIBRTTEST_EXP
#		endif		
#	endif


// include standard stuff

#include "shared/cpputils/src/cpputils.hpp"
#include "../brklib/brklib.hpp"
#include "../inqueue/inqueue.hpp"

#include "../math/math.hpp"


// redefined assert
#include "shared/cpputils/src/assertdef.hpp"

namespace BrkLib {
	/** Creator function  */
	extern "C" BRKLIBMETATRADER_EXP BrokerBase::CreatorBrokerFun createBroker;

	/** Terminator function function  */
	extern "C" BRKLIBMETATRADER_EXP BrokerBase::DestroyerBrokerFun destroyBroker;

}




//
#endif // _BRKLIBRTTESTDEFS_INCLUDED
