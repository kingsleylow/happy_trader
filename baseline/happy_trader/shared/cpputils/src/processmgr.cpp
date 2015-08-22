
/** \file

		File implementing ProcessManager.

		
*/



#include "processmgr.hpp"

#include "log.hpp"
#include "except.hpp"
#include "rootobj.hpp"
#include "assertdef.hpp"


namespace CppUtils {



#define PROCESS_MGR "PROCESS MANAGER"



//=============================================================
// Booting function
int startIdleThread (void *ptr) {
	// Run the thread
	ProcessManager *sthread = (ProcessManager *) ptr;
	sthread->idleMonitor();

	// Done
	return 0;
}




//=============================================================
// Class ProcessManager::Bootable
//=============================================================
//======================================
// Con- & Destructor
//======================================
ProcessManager::Bootable::~Bootable (
	)
{
}





//=============================================================
// Class ProcessManager
//=============================================================
//======================================
// Con- & Destructor
//======================================
ProcessManager::ProcessManager (
	) :
		bootObjs_m(),
		phase_m( P_Create ),
		runningEvent_m(),
		processLock_m(),
		shutdownMutex_m(),
		idleCounterMutex_m(),
		lastOperation_m(),
		idlePeriod_m( -1 ),
		idleFunctor_m( NULL ),
		runningOperations_m( 0 )
{
	// Start the idle monitor thread
	//idleMonitorThread_m = new Thread( startIdleThread, this );
}
//-------------------------------------------------------------
ProcessManager::~ProcessManager (
	)
{
	// Set correct phase to let idle monitor exit
	HTASSERT( phase_m == P_Shutdown || phase_m == P_Create );
	phase_m = P_Shutdown;

	// Kill idle thread
	//LOG( MSG_DEBUG, PROCESS_MGR, "Waiting for idle thread..." );
	//idleMonitorThread_m->killThread();

	// Delete thread
	//delete idleMonitorThread_m;
	//idleMonitorThread_m = NULL;
	//LOG( MSG_DEBUG, PROCESS_MGR, "Idle thread terminated." );

	// Delete idle functor
	if( idleFunctor_m != NULL )
		delete idleFunctor_m;
}
//-------------------------------------------------------------
ProcessManager &ProcessManager::singleton (
	)
{

	static ProcessManager *mgr_s = NULL;
  static Mutex mutex_s;
	
	if( !mgr_s ) {

		LOCK_FOR_SCOPE( mutex_s );
		if( !mgr_s ) {
		
			mgr_s = new ProcessManager;
		}
	}

	return *mgr_s;

}
//======================================
// Booting management
//======================================
void ProcessManager::registerBootable(
		Bootable &obj
	)
{
	LOCK_FOR_SCOPE( *this );

	HTASSERT( phase_m == P_Create );

	// Log
	LOG( MSG_DEBUG, PROCESS_MGR, "Registering bootable of type \"" << typeid( obj ).name() << "\" @ " << &obj << "." );

	// Register
	bootObjs_m.push_back( &obj );
}
//======================================
// Idle management
//======================================
void ProcessManager::registerIdleFunction (
		IdleFunction *functor,
		double period
	)
{
	LOCK_FOR_SCOPE( *this );

	// Log
	LOG( MSG_DEBUG, PROCESS_MGR, "Registering idle functor of type \"" <<
		typeid( *functor ).name() << "\" @ " << functor << " with period of " << period << "." );

	// Discard old functor, if any
	if( idleFunctor_m != NULL )
		delete idleFunctor_m;

	// Remember args
	idleFunctor_m = functor;
	idlePeriod_m = period;

	// Start counting
	lastOperation_m = getTime();
}
//-------------------------------------------------------------
void ProcessManager::setIdlePeriod (
		double period
	)
{
	LOCK_FOR_SCOPE( *this );

	idlePeriod_m = period;
}
//======================================
// Phase management
//======================================
bool ProcessManager::run (
	)
{
	LOCK_FOR_SCOPE( *this );

	// Ignore if we run already
	if( phase_m == P_Run )
		return false;

	HTASSERT( phase_m == P_Create );


	// Enter boot phase
	//
	LOG( MSG_INFO, PROCESS_MGR, "Entering boot-phase." );
	
	phase_m = P_Boot;


	// Call back bootables; exit on exceptions
	//
	while( !bootObjs_m.empty() ) {
		// Execute
		LOG( MSG_DEBUG, PROCESS_MGR, "Calling back bootable of type \"" <<
			typeid( *bootObjs_m.front() ).name() << "\" @ " << bootObjs_m.front() << "." );

		String problem = "";

		try {
			bootObjs_m.front()->boot();
		}
		catch( Exception const &x ) {
			problem = x.type() + "\n" + x.message() + "\n" + x.arg();
		}
		catch( ... ) {
			problem = "<unknown>";
		}

		if( !problem.empty() ) {
			LOG( MSG_FATAL, PROCESS_MGR, "Exception while booting : " << problem );
			
			displayError( String("An exception occurred while booting the process : " ) + problem );
			abort();
		}

		// Remove obj from list
		bootObjs_m.pop_front();
	}


	// Go into run phase
	//
	LOG( MSG_INFO, PROCESS_MGR, "Entering run-phase." );

	phase_m = P_Run;


	// Set the "running" event
	//
	runningEvent_m.signalEvent();


	// Done
	return true;
}
//-------------------------------------------------------------
void ProcessManager::shutdown (
	)
{
	LOCK_FOR_SCOPE( *this );

	// Ignore if we shut down already
	if( phase_m == P_Shutdown )
		return;


	// Log
	LOG( MSG_INFO, PROCESS_MGR, "Entering shutdown-phase." );

	
	Phase previousPhase;

	{
		// To prevent conflicts with beginOperation()
		LOCK_FOR_SCOPE( shutdownMutex_m );

		// Set shutdown phase
		//
		previousPhase = phase_m;
		phase_m = P_Shutdown;


		// Now wait for write lock on process
		//
		processLock_m.acquireWriteLock();
	}


	// Release any waiting operations, if we skipped run-phase
	//
	if( previousPhase != P_Run )
		runningEvent_m.signalEvent();


	// Log
	//
	LOG( MSG_INFO, PROCESS_MGR, "Entered shutdown-phase." );
}
//-------------------------------------------------------------
bool ProcessManager::isRunning (
	)
{
	return ( phase_m == P_Run );
}
//-------------------------------------------------------------
bool ProcessManager::isShuttingDown(
	)
{
	return ( phase_m == P_Shutdown );
}
//-------------------------------------------------------------
bool ProcessManager::beginOperation (
		bool idleCall
	)
{   
	
				 
		// Are we in shutdown-phase?
	if( phase_m == P_Shutdown )
		// Yes; return false
		return false;
	

	// Register operation in progress
	//
	if( !idleCall ) {
		LOCK_FOR_SCOPE( idleCounterMutex_m );
		++runningOperations_m;
	}


	// Wait for "running event"
	//
	runningEvent_m.lock();


	{
		LOCK_FOR_SCOPE( shutdownMutex_m );

		// Are we now in shutdown-phase?
		if( phase_m == P_Shutdown )
			// Yes; return false
			return false;


		// Acquire read lock on process
		//
		processLock_m.acquireReadLock();
	}


	// Done
	return true;
}
//-------------------------------------------------------------
void ProcessManager::endOperation (
		bool idleCall
	)
{
	// Unregister operation in progress
	//
	if( !idleCall ) {
		LOCK_FOR_SCOPE( idleCounterMutex_m );

		--runningOperations_m;
		lastOperation_m = getTime();
	}

	// Release read lock
	//
	processLock_m.releaseReadLock();
}
//-------------------------------------------------------------
void ProcessManager::idleMonitor (
	)
{
	double timeToSleep = 10.0;

	do {
		// Sleep a little
		sleep( timeToSleep );

		{
			// Check for idle
			LOCK_FOR_SCOPE( *this );

			// Shutdown?
			if( phase_m == P_Shutdown )
				return;
	

			// Evaluate idle counters
			//
			double timeTillIdle;
			{
				// Lock the counter
				LOCK_FOR_SCOPE( idleCounterMutex_m );

				// Does it make sense to check?
				if( phase_m != P_Run || idleFunctor_m == NULL || idlePeriod_m < 0.0 || runningOperations_m > 0 )
					continue;


				// Enough time elapsed?
				//
				timeTillIdle = (lastOperation_m + idlePeriod_m) - getTime();
			}
			

			// Evaluate results
			//
			if( timeTillIdle < 0.0 ) {
				LOG( MSG_DEBUG, PROCESS_MGR, "Calling idle function." );
				idleFunctor_m->evaluate();

				// Call again no sooner than another period
				timeToSleep = idlePeriod_m;
			}
			else
				timeToSleep = timeTillIdle + 0.1;
		}
	} while( true );
}




// End of namespace
}

