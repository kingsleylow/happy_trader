// dllmain.cpp : Implementation of DllMain.

#include "smartcom_proxy2defs.h"
#include "resource.h"
#include "smartcom_proxy2_i.h"
#include "dllmain.h"
#include "dlldatax.h"

Csmartcom_proxy2Module _AtlModule;

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
#ifdef _MERGE_PROXYSTUB
	if (!PrxDllMain(hInstance, dwReason, lpReserved))
		return FALSE;
#endif
	hInstance;
	return _AtlModule.DllMain(dwReason, lpReserved); 
}


