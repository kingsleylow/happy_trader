
#ifndef _CPPUTILS_ASSERTDEF_INCLUDED
#define _CPPUTILS_ASSERTDEF_INCLUDED

// redefines assert if necessary
// must be included AFTER other system-dependent headers

#ifndef NO_LOGGING

#ifdef _DEBUG 
#define HTASSERT( X ) { if( !(X) ) { CppUtils::Log::log( MSG_ERROR, "ASSERTION FAILED", #X, __FILE__, __LINE__ ); } }
#else
#define HTASSERT( X ) ((void)0)
#endif

#else
#define HTASSERT(X)  ((void)0)
#endif

#endif
