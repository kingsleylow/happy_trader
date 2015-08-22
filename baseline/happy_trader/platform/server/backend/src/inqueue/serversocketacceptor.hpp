#ifndef _BACKEND_INQUEUE_SERVERSOCKETACCEPTOR_INCLUDED
#define _BACKEND_INQUEUE_SERVERSOCKETACCEPTOR_INCLUDED

#include "inqueuedefs.hpp"
#include "commontypes.hpp"
#include "transportbase.hpp"
#include "transporttcpip.hpp"

namespace Inqueue {

	/**
	Interface to subscribe accept events
	Clients must only close these socket
	But must not delete them
	*/

	class INQUEUE_BCK_EXP ServerSocketControllerAceptor
	{
	public:
		virtual void onSelectedAccept(HtClientSocket& clientsocket) = 0;
		
	};

	/**
		The class is responsible for managing the single server socket object providing client sockets
	*/
	class INQUEUE_BCK_EXP ServerSocketController: 
		public CppUtils::RefCounted, 
		private CppUtils::Mutex,
		public CppUtils::ProcessManager::Bootable 
	{
		public:
			static CppUtils::String const &getRootObjectName();

			ServerSocketController(int const port, int const servicePort);

			~ServerSocketController();

			void threadRun();

			virtual void boot();

			// subscribe for client and set up selection strings
			void subscribeClient(ServerSocketControllerAceptor& acceptor, CppUtils::String const& selectionString);


			inline HtServerSocket& getServerSocket()
			{
				return *ssrvsock_m;
			}

			// clear client sockets ignoring all errors
			void uninitializeClientSockets();

			void clearClientSockets();

			void clearServerSocket();

			int getServivePort() const
			{
				return servicePort_m;
			}

			CppUtils::String const& getServiceHost() const
			{
				return serviceHost_m;
			}

		private:

			
			// shutdown event
			CppUtils::Event toShutdown_m;

			// started event
			CppUtils::Event started_m;


			// thread object to run in background
			CppUtils::Thread* thread_m;

			// server socket
			HtServerSocket* ssrvsock_m;

			// server socket port
			int port_m;

			// back connect for service
			int servicePort_m;

			CppUtils::String serviceHost_m;

			// list of accepted client sockets
			vector<HtClientSocket*> clients_m;

			// list of accepted clients
			map<CppUtils::String, ServerSocketControllerAceptor*> selectee_m;

			// XML parameter handler
			HlpStruct::ParametersHandler paramsProc_m;

			CppUtils::Event startDoingJob_m;



	};
};

#endif