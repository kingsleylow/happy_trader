
/** \file
		Header file declaring class implementing synchronization events.
*/


#ifndef _CPPUTILSS_EVENT_INCLUDED
#define _CPPUTILSS_EVENT_INCLUDED


#include "cpputilsdefs.hpp"
#include "mutex.hpp"


namespace CppUtils {


//=============================================================
// Class event
//=============================================================
/** Class event is an abstraction from OS-specific events
		implementations used to block a thread to wait for an event.

		The event object can be in \b signalled or \b non-signalled state.
		This class track multiple calls by an internal counter.

*/
class CPPUTILS_EXP Event {

public:

	//======================================
	// Creators

	/** Ctor. Throws ResourceFailure if the OS cannot create the event.
			The event is created in non-signalled state. */
	Event (
	);

	/** Dtor. */
	~Event (
	);


	//======================================
	// Methods

	/** Block the current thread if the event variable is not signaled.
			\a msecs specifies time of event waiting, negative msecs value 
			means infinite. */
	bool lock (
		int msecs = -1
	);

	/** Increments the internal counter and signals the event if the counter
			becomes > 0. */		
	void signalEvent (
	);

	/** Decrements the internal counter and clears the event if the counter
			becomes == 0. */		
	void clearEvent (
	);

	/** Resets internal counter and set event to non-signalled state */
	void initialize (
	);

	// if in signalled state 
	void resetEvent(
	);
	

	/** wait until all the events will be in signalled state */
	static bool lockForAllEvents(NativeEvent const* events, int const nCount, int msecs = -1 );

	/** wait until one of the events will be in signalled state, returns idx from event array in this case */
	static int lockForSomeEvents(NativeEvent const* events, int const nCount, int msecs = -1 );

	NativeEvent getNativeEvent() const {
		return event_m;
	}

private:

	//======================================
	// Creators

	/** Private, unimplemented ctor, */
	Event (
		Event const &
	);


	//======================================
	// Variables

	/** The native event. */
	NativeEvent event_m;

	/** Counter. */
	long counter_m;

	/** Helper mutex. */
	Mutex mutex_m;
};




// End of namespace
};


#endif // _CPPUTILSS_EVENT_INCLUDED
