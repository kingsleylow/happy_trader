#ifndef _BACKEND_INQUEUE_COMPROC_INCLUDED
#define _BACKEND_INQUEUE_COMPROC_INCLUDED

#include "inqueuedefs.hpp"
#include "inquote.hpp"
#include "engine.hpp"
#include "serversocketacceptor.hpp"
#include "transportbase.hpp"
#include "transporttcpip.hpp"

	
namespace Inqueue {
	
	


	/**
		Class is responsible for retreiving and processing commands.
		Shall run in a main thread.
		Also this class manages other objects life-cycle
	*/
	class INQUEUE_BCK_EXP InQueueController: /*public Admin::ManagedObject,*/ 
		public CppUtils::RefCounted, 
		public CppUtils::ProcessManager::Bootable,
		public ServerSocketControllerAceptor
	{

	public:

		// resolve session name as this is not changed
		static CppUtils::String const &resolveSessionName();

		static void setSessionName(CppUtils::String const& session);

		
		static CppUtils::String const &getRootObjectName();
			
		// ctor & dtor
		InQueueController(CppUtils::String const &configSection);
		
		~InQueueController();

		// main function processing input commands
		void run();

		// boot function perfroming any kind of initialization
		virtual void boot();

	
		virtual void onSelectedAccept(HtClientSocket& clientsocket);

		
		// this saves errored content in the file
		static CppUtils::String storeErroredContent(CppUtils::String const& prefix, CppUtils::MemoryChunk const& in_buf);

		// this is a callback which is called on any event generated
		void onAlgBrkEventArrive(HlpStruct::EventData const& sourceEvent, HlpStruct::EventData const& response);

	private:

		// helpers

		
		// process command and returns result back
		bool processCommand(HlpStruct::XmlParameter const& command, HlpStruct::XmlParameter& result); 

		
		// parameter processor
		HlpStruct::ParametersHandler paramsProc_m;

		void createErrorResponse(
			HlpStruct::XmlParameter& result,
			CppUtils::String const& message, 
			CppUtils::String const& arg, 
			CppUtils::String const& ctx);
				

		void loadServerResponse(HlpStruct::XmlParameter& result);

		// load & unload algorithms
		void loadAlgorithm(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result);
		
		void unloadAlgorithm(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result);

		// return loaded algorithm parameters
		void getLoadedAlgorithmParameters(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result);

		// returns currently loaded algorithm broker parameters for the thread
		void getLoadedAlgorithmParametersForTheThread(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result);


		// start & stop threads
		void startThread(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result);

		void stopThread(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result);

		// start & stop RT
		void startRT(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result);

		void stopRT(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result);

		// force thread to go into simulation mode
		void startHistorySimulation(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result);

		// force thread to stop simulation
		void stopHistorySimulation(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result);

		// take history data
		void pushHistoryData(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result);

		// explicetely clears history cache
		void clearHistoryCache(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result);


		// alg/ broker dialog
		void sendThreadEvent(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result);

		// alg/broker dialog blocking
		void sendThreadEventBlocking(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result);

		// returns the size of the queue for the defined thread
		void isInputQueueEmpty(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result);

		
		// return only loaded algorithms
		void getLoadedAlgorithms(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result);

		void isAlgorithmLoaded(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result);

		// return started threads
		void getStartedThreads(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result);

		// return advanced info about started threads
		void getStartedThreads2(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result);

		// get running therad status
		void getThreadStatus(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result);

		// this is set events types that server will send back
		void setAllowedOutEvents(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result);

		// this sets up logging level
		void setLogLevel(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result);

		// returns current log level
		void getLogLevel(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result);

		// returns allowed output events
		void getAllowedOutEvents(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result);

		// hallo event
		void createHalloEvent( HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result);

		// get cached symbols and periods
		void getCachedSymbols(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result);

		void getGeneratedSimulationTicksDetails(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result);

		// get cached in memory symbol detail
		void getMemoryCachedSymbolDetail(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result);


		// load another parameters to algorithm broker pairs
		// while is is still functioning
		void reloadAlgorithmBrokerParameters(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result);

		// this prepares simulation ticks
		void prepareSimulationTicks(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result);

		void queryAvailableSimCaches(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result);

		
		// members
		
		// engine
		Engine& engine_m;

		
		// context
		HistoryContext& hstContext_m;

		// event showing whether it accepted incoming connection
		CppUtils::Event acceptedConnection_m;

		// session name
		static CppUtils::String sessionName_m;

		// this is fired when a client socket object is ready and the connection was accepted and selected
		CppUtils::Event connectionAccepted_m;
		
		// by default muts be signalled
		CppUtils::Event algLibEventReady_m;
				
		HlpStruct::EventData algLibEvent_m;

		CppUtils::Mutex algLibEventListMtx_m;

		HtClientSocket* clientsocket_m;
	};

};




#endif