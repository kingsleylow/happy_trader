
/** \file

		File implementing ServiceThread.

*/
 

#include "servicethread.hpp"

#include "except.hpp"
#include "processmgr.hpp"
#include "assertdef.hpp"



namespace CppUtils {



#define SERVICE_THREAD "SERVICE THREAD"


//=============================================================
// Booting function
int bootServiceThread (void *ptr) {
	// Wait a little
	sleepThread( 500 );

	// Run the thread
	ServiceThread *sthread = (ServiceThread *) ptr;
	sthread->run();

	// Done
	return 0;
}




//=============================================================
// Class ServiceThread
//=============================================================
const double ServiceThread::maxSleepDuration_scm = 1.0;
//-------------------------------------------------------------
const double ServiceThread::stdSleepDuration_scm = 0.1;
//-------------------------------------------------------------
String const &ServiceThread::getRootObjectName (
	)
{
	static String name_s( "service_thread" );
	return name_s;
}
//======================================
// Creators
//======================================
ServiceThread::ServiceThread (
	) :
		terminate_m( false ),
		thread_m( NULL ),
		functors_m()
{
	// Prepare booting of the thread
	thread_m = new Thread( bootServiceThread, this );
}
//-------------------------------------------------------------
ServiceThread::~ServiceThread (
	)
{
	// Wait for thread
	LOG( MSG_DEBUG, SERVICE_THREAD, "Waiting for service thread..." );
	thread_m->waitForThread();

	// Delete thread
	delete thread_m;
	thread_m = NULL;
	LOG( MSG_DEBUG, SERVICE_THREAD, "Service thread terminated." );


	// Now lock; cannot lock before while thread is still running
	LOCK_FOR_SCOPE( *this );

	// cannot delete functor here!!!
	// cause new can be called from other DLL!!!
	//for(map<double, ServiceFunction *>::iterator i_fun = functors_m.begin(); i_fun != functors_m.end(); ++i_fun)
	//	delete i_fun->second;

	// Clear the map
	functors_m.clear();

}
//======================================
// Methods
//======================================
void ServiceThread::registerServiceFunction (
		ServiceFunction* functor,
		double delay
	)
{
	
	LOCK_FOR_SCOPE( *this );


	// Compute time of execution
	double execTime = getTime() + delay;

	// Make sure the time is not taken in the map
	while( functors_m.find( execTime ) != functors_m.end() )
		execTime += 0.001;

	// Add to the functions
	functors_m[ execTime ] = functor;

	LOG( MSG_DEBUG, SERVICE_THREAD, "Registering functor of type " << \
		typeid( functor ).name() << ", uid: " << functor->getUid()  << " for execution @ " << execTime << "; now "<< \
		functors_m.size() << " functors in queue." );
}


//-------------------------------------------------------------

void ServiceThread::unregisterServiceFunction( CppUtils::Uid const& uid	)
{
	if (uid.isValid()) {
		LOCK_FOR_SCOPE( *this );

		for(auto it = functors_m.begin(); it != functors_m.end(); it++) {
			if (it->second->getUid() == uid) {
				functors_m.erase(it);
				LOG( MSG_DEBUG, SERVICE_THREAD, "Removed functor of UID: " << uid );
				break;
			}
		}
	}
}

//-------------------------------------------------------------
void ServiceThread::run (
	)
{
	LOG( MSG_DEBUG, SERVICE_THREAD, "Service thread started" );

	double now, nextExecTime, nextDelay;
	

	// Service loop
	//
	double timeToSleep = 0.0;

	do {
		// Sleep a little
		//
		sleep( timeToSleep );
		timeToSleep = 0.0;


		{
			// Exit?
			//
			ON_SHUTDOWN_IDLE( break; );


			// Find next function to execute
			//
			now = getTime();
			nextExecTime = -1;

			{
				LOCK_FOR_SCOPE( *this );

				if( !functors_m.empty() )
					// Get next time; map is sorted on key
					nextExecTime = functors_m.begin()->first;
			}

			// Figure out the time to sleep
			if( nextExecTime < 0 || (nextExecTime - now > maxSleepDuration_scm) ) {
				// No fun or too long to wait; wait std duration and try again
				timeToSleep = stdSleepDuration_scm;
				continue;
			}

			// Sleep until the function has to be executed
			if( nextExecTime > now ) {
				timeToSleep = nextExecTime - now;
				continue;
			}


			// Now get the functor & execute
			//
			{
				LOCK_FOR_SCOPE( *this );

				// Functors vanished?
				if( functors_m.empty() )
					continue;

				// Get first functor; attention this may be a different
				// one than the one that we slept for, if there has been
				// a registration in the meantime!
				//
				ServiceFunction* functor = functors_m.begin()->second;

				LOG( MSG_DEBUG_DETAIL, SERVICE_THREAD, "Calling functor of type " << typeid( functor ).name() );

				// Call the functor
				String problem = "";
				
				// no sure if we have luck in the next call
				nextDelay = 0.0;
				try {
					nextDelay = (*functor)();
				}
				catch( Exception &x ) {
					problem = "Exception occurred in service thread:\r\n\r\n";
					problem += x.message();
					problem += "\r\n\r\nAborting...\r\n";
				}
				catch(...) {
					problem = "Unknown exception occurred in service thread.\r\n\r\nAborting...\r\n";
				}

				// Problems?
				if( !problem.empty() ) {
					LOG( MSG_ERROR, SERVICE_THREAD, problem );
				}


				// Schedule next execution
				//
				if( nextDelay > 0.0 ) {
					// Make sure the time is not taken in the map
					double execTime = getTime() + nextDelay;
					while( functors_m.find( execTime ) != functors_m.end() )
						execTime += 0.001;

					functors_m[ execTime ] = functor;
					//LOG( MSG_DEBUG, SERVICE_THREAD, "Removed functor as it returned invaid time" );
				}
				else {
					LOG( MSG_DEBUG, SERVICE_THREAD, "Removed (will not re-register) functor as it returned invaid time" );
				}

				// Remove this occurrence
				functors_m.erase( functors_m.begin() );
			}
		}
	} while( true );

	LOG( MSG_DEBUG, SERVICE_THREAD, "Terminating." );
}



// End of namespace
}

