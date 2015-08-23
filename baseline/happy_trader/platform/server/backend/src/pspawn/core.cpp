#include "core.hpp"

#define CORESPAWNER "CORESPAWNER"
#define WAIT_FINISH_TOUT 20

int bootThread (void *ptr)
	{
		// handle run-time exceptions
#ifdef HANDLE_NATIVE_EXCEPTIONS
			Testframe::init();
#endif

		CoreSpawner* pThis = (CoreSpawner*)ptr; 

		try {
			pThis->threadRun();
		}
		catch(CppUtils::Exception& e) {
			LOG(MSG_FATAL, CORESPAWNER, "Fatal exception: " << e.message());
		}
#ifdef HANDLE_NATIVE_EXCEPTIONS
		catch(Testframe::Exception& e) {
			LOG(MSG_FATAL, CORESPAWNER, "Fatal MSVC exception: " << e.message());
		}
#endif
		catch(...) {
			LOG(MSG_FATAL, CORESPAWNER, "Unknown fatal exception");
		}

		return 42;

	}

// -----------------------

CppUtils::String const &CoreSpawner::getRootObjectName()
{
	static CppUtils::String name_s( "core_spawner" );
	return name_s;
}

CoreSpawner::CoreSpawner(int const port):
port_m(port)
{
	thread_m = new CppUtils::Thread(bootThread, this);

	// Register with the process manager
	CppUtils::ProcessManager::singleton().registerBootable( *this );

	LOG(MSG_INFO, CORESPAWNER, "Spwaner starting, listening on the port: " << port_m );
}
	
CoreSpawner::~CoreSpawner()
{
				
		toShutdown_m.signalEvent();

		// normally happens on already closed sockets
				
		// send message
		
		CTCPSocket clientSocket;
		if (!clientSocket.Create()) {
			THROW(CppUtils::OperationFailed, "exc_CannotCreateSocket", "ctx_SpawnerDeinitialize", CTCPSocket::ErrorCodeToString(clientSocket.GetLastError()));
		}

		if (!clientSocket.Connect("127.0.0.1",port_m) ) {
			THROW(CppUtils::OperationFailed, "exc_CannotConnectToService", "ctx_SpawnerDeinitialize", CTCPSocket::ErrorCodeToString(clientSocket.GetLastError()));
		}
		
		
		// need tow wait until it starts
		if (!started_m.lock(5000)) {
			LOG( MSG_WARN, CORESPAWNER, "Will not completely deinitialize spawner thread as it may be dead-locked because initialization was not finished" );
			return;
		}

		thread_m->waitForThread();

		// Delete thread
		delete thread_m;
		thread_m = NULL;

		// clear server socket
		try {
			ssrvsock_m.Close();
		}
		catch(...) {
		}

		

		LOG( MSG_DEBUG, CORESPAWNER, "Spawner thread terminated. Spawner manager destroyed." );
}

void CoreSpawner::threadRun()
{
		// wait until startup phase finishes
		{
			ON_SHUTDOWN(1;); 
		}
 
		//CppUtils::String hostname = "127.0.0.1";
	
		// establish socket
		if (!ssrvsock_m.Create())
			THROW(CppUtils::OperationFailed, "exc_CannotCreateServerSocket", "ctx_CoreSpawnerThread", CTCPSocket::ErrorCodeToString(ssrvsock_m.GetLastError()));
			 
		// bind
		if (!ssrvsock_m.Bind("", port_m ))
			THROW(CppUtils::OperationFailed, "exc_CannotBindServerSocket", "ctx_CoreSpawnerThread", " Attempt to bind to port: "+ CppUtils::long2String(port_m) + " "+ CTCPSocket::ErrorCodeToString(ssrvsock_m.GetLastError()));

		
		// signal we started
		started_m.signalEvent();
		

		CppUtils::MemoryChunk in_buf, out_buf;
		HlpStruct::XmlParameter command, result;

		
		LOG( MSG_DEBUG, CORESPAWNER, "Started actual listening on port: " << port_m);
		if (!ssrvsock_m.Listen() ) 
			THROW(CppUtils::OperationFailed, "exc_CannotListenSocket", "ctx_CoreSpawnerThread", CTCPSocket::ErrorCodeToString(ssrvsock_m.GetLastError()));					
			
		
		do {

				// if shutdown
				if (toShutdown_m.lock(0))
						break;

				try {
				
					CTCPSocket* clientsocket = new CTCPSocket();
					if (ssrvsock_m.Accept(clientsocket)) {

						// if shutdown
						if (toShutdown_m.lock(0))
							break;
						
						// here we need to know who issued this to initialize serviceHost_m
					
						LOG( MSG_INFO, CORESPAWNER, "Connection accepted, connection was made from: " << 
							CSocketBase::LongToStdString(clientsocket->GetPeerAddress()) );
						
						in_buf.length(0);  
					
						if (readFromSocket<CTCPSocket>(*clientsocket, in_buf) < 0)
								THROW(CppUtils::OperationFailed, "ctx_CannotReadSocket", "ctx_CoreSpawnerThread", CTCPSocket::ErrorCodeToString(clientsocket->GetLastError()));					

						paramsProc_m.deserializeXmlParameter(command, in_buf);
						setupResult(result, 0, "Ok");

						try {
							if (command.getName() == "start_executable") {
								startExecutable(command, result);
							}
							else if (command.getName() == "destroy_process") {
								destroyProcess(command, result);
							}
							else if (command.getName() == "wait_for_finish") {
								waitForProcessFinish(command, result);
							}
							else
								THROW(CppUtils::OperationFailed, "ctx_InvalidCommand", "ctx_CoreSpawnerThread", command.getName());	
						}
						catch(CppUtils::Exception& e)
						{
							LOG( MSG_WARN, CORESPAWNER, "Exception when executing command: " << e.message() );
							setupResult(result, -1, "Exception: " + e.message() + "-" + e.context() +"-"+ e.arg() );
						}
						catch(...)
						{
							LOG( MSG_WARN, CORESPAWNER, "Unknown exception when executing command" );
							setupResult(result, -2, "Unknown exception" );
						}
						
						// out result
						paramsProc_m.serializeXmlParameter(result, out_buf);
						if (writeToSocket<CTCPSocket>(*clientsocket, out_buf) < 0)
							THROW(CppUtils::OperationFailed, "exc_CannotWriteSocket", "ctx_CoreSpawnerThread", CTCPSocket::ErrorCodeToString(clientsocket->GetLastError()));					
						
					}

					clientsocket->Shutdown(CSocket::ssBoth);
					clientsocket->Close();
					delete clientsocket;
					clientsocket = NULL;
				
				}

				catch(CppUtils::Exception& e) {
					LOG( MSG_WARN, CORESPAWNER, "Exception in server socket listener queue: " << e.message());
					CppUtils::sleep(0.5);
				}
		#ifdef HANDLE_NATIVE_EXCEPTIONS
				catch(Testframe::Exception& e) {
					LOG(MSG_FATAL, CORESPAWNER, "Fatal MSVC exception in server socket listener queue: " << e.message());
				}		
		#endif
				catch(...) {
					LOG( MSG_WARN, CORESPAWNER, "Unknown exception in server socket listener queue");
					CppUtils::sleep(0.5);
				}

			// Exit?
			//
			//ON_SHUTDOWN_IDLE( break; );
	
		} while(true);

		// clean up all processes
		cleanup();

		LOG( MSG_INFO, CORESPAWNER, "Spawner listener thread finished." );
}

void CoreSpawner::boot()
{
}

/**
* Helpers
*/
void CoreSpawner::startExecutable(HlpStruct::XmlParameter const& command, HlpStruct::XmlParameter& result)
{
	CppUtils::String const& executable = command.getParameter("executable").getAsString();
	CppUtils::String const& serverId = command.getParameter("server_id").getAsString();
	int const service_port = (int)command.getParameter("service_port").getAsInt();
	int const server_port = (int)command.getParameter("server_port").getAsInt();

	LOG( MSG_INFO, CORESPAWNER, "Starting server: " << serverId);


	CppUtils::StringMap const& newEntries = command.getParameter("new_entries").getAsStringMap();

	map<CppUtils::String, CppUtils::Process*>::const_iterator it = processes_m.find(serverId);
	if (it != processes_m.end())
		THROW(CppUtils::OperationFailed, "exc_ServerAlreadyStarted", "ctx_StartExecutable", serverId);

	CppUtils::String cmdline = "-port " + CppUtils::long2String(server_port) + 
			" -service_port " +  CppUtils::long2String(server_port) +
			" -errout false" + 
			" -sname " + "\""+ serverId + "\"";

	// set up env
	for(CppUtils::StringMap::const_iterator it = newEntries.begin(); it != newEntries.end() ; it++) {
		CppUtils::setEnvironmentVar(it->first, it->second );
	}

	
	CppUtils::Process* processP = new CppUtils::Process(executable, cmdline, CppUtils::expandDirName(executable), false, true);
	processes_m[serverId] = processP;
	
}

void CoreSpawner::destroyProcess(HlpStruct::XmlParameter const& command, HlpStruct::XmlParameter& result)
{
	CppUtils::String const& serverId = command.getParameter("server_id").getAsString();
	LOG( MSG_INFO, CORESPAWNER, "Destroying server: " << serverId);


	map<CppUtils::String, CppUtils::Process*>::const_iterator it = processes_m.find(serverId);
	if (it == processes_m.end())
		THROW(CppUtils::OperationFailed, "exc_ServerNotregistered", "ctx_waitForProcessFinish", serverId);

	it->second->killProcess();
	delete it->second;

	processes_m.erase(serverId);
}

void CoreSpawner::waitForProcessFinish(HlpStruct::XmlParameter const& command, HlpStruct::XmlParameter& result)
{
	CppUtils::String const& serverId = command.getParameter("server_id").getAsString();
	LOG( MSG_INFO, CORESPAWNER, "Shutdown server: " << serverId);

	map<CppUtils::String, CppUtils::Process*>::const_iterator it = processes_m.find(serverId);
	if (it == processes_m.end())
		THROW(CppUtils::OperationFailed, "exc_ServerNotregistered", "ctx_waitForProcessFinish", serverId);

	// waits for finish
	it->second->waitForProcess(WAIT_FINISH_TOUT);
	delete it->second;

	processes_m.erase(serverId);

}

void CoreSpawner::setupResult(HlpStruct::XmlParameter& result, int const resultcode, CppUtils::String const& resultString)
{
	result.getParameter("operation_code").getAsInt(true) = resultcode;
	result.getParameter("operation_result").getAsString(true) = resultString;
}

void CoreSpawner::cleanup()
{
	for( map<CppUtils::String, CppUtils::Process*>::iterator it = processes_m.begin(); it != processes_m.end(); it++) {
		
			if (it->second != NULL) {
				try {
					it->second->killProcess();
				}
				catch(...) 
				{
				};
				
				try {
					delete it->second;
				}
				catch(...)
				{
				}
			}
		
	}

	processes_m.clear();

}

