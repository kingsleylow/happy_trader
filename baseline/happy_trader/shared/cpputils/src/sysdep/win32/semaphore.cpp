


#include "../../semaphore.hpp"

#include "../../except.hpp"
//#include "../../stringtable.hpp"



namespace CppUtils {


//=============================================================
// Class Semaphore
//=============================================================
Semaphore::Semaphore (
		int max,
		int initial
	) :
		semaphore_m( NULL ),
		maxCount_m( max ),
		currentCount_m( initial )
{
	// Create the semaphore
	semaphore_m = CreateSemaphore( NULL, max - initial, max, NULL );

	// Success?
	if( semaphore_m == NULL )
		// No!
		THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Threading_CreatingSemaphore", getOSError().c_str() );
}
//-------------------------------------------------------------
Semaphore::~Semaphore (
	)
{
	if( semaphore_m != NULL ) {
		// Check if the semaphore is still locked
		assert( currentCount_m == 0 );

		// Close the semaphore
		if( CloseHandle( semaphore_m ) == NULL )
			THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Threading_DestroyingSemaphore", getOSError().c_str() );
	}
}
//======================================
// Methods
//======================================
void Semaphore::acquireSemaphore (
	)
{
	if( WaitForSingleObject( semaphore_m, INFINITE ) == WAIT_FAILED )
		THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Threading_WaitingForSemaphore", getOSError().c_str() );

	// Increment current counter
	InterlockedIncrement( &currentCount_m );
}
//-------------------------------------------------------------
void Semaphore::releaseSemaphore (
	)
{
	if( ReleaseSemaphore( semaphore_m, 1, NULL ) == NULL )
		THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Threading_ReleasingSemaphore", getOSError().c_str() );

	// Decrement current counter
	InterlockedDecrement( &currentCount_m );
}
//-------------------------------------------------------------
long Semaphore::currentCount (
	) const
{
	return currentCount_m;
}
//-------------------------------------------------------------
long Semaphore::maximumCount (
	) const
{
	return maxCount_m;
}



// End of namespace
};
