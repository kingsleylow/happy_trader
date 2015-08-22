
/** \file

		Header file declaring Mutex class.

*/



#ifndef _CPPUTILSS_MUTEX_INCLUDED
#define _CPPUTILSS_MUTEX_INCLUDED


#include "cpputilsdefs.hpp"



/** Helper define to lock a mutex for the local scope. */
#define LOCK_FOR_SCOPE(X) CppUtils::ScopedMutexLock scopedLock(X);


namespace CppUtils {


//=============================================================
// Class Mutex
//=============================================================
/** Class Mutex is an abstraction from OS-specific
		\b recursive mutex implementations.

		\warning The Windows implementation uses a CRITICAL_SECTION instead
		of a mutex, as this is vastly more efficient for intra-process
		synchronization. However, this mutex can therefore not be used
		for	inter-process synchronization.

*/
class CPPUTILS_EXP Mutex {

public:

	//======================================
	// Creators

	/** Ctor. Does not lock the mutex. Raises ResourceFailure
			if the OS-mutex could not be created. */
	Mutex (
	);

	Mutex (
		int const tag
	);

	/** Dtor. */
	~Mutex(
	);


	//======================================
	// Methods

	/** Lock the mutex. Blocks until the mutex is locked.
			If the calling thread already owns the mutex,
			this call increases its internal lock count,
			i.e. for each lock() and successful tryLock()
			there must be an unlock() call. 
			
			Raises ResourceFailure if the OS-mutex could not be locked. 
	*/
	void lock (
	) const;

	/** Try to lock the mutex, but do not block. Immediately
			returns false if the mutex is locked by another thread.
			If the calling thread already owns the mutex,
			this call increases its internal lock count,
			i.e. for each lock() and successful tryLock() there
			must be an unlock() call. 
			
			Raises ResourceFailure if the OS-mutex could not be locked. 
	*/
	bool tryLock (
	) const;

	/** Unlock the mutex. Raises ExceptionInternal
			if the OS-mutex could not be unlocked.

			\attention unlock() must be called as for each lock()
			and each successful tryLock() call.
	*/
	void unlock (
	) const;

	/** Check if the mutex is locked. */
	bool isLocked (
	) const;

	/** Return the lock recursion count. 
		
			\attention Only safe to call from a thread owning the mutex! Other
			threads may retrieve inconsistent results.
	*/
	unsigned int getLockCount (
	) const;


private:


	//======================================
	// Creators

	/** Private, unimplemented ctor, */
	Mutex (
		Mutex const &
	);


	//======================================
	// Variables

	/** The native mutex. */
	mutable NativeMutex mutex_m;

	/** Counter. */
	mutable long count_m;

	
};




//=============================================================
// Class ScopedMutexLock
//=============================================================
/** Helper class to use C++ scoping to lock and unlock a mutex:
		an instance of this class takes a mutex as argument in its
		ctor and locks it; in the dtor it is unlocked.

		\author Olaf Lenzmann
*/
class ScopedMutexLock {

public:

	//======================================
	// Creators

	/** Ctor locking argument mutex \a mutex. */
	ScopedMutexLock (
		Mutex const &mutex
	) :
		mutex_m( mutex )
	{
		mutex_m.lock();
	};

	
	/** Unlocks encapsulated mutex. */
	~ScopedMutexLock (
	)
	{
		mutex_m.unlock();
	};


private:


	//======================================
	// Creators

	/** \attention Not implemented. */
	ScopedMutexLock (
	);

	/** \attention Not implemented. */
	ScopedMutexLock (
		ScopedMutexLock const &
	);


	//======================================
	// Variables

	/** The encapsulated mutex. */
	Mutex const &mutex_m;
};




// End of namespace
};


#endif // _CPPUTILSS_MUTEX_INCLUDED
