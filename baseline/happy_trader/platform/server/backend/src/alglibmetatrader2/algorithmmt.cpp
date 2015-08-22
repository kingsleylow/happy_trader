#include "algorithmmt.hpp"
#include "../inqueue/commontypes.hpp"
#include "../math/math.hpp"
#include "shared.hpp"
#include "settingsmanager.hpp"





//#include <sstream>

#define ALG_MT "ALG_MT2"



namespace AlgLib {


	int bootMTReaderThread (void *ptr)
	{
#ifdef HANDLE_NATIVE_EXCEPTIONS
			Testframe::init();
#endif
			
		AlgorithmMt* pThis = (AlgorithmMt*)ptr; 
		try {
			pThis->mtReaderThreadRun();
		}
		catch(CppUtils::Exception& e) {
			LOG(MSG_FATAL, ALG_MT, "Fatal exception: " << e.message());
		}
#ifdef HANDLE_NATIVE_EXCEPTIONS
		catch(Testframe::Exception& e) {
			LOG(MSG_FATAL, ALG_MT, "Fatal MSVC exception: " << e.message());
		}
#endif
		catch(...) {
			LOG(MSG_FATAL, ALG_MT, "Unknown fatal exception");
		}
		return 42;

	}


	/**
	* functors
	*/

	
		QueuePurgeFunctor::QueuePurgeFunctor(AlgorithmMt& base, int const checkInterval):
			base_m(base),
			checkInterval_m(checkInterval)
		{
		}

		double QueuePurgeFunctor::operator() () const
		{
			base_m.workerThreadManager_m.checkQueuesforOldData();
			return checkInterval_m;
		}

	

	//------------------------
	/*
	ReadMTContentFunctor::ReadMTContentFunctor(AlgorithmMt& base, int const checkInterval):
		base_m(base),
		checkInterval_m(checkInterval)
	{
	}
	
	double ReadMTContentFunctor::operator() () const
	{
			base_m.processMTOutput();
			
			return checkInterval_m;
	}
	*/

	// ------------------

	ZombieKillerFunctor::ZombieKillerFunctor(AlgorithmMt& base, int const checkInterval):
		base_m(base),
		checkInterval_m(checkInterval)
	{
	}
	
	double ZombieKillerFunctor::operator() () const
	{
			base_m.killZombieMts();
			return checkInterval_m;
	}

	// -------------------
	HeartBeatFunctor::HeartBeatFunctor(AlgorithmMt& base):
		base_m(base),
		heartBeatInterval_m(base.settingsManager_m.getSettings().heartBeatJavaHTSec_m)
	{
	}

	double HeartBeatFunctor::operator() () const
	{
		base_m.sendHeartBeat();
		return heartBeatInterval_m;
	}


	// ---------------
	/*
	MTConnectorCheckerFunctor::MTConnectorCheckerFunctor(AlgorithmMt& base):
		base_m(base)
	{
	}

	double MTConnectorCheckerFunctor::operator() () const
	{
		base_m.workerThreadManager_m.checkMtConection();
		return base_m.settingsManager_m.getSettings().heartBeatMtproxySec_m;
	}
	*/
	
	

	//static int id_m = 0;

	AlgorithmBase *creator (Inqueue::AlgorithmHolder* algholder)
	{
		return new AlgorithmMt(*algholder);
	}

	void terminator (AlgorithmBase *pthis)
	{
		HTASSERT(pthis);
		delete pthis;
	}

	AlgorithmBase::CreatorAlgorithmFun createAlgorithm = creator;

	AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm = terminator;


	// intreface to be implemented

	AlgorithmMt::AlgorithmMt( Inqueue::AlgorithmHolder& algHolder):
		AlgorithmBase(algHolder),
		brokerConnect_m(NULL),
		//connCheckerFunctor_m(NULL),
		workerThreadManager_m(settingsManager_m, *this),
		settingsManager_m(*this),
		helperLoggerP_m(NULL),
		thread_m(NULL)
	{

	}

	AlgorithmMt::~AlgorithmMt()
	{
		
	}



	void AlgorithmMt::onLibraryInitialized(
		HlpStruct::AlgBrokerDescriptor const& descriptor, 
		BrkLib::BrokerBase* brokerlib, 
		BrkLib::BrokerBase* brokerlib2,
		CppUtils::String const& runName,
		CppUtils::String const& comment
		)
	{
		LOG(MSG_INFO, ALG_MT, "Library initialized");
		
		
	}

	void AlgorithmMt::onLibraryFreed()
	{

		LOG(MSG_INFO, ALG_MT, "Library deinitialized");
	}

	void AlgorithmMt::onBrokerResponseArrived(
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
		)
	{

	}


	void AlgorithmMt::onDataArrived(
		HlpStruct::RtData const& rtdata, 
		Inqueue::ContextAccessor& historyaccessor, 
		HlpStruct::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
		)
	{

	}    

	void AlgorithmMt::onRtProviderSynchEvent (
		Inqueue::ContextAccessor& contextaccessor,
		HlpStruct::RtData::SynchType const synchFlg,
		double const& synchTime,
		CppUtils::String const& providerName,
		CppUtils::String const& message
		)
	{
	}


	void AlgorithmMt::onLevel2DataArrived(
		HlpStruct::RtLevel2Data const& level2data,
		Inqueue::ContextAccessor& historyaccessor, 
		HlpStruct::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
		)
	{
	}

	void AlgorithmMt::onLevel1DataArrived(
		HlpStruct::RtLevel1Data const& level1data,
		Inqueue::ContextAccessor& historyaccessor, 
		HlpStruct::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
		)
	{



	}



	void AlgorithmMt::onEventArrived(
		Inqueue::ContextAccessor& contextaccessor, 
		HlpStruct::CallingContext& callContext,
		bool& result,
		CppUtils::String &customOutputData
		)
	{
		// set as OK

		// later will be reassigned if we get another response
		CppUtils::RefCountPtr<AlgLib_Event_Dialog_Response::AlgLibResponseBase> respBase = new AlgLib_Event_Dialog_Response::Resp_Dummy();


		try {


			CppUtils::String command = callContext.getCallCustomName();
			CppUtils::toupper(command);

			LOG(MSG_DEBUG, ALG_MT, "Command received: " << command);

			CppUtils::String const& message = callContext.getParameter<BrkLib::String_Proxy>("custom_data").get();


			// new approach
			CppUtils::RefCountPtr<BaseDlgEntity> ptr =  JsonDeserializerHelper<AlgLib_Event_Dialog_Request>()(message, command);


			if (CallIfMatch2<AlgLib_Event_Dialog_Request::Req_G_GET_ALL_METADATA>()(ptr)) {
		
				// get all metadata
				respBase =  n_processArrivedEvent_G_GET_ALL_METADATA(static_cast<AlgLib_Event_Dialog_Request::Req_G_GET_ALL_METADATA const&>(*ptr));

			}
			else if (CallIfMatch2<AlgLib_Event_Dialog_Request::Req_G_QUERY_AUTOLOAD_USERS>()(ptr)) {
				respBase =  n_processArrivedEvent_G_QUERY_AUTOLOAD_USERS(static_cast<AlgLib_Event_Dialog_Request::Req_G_QUERY_AUTOLOAD_USERS const&>(*ptr));

			}
			else if (CallIfMatch2<AlgLib_Event_Dialog_Request::Req_G_KILL_MT_PROCESS>()(ptr)) {
				THROW(CppUtils::OperationFailed, "exc_NotImplemented", "ctx_Req_G_KILL_MT_PROCESS", "");
			}
			else if (CallIfMatch2<AlgLib_Event_Dialog_Request::Req_START_ORDER_LIST>()(ptr)) {
				
				// push orders to specified instance - not very used so far
				respBase = n_processArrivedEvent_START_ORDER_LIST(static_cast<AlgLib_Event_Dialog_Request::Req_START_ORDER_LIST const&>(*ptr));

			}
			else if (CallIfMatch2<AlgLib_Event_Dialog_Request::Req_START_POLLING_USERS>()(ptr)) {
				// start poling user
				// let's resolve MT instance
				
				respBase = n_processArrivedEvent_START_POLLING_USERS(static_cast<AlgLib_Event_Dialog_Request::Req_START_POLLING_USERS const&>(*ptr));

			}
			else if (CallIfMatch2<AlgLib_Event_Dialog_Request::Req_REQUEST_POLLING_USER_LIST>()(ptr)) {
				respBase = n_processArrivedEvent_REQUEST_POLLING_USER_LIST(static_cast<AlgLib_Event_Dialog_Request::Req_REQUEST_POLLING_USER_LIST const&>(*ptr));
				
			}
			else if (CallIfMatch2<AlgLib_Event_Dialog_Request::Req_CLEAR_ALL_USERS>()(ptr)) {

				respBase = n_processArrivedEvent_CLEAR_ALL_USERS(static_cast<AlgLib_Event_Dialog_Request::Req_CLEAR_ALL_USERS const&>(*ptr));
		
			}
			else if (CallIfMatch2<AlgLib_Event_Dialog_Request::Req_REQUEST_MT_STATUS>()(ptr)) {
				respBase = n_processArrivedEvent_REQUEST_MT_STATUS(static_cast<AlgLib_Event_Dialog_Request::Req_REQUEST_MT_STATUS const&>(*ptr));
			}
			else if (CallIfMatch2<AlgLib_Event_Dialog_Request::Req_UPDATE_USER_ACCESS_TIME>()(ptr)) {
				respBase = n_processArrivedEvent_UPDATE_USER_ACCESS_TIME(static_cast<AlgLib_Event_Dialog_Request::Req_UPDATE_USER_ACCESS_TIME const&>(*ptr));

			}
			else
				THROW(CppUtils::OperationFailed, "exc_InvalidCommand", "ctx_onEventArrived", command);

			// these events must be send to MT layer as it is
			//auto const& ptrBaseRef = (AlgLib_Event_Dialog_Request::AlgLibRequestBase const&)(*ptr);
			
			
			//if (respBase->mt_instance_name_m.size() > 0) {
			//	auto& it = settingsManager_m.getSettings().mtSettingsMap_m.find(respBase->mt_instance_name_m);
			//	if (it != settingsManager_m.getSettings().mtSettingsMap_m.end()) {
			//		respBase->mt_instance_uid_m = it->second.getStartUpSettings().;
			//	}
			//}

			// resolve mt instance code
			//if (respBase->mt_instance_uid_m.isValid() && respBase->mt_instance_name_m.size() <= 0) {
			//	THROW(CppUtils::OperationFailed, "exc_InvalidInstanceNameButUidValid", "ctx_onEventArrived", "MT UID: " <<respBase->mt_instance_uid_m.toString() );
			//}

		

		}
		catch(CppUtils::Exception& e)
		{

			respBase->result_message = "Exception: [" + e.message() + "] - [" + e.context() + "] [" + e.arg() + "]";
			respBase->result_code = -1;
		}
		catch(json::Exception& ejson)
		{
			respBase->result_message = "Exception: [" + CppUtils::String(ejson.what()) + "]";
			respBase->result_code = -2;
		}
#ifdef HANDLE_NATIVE_EXCEPTIONS
		catch(Testframe::Exception& e) {
			respBase->result_message = "MSVC Exception: [" + e.message() + "]";
			respBase->result_code = -3;
		}
#endif
		catch(...)
		{

			respBase->result_message = "Unknown exception";
			respBase->result_code = -4;
		} 

		customOutputData  = JsonSerializerHelper()(*respBase);

		LOG(MSG_DEBUG_DETAIL, ALG_MT, "Returning:\n " << customOutputData);



		result = false;
	}




	
	



	void AlgorithmMt::sendMTConnectStatus(bool const connected, CppUtils::String const& mtname)
	{
		// serialize all
		HlpStruct::EventData response;
		response.getEventType() = HlpStruct::EventData::ET_CustomEvent;
		response.setType(HlpStruct::EventData::DT_CommonXml);

		HlpStruct::XmlParameter& respxml = response.getAsXmlParameter( true );

		Alglib_Out_Dialog::MtStatusOut out_packet_obj;
		out_packet_obj.mtInstance_m = mtname;

		out_packet_obj.status_m = (connected ? Alglib_Out_Dialog::MtStatusOut::MT_CONNECTED : Alglib_Out_Dialog::MtStatusOut::MT_DISCONNECTED);

		CppUtils::String data_to_send = JsonSerializerHelper()(out_packet_obj);
		respxml.getParameter("data").getAsString(true) = data_to_send;

		respxml.getName() = MT4_CONNECT_STATUS_EVENT;
		Inqueue::OutProcessor::pushevent(response);

		LOG(MSG_DEBUG_DETAIL, ALG_MT, "Connect event sent: " << data_to_send );  


	}

	void AlgorithmMt::sendRawDataFromMT(MtProxy_AlgLib_Dialog::Mtproxy_Data const& mtproxydata) const
	{
		// serialize all
		HlpStruct::EventData response;
		response.getEventType() = HlpStruct::EventData::ET_CustomEvent;
		response.setType(HlpStruct::EventData::DT_CommonXml);

		HlpStruct::XmlParameter& respxml = response.getAsXmlParameter( true );

		Alglib_Out_Dialog::MtDataPacketOut out_packet_obj;
		out_packet_obj.mtData_m = mtproxydata;

		CppUtils::String data_to_send = JsonSerializerHelper()(out_packet_obj);
		respxml.getParameter("data").getAsString(true) = data_to_send;

		respxml.getName() = MT4_DELIVERY_ENVELOPE_EVENT;
		Inqueue::OutProcessor::pushevent(response);

		LOG(MSG_DEBUG_DETAIL, ALG_MT, "Data sent to HT layer: " << data_to_send );  


	}

	void AlgorithmMt::onThreadStarted(Inqueue::ContextAccessor& contextaccessor, bool const firstLib, bool const lastLib)
	{	
		LOG(MSG_DEBUG, ALG_MT, "Faster version is initializing..." );

		// start reader thread
		thread_m = new CppUtils::Thread(bootMTReaderThread, this);

		// if cannot start thread for 10 seconds
		if (!threadStarted_m.lock(10000))
			THROW(CppUtils::OperationFailed, "exc_CannorStartThread", "ctx_CreateEngineThread", "");

		//
		//THROW(CppUtils::OperationFailed, "foo", "foo", "foo");
	
		// after that point no changes of settings
		settingsManager_m.parse( GET_ALG_TRADE_STRING(getAlgHolder().getAlgBrokerDescriptor().initialAlgBrkParams_m, "SETTING_FILE_PATH"));

		if (settingsManager_m.getSettings().allowdebugEvents_m) {
			// init console if needed
			helperLoggerP_m = new CConsoleLoggerEx();
			if (helperLoggerP_m->Create(getName().c_str(), 200, 80, NULL, DEFAULT_HELPER_EXE) <0) {

				LOG(MSG_WARN, ALG_MT, "Helper logger cannot be initialized" );

				delete helperLoggerP_m;
				helperLoggerP_m = NULL;
			}
			else {
				LOG(MSG_DEBUG, ALG_MT, "Debug console will be created" );
			}
		}

		LOG(MSG_DEBUG, ALG_MT, "Going to install mt..." );
		settingsManager_m.installMt();

		// no need to start

		workerThreadManager_m.startAllWork();

		
		// init heart beats 
		functor_m = new HeartBeatFunctor(*this);
		CppUtils::getRootObject<CppUtils::ServiceThread>()->registerServiceFunction(functor_m, settingsManager_m.getSettings().heartBeatJavaHTSec_m);
		LOG(MSG_DEBUG, ALG_MT, "Registered heartbeat functor with period: " << settingsManager_m.getSettings().heartBeatJavaHTSec_m << " sec(s)" );
		 
		// create mt connection checker hertbeat functor
		
		/*
		connCheckerFunctor_m = new MTConnectorCheckerFunctor( *this );
		CppUtils::getRootObject<CppUtils::ServiceThread>()->registerServiceFunction(connCheckerFunctor_m, settingsManager_m.getSettings().heartBeatMtproxySec_m);
		LOG(MSG_DEBUG, ALG_MT, "Registered connection/MT thread health checker functor with period: " << settingsManager_m.getSettings().heartBeatMtproxySec_m << " sec(s)" );
		*/

		queuesPurgeFunctor_m = new QueuePurgeFunctor(*this, REMOVE_OLD_MESSAGE_CHECK_PERIOD_SEC);
		CppUtils::getRootObject<CppUtils::ServiceThread>()->registerServiceFunction(queuesPurgeFunctor_m, queuesPurgeFunctor_m->getCheckInterval());
		LOG(MSG_DEBUG, ALG_MT, "Registered stale queue data checker functor with period: " << queuesPurgeFunctor_m->getCheckInterval() << " sec(s)" );

		/*
		readMtDataFunctor_m = new ReadMTContentFunctor(*this, settingsManager_m.getSettings().mt4datareadintrvalsec_m);
		CppUtils::getRootObject<CppUtils::ServiceThread>()->registerServiceFunction(readMtDataFunctor_m, readMtDataFunctor_m->getCheckInterval());
		LOG(MSG_DEBUG, ALG_MT, "Registered read MT data  functor with period: " << readMtDataFunctor_m->getCheckInterval() << " sec(s)" );
		*/
		zombieMt4KillerFunctor_m = new ZombieKillerFunctor(*this, settingsManager_m.getSettings().zombiemt4killersec_m);
		CppUtils::getRootObject<CppUtils::ServiceThread>()->registerServiceFunction(zombieMt4KillerFunctor_m, zombieMt4KillerFunctor_m->getCheckInterval());
		LOG(MSG_DEBUG, ALG_MT, "Registered zombie MT killer functor with period: " << zombieMt4KillerFunctor_m->getCheckInterval() << " sec(s)" );
	};

	void AlgorithmMt::onThreadFinished(	Inqueue::ContextAccessor& contextaccessor, HlpStruct::TradingParameters const& tradeParams, bool const firstLib, bool const lastLib)
	{
		if (zombieMt4KillerFunctor_m) {
			CppUtils::getRootObject<CppUtils::ServiceThread>()->unregisterServiceFunction(zombieMt4KillerFunctor_m->getUid());
			zombieMt4KillerFunctor_m = NULL;
		}

		if (queuesPurgeFunctor_m) {
			CppUtils::getRootObject<CppUtils::ServiceThread>()->unregisterServiceFunction(queuesPurgeFunctor_m->getUid());
			queuesPurgeFunctor_m = NULL;
		}
		
		/*
		if (readMtDataFunctor_m) {
			CppUtils::getRootObject<CppUtils::ServiceThread>()->unregisterServiceFunction(readMtDataFunctor_m->getUid());
			readMtDataFunctor_m = NULL;
		}
		*/

		if (functor_m) {
			CppUtils::getRootObject<CppUtils::ServiceThread>()->unregisterServiceFunction(functor_m->getUid());
			functor_m = NULL;
		}
		
		/*		
		if (connCheckerFunctor_m) {
			CppUtils::getRootObject<CppUtils::ServiceThread>()->unregisterServiceFunction(connCheckerFunctor_m->getUid());
			connCheckerFunctor_m = NULL;
		}
		*/
		

		
		// finishing all threads
		workerThreadManager_m.finishAllWork();

		if (helperLoggerP_m) {
			delete helperLoggerP_m;
			helperLoggerP_m = NULL;
		}

		shutDownThread_m.signalEvent();
		thread_m->waitForThread();

		// Delete thread
		delete thread_m;
		thread_m = NULL;
		LOG( MSG_DEBUG, ALG_MT, "MT data reader thread terminated" );
	}

	CppUtils::String const& AlgorithmMt::getName() const
	{
		static const CppUtils::String name("[ MT control algorithm ]");
		return name;
	}

	void AlgorithmMt::getPositionListForAccount(CppUtils::String const& account, BrkLib::PositionList &positionList)
	{

		// copy this cause may wokk in multithread enviroment
		//persistent_m.query(account, positionList);
		THROW(CppUtils::OperationFailed, "exc_NotImplemented", "ctx_processArrivedEvent_G_QUERY_POSITIONS_FOR_ACCOUNT", "");

	}

	

	/**
	* Helpers
	*/

	

	



	// very important fun
	// 

	
	void AlgorithmMt::mtReaderThreadRun()
	{
		threadStarted_m.signalEvent();
		int delay  = settingsManager_m.getSettings().mt4datareadintrvalsec_m;

		LOG(MSG_INFO, ALG_MT, "MT Reader thread started, delay period: " << delay);


		while(true) {

			CppUtils::sleep(delay);
			try {
				processMTOutput();
			}
			catch(CppUtils::Exception& e) {
				LOG(MSG_FATAL, ALG_MT, "Fatal exception within thread loop: " << e.message());
			}
#ifdef HANDLE_NATIVE_EXCEPTIONS
		catch(Testframe::Exception& e) {
			LOG(MSG_FATAL, ALG_MT, "Fatal MSVC exception within thread loop: " << e.message());
		}
#endif
			catch(...) {
				LOG(MSG_FATAL, ALG_MT, "Unknown fatal exception within thread loop");
			}

			// shutdown signal
			if (shutDownThread_m.lock(0)) {
				break;
			}


		
		} // end loop
		
		LOG(MSG_INFO, ALG_MT, "MT Reader thread is finishing...");
	}
	
	void AlgorithmMt::resolveAvailableMTInstance(CppUtils::String& instanceName, CppUtils::String const& companyId)
	{
		instanceName.clear();
		GlobalSettingsEntry const& gse = settingsManager_m.getSettings();

		MtSettingsMap::const_iterator it_found = gse.mtSettingsMap_m.end();

		bool first = true;
		int min_cnt = -1;
		for(MtSettingsMap::const_iterator it = gse.mtSettingsMap_m.begin(); it != gse.mtSettingsMap_m.end(); it++) {
			MTSettings const& set_i = it->second;
			
		

			if (it->second.getStartUpSettings().loadBalanceEnabled ) {
				if (workerThreadManager_m.isRunningNormally(set_i.getStartUpSettings().mtName)) {
					if (CppUtils::toUpperCase( it->second.getStartUpSettings().companyId_m)  == CppUtils::toUpperCase(companyId)) {
						int queue_size = workerThreadManager_m.getDataQueueObjectProxy(set_i.getStartUpSettings().mtName)().size();

						if (first) {
							min_cnt = queue_size;

							first = false;
							it_found = it;
						}
						else {
							if (queue_size < min_cnt) {
								min_cnt = queue_size;
								it_found = it;
							}
						}
					}
				}
			}

		}

		if (it_found != gse.mtSettingsMap_m.end()) {
			instanceName = it_found->second.getStartUpSettings().mtName;
			// minimum found
			LOG(MSG_DEBUG, ALG_MT, "Found the most available instance the most available MT instance " << instanceName  );

		}
		else {
			LOG(MSG_DEBUG, ALG_MT, "The system was not able to automatically resolve MT instance"  );
		}

		

		

	}

	CppUtils::RefCountPtr<AlgLib::AlgLib_Event_Dialog_Response::Resp_START_ORDER_LIST>
		AlgorithmMt::n_processArrivedEvent_START_ORDER_LIST(AlgLib_Event_Dialog_Request::AlgLibRequestBase const& req)
	{
		CppUtils::RefCountPtr< AlgLib_Event_Dialog_Response::Resp_START_ORDER_LIST> ptr = new AlgLib_Event_Dialog_Response::Resp_START_ORDER_LIST();
		
		AlgLib::GeneralCommandList gcList;
		auto& ptrRef = (AlgLib_Event_Dialog_Request::Req_START_ORDER_LIST&)req;
		
		// instalce name
	
		// put data
		for(CppUtils::String const& order: ptrRef.orders_m) {
					AlgLib::OrderStruct os(order);
					AlgLib::GeneralCommand gc(os);

					gcList.push_back(gc);
		}

		workerThreadManager_m.getDataQueueObjectProxy(ptrRef.getMTInstaceName())().put(gcList );
		ptr->mt_instance_name_m = ptrRef.getMTInstaceName();
			
		return ptr;
	}

	CppUtils::RefCountPtr< AlgLib_Event_Dialog_Response::Resp_START_POLLING_USERS> 
			AlgorithmMt::n_processArrivedEvent_START_POLLING_USERS(AlgLib_Event_Dialog_Request::AlgLibRequestBase const& req)
	{
		auto& ptrRef = (AlgLib_Event_Dialog_Request::Req_START_POLLING_USERS&)req;

		CppUtils::RefCountPtr< AlgLib_Event_Dialog_Response::Resp_START_POLLING_USERS> ptr = new AlgLib_Event_Dialog_Response::Resp_START_POLLING_USERS();
		

		// to chnage latter and remove dummy params
		CppUtils::String &inst_name  = ptrRef.getMTInstaceName();

		if (ptrRef.getMTInstaceName().length() <=0) {

			resolveAvailableMTInstance(ptrRef.getMTInstaceName(), ptrRef.companyName_m );

			if (ptrRef.getMTInstaceName().length() <=0) {
				
				DEBUG_LOG(*this, "Cannot resolve automatically MT instnace name for the command <START_POLLING_USERS>, company:  " << ptrRef.companyName_m );
				//THROW(CppUtils::OperationFailed,"exc_InvalidMTName", "ctx_n_processArrivedEvent_START_POLLING_USERS", "Cannot resolve MT instance automatically" );
			}
			
		}

		if (inst_name.size() > 0) {
			//ptrRef.setMTInstaceName(inst_name);
			

			AlgLib::GeneralCommandList gcList;
			for(auto const& user: ptrRef.userList_m) {

				// update access time
				AlgLib::GeneralCommand gc(AlgLib::UserInfoStruct(user.UserName_m, user.password_m, user.server_m, user.lifeTime_m));
				gc.lastAccessTime() = CppUtils::getTime();

				if (gc.getUserInfo().user_m.size() > 0 && gc.getUserInfo().password_m.size() > 0 && gc.getUserInfo().server_m.size() > 0) {
					gcList.push_back(gc);

					LOG(MSG_DEBUG, ALG_MT, "Added new user to the queue: " << gc.toString());

				}

			}

			
			workerThreadManager_m.getDataQueueObjectProxy(ptrRef.getMTInstaceName())().put(gcList );
		}

		ptr->mt_instance_name_m = ptrRef.getMTInstaceName();
		return ptr;

	}

	CppUtils::RefCountPtr<AlgLib_Event_Dialog_Response::Resp_G_QUERY_AUTOLOAD_USERS>
		AlgorithmMt::n_processArrivedEvent_G_QUERY_AUTOLOAD_USERS(AlgLib_Event_Dialog_Request::AlgLibRequestBase const& req)
	{
		CppUtils::RefCountPtr< AlgLib_Event_Dialog_Response::Resp_G_QUERY_AUTOLOAD_USERS> ptr = new AlgLib_Event_Dialog_Response::Resp_G_QUERY_AUTOLOAD_USERS();
		GlobalSettingsEntry const& gse = settingsManager_m.getSettings();
		
		for(auto it = gse.mtSettingsMap_m.begin(); it != gse.mtSettingsMap_m.end(); it++) {
			MTSettings const& set_i = it->second;
			
			MTWorkerThread::ThreadContext ctx;
			workerThreadManager_m.getContext(set_i.getStartUpSettings().mtName, ctx);

			AlgLib_Event_Dialog_Response::Resp_G_QUERY_AUTOLOAD_USERS::MtInstance inst;
			inst.companyId_m = set_i.getStartUpSettings().companyId_m;
			inst.isntanceUid_m = ctx.ID;
			inst.name_m = set_i.getStartUpSettings().mtName;

			if (set_i.getStartUpSettings().fstruct_m.rows_m.size() >0 ) {

				for(int i = 0; i < set_i.getStartUpSettings().fstruct_m.rows_m.size(); i++){
			
					AlgLib::AutoLoadFileStruct::AutoLoadFileRow const row_i = set_i.getStartUpSettings().fstruct_m.rows_m[i];

					AlgLib_Event_Dialog_Response::Resp_G_QUERY_AUTOLOAD_USERS::MtUser user;
			 
					user.username_m = row_i.user_m;
					user.password_m = row_i.pass_m;
					user.server_m = row_i.srv_m;
					user.lifeTime_m = row_i.tout_m;

					inst.autoLoadUserList_m.push_back(user);
			
				}

			}

			ptr->instances_m.push_back(inst);

		}

		return ptr;

	}
	

	CppUtils::RefCountPtr< AlgLib_Event_Dialog_Response::Resp_G_GET_ALL_METADATA>  
	AlgorithmMt::n_processArrivedEvent_G_GET_ALL_METADATA(AlgLib_Event_Dialog_Request::AlgLibRequestBase const& req)
	{
		//AlgLib_Event_Dialog_Response::Resp_G_GET_ALL_METADATA* result = new AlgLib_Event_Dialog_Response::Resp_G_GET_ALL_METADATA();

		// before we need to update pending size

		CppUtils::RefCountPtr< AlgLib_Event_Dialog_Response::Resp_G_GET_ALL_METADATA> ptr = new AlgLib_Event_Dialog_Response::Resp_G_GET_ALL_METADATA();


		GlobalSettingsEntry const& gse = settingsManager_m.getSettings();
		for(auto it = gse.mtSettingsMap_m.begin(); it != gse.mtSettingsMap_m.end(); it++) {
			MTSettings const& set_i = it->second;

			MTWorkerThread::ThreadContext ctx;
			workerThreadManager_m.getContext(set_i.getStartUpSettings().mtName, ctx);


			AlgLib_Event_Dialog_Response::Resp_G_GET_ALL_METADATA::MtEntry e;

			e.connected_m = workerThreadManager_m.isRunningNormally( set_i.getStartUpSettings().mtName );
			e.name_m = set_i.getStartUpSettings().mtName;
			e.lastHeartBeat_m = ctx.LastHeartBeat;
			e.loadBalanced_m = set_i.getStartUpSettings().loadBalanceEnabled;
			e.terminalUid_m = ctx.ID;
			e.pendingSize_m = workerThreadManager_m.getDataQueueObjectProxy(set_i.getStartUpSettings().mtName)().size();

			e.lastRestartTime_m = -1;
			e.restartCount_m = 0;
			e.pid_m = ctx.Tid;
			e.companyId_m = set_i.getStartUpSettings().companyId_m;

			ptr->data_m.push_back( e );

		}

		
		ptr->eventPluginParameters_m = gse.eventPluginParams_m;
		return ptr;

	}

	CppUtils::RefCountPtr< AlgLib_Event_Dialog_Response::Resp_REQUEST_POLLING_USER_LIST>
			AlgorithmMt::n_processArrivedEvent_REQUEST_POLLING_USER_LIST(AlgLib_Event_Dialog_Request::AlgLibRequestBase const& req)
	{
		CppUtils::RefCountPtr< AlgLib_Event_Dialog_Response::Resp_REQUEST_POLLING_USER_LIST> ptr = new AlgLib_Event_Dialog_Response::Resp_REQUEST_POLLING_USER_LIST();

		auto const& reqRef = (AlgLib::AlgLib_Event_Dialog_Request::Req_REQUEST_POLLING_USER_LIST const&)req;
	
		AlgLib::GeneralCommandList userList;
		workerThreadManager_m.getDataQueueObjectProxy(reqRef.getMTInstaceName())().getPendingData(userList);

		
	

		for(auto const& user: userList) {
			AlgLib::AlgLib_Event_Dialog_Response::Resp_REQUEST_POLLING_USER_LIST::MtUser user_obj;
			user_obj.lifeTime_m = user.lifetime();
			user_obj.password_m = user.getUserInfo().password_m;
			user_obj.UserName_m = user.getUserInfo().user_m;

			ptr->userList_m.push_back( user_obj );
		}

		ptr->mt_instance_name_m = reqRef.getMTInstaceName();
		return ptr;
	}

	CppUtils::RefCountPtr< AlgLib_Event_Dialog_Response::Resp_CLEAR_ALL_USERS>
			AlgorithmMt::n_processArrivedEvent_CLEAR_ALL_USERS(AlgLib_Event_Dialog_Request::AlgLibRequestBase const& req)
	{
		auto const& reqRef = (AlgLib::AlgLib_Event_Dialog_Request::Req_REQUEST_POLLING_USER_LIST const&)req;
		CppUtils::RefCountPtr< AlgLib_Event_Dialog_Response::Resp_CLEAR_ALL_USERS> ptr = new AlgLib_Event_Dialog_Response::Resp_CLEAR_ALL_USERS();

		workerThreadManager_m.getDataQueueObjectProxy(reqRef.getMTInstaceName())().clear();
		
		ptr->mt_instance_name_m = reqRef.getMTInstaceName();
		return ptr;


	}

	CppUtils::RefCountPtr< AlgLib_Event_Dialog_Response::Resp_REQUEST_MT_STATUS>
			AlgorithmMt::n_processArrivedEvent_REQUEST_MT_STATUS(AlgLib_Event_Dialog_Request::AlgLibRequestBase const& req)
	{
			auto const& reqRef = (AlgLib::AlgLib_Event_Dialog_Request::Req_REQUEST_MT_STATUS const&)req;
			CppUtils::RefCountPtr< AlgLib_Event_Dialog_Response::Resp_REQUEST_MT_STATUS> ptr = new AlgLib_Event_Dialog_Response::Resp_REQUEST_MT_STATUS();
			
	
			ptr->totalQueueSize_m = workerThreadManager_m.getDataQueueObjectProxy(reqRef.getMTInstaceName())().size();

			ptr->mt_instance_name_m = reqRef.getMTInstaceName();
			return ptr;
	}

	CppUtils::RefCountPtr< AlgLib_Event_Dialog_Response::Resp_UPDATE_USER_ACCESS_TIME>
			AlgorithmMt::n_processArrivedEvent_UPDATE_USER_ACCESS_TIME(AlgLib_Event_Dialog_Request::AlgLibRequestBase const& req)
	{
		auto const& reqRef = (AlgLib::AlgLib_Event_Dialog_Request::Req_UPDATE_USER_ACCESS_TIME const&)req;
		CppUtils::RefCountPtr< AlgLib_Event_Dialog_Response::Resp_UPDATE_USER_ACCESS_TIME> ptr = new AlgLib_Event_Dialog_Response::Resp_UPDATE_USER_ACCESS_TIME();
	

		for(auto const& user : reqRef.users_m ) {

			workerThreadManager_m.getDataQueueObjectProxy(reqRef.getMTInstaceName())().iterateThroughElementsSingleElement(
				 [&] (AlgLib::GeneralCommand& userinfo) {
					if (userinfo.equalKey(user))
						userinfo.lastAccessTime() = (int)CppUtils::getTime();
				}	
			);
		}

		ptr->mt_instance_name_m = reqRef.getMTInstaceName();
		return ptr;
	}



	

	void AlgorithmMt::sendHeartBeat() const
	{

		LOG(MSG_DEBUG, ALG_MT, "Sending heartbeat..." );

		HlpStruct::EventData response;
		response.getEventType() = HlpStruct::EventData::ET_CustomEvent;
		response.setType(HlpStruct::EventData::DT_CommonXml);

		Alglib_Out_Dialog::HeartBeatOut out_packet_obj;

		// set account number and data 
		HlpStruct::XmlParameter& respxml = response.getAsXmlParameter( true );
		json::Object upper_object;
		respxml.getParameter("data").getAsString(true) = JsonSerializerHelper()( out_packet_obj );

		respxml.getName() = MT4_HEARTBEAT_EVENT;

		Inqueue::OutProcessor::pushevent(response);


	}

	void AlgorithmMt::killZombieMts() const
	{
		LOG(MSG_DEBUG, ALG_MT, "Killing zombies MTs...");

		// get the list if running PIDs

		set<int> pids;
		workerThreadManager_m.getAllRunningPids(pids);

		// list of all current MTs
		CppUtils::Uid uid; uid.generate();

		// run task lsit
		CppUtils::String filep = CppUtils::getTempPath() + "/" + uid.toString() + "__c.txt";
		CppUtils::String command = CppUtils::String("tasklist  /FI \"IMAGENAME eq terminal.exe\" /FO CSV /NH > " + filep);

		system(command.c_str());
		if (!CppUtils::fileExists(filep))
			return;
		 
		CppUtils::String content;
		CppUtils::readContentFromFile2(filep, content);
		CppUtils::removeFile(filep);

		if (content.size() > 0) {

			CppUtils::StringList rows;
			CppUtils::tokenize(content,rows, "\n");

			for(int i = 0; i < rows.size();i++) {
				CppUtils::String const& row_i = rows[i];

				CppUtils::StringList data;
				CppUtils::parseCommaSeparated(',', row_i, data);

				if (data.size() >= 5) {
					CppUtils::trimBoth(data[1], "\"");

					int pid = atoi(data[1].c_str());
					LOG(MSG_DEBUG, ALG_MT, "MT detected running: " << pid);

					// try to find in orig list
					if (pids.find(pid) == pids.end()) {
						// not found - kill
						CppUtils::killProcess(pid, -1);
						LOG(MSG_DEBUG, ALG_MT, "Killed unnecessary MT instance: " << pid);
					} else {
						LOG(MSG_DEBUG, ALG_MT, "This MT instance belongs to us: " << pid);
					}
				}

			}

			
		}

		
	}

	void AlgorithmMt::processMTOutput() const
	{
		LOG(MSG_DEBUG, ALG_MT, "Grabbing MT output data...");
		DEBUG_LOG(*this, "Processing MT output");

		
		CppUtils::StringList fileNames;
		CppUtils::getAllFileMatchesRecursive(getSettingsManager().getSettings().baseTmpMtPathFileMatch_m, fileNames );
		
		if (fileNames.size() > 0) {

			int sent = 0;
			for(auto &it: fileNames) {
				LOG(MSG_DEBUG, ALG_MT, "Processing MT output file: " << it);


				CppUtils::FileLockHelper flocker;
				if (flocker.initialize(it.c_str(), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING)) {
					if (flocker.getFileSize() <=0) {
						flocker.unlock();
						LOG(MSG_DEBUG, MTWORKTHREAD, "MT data file is empty, continue..." );

						continue;
					}

					// read file
					CppUtils::String content;
					if (CppUtils::readContentFromFile(flocker.getFileHandler(), content)) {

						if (content.size() > 0) {
							CppUtils::RefCountPtr<MtProxy_AlgLib_Dialog::MTProxyPacketBase> mt_ptr = 
								JsonDeserializerHelper<MtProxy_AlgLib_Dialog>().deserialize<MtProxy_AlgLib_Dialog::MTProxyPacketBase>(content);

							if (CallIfMatch2<MtProxy_AlgLib_Dialog::Mtproxy_Data>()(mt_ptr) ) {

								auto const& thisRef= (MtProxy_AlgLib_Dialog::Mtproxy_Data const&)(*mt_ptr);

								//send everuthing to upper level
								sendRawDataFromMT(thisRef);

								DEBUG_LOG(*this, "Processed file OK: " << it);
								LOG(MSG_DEBUG, MTWORKTHREAD, "Received and sent to upper level MT data of size: " << content.size() );
								sent++;
							
							}
						}
						else {
							LOG(MSG_ERROR, ALG_MT, "Content of file: " << it << " is empty");
						}

					}
					else {
						LOG(MSG_ERROR, ALG_MT, "Cannot read file: " << it);
					}
				} else {
					DEBUG_LOG(*this, "File: " << it << " cannot be locked: " << flocker.errorMsg() );
					LOG(MSG_ERROR, ALG_MT, "File: " << it << " cannot be locked: " << flocker.errorMsg());
				}



				// remove file
				flocker.unlock();
				CppUtils::removeFile( it );


			} // end loop

		
			if (sent != fileNames.size()) {
				DEBUG_LOG(*this, "Some MT data files where not processed, sent " << sent << ", but expected: " << fileNames.size() );
			}
			else {
				DEBUG_LOG(*this, "Sent all MT data files OK: " << sent);
			}

		}

	}


	void AlgorithmMt::propagateDebugOutput(char const* str) const
	{
		if (helperLoggerP_m && str) {
			helperLoggerP_m->print(str);
		}
	}
	

	

} // end of namespace