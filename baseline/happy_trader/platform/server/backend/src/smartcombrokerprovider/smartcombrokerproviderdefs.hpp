#ifndef _SMARTCOMBROKERPROVIDERDEFS_INCLUDED
#define _SMARTCOMBROKERPROVIDERDEFS_INCLUDED

#	if defined (WIN32) 
#		if defined (SERVICE_BACKEND_DLLS)
#			if defined (SMARTCOMBROKERPROVIDER_BCK_EXPORTS)
#				define SMARTCOMBROKERPROVIDER_EXP __declspec(dllexport)
#			else
#				define SMARTCOMBROKERPROVIDER_EXP __declspec(dllimport)
#			endif
#		else // WIN32
#			define SMARTCOMBROKERPROVIDER_EXP
#		endif		
#	endif


// Include standard stuff
//
#include "shared/cpputils/src/cpputils.hpp"
#include "../alglib/alglib.hpp"
#include "../brklib/brklib.hpp"



// algorithm holder structure
#include "../inqueue/inqueue.hpp"
#include "../inqueue/commontypes.hpp"

#import "progid:smartcom_proxy2.MainSmartComEngine" raw_interfaces_only, raw_native_types, rename_namespace("SmartComEngine"), auto_search
#import "progid:stcln.StServer.2" raw_interfaces_only, raw_native_types, no_namespace, named_guids, auto_search

// debug or release versions
#ifdef _DEBUG
#define SMART_COM_ENGINE_DLL "smartcom_proxy2_d.dll" 
#else
#define SMART_COM_ENGINE_DLL "smartcom_proxy2.dll" 
#endif



namespace BrkLib {

/** Creator function  */
extern "C" SMARTCOMBROKERPROVIDER_EXP BrkLib::BrokerBase::CreatorBrokerFun createBroker;

/** Terminator function function  */
extern "C" SMARTCOMBROKERPROVIDER_EXP BrkLib::BrokerBase::DestroyerBrokerFun destroyBroker;

 
};


#endif // _SMARTCOMBROKERPROVIDERDEFS_INCLUDED
