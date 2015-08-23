
/** \file

		File implementing Mutex.

	
*/


#include "../../mutex.hpp"

#include "../../except.hpp"
#include "../../assertdef.hpp"
//#include "../../stringtable.hpp"


namespace CppUtils {


//=============================================================
// Class Mutex
//=============================================================
Mutex::Mutex (
	) :
		mutex_m(),
		count_m(0)
	
{
#if(_WIN32_WINNT >= 0x0403)

	// Use spin count and signal event pre-allocation
	DWORD scount = 4000;
	scount = scount | 0x80000000;
	InitializeCriticalSectionAndSpinCount( &mutex_m, scount );

#else

	// W95/98
	mutex_m = CreateMutex( NULL, 0, NULL );

	// Success?
	if( mutex_m == NULL )
		THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Threading_CreatingMutex", getOSError().c_str() );

#endif
}


//-------------------------------------------------------------

Mutex::~Mutex(
	)
{
	// Close the mutex
#if(_WIN32_WINNT >= 0x0400)

	DeleteCriticalSection( &mutex_m );

#else
	// W95/98
	if( CloseHandle( mutex_m ) == NULL )
		THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Threading_DestroyingMutex", getOSError().c_str() );

#endif

	HTASSERT( count_m == 0 );
}
//======================================
// Methods
//======================================
void Mutex::lock (
	) const throw (
		ExceptionInternal
	)
{
#if(_WIN32_WINNT >= 0x0400)

	EnterCriticalSection( &mutex_m );

#else

	// W95/98
	if( WaitForSingleObject( mutex_m, INFINITE ) == WAIT_FAILED )
		THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Threading_WaitingForMutex", getOSError().c_str() );

#endif

	// Inc counter; no locking needed on counter here
	++count_m;
}
//-------------------------------------------------------------
bool Mutex::tryLock (
	) const throw (
		ExceptionInternal
	)
{
	bool result = false;

#if(_WIN32_WINNT >= 0x0400)

	result = ( TryEnterCriticalSection( &mutex_m ) != 0 );

#else

	// W95/98
	DWORD res = WaitForSingleObject( mutex_m, 0 );
	if( res == WAIT_TIMEOUT )
		// Mutex not obtained
		result = false;
	else if( res == WAIT_FAILED )
		// A problem occured
		THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Threading_WaitingForMutex", getOSError().c_str() )
	else
		// Mutex locked now
		result = true;

#endif

	// Count if locked
	if( result )
		++count_m;

	// Done
	return result;
}
//-------------------------------------------------------------
void Mutex::unlock (
	) const throw (
		ExceptionInternal
	)
{
	// Dec counter before unlocking!
	--count_m;

#if(_WIN32_WINNT >= 0x0400)

	LeaveCriticalSection( &mutex_m );

#else

	// W95/98
	if( ReleaseMutex( mutex_m ) == NULL )
		THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Threading_ReleasingMutex", getOSError().c_str() );

#endif
}
//-------------------------------------------------------------
bool Mutex::isLocked (
	) const throw (
		ExceptionInternal
	)
{
#if(_WIN32_WINNT >= 0x0400)

	int res = TryEnterCriticalSection( &mutex_m );

	if( res == 0 )
		// Mutex is locked by somebody else
		return true;
	else {
		// Mutex available => release the lock that we've just obtained
		LeaveCriticalSection( &mutex_m );

		// Done
		return false;
	}

#else

	// W95/98
	DWORD res = WaitForSingleObject( mutex_m, 0 );

	if( res == WAIT_TIMEOUT )
		// Mutex is locked by somebody else
		return true;
	else if( res == WAIT_FAILED )
		// A problem occured
		THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Threading_WaitingForMutex", getOSError().c_str() );
	else {
		// Mutex available => release the lock that we've just obtained
		if( ReleaseMutex( mutex_m ) == NULL )
			THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Threading_ReleasingMutex", getOSError().c_str() );

		// Done
		return false;
	}

#endif
}
//-------------------------------------------------------------
unsigned int Mutex::getLockCount (
	) const
{
	return count_m;
}

// End of namespace
}
