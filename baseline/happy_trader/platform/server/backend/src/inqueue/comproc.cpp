
#include "comproc.hpp"
#include "outproc.hpp"
#include "utils.hpp"



#define INQUEUECONTROLLER "INQUEUECONTROLLER"
namespace Inqueue {
		CppUtils::String InQueueController::sessionName_m = "dummy";

		CppUtils::String const &InQueueController::resolveSessionName()
		{
			return InQueueController::sessionName_m;
		}

		void InQueueController::setSessionName(CppUtils::String const& session)
		{
			InQueueController::sessionName_m = session;
		}

		CppUtils::String const &InQueueController::getRootObjectName()
		{
			static CppUtils::String name_s( "inqueue_controller" );
			return name_s;
		}
 

		InQueueController::InQueueController(CppUtils::String const &configSection):
		//Admin::ManagedObject(configSection),
		engine_m(*CppUtils::getRootObject<Engine>()),
		hstContext_m(*CppUtils::getRootObject<HistoryContext>()),
		clientsocket_m(NULL)		
		{
			algLibEventReady_m.signalEvent();
			// Register with the process manager
			CppUtils::ProcessManager::singleton().registerBootable( *this );
		}
		  
		InQueueController::~InQueueController()
		{
			
			LOG(MSG_DEBUG, INQUEUECONTROLLER, "InQueueController destroyed" );
		}

		
		void InQueueController::run()
		{  
			//CoInitializeEx(NULL, COINIT_MULTITHREADED );
			//CoInitialize(NULL);

			if (connectionAccepted_m.lock()) {
					LOG(MSG_INFO, INQUEUECONTROLLER, "Command socket accepted incoming connection");
					
					
					bool doNotStop = true;
					while(1) {


						CppUtils::MemoryChunk in_buf, out_buf;
						HlpStruct::XmlParameter command, result;

						try {
							// reset buffers
							in_buf.length(0);
							out_buf.length(0);

							command.clear();
							result.clear();

							if (readFromSocket<HtClientSocket>(*clientsocket_m, in_buf) < 0)
								THROW(CppUtils::OperationFailed, "exc_InputQueueRun", "ctx_CannotReadSocket", CTCPSocket::ErrorCodeToString(clientsocket_m->GetLastError()));					

							LOG(MSG_DEBUG, INQUEUECONTROLLER, "Command data retreived of length: " << in_buf.length() << "\n" << in_buf.toString(256) << "\n" );

/*
#ifdef _DEBUG
							// we are logging the total message
							CppUtils::Log::debugOutput() << "XML message as follows: " << endl << "******XML******" << endl 

								<< in_buf.data() << endl << "******XML******" << endl;
#endif
*/
													
							// at this point no read should follow
							result.clear();
							 
							paramsProc_m.deserializeXmlParameter(command, in_buf);
							doNotStop = processCommand(command, result);
							
						}
						catch(CppUtils::Exception& e) {

							// no sence to store this in a file as this is an expected exception
							createErrorResponse(result, e.message(), e.arg(), e.context() );
							LOG( MSG_WARN, INQUEUECONTROLLER, "Exception in command queue: " << EXC2STR(e) );
							
						}
#ifdef HANDLE_NATIVE_EXCEPTIONS
						catch(Testframe::Exception& e) {

							CppUtils::String fileName = storeErroredContent("msvc_error_command_", in_buf);
							
							createErrorResponse(result, e.type(), "", "See command message in the file: " + fileName);
							LOG(MSG_FATAL, INQUEUECONTROLLER, "Fatal MSVC exception in command queue: " << e.message() << " see command in: " << fileName);
						}
#endif
						catch(...) {
							CppUtils::String fileName = storeErroredContent("unknown_error_command_", in_buf);

							createErrorResponse(result, "Unknown exception in command queue", "", "See command message in the file: " + fileName );
							LOG( MSG_WARN, INQUEUECONTROLLER, "Unknown exception in command queue, see command in: " << fileName );
						}

						paramsProc_m.serializeXmlParameter(result, out_buf);

						// send the response back
						if (writeToSocket<HtClientSocket>(*clientsocket_m, out_buf) < 0)
							THROW(CppUtils::OperationFailed, "exc_InputQueueRun", "ctx_CannotWriteSocket", CTCPSocket::ErrorCodeToString(clientsocket_m->GetLastError()));					

						LOG(MSG_DEBUG, INQUEUECONTROLLER, "The response sent of length: " << out_buf.length() << "\n" << out_buf.toString() << "\n" );
				
						if (!doNotStop) {
								LOG( MSG_INFO, INQUEUECONTROLLER, "Command to stop the server received.");
								break;
						}
				} // end of loop
					 
			}
			
			//CoUninitialize();
			
			

			// make engine cleanupfrom here
			engine_m.performCompleteCleanup();
									
		}
		
	
		CppUtils::String InQueueController::storeErroredContent(CppUtils::String const& prefix,CppUtils::MemoryChunk const& in_buf)
		{
			long time_event =  CppUtils::getTime();
			CppUtils::String fileName = CppUtils::getStartupDir() + prefix+CppUtils::long2String(time_event)+".xml"; 
			CppUtils::saveContentInFile2(fileName, in_buf);

			return fileName;

		}

		void InQueueController::boot()
		{
			
			
			// initialize this
			LOG( MSG_INFO, INQUEUECONTROLLER, "Successfully booted");
		}

		// helpers
		bool InQueueController::processCommand(HlpStruct::XmlParameter const& command, HlpStruct::XmlParameter& result)
		{
			// dummy
			result.clear();
			result.getName() = "response";

			bool shutDown = false;
			if (command.getName()=="load_server") {
				//loadServerResponse(result);
						
				// create hallo log event
				createHalloEvent(command, result);

			}
			else if (command.getName()=="shutdown_server") {
				shutDown =  true;
			}
			else if (command.getName()=="get_ping") {
				loadServerResponse(result);
			}
			else if (command.getName()=="load_algorithm") {
				loadAlgorithm(command, result);
			} 
			else if (command.getName()=="unload_algorithm") {
				unloadAlgorithm(command, result);
			}
			else if (command.getName()=="get_loaded_algorithm_params") {
				getLoadedAlgorithmParameters(command, result);
			}
			else if (command.getName()=="reload_algorithm_parameters") {
				reloadAlgorithmBrokerParameters(command, result);
			}
			else if (command.getName()=="reload_algorithm_parameters_for_thread") {
				getLoadedAlgorithmParametersForTheThread(command, result);
			}
			else if (command.getName()=="start_algorithm_thread") {
				startThread(command, result);
			}
			else if (command.getName()=="stop_algorithm_thread") {
				stopThread(command, result);
			}
			else if (command.getName()=="get_loaded_algorithms") {
				getLoadedAlgorithms(command, result);
			}
			else if (command.getName()=="is_algorithm_loaded") {
				isAlgorithmLoaded(command, result);
			}
			else if (command.getName()=="get_started_threads") {
				getStartedThreads(command, result);
			}
			else if (command.getName()=="get_started_threads2") {
				getStartedThreads2(command, result);
			}
			else if (command.getName()=="start_rt") {
				startRT(command, result);
			}
			else if (command.getName()=="stop_rt") {
				stopRT(command, result);
			}
			else if (command.getName()=="stop_history_simulation") {
				stopHistorySimulation(command, result);
			}
			else if (command.getName()=="start_history_simulation") {
				startHistorySimulation(command, result);
			}
			else if (command.getName()=="push_history_data") {
				pushHistoryData(command, result);
			}
			else if (command.getName()=="is_input_queue_empty") {
				isInputQueueEmpty(command, result);
			}
			else if (command.getName()=="get_thread_status") {
				getThreadStatus(command, result);
			}
			else if (command.getName()=="clear_history_cache") {
				clearHistoryCache(command, result);
			}
			// new set of commands managing broker
			else if (command.getName()=="push_thread_event") {
				// this will pass the command to alg/broker layer
				sendThreadEvent(command, result);
			}
			else if (command.getName()=="push_thread_event_blocking") {
				// this will pass result to block/alg layer and wait for a result
				sendThreadEventBlocking(command, result);
			}
			else if (command.getName()=="set_allowed_events") {
				// setAllowed Events
				setAllowedOutEvents(command, result);
			}
			else if (command.getName()=="set_log_level") {
				// set log level
				setLogLevel(command, result);
			}
			else if (command.getName()=="get_allowed_events") {
				// getAllowed Events
				getAllowedOutEvents(command, result);
			}
			else if (command.getName()=="get_log_level") {
				// get log level
				getLogLevel(command, result);
			}
			else if (command.getName()=="get_cached_symbols") {
				// get log level
				getCachedSymbols(command, result);
			}
			else if (command.getName()=="get_generated_simulation_ticks_details") {
				// get log level
				getGeneratedSimulationTicksDetails(command, result);
			}
			
			else if (command.getName()=="get_cached_symbol_detail_in_memory") {
				// get log level
				getMemoryCachedSymbolDetail(command, result);
			}
			else if (command.getName()=="prepare_simulation_ticks") {
				// get log level
				prepareSimulationTicks(command, result);
			}
			else if (command.getName()=="query_available_simulation_caches") {
				// get log level
				queryAvailableSimCaches(command, result);
			}
			else 
				THROW(CppUtils::OperationFailed, "exc_InputQueueRun", "ctx_UnknownCommand", command.getName() );

			// OK flags
			result.getParameter("status").getAsString(true)="OK";
			result.getParameter("status_code").getAsInt(true) = 0;

			return !shutDown;
		}
		
		void InQueueController::getGeneratedSimulationTicksDetails(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result)
		{
			CppUtils::Uid ticks_uid= command2.getParameter("ticks_uid").getAsUid();

			// create directory where metadata file resides
			CppUtils::String slistFilePath = SimulHistoryContext::getSymbolListPath(SimulHistoryContext::createStoragePrefixFromUid(ticks_uid));

			if (!CppUtils::fileExists(slistFilePath))
				THROW(CppUtils::OperationFailed, "exc_SymbolListFileNotExist", "ctx_getGeneratedSimulationTicksDetails", slistFilePath );

			HlpStruct::XmlParameter slistdata;
			CppUtils::String symbollist_content;
			CppUtils::readContentFromFile2(slistFilePath, symbollist_content);
			HlpStruct::deserializeXmlParameter(slistdata, symbollist_content); 

			// output
			result.getXmlParameter("result") = slistdata;


		}

		void InQueueController::getCachedSymbols(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result)
		{
			

			// if not valid - RT otherwise - simulation
			vector<CacheFileContext> cachedData;
			CppUtils::Uid simUid= command2.getParameter("simulation_uid").getAsUid();

			if (!simUid.isValid()) {
				hstContext_m.flushAll();
				hstContext_m.getCachedContext(cachedData);

				// 
				
			}
			else {
				// simulation
				Inqueue::SimulHistoryContext simhistcontext(simUid);
			
				// do not delete on exit
				simhistcontext.setReuseCacheFlag( true );


				simhistcontext.getCachedContext( cachedData );
			}

			// min and max times
			double min_total_time = -1;
			double max_total_time = -1;

			for(int i = 0; i < cachedData.size(); i++) {
				HlpStruct::XmlParameter par_i;

				par_i.getParameter("symbol").getAsString(true) = cachedData[i].getSymbol();
				par_i.getParameter("provider").getAsString(true) = cachedData[i].getProvider();
	
				double beg_time = cachedData[i].getBeginTime();
				double end_time = cachedData[i].getEndTime();

				if (beg_time > 0) {
					double beg_time_j = beg_time * 1000.0;
					if (min_total_time < 0)
						min_total_time = beg_time_j;
					else
						if (min_total_time > beg_time_j)
							min_total_time = beg_time_j;

					par_i.getParameter("begin_time").getAsDate(true) =beg_time_j;
					
				}
				else {
					par_i.getParameter("begin_time").getAsDate(true) = -1;
				}
				
				//
				if (end_time > 0) {
					double end_time_j = end_time * 1000.0;
					if (max_total_time < 0)
						max_total_time = end_time_j;
					else
						if (max_total_time < end_time_j)
							max_total_time = end_time_j;

					par_i.getParameter("end_time").getAsDate(true) = end_time_j;
				}
				else {
					par_i.getParameter("end_time").getAsDate(true) = -1;
				}

				result.getXmlParameterList("symbols").push_back(par_i);

			}

			result.getParameter("min_total_time").getAsDate(true) = min_total_time;
			result.getParameter("max_total_time").getAsDate(true) = max_total_time;

		}

		void InQueueController::getMemoryCachedSymbolDetail(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result)
		{
			int tid = command2.getParameter("algorithm_thread_id").getAsInt();
			HlpStruct::AggregationPeriods aggrPeriod = (HlpStruct::AggregationPeriods)command2.getParameter("aggr_period").getAsInt();
			int multFactor = command2.getParameter("mult_factor").getAsInt();

			CppUtils::String const& provider = command2.getParameter("provider").getAsString();
			CppUtils::String const& symbol = command2.getParameter("symbol").getAsString();

			int cookie;
			double begDate, endDate;
			engine_m.checkSymbolCached(tid, provider, symbol, aggrPeriod, multFactor, cookie, begDate, endDate);

			// output
			
			result.getParameter("cookie").getAsInt(true) = cookie;
			if (begDate > 0)
				result.getParameter("begin_date").getAsDate(true) = begDate * 1000.0;
			else
				result.getParameter("begin_date").getAsDate(true) = -1;

			if (endDate > 0)
				result.getParameter("end_date").getAsDate(true) = endDate * 1000.0;
			else
				result.getParameter("end_date").getAsDate(true) = -1;

		}

		void InQueueController::loadServerResponse(HlpStruct::XmlParameter& result) {
			result.getParameter("Common Parameters").getAsStringList(true).push_back( "Process ID: " + CppUtils::long2String( CppUtils::getPID() ) );

			// threads
			bool isRtAccept = CppUtils::getRootObject<QuoteProcessor>()->isAccepted();
			result.getParameter("Common Parameters").getAsStringList().push_back( "Accepted RT connection: " + CppUtils::String(isRtAccept ? "true" : "false") );
			result.getParameter("Common Parameters").getAsStringList().push_back( "Event log level: " + HlpStruct::CommonLog::getLogLevelName(OutProcessor::getLogLevel()) );

			CppUtils::IntList loadedThreads;
			engine_m.getLoadedThreads(loadedThreads);
			HlpStruct::XmlParameter thread_param;

			for(int i = 0; i < loadedThreads.size(); i++) {
				CppUtils::StringList threadAlgorithms;
				engine_m.getAlgorithmsForTheThread(loadedThreads[ i ], threadAlgorithms);
				thread_param.getXmlParameter(CppUtils::long2String(loadedThreads[ i ])).getParameter("Algorithms").getAsStringList(true) = threadAlgorithms;

				// thread status
				CppUtils::String t_status_str;
				ThreadStatus t_status = engine_m.getThreadStatus(loadedThreads[ i ]);
				
				t_status_str += ThreadStatusToString()(t_status);
			
				thread_param.getXmlParameter(CppUtils::long2String(loadedThreads[ i ])).getParameter("Status: " + t_status_str).getAsString(true) = "";

			}

			
			result.getXmlParameter("Loaded Threads") = thread_param;
			// algorithms


			CppUtils::StringList loadedAlgorithms;
			engine_m.getLoadedAlgorithms(loadedAlgorithms);
			
			// now sent proiorites 
			HlpStruct::XmlParameter alg_param;
			for(int i = 0; i < loadedAlgorithms.size();i++) {
				HlpStruct::AlgBrokerDescriptor descr = engine_m.getAlgorithmDescriptor(loadedAlgorithms[ i ]);
				
				alg_param.getParameter(descr.name_m).getAsStringList(true).push_back( "Priority: " + CppUtils::long2String(descr.priorityId_m) );
				
			}

			result.getXmlParameter("Loaded Algorithms") = alg_param;

			// return period of history and rt contexts
			HlpStruct::XmlParameter cached_data_param;

			hstContext_m.flushAll();

			vector<CacheFileContext> cachedData;
			hstContext_m.getCachedContext( cachedData );
			result.getParameter("Common Parameters").getAsStringList().push_back("The number of symbols cached: " + CppUtils::long2String(cachedData.size()));
			for(int i = 0; i < cachedData.size(); i++) {
				CppUtils::String hint = "Cached period: "+CppUtils::getGmtTime2(cachedData[ i ].getBeginTime()) + " - " + CppUtils::getGmtTime2(cachedData[ i ].getEndTime());
				cached_data_param.getXmlParameter(cachedData[ i ].getProvider()).getParameter(cachedData[ i ].getSymbol()).getAsStringList(true).push_back( hint );
			}	
		
			result.getXmlParameter("Cached data") = cached_data_param;

			for(int i = 0; i < HlpStruct::EventData::getAllAllowedEvents().size(); i++) {
				HlpStruct::EventData::EType event_type = HlpStruct::EventData::getAllAllowedEvents()[ i ];
				CppUtils::String data(HlpStruct::EventData::getEventTypeName(event_type));
				data += " - ";
				if (OutProcessor::isEventAllowed(event_type))
					data += " subscribed";
				else 
					data += " not subscribed";

				result.getParameter("Subscribed Events").getAsStringList(true).push_back( data  );
			}
			

		}

		void InQueueController::getLoadedAlgorithms(
			HlpStruct::XmlParameter const& command2, 
			HlpStruct::XmlParameter& result
		)
		{
			CppUtils::StringList loadedAlgorithms;
			

			// returns only algrithm for the current therad id or 
			int tid = command2.getParameter("thread_id").getAsInt();

			if (tid>0)
				engine_m.getAlgorithmsForTheThread(tid,loadedAlgorithms );
			else
				engine_m.getLoadedAlgorithms(loadedAlgorithms);
			
			result.getParameter("loaded_algorithms").getAsStringList(true) = loadedAlgorithms;

			

			for(int i = 0; i < loadedAlgorithms.size(); i++) {
				HlpStruct::AlgBrokerDescriptor descr = engine_m.getAlgorithmDescriptor(loadedAlgorithms[ i ]);
				result.getParameter("priorities").getAsIntList(true).push_back( descr.priorityId_m );
			}
		}

		void InQueueController::isAlgorithmLoaded(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result)
		{
				CppUtils::String alg_lib_pair = command2.getParameter("alg_lib_pair").getAsString();

				result.getParameter("is_algorithm_loaded").getAsInt(true) = (engine_m.isAlgorithmLoaded(alg_lib_pair) ? 1:0);
		}

		void InQueueController::getStartedThreads(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result)
		{
				CppUtils::IntList loadedThreads;
				engine_m.getLoadedThreads(loadedThreads);

				result.getParameter("started_threads").getAsIntList(true) = loadedThreads;

		}

		void InQueueController::getStartedThreads2(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result)
		{
				vector<ThreadDescriptor> loadedThreads;
				engine_m.getLoadedThreads(loadedThreads);

				HlpStruct::XmlParameter::XmlParameterList& param = result.getXmlParameterList("threads");
				param.clear();

				for(int i = 0; i < loadedThreads.size(); i++) {
					ThreadDescriptor const& tdesc_i = loadedThreads[ i ];
					HlpStruct::XmlParameter xmlparam_descr_i;

					xmlparam_descr_i.getParameter("tid").getAsInt(true) = tdesc_i.tid_m;
					xmlparam_descr_i.getParameter("loaded_alg_brk_pairs").getAsStringList(true) = tdesc_i.loadedAlgBrkPairs_m;
					xmlparam_descr_i.getParameter("input_queue_pending_size").getAsInt(true) = tdesc_i.inputQueuePendingSize_m;
					xmlparam_descr_i.getParameter("thread_status_int").getAsInt(true) = (long)tdesc_i.tstatus_m;
					xmlparam_descr_i.getParameter("thread_status").getAsString(true) = ThreadStatusToString()(tdesc_i.tstatus_m);

					param.push_back(xmlparam_descr_i);
				
				}

				

		}

		void InQueueController::getThreadStatus(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result)
		{

			int tid = command2.getParameter("thread_id").getAsInt();

			ThreadStatus t_status = engine_m.getThreadStatus(tid);

			result.getParameter("thread_status_int").getAsInt(true) = (long)t_status;
			result.getParameter("thread_status").getAsString(true) = ThreadStatusToString()(t_status);

		}

		
		void InQueueController::setAllowedOutEvents(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result)
		{
			HlpStruct::XmlParameter::XmlParameterList const& plist = command2.getXmlParameterList("allowed_events");

			for(int i = 0; i < plist.size(); i++) {
				HlpStruct::XmlParameter const& par_i = plist[ i ];

				HlpStruct::EventData::EType eventType = (HlpStruct::EventData::EType)par_i.getParameter("event_type").getAsInt();
				bool isAllowed = (par_i.getParameter("is_event_allowed").getAsInt()!=0);

				if (isAllowed)
					OutProcessor::setAllowedEvent(eventType);
				else
					OutProcessor::clearAllowedEvent(eventType);
			}
		}

		
		void InQueueController::setLogLevel(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result)
		{
			HlpStruct::CommonLog::LogLevel loglevel = (HlpStruct::CommonLog::LogLevel)command2.getParameter("log_level").getAsInt();
			OutProcessor::setLogLevel( loglevel );
			
		}

		
		void InQueueController::getLogLevel(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result)
		{
			HlpStruct::CommonLog::LogLevel loglevel = OutProcessor::getLogLevel();
			result.getParameter("log_level").getAsInt(true) = (int)loglevel;

		}

		
		void InQueueController::getAllowedOutEvents(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result)
		{
			vector<HlpStruct::EventData::EType> all = HlpStruct::EventData::getAllAllowedEvents();
			for(int i = 0; i < all.size(); i++) {
				HlpStruct::EventData::EType eventType = all[i];

				HlpStruct::XmlParameter par_i;
				par_i.getParameter("event_type").getAsInt(true) = (int)eventType;
				par_i.getParameter("is_event_allowed").getAsInt(true) = OutProcessor::isEventAllowed(eventType);
				
				result.getXmlParameterList("allowed_events").push_back(par_i);
			}	
		}


		void InQueueController::createErrorResponse(
			HlpStruct::XmlParameter& result,
			CppUtils::String const& message, 
			CppUtils::String const& arg, 
			CppUtils::String const& ctx)
		{
				result.getName() = "response";
				result.getParameter("status").getAsString(true)="ERROR";
				result.getParameter("status_code").getAsInt(true) = 1;
				result.getParameter("error_msg").getAsString(true) = CppUtils::hexlify(message);
				result.getParameter("error_ctx").getAsString(true) = CppUtils::hexlify(ctx);
				result.getParameter("error_arg").getAsString(true) = CppUtils::hexlify(arg);
		}


		

		void InQueueController::loadAlgorithm(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result)
		{
			
			HlpStruct::AlgBrokerDescriptor descriptor;
			

			descriptor.algLib_m = command2.getParameter("algorithm_path").getAsString();
			descriptor.brokerLib_m = command2.getParameter("broker_path").getAsString();
			descriptor.brokerLib2_m = command2.getParameter("broker_path2").getAsString();
			descriptor.name_m = command2.getParameter("name").getAsString();

			// ran name
			CppUtils::String const& runName = command2.getParameter("run_name").getAsString();
			CppUtils::String const& runComment = command2.getParameter("run_comment").getAsString();

			// parameters
			HlpStruct::ParametersHandler context_params;
			CppUtils::String unhex_param;
			CppUtils::String const& alg_param= command2.getParameter("algorithm_params").getAsString();
			CppUtils::String const& brok_param= command2.getParameter("broker_params").getAsString();
			CppUtils::String const& brok_param2= command2.getParameter("broker_params2").getAsString();
			
			
			unhex_param.resize(alg_param.size()/2);
			char * ptr = (char*)unhex_param.c_str();
			CppUtils::unhexlify(alg_param.size(), alg_param.c_str(), ptr);
			
			// set up initial parameters
			context_params.deserializeXmlParameter(descriptor.initialAlgBrkParams_m.algParams_m,unhex_param);

			unhex_param.resize(brok_param.size()/2);
			ptr = (char*)unhex_param.c_str();
			CppUtils::unhexlify(brok_param.size(), brok_param.c_str(), ptr);

			// set up initial parameters
			context_params.deserializeXmlParameter(descriptor.initialAlgBrkParams_m.brokerParams_m,unhex_param);

			unhex_param.resize(brok_param2.size()/2);
			ptr = (char*)unhex_param.c_str();
			CppUtils::unhexlify(brok_param2.size(), brok_param2.c_str(), ptr);

			// set up initial parameters
			context_params.deserializeXmlParameter(descriptor.initialAlgBrkParams_m.brokerParams2_m,unhex_param);

			descriptor.priorityId_m = command2.getParameter("priority_id").getAsInt();
			

			engine_m.loadAlgorithm(descriptor, sessionName_m, runName, runComment);
		}
		
		void InQueueController::unloadAlgorithm(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result)
		{
			// parse parameters
						
			engine_m.unloadAlgorithm(command2.getParameter("name").getAsString());

		}

		void InQueueController::getLoadedAlgorithmParameters(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result)
		{
			CppUtils::String const& alg_name = command2.getParameter("name").getAsString();

			HlpStruct::AlgBrokerDescriptor descr = engine_m.getAlgorithmDescriptor(alg_name);
			
			result.getXmlParameter("algorithm_params") = descr.initialAlgBrkParams_m.algParams_m;
			result.getXmlParameter("broker_params") = descr.initialAlgBrkParams_m.brokerParams_m;
		}

		

		void InQueueController::getLoadedAlgorithmParametersForTheThread(
			HlpStruct::XmlParameter const& command2, 
			HlpStruct::XmlParameter& result)
		{
			CppUtils::String const& alg_name = command2.getParameter("name").getAsString();
			int tid = command2.getParameter("thread_id").getAsInt();

			HlpStruct::TradingParameters tradeParams;
			engine_m.getTradingParametersForTheThread(tid, alg_name, tradeParams );

			result.getXmlParameter("algorithm_params") = tradeParams.algParams_m;
			result.getXmlParameter("broker_params") = tradeParams.brokerParams_m;

		}

		void InQueueController::startThread(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result)
		{
			
			// pass algorithm ids
			CppUtils::StringList const& algorithms = command2.getParameter("algorithm_list").getAsStringList();
			int tid = engine_m.startAlgorithmThread(algorithms);

			result.getParameter("algorithm_thread_id").getAsInt(true) = tid;


		}

		void InQueueController::stopThread(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result)
		{
		
			int tid = command2.getParameter("algorithm_thread_id").getAsInt();

			engine_m.stopAlgorithmThread(tid);

		}

		void InQueueController::startRT(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result)
		{
			int tid = command2.getParameter("algorithm_thread_id").getAsInt();
			CppUtils::String const& runName = command2.getParameter("run_name").getAsString();
			CppUtils::String const& runComment = command2.getParameter("run_comment").getAsString();

			LOG(MSG_INFO, INQUEUECONTROLLER, "Lazy->RT mode: " << tid );

			// optional parameter when we want to simulate RT
			BrkLib::SimulationProfileData simProfile;
			
			// if we pass additional info
			int add_info = command2.getParameter("pass_additional_info").getAsInt();
			if (add_info != 0) {

				CppUtils::StringList const& providers = command2.getParameter("provider_list").getAsStringList();
				CppUtils::StringList const& symbols = command2.getParameter("symbol_list").getAsStringList();

				HlpStruct::XmlParameter::XmlParameterList const& sxmlList = command2.getXmlParameterList("symbol_trade_info");
				for(int i = 0; i < sxmlList.size(); i++) {
					HlpStruct::XmlParameter const& sxml = sxmlList[i];

					BrkLib::SymbolTradingInfo tinfo;
					tinfo.comission_m = sxml.getParameter("comission").getAsDouble();
					tinfo.comissionPct_m = sxml.getParameter("comissionPct").getAsDouble();
					tinfo.executePosExactPrices_m = sxml.getParameter("execute_pos_exact_price").getAsInt();
					tinfo.minLimitOrderDistance_m = sxml.getParameter("min_limit_order_distance").getAsDouble();
					tinfo.minStopLossDistance_m = sxml.getParameter("min_stop_loss_distance").getAsDouble();
					tinfo.minStopOrderDistance_m = sxml.getParameter("min_stop_order_distance").getAsDouble();
					tinfo.minTakeProfitDistance_m = sxml.getParameter("min_take_profit_distance").getAsDouble();
					tinfo.slippage_m =sxml.getParameter("simulation_slippage").getAsDouble();
					tinfo.spread_m = sxml.getParameter("simulation_spread").getAsDouble();
					tinfo.signDigits_m = sxml.getParameter("sign_digits").getAsInt();
					tinfo.pointValue_m = sxml.getParameter("point_value").getAsDouble();

					simProfile.symbolsTradingInfo_m[providers[i]][symbols[i]] = tinfo;
					
				}
			}

			engine_m.startRT( tid , runName, runComment, simProfile);
		}

		void InQueueController::stopRT(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result)
		{
			int tid = command2.getParameter("algorithm_thread_id").getAsInt();

			LOG(MSG_INFO, INQUEUECONTROLLER, "RT->Lazy mode: " << tid );

			engine_m.stopRT( tid );


		}

		void InQueueController::startHistorySimulation(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result)
		{
			
			int tid = command2.getParameter("algorithm_thread_id").getAsInt();
			CppUtils::String const& runName = command2.getParameter("run_name").getAsString();
			CppUtils::String const& runComment = command2.getParameter("run_comment").getAsString();
					    
			bool singleSumRun = (bool)command2.getParameter("single_sim_run").getAsInt();
			CppUtils::Uid simulationHistoryUid = command2.getParameter("simulation_history_uid").getAsUid();

			LOG(MSG_INFO, INQUEUECONTROLLER, "Lazy->Simulation mode: " << tid );

			

			LOGEVENTA( HlpStruct::CommonLog::LL_INFO, INQUEUECONTROLLER, "Simulation pending" ,
				"Session name: " << sessionName_m <<
				"Run Name: " << runName <<
				"Command time: " <<CppUtils::getAscTime() <<
				"Single simulation run flag: " << singleSumRun <<
				"Simulation history passed UID: " << simulationHistoryUid.toString()
			);

				
			
			engine_m.startHistorySimulation(
				runName,
				runComment,
				tid, 
				singleSumRun,
				simulationHistoryUid
			);

			// need to return new simulation UID - it is valid and generated
			result.getParameter("simulation_history_uid").getAsUid(true) = simulationHistoryUid;


		}

		void InQueueController::stopHistorySimulation(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result)
		{
			int tid = command2.getParameter("algorithm_thread_id").getAsInt();
			LOG(MSG_INFO, INQUEUECONTROLLER, "Simulation->Lazy mode: " << tid );

			engine_m.stopHistorySimulation(tid);
		}

		void InQueueController::pushHistoryData(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result)
		{
			// here should be history data extracted from DB
			
			// before we get metadata (what we are going to send)
			HlpStruct::XmlParameter::XmlParameterList const& smbMetaData = command2.getXmlParameterList("symbol_metadata");
			bool keep_cache_after_shutdown = (bool)command2.getParameter("keep_cache_after_shutdown").getAsInt();
			bool no_data_to_load = (bool)command2.getParameter("no_data_to_load").getAsInt();

			double begin_query_date = command2.getParameter("begin_query_date").getAsDate() / 1000.0;
			if (begin_query_date <=0)
				begin_query_date = -1.0;

			double end_query_date = command2.getParameter("end_query_date").getAsDate() / 1000.0;
			if (end_query_date <=0)
				end_query_date = -1.0;

			map<CppUtils::String, HlpStruct::ProviderSymbolList> profileProviderSymbolMap;
			for(int i = 0; i < smbMetaData.size(); i++) {
				HlpStruct::XmlParameter const& mdata_i = smbMetaData[ i ];

				
				CppUtils::String const& provider = mdata_i.getParameter("provider").getAsString();
				CppUtils::String const& symbol = mdata_i.getParameter("symbol").getAsString();
				CppUtils::String const& profile = mdata_i.getParameter("profile").getAsString();

				HlpStruct::ProviderSymbolPair pair_i(provider, symbol);

				HlpStruct::ProviderSymbolList &plist = profileProviderSymbolMap[profile];
				plist.push_back(pair_i);
				  
				// now need to prepare cache
				hstContext_m.setReuseCacheFlag(provider, symbol, keep_cache_after_shutdown);

			}

			if (!no_data_to_load) {
				for(map<CppUtils::String, HlpStruct::ProviderSymbolList>::const_iterator it = profileProviderSymbolMap.begin(); it != profileProviderSymbolMap.end();it++) {
					bool res = loadDataFromMasterHelper(it->second, it->first, keep_cache_after_shutdown, begin_query_date, end_query_date);
					if (!res)
						THROW(CppUtils::OperationFailed, "exc_CannotLoadHistoryData", "ctx_PushHistoryData", CppUtils::String("profile: ") + it->first );
				}
			}
			else {
				LOG(MSG_INFO, INQUEUECONTROLLER, "No data were loaded as use only cached" );
			}
			

		}
		
		void InQueueController::clearHistoryCache(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result)
		{
			CppUtils::String const& symbol = command2.getParameter("symbol").getAsString();
			CppUtils::String const& provider = command2.getParameter("provider").getAsString();
			hstContext_m.clearHistoryCache(provider, symbol);
		}

		// broker dialog
		void InQueueController::sendThreadEvent(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result)
		{
			

			// the thread will accept any broker command 
			int tid = command2.getParameter("algorithm_thread_id").getAsInt();
			// must be 1 or 2 - our adsressee
			int broker_seq_num = command2.getParameter("broker_seq_num").getAsInt();

			HlpStruct::XmlParameter const& eventXmlParam = command2.getXmlParameter("event");

			HlpStruct::EventData eventData;
			HlpStruct::convertXmlParameterToEvent(eventData, eventXmlParam);

			
			engine_m.passThreadEvent(broker_seq_num, tid, eventData);

						
		}

		void InQueueController::sendThreadEventBlocking(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result)
		{
			
			// the thread will accept any broker command 
			int tid = command2.getParameter("algorithm_thread_id").getAsInt();
			// must be 1 or 2 - our adsressee
			int broker_seq_num = command2.getParameter("broker_seq_num").getAsInt();

			// how much tie if shall wait
			// -1 means infinitely
			int wait_sec = command2.getParameter("wait_sec").getAsInt();
			if (wait_sec <=0)
				wait_sec = -1;

			HlpStruct::XmlParameter const& eventXmlParam = command2.getXmlParameter("event");

			HlpStruct::EventData eventData;
			HlpStruct::convertXmlParameterToEvent(eventData, eventXmlParam);

			// reset event
			if (algLibEventReady_m.lock(0)) {

				// if signalleed - clear this
				algLibEventReady_m.clearEvent();
			}
			else {
				THROW(CppUtils::OperationFailed, "exc_EventInClearedState", "ctx_sendThreadEventBlocking", "");
			}
			
			
			engine_m.passThreadEvent(broker_seq_num, tid, eventData);

			if (algLibEventReady_m.lock(wait_sec * 1000)) {
				// leave in signalled state

				LOG(MSG_DEBUG, INQUEUECONTROLLER, "Event from broker or algorithm sucessfully arrived. Parent event id: " << eventData.getEventUid() );
							

				// we have our event
				HlpStruct::EventData event_response;
				{
					LOCK_FOR_SCOPE(algLibEventListMtx_m)
					event_response = this->algLibEvent_m;
				}

				if (event_response.getEventType() == HlpStruct::EventData::ET_AlgorithmBrokerEventResp_FromAlgorithm ) {
					HlpStruct::convertEventToXmlParameter(event_response, result.getXmlParameter("response_event"));
				}
				
			}
			else {
				// reset event
				algLibEventReady_m.signalEvent();

				THROW(CppUtils::OperationFailed, "exc_NoAlgOrBrokerEventReady", "ctx_sendThreadEventBlocking", "Timeout: " << wait_sec);
			}

						
		}

		void InQueueController::isInputQueueEmpty(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result)
		{
			int tid = command2.getParameter("algorithm_thread_id").getAsInt();

			result.getParameter("is_input_queue_empty").getAsInt(true) =	(engine_m.isInputQueueEmpty(tid) ? 1:0);
		}


		/*
		void InQueueController::waitUntilAcceptedIncomingConnection() {
			acceptedConnection_m.lock();
		}
		void InQueueController::resetWaitAcceptIncomingConnectionLock()
		{
			// release the lock
			if (!acceptedConnection_m.lock(0))
				acceptedConnection_m.signalEvent();
		}
		*/
		
		


		void InQueueController::createHalloEvent( HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result)
		{
			bool no_data_to_load =  command2.getParameter("no_data_to_load").getAsInt();
			bool keep_cache_after_shutdown =  command2.getParameter("keep_cache_after_shutdown").getAsInt();
			if (!no_data_to_load) {
					// need to remove all
					LOG(MSG_INFO, INQUEUECONTROLLER, "Removing old cache on start, as use only cached" );
					hstContext_m.clearHistoryCache();
			}

			if (keep_cache_after_shutdown) {
				hstContext_m.setReuseCacheFlag(true);
			}

			// add some results
			result.getParameter("log_file_name").getAsString(true) = CppUtils::prepareValidXml(CppUtils::Log::getLogFileName());
			LOGEVENTA( HlpStruct::CommonLog::LL_INFO, INQUEUECONTROLLER, "Session successfully started", sessionName_m << CppUtils::getAscTime() );
		};

		void InQueueController::reloadAlgorithmBrokerParameters(	HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result)
		{
			
			// alorithm broker entry name
			CppUtils::String const& name = command2.getParameter("name").getAsString();
			CppUtils::String const& alg_param = command2.getParameter("algorithm_params").getAsString();
			CppUtils::String const& brok_param = command2.getParameter("broker_params").getAsString();
			int tid = command2.getParameter("algorithm_thread_id").getAsInt();

			HlpStruct::TradingParameters newTradeParams;
					

			// parameters
			HlpStruct::ParametersHandler context_params;
			CppUtils::String unhex_param;
			
			
			unhex_param.resize(alg_param.size()/2);
			char * ptr = (char*)unhex_param.c_str();
			CppUtils::unhexlify(alg_param.size(), alg_param.c_str(), ptr);
			context_params.deserializeXmlParameter(newTradeParams.algParams_m,unhex_param);

			unhex_param.resize(brok_param.size()/2);
			ptr = (char*)unhex_param.c_str();
			CppUtils::unhexlify(brok_param.size(), brok_param.c_str(), ptr);
			context_params.deserializeXmlParameter(newTradeParams.brokerParams_m,unhex_param);

			engine_m.reloadTradingParametersForTheThread(tid, name, newTradeParams);

		}

		void InQueueController::prepareSimulationTicks(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result)
		{
			// this gets all cached data and based on the simulation profile
			// will prepare ticks for further simulation
			double begindate = command2.getParameter("begin_simulation_time").getAsDate();
			double enddate = command2.getParameter("end_simulation_time").getAsDate();

			CppUtils::String idString = command2.getParameter("id_string").getAsString();


			if (begindate >0)
				begindate /= 1000.0;

			if (enddate >0)
				enddate /= 1000.0;

			BrkLib::SimulationProfileData simProfile;

			simProfile.aggrPeriod_m = (HlpStruct::AggregationPeriods)command2.getParameter("aggr_period").getAsInt();
			simProfile.multFactor_m = command2.getParameter("mult_factor").getAsInt();
			simProfile.genTickMethod_m = (HlpStruct::TickGenerator)command2.getParameter("gen_tick_method").getAsInt();
			simProfile.tickSplitNumber_m = command2.getParameter("ticks_split_number").getAsInt();
			simProfile.tickUsage_m = (HlpStruct::TickUsage)command2.getParameter("tick_usage").getAsInt();

			CppUtils::StringList const& providers = command2.getParameter("provider_list").getAsStringList();
			CppUtils::StringList const& symbols = command2.getParameter("symbol_list").getAsStringList();

			
			// parameters 
			HlpStruct::XmlParameter::XmlParameterList const& sxmlList = command2.getXmlParameterList("symbol_trade_info");
			for(int i = 0; i < sxmlList.size(); i++) {
				HlpStruct::XmlParameter const& sxml = sxmlList[i];

				BrkLib::SymbolTradingInfo tinfo;
				tinfo.comission_m = sxml.getParameter("comission").getAsDouble();
				tinfo.comissionPct_m = sxml.getParameter("comissionPct").getAsDouble();
				tinfo.executePosExactPrices_m = sxml.getParameter("execute_pos_exact_price").getAsInt();
				tinfo.minLimitOrderDistance_m = sxml.getParameter("min_limit_order_distance").getAsDouble();
				tinfo.minStopLossDistance_m = sxml.getParameter("min_stop_loss_distance").getAsDouble();
				tinfo.minStopOrderDistance_m = sxml.getParameter("min_stop_order_distance").getAsDouble();
				tinfo.minTakeProfitDistance_m = sxml.getParameter("min_take_profit_distance").getAsDouble();
				tinfo.slippage_m =sxml.getParameter("simulation_slippage").getAsDouble();
				tinfo.spread_m = sxml.getParameter("simulation_spread").getAsDouble();
				tinfo.signDigits_m = sxml.getParameter("sign_digits").getAsInt();
				tinfo.pointValue_m = sxml.getParameter("point_value").getAsDouble();

				simProfile.symbolsTradingInfo_m[providers[i]][symbols[i]] = tinfo;
				
			}

			// generate candle period here
			if (simProfile.aggrPeriod_m==HlpStruct::AP_Minute) {
				simProfile.simCandlePeriod_m = 1;
			}
			else if (simProfile.aggrPeriod_m==HlpStruct::AP_Hour) {
				simProfile.simCandlePeriod_m = 60;
			}
			else if (simProfile.aggrPeriod_m==HlpStruct::AP_Day) {
				simProfile.simCandlePeriod_m = 60*24;
			}
			else
				THROW(CppUtils::OperationFailed, "exc_InvalidSimulationAggregationPeriod", "ctx_GenerateSimTicks" , (long)simProfile.aggrPeriod_m);
	    
			if (simProfile.multFactor_m <=0)	
				THROW(CppUtils::OperationFailed, "exc_InvalidSimulationMultFactor", "ctx_GenerateSimTicks" , (long)simProfile.multFactor_m);

			simProfile.simCandlePeriod_m *= simProfile.multFactor_m;


			// uid
			CppUtils::Uid uid;
			uid.generate();
			Inqueue::SimulHistoryContext simhistcontext(uid);
			simhistcontext.initStreamFile(); // need to do to allow writing

			BrkLib::BrokerBase::ProviderSymbolCookieMap providerSymbolCookieMap;
			prepareSimulationTicksHelper(simhistcontext, providerSymbolCookieMap, simProfile, begindate, enddate, idString );
			simhistcontext.closeStreamFile();

			// pass uid
			result.getParameter("ticks_uid").getAsUid(true) = uid;


		}

		void InQueueController::queryAvailableSimCaches(HlpStruct::XmlParameter const& command2, HlpStruct::XmlParameter& result)
		{
			vector<SimulHistoryContext::SimulationEntry> entryList;
			SimulHistoryContext::resolveAllSimulationEntries(entryList);

			for(int i = 0; i < entryList.size(); i++) {
				

				HlpStruct::XmlParameter param_i;
				param_i.getParameter("uid").getAsUid(true) = entryList[i].uid_m;
				param_i.getParameter("created_on").getAsDate(true) = entryList[i].created_m * 1000;
				param_i.getParameter("id").getAsString(true) = entryList[i].id_m;

				result.getXmlParameterList("result").push_back(param_i);

			}

			
		}

		void InQueueController::onSelectedAccept(HtClientSocket& clientsocket)
		{
			clientsocket_m = &clientsocket;
			connectionAccepted_m.signalEvent();
		}

		void InQueueController::onAlgBrkEventArrive(HlpStruct::EventData const& sourceEvent, HlpStruct::EventData const& response)
		{

			LOG(MSG_DEBUG, INQUEUECONTROLLER, "onAlgBrkEventArrive(...) - response event type: " << 
				HlpStruct::EventData::getEventTypeName(response.getEventType()) );

				// if cleared
				if (sourceEvent.getEventUid() == response.getParentEventUid()) {
					if (response.getEventType() == HlpStruct::EventData::ET_AlgorithmBrokerEventResp_FromAlgorithm ) {

						LOG(MSG_DEBUG, INQUEUECONTROLLER, "onAlgBrkEventArrive(...) - parent UID match" );

						if (!algLibEventReady_m.lock(0)) {
							// if cleared  - e .i other thread is awaiting

							LOCK_FOR_SCOPE(algLibEventListMtx_m);
							algLibEvent_m = response;

							// signal
							algLibEventReady_m.signalEvent();
							LOG(MSG_DEBUG, INQUEUECONTROLLER, "Event from broker or algorithm to be passed into main command processor thread" );
							

						}
					}
				}
			
			
			
		}
		
}; // end of ns