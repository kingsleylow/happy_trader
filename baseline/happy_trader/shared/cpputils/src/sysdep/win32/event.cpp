
/** \file

		File implementing Event.

	
*/



#include "../../event.hpp"

#include "../../except.hpp"
//#include "../../stringtable.hpp"



namespace CppUtils {


//=============================================================
// Class Event
//=============================================================
Event::Event (
	) :
		event_m( NULL ),
		counter_m( 0 ),
		mutex_m()
{
	event_m = ::CreateEvent(NULL, true, false, NULL);

	if (event_m == NULL)
		THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Threading_CreatingEvent", getOSError().c_str() );
}
//-------------------------------------------------------------
Event::~Event (
	)
{
	// Close the event
	if( ::CloseHandle( event_m ) == NULL )
		// A problem occured
		THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Threading_DestroyingEvent", getOSError().c_str() );
}
//======================================
// Methods
//======================================
bool Event::lock (
		int msecs
	)
{
	if(counter_m <= 0) {
		if (msecs < 0)
			msecs = INFINITE;

		DWORD res = ::WaitForSingleObject(event_m, msecs);
		if(res == WAIT_FAILED)
			THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Threading_WaitingForEvent", getOSError().c_str() )
		else 
			return (res == WAIT_OBJECT_0);
	}

	return true;
}
//-------------------------------------------------------------
void Event::signalEvent (
	)
{
	LOCK_FOR_SCOPE( mutex_m );

	// Only set event if counter crossed zero
	if( ++counter_m > 0 )
		if( ::SetEvent(event_m) == 0 )
			THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Threading_SignallingEvent", getOSError().c_str() )
}
//-------------------------------------------------------------
void Event::clearEvent (
	)
{
	LOCK_FOR_SCOPE( mutex_m );

	// Only reset event if counter drops to zero
	if( --counter_m == 0 )
	  if( ::ResetEvent(event_m) == 0 )
			THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Threading_ResettingEvent", getOSError().c_str() )
}

//-------------------------------------------------------------
void Event::resetEvent(
	)
{
	if (lock(0))
		clearEvent();
}
	
//-------------------------------------------------------------

void Event::initialize (
	)
{
	LOCK_FOR_SCOPE( mutex_m );
	counter_m = 0;

	if( ::ResetEvent(event_m) == 0 )
		THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Threading_ResettingEvent", getOSError().c_str() )
}

bool Event::lockForAllEvents(NativeEvent const* events, int const nCount, int msecs )
{

	if (msecs < 0)
		msecs = INFINITE;

	DWORD res = ::WaitForMultipleObjects(nCount, events, TRUE,  msecs);

	if(res == WAIT_FAILED)
		THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Threading_WaitingForAllEvents", getOSError().c_str() );

	return (res == WAIT_OBJECT_0);

}

int Event::lockForSomeEvents(NativeEvent const* events, int const nCount, int msecs )
{
	if (msecs < 0)
		msecs = INFINITE;

	DWORD res = ::WaitForMultipleObjects(nCount, events, FALSE,  msecs);

	if(res == WAIT_FAILED)
		THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Threading_WaitingForSomeEvents", getOSError().c_str() );

	if (  (res >= WAIT_OBJECT_0) && (res <= (WAIT_OBJECT_0+nCount-1))  )
		return (res - WAIT_OBJECT_0);

	return -1;
	 

}

// End of namespace
}
