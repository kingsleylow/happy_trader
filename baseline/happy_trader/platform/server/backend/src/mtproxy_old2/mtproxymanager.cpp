#include "mtproxymanager.hpp"
#include "../alglibmetatrader/exchange_packets_base.hpp"
#include "../alglibmetatrader/exchange_packets_mt.hpp"
#include "../alglibmetatrader/exchange_packet_alglib_mt_request.hpp"
#include "../alglibmetatrader/exchange_packet_alglib_mt_response.hpp"




namespace MtProxy {

	HMODULE MtProxyManager::moduleHandle_m  =0;

	atomic<int> BaseQueueElement::sequence_m;

	// Booting function
	int bootServiceThread (void *ptr) {
		// Wait a little
		CppUtils::sleepThread( 500 );

		MtProxyManager* pThis = (MtProxyManager*)ptr; 
		
		pThis->run();
		
		return 42;

	}




	// initialize and deinitialize 
	// need to be a real singleton
	// perform ref counting
	void MtProxyManager::initialize(int const opFlag, MtProxy::MtProxyManager** ppProxymanager, int* refCountPar)
	{
		static CppUtils::Mutex mutex;
		static int refCounted = 0;
		static MtProxy::MtProxyManager* pmPtr = NULL;

		LOCK_FOR_SCOPE(mutex);

		if (opFlag == IF_CREATE) {
			
			if (refCounted == 0) {
				if(!pmPtr) {
					pmPtr = new MtProxy::MtProxyManager();
					LOG(MSG_INFO, MTPROXYMANAGER, "Root object created");
				}
			}

			refCounted++;
		}
		else if (opFlag == IF_FREE){
			// destroy
			if (--refCounted <=0) {
				if(pmPtr) {
					delete pmPtr;
					pmPtr = NULL;

					LOG(MSG_INFO, MTPROXYMANAGER, "Root object fully destroyed");
				}

				refCounted = 0;
			}
		}
		else if (opFlag == IF_RESOLVE){
			//LOG(MSG_DEBUG, MTPROXYMANAGER, "Root object resolved");
		}

		if (refCountPar)
			*refCountPar = refCounted;

	
		if (ppProxymanager)
		*ppProxymanager = pmPtr;
	}



	MtProxyManager::MtProxyManager():
		CppUtils::PipeServer( ),
		thread_m(NULL),
		idle_m(true),
		connectedToPipeServer_m(false),
		hWnd_m(0),
		upperMtWindow_m(0),
		pid_m(-1)
#ifdef IS_DEMO		
		,
		requestCounter_m(0)
#endif
	{  
		settings_m.terminalPath_m = CppUtils::getStartupDir();
		
		if (settings_m.terminalPath_m.length() <=0)
			THROW(CppUtils::OperationFailed, "exc_TerminalPathInvalidName", "ctx_ctorMtProxyManager", "");


		LOG(MSG_INFO, MTPROXYMANAGER, "Ctor: startup dir resolved: " << settings_m.terminalPath_m);

	}

	MtProxyManager::~MtProxyManager()
	{
		// shutdown just in xase

		shutdown();

		LOG(MSG_DEBUG, MTPROXYMANAGER, "Shutdown finished in dtor");

	}


	// startup
	void MtProxyManager::startup(char const* configXml, int hWnd)
	{
		LOCK_FOR_SCOPE(*this);

		if (idle_m) {

			CppUtils::Log::setLogLevel(settings_m.logLevel_m);
			LOG(MSG_INFO, MTPROXYMANAGER, "Adjusted log level to: " << CppUtils::Log::getLogLevel());
			
			// resolve upper MT window

#ifdef IS_DEMO
			requestCounter_m = 0;
			LOG(MSG_DEBUG, MTPROXYMANAGER, "DVR restriction set: " << DEMO_REQUEST_NUMBER );
#endif

			// init all data
			readXMLConfiguration(configXml);

			hWnd_m = (HWND)hWnd;
			if (hWnd_m == 0)
				THROW(CppUtils::OperationFailed, "exc_InvalidMainWindowHandle", "MtProxyManager", "");

			upperMtWindow_m = resolveUpperMtWindow(hWnd_m);
			if (upperMtWindow_m == 0)
				THROW(CppUtils::OperationFailed, "exc_InvalidParentMTWindowHandle", "MtProxyManager", "");

			LOG(MSG_DEBUG, MTPROXYMANAGER,"Parent MT window resolved " << upperMtWindow_m);

			pid_m = ::GetCurrentProcessId();
			LOG(MSG_DEBUG, MTPROXYMANAGER,"PID resolved: " << pid_m);
			
				// service thread
			thread_m = new CppUtils::Thread( bootServiceThread, this );

			// if cannot start thread for 10 seconds
			if (!threadStarted_m.lock(10000))
				THROW(CppUtils::OperationFailed, "exc_CannorStartProcessingThread", "MtProxyManager", "");


			// generate terminal UID
			settings_m.terminalUid_m.generate();


			if (settings_m.serverPipe_m.length() <=0) {
				LOG(MSG_ERROR, MTPROXYMANAGER, "Invalid pipe name");
				return;
			}

			start(false, 1, settings_m.serverPipe_m);
			LOG(MSG_DEBUG, MTPROXYMANAGER, "Proxy manager created and server started pipe listening on: " << settings_m.serverPipe_m << ", terminal UID: " << settings_m.terminalUid_m.toString());

			idle_m = false;

		}
		else {
				LOG(MSG_INFO, MTPROXYMANAGER, "Cannot startup as running");
		}
	}
	
		// shutdown
	void MtProxyManager::shutdown()
	{
			LOCK_FOR_SCOPE(*this);
			if (!idle_m) {

				disconnectFromPipeServer();

				stop();
				LOG(MSG_DEBUG, MTPROXYMANAGER, "Proxy manager is destroying and pipe server stopped");

				if (thread_m) {
					shutDownThread_m.signalEvent();
					thread_m->waitForThread();

					delete thread_m;
					thread_m = NULL;
					LOG(MSG_DEBUG, MTPROXYMANAGER, "Helper thread stopped");
				}

				settings_m.terminalUid_m.invalidate();

				idle_m = true;
			}
			else {
				LOG(MSG_INFO, MTPROXYMANAGER, "Cannot shutdown as idle");
			}
	}


	



	// connect to pipe server
	void MtProxyManager::connectToPipeServer()
	{
		LOCK_FOR_SCOPE(*this);
		

		
		bool result = client_m.connect(settings_m.remotePipe_m);
		if (!result)
			THROW(CppUtils::OperationFailed, "exc_CannotConnectPipe", "ctx_connectToPipeServer", settings_m.remotePipe_m << " - " << CppUtils::getOSError());


		// need to obtain terminal ID
		CppUtils::MemoryChunk request, response;

		//CppUtils::String terminal_uid_str = terminalUid_m.toString();

		AlgLib::MtProxy_AlgLib_Dialog::Mtproxy_Initialize mtproxy_init;
		mtproxy_init.mtUid_m = settings_m.terminalUid_m;
		mtproxy_init.terminalPath_m  = settings_m.terminalPath_m;
		mtproxy_init.pid_m  = pid_m;

		CppUtils::String data_to_send = AlgLib::JsonSerializerHelper()(mtproxy_init);
		LOG(MSG_DEBUG, MTPROXYMANAGER, "On initialization sending data: \n" << data_to_send);
		
		request.fromString(  data_to_send );

		if (!client_m.makePipeTransaction(request, response))
			THROW(CppUtils::OperationFailed, "exc_CannotMakeInitialPipeTransaction", "ctx_connectToPipeServer", settings_m.remotePipe_m << " - " << CppUtils::getOSError());

		if (response.toString() != RETURN_MT_DATA_ACCEPTED)		
			THROW(CppUtils::OperationFailed, "exc_InitialPipeTransactionFailed", "ctx_connectToPipeServer", response.toString());

		connectedToPipeServer_m  = true;
		LOG(MSG_INFO, MTPROXYMANAGER, "Passed terminal path: " << settings_m.terminalPath_m << " and terminal ID: " << mtproxy_init.mtUid_m);
		

		// tries to  load
		if (settings_m.autoLoadFile_m.size() > 0) {
			autoLoadStartUpFile();
			LOG(MSG_INFO, MTPROXYMANAGER, "Autoloaded startup file: " << settings_m.autoLoadFile_m );
		}

	
	}

	// disconnect from pipe serever
	void MtProxyManager::disconnectFromPipeServer()
	{
		LOCK_FOR_SCOPE(*this);

		client_m.disconnect();
		connectedToPipeServer_m= false;
		LOG(MSG_INFO, MTPROXYMANAGER, "Disconnected from remote pipe");
	}

	



	void MtProxyManager::pushDataToPipeServer(char const* cmd, CppUtils::String& resp)
	{
		LOCK_FOR_SCOPE(*this);

		if (!connectedToPipeServer_m) {
			THROW(CppUtils::OperationFailed, "exc_NotConnectedToPipeServer", "ctx_pushDataToPipeServer", "");
		}


		CppUtils::String cmd_s(cmd != NULL ? cmd : "");	

		if (cmd_s.size() <=0)
			THROW(CppUtils::OperationFailed, "exc_CannotMakePipeTransaction", "ctx_pushDataToPipeServer", "Invalid command");

		CppUtils::MemoryChunk request, response;
		request.fromString(cmd_s);
		bool result = client_m.makePipeTransaction(request, response);
		if (!result) {
			connectedToPipeServer_m = false;
			THROW(CppUtils::OperationFailed, "exc_CannotMakePipeTransaction", "ctx_pushDataToPipeServer", CppUtils::getOSError() );
		}

		resp = response.toString();

		if (resp != RETURN_MT_DATA_ACCEPTED)
			THROW(CppUtils::OperationFailed, "exc_InvalidReturnValue", "ctx_pushDataToPipeServer", resp );



	}



	void MtProxyManager::onClientRequestArrived(int const instId, CppUtils::MemoryChunk const& request, CppUtils::MemoryChunk& response)
	{

		// parse request from XML
	
		json::Object response_object;

		CppUtils::RefCountPtr<AlgLib::AlgLib_Event_Dialog_Response::AlgLibResponseBase> respBase = new AlgLib::AlgLib_Event_Dialog_Response::Resp_Dummy();
		CppUtils::String request_s;
		
		try {
			// need to parse command
			 request_s = request.toString();
			if (request_s.size() <=0)
				THROW(CppUtils::OperationFailed, "exc_InvalidInputData", "ctx_onClientRequestArrived", "" );

			LOG(MSG_DEBUG, MTPROXYMANAGER, "Got input data from remote client: \"" << request_s << "\"");


			CppUtils::RefCountPtr<AlgLib::BaseDlgEntity> ptr =  AlgLib::JsonDeserializerHelper<AlgLib::AlgLib_Event_Dialog_Request>()(request_s);
			if (AlgLib::CallIfMatch2<AlgLib::AlgLib_Event_Dialog_Request::Req_REQUEST_MT_STATUS>()(ptr)) {
				CppUtils::RefCountPtr<AlgLib::AlgLib_Event_Dialog_Response::Resp_REQUEST_MT_STATUS> respBase_raw = new AlgLib::AlgLib_Event_Dialog_Response::Resp_REQUEST_MT_STATUS();

				readStatus(respBase_raw->totalQueueSize_m);
				
				respBase= respBase_raw;
			}
			else if (AlgLib::CallIfMatch2<AlgLib::AlgLib_Event_Dialog_Request::Req_UPDATE_USER_ACCESS_TIME>()(ptr)) {
				
				auto const& reqRef = (AlgLib::AlgLib_Event_Dialog_Request::Req_UPDATE_USER_ACCESS_TIME const&)(*ptr);
				for(auto const& user : reqRef.users_m ) {
					updateUsersWithAccessTime(user);
				}
				
				respBase = new AlgLib::AlgLib_Event_Dialog_Response::Resp_UPDATE_USER_ACCESS_TIME();
				
			}
			else if (AlgLib::CallIfMatch2<AlgLib::AlgLib_Event_Dialog_Request::Req_CLEAR_ALL_USERS>()(ptr)) {
				
				clearUserPeriodicalQueue();
				
				respBase = new AlgLib::AlgLib_Event_Dialog_Response::Resp_CLEAR_ALL_USERS();
				
			}
			else if (AlgLib::CallIfMatch2<AlgLib::AlgLib_Event_Dialog_Request::Req_START_ORDER_LIST>()(ptr)) {
				auto const& reqRef = (AlgLib::AlgLib_Event_Dialog_Request::Req_START_ORDER_LIST const&)(*ptr);

				GeneralCommandList gcList;

				// pust data
				for(CppUtils::String const& order: reqRef.orders_m) {
					OrderStruct os(order);
					GeneralCommand gc(os);
				
					gcList.push_back(gc);
				}

				initUserPeriodicalQueue( gcList );

				// response
				respBase = new AlgLib::AlgLib_Event_Dialog_Response::Resp_START_ORDER_LIST();
			}
			else if (AlgLib::CallIfMatch2<AlgLib::AlgLib_Event_Dialog_Request::Req_START_POLLING_USERS>()(ptr)) {
				auto const& reqRef = (AlgLib::AlgLib_Event_Dialog_Request::Req_START_POLLING_USERS const&)(*ptr);
			
				GeneralCommandList gcList;
				for(auto const& user: reqRef.userList_m) {
				
					// update access time
					GeneralCommand gc(UserInfoStruct(user.UserName_m, user.password_m, user.server_m, user.lifeTime_m));

					gc.lastAccessTime() = CppUtils::getTime();

					if (gc.getUserInfo().user_m.size() > 0 && gc.getUserInfo().password_m.size() > 0 && gc.getUserInfo().server_m.size() > 0) {
						gcList.push_back(gc);
						
						LOG(MSG_DEBUG, MTPROXYMANAGER, "Added new user to the queue: " << gc.toString());
						
					}

				}

				initUserPeriodicalQueue( gcList );
				
				respBase = new AlgLib::AlgLib_Event_Dialog_Response::Resp_START_POLLING_USERS();
			
				
			}
			else if (AlgLib::CallIfMatch2<AlgLib::AlgLib_Event_Dialog_Request::Req_REQUEST_POLLING_USER_LIST>()(ptr)) {
				auto const& reqRef = (AlgLib::AlgLib_Event_Dialog_Request::Req_REQUEST_POLLING_USER_LIST const&)(*ptr);
				CppUtils::RefCountPtr<AlgLib::AlgLib_Event_Dialog_Response::Resp_REQUEST_POLLING_USER_LIST> respBase_raw = new AlgLib::AlgLib_Event_Dialog_Response::Resp_REQUEST_POLLING_USER_LIST();
		
				GeneralCommandList userList = readUserPeriodicalQueue();
				for(auto const& user: userList) {
					AlgLib::AlgLib_Event_Dialog_Response::Resp_REQUEST_POLLING_USER_LIST::MtUser user_obj;
					user_obj.lifeTime_m = user.lifetime();
					user_obj.password_m = user.getUserInfo().password_m;
					user_obj.UserName_m = user.getUserInfo().user_m;

					respBase_raw->userList_m.push_back( user_obj );
				}

				respBase= respBase_raw;
			}
			else
				THROW(CppUtils::OperationFailed, "exc_InvalidOperation", "ctx_onClientRequestArrived", "Request: [" + request_s + "]" );

			

		}
		catch(CppUtils::Exception &e) {
			LOG(MSG_ERROR, MTPROXYMANAGER, "Exception on pipe data retrival handler: " << e.message());
			respBase->mt_result_code = -1;
			respBase->mt_result_message = "While performing operation: [" + request_s+ "] ERROR: " + e.message();
		}
		catch(...) {
			LOG(MSG_ERROR, MTPROXYMANAGER, "Unknown exception on pipe data retreival handler ");
			respBase->mt_result_code = -2;
			respBase->mt_result_message = "While performing operation: [" + request_s+ "] UNKNOWN ERROR";
		}

		
		// some default valus
		respBase->mt_instance_uid_m = settings_m.terminalUid_m;
		respBase->mt_instance_name_m = settings_m.instanceName_m;


		CppUtils::String response_s = AlgLib::JsonSerializerHelper()(*respBase);
		response.fromString(response_s);
		LOG(MSG_DEBUG, MTPROXYMANAGER, "Sent response to remote client: \"" << response_s << "\"");

	};

	/*
	bool MtProxyManager::waitForDataAvailable(int tout_msec)
	{

		return dataInPolingQueueAvailable_m.lock(tout_msec);
	}
	*/

	

	void MtProxyManager::getTerminalUid(CppUtils::Uid& termUid) const
	{
		//LOG(MSG_DEBUG, MTPROXYMANAGER, "Returnning terminal UID: " << terminalUid_m);
		termUid = settings_m.terminalUid_m;
	}

	/*
	void MtProxyManager::pushCommandData( Command const& data)
	{
		commandQueue_m.put(data);
	}

	void MtProxyManager::clearCommandQueue()
	{
		commandQueue_m.clear();
	}
		
	// 
	void MtProxyManager::waitForCommandData(int const tout_msec, Command& data, bool& data_found, bool& data_available, bool& is_last)
	{
		static const struct truePredicate {
			bool operator()(Command const&) const
			{
				return true;
			}
		} pr;
		
		// remove element after first usage
		commandQueue_m.front(tout_msec, data, data_found, data_available,is_last, pr);
	}
	*/

	/**
	Helpers
	*/

	void MtProxyManager::readXMLConfiguration(char const* configXml)
	{
		if (configXml == NULL)
			THROW(CppUtils::OperationFailed, "exc_InvalidConfigFile", "ctx_readXMLConfiguration", "" );

		CppUtils::String configXml_s = configXml;
		if (!CppUtils::fileExists(configXml_s))
			THROW(CppUtils::OperationFailed, "exc_FileNotExists", "ctx_readXMLConfiguration", configXml_s );

		try {
			rapidxml::file<> xmlFile(configXml_s.c_str()); // Default template is char
			rapidxml::xml_document<> doc;
			doc.parse<0>(xmlFile.data());

			rapidxml::xml_node<> *settings_node = doc.first_node("settings");
			if (settings_node) {
				// need to resolve serverPipe_m
				// go througl all instances
				rapidxml::xml_node<> *mtinstances_node = settings_node->first_node("mtinstances");
				if (mtinstances_node) {
					// instance
					// iterate
					for (rapidxml::xml_node<> *instance_node = mtinstances_node->first_node(); 
							instance_node != NULL; instance_node = instance_node->next_sibling())
					{
							// get path
							rapidxml::xml_node<> *path_node = instance_node->first_node("path");
							if (path_node) {
								char const* path_c = path_node->value();

								if (!path_c)
									THROW(CppUtils::OperationFailed, "exc_NodeValue_path_Invalid", "ctx_readXMLConfiguration", "" );

								CppUtils::String path_s = path_c;
								LOG(MSG_INFO, MTPROXYMANAGER, "Found the instance path: " << path_s);
								if (CppUtils::compareTwoPaths(path_s, settings_m.terminalPath_m)) {

									// read server pipe
									LOG(MSG_INFO, MTPROXYMANAGER, "Found match on the path: " << path_s );

									rapidxml::xml_node<> *mtpipe_node = instance_node->first_node("mtpipe");
									if (mtpipe_node) {
										char const* serverPipe_c = mtpipe_node->value();

										if (!serverPipe_c)
											THROW(CppUtils::OperationFailed, "exc_NodeValue_mtpipe_Invalid", "ctx_readXMLConfiguration", "" );

										settings_m.serverPipe_m = serverPipe_c;
										LOG(MSG_INFO, MTPROXYMANAGER, "Read from config XML server pipe: " << settings_m.serverPipe_m);
									}
									else
										THROW(CppUtils::OperationFailed, "exc_Node_mtpipe_Invalid", "ctx_readXMLConfiguration", "" );

									// read instnace name
									//instanceName_m

									rapidxml::xml_node<> *name_node = instance_node->first_node("name");
									if (name_node) {
										char const* name_c = name_node->value();

										if (!name_c)
											THROW(CppUtils::OperationFailed, "exc_NodeValue_name_Invalid", "ctx_readXMLConfiguration", "" );

										settings_m.instanceName_m = name_c;
										LOG(MSG_INFO, MTPROXYMANAGER, "Read from config XML instance name: " << settings_m.instanceName_m);
									}
									else
										THROW(CppUtils::OperationFailed, "exc_Node_mtpipe_Invalid", "ctx_readXMLConfiguration", "" );

									
									// auto load 
									rapidxml::xml_node<> *autoload_node = instance_node->first_node("autoload");
									if (autoload_node) {
										// can be NULL
										char const* autoload_c = autoload_node->value();

										settings_m.autoLoadFile_m = autoload_c ? autoload_c : "";
										LOG(MSG_INFO, MTPROXYMANAGER, "Read from config XML - autoload file: " << settings_m.autoLoadFile_m);

										// check if exists
										if (!CppUtils::fileExists(settings_m.autoLoadFile_m))
											THROW(CppUtils::OperationFailed, "exc_AutoLoadFile_Invalid", "ctx_readXMLConfiguration", "" );

									}


								}

							}
							else
								THROW(CppUtils::OperationFailed, "exc_NodeNotFound", "ctx_readXMLConfiguration", "instance/path" );
					}
					

				}
				else
					THROW(CppUtils::OperationFailed, "exc_NodeNotFound", "ctx_readXMLConfiguration", "mtinstances" );


				rapidxml::xml_node<> *servercollectorpipe_node = settings_node->first_node("servercollectorpipe");

				if (servercollectorpipe_node) {
					char const* remotePipe_c = servercollectorpipe_node->value();
					if (!remotePipe_c)
						THROW(CppUtils::OperationFailed, "exc_NodeValue_servercollectorpipe_Invalid", "ctx_readXMLConfiguration", "" );

					settings_m.remotePipe_m = remotePipe_c;
					if (settings_m.remotePipe_m.length() <=0)
						THROW(CppUtils::OperationFailed, "exc_RemotePipeNameInvalid", "ctx_readXMLConfiguration", "");

					LOG(MSG_INFO, MTPROXYMANAGER, "Read from config XML - remote (MT alglib) server pipe value: " << settings_m.remotePipe_m);
				}
				else
					THROW(CppUtils::OperationFailed, "exc_NodeNotFound", "ctx_readXMLConfiguration", "servercollectorpipe" );

				// heartbeatmtproxysec
				rapidxml::xml_node<> *heartbeatmtproxysec_node = settings_node->first_node("heartbeatmtproxysec");
				if (heartbeatmtproxysec_node) {

					char const* heartbeatmtproxysec_c = heartbeatmtproxysec_node->value();
					if (heartbeatmtproxysec_c != NULL) {

						settings_m.heartBeatMtproxySec_m = CppUtils::string2LongDefVal(heartbeatmtproxysec_c);

						if (settings_m.heartBeatMtproxySec_m <= 0)
							settings_m.heartBeatMtproxySec_m = HELPER_THREAD_HEARTBEAT_PERIOD_SEC_DEAFULT;

						LOG(MSG_INFO, MTPROXYMANAGER, "Read from config XML - heartbeat MT proxy to alglibmetatrader sec: " << settings_m.heartBeatMtproxySec_m);
					
					}

				}
				else
					THROW(CppUtils::OperationFailed, "exc_NodeNotFound", "ctx_readXMLConfiguration", "heartbeatmtproxysec" );

				// mt4attemptstoconnect
				rapidxml::xml_node<> *mt4attemptstoconnect_node = settings_node->first_node("mt4attemptstoconnect");
				if (mt4attemptstoconnect_node) {

					
					char const* mt4attemptstoconnect_c = mt4attemptstoconnect_node->value();
					if (mt4attemptstoconnect_c != NULL) {
						settings_m.mt4attemptstoconnect_m = CppUtils::string2LongDefVal(mt4attemptstoconnect_c);
					}
					
					LOG(MSG_INFO, MTPROXYMANAGER, "Read from config XML - MT attempts to connect value: " << settings_m.mt4attemptstoconnect_m);
				}
				else
					THROW(CppUtils::OperationFailed, "exc_NodeNotFound", "ctx_readXMLConfiguration", "mt4attemptstoconnect" );



				// mt4loglevel
				rapidxml::xml_node<> *mt4loglevel_node = settings_node->first_node("mt4loglevel");
				if (mt4loglevel_node) {

					settings_m.logLevel_m = MSG_DEBUG_DETAIL;
					char const* mt4logleve_c = mt4loglevel_node->value();
					if (mt4logleve_c != NULL) {

						settings_m.logLevel_m = CppUtils::string2LongDefVal(mt4logleve_c);

						if (settings_m.logLevel_m < MSG_FATAL)
							settings_m.logLevel_m = MSG_FATAL;
						else if (settings_m.logLevel_m > MSG_DEBUG_DETAIL)
							settings_m.logLevel_m = MSG_DEBUG_DETAIL;
					}
					
				
					LOG(MSG_INFO, MTPROXYMANAGER, "Read from config XML - log level value: " << settings_m.logLevel_m);
				}
				else
					THROW(CppUtils::OperationFailed, "exc_NodeNotFound", "ctx_readXMLConfiguration", "mt4loglevel" );
			}
			else
				THROW(CppUtils::OperationFailed, "exc_NodeNotFound", "ctx_readXMLConfiguration", "settings" );

			

		}
		catch(rapidxml::parse_error &e)
		{
			THROW(CppUtils::OperationFailed, "exc_XMLParseError", "ctx_readXMLConfiguration", e.what() );
		}

		LOG(MSG_INFO, MTPROXYMANAGER, "Successfully read XML config from: " << configXml_s);

	}

	

	void MtProxyManager::sendHeartBeatToHT()
	{
		// must wrap into try catch cause executing in a separate thread

		if (!connectedToPipeServer_m) {
			LOG(MSG_DEBUG, MTPROXYMANAGER, "Still not connected to pipe server");
			return;
		}

		try {
				CppUtils::String terminal_uid_str = settings_m.terminalUid_m.toString();
				

				AlgLib::MtProxy_AlgLib_Dialog::Mtproxy_HeartBeat mtproxy_hb;
				mtproxy_hb.mtUid_m = settings_m.terminalUid_m;
				readStatus(mtproxy_hb.pendingSize_m);
			

				CppUtils::String response_s;
				
				
				pushDataToPipeServer(AlgLib::JsonSerializerHelper()(mtproxy_hb).c_str(), response_s);

				LOG(MSG_DEBUG, MTPROXYMANAGER, "Heartbeat sent, result is " << response_s);

		}
		catch(CppUtils::Exception& e) {
			LOG(MSG_ERROR, MTPROXYMANAGER, "Error sending heartbeat: " << e.message());
		}
		catch(...) {
			LOG(MSG_ERROR, MTPROXYMANAGER, "Unknown error sending heartbeat");
		}


	}


	// update all data
	void MtProxyManager::initUserPeriodicalQueue(GeneralCommandList const& gcList)
	{
		// need to update access time

		dataQueue_m.put(gcList);
		LOG(MSG_DEBUG, MTPROXYMANAGER, "Initialized user periodical queue with: " << gcList.size() << " entries");
	}




	void MtProxyManager::readStatus(int& totalQueueSize) const
	{
		totalQueueSize = dataQueue_m.size();
	}

	void MtProxyManager::updateUsersWithAccessTime(CppUtils::String const& user)
	{
		auto func = [&] (GeneralCommand& userinfo) {
			if (userinfo.equalKey(user))
						userinfo.lastAccessTime() = (int)CppUtils::getTime();
		};
		
		
		dataQueue_m.iterateThroughElementsSingleElement(func);

	}

	void MtProxyManager::updateAllUsersWithAccessTime()
	{
		auto func = [] (list<GeneralCommand>::iterator & beg_it,list<GeneralCommand>::iterator & end_it) {
			for(auto it = beg_it; it != end_it; it++) {
				it->lastAccessTime() = (int)CppUtils::getTime();
			}
		};

		dataQueue_m.iterateThroughElementsAllElements(func);
	}

	// clear data
	void MtProxyManager::clearUserPeriodicalQueue()
	{

		dataQueue_m.clear();

		LOG(MSG_INFO, MTPROXYMANAGER, "Cleared user periodical queue");
	}

	// get data
	GeneralCommandList MtProxyManager::readUserPeriodicalQueue() const
	{

		GeneralCommandList datalist;
		dataQueue_m.getPendingData(datalist);

		LOG(MSG_INFO, MTPROXYMANAGER, "Returning user periodical queue, currently: " << datalist.size() << " entries");

		return datalist;
	}

	void MtProxyManager::waitForUserPeriodicalQueueData(
		int const tout_msec, 
		GeneralCommand& data, 
		bool& data_found, 
		bool& data_available,
		bool& is_last)
	{

		checkForRemovals_front_if pr;
		dataQueue_m.front(tout_msec, data, data_found, data_available,is_last, pr);

	
#ifdef IS_DEMO
		if (DEMO_REQUEST_NUMBER > 0) {
			
			if ((requestCounter_m % 100) == 0) {
					LOG(MSG_DEBUG, MTPROXYMANAGER, "DVR: " << requestCounter_m );
			}
			
			if (requestCounter_m++ > DEMO_REQUEST_NUMBER) {

				LOG(MSG_FATAL, MTPROXYMANAGER, "Trying to close the instance, reason: DVR");
				//closeInstance();
			}
		}
#endif

	} 

	

	void MtProxyManager::run()
	{
		threadStarted_m.signalEvent();
		LOG(MSG_INFO, MTPROXYMANAGER,"Helper  thread started");

		int sleep_msec = HELPER_THREAD_SLEEP_PERIOD_SEC * 1000;
		int heartbeat_sent = CppUtils::getTime();
		CppUtils::NativeEvent events[] = 
		{
			shutDownThread_m.getNativeEvent()				// 0	
		};

		while(true) {


			try {
				// wait some of them 
				int res = CppUtils::Event::lockForSomeEvents(events, 1, sleep_msec);

				if (res==0) {
					// shutdown
					break;
				}

				// check if we need to clean up queues
				checkForRemovals_remove_if pr;
				int removed_cnt = dataQueue_m.checkForRemovals<checkForRemovals_remove_if>( pr );
				LOG(MSG_DEBUG, MTPROXYMANAGER, "Checked if queue has stale data, removed: " << removed_cnt << ", total size is: " <<  dataQueue_m.size());

				// need to send update to HT layer about how busy we are
				
			
				if ( connectSpan_m.exceededTime(MAX_MT_CONNECT_TIME_SEC)) {
					LOG(MSG_INFO, MTPROXYMANAGER, "Main MT thread is hanging more than: " << MAX_MT_CONNECT_TIME_SEC << ", suiciding...");
					LOG(MSG_FATAL, MTPROXYMANAGER,"Terminal: " << settings_m.instanceName_m << " is aborting");

					CppUtils::sleep(35);
				}

				if (CppUtils::getTime() >= heartbeat_sent + settings_m.heartBeatMtproxySec_m) {
					sendHeartBeatToHT();
					heartbeat_sent = CppUtils::getTime();
				}
				


			}
			catch(CppUtils::Exception& e) {
				LOG(MSG_ERROR, MTPROXYMANAGER, "Error in helper thread: " << e.message());
			}
			catch(...) {
				LOG(MSG_ERROR, MTPROXYMANAGER, "Unknown error in event processing thread");
			}

		}

		LOG(MSG_INFO, MTPROXYMANAGER,"Helper  thread finished");
	}

	HWND MtProxyManager::resolveUpperMtWindow(HWND const chartHwnd) const
	{
		HWND curHwnd = chartHwnd;

		while(true) {
			HWND newHwnd = ::GetParent(curHwnd);

			if (newHwnd==NULL)
				break;

			curHwnd = newHwnd;
		}

		return curHwnd;

	}

	void MtProxyManager::autoLoadStartUpFile()
	{
		//settings_m.autoLoadFile_m
		// user, password, sevver
		GeneralCommandList gclist;

		CppUtils::String content;
		CppUtils::readContentFromFile2(settings_m.autoLoadFile_m, content);
		if (content.size() < 0)
			return;
		// parse by carriage retiurn

		CppUtils::StringList rows;
		CppUtils::StringList line;
		CppUtils::tokenize(content, rows, "\r\n" );
		for(int i = 0; i < rows.size(); i++) {
			CppUtils::String & row_i = rows[i];

			// parse row
			CppUtils::trimBoth(row_i);
			CppUtils::tokenize(row_i,line, "," );
			if (line.size() != 4)
				THROW(CppUtils::OperationFailed, "exc_InvalidLineFormat", "ctx_autoLoadStartUpFile", row_i );

			CppUtils::String const& user_i = line[0];
			CppUtils::String const& pass_i = line[1];
			CppUtils::String const& srv_i = line[2];
			int tout_i = atoi(line[3].c_str());
			
			LOG(MSG_DEBUG, MTPROXYMANAGER, "Adding: " << user_i << "/***/" << srv_i << "/" << tout_i);
			GeneralCommand gc(UserInfoStruct(user_i, pass_i, srv_i, tout_i) );
			gclist.push_back( gc );
		}

		initUserPeriodicalQueue( gclist );


	}
	
	void MtProxyManager::callConnectForm(char const* user, char const* password, char const* server)
	{
	
		connectSpan_m.start();

		bool sucess = false;

		static const int OPENLOGINWINDOW_COMMAND_CODE  = 35429;

		// SendMessage - freeze until thi released
		PostMessage(upperMtWindow_m,WM_COMMAND,OPENLOGINWINDOW_COMMAND_CODE,0);
		CppUtils::sleep(1.0);
		int cnt = 0;

		while(!sucess) {
			HWND dialog_login_hwnd=::GetLastActivePopup(upperMtWindow_m);  
		
			if (dialog_login_hwnd != NULL) {
				LOG(MSG_DEBUG, MTPROXYMANAGER,"callConnectForm(..) - last active popup resolved");

			
				HWND hlogin_edit=::GetDlgItem(::GetDlgItem(dialog_login_hwnd,0x49D),0x3E9);
				HWND hlogin_password=::GetDlgItem(dialog_login_hwnd,0x4C4);
				HWND hlogin_server=::GetDlgItem(GetDlgItem(dialog_login_hwnd,0x50D),0x3E9);
		
				// button
				HWND hbutton_login=::GetDlgItem(dialog_login_hwnd,0x1);

				if (hlogin_edit != NULL && hlogin_password != NULL && hlogin_server != NULL && hbutton_login != NULL) {
					LOG(MSG_DEBUG, MTPROXYMANAGER,"callConnectForm(..) - all controls resolved");

					// updating controls
					::SendMessage(hlogin_edit,WM_CLEAR,0,0);
					::SetWindowText(hlogin_edit,user);

					::SendMessage(hlogin_password,WM_CLEAR,0,0);
					::SetWindowText(hlogin_password,password);

					::SendMessage(hlogin_server,WM_CLEAR,0,0);
					::SetWindowText(hlogin_server,server);

					::SendMessage(hbutton_login,BM_CLICK,0,0);
					
					CppUtils::sleep(1.0);
					sucess = true;
				
					LOG(MSG_DEBUG, MTPROXYMANAGER,"callConnectForm(..) - OK pressed");
					break;

				}
			
			} else {
				LOG(MSG_DEBUG, MTPROXYMANAGER,"callConnectForm(..) - popup not resolved - returning...");
				break;
			}

		 

			//if (true) {
			//	if (true) {
			if(!sucess) {
				if (++cnt >= ATTEMPT_TO_CALLMT4LOGIN) {
					LOG(MSG_ERROR, MTPROXYMANAGER,"callConnectForm(..) - No sucess - exceeded max attempt number. Suiciding. Terminal: " << settings_m.instanceName_m);
					LOG(MSG_FATAL, MTPROXYMANAGER,"Terminal: " << settings_m.instanceName_m << " is aborting");
					
					// sleeping to avoid other activity
					CppUtils::sleep(35);
					//abort();

					break;
				}

				LOG(MSG_DEBUG, MTPROXYMANAGER,"callConnectForm(..) - No sucess - waiting...");
				CppUtils::sleep(1.0);
			}
		
		}// loop
		
		connectSpan_m.stop();

	}

	// close MT instnace
	void MtProxyManager::closeInstance()
	{
		// try to slose 
		PostMessage(hWnd_m, WM_CLOSE, 0, 0);
	}


}; // end of namespace