#include "engine.hpp"
#include "enginethread.hpp"
#include "algholder.hpp"

namespace Inqueue {

	

	CppUtils::String const & Engine::getRootObjectName () {
		static CppUtils::String string_s("engine");
		return string_s;
	}

	Engine::Engine()
	{
		CppUtils::ProcessManager::singleton().registerBootable( *this );
	}

	Engine::~Engine()
	{
		
	}

	void Engine::boot()
	{
			// initialize this
			LOG( MSG_INFO, "ENGINE", "Successfully booted");
	}

	void Engine::pushRtData(HlpStruct::RtData& rtdata, bool const& tickerInsideExistingTime)
	{
		LOCK_FOR_SCOPE(*this); 
		for(map<int, EngineThread*>::iterator it = activeThreads_m.begin(); it != activeThreads_m.end(); it++) {
			it->second->pushRtData(rtdata, tickerInsideExistingTime);
		}
	}

	void Engine::pushLevel2Data(HlpStruct::RtLevel2Data& level2data)
	{
		LOCK_FOR_SCOPE(*this); 
		for(map<int, EngineThread*>::iterator it = activeThreads_m.begin(); it != activeThreads_m.end(); it++) {
			it->second->pushLevel2Data(level2data);
		}
	}

	void Engine::pushLevel1Data(HlpStruct::RtLevel1Data& level1data)
	{
		LOCK_FOR_SCOPE(*this); 
		for(map<int, EngineThread*>::iterator it = activeThreads_m.begin(); it != activeThreads_m.end(); it++) {
			it->second->pushLevel1Data(level1data);
		}
	}

	void Engine::forceThreadsToUpdateCache()
	{
		LOCK_FOR_SCOPE(*this); 
		
		for(map<int, EngineThread*>::iterator it = activeThreads_m.begin(); it != activeThreads_m.end(); it++) {

			// thats for simulation
			it->second->forceToReReadChacheFromFile();

		

		}
	}

	void Engine::loadAlgorithm(
		HlpStruct::AlgBrokerDescriptor const& descriptor, 
		CppUtils::String const& session, 
		CppUtils::String const& runName,
		CppUtils::String const& runComment
	)
	{
			LOCK_FOR_SCOPE(*this); 
			map<CppUtils::String, AlgorithmHolder*>::iterator it = loadedAlgorithms_m.find(descriptor.name_m);
			if (it!=loadedAlgorithms_m.end()) {
					THROW(CppUtils::OperationFailed, "exc_Engine", "ctx_AlgorithmExist", descriptor.name_m);
			}

			// separate loading and deinitialization to avoid inconsistent state
			loadedAlgorithms_m[descriptor.name_m] = new AlgorithmHolder(descriptor, runName, runComment);
			LOG(MSG_DEBUG, "ENGINE", "Algorithm/Broker pair created \"" << descriptor.name_m << "\"" );

			// time to init
			loadedAlgorithms_m[descriptor.name_m]->initAlgorithmLibrary(runName, runComment);
			LOG(MSG_DEBUG, "ENGINE", "Algorithm initialized \"" << descriptor.name_m << "\"" );

			loadedAlgorithms_m[descriptor.name_m]->initBrokerLibrary();
			LOG(MSG_DEBUG, "ENGINE", "Broker (if any) initialized \"" << descriptor.name_m << "\"" );

			loadedAlgorithms_m[descriptor.name_m]->initBrokerLibrary2();
			LOG(MSG_DEBUG, "ENGINE", "Broker 2 (if any) initialized \"" << descriptor.name_m << "\"" );
			
	}

	void Engine::unloadAlgorithm(CppUtils::String const& algId )
	{
				LOCK_FOR_SCOPE(*this);

				map<CppUtils::String, AlgorithmHolder*>::iterator it = loadedAlgorithms_m.find(algId);
				if (it==loadedAlgorithms_m.end()) {
					THROW(CppUtils::OperationFailed, "exc_Engine", "ctx_AlgorithmNotExist", algId);
				}

				// need to check ref count
				if(it->second->getRefCount() > 0) {
					THROW(CppUtils::OperationFailed, "exc_Engine", "ctx_AlgorithmIsBusyWithThread", algId);
				}

				// deinit
				
				it->second->deinitAlgorithmLibrary();
				LOG(MSG_DEBUG, "ENGINE", "Algorithm deinitialized \"" << algId << "\"");

				it->second->deinitBrokerLibrary();
				LOG(MSG_DEBUG, "ENGINE", "Broker (if any) deinitialized \"" << algId << "\"");

				it->second->deinitBrokerLibrary2();
				LOG(MSG_DEBUG, "ENGINE", "Broker 2 (if any) deinitialized \"" << algId << "\"");

				// remove
				delete it->second;
				
				loadedAlgorithms_m.erase(algId);
				LOG(MSG_DEBUG, "ENGINE", "Algorithm/Broker pair removed \"" << algId << "\"");
			
	}

	int Engine::startAlgorithmThread(CppUtils::StringList const& algorithms)
	{
		LOCK_FOR_SCOPE(*this);

		map<int, AlgorithmHolder*> to_subscribe;
		map<CppUtils::String, HlpStruct::TradingParameters> initialTradeParameters;
		for(int i = 0; i < algorithms.size(); i++) {
			map<CppUtils::String, AlgorithmHolder*>::iterator it = loadedAlgorithms_m.find(algorithms[ i ]);
			if (it==loadedAlgorithms_m.end()) {
					THROW(CppUtils::OperationFailed, "exc_Engine", "ctx_AlgorithmNotExist", algorithms[ i ]);
			}

			int priority = it->second->getAlgBrokerDescriptor().priorityId_m;
			map<int, AlgorithmHolder*>::iterator it2 = to_subscribe.find(priority);

			if (it2 != to_subscribe.end())
				THROW(CppUtils::OperationFailed, "exc_Engine", "ctx_PriorityMustBeUnique", algorithms[ i ]);
			
			to_subscribe[ priority ] = it->second;
			initialTradeParameters[it->second->getAlgBrokerDescriptor().name_m] = it->second->getAlgBrokerDescriptor().initialAlgBrkParams_m;

			// increment ref count
			++it->second->getRefCount();

		}

		EngineThread *newThread = new EngineThread(*this, to_subscribe, initialTradeParameters);
		
		
		activeThreads_m[ newThread->getThreadId() ] = newThread;
		

		LOG(MSG_DEBUG, "ENGINE", "Algorithm thread started " << newThread->getThreadId() );
		return newThread->getThreadId();
	}

	void Engine::stopAlgorithmThread(int const threadId)
	{
		LOCK_FOR_SCOPE(*this);

		map<int, EngineThread*>::iterator it = activeThreads_m.find(threadId);
		if (it==activeThreads_m.end() )
			THROW(CppUtils::OperationFailed, "exc_Engine", "ctx_ThreadNotExist", (long)threadId);

		// decrement reference count for algorithms
		for(map<int , AlgorithmHolder*>::iterator it2 = it->second->algorithmHolderList_m.begin(); 
			it2 != it->second->algorithmHolderList_m.end(); it2++ ) 
		{
			--it2->second->getRefCount();
		}

		delete it->second;
		activeThreads_m.erase(threadId);

		LOG(MSG_DEBUG, "ENGINE", "Algorithm thread stopped " << threadId );
	}



	// simulation profile is optional parameter
	void Engine::startRT(
		int const threadId, 
		CppUtils::String const& runName, 
		CppUtils::String const& runComment,
		BrkLib::SimulationProfileData const& simProfile
	)
	{
		LOCK_FOR_SCOPE(*this);
			

		map<int, EngineThread*>::iterator it = activeThreads_m.find(threadId);
		if (it==activeThreads_m.end() )
			THROW(CppUtils::OperationFailed, "exc_Engine", "ctx_ThreadNotExist", (long)threadId);
		
		it->second->forceThreadToRtMode(runName, runComment, simProfile);
		
	}

	void Engine::stopRT(int const threadId)
	{
		LOCK_FOR_SCOPE(*this);

		map<int, EngineThread*>::iterator it = activeThreads_m.find(threadId);
		if (it==activeThreads_m.end() )
			THROW(CppUtils::OperationFailed, "exc_Engine", "ctx_ThreadNotExist", (long)threadId);

		it->second->forceThreadToLazyMode();
	}

	void Engine::stopHistorySimulation(int const threadId)
	{
		LOCK_FOR_SCOPE(*this);

		map<int, EngineThread*>::iterator it = activeThreads_m.find(threadId);
		if (it==activeThreads_m.end() )
			THROW(CppUtils::OperationFailed, "exc_Engine", "ctx_ThreadNotExist", (long)threadId);

		it->second->stopSimulationForcibly();
	}

	void Engine::startHistorySimulation(
		CppUtils::String const& runName,
		CppUtils::String const& runComment,
		int const threadId, 
		bool singleSimRun,
		CppUtils::Uid & simulationHistoryUid
	)
	{
		LOCK_FOR_SCOPE(*this);

		map<int, EngineThread*>::iterator it = activeThreads_m.find(threadId);
		if (it==activeThreads_m.end() )
			THROW(CppUtils::OperationFailed, "exc_Engine", "ctx_ThreadNotExist", (long)threadId);

		it->second->forceThreadToSimulation(
			runName,
			runComment,
			singleSimRun,
			simulationHistoryUid
		);

	}

	// checks symbol cached in memory
	void Engine::checkSymbolCached(int const threadId, 
			CppUtils::String const& provider, 
			CppUtils::String const& symbol, 
			HlpStruct::AggregationPeriods const period,
			int const multfactor,
			int &cookie,
			double & beginData,
			double & endData)
	{	
			LOCK_FOR_SCOPE(*this);
			map<int, EngineThread*>::iterator it = activeThreads_m.find(threadId);
			if (it==activeThreads_m.end() )
				THROW(CppUtils::OperationFailed, "exc_Engine", "ctx_ThreadNotExist", (long)threadId);
			it->second->checkSymbolCached(provider, symbol, period, multfactor, cookie, beginData,endData );
	}
		

	void Engine::getLoadedThreads(CppUtils::IntList& loadedThreads) const
	{
			LOCK_FOR_SCOPE(*this);
			loadedThreads.clear();

			for(map<int, EngineThread*>::const_iterator it = activeThreads_m.begin(); it!=activeThreads_m.end();it++) {
				loadedThreads.push_back(it->first);
			}

	}

		void Engine::getLoadedAlgorithms(CppUtils::StringList& loadedAlgorithms) const
		{
			LOCK_FOR_SCOPE(*this);
			loadedAlgorithms.clear();

			for(map<CppUtils::String, AlgorithmHolder*>::const_iterator it = loadedAlgorithms_m.begin(); it != loadedAlgorithms_m.end(); it++) {
				loadedAlgorithms.push_back(it->first);
			}

		}

		void Engine::getAlgorithmsForTheThread(int const threadId, CppUtils::StringList& threadAlgorithms) const
		{
			LOCK_FOR_SCOPE(*this);
			threadAlgorithms.clear();

			map<int, EngineThread*>::const_iterator it = activeThreads_m.find(threadId);
			if (it==activeThreads_m.end() )
				THROW(CppUtils::OperationFailed, "exc_Engine", "ctx_ThreadNotExist", (long)threadId);

			
			EngineThread const* eh = it->second;

			for(map<int , AlgorithmHolder*>:: const_iterator it = eh->algorithmHolderList_m.begin(); 
				it != eh->algorithmHolderList_m.end();
				it++) 
			{
				threadAlgorithms.push_back(it->second->getAlgBrokerDescriptor().name_m);
			}

		}

		void Engine::getLoadedThreads( vector<ThreadDescriptor>& threads ) const
		{
			LOCK_FOR_SCOPE(*this);

			threads.clear();

			for(map<int, EngineThread*>::const_iterator it = activeThreads_m.begin(); it!=activeThreads_m.end();it++) {

				ThreadDescriptor descr;
				descr.tid_m = it->first;
				descr.tstatus_m = it->second->status_m;
				descr.inputQueuePendingSize_m = it->second->inputQueuePendingSize();

				EngineThread const* eh = it->second;

				for(map<int , AlgorithmHolder*>:: const_iterator it2 = eh->algorithmHolderList_m.begin(); it2 != eh->algorithmHolderList_m.end();	it2++) {
					descr.loadedAlgBrkPairs_m.push_back(it2->second->getAlgBrokerDescriptor().name_m);
				}

				threads.push_back(descr);

			}

		}

		HlpStruct::AlgBrokerDescriptor  Engine::getAlgorithmDescriptor(CppUtils::String const& algId) const
		{
			LOCK_FOR_SCOPE(*this);

			map<CppUtils::String, AlgorithmHolder*>::const_iterator it = loadedAlgorithms_m.find(algId);
			if (it==loadedAlgorithms_m.end()) {
					THROW(CppUtils::OperationFailed, "exc_Engine", "ctx_AlgorithmNotExist", algId);
			}

			return it->second->getAlgBrokerDescriptor();

		}

		
		void Engine::reloadTradingParametersForTheThread(
			int const threadId, 
			CppUtils::String const& algBrkName,
			HlpStruct::TradingParameters const& tradeParams)
		{
			LOCK_FOR_SCOPE(*this);

			map<int, EngineThread*>::iterator it = activeThreads_m.find(threadId);
			if (it!=activeThreads_m.end() ) {
				it->second->updateTradingParameters(algBrkName, tradeParams);
			}
			else {
				// threadId < 0 means propagations to all the threads
				for(it = activeThreads_m.begin(); it != activeThreads_m.end(); it++) {
					it->second->updateTradingParameters(algBrkName, tradeParams);
				}
			}
		}

		void Engine::getTradingParametersForTheThread(
			int const threadId, 
			CppUtils::String const& algBrkName,
			HlpStruct::TradingParameters& tradeParams
		)
		{
				map<int, EngineThread*>::iterator it = activeThreads_m.find(threadId);
				if (it==activeThreads_m.end() ) 
					THROW(CppUtils::OperationFailed, "exc_Engine", "ctx_NoThreadExist", (long)threadId);
				
				tradeParams = it->second->getTradingParameters(algBrkName);

		}


		void Engine::passThreadEvent(int const broker_seq_num, int const threadId, HlpStruct::EventData const& event )
		{
			LOCK_FOR_SCOPE(*this); 

			map<int, EngineThread*>::iterator it = activeThreads_m.find(threadId);
			if (it!=activeThreads_m.end() ) {
				it->second->pushEventData(broker_seq_num, event);
			}
			else {
				// threadId < 0 means propagations to all the threads
				for(it = activeThreads_m.begin(); it != activeThreads_m.end(); it++) {
					it->second->pushEventData(broker_seq_num, event);
				}
			}

			
		}

		

		bool Engine::isInputQueueEmpty(int const threadId) const
		{
			LOCK_FOR_SCOPE(*this); 

			map<int, EngineThread*>::const_iterator it = activeThreads_m.find(threadId);

			if (it==activeThreads_m.end() )
				THROW(CppUtils::OperationFailed, "exc_Engine", "ctx_ThreadNotExist", (long)threadId);

			return it->second->isInputQueueEmpty();

		}

		bool Engine::isAlgorithmLoaded(CppUtils::String const& algBrkName) const
		{
			// check if it is loaded for all threads
			LOCK_FOR_SCOPE(*this); 

			map<CppUtils::String, AlgorithmHolder*>::const_iterator it = loadedAlgorithms_m.find(algBrkName);

			return (it != loadedAlgorithms_m.end());
		}

		ThreadStatus Engine::getThreadStatus(int const threadId)
		{
			LOCK_FOR_SCOPE(*this);

			map<int, EngineThread*>::iterator it = activeThreads_m.find(threadId);
			if (it==activeThreads_m.end() )
				THROW(CppUtils::OperationFailed, "exc_Engine", "ctx_ThreadNotExist", (long)threadId);

			return it->second->status_m;
		}

		void Engine::performCompleteCleanup()
		{
			LOCK_FOR_SCOPE(*this);

			LOG(MSG_DEBUG, "ENGINE", "Begin engine cleanup" );
			// need to clean up all negine threads
			CppUtils::IntList threads;
			getLoadedThreads(threads);

			for(int i = 0; i < threads.size(); i++) {
				try {
					stopAlgorithmThread(threads[i]);
				}
				catch(CppUtils::Exception& e) {
					LOG(MSG_WARN, "ENGINE", "On stopping algorithm thread \"" << threads[i] << "\" exception happened: " << e.message());
				}
				catch(...) {
					LOG(MSG_WARN, "ENGINE", "On stopping algorithm thread  \"" << threads[i] << "\" unknown exception happened");
				}
			}

			// and algorithms-broker pairs
			CppUtils::StringList loadedAlgorithms;
			getLoadedAlgorithms(loadedAlgorithms);

			for(int i = 0; i < loadedAlgorithms.size(); i++) {
				try {
					unloadAlgorithm(loadedAlgorithms[ i ]);
				}
				catch(CppUtils::Exception& e) {
					LOG(MSG_WARN, "ENGINE", "On unloading algorithm \"" << loadedAlgorithms[i] << "\" exception happened: " << e.message());
				}
				catch(...) {
					LOG(MSG_WARN, "ENGINE", "On unloading algorithm \"" << loadedAlgorithms[i] << "\" unknown exception happened");
				}
			}
			

			LOG(MSG_DEBUG, "ENGINE", "Finish engine cleanup" );
		}


}; // end of namespace