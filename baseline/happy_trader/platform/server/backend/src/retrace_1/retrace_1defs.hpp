#ifndef _RETRACE_1DEFS_INCLUDED
#define _RETRACE_1DEFS_INCLUDED



// Handle DLL decl specifier
//
#if defined (WIN32) && defined (SERVICE_BACKEND_DLLS)

#if defined (RETRACE_1_BCK_EXPORTS)
#define RETRACE_1_EXP __declspec(dllexport)
#else
#define RETRACE_1_EXP __declspec(dllimport)
#endif


#else // WIN32

#define RETRACE_1_EXP

#endif

// Include standard stuff
//
#include "shared/cpputils/src/cpputils.hpp"
#include "../alglib/alglib.hpp"


// algorithm holder structure
#include "../inqueue/inqueue.hpp"
#include "../inqueue/commontypes.hpp"


namespace AlgLib {

  /** Creator function  */
  extern "C" RETRACE_1_EXP AlgorithmBase::CreatorAlgorithmFun createAlgorithm;

	/** Terminator function function  */
	extern "C" RETRACE_1_EXP AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm;


};

#endif // _ALGLIBDEFS_INCLUDED
