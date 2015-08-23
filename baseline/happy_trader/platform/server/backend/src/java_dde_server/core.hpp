#ifndef _JAVA_DDE_SERVER_CORE_INCLUDED
#define _JAVA_DDE_SERVER_CORE_INCLUDED

#include "java_dde_serverdefs.hpp"
#include "HtDDEServer_C.h"

extern "C" {

// forward declaration

// usual data
void callback_onDDEDataArrived(CppUtils::DDEItem const& item, CppUtils::String const& value);

// XLTable data
void callback_onDDEXLTableDataArrived(CppUtils::DDEItem const& item, vector< vector <CppUtils::String> > const& out);


}; // end of C




#endif 