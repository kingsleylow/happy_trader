// Copyright VPIsystems Inc., 2001. All rights reserved.
// Carnotstrasse 6, 10587 Berlin, Germany
//
// The copyright to the computer program(s) herein is the
// property of VPI. The program(s) may be used and/or copied
// only with the written permission of VPI or in accordance
// with the terms and conditions stipulated in the agreement/
// contract under which the program(s) have been supplied.
// This copyright notice must not be removed.
//
//




#ifndef _BACKEND_INQUEUE_INQUEUEDEFS_INCLUDED
#define _BACKEND_INQUEUE_INQUEUEDEFS_INCLUDED


// Names of the services
//
//#include "platform/server/backend/src/types.hpp"
//#include "platform/server/backend/src/services.hpp"



// Handle DLL decl specifier
//
#if defined (WIN32) && defined (SERVICE_BACKEND_DLLS)

#if defined (INQUEUE_BCK_EXPORTS)
#define INQUEUE_BCK_EXP __declspec(dllexport)
#else
#define INQUEUE_BCK_EXP __declspec(dllimport)
#endif

#else // defined (WIN32) && defined (SERVER_BACKEND_DLLS)

#define INQUEUE_BCK_EXP

#endif



// Include standard stuff
//
#include "shared/cpputils/src/cpputils.hpp"

// Common decls
//
#include "shared/socketutils/src/socketutils.hpp"
#include "../hlpstruct/hlpstruct.hpp"

// testframe handlers
#ifdef HANDLE_NATIVE_EXCEPTIONS
#include "shared/testframe/src/testsysdep.hpp"
#include "shared/testframe/src/testexcept.hpp"

#endif



#endif // _BACKEND_INQUEUE_ADMINDEFS_INCLUDED