
/** \file

		DLL initialization file.

*/


#ifdef _WIN32

#ifndef _BIND_TO_CURRENT_VCLIBS_VERSION
#define _BIND_TO_CURRENT_VCLIBS_VERSION 1
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>



BOOL __declspec(dllexport) __stdcall DllMain(
		HANDLE hModule,
		DWORD  ul_reason_for_call,
		LPVOID lpReserved
  )
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            break;

        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            break;

        case DLL_PROCESS_DETACH:
            break;
  }
  return TRUE;
}


#endif // _WIN32
