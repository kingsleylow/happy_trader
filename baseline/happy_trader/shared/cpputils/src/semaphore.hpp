
/** \file

		Header file declaring Semaphore synchronization class.

		
*/



#ifndef _CPPUTILSS_SEMAPHORE_INCLUDED
#define _CPPUTILSS_SEMAPHORE_INCLUDED


#include "cpputilsdefs.hpp"
#include "mutex.hpp"


namespace CppUtils {



//=============================================================
// Class Semaphore
//=============================================================
/** Class Semaphore is an abstraction from OS-specific
		semaphore implementations. A semaphore can control a pool
		of resources: it contains a maximum count and a current
		count; the semaphore is unavailable the current count
		reaches the maximum count. Any attempt to acquire the
		semaphore will block the calling thread until the count
		has dropped below the maximum.

*/
class CPPUTILS_EXP Semaphore {

public:

	//======================================
	// Ctors & Dtors

	/** Ctor. Defines the maximum count \a max of the
			semaphore and specifies the initial count \a initial.
			Raises ExceptionInternal if the semaphore
			could not be created. */
	Semaphore (
		int max,
		int initial = 0
	);

	/** Dtor. */
	~Semaphore (
	);


	//======================================
	// Methods

	/** Acquire the semaphore, i.e. increment the counter by one.
			If the counter is already at the maximum, the call blocks
			until it drops below the maximum. */
	void acquireSemaphore (
	);

	/** Release the semaphore, i.e. decrement the counter by one.
			The counter must not drop below zero. */
	void releaseSemaphore (
	);

	/** Return the current count of the semaphore. */
	long currentCount (
	) const;

	/** Return the maximum count of the semaphore. */
	long maximumCount (
	) const;

	NativeSemaphore getNativeSemaphore() const {
		return semaphore_m;
	}

private:

	//======================================
	// Creators

	/** Private, unimplemented ctor, */
	Semaphore (
		Semaphore const &
	);


	//======================================
	// Variables

	/** The native semaphore. */
	NativeSemaphore semaphore_m;

	/** The maximum count. */
	long maxCount_m;

	/** The current count. */
	long currentCount_m;
};



// End of namespace
};


#endif // _CPPUTILSS_SEMAPHORE_INCLUDED
