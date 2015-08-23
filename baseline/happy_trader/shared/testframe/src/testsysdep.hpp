


#ifndef TESTFRAME_TESTSYSDEP_INCLUDED
#define TESTFRAME_TESTSYSDEP_INCLUDED





#if defined (_WIN32)
//=============================================================
// Windows
//=============================================================


#ifndef _BIND_TO_CURRENT_VCLIBS_VERSION
#define _BIND_TO_CURRENT_VCLIBS_VERSION 1
#endif


// We compile for XP
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#else
#define _WIN32_WINNT 0x0501
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN

// Handle DLL decl specifier
//
#if defined (TESTFRAME_EXPORTS)
#define TESTFRAME_EXP __declspec(dllexport)
#else
#define TESTFRAME_EXP __declspec(dllimport)
#endif


// MS VC specific
//

// MSVC 2008 4996
#if defined (_MSC_VER)
#pragma warning(disable: 4786 4251 4275 4996)

#include <stdlib.h>
#include <string.h>

double getDoubleTime(void);


#define strdup _strdup


#endif // MSVC



#else // _WIN32
//=============================================================
// Non-Windows
//=============================================================

#define TESTFRAME_EXP

#include <stdlib.h>
#include <string.h>

double getDoubleTime(void);

#endif // _WIN32




// Generic decls
//

#include "testexcept.hpp"
#include <list>
#include <map>



namespace Testframe {

	using namespace std;

	/** Boot code function */
	TESTFRAME_EXP void init (int newCw = 0);
	
}



#endif // TESTFRAME_TESTSYSDEP_INCLUDED

