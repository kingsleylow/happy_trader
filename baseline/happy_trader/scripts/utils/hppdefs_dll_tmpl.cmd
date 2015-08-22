@echo off
echo #ifndef %DEFS_HEADER_FILE_HEADER%
echo #define %DEFS_HEADER_FILE_HEADER%
echo:
echo #	if defined (WIN32) 
echo #		if defined (SERVICE_BACKEND_DLLS)
echo #			if defined (%EXPORT_CPP_DEFINITION%)
echo #				define %DEFS_HEADER_FILE_EXPORT_PRFX% __declspec(dllexport)
echo #			else
echo #				define %DEFS_HEADER_FILE_EXPORT_PRFX% __declspec(dllimport)
echo #			endif
echo #		else // WIN32
echo #			define %DEFS_HEADER_FILE_EXPORT_PRFX%
echo #		endif		
echo #	endif
echo:
echo:
echo // Include standard stuff
echo //
echo #include "shared/cpputils/src/cpputils.hpp"
echo #include "../alglib/alglib.hpp"
echo:
echo // redefined assert
echo: #include "shared/cpputils/src/assertdef.hpp"
echo:
echo // algorithm holder structure
echo #include "../inqueue/inqueue.hpp"
echo #include "../inqueue/commontypes.hpp"
echo:
echo:
echo namespace AlgLib {
echo:
echo	/** Creator function  */
echo	extern "C" %DEFS_HEADER_FILE_EXPORT_PRFX% AlgorithmBase::CreatorAlgorithmFun createAlgorithm;
echo:
echo	/** Terminator function function  */
echo	extern "C" %DEFS_HEADER_FILE_EXPORT_PRFX% AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm;
echo:
echo:
echo };
echo:
echo:
echo #endif // %DEFS_HEADER_FILE_HEADER%
