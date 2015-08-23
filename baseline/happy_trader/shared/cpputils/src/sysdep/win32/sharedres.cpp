
#define NO_HELPER_DECL 1

#include "../../sharedres.hpp"

#include "../../except.hpp"
//#include "../../stringtable.hpp"
#include "../../thread.hpp" 
#include "../../assertdef.hpp"



#define SHARED_RESOURCE "SHARED RESOURCE"



namespace CppUtils {


//=============================================================
// Class SharedResource
//=============================================================
SharedResource::SharedResource (
	) :
		noReadEvent_m(),
		noWriteEvent_m(),
		readLocks_m(),
		writeLocks_m(0),
		writeMutex_m(),
		writingThreadHandle_m(0)
{
	// Create native event objects
  noReadEvent_m = ::CreateEvent(NULL, true,	true, NULL);
  noWriteEvent_m = ::CreateEvent(NULL, true, true, NULL);

	if( noWriteEvent_m == NULL || noReadEvent_m == NULL )
		THROW( ResourceFailure, "exc_OsError", "util_ctx_Threading_CreatingSharedResource", getOSError().c_str() );
}
//-------------------------------------------------------------
SharedResource::~SharedResource(
	)
{
	// Close native events
	if( ::CloseHandle( noReadEvent_m ) == NULL )
		THROW( ResourceFailure, "exc_OsError", "util_ctx_Threading_DestroyingSharedResource", getOSError().c_str() );

	if( ::CloseHandle( noWriteEvent_m ) == NULL )
		THROW( ResourceFailure, "exc_OsError", "util_ctx_Threading_DestroyingSharedResource", getOSError().c_str() );
}
//======================================
// Methods
//======================================
void SharedResource::acquireReadLock (
	) const
{
	// Get caller thread
	//
	unsigned int th = getThreadHandle();

	// ZVV
	{
		LOCK_FOR_SCOPE( readLocksMtx_m );

		// Check if the caller has the lock already
		//
		map<unsigned int, int>::iterator i_readLock = readLocks_m.find( th );
		bool readingAlready = (i_readLock != readLocks_m.end());


		// Take the short-cut if the thread is reading already
		//
		if( th == writingThreadHandle_m || readingAlready ) {
			// The same thread has already a read or write lock;
			// record read lock and done
			if( !readingAlready ) {
				// Had only write-lock -> add read lock enrty
				readLocks_m.insert( pair<unsigned int const, int>( th, 1 ) );
				
			}
			else {
				// Just inc the read lock entry
				++i_readLock->second; 
			}

			return;
		}

	} // readLocksMtx_m


	// Acquire the lock; this section locks the entire object.
	//
	// Do not disturb
	LOCK_FOR_SCOPE( *this );
	

	// Wait until writing is finished
	if( ::WaitForSingleObject( noWriteEvent_m, INFINITE ) == WAIT_FAILED )
		THROW( ResourceFailure, "exc_OsError", "util_ctx_Threading_WaitingForNoWriteOnSharedResource", getOSError().c_str() );



	// Record read lock
	//
	{
		LOCK_FOR_SCOPE(readLocksMtx_m);
		readLocks_m.insert( pair<unsigned int const, int>( th, 1 ) );
	}
	
	// Signal that we start reading
	//
	if( ::ResetEvent( noReadEvent_m ) == 0 )
		THROW( ResourceFailure, "exc_OsError", "util_ctx_Threading_ResettingNoReadOnSharedResource", getOSError().c_str() );
}
//-------------------------------------------------------------
void SharedResource::releaseReadLock (
	) const
{
	// Get caller thread
	//
	unsigned int th = getThreadHandle();

	bool issueNoReadEvent = false;

	{
		// ZVV
		LOCK_FOR_SCOPE( readLocksMtx_m );
		
		// Find entry in read lock table
		//
		map<unsigned int, int>::iterator i_readLock = readLocks_m.find( th );


		// Check lock counters
		//
		
		HTASSERT( i_readLock != readLocks_m.end() );
		

	// -----------------
	  
		if( --i_readLock->second == 0 ) {
			// No more locks from this thread
			readLocks_m.erase( i_readLock );

			// Is this the last read lock?
			if( readLocks_m.empty() ) {
				issueNoReadEvent = true;
			}
		}

	} // readLocksMtx_m

	// Signal reading ended
	if (issueNoReadEvent) {
		if( ::SetEvent( noReadEvent_m ) == 0 )
			THROW( ResourceFailure, "exc_OsError", "util_ctx_Threading_SignallingNoReadOnSharedResource", getOSError().c_str() );
	}

}
//-------------------------------------------------------------
void SharedResource::acquireWriteLock (
	) const
{
	// Get caller thread
	//
	unsigned int th = getThreadHandle();


	// Already have the lock?
	//
	if(	writingThreadHandle_m == th ) {
		// We alredy have the lock
		++writeLocks_m;
		return;
	}


	// Valid request?
	//
	{
		LOCK_FOR_SCOPE( readLocksMtx_m );

		if( readLocks_m.find( th ) != readLocks_m.end() ) 
			LOG( MSG_ERROR, SHARED_RESOURCE, "Acquiring write lock within a read lock." );
	}

 

	// Make sure that nobody else writes;
	// Lock the helper mutex beyond the scope
	// of this call.
	//
	if( !writeMutex_m.tryLock() ) {
		// Another thread has acquired the write lock already;
		// if we have read locks open, we would block the other
		// thread waiting for them to disappear -> we need to
		// throw an exception to resolve the deadlock in this case
		//

		{
			LOCK_FOR_SCOPE( readLocksMtx_m );

			// Check if we have a read-lock
			if( readLocks_m.find( th ) != readLocks_m.end() )
				// Deadlock
				THROW( ExceptionInternal, "exc_Deadlock", "util_ctx_Threading_AcquiringWriteLockOnSharedResource", "" );
		}


		// No, we do not have any read locks open, so we can
		// simply wait for the write mutex to become available
		writeMutex_m.lock();
	}


	// Do not disturb: avoid conflict with other waits
	LOCK_FOR_SCOPE( *this );

	// Signal that we go into writing state
	if( ::ResetEvent( noWriteEvent_m ) == NULL )
		THROW( ResourceFailure, "exc_OsError", "util_ctx_Threading_ResettingNoWriteOnSharedResource", getOSError().c_str() );


	// Set the writing thread handle
	//
	writingThreadHandle_m = th;


	// Check if there are read locks from threads other than this
	//

	bool waitForNoRead = false;

	{
			LOCK_FOR_SCOPE( readLocksMtx_m );
			if( !readLocks_m.empty() ) {
				// There are other read-locks we must wait
				waitForNoRead = true;
		
			}
	}

	if (waitForNoRead) {
		if( ::WaitForSingleObject( noReadEvent_m, INFINITE ) == WAIT_FAILED )
			THROW( ResourceFailure, "exc_OsError", "util_ctx_Threading_WaitingForNoReadOnSharedResource", getOSError().c_str() );
	}


	// Record lock
	//
	writeLocks_m = 1;
}
//-------------------------------------------------------------
void SharedResource::releaseWriteLock (
		bool force
	) const
{
	// Get caller thread
	//
	unsigned int th = getThreadHandle();


	// Check consistency
	//
	HTASSERT( th == writingThreadHandle_m );
	HTASSERT( writeLocks_m > 0 );


	// Count
	//
	if( !force && --writeLocks_m > 0 )
		// Still other locks open
		return;


	// This is the last lock
	writeLocks_m = 0;

	// Clear writing thread handle
	writingThreadHandle_m = 0;


	// Signal that we leave writing state
	if( ::SetEvent( noWriteEvent_m ) == NULL )
		THROW( ResourceFailure, "exc_OsError", "util_ctx_Threading_SignallingNoWriteOnSharedResource", getOSError().c_str() );


	// Let other threads write
	writeMutex_m.unlock();
}
//-------------------------------------------------------------
/*
unsigned int SharedResource::getWriteLockCount (
	) const
{
	return writeLocks_m;
}
*/




// End of namespace
};
