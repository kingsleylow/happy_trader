#include "implement.hpp"
#include "../brkliblaser/brkliblaser.hpp"
#include "utils.hpp"
#include "statemachine.hpp"

#include "../math/math.hpp"

#define PERIOD_MIN 5

namespace AlgLib {



	// ------------------------

	int bootProcssingThread (void *ptr)
	{
		// handle run-time exceptions
#ifdef HANDLE_NATIVE_EXCEPTIONS
		Testframe::init();
#endif


		MRangeBreak* pThis = (MRangeBreak*)ptr; 
		try {
			pThis->threadProcessingThread();
		}
		catch(CppUtils::Exception& e) {
			LOG(MSG_FATAL, ALG_M_RANGE_BREAK2, "Fatal exception: " << e.message());
		}
#ifdef HANDLE_NATIVE_EXCEPTIONS
		catch(Testframe::Exception& e) {
			LOG(MSG_FATAL, ALG_M_RANGE_BREAK2, "Fatal MSVC exception: " << e.message());
		}
#endif
		catch(...) {
			LOG(MSG_FATAL, ALG_M_RANGE_BREAK2, "Unknown fatal exception");
		}
		return 42;

	}


	// ------------------------

	AlgorithmBase *creator (Inqueue::AlgorithmHolder* algholder)
	{
		return new MRangeBreak(*algholder);
	}

	void terminator (AlgorithmBase *pthis)
	{
		assert(pthis);
		delete pthis;
	}

	AlgorithmBase::CreatorAlgorithmFun createAlgorithm = creator;

	AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm = terminator;


	// ------------------------

	MRangeBreak::MRangeBreak( Inqueue::AlgorithmHolder& algHolder):
	AlgorithmBase(algHolder),
		thread_m(NULL),
		brokerConnect_m(NULL),
		metacommandProcessor_m(NULL)
	{  

		thread_m = new CppUtils::Thread(bootProcssingThread, this);

		// if cannot start thread for 10 seconds
		if (!threadStarted_m.lock(10000))
			THROW(CppUtils::OperationFailed, "exc_CannorStartProcessingThread", "ctx_MRangeBreakCtor", "");



	}

	MRangeBreak::~MRangeBreak()
	{  
		shutDownThread_m.signalEvent();
		thread_m->waitForThread();
		// Delete thread
		delete thread_m;
		thread_m = NULL;


		LOG(MSG_INFO, ALG_M_RANGE_BREAK2, MRangeBreak::getName() << " - deinitialized");

	}

	void MRangeBreak::onLibraryInitialized(
		HlpStruct::AlgBrokerDescriptor const& descriptor, 
		BrkLib::BrokerBase* brokerlib,
		BrkLib::BrokerBase* brokerlib2,
		CppUtils::String const& runName,
		CppUtils::String const& comment  
		)
	{

		if (!brokerConnect_m) {
			brokerConnect_m = new BrkLib::BrokerConnect(brokerlib, "gerchik_range_break2", false, false, internalRun_name_m.c_str(), comment.c_str());
			brokerConnect_m->connect();
		}

		if (!brokerConnect_m->getSession().isConnected()) {
			LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  ALG_M_RANGE_BREAK2, "onLibraryInitialized(...) session cannot be established: " << brokerConnect_m->getSession().getConnection());
			return;
		}


		// will be used for drawable objects
		internalRun_name_m = runName;

		// defaults


		// --------------------

		hoursBegin_m = 9;
		minutesBegin_m  = 30;

		hourEnd_m = 15;
		minutesEnd_m = 59;


		// hours of trading
		CppUtils::StringList reslst;
		CppUtils::String const& time_trade_begin_s = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "TIME_TRADE_BEGIN");
		CppUtils::tokenize(time_trade_begin_s,reslst,":");

		if (reslst.size() ==2) {
			hoursBegin_m=atoi(reslst[0].c_str());
			minutesBegin_m = atoi(reslst[1].c_str());
		}

		// 16:40
		CppUtils::String const& time_trade_end_s = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "TIME_TRADE_END");
		CppUtils::tokenize(time_trade_end_s,reslst,":");

		if (reslst.size() ==2) {
			hourEnd_m=atoi(reslst[0].c_str());
			minutesEnd_m = atoi(reslst[1].c_str());
		}

		unixTimeBeginTrade_m = hoursBegin_m * 60 * 60 + minutesBegin_m*60;
		unixTimeEndTrade_m = hourEnd_m * 60 * 60 + minutesEnd_m*60;


		providerForRTSymbols_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "PROVIDER_FOR_RT_SYMBOLS"); // this will be used for drawable objects


		CppUtils::String const& symbol_rt_file = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "FILE_FOR_RT_SYMBOLS");
		if (CppUtils::fileExists(symbol_rt_file)) {

			CppUtils::parseSymbolListFile(symbolLevel1_rt_m, symbol_rt_file);

			// send available symbols
			for(CppUtils::StringSet::const_iterator it = symbolLevel1_rt_m.begin(); it != symbolLevel1_rt_m.end(); it++) {
				LOG_SYMBOL(internalRun_name_m,  providerForRTSymbols_m, BrkLib::AP_MEDIUM, *it, "symbol will be processed" );

			}

		};


		// the maximum nuber of concurrently traded securities
		// -1 means no restriction
		//CONCURRENT_TRADE_SECURITIES_m = GET_ALG_TRADE_INT(descriptor.initialAlgBrkParams_m, "CONCURRENT_TRADE_SECURITIES");


		LOG_COMMON( internalRun_name_m, providerForRTSymbols_m, BrkLib::AP_MEDIUM, 
			" Gerchik range break library started " << "\n" <<
			" Symbols totally: " << symbolLevel1_rt_m.size() << "\n" <<
			" time work begin: " << hoursBegin_m << ":" << minutesBegin_m << "\n" <<
			" time work end: " << hourEnd_m << ":" << minutesEnd_m << "\n" 
			);

		// now need to create helper class 
		metacommandProcessor_m = new MetacommandProcessor(*this);


	}

	void MRangeBreak::onLibraryFreed()
	{
		LOG_COMMON( internalRun_name_m, providerForRTSymbols_m, BrkLib::AP_MEDIUM,"Gerchik range break2 library freed");

		if (brokerConnect_m) {
			delete brokerConnect_m;
			brokerConnect_m = NULL;
		}

		if (metacommandProcessor_m) {
			delete metacommandProcessor_m;
			metacommandProcessor_m = NULL;
		}


	}

	/**
	* This process asych events from broker and generate results
	*/
	void MRangeBreak::onBrokerResponseArrived( 
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
		)
	{

		// process all responses
		if (bresponse.type_m == BrkLib::RT_BrokerResponseEquity) {

		}
		else if (bresponse.type_m == BrkLib::RT_BrokerResponseDelayedOrder) {
			BrkLib::BrokerResponseDelayedOrder const& bRespDelayedOrder = (BrkLib::BrokerResponseDelayedOrder const&)bresponse;

			if (bRespDelayedOrder.doType_m == BrkLib::DORT_ISSUE_ORDER) {
				// delayed order is issued
				Action machine_action = resolveMachineAction(bRespDelayedOrder.parentID_m);

			}
			else if (bRespDelayedOrder.doType_m == BrkLib::DORT_REGISTER_ORDER) {
				// order is registered
				Action machine_action = resolveMachineAction(bRespDelayedOrder.parentID_m);

				ACTION_BEGIN(machine_action.getAction())
					ACTION_ENTRY_BEGIN(AS_IssueOrder) 
						TradingEvent e(bRespDelayedOrder.parentID_m, TE_BrokerConfirm_Delayed, -1);

						pushProcessingEvent(brkConnect, e);
					ACTION_ENTRY_END()
				ACTION_END()
			}
			else if (bRespDelayedOrder.doType_m == BrkLib::DORT_UNREGISTER_ORDER) {
				// order is unregistered
				Action machine_action = resolveMachineAction(bRespDelayedOrder.parentID_m);

				ACTION_BEGIN(machine_action.getAction())
					ACTION_ENTRY_BEGIN(AS_RemoveDelayedOrder) 

						TradingEvent e(bRespDelayedOrder.parentID_m, TE_BrokerConfirm_Cancelled, -1);
						pushProcessingEvent(brkConnect, e);
					ACTION_ENTRY_END()
				ACTION_END()
			}
		}
		else if (bresponse.type_m == BrkLib::RT_BrokerResponseOrder) {
			BrkLib::BrokerResponseOrder const& bRespOrder = (BrkLib::BrokerResponseOrder const&)bresponse;


			if (bRespOrder.resultCode_m==BrkLib::PT_ORDER_ESTABLISHED ) {

				// PENDING!!!
				if (bRespOrder.resultDetailCode_m & BrkLib::ODR_ORDER_INSTALLED) {
					Action machine_action = resolveMachineAction(bRespOrder.parentID_m);
					ACTION_BEGIN(machine_action.getAction())
						ACTION_ENTRY_BEGIN(AS_IssueOrder) 
							TradingEvent e(bRespOrder.parentID_m, TE_BrokerConfirm_Pending, bRespOrder.opTime_m);
							e.context().installTime_m = bRespOrder.opTime_m;
							e.context().installPrice_m = bRespOrder.opPrice_m;
							e.context().brokerPositionId_m = bRespOrder.brokerPositionID_m;


							pushProcessingEvent(brkConnect, e);
						ACTION_ENTRY_END()
					ACTION_END()

				}
				else if (bRespOrder.resultDetailCode_m & BrkLib::ODR_ORDER_EXECUTED) {

					// EXECUTED!!!
					// we have rights return only final trades
					if (bRespOrder.opRemainingVolume_m == 0 ) {


						//
						Action machine_action = resolveMachineAction(bRespOrder.parentID_m);
						ACTION_BEGIN(machine_action.getAction())
							ACTION_ENTRY_BEGIN(AS_IssueOrder) 
								TradingEvent e(bRespOrder.parentID_m, TE_BrokerConfirm_Open, bRespOrder.opTime_m);
								e.context().executeTime_m = bRespOrder.opTime_m;
								e.context().executePrice_m = bRespOrder.opPrice_m;
								e.context().brokerPositionId_m = bRespOrder.brokerPositionID_m;

								pushProcessingEvent(brkConnect,e);
							ACTION_ENTRY_END()
						ACTION_END()

					} 

				}
			}
			else if (bRespOrder.resultCode_m==BrkLib::PT_ORDER_CANCELLED) {
				// order cancelled
				Action machine_action = resolveMachineAction(bRespOrder.parentID_m);
				ACTION_BEGIN(machine_action.getAction())
					ACTION_ENTRY_BEGIN(AS_CancelOrder) 
						TradingEvent e(bRespOrder.parentID_m, TE_BrokerConfirm_Cancelled, bRespOrder.opTime_m);
						e.context().cancelTime_m = bRespOrder.opTime_m;
						e.context().cancelPrice_m = bRespOrder.opPrice_m;

						pushProcessingEvent(brkConnect, e);
					ACTION_ENTRY_END()
				ACTION_END()


			}
			else if (bRespOrder.resultCode_m==BrkLib::PT_GENERAL_ERROR_HAPPENED) {
				// order errored
				// return errors everywhere
				Action machine_action = resolveMachineAction(bRespOrder.parentID_m);
				ACTION_BEGIN(machine_action.getAction())
					ACTION_ENTRY_BEGIN(AS_CancelOrder) 
						TradingEvent e(bRespOrder.parentID_m, TE_BrokerReturnError, bRespOrder.opTime_m);
						e.context().errorReason_m = bRespOrder.brokerComment_m;
						e.context().errorTime_m =bRespOrder.opTime_m;
						pushProcessingEvent(brkConnect, e);
					ACTION_ENTRY_END()
					ACTION_ENTRY_BEGIN(AS_IssueOrder) 
						TradingEvent e(bRespOrder.parentID_m, TE_BrokerReturnError, bRespOrder.opTime_m);
						e.context().errorReason_m = bRespOrder.brokerComment_m;
						e.context().errorTime_m = bRespOrder.opTime_m;
						pushProcessingEvent(brkConnect, e);
					ACTION_ENTRY_END()
				ACTION_END()


			}

		}


	}

	void MRangeBreak::onRtProviderSynchEvent (
		Inqueue::ContextAccessor& contextaccessor,
		HlpStruct::RtData::SynchType const synchFlg,
		double const& synchTime,
		CppUtils::String const& providerName,
		CppUtils::String const& message
		)
	{
	}

	void MRangeBreak::onDataArrived(
		HlpStruct::RtData const& rtdata, 
		Inqueue::ContextAccessor& historyaccessor, 
		HlpStruct::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
		)
	{

		if (!brokerConnect_m) {
			brokerConnect_m = new BrkLib::BrokerConnect(brokerlib, "gerchik_range_break2", false, false, internalRun_name_m.c_str(), comment.c_str());
			brokerConnect_m->connect();
		}

		if (!brokerConnect_m->getSession().isConnected()) {
			LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  ALG_M_RANGE_BREAK2, "onDataArrived(...) session cannot be established: " << brokerConnect_m->getSession().getConnection());
			return;
		}


		// need to check our delayed orders
		brokerlib->onDelayedOrderExecutionCheck(brokerConnect_m->getSession(), rtdata.getRtData().bid_m, rtdata.getProvider2(), rtdata.getSymbol2() );

	}

	void MRangeBreak::onLevel2DataArrived(
		HlpStruct::RtLevel2Data const& level2data,
		Inqueue::ContextAccessor& historyaccessor, 
		HlpStruct::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
		)
	{
	}

	void MRangeBreak::onLevel1DataArrived(
		HlpStruct::RtLevel1Data const& level1data,
		Inqueue::ContextAccessor& historyaccessor, 
		HlpStruct::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
		)
	{
	}


	void MRangeBreak::onEventArrived(
		Inqueue::ContextAccessor& contextaccessor, 
		HlpStruct::CallingContext& callContext,
		bool& result,  
		CppUtils::String &customOutputData
		)
	{   
		// here we will implemnt open of our position

		result = false;
		// not custom
		if (callContext.getCallCustomName().size() <=0)
			return;



		

		CppUtils::String const& data = callContext.getParameter<BrkLib::String_Proxy>("custom_data").get();

		if (callContext.getCallCustomName()=="Force_TE_UserConfirmFix") {

			CppUtils::Uid dummyUid;
			ordermachine_Force_TE_UserConfirmFix(data, dummyUid);
			customOutputData = "command accepted";

		}
		else if (callContext.getCallCustomName()=="Force_TE_InstallOrder") {

			// this enforce trading
			
			CppUtils::Uid dummyUid;
			ordermachine_Force_TE_InstallOrder(data, dummyUid);
			customOutputData = "command accepted";

			

		}
		else if (callContext.getCallCustomName()=="Force_TE_CancelOrder") {

			// this enforce trading
			CppUtils::Uid dummyUid;
			ordermachine_Force_TE_CancelOrder(data, dummyUid);
			customOutputData = "command accepted";
			
		}
		// not directly events
		else if (callContext.getCallCustomName()=="Force_Ping") {

			// retreive all trading symbols
			HlpStruct::XmlParameter xmlparameter;
			for(CppUtils::StringSet::const_iterator it = symbolLevel1_rt_m.begin(); it != symbolLevel1_rt_m.end(); it++) {
				xmlparameter.getParameter("symbols").getAsStringList(true).push_back(*it);
			}

			CppUtils::String content;
			HlpStruct::serializeXmlParameter(xmlparameter, content );

			// get symbol list
			customOutputData = content;
		}
		// ------------------------
		// call metaprocessor
		else if (callContext.getCallCustomName()=="Force_Run_Breakthrough_Up") {

			// data will contain info for 3 orders!!!


			// extract content
			HlpStruct::XmlParameter param_data;
			HlpStruct::deserializeXmlParameter(param_data, data);
			customOutputData = "command accepted";

			if (metacommandProcessor_m)
				metacommandProcessor_m->runBreakThroughUp(param_data);
			else
				THROW(CppUtils::OperationFailed, "exc_MetacommandProcessorNotInitialized", "ctx_onEventArrived", "");
		}
		else if (callContext.getCallCustomName()=="Force_Run_Breakthrough_Down") {

			// data will contain info for 3 orders!!!


			// extract content
			HlpStruct::XmlParameter param_data;
			HlpStruct::deserializeXmlParameter(param_data, data);
			customOutputData = "command accepted";

			if (metacommandProcessor_m)
				metacommandProcessor_m->runBreakThroughDown(param_data);
			else
				THROW(CppUtils::OperationFailed, "exc_MetacommandProcessorNotInitialized", "ctx_onEventArrived", "");
		}
		


		result = true;
	}

	void MRangeBreak::onThreadStarted(Inqueue::ContextAccessor& contextaccessor , bool const firstLib, bool const lastLib) {
		LOGEVENT(HlpStruct::CommonLog::LL_INFO,  ALG_M_RANGE_BREAK2, "RT Thread started");

	}


	CppUtils::String const& MRangeBreak::getName() const
	{
		static const CppUtils::String build_date(__TIMESTAMP__);
		static const CppUtils::String name( "[ gerchik range break 2 ]: " + build_date );
		return name;
	}

	/**
	* Helpers
	*/

	void MRangeBreak::ordermachine_Force_TE_InstallOrder(CppUtils::String const& orderSignallerString, CppUtils::Uid const& parentUid)
	{
		TradingEvent te;
		te.context().signaller_m.fromString(orderSignallerString);
		te.initialize(te.context().signaller_m.orderUid_m, TE_InstallOrder, CppUtils::getTime() );

		acceptNewlyArrivedEvent(te, false,parentUid );
	}

	void MRangeBreak::ordermachine_Force_TE_UserConfirmFix(CppUtils::String const& orderSignallerString, CppUtils::Uid const& parentUid)
	{
		TradingEvent te;
		te.context().signaller_m.fromString(orderSignallerString);
		te.initialize(te.context().signaller_m.orderUid_m, TE_UserConfirmFix, CppUtils::getTime() );

		acceptNewlyArrivedEvent(te, true,parentUid );
	}

	void MRangeBreak::ordermachine_Force_TE_CancelOrder(CppUtils::String const& orderSignallerString, CppUtils::Uid const& parentUid)
	{
		TradingEvent te;
		te.context().signaller_m.fromString(orderSignallerString);
		te.initialize(te.context().signaller_m.orderUid_m, TE_UserCancel, CppUtils::getTime() );

		acceptNewlyArrivedEvent(te, true,parentUid );
	}


	void MRangeBreak::acceptNewlyArrivedEvent(TradingEvent const& te, bool const machine_must_exist, CppUtils::Uid const& parentUid)
	{
		LOCK_FOR_SCOPE(orderStateMachinesMtx_m);

		if (te.context().signaller_m.orderUid_m.isValid()) {

			OrderStateMachine newMachine;
			map<CppUtils::Uid, OrderStateMachine>::iterator it = orderStateMachines_m.find(te.context().signaller_m.orderUid_m);

			if (it==orderStateMachines_m.end()) {
				if (machine_must_exist) {
					THROW(CppUtils::OperationFailed, "exc_StateMachineMustExist", "ctx_onEventArrived", te.context().signaller_m.orderUid_m.toString());
				}

				newMachine.initialize(
					te.context().signaller_m.orderUid_m, 
					static_cast<OrderStateMachineListener*>(this), 
					te.context().signaller_m.description_m,
					parentUid
				);
				
				std::pair <CppUtils::Uid, OrderStateMachine> p(te.context().signaller_m.orderUid_m, newMachine);

				orderStateMachines_m.insert(p);
			}

		}

		if (te.getEvent() != TE_Dummy) {
			machineEventsReaction(*brokerConnect_m, te);  
		}
	}

	void MRangeBreak::requestIssueOrderInfo(CppUtils::Uid const& orderUid, Action const& oldaction, Action const& newaction, OrderDescriptor& orderDescriptor)
	{
		if (!brokerConnect_m)
			return;

		BrkLib::BrokerOperation oper(*brokerConnect_m);
		orderDescriptor.orderIssueTime_m = oper.getBrokerSystemTime();

	}

	void MRangeBreak::requestCancelOrderInfo(CppUtils::Uid const& orderUid, Action const& oldaction, Action const& newaction, OrderDescriptor& orderDescriptor)
	{
		if (!brokerConnect_m)
			return;
	}

	void MRangeBreak::requestRemoveDelayedOrderInfo(CppUtils::Uid const& orderUid, Action const& oldaction, Action const& newaction, OrderDescriptor& orderDescriptor)
	{
		if (!brokerConnect_m)
			return;
	}

	void MRangeBreak::requestClearInfo(CppUtils::Uid const& orderUid, Action const& oldaction, Action const& newaction, OrderDescriptor& orderDescriptor)
	{
		if (!brokerConnect_m)
			return;
	}

	void MRangeBreak::executeIssueOrder(CppUtils::Uid const& orderUid, Action const& action, OrderDescriptor const& orderDescriptor)
	{
		if (!brokerConnect_m)
			return;

		BrkLib::BrokerOperation oper(*brokerConnect_m);
		BrkLib::Order order;

		// must have this order ID
		order.ID_m = orderDescriptor.orderID_m;
		order.symbol_m = orderDescriptor.symbol_m;
		order.method_m = orderDescriptor.orderMethod_m;
		order.place_m = orderDescriptor.orderPlace_m;
		order.orVolume_m = orderDescriptor.volume_m;
		order.orPrice_m = orderDescriptor.orderIssuePrice_m;
		order.provider_m = providerForRTSymbols_m;

		order.additionalInfo_m = orderDescriptor.additionalOrderInfo_m;
		order.orderType_m = orderDescriptor.orderType_m;
		order.orPriceType_m = BrkLib::PC_INPUT;

		/*
		OP_BUY								=	0x0001,
		OP_SELL								= 0x0002,
		OP_BUY_LIMIT					=	0x0004,
		OP_SELL_LIMIT					=	0x0008,
		OP_BUY_STOP						=	0x0010,
		OP_SELL_STOP					=	0x0020,
		*/

		if (orderDescriptor.direction_m == DIRECTION_LONG) {
			if (!orderDescriptor.orderIsDelayedLocally_m) 
				oper.unblockingIssueOrder(order);
			else
				oper.registerDelayedOrder(BrkLib::DO_EXEC_PRICE_MORE, order.orPrice_m, order, "");
		}
		else if (orderDescriptor.direction_m == DIRECTION_SHORT) {
			if (!orderDescriptor.orderIsDelayedLocally_m) 
				oper.unblockingIssueOrder(order);
			else
				oper.registerDelayedOrder(BrkLib::DO_EXEC_PRICE_LESS, order.orPrice_m, order, "");
		}
		else {
		}


	}

	void MRangeBreak::executeCancelOrder(CppUtils::Uid const& orderUid, Action const& action, OrderDescriptor const& orderDescriptor)
	{
		if (!brokerConnect_m)
			return;

		BrkLib::BrokerOperation oper(*brokerConnect_m);
		BrkLib::Order order;
		order.orderType_m = BrkLib::OP_ORDER_CANCEL;
		order.brokerPositionID_m = orderDescriptor.brokerPositionId_m;

		// try to remove as delayed order
		// if descriptor

		oper.unblockingIssueOrder(order);

	}

	void MRangeBreak::executeRemoveDelayedOrder(CppUtils::Uid const& orderUid, Action const& action, OrderDescriptor const& orderDescriptor)
	{
		if (!brokerConnect_m)
			return;

		BrkLib::BrokerOperation oper(*brokerConnect_m);
		BrkLib::Order order;
		oper.removeDelayedOrder(orderUid);

	}

	void MRangeBreak::executeClear(CppUtils::Uid const& orderUid, Action const& action, OrderDescriptor const& orderDescriptor)
	{
		if (!brokerConnect_m)
			return;

		BrkLib::BrokerOperation oper(*brokerConnect_m);
		oper.removeAllDelayedOrders();
	}

	void MRangeBreak::onStateChangedNotification(CppUtils::Uid const& parentUid, CppUtils::Uid const& orderUid, TradingState const& oldState, TradingState const& newState)
	{
		//LOG_MACHINE_STATE( internalRun_name_m, providerForRTSymbols_m, orderUid, oldState, newState);
		
		
	}


	void MRangeBreak::onOrderDescriptorChanged(CppUtils::Uid const& parentUid, CppUtils::Uid const& orderUid, OrderDescriptor const& orderDescriptor)
	{
		//LOG_MACHINE_ORDER_DESCRIPTOR(internalRun_name_m, providerForRTSymbols_m, orderUid, orderDescriptor);	
	}

	void MRangeBreak::machineConfirmOrderIssued(CppUtils::Uid const& parentUid, CppUtils::Uid const& orderUid, OrderDescriptor const& orderDescriptor)
	{
		// delegate
		if (metacommandProcessor_m)
			metacommandProcessor_m->onMachineConfirmOrderIssued(parentUid, orderUid,orderDescriptor );
	}

	void MRangeBreak::machineConfirmOrderPending(CppUtils::Uid const& parentUid, CppUtils::Uid const& orderUid, OrderDescriptor const& orderDescriptor)
	{
		// delegate
		if (metacommandProcessor_m)
			metacommandProcessor_m->onMachineConfirmOrderPending(parentUid, orderUid,orderDescriptor );
	}

	void MRangeBreak::machineConfirmOrderDelayed(CppUtils::Uid const& parentUid, CppUtils::Uid const& orderUid, OrderDescriptor const& orderDescriptor)
	{
		// delegate
		if (metacommandProcessor_m)
			metacommandProcessor_m->onMachineConfirmOrderDelayed(parentUid, orderUid,orderDescriptor );
	}


	void MRangeBreak::machineConfirmOrderOpened(CppUtils::Uid const& parentUid, CppUtils::Uid const& orderUid, OrderDescriptor const& orderDescriptor)
	{
		// delegate
		if (metacommandProcessor_m)
			metacommandProcessor_m->onMachineConfirmOrderOpened(parentUid, orderUid,orderDescriptor );
	}

	void MRangeBreak::machineConfirmOrderCancelled(CppUtils::Uid const& parentUid, CppUtils::Uid const& orderUid, OrderDescriptor const& orderDescriptor)
	{
		// delegate
		if (metacommandProcessor_m)
			metacommandProcessor_m->onMachineConfirmOrderCancelled(parentUid, orderUid,orderDescriptor );
	}

	void MRangeBreak::machineConfirmErrorReturned(CppUtils::Uid const& parentUid, CppUtils::Uid const& orderUid, OrderDescriptor const& orderDescriptor)
	{
		// delegate
		if (metacommandProcessor_m)
			metacommandProcessor_m->onMachineConfirmErrorReturned(parentUid, orderUid,orderDescriptor );
	}

	// ---------------------
	//

	void MRangeBreak::machineEventsReaction(BrkLib::BrokerConnect& brconnect, TradingEvent const& event)
	{

		LOCK_FOR_SCOPE(orderStateMachinesMtx_m);

		map<CppUtils::Uid, OrderStateMachine>::iterator it = orderStateMachines_m.find(event.getRecipientUid());
		if (it == orderStateMachines_m.end()) {
			return;
		}

		// process
		OrderStateMachine &target_machine = it->second;
		target_machine.newEventArrived(event);
	}

	void MRangeBreak::log(
		CppUtils::String const& provider, 
		CppUtils::String const& runname,
		CppUtils::Uid const& machineUid, 
		CppUtils::Uid const& orderUid, 
		char const* content
	)
	{
		CppUtils::String recepient = machineUid.toString()+orderUid.toString();
		DRAWOBJ_TEXT(runname,CppUtils::getTime(),ALG_M_RANGE_BREAK2, ALL_SYMBOL_ID, provider, BrkLib::AP_MEDIUM, recepient, content);	\
	}

	void MRangeBreak::log(
			CppUtils::String const& provider, 
			CppUtils::String const& runname, 
			CppUtils::Uid const& machineUid, 
			char const* content
		)
	{
		CppUtils::String recepient = machineUid.toString();
		DRAWOBJ_TEXT(runname,CppUtils::getTime(),ALG_M_RANGE_BREAK2, ALL_SYMBOL_ID, provider, BrkLib::AP_MEDIUM, recepient, content);	\
	}




	// ------------------------------
	// HELPERS
	// ------------------------------


	Action MRangeBreak::resolveMachineAction(CppUtils::Uid const& uid)
	{
		LOCK_FOR_SCOPE(orderStateMachinesMtx_m);

		map<CppUtils::Uid, OrderStateMachine>::const_iterator it  = orderStateMachines_m.find(uid);

		// return dummy action
		if (it == orderStateMachines_m.end())
			return Action(AS_Dummy);

		// return a copy
		return it->second.getLastAction();

	}


	void MRangeBreak::pushProcessingEvent(BrkLib::BrokerConnect const& brkConnect, TradingEvent const& event)
	{
		// this will push event data that must be processed in the algorithm/thread
		EventProcessingStruct procStruct(brkConnect, event);

		{
			LOCK_FOR_SCOPE(receivedProcessingDataMtx_m);

			receivedProcessingData_m.push_back(procStruct);
			receivedProcessingDataArrived_m.signalEvent();
		}
	}

	void MRangeBreak::threadProcessingThread()
	{
		threadStarted_m.signalEvent();
		LOG(MSG_INFO, ALG_M_RANGE_BREAK2, MRangeBreak::getName() << " - event processing thread started");


		CppUtils::NativeEvent events[] = 
		{
			shutDownThread_m.getNativeEvent(),				// 0	
			receivedProcessingDataArrived_m.getNativeEvent()        // 1
		};

		while(true) {


			try {
				// wait some of them 
				int res = CppUtils::Event::lockForSomeEvents(events, 2);

				if (res==0) {
					// shutdown
					break;
				}
				else if (res==1) {
					// event arrived
					EventProcessingStruct procstruct;
					{
						LOCK_FOR_SCOPE(receivedProcessingDataMtx_m);
						if (!receivedProcessingData_m.isEmpty()) {

							// copy event data - need to propagate furher, do not want to block
							procstruct = receivedProcessingData_m.get_front();
							receivedProcessingData_m.delete_front();

						};

						// reset event
						receivedProcessingDataArrived_m.clearEvent();

					}

					// processing

					machineEventsReaction(procstruct.brokerConnect_m, procstruct.event_m);

				}


			}
			catch(CppUtils::Exception& e) {
				LOG_COMMON(internalRun_name_m, providerForRTSymbols_m, BrkLib::AP_MEDIUM, "Error in event processing thread: " << e.message());
			}
#ifdef HANDLE_NATIVE_EXCEPTIONS
			catch(Testframe::Exception& e) {
				LOG_COMMON(internalRun_name_m, providerForRTSymbols_m, BrkLib::AP_MEDIUM, "MSVC error in event processing thread: " << e.message());
			}
#endif
			catch(...) {
				LOG_COMMON(internalRun_name_m, providerForRTSymbols_m, BrkLib::AP_MEDIUM,  "Unknown error in event processing thread");
			}

		}

		LOG(MSG_INFO, ALG_M_RANGE_BREAK2, MRangeBreak::getName() << " - event processing thread finished");
	}



	bool MRangeBreak::isWithinWorkingTime(double const &ttime) const
	{
		double diffFromDayBegin = ttime - CppUtils::roundToBeginningOfTheDay(ttime);

		if (diffFromDayBegin < unixTimeBeginTrade_m || diffFromDayBegin > unixTimeEndTrade_m)
			return false;

		return true;

	}



}; // end of namespace