#ifndef _JAVA_POWER_LISTENER_CORE_INCLUDED
#define _JAVA_POWER_LISTENER_CORE_INCLUDED

#include "java_power_listenerdefs.hpp"
#include "HtPowerEventDetector_C.h"


extern "C" {

// forward declaration


void callback_onDDEDataArrived(CppUtils::DDEItem const& item, CppUtils::String const& value);




}; // end of C




#endif 