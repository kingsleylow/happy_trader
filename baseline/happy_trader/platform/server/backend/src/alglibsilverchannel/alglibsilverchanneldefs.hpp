
#ifndef _ALGLIBSILVERCHANNELDEFS_INCLUDED
#define _ALGLIBSILVERCHANNELDEFS_INCLUDED



// Handle DLL decl specifier
//
#if defined (WIN32) && defined (SERVICE_BACKEND_DLLS)

#if defined (ALGLIBSILVERCHANNEL_BCK_EXPORTS)
#define ALGLIBSILVERCHANNEL_EXP __declspec(dllexport)
#else
#define ALGLIBSILVERCHANNEL_EXP __declspec(dllimport)
#endif


#else // WIN32

#define ALGLIBSILVERCHANNEL_EXP

#endif

// Include standard stuff
//
#include "shared/cpputils/src/cpputils.hpp"
#include "../alglib/alglib.hpp"

// algorithm holder structure
#include "../inqueue/inqueue.hpp"
#include "../inqueue/commontypes.hpp"

// redefined assert
#include "shared/cpputils/src/assertdef.hpp"


namespace AlgLib {

  /** Creator function  */
  extern "C" ALGLIBSILVERCHANNEL_EXP AlgorithmBase::CreatorAlgorithmFun createAlgorithm;

	/** Terminator function function  */
	extern "C" ALGLIBSILVERCHANNEL_EXP AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm;


};

#endif // _ALGLIBDEFS_INCLUDED
