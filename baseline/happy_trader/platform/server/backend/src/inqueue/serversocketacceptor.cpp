#include "serversocketacceptor.hpp"
#include "comproc.hpp"
#include "outproc.hpp"

#define SRVSCOCKETCONTROLLER "SRVSCOCKETCONTROLLER"

namespace Inqueue {

	int bootSocketServerThread (void *ptr)
	{
		// handle run-time exceptions
#ifdef HANDLE_NATIVE_EXCEPTIONS
			Testframe::init();
#endif

		ServerSocketController* pThis = (ServerSocketController*)ptr; 

		try {
			pThis->threadRun();
		}
		catch(CppUtils::Exception& e) {
			LOG(MSG_FATAL, SRVSCOCKETCONTROLLER, "Fatal exception: " << e.message());
		}
#ifdef HANDLE_NATIVE_EXCEPTIONS
		catch(Testframe::Exception& e) {
			LOG(MSG_FATAL, SRVSCOCKETCONTROLLER, "Fatal MSVC exception: " << e.message());
		}
#endif
		catch(...) {
			LOG(MSG_FATAL, SRVSCOCKETCONTROLLER, "Unknown fatal exception");
		}

		return 42;

	}

	// ---------------------------------------

	CppUtils::String const &ServerSocketController::getRootObjectName()
	{
			static CppUtils::String name_s( "srvsocket_controller" );
			return name_s;
	}

	ServerSocketController::ServerSocketController(int const port, int const servicePort):
	port_m(port),
	servicePort_m(servicePort),
	serviceHost_m("127.0.0.1")
	{
		
		ssrvsock_m = HtServerSocket_TCPIP::createTCPIPSocket();
		thread_m = new CppUtils::Thread(bootSocketServerThread, this);

		// Register with the process manager
		CppUtils::ProcessManager::singleton().registerBootable( *this );

		LOG(MSG_INFO, SRVSCOCKETCONTROLLER, "Server socket controller starting, listening on the port: " << port_m );
	}
 
	ServerSocketController::~ServerSocketController()
	{
		LOG( MSG_DEBUG, SRVSCOCKETCONTROLLER, "Server listener finishing..." );
		
		toShutdown_m.signalEvent();

		// normally happens on already closed sockets
		clearClientSockets();
		clearServerSocket();

				
		
		// need tow wait until it starts
		if (!started_m.lock(5000)) {
			LOG( MSG_WARN, SRVSCOCKETCONTROLLER, "Will not completely deinitialize Server socket controller thread as it may be dead-locked because initialization was not finished" );
			return;
		}

		thread_m->waitForThread();
		 
		// Delete thread
		delete thread_m;
		thread_m = NULL;

		delete ssrvsock_m;
		LOG( MSG_DEBUG, SRVSCOCKETCONTROLLER, "Server listener thread terminated. Server socket controller destroyed." );
	}

	
	void ServerSocketController::boot()
	{
		
		InQueueController* inqueuecontrollerptr = CppUtils::getRootObject<Inqueue::InQueueController>();
		QuoteProcessor* quoteprocessor = CppUtils::getRootObject<Inqueue::QuoteProcessor>();
		OutProcessor* outprocessor = CppUtils::getRootObject<Inqueue::OutProcessor>();

		subscribeClient(*inqueuecontrollerptr, Inqueue::InQueueController::getRootObjectName());
		subscribeClient(*quoteprocessor, Inqueue::QuoteProcessor::getRootObjectName());
		subscribeClient(*outprocessor, Inqueue::OutProcessor::getRootObjectName());


		startDoingJob_m.signalEvent();
		LOG( MSG_DEBUG, SRVSCOCKETCONTROLLER, "Successfully booted, subscribed all clients" );
	}

	void ServerSocketController::threadRun()
	{
		// wait until startup phase finishes
		{
			ON_SHUTDOWN(1;); 
		}
 
		//CppUtils::String hostname = "127.0.0.1";

		// establish socket
		if (!ssrvsock_m->Create())
			THROW(CppUtils::OperationFailed, "exc_ServerSocketQueueRun", "ctx_CannotCreateServerSocket", CTCPSocket::ErrorCodeToString(ssrvsock_m->GetLastError()));
			 
		// bind
		if (!ssrvsock_m->Bind("", port_m ))
			THROW(CppUtils::OperationFailed, "exc_ServerSocketQueueRun", "ctx_CannotBindServerSocket", " Attempt to bind to port: "+ CppUtils::long2String(port_m) + " "+ CTCPSocket::ErrorCodeToString(ssrvsock_m->GetLastError()));

		
		// signal we started
		started_m.signalEvent();

		CppUtils::MemoryChunk in_buf;
		HlpStruct::XmlParameter select_command;

		LOG( MSG_INFO, SRVSCOCKETCONTROLLER, "Started actual listening on port: " << port_m);
		if (!ssrvsock_m->Listen() ) 
			THROW(CppUtils::OperationFailed, "exc_ServerSocketQueueRun", "ctx_CannotListenSocket", CTCPSocket::ErrorCodeToString(ssrvsock_m->GetLastError()));					

		// lock here as waiting initialization
		startDoingJob_m.lock();

		int cnt = 0;
		do {

				// if shutdown
				if (toShutdown_m.lock(0))
						break;

				try {
				
					
					HtClientSocket* clientsocketP = HtClientSocket_TCPIP::createTCPIPSocket();
					clients_m.push_back(clientsocketP);

					LOG( MSG_INFO, SRVSCOCKETCONTROLLER, "Ready to accept");
					if (ssrvsock_m->Accept(clientsocketP)) {
						
						// here we need to know who issued this to initialize serviceHost_m
						
						LOG( MSG_INFO, SRVSCOCKETCONTROLLER, "Connection accepted, will determine who is this");
						
						in_buf.length(0);  
						select_command.clear(); 

						if (readFromSocket<HtClientSocket>(*clientsocketP, in_buf) < 0)
								THROW(CppUtils::OperationFailed, "exc_ServerSocketQueueRun", "ctx_CannotReadSocket", CTCPSocket::ErrorCodeToString(ssrvsock_m->GetLastError()));					

						paramsProc_m.deserializeXmlParameter(select_command, in_buf);

						// select
						CppUtils::String const& select_string = select_command.getName();

						if (Inqueue::InQueueController::getRootObjectName() == select_string ) {
							
							
							serviceHost_m = CSocketBase::LongToStdString( clientsocketP->GetPeerAddress());
							LOG( MSG_INFO, SRVSCOCKETCONTROLLER, "Completely setup service at the adress: " << getServiceHost() << ":" << getServivePort());
							
						}

						map<CppUtils::String, ServerSocketControllerAceptor*>::iterator it = selectee_m.find(select_string);

						if (it != selectee_m.end()) {
							it->second->onSelectedAccept(*clientsocketP);
						
						}
						else {
							LOG( MSG_WARN, SRVSCOCKETCONTROLLER, "Unknown socket recepient: " << select_string);
						}

						
						if (++cnt >= selectee_m.size()) {
							LOG( MSG_WARN, SRVSCOCKETCONTROLLER, "All connections are accepted, going to idle");
							toShutdown_m.lock();
							break;
						}	
						


					}
				
				}
				catch(CppUtils::Exception& e) {
					LOG( MSG_WARN, SRVSCOCKETCONTROLLER, "Exception in server socket listener queue: " << e.message());
					CppUtils::sleep(0.5);
				}
		#ifdef HANDLE_NATIVE_EXCEPTIONS
				catch(Testframe::Exception& e) {
					LOG(MSG_FATAL, SRVSCOCKETCONTROLLER, "Fatal MSVC exception in server socket listener queue: " << e.message());
				}		
		#endif
				catch(...) {
					LOG( MSG_WARN, SRVSCOCKETCONTROLLER, "Unknown exception in server socket listener queue");
					CppUtils::sleep(0.5);
				}

			// Exit?
			//
			//ON_SHUTDOWN_IDLE( break; );
	
		} while(true);

		LOG( MSG_INFO, SRVSCOCKETCONTROLLER, "Server socket listener thread finished." );

	}

	

	void ServerSocketController::subscribeClient(ServerSocketControllerAceptor& acceptor, CppUtils::String const& selectionString)
	{
		selectee_m[selectionString] = &acceptor;
	}

	void ServerSocketController::uninitializeClientSockets()
	{
		LOG( MSG_DEBUG, SRVSCOCKETCONTROLLER, "Uninitialize all sockets..." );

		for(int i = 0; i < clients_m.size(); i++) {
			try {
				clients_m[i]->Close();
				clients_m[i]->Shutdown(CSocket::ssBoth);
				
			}
			catch(...)
			{
				LOG( MSG_WARN, SRVSCOCKETCONTROLLER, "Error on closing client socket object, ignore..." );
			}

			
		}

	}

	void ServerSocketController::clearClientSockets()
	{
		LOG( MSG_DEBUG, SRVSCOCKETCONTROLLER, "Clear all client sockets..." );

		for(int i = 0; i < clients_m.size(); i++) {
			try {
				delete clients_m[i];
			}
			catch(...)
			{
				LOG( MSG_WARN, SRVSCOCKETCONTROLLER, "Error on deleting client socket object, ignore..." );
			}

			
			
		}

		clients_m.clear();
		
	}

	void ServerSocketController::clearServerSocket()
	{
		// remove srv socket
		LOG( MSG_DEBUG, SRVSCOCKETCONTROLLER, "Clearing server socket..." );
		try {
				ssrvsock_m->Shutdown(CSocket::ssBoth);
				ssrvsock_m->Close();
		}
		catch(...) {
		}
	}


}; // end of namespace