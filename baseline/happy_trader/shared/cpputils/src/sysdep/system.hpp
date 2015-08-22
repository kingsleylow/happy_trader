
#ifndef _CPPUTILS_SYSDEP_SYSTEM_INCLUDED
#define _CPPUTILS_SYSDEP_SYSTEM_INCLUDED


//=============================================================
// Set system dependent environment variables
//=============================================================

#if defined (_WIN32)

//=============================================================
// Windows
//=============================================================

#define PLATF win32

// Pull in system default includes
//
#include "win32/system.hpp"




#else // WIN32

//=============================================================
// Unsupported
//=============================================================
#error 'Platform not supported'


#endif // Platform dependencies




#endif // _CPPUTILS_SYSDEP_SYSTEM_INCLUDED