/** 

    Header file declaring ProcessManager.

*/



#ifndef _CPPUTILS_PROCESSMANAGER_INCLUDED
#define _CPPUTILS_PROCESSMANAGER_INCLUDED


#include "cpputilsdefs.hpp"

#include "functor.hpp"
#include "refcounted.hpp"
#include "mutex.hpp"
#include "event.hpp"
#include "sharedres.hpp"
#include "thread.hpp"

#include <list>



namespace CppUtils {





//=============================================================
// Class ProcessManager
//=============================================================
/** Class ProcessManager supports life-cycle of a multi-threaded
    process by staging the create-, boot-, run- and shut-down phases.  
		Objects implementing Bootable can be registered for call-back 
		during boot-phase.

		Clients can install an idle function that is called when the
		process manager is not notified of any operations for a given time
		while in run-phase.
		
		Threads in the process use the functions beginOperation() and 
		endOperation() to synchronize with the phases:

    in create- and boot-phase, beginOperation() blocks until run-phase
    in run-phase, beginOperation() returns true
    in shutdown-phase, beginOperation() returns false

    Any call to beginOperation() must be match by a call to endOperation().
    The transition to shutdown-phase blocks until no thread has any
    operations running.


*/
class CPPUTILS_EXP ProcessManager : private Mutex {

public:

  //======================================
  // Typedefs

  /** Base class for idle function functors. */
  typedef VoidVoidFuncBase IdleFunction;

	/** Abstract base class for classes that register instances for
			booting with the ProcessManager. Method boot() needs to be
			overloaded and will be called upon transition of the program
			from booting to running phase. */
	class CPPUTILS_EXP Bootable {

		public:

			/** No-op dtor. */
			virtual ~Bootable (
			);

			/** To be overloaded to execute boot code. */
			virtual void boot (
			) = 0;
	};

  /** Helper class encapsulating a request from a thread to perform
      an operation with the ProcessManager. */
  class Operation {
		
    private:
      /** The process manager. */
      ProcessManager &mgr_m;

      /** Indicates if the operation can be performed. */
      bool canRun_m;

      /** Indicates if this is an idle call. */
      bool idleCall_m;

      /** Private, unimplemented ctor. */
      Operation(
      );

      /** Private, unimplemented ctor. */
      Operation(
        Operation const &
      );


    public:

      /** Ctor, calling beginOperation() on the manager. */
      Operation(
        ProcessManager &pm,
        bool idleCall = false
      ) :
        mgr_m( pm ),
        idleCall_m( idleCall )
      {
        canRun_m = mgr_m.beginOperation( idleCall_m );
      }

      /** Dtor, calling endOperation() on the manager if needed. */
      ~Operation(
      )
      {
        if( canRun_m )
          mgr_m.endOperation( idleCall_m );
      }

      /** Indicates if the operation can be performed. */
      bool canRun (
      ) const
      {
        return canRun_m;
      }
  };



  //======================================
  // Con- & Destructor

  /** Ctor, creating the object in create-phase. */
  ProcessManager (
  );

  /** Dtor. */
  ~ProcessManager (
  );



  //======================================
  // Booting management

	/** Register an object implementing ProcessManager::Bootable for 
			call-back during boot-phase. */
	void registerBootable (
		Bootable &obj
	);



  //======================================
  // Idle management

  /** Change the idle shut down period. */
  void setIdlePeriod (
    double period
  );

  /** Register the given function to be called when no calles to beginOperation()
      have been made for the given period and no calls are pending. Overwrites
      and discards any previously installed idle functor. */
  void registerIdleFunction (
    IdleFunction *functor,
    double period
  );



  //======================================
  // Phase management

  /** Function to call all registered bootable objects and
      to switch the object into run-phase. While this function is running,
			the process manager is in boot-phase; before it must have been in 
			create-phase or the call is ignored and false is retured. 
	*/
  bool run (
  );

  /** Function to switch the object into shutdown-phase. */
  void shutdown (
  );

  /** Returns true if in run-phase. */
  bool isRunning (
  );

  /** Returns true if in shutdown-phase. */
  bool isShuttingDown (
  );

  /** Block during create/boot-phase, return true during run-phase, false
      during shutdown-phase. If \a idleCall is true, the process manager
			will not evaluate the operation for determining when the process
			is idle. */
  bool beginOperation (
    bool idleCall = false
  );

  /** Release the lock that beginOperation() has acquired. If \a idleCall
			is true, the process manager will not evaluate the operation for 
			determining when the process is idle. */
  void endOperation (
    bool idleCall = false
  );




  //======================================
  // Helper

  /** Helper run in a separate thread to manage idle function call.
      Do not call directly. */
  void idleMonitor (
  );

	/** Access a global static instance. */
	static ProcessManager &singleton (
	);



private:


  //======================================
  // Creators

  /** Private, unimplemented ctor. */
  ProcessManager (
    ProcessManager const &
  );


  //======================================
  // Typedefs

  /** The phases of the manager. */
  enum Phase {
		/** Creating. */
		P_Create,
		/** Booting: boot functions are called. */
    P_Boot,
		/** Running. */
    P_Run,
		/** Shutting down. */
    P_Shutdown
  };



  //======================================
  // Variables

  /** List holding the bootable objects to be called-back during boot phase. */
  list<Bootable *> bootObjs_m;

  /** The current phase. */
  Phase phase_m;

	
  /** Event used to signal entering run-phase. */
  Event runningEvent_m;

  /** Mutex to coordinate shutting down. */
  Mutex shutdownMutex_m;

	/** Mutex to coordinate idle counter adjustment. */
	Mutex idleCounterMutex_m;
	
  /** Shared resource used to control shutting down; the shut-down
      requesting thread will acquire a write lock; beginOperation()
      always acquires a read lock. */
  SharedResource processLock_m;



  /** Timestamp of last operation, or negative if operation in progress. */
  double lastOperation_m;

  /** Time after which the idle function is to be called in run-phase. */
  double idlePeriod_m;

  /** The number of running operations. */
  int runningOperations_m;

  /** Idle function. */
  IdleFunction *idleFunctor_m;

  /** The idle thread. */
  Thread *idleMonitorThread_m;
};






/** Macro to init an operation with the ProcessManager; if the process
    manager decides that no operation can be performed now, because the
    run-phase is not active. */
#define ON_SHUTDOWN( OP ) \
  CppUtils::ProcessManager::Operation _oper( CppUtils::ProcessManager::singleton() ); \
  if( !_oper.canRun() ) { \
    OP \
  }

/** Macro to init 2 operations with the ProcessManager;  */
#define ON_SHUTDOWN_2( ONSHUTDOWN_OPER, OTHER_OPER ) \
  CppUtils::ProcessManager::Operation _oper( CppUtils::ProcessManager::singleton() ); \
  if( !_oper.canRun() ) { \
    ONSHUTDOWN_OPER \
	} else {	\
		OTHER_OPER	\
	}


/** Macro to init an operation with the ProcessManager in idle mode. */
#define ON_SHUTDOWN_IDLE( OP ) \
  CppUtils::ProcessManager::Operation _oper( CppUtils::ProcessManager::singleton(), true ); \
  if( !_oper.canRun() ) { \
    OP \
  }



// End of namespace
}


#endif // _CPPUTILS_PROCESSMANAGER_INCLUDED
