
/** \file

		Header file declaring SharedResource synchronization class.

		
*/



#ifndef _CPPUTILSS_RESOURCE_INCLUDED
#define _CPPUTILSS_RESOURCE_INCLUDED


#include "cpputilsdefs.hpp"

#include "mutex.hpp"
#include "event.hpp"
#include "thread.hpp"
#include "uid.hpp"



namespace CppUtils {



//=============================================================
// Class SharedResource
//=============================================================
/** Class SharedResource is a class that allows multiple threads
		to concurrently lock it for read access, but only one to
		lock it for write access. Locking is not fair, since the writer
		always has to wait until no more read requests are present
		(this helps to avoid deadlocks).

		Read and write locks can be acquired recursively; read and
		write locks acquired from the same party do _not_ block
		each other.

		Clients who want to acquire a write lock, should acquire
		the write lock before any read locks! Otherwise the attempt to acquire
		the write lock might result in a deadlock, which is resolved by the
		object by throwing an exception.

*/
class CPPUTILS_EXP SharedResource : private Mutex {

public:

	//======================================
	// Ctors & Dtors

	/** Ctor. Raises ExceptionInternal if the internal mutexes
			cannot be initialized. */
	SharedResource (
	);

	/** Dtor. */
	~SharedResource (
	);


	//======================================
	// Functions

	/** Lock the resource for read access; blocks if the resource
			is currently locked for write access by another thread.

			\warning Calling this function counts the locks acquired:
			a client must call releaseReadLock() as many times as it
			calls acquireReadLock().
	*/
	void acquireReadLock (
	) const;

	/** Unlock the resource for read access.

			\warning Must be called as many times as acquireReadLock()
			has been called to actually release the read lock.
	*/
	void releaseReadLock (
	) const;

	/** Lock the resource for write access; blocks if the resource
			is currently locked for read access by another thread.

			\warning Calling this function counts the locks acquired:
			a client must call releaseWriteLock() as many times as it
			calls acquireWriteLock().

			\warning This function may throw ExceptionInternal
			if the attempt to acquire the lock results in a deadlock; this
			happens when the client calling this function already has acquired
			a read lock and another thread is already waiting for the write lock.
	*/
	void acquireWriteLock (
	) const;

	/** Unlock the resource for write access. If \a force is true,
			the lock will be released regardless of the recursion depth.

			\warning Must be called as many times as acquireWriteLock()
			has been called to actually release the write lock.
	*/
	void releaseWriteLock (
		bool force = false
	) const;

	/** Return the write lock recursion count. 

			\attention Only safe to call from a thread owning write access! 
			Other threads may retrieve inconsistent results.
	*/
	/*
	unsigned int getWriteLockCount (
	) const;
	*/


private:

	//======================================
	// Creators

	/** Private, unimplemented ctor, */
	SharedResource (
		SharedResource const &
	) 
	{
	};


	//======================================
	// Variables

	/** Native event objects to monitor release of locks. */
	NativeEvent noReadEvent_m, noWriteEvent_m;

	/** Number of concurrent read locks keyed by the thread handle. */
	mutable map<unsigned int, int> readLocks_m;

	/** Number of concurrent write locks. */
	mutable int writeLocks_m;

	/** Helper mutex. */
	Mutex writeMutex_m;

	/** Mutex securing read locks map*/
	Mutex readLocksMtx_m;

	/** Handle of the writing thread. */
	mutable unsigned int writingThreadHandle_m;
};







//=============================================================
// Class ScopedResourceReadLock
//=============================================================
/** Helper class to use C++ scoping to lock and
		unlock a thread specific shared resource for
		read access.

		\author Olaf Lenzmann
*/
class ScopedResourceReadLock {

public:

	//======================================
	// Ctor & Dtor

	/** Ctor taking instance of SharedResource
			as argument, locking it for read access. 
		
			When \a doLock is true, locking is \b not performed.
	*/
	ScopedResourceReadLock (
			SharedResource const &resource,
			bool doLock = true
		) : resource_m(resource), doLock_m( doLock )
	{
		if( doLock_m )
			resource_m.acquireReadLock();
	};

	/** Unlocks argument resource. */
	~ScopedResourceReadLock (
		)
	{
		if( doLock_m )
			resource_m.releaseReadLock();
	};


private:

	//======================================
	// Ctors

	/** \attention Not implemented. */
	ScopedResourceReadLock (
	);

	/** \attention Not implemented. */
	ScopedResourceReadLock (
		ScopedResourceReadLock const &
	);


	//======================================
	// Variables

	/** The shared resource subject to locking. */
	SharedResource const &resource_m;

	/** Locking ignore flag. */
	bool doLock_m;
};






//=============================================================
// Class ScopedResourceWriteLock
//=============================================================
/** Helper class to use C++ scoping to lock and
		unlock a thread specific shared resource for 
		write access.

		\author Olaf Lenzmann
*/
class ScopedResourceWriteLock {

public:

	//======================================
	// Ctor & Dtor

	/** Ctor taking instance of SharedResource
			as argument, locking it for write access.
		
			When \a doLock is false, locking is \b not performed.
	*/
	ScopedResourceWriteLock (
		SharedResource const &resource,
		bool doLock = true
		) : resource_m(resource), doLock_m( doLock )
	{
		if( doLock_m )
			resource_m.acquireWriteLock();
	};

	/** Unlocks argument resource. */
	~ScopedResourceWriteLock (
		)
	{
		if( doLock_m )
			resource_m.releaseWriteLock();
	};



private:


	//======================================
	// Ctors

	/** \attention Not implemented. */
	ScopedResourceWriteLock (
	);

	/** \attention Not implemented. */
	ScopedResourceWriteLock (
		ScopedResourceWriteLock const &
	);


	//======================================
	// Variables

	/** The shared resource subject to locking. */
	SharedResource const &resource_m;

	/** Locking ignore flag. */
	bool doLock_m;
};





/** Helper define to lock a shared resource for read access for the local scope */
#define READ_LOCK_FOR_SCOPE(X) CppUtils::ScopedResourceReadLock scopedReadLock(X);

/** Helper define to conditionally lock a shared resource for read access for the local scope */
#define CONDITIONAL_READ_LOCK_FOR_SCOPE(X,Y) CppUtils::ScopedResourceReadLock scopedReadLock(X,Y);

/** Helper define to lock a shared resource for write access for the local scope */
#define WRITE_LOCK_FOR_SCOPE(X) CppUtils::ScopedResourceWriteLock scopedWriteLock(X);

/** Helper define to conditionally lock a shared resource for write access for the local scope */
#define CONDITIONAL_WRITE_LOCK_FOR_SCOPE(X,Y) CppUtils::ScopedResourceWriteLock scopedWriteLock(X,Y);




// End of namespace
};



#endif // _CPPUTILSS_MUTEX_INCLUDED
