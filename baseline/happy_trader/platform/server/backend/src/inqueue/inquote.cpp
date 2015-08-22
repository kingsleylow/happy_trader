#include "inquote.hpp"
#include "engine.hpp"
#include "comproc.hpp"

#define QUOTEPROC "QUOTEPROC"
namespace Inqueue {

	int bootThread (void *ptr)
	{
		// handle run-time exceptions
#ifdef HANDLE_NATIVE_EXCEPTIONS
			Testframe::init();
#endif

		QuoteProcessor* pThis = (QuoteProcessor*)ptr; 

		try {
			pThis->threadRun();
		}
		catch(CppUtils::Exception& e) {
			LOG(MSG_FATAL, QUOTEPROC, "Fatal exception: " << e.message());
		}
#ifdef HANDLE_NATIVE_EXCEPTIONS
		catch(Testframe::Exception& e) {
			LOG(MSG_FATAL, QUOTEPROC, "Fatal MSVC exception: " << e.message());
		}
#endif
		catch(...) {
			LOG(MSG_FATAL, QUOTEPROC, "Unknown fatal exception");
		}

		return 42;

	}

	CppUtils::String const & QuoteProcessor::getRootObjectName () {
		static CppUtils::String string_s("quote_processor");
		return string_s;
	}

	QuoteProcessor::QuoteProcessor():
	thread_m(NULL),
	hstContext_m(*CppUtils::getRootObject<HistoryContext>()),
	clientsocket_m(NULL)
	{
		thread_m = new CppUtils::Thread(bootThread, this);

		
		// Register with the process manager
		CppUtils::ProcessManager::singleton().registerBootable( *this );
	}
		
	QuoteProcessor::~QuoteProcessor()
	{
		LOG( MSG_DEBUG, QUOTEPROC, "RT receiving thread finishing..." );
		

		toShutdown_m.signalEvent();
		
		// need tow wait until it starts
		if (!started_m.lock(5000)) {
				LOG( MSG_WARN, QUOTEPROC, "Will not completely deinitialize Quote processor thread as it may be dead-locked because initialization was not finished" );
				return;
		}

		thread_m->waitForThread();

		// Delete thread
		delete thread_m;
		thread_m = NULL;
		LOG( MSG_DEBUG, QUOTEPROC, "RT receiving  thread terminated. Quote processor destroyed." );


	}

	
	bool QuoteProcessor::isAccepted()
	{
		return started_m.lock(0);
	}

	void QuoteProcessor::threadRun()
	{
		
		// wait until startup phase finishes
		{
			ON_SHUTDOWN(1;);
		}

		LOG( MSG_DEBUG, QUOTEPROC, "RT receiving thread started." );
				
		Engine* engine = CppUtils::getRootObject<Engine>();

			
		

		// wait until we have incoming connection
		

		if (connectionAccepted_m.lock()) {
				LOG(MSG_INFO, QUOTEPROC, "RT thread socket accepted incoming connection");

				// signal we started
				started_m.signalEvent();

				
				HtInquotePacket in_packet;
				HlpStruct::RtData rt_data;
				HlpStruct::RtLevel2Data level2_data;
				HlpStruct::RtLevel1Data level1_data;
				HlpStruct::XmlParameter request;
				HlpStruct::EventData eventData;
				bool insideExistingTime;

				

				do {

					// check this flag before any lengthy operation
					if (toShutdown_m.lock(0))
						break;

					try {

						
						// reset buffers
						//in_packet.data_m.length(0);
						request.clear();


						// read from socket RT data
						if (readFromSocketHtInquotePacket<HtClientSocket>(*clientsocket_m, in_packet) == false)
							THROW(CppUtils::OperationFailed, "exc_RtQueueRun", "ctx_CannotReadSocket", CTCPSocket::ErrorCodeToString(clientsocket_m->GetLastError()));					

						//LOG(MSG_DEBUG_DETAIL, QUOTEPROC, in_packet.toString() );
					
										
						if (in_packet.flag_m == HtInquotePacket::FLAG_XMLEVENT_PACKET) {
							
							paramsProc_m.deserializeXmlParameter(request, in_packet.data_m);
							HlpStruct::convertXmlParameterToEvent(eventData, request);

							// have event data here - to all threads like RT data
							// dispatch to all layers
							engine->passThreadEvent(-1, -1, eventData);
							
							
						}
						else if (in_packet.flag_m == HtInquotePacket::FLAG_RT_PACKET) {
							

							
							HlpStruct::convertFromBinary(rt_data, in_packet.data_m );


							// propagate RT data
							hstContext_m.passRtData(rt_data,insideExistingTime);
						
							// propagate
							engine->pushRtData(rt_data, insideExistingTime); 
						}
						else if (in_packet.flag_m == HtInquotePacket::FLAG_LEVEL2_PACKET) {

							// level 2
							HlpStruct::convertFromBinary(level2_data, in_packet.data_m);

							engine->pushLevel2Data( level2_data );

						}
						else if (in_packet.flag_m == HtInquotePacket::FLAG_LEVEL1_PACKET) {
							
							HlpStruct::convertFromBinary(level1_data, in_packet.data_m );

							// TODO
							engine->pushLevel1Data( level1_data );


						}

						
					}
					catch(HlpStruct::XmlParsingError& xmle) {
						CppUtils::String fdump = in_packet.dumpToFile();
						LOG( MSG_ERROR, QUOTEPROC, "XML parser exception in RT queue: " << xmle.message() << " dumped to: " << fdump);
						CppUtils::sleep(0.5);
					}
					catch(CppUtils::Exception& e) {
							LOG( MSG_ERROR, QUOTEPROC, "Exception in RT queue: " << e.message());
							CppUtils::sleep(0.5);
					}
#ifdef HANDLE_NATIVE_EXCEPTIONS
					catch(Testframe::Exception& e) {
						
						LOG(MSG_FATAL, QUOTEPROC, "Fatal MSVC exception in RT queue: " << e.message());
						CppUtils::sleep(0.5);
					}		
#endif
					catch(...) {
							LOG( MSG_ERROR, QUOTEPROC, "Unknown exception in RT queue");
							CppUtils::sleep(0.5);
					}


					// Exit?  
					//
					//ON_SHUTDOWN_IDLE( break; );
				
				} while(true);


				// shutdown of history context
				hstContext_m.shutdown();

		}
	
	
		LOG( MSG_INFO, QUOTEPROC, "RT thread finished." );
	}

	void QuoteProcessor::boot()
	{
			// initialize this
			LOG( MSG_INFO, QUOTEPROC, "Successfully booted");
	}

	
	void QuoteProcessor::onSelectedAccept(HtClientSocket& clientsocket)
	{
		clientsocket_m = &clientsocket;
		connectionAccepted_m.signalEvent();
	}


} // end of namespace