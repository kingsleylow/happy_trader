#include "core.hpp"
#include "shared/cpputils/src/cpputils.hpp"
#include <conio.h>
// =============================================================
// Main
//

#define SPAWNER "SPAWNER"


int main (int argc, char** argv)
{
	try {			
				
		// allow output to console
		CppUtils::Log::setErrOut();

		CppUtils::StringList simpleOptions;
		CppUtils::StringMap complexOptions;  
		CppUtils::StringList unassociated;

		CppUtils::parseArguments((const char**)argv, argc, simpleOptions, complexOptions, unassociated);

		CppUtils::String port_s = complexOptions["port"];
		int port = -9999;

		if (port_s.size() > 0) {
			port = atoi(port_s.c_str());	
		}
		else 
			THROW(CppUtils::OperationFailed, "exc_InvalidPortParameter", "ctx_SpawnerMain", port_s);


		// start thread

		LOG( MSG_INFO, SPAWNER, "Started spawner" );
		if (!CTCPSocket::InitializeSockets(TRUE))
			THROW(CppUtils::OperationFailed, "exc_ServerMain", "ctx_CannotInitSockets", CppUtils::getOSError());

		// this shall  be the main thread
		CppUtils::RootObjectManager::singleton().registerRootObject( CoreSpawner::getRootObjectName(), new CoreSpawner(port));

		CppUtils::ProcessManager::singleton().run();
		
		// ----
		cout << "Please press any key" << endl; 
		_getch();

		// ----

		
		CppUtils::ProcessManager::singleton().shutdown();
		CppUtils::RootObjectManager::singleton().unregisterRootObject( CoreSpawner::getRootObjectName());


		// deinitialize sockets
		CTCPSocket::ShutdownSockets();
	
		
	}
	catch(CppUtils::Exception& e)
	{
		LOG( MSG_WARN, SPAWNER, "Terminating spawner as exception occured: " << e.message() );
		return -1;
	}
	catch(...) 
	{
		LOG( MSG_WARN, SPAWNER, "Terminating spawner as unknown exception occured" );
		return -2;
	}

	LOG( MSG_INFO, SPAWNER, "Successfully Finished spawner" );
	return 0;
};
