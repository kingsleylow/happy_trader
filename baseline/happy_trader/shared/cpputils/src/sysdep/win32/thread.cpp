
//
/** \file

		File implementing Thread.

	
*/


#include "../../thread.hpp"
#include "../../except.hpp"
#include "../../assertdef.hpp"
//#include "../../stringtable.hpp"


namespace CppUtils {


#define THREAD_STR "THREAD"


//=============================================================
// Class Thread
//=============================================================
Thread::Thread(
		ThreadFun tf,
		void *data,
		int priority,
		bool suspended
	) :
		threadHandle_m( NULL )

{
	// Create the thread
	threadHandle_m = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE) tf, data, suspended ? CREATE_SUSPENDED : 0, NULL );

	// Success?
	if( threadHandle_m == NULL )
		// No!
		THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Threading_CreatingThread", getOSError().c_str() );

	// Set priority
	int prio;

	switch( priority ) {
		case -1:
			prio = THREAD_PRIORITY_BELOW_NORMAL;
			break;
		case -2:
			prio = THREAD_PRIORITY_LOWEST;
			break;
		case 1:
			prio = THREAD_PRIORITY_ABOVE_NORMAL;
			break;
		case 2:
			prio = THREAD_PRIORITY_HIGHEST;
			break;
		default:
			prio = THREAD_PRIORITY_NORMAL;
	}

	SetThreadPriority( threadHandle_m, prio );


	LOG( MSG_DEBUG, THREAD_STR, "Created thread with handle " << threadHandle_m << " and priority " << prio );
}
//-------------------------------------------------------------
Thread::~Thread(
	)
{
	if( threadHandle_m != NULL ) {
		// Make sure the thread is terminated
		waitForThread();

		// Release the handle
		if( CloseHandle( threadHandle_m ) == 0 )
			// A problem occured
			THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Threading_DestroyingThread", getOSError().c_str() );

		LOG( MSG_DEBUG, THREAD_STR, "Destroyed thread with handle " << threadHandle_m );
	}
}
//======================================
// Methods
//======================================
ThreadHandle const & Thread::getHandle (
	) const
{
	return threadHandle_m;
}
//-------------------------------------------------------------
int Thread::getResult (
	) const throw (
		ExceptionInternal
	)
{
	if( !hasTerminated() )
		// Thread is still running
		return -1;
	else {
		// Get the exit code
		DWORD ec;
		if( GetExitCodeThread( threadHandle_m, &ec ) == 0 )
			// A problem occured
			THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Threading_GettingThreadExitCode", getOSError().c_str() );

		// Return the exit code
		return ec;
	}
}
//-------------------------------------------------------------
bool Thread::hasTerminated (
	) const throw (
		ExceptionInternal
	)
{
	DWORD res = WaitForSingleObject( threadHandle_m, 0 );

	if( res == WAIT_TIMEOUT )
		// Thread busy
		return false;
	else if( res == WAIT_FAILED )
		// A problem occured
		THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Threading_WaitingForThread", getOSError().c_str() )
	else
		// Thread finished
		return true;
}
//-------------------------------------------------------------
void Thread::killThread (
	) throw (
		ExceptionInternal
	)
{
	// Ignore if terminated
	if( hasTerminated() )
		return;

	LOG( MSG_INFO, THREAD_STR, "Killing thread with handle " << threadHandle_m );

	if( TerminateThread( threadHandle_m, -1 ) == 0 )
		// A problem occured
		THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Threading_KillingThread", getOSError().c_str() );
}
//-------------------------------------------------------------
bool Thread::waitForThread (
		double secs
	) const throw (
		ExceptionInternal
	)
{
	if( secs < 0 ) {
		if( WaitForSingleObject( threadHandle_m, INFINITE ) == WAIT_FAILED )
			THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Threading_WaitingForThread", getOSError().c_str() );

		return true;
	}
	else {
		DWORD res = WaitForSingleObject( threadHandle_m, (DWORD) (secs*1000) ); 
		if( res == WAIT_FAILED )
			THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Threading_WaitingForThread", getOSError().c_str() );

		return !(res == WAIT_TIMEOUT);
	}
}
//-------------------------------------------------------------
void Thread::suspendThread (
	) throw (
		ExceptionInternal
	)
{
	LOG( MSG_DEBUG, THREAD_STR, "Suspending thread with handle " << threadHandle_m );

	if( SuspendThread( threadHandle_m ) == 0xFFFFFFFF )
		// A problem occured
		THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Threading_SuspendingThread", getOSError().c_str() );
}
//-------------------------------------------------------------
void Thread::resumeThread (
	) throw (
		ExceptionInternal
	)
{
	LOG( MSG_DEBUG, THREAD_STR, "Resuming thread with handle " << threadHandle_m );

	if( ResumeThread( threadHandle_m ) == 0xFFFFFFFF )
		// A problem occured
		THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Threading_ResumingThread", getOSError().c_str() );
}




//=============================================================
// Helper functions
//=============================================================
void exitThread (
		int exitCode
	)
{
	LOG( MSG_DEBUG, THREAD_STR, "Exiting thread with handle " << getThreadHandle() << " and exit code " << exitCode );
	ExitThread( exitCode );
}
//-------------------------------------------------------------
void relinquishTimeslice (
	)
{
	Sleep( 0 );
}
//-------------------------------------------------------------
unsigned int getThreadHandle (
	)
{
	return GetCurrentThreadId();
}
//-------------------------------------------------------------
void sleepThread (
		unsigned int milliseconds
	)
{
	HTASSERT( milliseconds >= 0 );
	Sleep(milliseconds);
}




// End of namespace
}
