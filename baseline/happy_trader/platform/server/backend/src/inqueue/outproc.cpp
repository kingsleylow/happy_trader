#include "outproc.hpp"
#include "comproc.hpp"
//#include "engine.hpp"


#define OUTPROC "OUTPROC"
namespace Inqueue {

	// ---------------------------------------------

	// default logging leve
	// TODO- manage as a command
	HlpStruct::CommonLog::LogLevel OutProcessor::eventLevel_m = HlpStruct::CommonLog::LL_DEBUGDETAIL;

	CppUtils::Mutex OutProcessor::mtx_eventLevel_m;

	set<int> OutProcessor::allowedEvents_m;

	CppUtils::Mutex OutProcessor::mtx_allowedEvents_m;

	OutProcessor* OutProcessor::thisPtr_m = NULL;

	// ---------------------------------------------


	//
	int bootLoggerThread (void *ptr)
	{
		// handle run-time exceptions
#ifdef HANDLE_NATIVE_EXCEPTIONS
			Testframe::init();
#endif

		OutProcessor* pThis = (OutProcessor*)ptr; 

		try {
			pThis->threadRun();
		}
		catch(CppUtils::Exception& e) {
			LOG(MSG_FATAL, OUTPROC, "Fatal exception: " << e.message());
		}
#ifdef HANDLE_NATIVE_EXCEPTIONS
		catch(Testframe::Exception& e) {
			LOG(MSG_FATAL, OUTPROC, "Fatal MSVC exception: " << e.message());
		}
#endif
		catch(...) {
			LOG(MSG_FATAL, OUTPROC, "Unknown fatal exception");
		}

		return 42;

	}

	CppUtils::String const & OutProcessor::getRootObjectName () {
		static CppUtils::String string_s("out_processor");
		return string_s;
	}

	OutProcessor::OutProcessor(	):
		thread_m(NULL),
		clientsocket_m(NULL),
		hstContext_m(*CppUtils::getRootObject<HistoryContext>())
	{
		thisPtr_m = this;

		thread_m = new CppUtils::Thread(bootLoggerThread, this);

		// Register with the process manager
		CppUtils::ProcessManager::singleton().registerBootable( *this );

	}
		
	OutProcessor::~OutProcessor()
	{
		LOG( MSG_DEBUG, OUTPROC, "Out sender thread finishing..." );
		
		// reset this lock if therad is still waiting
		//CppUtils::getRootObject<Inqueue::InQueueController>()->resetWaitAcceptIncomingConnectionLock();

	
		shutDownThread_m.signalEvent();
		// need now wait until it starts

		if (!started_m.lock(5000)) {
				LOG( MSG_WARN, OUTPROC, "Will not completely deinitialize Out processor thread as it may be dead-locked because initialization was not finished" );
				return;
		}

		thread_m->waitForThread();

		// Delete thread
		delete thread_m;
		thread_m = NULL;
		LOG( MSG_DEBUG, OUTPROC, "Out sender  thread terminated. Out processor destroyed." );
	}


	void OutProcessor::threadRun()
	{
		// wait until startup phase finishes
		{
			ON_SHUTDOWN(1;);
		}

		
		LOG( MSG_DEBUG, OUTPROC, "Out sender thread started." );
	
		// connect to the server only after command processor starter
		//CppUtils::getRootObject<InQueueController>()->waitUntilAcceptedIncomingConnection();
		
		/*
		if (shutDownThread_m.lock(0)) {
			// shut down signal
			LOG( MSG_DEBUG, OUTPROC, "Out sender thread finished before actual startup" );
			return;
		}
		*/
		
		
		if (connectionAccepted_m.lock()) {

			LOG(MSG_INFO, OUTPROC, "Out sender thread accepted incoming connection");

			// signal we started
			started_m.signalEvent();
			
			//CppUtils::MemoryChunk out_buf;
			HlpStruct::XmlParameter dataToPush;

			HtInquotePacket out_packet;
			
			
			do {
				// Exit?
				//
				try {

						// events to listen
						CppUtils::NativeEvent events[] = 
						{
								shutDownThread_m.getNativeEvent(), 
								dataPushed_m.getNativeEvent()
						};
						
						
					
						// wait some of them
						int res = CppUtils::Event::lockForSomeEvents(events, 2);

						// shutdown signal
						if (res==0)
							break;

						// history simulation
						if (res==1) {
							// data need to be pushed
							
							
							int eventType = -1;
							{
								LOCK_FOR_SCOPE(*this);
								  
								if (!dataToSend_m.isEmpty()) {

									HlpStruct::EventData& evdata = dataToSend_m.get_front();

									
									if (evdata.getEventType() == HlpStruct::EventData::ET_RtProviderTicker) {
										out_packet.flag_m = HtInquotePacket::FLAG_RT_PACKET;
										HlpStruct::convertToBinary(evdata.getAsRtData(), out_packet.data_m );
									}
									else if (evdata.getEventType() == HlpStruct::EventData::ET_Level1Data) {
										out_packet.flag_m = HtInquotePacket::FLAG_LEVEL1_PACKET;
										HlpStruct::convertToBinary(evdata.getAsLevel1Data(), out_packet.data_m );
									}
									else if (evdata.getEventType() == HlpStruct::EventData::ET_Level2Data ) {
										out_packet.flag_m = HtInquotePacket::FLAG_LEVEL2_PACKET;
										HlpStruct::convertToBinary(evdata.getAsLevel2Data(), out_packet.data_m);
									}
									else {
									
										out_packet.flag_m = HtInquotePacket::FLAG_XMLEVENT_PACKET;
										HlpStruct::convertEventToXmlParameter(evdata,dataToPush);
									}
									
									
									eventType = evdata.getEventType();
									dataToSend_m.delete_front();
								};
							
								// extracted
								dataPushed_m.clearEvent();
							};
							
							if (	out_packet.flag_m == HtInquotePacket::FLAG_XMLEVENT_PACKET	) {
								// for these types have complicated serialization
								
								paramsProc_m.serializeXmlParameter(dataToPush, out_packet.data_m);

								//Inqueue::InQueueController::storeErroredContent("shit_1", out_packet.data_m);
								
							}  

							// send the response back
						
							if (writeToSocketHtInquotePacket<HtClientSocket>(*clientsocket_m, out_packet) == false)
									THROW(CppUtils::OperationFailed, "exc_OutProcThreadRun", "ctx_CannotWriteSocket", CTCPSocket::ErrorCodeToString(clientsocket_m->GetLastError()));					

							//LOG(MSG_DEBUG_DETAIL, OUTPROC, "The packet was sent: " << out_packet.toString());
							
					
						}
   
				}
				catch(CppUtils::Exception& e) {
					LOG( MSG_WARN, OUTPROC, "Exception in Out sender thread: " << e.message() << ", packet: " << out_packet.toString());
					CppUtils::sleep(0.5);
				}
	#ifdef HANDLE_NATIVE_EXCEPTIONS
				catch(Testframe::Exception& e) {
					LOG(MSG_FATAL, OUTPROC, "Fatal MSVC exception in Out sender thread: " << e.message() << ", packet: " << out_packet.toString());
					CppUtils::sleep(0.5);
				}
	#endif
				catch(...) {
					LOG( MSG_WARN, OUTPROC, "Unknown Exception in Out sender thread, packet: " << out_packet.toString());
					CppUtils::sleep(0.5);
				}
				

				//ON_SHUTDOWN_IDLE( break; );
			} while(true);

		}

		LOG( MSG_INFO, OUTPROC, "Out sender thread finished." );
		
	}

	void OutProcessor::boot()
	{
			// set allowed events
			setAllowedEvent(HlpStruct::EventData::ET_CommonLog);
			setAllowedEvent(HlpStruct::EventData::ET_AlgorithmEvent);
			setAllowedEvent(HlpStruct::EventData::ET_AlgorithmBrokerEventResp_FromAlgorithm);
			setAllowedEvent(HlpStruct::EventData::ET_AlgorithmBrokerEventResp_FromBroker);
			
			setAllowedEvent(HlpStruct::EventData::ET_BrokerResponseEventResp);
			setAllowedEvent(HlpStruct::EventData::ET_BrokerOrderEvent);
			setAllowedEvent(HlpStruct::EventData::ET_BrokerSessionEvent);
			setAllowedEvent(HlpStruct::EventData::ET_AlgorithmAlert);

			setAllowedEvent(HlpStruct::EventData::ET_DrawableObject);
			setAllowedEvent(HlpStruct::EventData::ET_CustomEvent);

			setAllowedEvent(HlpStruct::EventData::ET_BrokerEvent);
			setAllowedEvent(HlpStruct::EventData::ET_Notification);

		
			
			LOG( MSG_INFO, OUTPROC, "Successfully booted");
	}
	
	void OutProcessor::pushevent(HlpStruct::EventData& eventdata)
	{
		if (Inqueue::OutProcessor::isEventAllowed(eventdata.getEventType())) {	
			eventdata.setEventDate(CppUtils::getTime());	
			Inqueue::OutProcessor::sendEvent(eventdata);	
		}	
	}

	void OutProcessor::pushEventData(HlpStruct::EventData const& eventdata)
	{
		LOCK_FOR_SCOPE(*this);
		dataToSend_m.push_back(eventdata);
	
		dataPushed_m.signalEvent();

	}

	void OutProcessor::setLogLevel(HlpStruct::CommonLog::LogLevel const eventLevel)
	{
		LOCK_FOR_SCOPE(mtx_eventLevel_m);
		eventLevel_m = eventLevel;
	}

	HlpStruct::CommonLog::LogLevel OutProcessor::getLogLevel()
	{
		LOCK_FOR_SCOPE(mtx_eventLevel_m);
		return eventLevel_m;
	}

	void OutProcessor::setAllowedEvent(HlpStruct::EventData::EType const eventtype)
	{
		LOCK_FOR_SCOPE(mtx_allowedEvents_m);
		allowedEvents_m.insert((int)eventtype);
	}

	void OutProcessor::clearAllowedEvent(HlpStruct::EventData::EType const eventtype)
	{
		LOCK_FOR_SCOPE(mtx_allowedEvents_m);
		allowedEvents_m.erase((int)eventtype);
	}

	bool OutProcessor::isEventAllowed(HlpStruct::EventData::EType const eventtype)
	{
		LOCK_FOR_SCOPE(mtx_allowedEvents_m);
		// check whether we are allowing to send this
		set<int>::const_iterator it = allowedEvents_m.find(eventtype);
		return (it!=allowedEvents_m.end());
	}

	void OutProcessor::sendEvent(
			HlpStruct::EventData const& eventdata
		)
	{
		
		thisPtr_m->pushEventData(eventdata);
	}

	void OutProcessor::sendEngineThreadStartedEvent(CppUtils::String const& server, int const tid, CppUtils::String const& algLibPair)
	{
		if (Inqueue::OutProcessor::isEventAllowed(HlpStruct::EventData::ET_Notification)) {	
			HlpStruct::EventData notif_event;	
			notif_event.setEventDate(CppUtils::getTime());
			notif_event.getEventType() = HlpStruct::EventData::ET_Notification;
			notif_event.getAsXmlParameter(true).getParameter("server_id").getAsString(true) = server;
			notif_event.getAsXmlParameter().getParameter("tid").getAsInt(true) = tid;
			notif_event.getAsXmlParameter().getParameter("alg_lib_pair").getAsString(true) = algLibPair;
			
			// type
			notif_event.getAsXmlParameter().getParameter("notification_type").getAsInt(true) = HlpStruct::EventData::ETNT_ThreadStarted;
			Inqueue::OutProcessor::sendEvent(notif_event);
		}
	}

	void OutProcessor::sendEngineThreadFinishedEvent(CppUtils::String const& server, int const tid, CppUtils::String const& algLibPair)
	{
		if (Inqueue::OutProcessor::isEventAllowed(HlpStruct::EventData::ET_Notification)) {	
			HlpStruct::EventData notif_event;	
			notif_event.setEventDate(CppUtils::getTime());
			notif_event.getEventType() = HlpStruct::EventData::ET_Notification;
			notif_event.getAsXmlParameter(true).getParameter("server_id").getAsString(true) = server;
			notif_event.getAsXmlParameter().getParameter("tid").getAsInt(true) = tid;
			notif_event.getAsXmlParameter().getParameter("alg_lib_pair").getAsString(true) = algLibPair;
			
			// type
			notif_event.getAsXmlParameter().getParameter("notification_type").getAsInt(true) = HlpStruct::EventData::ETNT_ThreadFinished;
			Inqueue::OutProcessor::sendEvent(notif_event);
		}
	}

	void OutProcessor::sendSimpleCommonLoggingEvent(char const* context, CppUtils::String const& str, char const* arg)
	{
		LOGEVENT(HlpStruct::CommonLog::LL_INFO, context, str << " - " << (arg ? arg : ""));
	}
	
	void OutProcessor::sendSimpleErrorLoggingEvent(char const* context, CppUtils::String const& str, char const* arg)
	{
		LOGEVENT(HlpStruct::CommonLog::LL_ERROR, context, str << " - " << (arg ? arg : ""));
	}

	void OutProcessor::send_rt_data(
			double const& optime, 
			char const* provider, 
			char const* symbol, 
			double const& bidPrice, 
			double const& askPrice, 
			double const& volume, 
			int const& color
		)
	{
		if (Inqueue::OutProcessor::isEventAllowed(HlpStruct::EventData::ET_RtProviderTicker)) {	
			HlpStruct::EventData print_event;	
			print_event.getEventType() = HlpStruct::EventData::ET_RtProviderTicker;	
			HlpStruct::RtData& rtdata = print_event.getAsRtData(true);	
			rtdata.setProvider(provider);	
			rtdata.setSymbol(symbol);	
			rtdata.getType() = HlpStruct::RtData::RT_Type;	
			rtdata.getTime() = optime;	
			rtdata.getRtData().bid_m = bidPrice;	
			rtdata.getRtData().ask_m = askPrice;	
			rtdata.getRtData().volume_m = volume;	
			rtdata.getRtData().color_m =color;	
			Inqueue::OutProcessor::sendEvent(print_event);	
		}
	}

	void OutProcessor::send_rt_data_operation(
			double const& optime, 
			char const* provider, 
			char const* symbol, 
			double const& bidPrice, 
			double const& askPrice, 
			double const& volume, 
			int const& color, 
			int const& operation)
	{
		if (Inqueue::OutProcessor::isEventAllowed(HlpStruct::EventData::ET_RtProviderTicker)) {	
			HlpStruct::EventData print_event;	
			print_event.getEventType() = HlpStruct::EventData::ET_RtProviderTicker;	
			HlpStruct::RtData& rtdata = print_event.getAsRtData(true);	
			rtdata.setProvider(provider);	
			rtdata.setSymbol(symbol);	
			rtdata.getType() = HlpStruct::RtData::RT_Type;	
			rtdata.getTime() = optime;	
			rtdata.getRtData().bid_m = bidPrice;	
			rtdata.getRtData().ask_m = askPrice;	
			rtdata.getRtData().volume_m = volume;	
			rtdata.getRtData().color_m =color;	
			rtdata.getRtData().operation_m =operation;	
			
			Inqueue::OutProcessor::sendEvent(print_event);	
		}
	}

	void OutProcessor::send_rt_provider_synch(double const& optime, char const* provider, int const& otype, char const* message)
	{
		if (Inqueue::OutProcessor::isEventAllowed(HlpStruct::EventData::ET_RtProviderSynchronizationEvent)) {	
			HlpStruct::EventData synch_event;	
			synch_event.getEventType() = HlpStruct::EventData::ET_RtProviderSynchronizationEvent;	
			synch_event.setEventDate(optime);	
			HlpStruct::XmlParameter& synch_param = synch_event.getAsXmlParameter(true);	
			synch_param.getParameter("server_id").getAsString(true) = Inqueue::InQueueController::resolveSessionName();	
			synch_param.getParameter("provider_name").getAsString(true) = provider;	
			synch_param.getParameter("type").getAsInt(true) = otype;	
			synch_param.getParameter("message").getAsString(true) = (message?message:"");	
			Inqueue::OutProcessor::sendEvent(synch_event);	
		};
	}

	

	void OutProcessor::send_rt_data_ohcl(
			double const& optime, 
			char const* provider, 
			char const* symbol, 
			double const& open, 
			double const& high, 
			double const& low, 
			double const& close, 
			double const& volume)
	{
		if (Inqueue::OutProcessor::isEventAllowed(HlpStruct::EventData::ET_RtProviderTicker)) {
			HlpStruct::EventData print_event;	
			print_event.getEventType() = HlpStruct::EventData::ET_RtProviderTicker;	
			HlpStruct::RtData& rtdata = print_event.getAsRtData(true);	
			rtdata.setProvider(provider);	
			rtdata.setSymbol(symbol);	
			rtdata.getType() = HlpStruct::RtData::HST_Type;	
			rtdata.getTime() = optime;	
			rtdata.getHistData().open_m = open;	
			rtdata.getHistData().high_m = high;	
			rtdata.getHistData().low_m = low;	
			rtdata.getHistData().close_m = close;	
			rtdata.getHistData().open2_m = open;	
			rtdata.getHistData().high2_m = high;	
			rtdata.getHistData().low2_m = low;	
			rtdata.getHistData().close2_m = close;	
			rtdata.getHistData().volume_m = volume;	
			Inqueue::OutProcessor::sendEvent(print_event);	
		}
	}

	void OutProcessor::send_rt_data_ohcl2(
			double const& optime, 
			char const* provider, 
			char const* symbol, 
			double const& open, 
			double const& high, 
			double const& low, 
			double const& close, 
			double const& open2, 
			double const& high2, 
			double const& low2, 
			double const& close2, 
			double const& volume
		)
	{
		if (Inqueue::OutProcessor::isEventAllowed(HlpStruct::EventData::ET_RtProviderTicker)) {
			HlpStruct::EventData print_event;	
			print_event.getEventType() = HlpStruct::EventData::ET_RtProviderTicker;	
			HlpStruct::RtData& rtdata = print_event.getAsRtData(true);	
			rtdata.setProvider(provider);	
			rtdata.setSymbol(symbol);	
			rtdata.getType() = HlpStruct::RtData::HST_Type;	
			rtdata.getTime() = optime;	
			rtdata.getHistData().open_m = open;	
			rtdata.getHistData().high_m = high;	
			rtdata.getHistData().low_m = low;	
			rtdata.getHistData().close_m = close;	
			rtdata.getHistData().open2_m = open2;	
			rtdata.getHistData().high2_m = high2;	
			rtdata.getHistData().low2_m = low2;	
			rtdata.getHistData().close2_m = close2;	
			rtdata.getHistData().volume_m = volume;	
			Inqueue::OutProcessor::sendEvent(print_event);	
		}
	}

	void OutProcessor::send_level1_data(
			double const& optime, 
			char const* provider, 
			char const* symbol, 
			double const& bestbidprice, 
			double const& bestbidvolume, 
			double const& bestaskprice, 
			double const& bestaskvolume,
			double const& open_interest
		)
	{
		if (Inqueue::OutProcessor::isEventAllowed(HlpStruct::EventData::ET_Level1Data)) {	
			HlpStruct::EventData level1_event;	
			level1_event.getEventType() = HlpStruct::EventData::ET_Level1Data;	
			level1_event.setType(HlpStruct::EventData::DT_Level1Data);	
			HlpStruct::RtLevel1Data& level1data = level1_event.getAsLevel1Data(true);	
			level1data.best_ask_price_m =  bestaskprice;	
			level1data.best_bid_price_m =  bestbidprice;	
			level1data.best_ask_volume_m = bestaskvolume;	
			level1data.best_bid_volume_m = bestbidvolume;	
			level1data.open_interest_m = open_interest;
			level1data.setSymbol(symbol);	
			level1data.setProvider(provider);	
			level1data.getTime() = optime;	
			Inqueue::OutProcessor::sendEvent(level1_event);	
		}
	}

	void OutProcessor::send_level2_data( HlpStruct::RtLevel2Data const& level2data)
	{
		if (Inqueue::OutProcessor::isEventAllowed(HlpStruct::EventData::ET_Level2Data)) {
			HlpStruct::EventData level2_event;
			level2_event.getEventType() = HlpStruct::EventData::ET_Level2Data;
			level2_event.getAsLevel2Data(true) = level2data;

			Inqueue::OutProcessor::sendEvent(level2_event);
		}
	}

	void OutProcessor::send_custom_event(HlpStruct::XmlParameter const& xml_param)
	{
		if (Inqueue::OutProcessor::isEventAllowed(HlpStruct::EventData::ET_CustomEvent)) {
			HlpStruct::EventData custom_event;
			custom_event.getEventType() = HlpStruct::EventData::ET_CustomEvent;
			custom_event.getAsXmlParameter(true) = xml_param;
			
			Inqueue::OutProcessor::sendEvent(custom_event);
		}
	}

	
	void OutProcessor::sendLogEvent(
			HlpStruct::EventData::EType const etype, 
			HlpStruct::CommonLog::LogLevel const level, 
			CppUtils::String const& session,
			CppUtils::String const& content,
			CppUtils::String const& context
		)
	{
	
  
		//
		HlpStruct::EventData eventdata;
		eventdata.getEventType() = etype;
		eventdata.setType(HlpStruct::EventData::DT_Log);
			
		HlpStruct::CommonLog& clog = eventdata.getAsLog(true);
		clog.logLevel_m = level;

		// 
		clog.context_m = context;
		clog.logThread_m = CppUtils::getThreadHandle();
			
		//clog.content_m = CppUtils::prepareValidXml(content);
		clog.content_m = content;
		clog.session_m = session;

		thisPtr_m->pushEventData(eventdata);
	}

	
	
	void OutProcessor::onSelectedAccept(HtClientSocket &clientsocket)
	{
		clientsocket_m = &clientsocket;
		BOOL res = clientsocket_m->SetNagle(TRUE);
		if (!res) {
			LOG(MSG_WARN, OUTPROC, "Canot set client socket to NAGLE");
		}
		else {
			LOG(MSG_INFO, OUTPROC, "Set client socket to NAGLE");
		}


		connectionAccepted_m.signalEvent();
	}
	

}; // end of namespace