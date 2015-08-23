

#ifndef _CPPUTILS_SYSTEM_WIN32_INCLUDED
#define _CPPUTILS_SYSTEM_WIN32_INCLUDED


//=============================================================
// Windows
//=============================================================

// MS VC specific
//

// 4996 - security warn for 2008
// 4430
// 4005 - macro redef
#if defined (_MSC_VER)
#pragma warning(disable: 4251 4275 4290 4250 4503 4786 4018 4244 4996)
#endif

#ifndef _USE_32BIT_TIME_T
#define _USE_32BIT_TIME_T
#endif


// We compile for XP
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN



// Includes
//

// Complex numbers
#include <complex>

// Strings & STL
#include <string>
#include <vector>
#include <map>
#include <set>
#include <atomic>
	

// Streams
#include <iostream>
#include <fstream>

// Block allocated STL objects
#define DEFINE_BLOCK_ALLOCATED_MAP
#define DEFINE_BLOCK_ALLOCATED_SET
#define DEFINE_BLOCK_ALLOCATED_LIST

#include "blockallocator.h"


// Bring std into scope
//
using namespace std;


// Typedefs
//
namespace CppUtils {

	/** Alias for std::complex<double> */
	typedef complex<double> Complex;

	/** Char. */
	typedef char Char;

	/** Byte. */
	typedef char Byte;

	/** Alias for std::string. */
	typedef std::string String;

	/** Alias for std::vector<std::string> */
	typedef std::vector<String> StringList;

	/** Alias for std::set<std::string> */
	typedef std::set<String> StringSet;

	/** Alias for std::vector<int> */
	typedef std::vector<int> IntList;

	/** Alias for std::vector<double> */
	typedef std::vector<double> DoubleList;

	/** Alias for std::vector<bool> */
	typedef std::vector<bool> BoolList;

	/** Alias for std::map<std::string, std::string> */
	typedef std::map<String, String> StringMap;

	/** Alias for std::vector< std::pair<String, String> > */
	typedef std::vector< std::pair<String, String> > StringPairList;

	/** Thread Execution State */
	typedef EXECUTION_STATE ThreadExecutionState;

	/** Declare Windows process handle type. */
	typedef HANDLE ProcessHandle;

	/** Declare Windows threading handle type. */
	typedef HANDLE ThreadHandle;

	/** Declare Windows threading semaphore type. */
	typedef HANDLE NativeSemaphore;

	/** Declare Windows event type. */
	typedef HANDLE NativeEvent;

	/** Declare file handle type. */
	typedef HANDLE FileHandle;

	/** Unsigned 64 bit integer*/
	typedef unsigned __int64 UnsignedLongLong;

	/** Signed 64 bit integer*/
	typedef signed __int64 LongLong;

	typedef __int64 Int64;

#if(_WIN32_WINNT >= 0x0400)

	/** Declare Windows mutex type. Use
			critical sections instead of mutex on NT. */
	typedef CRITICAL_SECTION NativeMutex;

#else

	/** Declare Windows mutex type. Use
			a normal mutex on 9X. */
	typedef HANDLE NativeMutex;

#endif


#define STRUCT_ALIGN(X) __declspec(align(X))

	

// End of namespace
};


#endif // _CPPUTILS_SYSTEM_WIN32_INCLUDED

