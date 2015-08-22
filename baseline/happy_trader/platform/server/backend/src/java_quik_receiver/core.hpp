#ifndef _JAVA_QUIK_RECEIVER_CORE_INCLUDED
#define _JAVA_QUIK_RECEIVER_CORE_INCLUDED

#include "java_quik_receiverdefs.hpp"
#include "HtQuikListener_C.h"
#include "..\..\src\udf_mysql_quik\udf_mysql_quik.hpp"

extern "C" {
	LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	void registerWindowClass(HINSTANCE hinstDll);
	void callJavaCallback(LPWM_DATASTRUCTURE lpMsg);
	void callJavaErrorCallback(CppUtils::String const& error);
};


#endif