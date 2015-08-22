


/* Versions of the executable:
*/
#define VERSION_NUM "3.0.4"


// includes

#include "platform/server/backend/src/hlpstruct/hlpstruct.hpp"
#include "platform/server/backend/src/inqueue/inqueue.hpp"
#include "shared/cpputils/src/cpputils.hpp"
#include "platform/server/backend/src/math/math.hpp"




#include <deque>
using namespace std;
 


// ---------------------------------------




void startup(short const port, short const servicePort,  CppUtils::String const& sessionName)
{

	
	


	Inqueue::InQueueController::setSessionName(sessionName);
	
	// need to explecetely have this initialized
	CppUtils::ProcessManager& pmManange = CppUtils::ProcessManager::singleton();
	
	

	//
	if (!pmManange.isRunning()) {
		LOG( MSG_INFO, "SERVER", "Session name \"" << sessionName << "\"" );
		LOG( MSG_INFO, "SERVER", "OS \"" << CppUtils::getOsInfo() << "\"" );
	}
	
	// init sockets
	if (!CTCPSocket::InitializeSockets(TRUE))
		THROW(CppUtils::OperationFailed, "exc_ServerMain", "ctx_CannotInitSockets", CppUtils::getOSError());

	// start service thread
	CppUtils::BootManager::boot( true );
 
	// set up no sleeping
	CppUtils::setThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED);

	// srv socket
	CppUtils::RootObjectManager::singleton().registerRootObject( Inqueue::ServerSocketController::getRootObjectName(), new Inqueue::ServerSocketController(port, servicePort));
					
	// engine object
	CppUtils::RootObjectManager::singleton().registerRootObject( Inqueue::Engine::getRootObjectName(), new Inqueue::Engine() );

	// history context object
	CppUtils::RootObjectManager::singleton().registerRootObject( Inqueue::HistoryContext::getRootObjectName(), new Inqueue::HistoryContext("rt"));
	
	// register as root
	// main server class
	CppUtils::RootObjectManager::singleton().registerRootObject(Inqueue::InQueueController::getRootObjectName(), new Inqueue::InQueueController(CppUtils::String("AIS/") + "default_domain" + "/" + "default_process" + "/" + CppUtils::String("/Settings/")
));
	

	// quote processor
	CppUtils::RootObjectManager::singleton().registerRootObject( Inqueue::QuoteProcessor::getRootObjectName(), new Inqueue::QuoteProcessor() );

	// out processor
	// highest log level
	CppUtils::RootObjectManager::singleton().registerRootObject( Inqueue::OutProcessor::getRootObjectName(), new Inqueue::OutProcessor() );
  
	// this shall  be the main thread
	CppUtils::ProcessManager::singleton().run();

}


void shutdown() {
	
	// Signal to shut down
  CppUtils::ProcessManager::singleton().shutdown();

  // Get rid of root of some objects before 

	// delete sockets
	CppUtils::getRootObject<Inqueue::ServerSocketController>()->uninitializeClientSockets();

	CppUtils::RootObjectManager::singleton().unregisterRootObject( Inqueue::OutProcessor::getRootObjectName());

	CppUtils::RootObjectManager::singleton().unregisterRootObject( Inqueue::QuoteProcessor::getRootObjectName());

	CppUtils::RootObjectManager::singleton().unregisterRootObject( Inqueue::ServerSocketController::getRootObjectName());

	// Get rid of other root objects
  CppUtils::RootObjectManager::singleton().cleanup();

	// deinitialize sockets
	CTCPSocket::ShutdownSockets();

		
}

void runPhase()
{
	

	

	try {
	
		Inqueue::InQueueController* ctr = CppUtils::getRootObject<Inqueue::InQueueController>();

		ctr->run();
		// perform run phase
	} 
	catch(CppUtils::Exception& e) {
		LOG( MSG_WARN, "SERVER", "Exception on run phase: " << e.message());
	}
	#ifdef HANDLE_NATIVE_EXCEPTIONS
		catch(Testframe::Exception& e) {
			LOG(MSG_WARN, "SERVER", "Fatal MSVC exception in run phase: " << e.message());
		}
	#endif
	catch(...) {
		LOG( MSG_WARN, "SERVER", "Unknown exception on run phase");
	}
}

// -----------------------------------



// =============================================================
// Main
//
int main (int argc, char** argv)
{
	 
// handle run-time exceptions
#ifdef HANDLE_NATIVE_EXCEPTIONS
			Testframe::init();
#endif
	
	try {		
	
		//-------

		CppUtils::Event event2;
		event2.resetEvent();



		CppUtils::StringList simpleOptions;
		CppUtils::StringMap complexOptions;  
		CppUtils::StringList unassociated;			   

		CppUtils::parseArguments((const char**)argv, argc, simpleOptions, complexOptions, unassociated);
		CppUtils::String port_s = complexOptions["port"];
		CppUtils::String serviceport_s = complexOptions["service_port"];
		

		CppUtils::String cout_s = complexOptions["errout"];
		CppUtils::String session_s = complexOptions["sname"];
		
				 
		CppUtils::tolower(cout_s);
		

		short port = -9999;
		short sport = -9999;
		bool cout_flag = false;

		
		// switch on console output
		  
		if (cout_s == "true") {
			CppUtils::Log::setErrOut();
		}


		// beggin logging
		// Dump version info
		int log_level = CppUtils::Log::getLogLevel();
		CppUtils::String logLevelStr;
		CppUtils::getEnvironmentVar( LOG_LEVEL_ENVIROMENT_ENTRY, logLevelStr );

		LOG( MSG_INFO, "SERVER", "Version " << VERSION_NUM << " (" << BUILD_TARGET_STR << " build, created on: " << __DATE__ << ", MSVC version: " << _MSC_VER  << ")." << " Started at " << CppUtils::getAscTime(-1));
		LOG( MSG_INFO, "SERVER", "Log level enviroment entry \"" << LOG_LEVEL_ENVIROMENT_ENTRY << "\" set to \"" << logLevelStr << "\"" << 
			" log level was set to \"" << log_level << "\" log file is: \"" << CppUtils::Log::getLogFileName() << "\"");
		

		// command port
		if (port_s.size() > 0) {
			port = (short)atol(port_s.c_str());	
		}
		else 
			THROW(CppUtils::OperationFailed, "exc_ServerMain", "ctx_InvalidPortParameter", port_s);

		
		if (serviceport_s.size() > 0) {
			sport = (short)atol(serviceport_s.c_str());	
		}
		else 
			THROW(CppUtils::OperationFailed, "exc_ServerMain", "ctx_InvalidServicePortParameter", serviceport_s);

		if (port <=0)
			THROW(CppUtils::OperationFailed, "exc_ServerMain", "ctx_InvalidPortParameter", (long)port);

		if (sport <=0)
			THROW(CppUtils::OperationFailed, "exc_ServerMain", "ctx_InvalidServicePortParameter", (long)sport);
		
	

		if (session_s.size() <=0)
			THROW(CppUtils::OperationFailed, "exc_ServerMain", "ctx_InvalidSessionNameParameter", "");

	
	
		// statrtup
		startup(port, sport, session_s);

		runPhase();
	
		shutdown();
	}
	catch(CppUtils::Exception& e) {
		LOG( MSG_WARN, "SERVER", "Exception on startup/shutdown phase: " << e.message() << " - aborting");
		
		// Now discard debug output
		//CppUtils::Log::discardDebugOutput();

		return -1;
	}
#ifdef HANDLE_NATIVE_EXCEPTIONS
	catch(Testframe::Exception& e) {
			LOG(MSG_WARN, "SERVER", "Fatal MSVC exception startup/shutdown phase: " << e.message());
	}
#endif
	catch(...) {
		LOG( MSG_WARN, "SERVER", "Unknown exception on startup/shutdown phase - aborting");

		// Now discard debug output
		//CppUtils::Log::discardDebugOutput();

		return -2;
	}


	LOG( MSG_INFO, "SERVER", "Server normally comlpeted its execution. Many thanks to Victor Zoubok for programming this." );
	//CppUtils::Log::discardDebugOutput();
  // Done


  return EXIT_SUCCESS;
}
