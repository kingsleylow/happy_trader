#include "enginethread.hpp"
#include "histcontext.hpp"
#include <time.h>
#include "outproc.hpp"
#include "utils.hpp"
#include "comproc.hpp"


namespace Inqueue {

	#define ENGINETHREAD "ENGINETHREAD"

	int bootEngineThread (void *ptr)
	{
		// handle run-time exceptions
#ifdef HANDLE_NATIVE_EXCEPTIONS
			Testframe::init();
#endif

			
		EngineThread* pThis = (EngineThread*)ptr; 
		try {
			pThis->threadRun();
		}
		catch(CppUtils::Exception& e) {
			LOG(MSG_FATAL, "ENGINETHREAD", "Fatal exception: " << e.message());
		}
#ifdef HANDLE_NATIVE_EXCEPTIONS
		catch(Testframe::Exception& e) {
			LOG(MSG_FATAL, "ENGINETHREAD", "Fatal MSVC exception: " << e.message());
		}
#endif
		catch(...) {
			LOG(MSG_FATAL, "ENGINETHREAD", "Unknown fatal exception");
		}
		return 42;

	}

	CppUtils::String EngineThread::getStringId() const
	{
		return CppUtils::String("ENGINETHREAD " + CppUtils::long2String(id_m));
	}
	
	EngineThread::EngineThread(
		Engine& engine, 
		map<int, AlgorithmHolder*> algorithmholderlist,
		map<CppUtils::String, HlpStruct::TradingParameters> const& tradingParameters
	):
		engine_m(engine),
		algorithmHolderList_m(algorithmholderlist),
		status_m(TH_Initializing), // when creating thread set this stytus to initializing
		thread_m(NULL),
		hstContext_m(*CppUtils::getRootObject<HistoryContext>()),
		tradingParameters_m(tradingParameters),
		aggregatedCache_m( CppUtils::getRootObject<HistoryContext>()),
		inqueueController_m(*CppUtils::getRootObject<InQueueController>())
	{
		
		thread_m = new CppUtils::Thread(bootEngineThread, this);

		// if cannot start thread for 10 seconds
		if (!threadStarted_m.lock(10000))
			THROW(CppUtils::OperationFailed, "exc_CannorStartThread", "ctx_CreateEngineThread", "");
	}

		
	EngineThread::~EngineThread()
	{
		LOG( MSG_DEBUG, getStringId().c_str(), "Engine thread finishing..." );
		
		// start shutdown
		
		// if we are simulating....
		stopSimulation_m.signalEvent();

		shutDownThread_m.signalEvent();

		thread_m->waitForThread();

		// Delete thread
		delete thread_m;
		thread_m = NULL;
		LOG( MSG_DEBUG, getStringId().c_str(), "Engine thread finishing terminated." );
		
		
		getAggregatedCache().cleanupAllCache();

	

		LOG( MSG_DEBUG, getStringId().c_str(), "EngineThread destroyed." );
		

	}

		
	void EngineThread::threadRun()
	{
			id_m = CppUtils::getThreadHandle();
			threadStarted_m.signalEvent();

			//CoInitializeEx(NULL, COINIT_MULTITHREADED );

			// initialize array of events
			CppUtils::NativeEvent events[] = 
			{
							eventArrived_m.getNativeEvent(),        // 0 
							level2DataArrived_m.getNativeEvent(),   // 1
							level1DataArrived_m.getNativeEvent(),   // 2
							dataArrived_m.getNativeEvent(),         // 3
							shutDownThread_m.getNativeEvent(),      // 4
							startSimulation_m.getNativeEvent()      // 5
			};

			

			LOG( MSG_DEBUG, getStringId().c_str(), "Engine thread started, will call process_Event_OnThreadStarted()  : " << id_m );

			// make initial notification - me is started !!!
			//  context accessor object
			ContextAccessor contextAccessor0(
							getAggregatedCache(),
							*this, 
							false, // no simulation
							false, // no last ticker
							false
			); 

			// delegate event data
			int it_idx = 1;
			int max_idx = algorithmHolderList_m.size();
			for(map<int, AlgorithmHolder*>::iterator it = algorithmHolderList_m.begin(); it != algorithmHolderList_m.end(); it++) {
					process_Event_OnThreadStarted(it->second, contextAccessor0, 1 == it_idx, it_idx == max_idx);

					// notify
					OutProcessor::sendEngineThreadStartedEvent(InQueueController::resolveSessionName(), getThreadId(), it->second->getAlgBrokerDescriptor().name_m);

					it_idx++;
			} // end algorithm loop

			
			LOG( MSG_DEBUG, getStringId().c_str(), "process_Event_OnThreadStarted() finished: " << id_m );

			// we are back to lazy
			{
				LOCK_FOR_SCOPE(*this);
				status_m = TH_Lazy;
			}

			threadInitialized_m.signalEvent();
			
			
		
			// map containing unclosed candles
			map<int, HlpStruct::PriceData> unclosedCandlesMap;
			while(true) {

				try {

					
								
					// wait some of them 
					int res = CppUtils::Event::lockForSomeEvents(events, 6);

					// event arrived
					if (res==0) {
						
						
						EventDataHelper eventDataHelper;
						{
							LOCK_FOR_SCOPE(*this);

							// must have always value
							if (!eventData_m.isEmpty()) {
							
								// copy event data - need to propagate furher, do not want to block
								eventDataHelper = eventData_m.get_front();
								eventData_m.delete_front();
							
							};
							
							// reset event
							eventArrived_m.clearEvent();
						}

						//  context accessor object
						ContextAccessor contextAccessor(
							getAggregatedCache(),
							*this, 
							//-1, // no ticker arrived - e.i. don't use time here as no new tiker arrived
							false, // no simulation
							false, // no last ticker
							false
						); 

						LOG( MSG_DEBUG, getStringId().c_str(), "Got event data in processing thread: \"" << HlpStruct::EventData::getEventTypeName(eventDataHelper.event_m.getEventType()) << "\"" );

						// delegate event data
						for(map<int, AlgorithmHolder*>::iterator it = algorithmHolderList_m.begin(); it != algorithmHolderList_m.end(); it++) {
							
							processAlgorithmBrokerInternalEvent(eventDataHelper.broker_seq_num_m, it->second, contextAccessor, eventDataHelper.event_m);
						
						} // end algorithm loop


						continue;
					}

					
					// shutdown signal
					if (shutDownThread_m.lock(0)) {
						break;
					}

					
					// history simulation
					if (res==5) {

						// simulation
						simulationHelper2();

						// at the finish clear the state
						{
							LOCK_FOR_SCOPE(*this);
							status_m = TH_Lazy;

							// reset event
							startSimulation_m.clearEvent();
						}
						
					}
						
					// level 2
					if (res==1) {
						// level 2 data arrived
						bool haveLevel2Data = false;
						HlpStruct::RtLevel2Data level2data;
						
						{
							LOCK_FOR_SCOPE(*this);

							if (!receivedLevel2Data_m.isEmpty()) {
								// copy this ticker as processing may take a lot of time
								// don't want to mutex this
								level2data = receivedLevel2Data_m.get_front();
								receivedLevel2Data_m.delete_front();

								haveLevel2Data = (status_m==TH_RT);

							}

							// extracted
							level2DataArrived_m.clearEvent();

						}

						// 
						if (haveLevel2Data) {
							// do all processing with level 2 data
							LOG( MSG_DEBUG, getStringId().c_str(), "Got Level2 data in processing thread");

							ContextAccessor contextAccessor(
								getAggregatedCache(),
								*this, 
								false, // no simulation
								false, // no last ticker
								false
							);

							{
									// prevent from changing here
									LOCK_FOR_SCOPE(tradingParametersMtx_m);


									// delegate level 2 data
									for(map<int, AlgorithmHolder*>::iterator it = algorithmHolderList_m.begin(); it != algorithmHolderList_m.end(); it++) {
										processLevel2Data(
											it->second, 
											contextAccessor,
											level2data,
											tradingParameters_m[it->second->getAlgBrokerDescriptor().name_m], 
											runName_m,
											runComment_m
										);
									};

							}
							
						}

					};






					
					// level 1
					if (res==2) {
						
						// level 1 data arrived
						bool haveLevel1Data = false;
						HlpStruct::RtLevel1Data level1data;
						
						{
							LOCK_FOR_SCOPE(*this);

							if (!receivedLevel1Data_m.isEmpty()) {
								// copy this ticker as processing may take a lot of time
								// don't want to mutex this
								level1data = receivedLevel1Data_m.get_front();
								receivedLevel1Data_m.delete_front();

								haveLevel1Data = (status_m==TH_RT);

							}

							// extracted
							level1DataArrived_m.clearEvent();

						}

						//
						if (haveLevel1Data) {
							// do all processing with level 2 data
							LOG( MSG_DEBUG, getStringId().c_str(), "Got Level1 data in processing thread");

							ContextAccessor contextAccessor(
								getAggregatedCache(),
								*this, 
								false, // no simulation
								false, // no last ticker
								false
							);

							{
									// prevent from changing here
									LOCK_FOR_SCOPE(tradingParametersMtx_m);

									// delegate level 2 data
									for(map<int, AlgorithmHolder*>::iterator it = algorithmHolderList_m.begin(); it != algorithmHolderList_m.end(); it++) {
										processLevel1Data(
											it->second, 
											contextAccessor,
											level1data,
											tradingParameters_m[it->second->getAlgBrokerDescriptor().name_m], 
											runName_m,
											runComment_m
										);
									};

							}
							
						}

							
					}; 
					
					// data arrived 
					if (res == 3) {
							bool haveRtData = false;
							HlpStruct::RtData rtdata;

							{
								LOCK_FOR_SCOPE(*this);

								if (!receivedData_m.isEmpty()) {
									
									// copy this ticker as processing may take a lot of time
									// don't want to mutex this
									rtdata = receivedData_m.get_front();
									receivedData_m.delete_front();

									haveRtData = (status_m==TH_RT);
								};
								
								// extracted  
								dataArrived_m.clearEvent();
							}
							
							if (haveRtData) {
								// process here
								LOG( MSG_DEBUG_DETAIL, getStringId().c_str(), "Got RT data in processing thread: \"" << rtdata.toString() << "\"" );

																				
								//  context accessor objecr
								ContextAccessor contextAccessor(
									getAggregatedCache(),
									*this, 
									//rtdata.getTime(), 
									false, // no simulation
									false, // no last ticker
									false
								); 
								
								{
									// prevent from changing here
									LOCK_FOR_SCOPE(tradingParametersMtx_m);
									
									// delegate RT data
									for(map<int, AlgorithmHolder*>::iterator it = algorithmHolderList_m.begin(); it != algorithmHolderList_m.end(); it++) {

										processRTTick(
											it->second, 
											rtdata, 
											contextAccessor, 
											tradingParameters_m[it->second->getAlgBrokerDescriptor().name_m], 
											runName_m,
											runComment_m);
										
									
									} // end algorithm loop
								} // end update parameter mtx

								
							}

						}


			}
			catch(CppUtils::Exception &e) {
				LOG( MSG_WARN, getStringId().c_str(), "Exception in Engine thread: " << e.message());
			}
#ifdef HANDLE_NATIVE_EXCEPTIONS
			catch(Testframe::Exception& e) {
				LOG(MSG_FATAL, getStringId().c_str(), "Fatal MSVC exception in Engine thread : " << e.message());
			}
#endif
			catch(...) {
				LOG( MSG_WARN, getStringId().c_str(), "Unknown exception in Engine thread");
			}

		} // end loop

		
		it_idx = 1;
		
		// delegate event data
		for(map<int, AlgorithmHolder*>::iterator it = algorithmHolderList_m.begin(); it != algorithmHolderList_m.end(); it++) {
				
				// pass trding parameters
				LOCK_FOR_SCOPE(tradingParametersMtx_m);
				
			
				process_Event_OnThreadFinished(
					it->second, 
					contextAccessor0, 
					tradingParameters_m[it->second->getAlgBrokerDescriptor().name_m], 
					it_idx == 1, 
					max_idx == it_idx 
				);

				// notify
				OutProcessor::sendEngineThreadFinishedEvent(InQueueController::resolveSessionName(), getThreadId(), it->second->getAlgBrokerDescriptor().name_m);

				it_idx++;
		} // end algorithm loop

			
		LOG( MSG_DEBUG, getStringId().c_str(), "process_Event_OnThreadFinished() finished: " << id_m );

		
		// notify the status
		{
				LOCK_FOR_SCOPE(*this);
				status_m = TH_Failure;
		}
	}
	
	
	void EngineThread::initTradeParamCacheAndBtest(
			BrkLib::SimulationProfileData const& simulationProfileStorage,
			BrkLib::BrokerBase::ProviderSymbolCookieMap const& providerSymbolCookieMap,
			HlpStruct::TradingParametersList& traderParamsCache,
			vector<AlgorithmHolder*>& algHolderCache
	)
	{
		traderParamsCache.clear();
		algHolderCache.clear();

		{
			LOCK_FOR_SCOPE(tradingParametersMtx_m);
			// initialization step
			for(map<int, AlgorithmHolder*>::iterator it = algorithmHolderList_m.begin(); it != algorithmHolderList_m.end(); it++) {
				

				// must be backtest library, otherwise finish 
				if (it->second->getBroker() != NULL) {
					if (!it->second->isBtestLib()) {
								THROW(CppUtils::OperationFailed, "exc_BrokerLibraryNotBacktest", "ctx_SimulationHelper", it->second->getAlgBrokerDescriptor().name_m	);
					}

					// init backtest if broker is not null
					it->second->initializeBacktest(simulationProfileStorage, providerSymbolCookieMap);
				}


				// must be backtest library, otherwise finish 
				if (it->second->getBroker2() != NULL) {
					if (!it->second->isBtestLib2()) {
								THROW(CppUtils::OperationFailed, "exc_BrokerLibrary2NotBacktest", "ctx_SimulationHelper", it->second->getAlgBrokerDescriptor().name_m	);
					}

					// init backtest if broker is not null
					it->second->initializeBacktest2(simulationProfileStorage, providerSymbolCookieMap);
				}
				
					

				// store descriptor and ptr
				traderParamsCache.push_back( tradingParameters_m[it->second->getAlgBrokerDescriptor().name_m] );
				algHolderCache.push_back(it->second);

			}
	
		}
	}

	CppUtils::Uid EngineThread::getLastSimulationUid() const
	{
		LOCK_FOR_SCOPE(*this);
		
		return simulationHistoryUid_m;
	}

	
	// new version
void EngineThread::simulationHelper2() 
{ 
	// first prepare cache

	CppUtils::IntList cookies;

	// create history context for simulations

			
	
	CppUtils::Uid suid_uid;
	
	{
		LOCK_FOR_SCOPE(*this); 
		suid_uid = simulationHistoryUid_m;
	}
	
	
	Inqueue::SimulHistoryContext simhistcontext(suid_uid);

	// do not delete symbols
	simhistcontext.setReuseCacheFlag(true);
			  

	try {
		LOG( MSG_INFO, getStringId().c_str(), "Simulation2 started" );
		LOGEVENTA(HlpStruct::CommonLog::LL_INFO, ENGINETHREAD, "Simulation2 started", getStringId().c_str()	);

		// create provider symbol list
		HlpStruct::ProviderSymbolList provSymbSimulationList;
		BrkLib::BrokerBase::ProviderSymbolCookieMap providerSymbolCookieMap;
		
		// for fast access to spread
		CppUtils::DoubleList provSymbSimulationSpreadList;
		BrkLib::SimulationProfileData simulationProfileStorage;

		// restore saved data
		HlpStruct::XmlParameter matadata;
		CppUtils::String matadata_content;
		CppUtils::readContentFromFile2(simhistcontext.getMetadataPath(), matadata_content);
		if (matadata_content.size() ==0)
			THROW(CppUtils::OperationFailed, "exc_InvalidMetadataContent", "ctx_SimulationHelper2", simhistcontext.getMetadataPath());

		HlpStruct::deserializeXmlParameter(matadata, matadata_content); 
		deserializeSimulationProfile(simulationProfileStorage, matadata);
		
		
		initProviderSimulationListFromParameters(simulationProfileStorage, provSymbSimulationSpreadList,provSymbSimulationList);
		
		// for fast access 
		HlpStruct::TradingParametersList traderParamsCache;
		vector<AlgorithmHolder*> algHolderCache;
			
					
		for(int i = 0; i < provSymbSimulationList.size(); i++) {
			CppUtils::String const& provider = provSymbSimulationList[ i ].provider_m;
			CppUtils::String const& symbol = provSymbSimulationList[ i ].symbol_m;
			providerSymbolCookieMap[provider][symbol]=(i+1);
		}

		// init some caches
		initTradeParamCacheAndBtest(
			simulationProfileStorage,
			providerSymbolCookieMap,
			traderParamsCache,
			algHolderCache
		);
		

		// pass ticks here - in simulation 2
		/////////////////////

		// simulation cache used to colled simulated data
		AggregatedCache simCache(&simhistcontext);

		SimulationCollector collector(simCache, traderParamsCache, algHolderCache, simhistcontext, *this);

		long ticks = simhistcontext.readAllData(provSymbSimulationList, collector );
	

		// need to kill what all we cached
		simCache.cleanupAllCache();



		LOG( MSG_INFO, getStringId().c_str(), "Simulation2 successfully finished: " << ticks );
		LOGEVENTA(HlpStruct::CommonLog::LL_INFO, ENGINETHREAD, "Simulation2 successfully finished", getStringId().c_str()	);

	}
	catch(CppUtils::Exception &e) {
				LOGEVENTA(HlpStruct::CommonLog::LL_ERROR, ENGINETHREAD, "Exception in simulation2 function", getStringId().c_str() << e.message() << " " << e.context() << " " << e.arg()<< " " << e.addInfo() );
				LOG( MSG_WARN, getStringId().c_str(), "Exception in simulation2 function: " << e.message());
	}
#ifdef HANDLE_NATIVE_EXCEPTIONS
	catch(Testframe::Exception& e) {
				LOGEVENTA(HlpStruct::CommonLog::LL_ERROR,  ENGINETHREAD, "Fatal MSVC exception in simulation2 function", getStringId().c_str() << e.message()  );
				LOG(MSG_FATAL, getStringId().c_str(), "Fatal MSVC exception in simulation2 function : " << e.message());
	}
#endif
	catch(...) {
				LOGEVENTA(HlpStruct::CommonLog::LL_ERROR, ENGINETHREAD, "Unknown exception in simulation2 function", getStringId().c_str()   );
				LOG( MSG_WARN, getStringId().c_str(), "Unknown exception2 in simulation function");
	}

	LOG( MSG_INFO, getStringId().c_str(), "Completed simulation2 fully" );
	LOGEVENTA(HlpStruct::CommonLog::LL_INFO, ENGINETHREAD, "Completed simulation2 fully", getStringId().c_str()	);

}


// ------------------------------

void EngineThread::processLevel2Data(
		AlgorithmHolder* algHolder, 
		ContextAccessor& historyaccessor, 
		HlpStruct::RtLevel2Data const& level2data,
		HlpStruct::TradingParameters const& tradeParams,
		CppUtils::String const& runName,
		CppUtils::String const& runComment
	)
{
	try {
		algHolder->processLevel2Data( level2data, historyaccessor, tradeParams,runName,runComment  );
	}
	catch(CppUtils::Exception &e) {
				LOGEVENTA(HlpStruct::CommonLog::LL_ERROR, ENGINETHREAD,  "Exception occured in (processLevel2Data) call", getStringId().c_str() << e.message() << " " << e.context() << " " << e.arg() << " " << e.addInfo());
				LOG( MSG_WARN, getStringId().c_str(), "Exception occured in (processLevel2Data) call: " << e.message());
	}
#ifdef HANDLE_NATIVE_EXCEPTIONS
	catch(Testframe::Exception& e) {
				// as this should be plugin do need this to crash
				LOGEVENTA(HlpStruct::CommonLog::LL_ERROR, ENGINETHREAD,  "MSVC Exception occured in (processLevel2Data) call", getStringId().c_str() << e.message() );
				LOG(MSG_WARN, getStringId().c_str(), "Fatal MSVC exception in (processLevel2Data) call : " << e.message());
	}
#endif
	catch(...) {
				LOGEVENTA(HlpStruct::CommonLog::LL_ERROR, ENGINETHREAD,  "Unknown exception occured in (processLevel2Data) call", getStringId().c_str() );
				LOG( MSG_WARN, getStringId().c_str(), "Unknown exception in (processLevel2Data) call");
	}
}

// -------------------------

void EngineThread::processLevel1Data(
		AlgorithmHolder* algHolder, 
		ContextAccessor& historyaccessor, 
		HlpStruct::RtLevel1Data const& level1data,
		HlpStruct::TradingParameters const& tradeParams,
		CppUtils::String const& runName,
		CppUtils::String const& runComment
	)
{
	try {
		algHolder->processLevel1Data( level1data,historyaccessor, tradeParams,runName,runComment  );
	}
	catch(CppUtils::Exception &e) {
				LOGEVENTA(HlpStruct::CommonLog::LL_ERROR, ENGINETHREAD,  "Exception occured in (processLevel1Data) call", getStringId().c_str() << e.message() << " " << e.context() << " " << e.arg() << " " << e.addInfo());
				LOG( MSG_WARN, getStringId().c_str(), "Exception occured in (processLevel1Data) call: " << e.message());
	}
#ifdef HANDLE_NATIVE_EXCEPTIONS
	catch(Testframe::Exception& e) {
				// as this should be plugin do need this to crash
				LOGEVENTA(HlpStruct::CommonLog::LL_ERROR, ENGINETHREAD,  "MSVC Exception occured in (processLevel1Data) call", getStringId().c_str() << e.message() );
				LOG(MSG_WARN, getStringId().c_str(), "Fatal MSVC exception in (processLevel1Data) call : " << e.message());
	}
#endif
	catch(...) {
				LOGEVENTA(HlpStruct::CommonLog::LL_ERROR, ENGINETHREAD,  "Unknown exception occured in (processLevel1Data) call", getStringId().c_str() );
				LOG( MSG_WARN, getStringId().c_str(), "Unknown exception in (processLevel1Data) call");
	}
}

// ---------------------

void EngineThread::processAlgorithmBrokerInternalEvent(
	int const broker_seq_num,
	AlgorithmHolder* algHolder, 
	ContextAccessor& contextaccessor, 
	HlpStruct::EventData const &eventdata
)
{
		// internal event processin
		// generally used to pass commands directly to broker library in the form of XmlParameter

	try {

		//bool result;
		

			// note that for broker and algorithm layers are the same event types
			if (eventdata.getEventType() == HlpStruct::EventData::ET_AlgorithmEvent ) {
				
				
				// send this to algorithm layer
				algHolder->processAlgorithmRelatedEvent(eventdata, contextaccessor, inqueueController_m);
				
			}
			else if (eventdata.getEventType() == HlpStruct::EventData::ET_BrokerEvent ) {
				
			

				// send this to broker layer
				// for now here we have broker commands to be processed (like open order, close position, etc...)
				algHolder->processBrokerRelatedEvent(broker_seq_num, eventdata, inqueueController_m );
					
				
			}
			else if (eventdata.getEventType() == HlpStruct::EventData::ET_RtProviderSynchronizationEvent) {
				// this is special type as synch event from provider
				// propagate
			
				// delegate to algorithm
				algHolder->processRtProviderSynchEvent (
					contextaccessor,
					(HlpStruct::RtData::SynchType)eventdata.getAsXmlParameter().getParameter("type").getAsInt(),
					eventdata.getEventDate(),
					eventdata.getAsXmlParameter().getParameter("provider_name").getAsString(),
					eventdata.getAsXmlParameter().getParameter("message").getAsString()
				);
				
			}
			else
				THROW(CppUtils::OperationFailed, "exc_InvalidEventType", "ctx_InternalEventProcessingInsideEngineThread", (long)eventdata.getEventType());
			
		}
		catch(CppUtils::Exception &e) {
					LOGEVENTA(HlpStruct::CommonLog::LL_ERROR, ENGINETHREAD,  "Exception occured in (processAlgorithmBrokerInternalEvent) call", getStringId().c_str() << e.message() << " " << e.context() << " " << e.arg() << " " << e.addInfo());
					LOG( MSG_WARN, getStringId().c_str(), "Exception occured in (processAlgorithmBrokerInternalEvent) call: " << e.message());
		}
#ifdef HANDLE_NATIVE_EXCEPTIONS
		catch(Testframe::Exception& e) {
								// as this should be plugin do need this to crash
					LOGEVENTA(HlpStruct::CommonLog::LL_ERROR, ENGINETHREAD,  "MSVC Exception occured in (processAlgorithmBrokerInternalEvent) call", getStringId().c_str() << e.message() );
					LOG(MSG_WARN, getStringId().c_str(), "Fatal MSVC exception in (processAlgorithmBrokerInternalEvent) call : " << e.message());
		}
#endif
		catch(...) {
					LOGEVENTA(HlpStruct::CommonLog::LL_ERROR, ENGINETHREAD,  "Unknown exception occured in (processAlgorithmBrokerInternalEvent) call", getStringId().c_str() );
					LOG( MSG_WARN, getStringId().c_str(), "Unknown exception in (processAlgorithmBrokerInternalEvent) call");
		}

		LOG( MSG_DEBUG_DETAIL, getStringId().c_str(), "processAlgorithmBrokerInternalEvent(...) completed");

}


void EngineThread::processSimulationTickUpperLevel (
		HlpStruct::RtData const& rtdata, 
		AggregatedCache& simCache,
		bool const& isFirst,
		bool const& isLast,
		HlpStruct::TradingParametersList& traderParamsCache,
		vector<AlgorithmHolder*>& algHolderCache
	)
{
	if (
		(doSingleSimulationCall_m && (isFirst || isLast) ) ||
		(!doSingleSimulationCall_m)
	){
		// prepare accessor object
		ContextAccessor contextAccessor(
			simCache,
			*this, 
			//rtdata.getTime(), 
			true, // simulation
			isLast, // is last flag,
			isFirst // if first flag
		);
   
	
		for(int t = 0; t < algHolderCache.size(); t++) {
			processSimulationTick(rtdata, contextAccessor, traderParamsCache[ t ], algHolderCache[ t ]);
		}
	}
}


void EngineThread::processSimulationTick(
		HlpStruct::RtData const& rtdata, 
		ContextAccessor& historyaccessor,
		HlpStruct::TradingParameters const& tradeParams,
		AlgorithmHolder* algHolderPtr
)
{
	
			

			// pass this to all the algorithm-broker pairs
			try {
				// notify simulation broker if any
				if (algHolderPtr->getBroker() != NULL) {
					algHolderPtr->onSimulationBrokerDataArrived(rtdata);
				}

				if (algHolderPtr->getBroker2() != NULL) {
					algHolderPtr->onSimulationBrokerDataArrived2(rtdata);
				}
				
				// notify algorithm library
				algHolderPtr->onDataArrived(rtdata, historyaccessor, tradeParams, runName_m, runComment_m);
				

			}
			catch(CppUtils::Exception &e) {
				LOGEVENTA(HlpStruct::CommonLog::LL_ERROR, ENGINETHREAD, "Exception occured in simulation (processSimulationTick) call", getStringId().c_str() << e.message() << " " << e.context() << " " << e.arg() << " " << e.addInfo());
				LOG( MSG_WARN, getStringId().c_str(), "Exception occured in simulation <processSimulationTick> call: " << e.message());

				THROW(CppUtils::OperationFailed, "exc_simulTickException", "ctx_ProcessSimulTick", e.message() + " - " + e.arg() + " - " + e.context() );

			}
		#ifdef HANDLE_NATIVE_EXCEPTIONS
			catch(Testframe::Exception& e) {
				// as this should be plugin do need this to crash
				LOGEVENTA(HlpStruct::CommonLog::LL_ERROR,  ENGINETHREAD, "MSVC Exception occured in simulation (processSimulationTick) call", getStringId().c_str() << e.message()  );
				LOG(MSG_WARN, getStringId().c_str(), "Fatal MSVC exception in simulation <processSimulationTick> call : " << e.message());
				
				THROW(CppUtils::OperationFailed, "exc_MSVCsimulTickException", "ctx_ProcessSimulTick", e.message()  );

			}
		#endif
			catch(...) {
				LOGEVENTA(HlpStruct::CommonLog::LL_ERROR, ENGINETHREAD, "Unknown exception occured in simulation (processSimulationTick) call", getStringId().c_str()   );
				LOG( MSG_WARN, getStringId().c_str(), "Unknown exception in simulation <processSimulationTick> call");

				THROW(CppUtils::OperationFailed, "exc_UnknownSimulTickException", "ctx_ProcessSimulTick", "" );

			}

		
	
}


// -----------------------------
void EngineThread::process_Event_OnThreadStarted(
	AlgorithmHolder* aholder,
	ContextAccessor& historyaccessor,
	bool const firstLib,
	bool const lastLib
)
{
	try {
		aholder->event_OnThreadStarted(historyaccessor, firstLib, lastLib);
	}
	catch(CppUtils::Exception &e) {
		LOGEVENTA(HlpStruct::CommonLog::LL_ERROR, ENGINETHREAD,  "Exception occured in (process_Event_OnThreadStarted) call", getStringId().c_str() << e.message() << " " << e.context() << " " << e.arg() << " " << e.addInfo() );
		LOG( MSG_WARN, getStringId().c_str(), "Exception occured in (process_Event_OnThreadStarted) call: " << e.message());
	}
#ifdef HANDLE_NATIVE_EXCEPTIONS
	catch(Testframe::Exception& e) {
		// as this should be plugin do need this to crash
		LOGEVENTA(HlpStruct::CommonLog::LL_ERROR, ENGINETHREAD,  "MSVC Exception occured in (process_Event_OnThreadStarted) call", getStringId().c_str() << e.message() );
		LOG(MSG_WARN, getStringId().c_str(), "Fatal MSVC exception in (process_Event_OnThreadStarted) call : " << e.message());
	}
#endif
	catch(...) {
		LOGEVENTA(HlpStruct::CommonLog::LL_ERROR, ENGINETHREAD,  "Unknown exception occured in (onDataArrived) call", getStringId().c_str() );
		LOG( MSG_WARN, getStringId().c_str(), "Unknown exception in (onDataArrived) call");
	}

}

// -----------------------------

void EngineThread::process_Event_OnThreadFinished(
	AlgorithmHolder* aholder,
	ContextAccessor& historyaccessor,
	HlpStruct::TradingParameters const& tradeParams,
	bool const firstLib,
	bool const lastLib
)
{
	try {
		aholder->event_OnThreadFinished(historyaccessor, tradeParams, firstLib, lastLib);
	}
	catch(CppUtils::Exception &e) {
		LOGEVENTA(HlpStruct::CommonLog::LL_ERROR, ENGINETHREAD,  "Exception occured in (process_Event_OnThreadFinished) call", getStringId().c_str() << e.message() << " " << e.context() << " " << e.arg() << " " << e.addInfo() );
		LOG( MSG_WARN, getStringId().c_str(), "Exception occured in (process_Event_OnThreadFinished) call: " << e.message());
	}
#ifdef HANDLE_NATIVE_EXCEPTIONS
	catch(Testframe::Exception& e) {
		// as this should be plugin do need this to crash
		LOGEVENTA(HlpStruct::CommonLog::LL_ERROR, ENGINETHREAD,  "MSVC Exception occured in (process_Event_OnThreadFinished) call", getStringId().c_str() << e.message() );
		LOG(MSG_WARN, getStringId().c_str(), "Fatal MSVC exception in (process_Event_OnThreadFinished) call : " << e.message());
	}
#endif
	catch(...) {
		LOGEVENTA(HlpStruct::CommonLog::LL_ERROR, ENGINETHREAD,  "Unknown exception occured in (onDataArrived) call", getStringId().c_str() );
		LOG( MSG_WARN, getStringId().c_str(), "Unknown exception in (onDataArrived) call");
	}

}
// -----------------------------
void EngineThread::processRTTick(
		AlgorithmHolder* aholder,
		HlpStruct::RtData const& rtdata, 
		ContextAccessor& historyaccessor, 
		HlpStruct::TradingParameters const& tradeParams,
		CppUtils::String const& runName,
		CppUtils::String const& runComment
)
{
	try {

		// if simulation library
		if (aholder->isBtestLib()) {
			aholder->onSimulationBrokerDataArrived( rtdata );
		}

		if (aholder->isBtestLib2()) {
			aholder->onSimulationBrokerDataArrived2( rtdata );
		}
		
		aholder->onDataArrived(rtdata, historyaccessor, tradeParams, runName, runComment);
	}
	catch(CppUtils::Exception &e) {
		LOGEVENTA(HlpStruct::CommonLog::LL_ERROR, ENGINETHREAD,  "Exception occured in (processRTTick) call", getStringId().c_str() << e.message() << " " << e.context() << " " << e.arg() << " " << e.addInfo() );
		LOG( MSG_WARN, getStringId().c_str(), "Exception occured in (processRTTick) call: " << e.message());
	}
#ifdef HANDLE_NATIVE_EXCEPTIONS
	catch(Testframe::Exception& e) {
		// as this should be plugin do need this to crash
		LOGEVENTA(HlpStruct::CommonLog::LL_ERROR, ENGINETHREAD,  "MSVC Exception occured in (processRTTick) call", getStringId().c_str() << e.message() );
		LOG(MSG_WARN, getStringId().c_str(), "Fatal MSVC exception in (processRTTick) call : " << e.message());
	}
#endif
	catch(...) {
		LOGEVENTA(HlpStruct::CommonLog::LL_ERROR, ENGINETHREAD,  "Unknown exception occured in (onDataArrived) call", getStringId().c_str() );
		LOG( MSG_WARN, getStringId().c_str(), "Unknown exception in (onDataArrived) call");
	}
}


// --------------------------------

void EngineThread::pushLevel2Data(HlpStruct::RtLevel2Data& level2data)
{
	LOCK_FOR_SCOPE(*this);


	if (status_m==TH_RT) {

		receivedLevel2Data_m.push_back( level2data );

		level2DataArrived_m.signalEvent();
	}
}

// --------------------------------

void EngineThread::pushLevel1Data(HlpStruct::RtLevel1Data& level1data)
{
	
	LOCK_FOR_SCOPE(*this);

	if (status_m==TH_RT) {
	
		receivedLevel1Data_m.push_back( level1data );

		level1DataArrived_m.signalEvent();
		
	}
}

// --------------------------------
	void EngineThread::pushRtData(HlpStruct::RtData & rtdata, bool const& tickerInsideExistingTime)
	{
		
		LOCK_FOR_SCOPE(*this);

		getAggregatedCache().pushRtData(rtdata, this, tickerInsideExistingTime);
		
	};

	// called back from AggregatedCache::pushRtData
	void EngineThread::pushRtDataHelper(HlpStruct::RtData & rtdata)
	{
		
		LOCK_FOR_SCOPE(*this);

		if (status_m==TH_RT) {

					receivedData_m.push_back(rtdata);
				
					// data arrived event
					dataArrived_m.signalEvent();
			
		}
	}

	

	void EngineThread::pushEventData(int const broker_seq_num, HlpStruct::EventData const& eventdata)
	{
		// this will push event data that must be processed in the algorithm/thread
		
		
		EventDataHelper eventDataHelper(eventdata, broker_seq_num);
		
		{
			LOCK_FOR_SCOPE(*this);
			eventData_m.push_back(eventDataHelper);

			eventArrived_m.signalEvent();
		}

		
	}

	bool EngineThread::isInputQueueEmpty() const
	{
		LOCK_FOR_SCOPE(*this);

		return eventData_m.isEmpty();
	}

	int EngineThread::inputQueuePendingSize() const
	{
		LOCK_FOR_SCOPE(*this);

		return eventData_m.size();
	}


	void EngineThread::forceThreadToRtMode(
		CppUtils::String const& runName, 
		CppUtils::String const& runComment,
		BrkLib::SimulationProfileData const& simProfile
	)
	{
		
		threadInitialized_m.lock();
		LOG(MSG_INFO, getStringId().c_str(), "Thread finished its initialization, can change status now");
		

		{

			LOCK_FOR_SCOPE(*this); 

			if (status_m != TH_Lazy)
					THROW(CppUtils::OperationFailed, "exc_EngineThread", "ctx_WrongThreadStatus", id_m);
		
			

			status_m = TH_RT;
			
			// change statuses of algorithms
			for(map<int , AlgorithmHolder*>::iterator it2 = algorithmHolderList_m.begin(); it2 != algorithmHolderList_m.end();it2++) {
					it2->second->setStatus( AS_RTParticipator );

					// if this is backtest library init simulation
					if (it2->second->isBtestLib()) {
						// init backtest if broker is not null
						it2->second->initializeBacktest(simProfile );

						LOG(MSG_INFO, getStringId().c_str(), "For rt btest simulation profile was provided as follows... ->");
						simProfile.printDebugInfo();
						LOG(MSG_INFO, getStringId().c_str(), "~~~~~~~~~~~~~~~~~~~~");

						
					}

					if (it2->second->isBtestLib2()) {
						// init backtest if broker is not null
						it2->second->initializeBacktest2(simProfile );

						LOG(MSG_INFO, getStringId().c_str(), "For rt btest simulation profile was provided as follows (broker lib 2)... ->");
						simProfile.printDebugInfo();
						LOG(MSG_INFO, getStringId().c_str(), "~~~~~~~~~~~~~~~~~~~~");
						
					}
				
			}
			runName_m = runName;
			runComment_m = runComment;

			
			LOG(MSG_DEBUG, getStringId().c_str(), "Algorithm thread " << id_m << " started RT processing, run name: " << runName_m );
		}
#ifdef _DEBUG
		simProfile.printDebugInfo();
#endif

	}

	

	void EngineThread::forceThreadToLazyMode()
	{
		LOCK_FOR_SCOPE(*this);
		if (status_m != TH_RT)
				THROW(CppUtils::OperationFailed, "exc_Engine", "ctx_WrongThreadStatus", (long)id_m);

		status_m = TH_Lazy;

			// change statuses of algorithms
		for(map<int , AlgorithmHolder*>::iterator it2 = algorithmHolderList_m.begin(); it2 != algorithmHolderList_m.end();it2++)
				it2->second->setStatus( AS_Free );
			

		LOG(MSG_DEBUG, getStringId().c_str(), "Algorithm thread " << id_m << " stoped RT processing");
	}


	void EngineThread::forceThreadToSimulation(
		CppUtils::String const& runName,
		CppUtils::String const& runComment,
		bool singleSimRun,
		CppUtils::Uid& simulationHistoryUid
	) 
	{
		LOCK_FOR_SCOPE(*this);

	
		if (startSimulation_m.lock(0))
			THROW(CppUtils::OperationFailed, "exc_ThreadAlreadyStartedSimulation", "ctx_StartThreadSimulation" , id_m);

		if (status_m != TH_Lazy)
				THROW(CppUtils::OperationFailed, "exc_WrongThreadStatus", "ctx_StartThreadSimulation" , id_m);

		
		// clear this event
		if (stopSimulation_m.lock(0))
			stopSimulation_m.clearEvent();
	


		// simulation time
		runName_m = runName;
		runComment_m = runComment;
		doSingleSimulationCall_m = singleSimRun;
		
		if (!simulationHistoryUid.isValid()) {
			THROW(CppUtils::OperationFailed, "exc_InvalidSimulationTicksUid", "ctx_StartThreadSimulation" , "");
		}

		// init with history UID
		simulationHistoryUid_m = simulationHistoryUid;
		


		LOG(MSG_INFO, getStringId().c_str(), "Algorithm thread " << id_m << 
			" run name: " << runName_m <<
			" single run flag: " << doSingleSimulationCall_m <<
			" simulation history data UID: " << simulationHistoryUid_m
		);

			

		// change statuses of algorithms
		for(map<int , AlgorithmHolder*>::iterator it2 = algorithmHolderList_m.begin(); it2 != algorithmHolderList_m.end();it2++)
				it2->second->setStatus( AS_SimParticipator );

		// start simualtion
		status_m = TH_Simulation;
		startSimulation_m.signalEvent();
		
	}

void EngineThread::stopSimulationForcibly()
{
	LOCK_FOR_SCOPE(*this);

	if (status_m != TH_Simulation)
			THROW(CppUtils::OperationFailed, "exc_WrongThreadStatus", "ctx_stopThreadSimulation" , id_m);
	
	// signal if not signalled
	if (!stopSimulation_m.lock(0))
		stopSimulation_m.signalEvent();

}

void EngineThread::forceToReReadChacheFromFile()
{
	LOCK_FOR_SCOPE(*this);
	
	// now we are sure that no RT is done upon the thread
	// so go through cache and  
	getAggregatedCache().cleanupAllCache();
}

void EngineThread::checkSymbolCached(
	CppUtils::String const& provider, 
	CppUtils::String const& symbol, 
	HlpStruct::AggregationPeriods const period,
	int const multfactor,
	int &cookie,
	double & beginData,
	double & endData
)
{
	LOCK_FOR_SCOPE(*this);
	getAggregatedCache().checkSymbolCached(provider,symbol, period,multfactor,cookie,beginData,endData );
}

void EngineThread::updateTradingParameters(CppUtils::String const& algBrkName, HlpStruct::TradingParameters const& parameters)
{
	LOCK_FOR_SCOPE(tradingParametersMtx_m);
	tradingParameters_m[ algBrkName ] = parameters;
}
	
HlpStruct::TradingParameters EngineThread::getTradingParameters(CppUtils::String const& algBrkName) const
{
	LOCK_FOR_SCOPE(tradingParametersMtx_m);

	map<CppUtils::String, HlpStruct::TradingParameters>::const_iterator it = tradingParameters_m.find(algBrkName);
	if (it==tradingParameters_m.end())
		THROW(CppUtils::OperationFailed, "exc_NoAlgorithmForThreadTradingParameters", "ctx_CreateEngineThread", algBrkName);
	return it->second;
}

//////
	HlpStruct::GlobalStorage& EngineThread::getThreadGlobalStorage()
	{
		return globalThreadStorage_m;
	}
	


	bool SimulationCollector::passRtData(HlpStruct::RtData& rtdata, bool const isFirst, bool const isLast)
	{
				if (engineThread_m.stopSimulation_m.lock(0)) {
						// signal to stop simulation
						LOG( MSG_INFO, engineThread_m.getStringId().c_str(), "Simulation2 forcibly stopped" );
						LOGEVENTA(HlpStruct::CommonLog::LL_INFO, ENGINETHREAD, "Simulation2 forcibly stopped ", engineThread_m.getStringId().c_str()	);
						return false;
				}

				simCache_m.pushRtData(rtdata, NULL, false);
				
				simhistcontext_m.setSimulationTimeOffset(rtdata.getTime());
				engineThread_m.processSimulationTickUpperLevel (
						rtdata, 
						simCache_m,
						isFirst,
						isLast,
						traderParamsCache_m,
						algHolderCache_m  
				);

				return true;
	}

}; // end of namespace