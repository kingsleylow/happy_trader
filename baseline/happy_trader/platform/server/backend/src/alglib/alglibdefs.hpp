
#ifndef _ALGLIBDEFS_INCLUDED
#define _ALGLIBDEFS_INCLUDED



// Handle DLL decl specifier
//
#if defined (WIN32) && defined (SERVICE_BACKEND_DLLS)

#if defined (ALGLIB_BCK_EXPORTS)
#define ALGLIB_EXP __declspec(dllexport)
#else
#define ALGLIB_EXP __declspec(dllimport)
#endif


#else // WIN32

#define ALGLIB_EXP

#endif

// Include standard stuff
//
#include "shared/cpputils/src/cpputils.hpp"



#include "../hlpstruct/hlpstruct.hpp"


// Defines and constants
//
namespace AlgLib {

	
}

#endif // _ALGLIBDEFS_INCLUDED
