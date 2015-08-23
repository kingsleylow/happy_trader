#ifndef _BRKLIBDEFS_INCLUDED
#define _BRKLIBDEFS_INCLUDED
//

#if defined (WIN32) && defined (SERVICE_BACKEND_DLLS)

#if defined (BRKLIB_BCK_EXPORTS)
#define BRKLIB_EXP __declspec(dllexport)
#else
#define BRKLIB_EXP __declspec(dllimport)
#endif


#else // WIN32

#define BRKLIB_EXP

#endif


//

// Include standard stuff
//
#include "shared/cpputils/src/cpputils.hpp"
#include "../hlpstruct/hlpstruct.hpp"





#endif // _BRKLIBDEFS_INCLUDED
