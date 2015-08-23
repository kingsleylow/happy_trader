#ifndef _MATHDEFS_INCLUDED
#define _MATHDEFS_INCLUDED



// Handle DLL decl specifier
//
#if defined (WIN32) && defined (SERVICE_BACKEND_DLLS)

#if defined (MATH_BCK_EXPORTS)
#define MATH_EXP __declspec(dllexport)
#else
#define MATH_EXP __declspec(dllimport)
#endif


#else // WIN32

#define MATH_EXP

#endif


// common headers
#include "shared/cpputils/src/cpputils.hpp"
#include "../hlpstruct/hlpstruct.hpp"

#define MATH "MATH"


#endif // _MATHDEFS_INCLUDED
