

#ifndef _ALGLIBDUMMYDEFS_INCLUDED
#define _ALGLIBDUMMYDEFS_INCLUDED



// Handle DLL decl specifier
//
#if defined (WIN32) && defined (SERVICE_BACKEND_DLLS)

#if defined (ALGLIBDUMMY_BCK_EXPORTS)
#define ALGLIBDUMMY_EXP __declspec(dllexport)
#else
#define ALGLIBDUMMY_EXP __declspec(dllimport)
#endif


#else // WIN32

#define ALGLIBDUMMY_EXP

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
  extern "C" ALGLIBDUMMY_EXP AlgorithmBase::CreatorAlgorithmFun createAlgorithm;

	/** Terminator function function  */
	extern "C" ALGLIBDUMMY_EXP AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm;


};

#endif // _ALGLIBDEFS_INCLUDED
