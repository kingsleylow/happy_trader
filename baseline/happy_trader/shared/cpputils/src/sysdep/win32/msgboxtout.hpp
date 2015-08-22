#ifndef _MSGBOX_TOUT_INCLUDED
#define _MSGBOX_TOUT_INCLUDED

#include "../../cpputilsdefs.hpp"
#include <tchar.h>

#define MB_TIMEDOUT 32000
namespace CppUtils {

typedef int (__stdcall *MSGBOXAAPI)(IN HWND hWnd, 
        IN LPCSTR lpText, IN LPCSTR lpCaption, 
        IN UINT uType, IN WORD wLanguageId, IN DWORD dwMilliseconds);
typedef int (__stdcall *MSGBOXWAPI)(IN HWND hWnd, 
        IN LPCWSTR lpText, IN LPCWSTR lpCaption, 
        IN UINT uType, IN WORD wLanguageId, IN DWORD dwMilliseconds);

int MessageBoxTimeoutA(IN HWND hWnd, IN LPCSTR lpText, 
    IN LPCSTR lpCaption, IN UINT uType, 
    IN WORD wLanguageId, IN DWORD dwMilliseconds);
int MessageBoxTimeoutW(IN HWND hWnd, IN LPCWSTR lpText, 
    IN LPCWSTR lpCaption, IN UINT uType, 
    IN WORD wLanguageId, IN DWORD dwMilliseconds);

#ifdef UNICODE
    #define MessageBoxTimeout MessageBoxTimeoutW
#else
    #define MessageBoxTimeout MessageBoxTimeoutA
#endif 


};

#endif