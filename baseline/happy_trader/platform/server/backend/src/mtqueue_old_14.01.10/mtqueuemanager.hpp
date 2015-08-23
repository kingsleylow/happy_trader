#ifndef _MTQUEUE_MTPROXYMANAGER_INCLUDED
#define _MTQUEUE_MTPROXYMANAGER_INCLUDED

#include "mtqueuedefs.hpp"
#include "helperstruct.hpp"
#include "transport.hpp"
#include <queue>



namespace MtQueue {
	// base class for iner-pipe communications to make MT4 communication
	

	class MtQueueManager:  public CppUtils::Mutex {
	public:
		enum InitFlag{
			IF_CREATE = 0,
			IF_FREE  =1,
			IF_RESOLVE
		};

		// function to free this instance if ref counter is zero
		static void initialize(int const opFlag, MtQueue::MtQueueManager** ppProxymanager = NULL, int* refCountPar = NULL);

		

		// ctor & dtor
		MtQueueManager();

		~MtQueueManager();

		void startup(int const workThreadsCount = 1, int const queueMaxCapacity = QUEUE_MAX_CAPACITY, int logLevel = MSG_DEBUG_DETAIL);

		void shutdown();
	
		void run();

		void runWorkThread();

		// check that we are in shutdown state
		inline bool isShutDownState()
		{
			return shutDownThread_m.lock(0);
		}

		// push data
		void push(DataToSend const& data);

	
		CppUtils::String rbt_getInfo(  char const* key, char const* secret);

		
		CppUtils::String signRequest(CppUtils::String const& requestString, CppUtils::String const& secret);

	private:

		// helper for pushing queue
		void pushInternally(DataToSend const& data);

		
		
		// consiming threads
		vector<WorkingThread* > consumers_m;

		CppUtils::Thread* controlThread_m;

			// event whether thread started
		CppUtils::Event threadStarted_m;

		
		// signal to shutdown
		CppUtils::Event shutDownThread_m;

		CppUtils::Event dataArrived_m;

		int maxCapacity_m;

		list<DataToSend> queue_m;

		CppUtils::Mutex queueMtx_m;

		TransportLayer transport_m;

	
		CppUtils::Mutex someMutex_m;


	};

};

#endif
