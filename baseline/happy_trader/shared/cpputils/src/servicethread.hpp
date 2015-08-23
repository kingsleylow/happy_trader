
//
/** \file

		Header file declaring ServiceThread class.

	
*/



#ifndef _CPPUTILS_SERVICETHREAD_INCLUDED
#define _CPPUTILS_SERVICETHREAD_INCLUDED


#include "cpputilsdefs.hpp"

#include "functor.hpp"
#include "rootobj.hpp"
#include "mutex.hpp"
#include "thread.hpp"
#include <functional>



namespace CppUtils {


//=============================================================
// Class ServiceThread
//=============================================================
/** Class ServiceThread is a support class that enables clients
		to register functions to be periodically executed in a
		separate thread, where the function may define when it desires
		to be called again. However, the class does not manage priorities
		nor does it guarantee that a function will be called at the desired
		time; it may as well be called later.

		One instance of this class is registered with the root object manager.

	
*/
class CPPUTILS_EXP ServiceThread : public RefCounted, private Mutex {

public:

	//======================================
	// Typedefs

	/** Base class for service function functors;
			clients must derive from this class and register
			an instance with the service thread for execution. */
	typedef VoidFuncBase<double> ServiceFunction;


	//typedef std::function< double() > ServiceFunction;

	//typedef double (*ServiceFunction) ();

	//======================================
	// Con- & Destructor

	/** Ctor; generating the service thread. */
	ServiceThread (
	);

	/** Dtor; killing the service thread. */
	~ServiceThread (
	);



	//======================================
	// Methods

	/** Registers a functor for execution in the service thread;
			will be executed not before the given delay has expired.
			Registered functors will be owned and destroyed by the
			service thread.	*/
	void registerServiceFunction (
		ServiceFunction* functor,
		double delay
	);
	

	void unregisterServiceFunction ( CppUtils::Uid const& uid	);




	//======================================
	// Helper

	/** Run the service thread. Not to be called externally. */
	void run (
	);

	/** The name under which an instance of StateFactoryManager is
			available with the root object manager. \c service_thread */
	static String const &getRootObjectName (
	);



private:


	//======================================
	// Creators

	/** Private, unimplemented ctor. */
	ServiceThread (
		ServiceThread const &
	);


	//======================================
	// Variables

	/** Indicated if to terminate the server thread. */
	bool terminate_m;

	/** Thread object. */
	Thread *thread_m;

	/** Map holding the service functions to be executed, keyed
			by the time of execution as Unix time. */
	map<double, ServiceFunction* > functors_m;

	/** The maximum time to sleep: 1000 milliseconds. */
	static const double maxSleepDuration_scm;

	/** The standard time to sleep: 100 milliseconds. */
	static const double stdSleepDuration_scm;
};





// End of namespace
}


#endif // _CPPUTILS_SERVICETHREAD_INCLUDED
