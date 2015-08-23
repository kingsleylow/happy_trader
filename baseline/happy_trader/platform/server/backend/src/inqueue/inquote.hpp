#ifndef _BACKEND_INQUEUE_INQUOTE_INCLUDED
#define _BACKEND_INQUEUE_INQUOTE_INCLUDED

#include "inqueuedefs.hpp"
#include "commontypes.hpp"
#include "histcontext.hpp"
#include "serversocketacceptor.hpp"
#include "transportbase.hpp"
#include "transporttcpip.hpp"



namespace Inqueue {

	/**
	Class is repsonsible for collecting input quotes. Collects quotes in the thread-safe queue. Retreives quotes as having
	server socket listener to the main client.
	*/

	class INQUEUE_BCK_EXP QuoteProcessor: 
		public CppUtils::RefCounted, 
		private CppUtils::Mutex, 
		public CppUtils::ProcessManager::Bootable,
		public ServerSocketControllerAceptor
{
	public:
		static CppUtils::String const &getRootObjectName ();

		
		QuoteProcessor();
		
		~QuoteProcessor();

		

		// this function starts in a separate thread, establishes socket connection and begin to retreive data
		void threadRun();

		// boot function perfroming any kind of initialization
		virtual void boot();

		// whether it accepted connection
		bool isAccepted();

		virtual void onSelectedAccept(HtClientSocket& clientsocket);
		
	private:
		
		// thread object to run in background
		CppUtils::Thread* thread_m;

		// XML parameter handler
		HlpStruct::ParametersHandler paramsProc_m;

		// history context object
		HistoryContext& hstContext_m;

		// shutdown event
		CppUtils::Event toShutdown_m;

		// started event
		CppUtils::Event started_m;

		
		// this is fired when a client socket object is ready and the connection was accepted and selected
		CppUtils::Event connectionAccepted_m;

		HtClientSocket* clientsocket_m;


		// server socket
		//CTCPSocket ssrvsock_m;

	};

}

#endif