// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#ifndef _SMARTCOM_PROXY2STDAFX_INCLUDED
#define _SMARTCOM_PROXY2STDAFX_INCLUDED

#ifndef STRICT
#define STRICT
#endif

#include "targetver.h"

#	if defined (WIN32) 
#		if defined (SERVICE_BACKEND_DLLS)
#			if defined (SMARTCOMPROXY2_BCK_EXPORTS)
#				define SMARTCOMPROXY2_EXP __declspec(dllexport)
#			else
#				define SMARTCOMPROXY2_EXP __declspec(dllimport)
#			endif
#		else // WIN32
#			define SMARTCOMPROXY2_EXP
#		endif		
#	endif


#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#ifdef _ATL_MAX_VARTYPES
#undef _ATL_MAX_VARTYPES
#endif
#define _ATL_MAX_VARTYPES 0030


#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#include <AtlConv.h>
#include <atlsafe.h>
#include <ATLComTime.h>
using namespace ATL;

#include "shared/cpputils/src/cpputils.hpp"
#include "../alglib/alglib.hpp"
#include "../brklib/brklib.hpp"

#define SMARTCOM_TEMP_DIR "smartcom_tmp"
#define SMARTCOM_LIB_VER "1.0.7"

//#import "progid:stcln.StServer.2" raw_interfaces_only, raw_native_types, no_namespace, named_guids, auto_search
#import "c:\\Program Files\\IC IT Invest\\SmartCOM 2\\stcln-2.dll"  raw_interfaces_only, raw_native_types, no_namespace, named_guids, auto_search
// raw_dispinterfaces
#endif

