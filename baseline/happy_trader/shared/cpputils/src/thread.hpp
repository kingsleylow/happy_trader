
#ifndef _CPPUTILSS_THREAD_INCLUDED
#define _CPPUTILSS_THREAD_INCLUDED


#include "cpputilsdefs.hpp"


namespace CppUtils {


//=============================================================
// Class Thread
//=============================================================
/** Class Thread is an abstraction from OS-specific thread
		implementations. It spawns a new thread and offers functionality
		to control execution	of that thread.

*/
class CPPUTILS_EXP Thread {

public:


	//======================================
	// Typedef

	typedef int (*ThreadFun) (void *);


	//======================================
	// Con- & Destructor

	/** Ctor. Creates a new thread, starting to run the thread with the
			thread entry functions \a tf. The argument \a data is passed
			to the thread entry function. If \a suspended is true, the thread is
			created as suspended thread and can be started by calling resumeThread().
			Raises ExceptionInternal if the thread could not be created. */
	Thread (
		ThreadFun tf,
		void *data,
		int priority = 0,
		bool suspended = false
	);

	/** Dtor. Waits for the thread to terminate.

			\attention This call blocks until the thread terminates.
	*/
	~Thread(
	);


	//======================================
	// Methods

	/** Returns the OS-specific thread handle. */
	ThreadHandle const & getHandle (
	) const;

	/** Returns the result code that the thread entry
			function has returned. If the thread entry function
			has not returned yet, this function returns -1.
			Raises ExceptionInternal if the thread cannot be accessed. */
	int getResult (
	) const;

	/** Returns true if the spawned thread has terminated.
			Raises ExceptionInternal if the thread cannot be accessed. */
	bool hasTerminated (
	) const;

	/** Kills the spawned thread from another thread. Raises
			ExceptionInternal if the thread cannot be accessed.

			\warning Extremely dangerous function, as all
			sorts of chaos can happen.
	*/
	void killThread (
	);

	/** Blocks until the spawned thread has finished or until the specified
			time has elapsed; block infinitly if the argument is < 0. Raises
			ExceptionInternal if the thread cannot be accessed. Returns true
			if the thread has stopped.
	*/
	bool waitForThread (
		double secs = -1
	) const;

	/** Called to suspend the spawned thread. Raises ExceptionInternal
			if the thread cannot be accessed.

			\attention For each call of this function,
			resumeThread() needs to be called.
	*/
	void suspendThread (
	);

	/** Called to resume a suspended thread. Raises ExceptionInternal
			if the thread cannot be accessed.

			\attention Must be called as many times as
			suspendThread() was called to actually resume the thread.
	*/
	void resumeThread (
	);


private:

	//======================================
	// Creators

	/** Private, unimplemented ctor, */
	Thread (
		Thread const &
	);



	//======================================
	// Variables

	/** Native handle of the thread. */
	ThreadHandle threadHandle_m;
};





//=============================================================
// Helper functions
//=============================================================
/** Terminates the thread that calls the function
		with the given exit code. */
void CPPUTILS_EXP exitThread (
	int exitCode
);

/** Relinquish the remainder of the calling thread's timeslice. */
void CPPUTILS_EXP relinquishTimeslice (
);

/** Returns the id of the calling thread as unsiged int. */
unsigned int CPPUTILS_EXP getThreadHandle (
);

/** Halt the thread for the given time. */
void CPPUTILS_EXP sleepThread (
	unsigned int milliseconds
);



// End of namespace
}


#endif // _CPPUTILSS_THREAD_INCLUDED
