#ifndef _SOCKETUTILSDEFS_INCLUDED
#define _SOCKETUTILSDEFS_INCLUDED



// Handle DLL decl specifier
//
#if defined (WIN32) && defined (SOCKETUTILS_DLL)

#if defined (SOCKETUTILS_EXPORTS)
#define SOCKETUTILS_EXP __declspec(dllexport)
#else
#define SOCKETUTILS_EXP __declspec(dllimport)
#endif

#else // WIN32

#define SOCKETUTILS_EXP

#endif

#pragma warning(disable: 4275)
#pragma warning(disable: 4251)



#endif // _SOCKETUTILSDEFS_INCLUDED
