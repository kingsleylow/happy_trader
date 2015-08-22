#include "implement.hpp"
// Many thanks to Zoubok Victor for programming this





namespace AlgLib {

	// ---------------------------
	// context
	// ---------------------------

	MyBrokerContext::MyBrokerContext():
issueState_m(NULL)
{
}

MyBrokerContext::MyBrokerContext(CppUtils::BaseState<MachineContext> const& issueState):
issueState_m(&issueState)
{
}

MyBrokerContext::~MyBrokerContext()
{
}

BrkLib::BrokerContext* MyBrokerContext::clone() const
{
	MyBrokerContext* brkctx = new MyBrokerContext();
	brkctx->issueState_m = issueState_m;

	return brkctx;
}

void MyBrokerContext::deallocate()
{
	delete this;
}

CppUtils::String MyBrokerContext::toString()	const
{
	return getState().toString();
}

CppUtils::BaseState<MachineContext> const& MyBrokerContext::getState() const
{
	return *issueState_m;
}

// -----------------------------


int bootProcessingThread (void *ptr)
{
	// handle run-time exceptions
#ifdef HANDLE_NATIVE_EXCEPTIONS
	Testframe::init();
#endif


	CAlgMisha* pThis = (CAlgMisha*)ptr; 
	try {
		pThis->helperThreadFun();
	}
	catch(CppUtils::Exception& e) {
		LOG(MSG_FATAL, SMARTCOM_ALG_MISHA, "Fatal exception: " << e.message());
	}
#ifdef HANDLE_NATIVE_EXCEPTIONS
	catch(Testframe::Exception& e) {
		LOG(MSG_FATAL, SMARTCOM_ALG_MISHA, "Fatal MSVC exception: " << e.message());
	}
#endif
	catch(...) {
		LOG(MSG_FATAL, SMARTCOM_ALG_MISHA, "Unknown fatal exception");
	}
	return 42;

}


AlgorithmBase *creator (Inqueue::AlgorithmHolder* algholder)
{
	return new CAlgMisha(*algholder);
}

void terminator (AlgorithmBase *pthis)
{
	assert(pthis);
	delete pthis;
}

AlgorithmBase::CreatorAlgorithmFun createAlgorithm = creator;

AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm = terminator;

// intreface to be implemented
CAlgMisha::CAlgMisha( Inqueue::AlgorithmHolder& algHolder):
AlgorithmBase(algHolder),
brokerConnectDemo_m(NULL),
brokerConnect_m(NULL)
{
	thread_m = new CppUtils::Thread(bootProcessingThread, this);

	// if cannot start thread for 10 seconds
	if (!threadStarted_m.lock(10000))
		THROW(CppUtils::OperationFailed, "exc_CannorStartProcessingThread", "ctx_CAlgMishaCtor", "");

}

CAlgMisha::~CAlgMisha()
{
	shutDownThread_m.signalEvent();
	thread_m->waitForThread();
	// Delete thread
	delete thread_m;
	thread_m = NULL;

	LOG(MSG_INFO, SMARTCOM_ALG_MISHA, CAlgMisha::getName() << " - deinitialized");
}

void CAlgMisha::onLibraryInitialized(
																		 HlpStruct::AlgBrokerDescriptor const& descriptor, 
																		 BrkLib::BrokerBase* brokerlib, 
																		 CppUtils::String const& runName,
																		 CppUtils::String const& comment
																		 )
{

	//LOG(MSG_INFO, SMARTCOM_ALG_MISHA, descriptor.dump());


	//brokerConnectDummy_m = NULL;

	brokerConnect_m = NULL;
	brokerConnectDemo_m = NULL;
	connectRunName_m = runName;

	allowTrading_m = BrkLib::TD_NONE;

	// read params
	historyDataProfile_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "HISTORY_DATA_PROFILE");

	rtDataProvider_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "RT_PROVIDER");

	barDataProvider_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "RT_BAR_PROVIDER");

	tradeParam_m.cutoffVolume_m = GET_ALG_TRADE_DOUBLE(descriptor.initialAlgBrkParams_m, "CUTOFF_VOLUME");

	tradeParam_m.maxPriceDifference_m =	 GET_ALG_TRADE_DOUBLE(descriptor.initialAlgBrkParams_m, "MAX_PRICE_DIFFERENCE");

	tradeParam_m.maxMinutesLevelTrack_m = GET_ALG_TRADE_DOUBLE(descriptor.initialAlgBrkParams_m, "MAX_MINUTES_LEVEL_TRACK");

	tradeParam_m.workTimeHolder_m.setUp(descriptor.initialAlgBrkParams_m.algParams_m, "BEGIN_WORK_TIME", "END_WORK_TIME");

	tradeParam_m.period_m = GET_ALG_TRADE_INT(descriptor.initialAlgBrkParams_m, "PERIOD");

	tradeParam_m.tradeVolume_m = GET_ALG_TRADE_DOUBLE(descriptor.initialAlgBrkParams_m, "TRADE_VOLUME");

	tradeParam_m.tp_relativeLevel_m = GET_ALG_TRADE_DOUBLE(descriptor.initialAlgBrkParams_m, "TP_RELATIVE_LEVEL");

	tradeParam_m.sl_relativeLevel_m = GET_ALG_TRADE_DOUBLE(descriptor.initialAlgBrkParams_m, "SL_RELATIVE_LEVEL");

	tradeParam_m.isDemoTrade_m = GET_ALG_TRADE_BOOL(descriptor.initialAlgBrkParams_m, "IS_DEMO_TRADE");

	tradeParam_m.onlyMarketOrders_m = GET_ALG_TRADE_BOOL(descriptor.initialAlgBrkParams_m, "ONLY_MARKET_ORDERS");

	CppUtils::String symbolsFileName = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "SYMBOL_FILE");

	tradeParam_m.onlyOpenPosOpes_m = GET_ALG_TRADE_BOOL(descriptor.initialAlgBrkParams_m, "ONLY_OPEN_OPERATIONS");

	CppUtils::String tradeMode_s = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "TRADE_MODE");
	CppUtils::toupper(tradeMode_s);

	if (tradeMode_s == "OVERCOME") {
		tradeParam_m.tradeMode_m = TradeParam::TM_OVERCOME_LEVEL;
	}
	else if (tradeMode_s == "REFLECT") {
		tradeParam_m.tradeMode_m = TradeParam::TM_REFLECTION_LEVEL;
	}
	else
		THROW(CppUtils::OperationFailed, "exc_InvalidTradeMode", "ctx_onLibraryInitialized", tradeMode_s );


	tradeParam_m.tradePeriodMinutes_m = GET_ALG_TRADE_INT(descriptor.initialAlgBrkParams_m, "TRADE_PERIOD_MINUTES");
	if (tradeParam_m.tradePeriodMinutes_m <=0)
		tradeParam_m.tradePeriodMinutes_m = 15;

	baseExportPath_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "BASE_EXPORT_PATH");
	// need to export - when new period comes
	baseExportPath_m = baseExportPath_m + "\\" + CppUtils::long2String(tradeParam_m.tradePeriodMinutes_m);
	CppUtils::makeDir(baseExportPath_m);



	if (CppUtils::fileExists(symbolsFileName)) {

		symbols_m.clear();
		CppUtils::parseSymbolListFile(symbols_m, symbolsFileName);

		// init state machine
		initStateMachine(symbols_m);
	}
	else
		THROW(CppUtils::OperationFailed, "exc_SymbolFileNotExists", "ctx_onLibraryInitialized", symbolsFileName );

	// connect to broker

	brokerConnect_m = new BrkLib::BrokerConnect(brokerlib, SMARTCOM_ALG_MISHA, false, false, connectRunName_m.c_str(), comment.c_str());
	brokerConnect_m->connect();


	if (!brokerConnect_m->getSession().isConnected()) {
		LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  SMARTCOM_ALG_MISHA, "onLibraryInitialized(...) session cannot be established: " << brokerConnect_m->getSession().getConnection());
	}


	if (!tradeParam_m.onlyOpenPosOpes_m) {
		getAlgHolder().getBroker()->setUpMode(BrkLib::BM_USE_CLOSE_POS_COMMANDS);
	}
	else {
		getAlgHolder().getBroker()->setUpMode(BrkLib::BM_NOTHING);
	}

	// broker connect 2
	BrkLib::BrokerBase* brokerlib2 = getAlgHolder().getBroker2();
	if (brokerlib2) {
		brokerConnectDemo_m = new BrkLib::BrokerConnect(brokerlib2, "Smartcom_Misha_demo", false, false, connectRunName_m.c_str(), comment.c_str());
		brokerConnectDemo_m->connect();
		if (!tradeParam_m.onlyOpenPosOpes_m) {
			brokerlib2->setUpMode(BrkLib::BM_USE_CLOSE_POS_COMMANDS);
		}
		else {
			brokerlib2->setUpMode(BrkLib::BM_NOTHING);
		}

	}

	if (!brokerConnect_m->getSession().isConnected()) {
		LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  SMARTCOM_ALG_MISHA, "onLibraryInitialized(...) session with broker lib 2 cannot be established: " << brokerConnectDemo_m->getSession().getConnection());
	}


	if (tradeParam_m.isInvalid())
		THROW(CppUtils::OperationFailed, "exc_TradeParamsInvalid", "ctx_onLibraryInitialized", tradeParam_m.toString() );

	LOG_COMMON(connectRunName_m, rtDataProvider_m, BrkLib::AP_MEDIUM, "Trading parameters: " << tradeParam_m.toString());

	/*
	// dummy connect
	if (brokerlib2) {
	brokerConnectDummy_m = new BrkLib::BrokerConnect(brokerlib2, "Smartcom_Misha_demo_run", false, false, runName.c_str(), comment.c_str());
	brokerConnectDummy_m->connect();
	if (!brokerConnectDummy_m->getSession().isConnected()) {
	LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  SMARTCOM_ALG_MISHA, "onLibraryInitialized(...) dummy session with broker lib 2 cannot be established: " << brokerConnectDummy_m->getSession().getConnection());
	}		
	}
	*/


}

void CAlgMisha::onLibraryFreed()
{
	deinitStateMachine();

	if (brokerConnect_m) {
		delete brokerConnect_m;
		brokerConnect_m = NULL;
	}

	if (brokerConnectDemo_m) {
		delete brokerConnectDemo_m;
		brokerConnectDemo_m = NULL;
	}
}



void CAlgMisha::onBrokerResponseArrived(
																				BrkLib::BrokerConnect &brkConnect,
																				BrkLib::BrokerResponseBase const& bresponse
																				)
{


	MyBrokerContext const* myCtxPtr = (MyBrokerContext const*)bresponse.context_m.getContext();



	int const order_issue_state_id = TS2_Ready_Start::CLASS_ID;
	int const tp_issue_state_id = TS2_In_Position::CLASS_ID;
	int const sl_issue_state_id = TS2_SL_Pending::CLASS_ID;

	if (bresponse.type_m == BrkLib::RT_BrokerResponseDelayedOrder) {
		// delayed order notification
		BrkLib::BrokerResponseDelayedOrder const& dRespOrder = (BrkLib::BrokerResponseDelayedOrder const&)bresponse;


		double cur_price = -1;
		{
			LOCK_FOR_SCOPE(contextMapMtx_m);

			SymbolContext &ctx = contextMap_m[dRespOrder.order_m.symbol_m];
			cur_price = ctx.curTickPrice_m;
		}

		if (cur_price <= 0) {
			LOG_SYMBOL_SIGNAL( getRunName(), getRtdataProvider(), dRespOrder.order_m.symbol_m, "Current price is invalid for delayed order!: " << BrkLib::getOrderSmallDescription(dRespOrder.order_m));
			return;
		}


		// type of deleayed order
		BrkLib::TradeDirection direction = BrkLib::orderTypeToDirection(dRespOrder.order_m.orderType_m);

		if (dRespOrder.doType_m == BrkLib::DORT_REGISTER_ORDER ) {
			// this is the analog as order is installed

			LOG_SYMBOL_SIGNAL( getRunName(), getRtdataProvider(), dRespOrder.order_m.symbol_m, 
				"DO Response - delayed order registered: " << dRespOrder.order_m.symbol_m << 
				", direction: " << BrkLib::tradeDirection2String(direction) <<
				", exec DO price: " << dRespOrder.execPrice_m << 
				", cur price: " << cur_price <<
				", comment: [ " << dRespOrder.comment_m << " ]"	<<
				", context: " << (myCtxPtr ? myCtxPtr->toString() : "NULL")  <<
				", order type: " << BrkLib::orderTypeBitwise2String(dRespOrder.order_m.orderType_m)
				);

			if (myCtxPtr == NULL) {
				return; 
			}



			if (myCtxPtr->getState().getClassId() == order_issue_state_id) {
				// main order
				TE2_Confirm_Order_Issued_Open event_i;
				event_i.brokerOrderId_m = dRespOrder.order_m.brokerOrderID_m;

				ordersMachines_m.processEvent< TE2_Confirm_Order_Issued_Open >(dRespOrder.order_m.symbol_m,event_i);
			}
			else if (myCtxPtr->getState().getClassId() == tp_issue_state_id) {
				// TP
				TE2_Confirm_Order_Issued_TP event_i;
				event_i.brokerOrderId_m = dRespOrder.order_m.brokerOrderID_m;
				ordersMachines_m.processEvent< TE2_Confirm_Order_Issued_TP >(dRespOrder.order_m.symbol_m,event_i);
			}
			else if (myCtxPtr->getState().getClassId() == sl_issue_state_id) {
				// SL
				TE2_Confirm_Order_Issued_SL event_i;

				event_i.brokerOrderId_m = dRespOrder.order_m.brokerOrderID_m;
				ordersMachines_m.processEvent< TE2_Confirm_Order_Issued_SL >(dRespOrder.order_m.symbol_m,event_i);
			}


		}
		else if (dRespOrder.doType_m == BrkLib::DORT_UNREGISTER_ORDER) {
			//cancelled

			LOG_SYMBOL_SIGNAL( getRunName(), getRtdataProvider(), dRespOrder.order_m.symbol_m, 
				"DO Response - delayed order cancelled: " << dRespOrder.order_m.symbol_m << 
				", direction: " << BrkLib::tradeDirection2String(direction) <<
				", comment: [ " << dRespOrder.comment_m << " ]" <<
				", context: " << (myCtxPtr ? myCtxPtr->toString() : "NULL")  <<
				", order type: " << BrkLib::orderTypeBitwise2String(dRespOrder.order_m.orderType_m)
				);

			if (myCtxPtr == NULL) {
				return; 
			}

			if (myCtxPtr->getState().getClassId() == sl_issue_state_id) {
				TE2_Confirm_Order_Cancelled_SL event_i;
				ordersMachines_m.processEvent< TE2_Confirm_Order_Cancelled_SL >(dRespOrder.order_m.symbol_m,event_i);
			}
			else if (myCtxPtr->getState().getClassId() == tp_issue_state_id) {
				TE2_Confirm_Order_Cancelled_TP event_i;
				ordersMachines_m.processEvent< TE2_Confirm_Order_Cancelled_TP >(dRespOrder.order_m.symbol_m,event_i);
			}

		}
		else if (dRespOrder.doType_m == BrkLib::DORT_ISSUE_ORDER) {
			// nothing
			LOG_SYMBOL_SIGNAL( getRunName(), getRtdataProvider(), dRespOrder.order_m.symbol_m, 
				"DO Response - delayed order is going to be issued: " << dRespOrder.order_m.symbol_m << 
				", cur price: " << cur_price <<
				", direction: " << BrkLib::tradeDirection2String(direction) <<
				", comment: [ " << dRespOrder.comment_m << " ]"	<<
				", context: " << (myCtxPtr ? myCtxPtr->toString() : "NULL")	 <<
				", order type: " << BrkLib::orderTypeBitwise2String(dRespOrder.order_m.orderType_m)
				); 
		}

	}
	else if (bresponse.type_m == BrkLib::RT_BrokerResponseMetaInfo) {

	}
	else if (bresponse.type_m == BrkLib::RT_BrokerResponseOrder) {

		BrkLib::BrokerResponseOrder const& bRespOrder = (BrkLib::BrokerResponseOrder const&)bresponse;

		double cur_price = -1;
		{
			LOCK_FOR_SCOPE(contextMapMtx_m);

			SymbolContext &ctx = contextMap_m[bRespOrder.symbol_m];
			cur_price = ctx.curTickPrice_m;
		}

		if (cur_price <= 0) {
			LOG_SYMBOL_SIGNAL( getRunName(), getRtdataProvider(), bRespOrder.symbol_m, "Current price is invalid!" );
			return;
		}


		BrkLib::TradeDirection direction = BrkLib::orderTypeToDirection(bRespOrder.opOrderType_m);
		if (bRespOrder.resultCode_m ==  BrkLib::PT_ORDER_ESTABLISHED)	{
			if (bRespOrder.resultDetailCode_m & BrkLib::ODR_ORDER_PENDING) {
				LOG_SYMBOL_SIGNAL( getRunName(), getRtdataProvider(), bRespOrder.symbol_m, 
					"Response - order pending: " << bRespOrder.symbol_m << " - " << bRespOrder.brokerOrderID_m << 
					", direction: " << BrkLib::tradeDirection2String(direction)  <<
					", broker comment: [ " << bRespOrder.brokerComment_m << " ]" <<
					", context: " << (myCtxPtr ? myCtxPtr->toString() : "NULL" ) <<
					", order type: " << BrkLib::orderTypeBitwise2String(bRespOrder.opOrderType_m)
					);

				if (myCtxPtr == NULL) {
					return; 
				}


				if (myCtxPtr->getState().getClassId() == sl_issue_state_id) {
					// SL
					TE2_Confirm_Order_Pending_SL event_i;

					event_i.brokerOrderId_m = bRespOrder.brokerOrderID_m;
					ordersMachines_m.processEvent< TE2_Confirm_Order_Pending_SL >(bRespOrder.symbol_m,event_i);
				}


			} else if (bRespOrder.resultDetailCode_m & BrkLib::ODR_ORDER_INSTALLED) {

				LOG_SYMBOL_SIGNAL( getRunName(), getRtdataProvider(), bRespOrder.symbol_m, 
					"Response - order installed: " << bRespOrder.symbol_m << " - " << bRespOrder.brokerOrderID_m << 
					", direction: " << BrkLib::tradeDirection2String(direction)  <<
					", broker comment: [ " << bRespOrder.brokerComment_m << " ]" <<
					", context: " << (myCtxPtr ? myCtxPtr->toString() : "NULL" ) <<
					", order type: " << BrkLib::orderTypeBitwise2String(bRespOrder.opOrderType_m)
					);

				if (myCtxPtr == NULL) {
					return; 
				}

				if (myCtxPtr->getState().getClassId() == order_issue_state_id) {
					// main order
					TE2_Confirm_Order_Issued_Open event_i;

					event_i.brokerOrderId_m = bRespOrder.brokerOrderID_m;

					ordersMachines_m.processEvent< TE2_Confirm_Order_Issued_Open >(bRespOrder.symbol_m,event_i);
				}
				else if (myCtxPtr->getState().getClassId() == tp_issue_state_id) {
					// TP
					TE2_Confirm_Order_Issued_TP event_i;


					event_i.brokerOrderId_m = bRespOrder.brokerOrderID_m;
					ordersMachines_m.processEvent< TE2_Confirm_Order_Issued_TP >(bRespOrder.symbol_m,event_i);
				}
				else if (myCtxPtr->getState().getClassId() == sl_issue_state_id) {
					// SL
					TE2_Confirm_Order_Issued_SL event_i;

					event_i.brokerOrderId_m = bRespOrder.brokerOrderID_m;
					ordersMachines_m.processEvent< TE2_Confirm_Order_Issued_SL >(bRespOrder.symbol_m,event_i);
				}
			}
			else if (bRespOrder.resultDetailCode_m & BrkLib::ODR_ORDER_EXECUTED) {

				LOG_SYMBOL_SIGNAL( getRunName(), getRtdataProvider(), bRespOrder.symbol_m, 
					"Response - order executed: " << bRespOrder.symbol_m << 
					", direction: " << BrkLib::tradeDirection2String(direction)  <<
					", execution price: " << bRespOrder.opPrice_m << 
					", broker position ID: " << bRespOrder.brokerPositionID_m << 
					", cur price: " << cur_price << 
					", broker comment: [ " << bRespOrder.brokerComment_m << " ]" <<
					", context: " << (myCtxPtr ? myCtxPtr->toString() : "NULL" ) <<
					", order type: " << BrkLib::orderTypeBitwise2String(bRespOrder.opOrderType_m)
					);

				if (myCtxPtr == NULL) {
					return; 
				}


				if (bRespOrder.opPrice_m > 0) {
					if (myCtxPtr->getState().getClassId() == order_issue_state_id) {
						// main order
						TE2_Confirm_Order_Executed_Open event_i;

						event_i.brokerOrderId_m = bRespOrder.brokerOrderID_m;
						event_i.brokerPositionId_m = bRespOrder.brokerPositionID_m;
						event_i.executionPrice_m = bRespOrder.opPrice_m;
						//event_i.executionPrice_m = cur_price;

						ordersMachines_m.processEvent< TE2_Confirm_Order_Executed_Open >(bRespOrder.symbol_m,event_i);
					}
					else if (myCtxPtr->getState().getClassId() == tp_issue_state_id) {
						// TP
						TE2_Confirm_Order_Executed_TP event_i;
						ordersMachines_m.processEvent< TE2_Confirm_Order_Executed_TP >(bRespOrder.symbol_m,event_i);
					}
					else if (myCtxPtr->getState().getClassId() == sl_issue_state_id) {
						// SL
						TE2_Confirm_Order_Executed_SL event_i;
						ordersMachines_m.processEvent< TE2_Confirm_Order_Executed_SL >(bRespOrder.symbol_m,event_i);
					}
					// here we are trying to close by market
					else if (myCtxPtr->getState().getClassId() == TS2_Close_By_Market::CLASS_ID) {
						TE2_Confirm_Order_Executed_Market_Close event_i;
						ordersMachines_m.processEvent< TE2_Confirm_Order_Executed_Market_Close >(bRespOrder.symbol_m,event_i);
					}

				}
			}
		}
		else if (bRespOrder.resultCode_m==BrkLib::PT_ORDER_CANCELLED) {

			// participate only in cancellations
			MyBrokerContext const* myCauseCtxPtr = (MyBrokerContext const*)bresponse.causeContext_m.getContext();

			LOG_SYMBOL_SIGNAL( getRunName(), getRtdataProvider(), bRespOrder.symbol_m, 
				"Response - order cancelled: " << bRespOrder.symbol_m << 
				", direction: " << BrkLib::tradeDirection2String(direction)  <<
				", broker comment: [ " << bRespOrder.brokerComment_m << " ]" <<
				", context: " << (myCtxPtr ? myCtxPtr->toString() : "NULL" ) <<
				", cause context: " << (myCauseCtxPtr ? myCauseCtxPtr->toString() : "NULL")	<<
				", order type: " << BrkLib::orderTypeBitwise2String(bRespOrder.opOrderType_m)
				);

			if (myCtxPtr == NULL) {
				return; 
			}

			// here as a context there is an original order

			if (myCtxPtr->getState().getClassId() == sl_issue_state_id) {
				// SL
				TE2_Confirm_Order_Cancelled_SL event_i;
				ordersMachines_m.processEvent< TE2_Confirm_Order_Cancelled_SL >(bRespOrder.symbol_m,event_i);
			}
			else if (myCtxPtr->getState().getClassId() == tp_issue_state_id) {
				// TP
				TE2_Confirm_Order_Cancelled_TP event_i;
				ordersMachines_m.processEvent< TE2_Confirm_Order_Cancelled_TP >(bRespOrder.symbol_m,event_i);
			}
		}
		else if (bRespOrder.resultCode_m==BrkLib::PT_GENERAL_ERROR_HAPPENED) {

			LOG_SYMBOL_SIGNAL( getRunName(), getRtdataProvider(), bRespOrder.symbol_m, "Response - order error: " 
				<< bRespOrder.symbol_m <<
				", direction: " << BrkLib::tradeDirection2String(direction)  <<
				", broker comment: [ " << bRespOrder.brokerComment_m << " ]" <<
				", cur price: " << cur_price << 
				", broker order ID: " << bRespOrder.brokerOrderID_m << 
				", broker position ID: " << bRespOrder.brokerPositionID_m << 
				", context: " << (myCtxPtr ? myCtxPtr->toString() : "NULL" ) <<
				", order type: " << BrkLib::orderTypeBitwise2String(bRespOrder.opOrderType_m)
				);

			if (myCtxPtr == NULL) {
				return; 
			}


			if (myCtxPtr->getState().getClassId() == order_issue_state_id) {
				//open order
				TE2_Confirm_Order_Error_Open event_i;

				event_i.brokerOrderId_m =	 bRespOrder.brokerOrderID_m;
				event_i.reason = bRespOrder.brokerComment_m;

				ordersMachines_m.processEvent< TE2_Confirm_Order_Error_Open >(bRespOrder.symbol_m,event_i);
			}
			else if (myCtxPtr->getState().getClassId() == tp_issue_state_id) {
				// TP order
				TE2_Confirm_Order_Error_TP event_i;

				event_i.brokerOrderId_m =	 bRespOrder.brokerOrderID_m;
				event_i.reason = bRespOrder.brokerComment_m;

				ordersMachines_m.processEvent< TE2_Confirm_Order_Error_TP >(bRespOrder.symbol_m,event_i);
			}
			else if (myCtxPtr->getState().getClassId() == sl_issue_state_id) {
				// SL order
				TE2_Confirm_Order_Error_SL event_i;

				event_i.brokerOrderId_m =	 bRespOrder.brokerOrderID_m;
				event_i.reason = bRespOrder.brokerComment_m;


				ordersMachines_m.processEvent< TE2_Confirm_Order_Error_SL >(bRespOrder.symbol_m,event_i);
			}
			else if (myCtxPtr->getState().getClassId() == TS2_Need_Clear_SL::CLASS_ID) {
				// cancel	SL
				TE2_Confirm_Order_Error_Cancel_SL	event_i;

				event_i.brokerOrderId_m =	 bRespOrder.brokerOrderID_m;
				event_i.reason = bRespOrder.brokerComment_m;

				ordersMachines_m.processEvent< TE2_Confirm_Order_Error_Cancel_SL >(bRespOrder.symbol_m,event_i);
			}
			else if (myCtxPtr->getState().getClassId() == TS2_Need_Clear_TP::CLASS_ID) {
				// cancel	SL
				TE2_Confirm_Order_Error_Cancel_TP	event_i;

				event_i.brokerOrderId_m =	 bRespOrder.brokerOrderID_m;
				event_i.reason = bRespOrder.brokerComment_m;

				ordersMachines_m.processEvent< TE2_Confirm_Order_Error_Cancel_TP >(bRespOrder.symbol_m,event_i);

			}
			// here we are trying to close by market
			else if (myCtxPtr->getState().getClassId() == TS2_Close_By_Market::CLASS_ID) {
				TE2_Confirm_Order_Error_Market_Close event_i;
				event_i.reason_m =  bRespOrder.brokerComment_m;

				ordersMachines_m.processEvent< TE2_Confirm_Order_Error_Market_Close >(bRespOrder.symbol_m,event_i);
			}


		}

	}
}

void CAlgMisha::onDataArrived(
															HlpStruct::RtData const& rtdata, 
															Inqueue::ContextAccessor& historyaccessor, 
															HlpStruct::TradingParameters const& descriptor,
															BrkLib::BrokerBase* brokerlib,
															CppUtils::String const& runName,
															CppUtils::String const& comment
															)
{
	// request for metainfo
	/*
	BrkLib::BrokerBase* brk = getAlgHolder().getBroker2();
	if (brk != NULL && brokerConnectDummy_m != NULL) {

	BrkLib::Order order;

	order.provider_m = rtDataProvider_m;
	order.symbol_m = "GAZP";
	order.orderType_m = BrkLib::OP_GET_SYMBOL_METAINFO;

	BrkLib::BrokerOperation oper(*brokerConnectDummy_m);
	oper.unblockingIssueOrder(order);

	delete brokerConnectDummy_m;
	brokerConnectDummy_m = NULL;
	}
	*/

	if (rtdata.getProvider2() == rtDataProvider_m) {
		// resolve context

		double prev_tick_price;
		{
			LOCK_FOR_SCOPE(contextMapMtx_m);

			SymbolContext &ctx = contextMap_m[rtdata.getSymbol2()];
			ctx.curTime_m = rtdata.getTime();
			prev_tick_price = ctx.prevTickPrice_m;
		}

		// send event
		TE_On_Tick_Data	event_i;
		event_i.curPrice_m = rtdata.getRtData().bid_m;
		event_i.prevPrice_m = prev_tick_price;
		tradingMachines_m.processEvent<TE_On_Tick_Data>(rtdata.getSymbol2(), event_i);

		{
			LOCK_FOR_SCOPE(contextMapMtx_m);
			SymbolContext &ctx = contextMap_m[rtdata.getSymbol2()];

			// update context
			ctx.prevTickPrice_m = ctx.curTickPrice_m;
			ctx.curTickPrice_m = rtdata.getRtData().bid_m;
		}

		// check execution
		if (tradeParam_m.onlyMarketOrders_m)
			checkDelayedOrderExecution(rtdata);

	}



}

void CAlgMisha::onRtProviderSynchEvent (
																				Inqueue::ContextAccessor& contextaccessor, HlpStruct::RtData::SynchType const synchFlg,
																				double const& synchTime,
																				CppUtils::String const& providerName,
																				CppUtils::String const& message
																				)
{
	// RT provider events - must check for sisconnections
	if (synchFlg==HlpStruct::RtData::SYNC_EVENT_PROVIDER_START) {
		LOG_COMMON(connectRunName_m,rtDataProvider_m,BrkLib::AP_MEDIUM,"RT Provider started: " << providerName);
	}
	else if (synchFlg==HlpStruct::RtData::SYNC_EVENT_PROVIDER_FINISH) {
		LOG_COMMON(connectRunName_m,rtDataProvider_m,BrkLib::AP_MEDIUM,"RT Provider finished: " << providerName);

		// reset everything
		//bool result;
		//CppUtils::String log;
		//contextaccessor.requestRunExternalPackage("german_starter.jar", true, result, log);

	}
	else if (synchFlg==HlpStruct::RtData::SYNC_EVENT_PROVIDER_TRANSMIT_ERROR) {
		LOG_COMMON(connectRunName_m,rtDataProvider_m,BrkLib::AP_HIGH,"RT Provider transmit error: " << providerName << " - " << message);


	}
	else if (synchFlg==HlpStruct::RtData::SYNC_EVENT_CUSTOM_MESSAGE) {
		LOG_COMMON(connectRunName_m,rtDataProvider_m,BrkLib::AP_HIGH,"RT Provider custom message: " << providerName << " - " << message);
	}
}

void CAlgMisha::onLevel2DataArrived(
																		HlpStruct::RtLevel2Data const& level2data,
																		Inqueue::ContextAccessor& historyaccessor, 
																		HlpStruct::TradingParameters const& descriptor,
																		BrkLib::BrokerBase* brokerlib,
																		CppUtils::String const& runName,
																		CppUtils::String const& comment
																		)
{
	// 
	if (level2data.getProvider2() == rtDataProvider_m) {
		TE_On_Level2_Data event_i;

		{
			LOCK_FOR_SCOPE(contextMapMtx_m);
			SymbolContext &ctx = contextMap_m[level2data.getSymbol2()];

			event_i.curTickPrice_m = ctx.curTickPrice_m;
		}


		event_i.level2_m = level2data;


		tradingMachines_m.processEvent<TE_On_Level2_Data>(level2data.getSymbol2(), event_i);


	}
}

void CAlgMisha::onLevel1DataArrived(
																		HlpStruct::RtLevel1Data const& level1data,
																		Inqueue::ContextAccessor& historyaccessor, 
																		HlpStruct::TradingParameters const& descriptor,
																		BrkLib::BrokerBase* brokerlib,
																		CppUtils::String const& runName,
																		CppUtils::String const& comment
																		)
{

}

void CAlgMisha::onEventArrived(
															 Inqueue::ContextAccessor& contextaccessor, 
															 HlpStruct::CallingContext& callContext,
															 bool& result,
															 CppUtils::String& customOutputData
															 )
{
	customOutputData = "FAILED";
	CppUtils::String const& command = callContext.getCallCustomName();

	CppUtils::String const& data = callContext.getParameter<BrkLib::String_Proxy>("custom_data").get();

	// parse as simple structure
	HlpStruct::XmlParameter xmlparam_data;
	HlpStruct::HumanSerializer::deserializeXmlParameter(data, xmlparam_data);

	if (command=="hallo") {

	}
	else if (command=="dump_symbol") {
		CppUtils::String const& symbol = xmlparam_data.getParameter("symbol").getAsString();

		exportSymbol(contextaccessor, symbol);

		LOG_COMMON(connectRunName_m,rtDataProvider_m, BrkLib::AP_MEDIUM, "Symbol dumped");
	}  
	else if (command=="start_trade") {

		allowTrading_m = BrkLib::TD_BOTH;
		if ( xmlparam_data.isKeyExists("direction")) {
			CppUtils::String  direction = xmlparam_data.getParameter("direction").getAsString();

			CppUtils::toupper( direction );
			if (direction == "SHORT") {
				allowTrading_m = BrkLib::TD_SHORT;
				LOG_COMMON(connectRunName_m,rtDataProvider_m, BrkLib::AP_MEDIUM, "Allowed SHORT broker operations");
			}
			else if (direction == "LONG") {
				allowTrading_m = BrkLib::TD_LONG;
				LOG_COMMON(connectRunName_m,rtDataProvider_m, BrkLib::AP_MEDIUM, "Allowed LONG broker operations");
			}
			else if (direction == "BOTH") {
				allowTrading_m = BrkLib::TD_BOTH;
				LOG_COMMON(connectRunName_m,rtDataProvider_m, BrkLib::AP_MEDIUM, "Allowed LONG/SHORT broker operations");
			}

		}
		else	{
			allowTrading_m = BrkLib::TD_BOTH;
			LOG_COMMON(connectRunName_m,rtDataProvider_m, BrkLib::AP_MEDIUM, "Allowed LONG/SHORT broker operations");
		}




	}
	else if (command=="stop_trade") {
		allowTrading_m = BrkLib::TD_NONE;
		LOG_COMMON(connectRunName_m,rtDataProvider_m, BrkLib::AP_MEDIUM, "Prohibited broker operations broker operations");

	}
	else if (command=="load_data") {
		bool loaded = contextaccessor.loadDataFromMaster(historyDataProfile_m, false);

		if (!loaded)
			THROW(CppUtils::OperationFailed, "exc_CannotLoadHistoryData", "ctx_onEventArrived", "");


		LOG_COMMON(connectRunName_m,rtDataProvider_m, BrkLib::AP_MEDIUM, "Loaded history OK, working with minutes period: " << tradeParam_m.tradePeriodMinutes_m);
	}
	else if (command=="manual_fix") {
		TE_Manual_Fix event_i;
		tradingMachines_m.processEvent<TE_Manual_Fix>(xmlparam_data.getParameter("symbol").getAsString(), event_i);
	}
	else if (command=="reset_track") {
		// reseting tracking search
		TE_Reset event_i;

		tradingMachines_m.processEvent<TE_Reset>(xmlparam_data.getParameter("symbol").getAsString(), event_i);
	}
	else if (command=="dump_cache") {
		// this command dump cache file to see what we have in memory
		CppUtils::String const& fileTmp = CppUtils::getTempPath() + "\\dump_" + CppUtils::createValidFileName(getRunName()) + "_" + CppUtils::long2String(CppUtils::getTime()) + ".txt";
		CppUtils::saveContentInFile2(fileTmp,  contextaccessor.makeDebugDump(true));
	}
	else
		THROW(CppUtils::OperationFailed, "exc_InvalidCommand", "ctx_onEventArrived", command);

	// no exceptions
	customOutputData ="OK";
}

void CAlgMisha::onThreadStarted(
																Inqueue::ContextAccessor& contextaccessor, bool const firstLib, bool const lastLib
																)
{

}

void CAlgMisha::onThreadFinished(
																 Inqueue::ContextAccessor& contextaccessor, HlpStruct::TradingParameters const& tradeParams, bool const firstLib, bool const lastLib
																 )
{

}

CppUtils::String const& CAlgMisha::getName() const
{
	static const CppUtils::String name("[ alg lib: CAlgMisha ]");
	return name;
}



void CAlgMisha::initStateMachine(CppUtils::StringSet const& symbols)
{



	for(CppUtils::StringSet::const_iterator it = symbols.begin(); it != symbols.end(); it++) {
		MachineContext::MachineContextStructure ctx(*it, *this, MAIN_FSM);
		// machine managing basse algorithm
		tradingMachines_m.getMachineMap()[*it] = new CppUtils::StateMachine<MachineContext>(&ctx);

		// machine managing orders
		MachineContext::MachineContextStructure ctx_o(*it, *this, ORDER_FSM);
		ordersMachines_m.getMachineMap()[*it] =	 new CppUtils::StateMachine<MachineContext>(&ctx_o);


		LOG_SYMBOL(getRunName(),getRtdataProvider(), *it, "State machines created");
	}

	// main FSM
	for(map<CppUtils::String, CppUtils::StateMachine<MachineContext>* >::iterator it = tradingMachines_m.getMachineMap().begin(); it != tradingMachines_m.getMachineMap().end(); it++) {
		// register states
		CppUtils::StateMachine<MachineContext> & machine = *it->second;

		CREATE_STATE(s_0,CppUtils::InitialState<MachineContext>,machine);
		CREATE_STATE(s_TS_Inconsistent,TS_Inconsistent,machine);
		CREATE_STATE(s_TS_Initial,TS_Initial,machine);
		CREATE_STATE(s_TS_LevelTrack,TS_LevelTrack,machine);
		CREATE_STATE(s_TS_IssueBuy,TS_IssueBuy,machine);
		CREATE_STATE(s_TS_IssueSell,TS_IssueSell,machine);
		CREATE_STATE(s_TS_Fish_Long,TS_Fish_Long,machine);
		CREATE_STATE(s_TS_Fish_Short,TS_Fish_Short,machine);

		// matrix
		s_0->connect(s_TS_Initial, CppUtils::Unconditional());
		s_TS_Initial->connect<TE_Detected_Big_Volume>(s_TS_LevelTrack);
		s_TS_Initial->connect<TE_On_Level2_Data>(s_TS_Initial);

		s_TS_LevelTrack->connect<TE_Price_Go_Far>(s_TS_Initial);
		s_TS_LevelTrack->connect<TE_Reset>(s_TS_Initial);
		s_TS_LevelTrack->connect<TE_On_Tick_Data>(s_TS_LevelTrack);
		s_TS_LevelTrack->connect<TE_On_Timer>(s_TS_LevelTrack);
		s_TS_LevelTrack->connect<TE_Detect_Time_Excess>(s_TS_Initial);
		if (tradeParam_m.tradeMode_m == TradeParam::TM_OVERCOME_LEVEL) {
			s_TS_LevelTrack->connect<TE_Price_Crossed_Up>(s_TS_IssueBuy);
			s_TS_LevelTrack->connect<TE_Price_Crossed_Down>(s_TS_IssueSell);
		}
		else if (tradeParam_m.tradeMode_m == TradeParam::TM_REFLECTION_LEVEL) {
			s_TS_LevelTrack->connect<TE_Price_Crossed_Up>(s_TS_IssueSell);
			s_TS_LevelTrack->connect<TE_Price_Crossed_Down>(s_TS_IssueBuy);
		}
		else
			THROW(CppUtils::OperationFailed, "exc_InvalidTradeParam", "ctx_InitStateMachine", "");

		s_TS_IssueBuy->connect<TE_Confirm_Orders_Setup_Long>(s_TS_Fish_Long);
		s_TS_IssueBuy->connect<TE_Confirm_Orders_Setup_Error_Long>(s_TS_Inconsistent);

		s_TS_IssueSell->connect<TE_Confirm_Orders_Setup_Short>(s_TS_Fish_Short);
		s_TS_IssueSell->connect<TE_Confirm_Orders_Setup_Error_Short>(s_TS_Inconsistent);

		s_TS_Fish_Long->connect<TE_Clear_Long>(s_TS_Initial);
		s_TS_Fish_Short->connect<TE_Clear_Short>(s_TS_Initial);

		machine.init();

	}

	// order FSM
	for(map<CppUtils::String, CppUtils::StateMachine<MachineContext>* >::iterator it = ordersMachines_m.getMachineMap().begin(); it != ordersMachines_m.getMachineMap().end(); it++) {
		CppUtils::StateMachine<MachineContext> & machine = *it->second;

		CREATE_STATE(s_02,CppUtils::InitialState<MachineContext>,machine);
		CREATE_STATE(s_TS2_Inconsistent,TS2_Inconsistent,machine);
		CREATE_STATE(s_TS2_Initial,TS2_Initial,machine);
		CREATE_STATE(s_TS2_Pending,TS2_Pending,machine);
		CREATE_STATE(s_TS2_Ready_Start,TS2_Ready_Start,machine);
		CREATE_STATE(s_TS2_SL_Pending,TS2_SL_Pending,machine);
		CREATE_STATE(s_TS2_In_Position,TS2_In_Position,machine);
		CREATE_STATE(s_TS2_Ready,TS2_Ready,machine);

		CREATE_STATE(s_TS2_Need_Clear_SL,TS2_Need_Clear_SL,machine);
		CREATE_STATE(s_TS2_Need_Clear_TP,TS2_Need_Clear_TP,machine);
		CREATE_STATE(s_TS2_Trade_Finished,TS2_Trade_Finished,machine);

		CREATE_STATE(s_TS2_Check_SL_Error,TS2_Check_SL_Error,machine);
		CREATE_STATE(s_TS2_Close_By_Market,TS2_Close_By_Market,machine);



		// matrix	 for order machine
		s_02->connect(s_TS2_Initial, CppUtils::Unconditional());

		s_TS2_Initial->connect<TE2_Start_Signal>(s_TS2_Ready_Start);

		s_TS2_Ready_Start->connect<TE2_Confirm_Order_Issued_Open>(s_TS2_Pending);
		s_TS2_Ready_Start->connect<TE2_Confirm_Order_Error_Open>(s_TS2_Inconsistent);

		s_TS2_Pending->connect<TE2_Confirm_Order_Executed_Open>(s_TS2_SL_Pending);
		s_TS2_Pending->connect<TE2_Confirm_Order_Error_Open>	(s_TS2_Inconsistent);

		s_TS2_SL_Pending->connect<TE2_Confirm_Order_Issued_SL>(s_TS2_In_Position);
		s_TS2_SL_Pending->connect<TE2_Confirm_Order_Pending_SL>(s_TS2_In_Position); // if SMARTCOM do not want to give install event
		s_TS2_SL_Pending->connect<TE2_Confirm_Order_Error_SL>(s_TS2_Check_SL_Error);

		s_TS2_Check_SL_Error->connect<TE2_Common_Error_SL>(s_TS2_Inconsistent);
		s_TS2_Check_SL_Error->connect<TE2_Error_Invalid_Stop_Price_SL>(s_TS2_Close_By_Market);

		s_TS2_Close_By_Market->connect<TE2_Confirm_Order_Error_Market_Close>(s_TS2_Inconsistent);
		s_TS2_Close_By_Market->connect<TE2_Confirm_Order_Executed_Market_Close>(s_TS2_Trade_Finished);

		s_TS2_In_Position->connect<TE2_Confirm_Order_Issued_TP>(s_TS2_Ready);		 
		s_TS2_In_Position->connect<TE2_Confirm_Order_Error_TP>(s_TS2_Inconsistent);


		s_TS2_Ready->connect<TE2_Confirm_Order_Executed_SL>(s_TS2_Need_Clear_TP);
		s_TS2_Ready->connect<TE2_Confirm_Order_Executed_TP>(s_TS2_Need_Clear_SL);
		s_TS2_Ready->connect<TE2_Confirm_Order_Error_SL>(s_TS2_Inconsistent);
		s_TS2_Ready->connect<TE2_Confirm_Order_Error_SL>(s_TS2_Inconsistent);


		s_TS2_Need_Clear_SL->connect<TE2_Confirm_Order_Error_Cancel_SL>(s_TS2_Inconsistent);
		s_TS2_Need_Clear_SL->connect<TE2_Confirm_Order_Cancelled_SL>(s_TS2_Trade_Finished);
		s_TS2_Need_Clear_TP->connect<TE2_Confirm_Order_Error_Cancel_TP>(s_TS2_Inconsistent);
		s_TS2_Need_Clear_TP->connect<TE2_Confirm_Order_Cancelled_TP>(s_TS2_Trade_Finished);


		s_TS2_Trade_Finished->connect(s_TS2_Initial, CppUtils::Unconditional());

		// init
		machine.init();


	}


}

void CAlgMisha::deinitStateMachine()
{

	ordersMachines_m.clear();
	tradingMachines_m.clear();

}



double CAlgMisha::getCurrentPrice(CppUtils::String const& symbol) const
{
	{
		LOCK_FOR_SCOPE(contextMapMtx_m);

		map<CppUtils::String, SymbolContext>::const_iterator it =	 contextMap_m.find(symbol);
		if (it != contextMap_m.end())
			return it->second.curTickPrice_m;
	}

	return -1.0;
}

void CAlgMisha::issueBuyOrder(MyBrokerContext const& ctx, CppUtils::String const& symbol,double const& volume)
{

	BrkLib::Order order;

	//order.ID_m.generate();
	order.provider_m = rtDataProvider_m;
	order.symbol_m = symbol;
	order.orderType_m = BrkLib::OP_BUY;
	order.orderValidity_m = BrkLib::OV_DAY;
	order.orVolume_m = volume;
	order.context_m.setContext(ctx);

	if (!tradeParam_m.isDemoTrade_m) {
		BrkLib::BrokerOperation oper(*brokerConnect_m);
		oper.unblockingIssueOrder(order);
	} else {
		BrkLib::BrokerOperation oper(*brokerConnectDemo_m);
		oper.unblockingIssueOrder(order);
	}

}

void CAlgMisha::issueBuyOrder_Limit(MyBrokerContext const& ctx, double const& limitPrice, CppUtils::String const& symbol,double const& volume)
{

	BrkLib::Order order;

	//order.ID_m.generate();
	order.provider_m = rtDataProvider_m;
	order.symbol_m = symbol;
	order.orderType_m = BrkLib::OP_BUY_LIMIT;
	order.orderValidity_m = BrkLib::OV_DAY;
	order.orVolume_m = volume;
	order.orPrice_m =limitPrice; 
	order.context_m.setContext(ctx);


	if (!tradeParam_m.isDemoTrade_m) {
		BrkLib::BrokerOperation oper(*brokerConnect_m);
		oper.unblockingIssueOrder(order);
	} else {
		BrkLib::BrokerOperation oper(*brokerConnectDemo_m);
		oper.unblockingIssueOrder(order);
	}
}

void CAlgMisha::issueSellOrder_Limit(MyBrokerContext const& ctx, double const& limitPrice, CppUtils::String const& symbol, double const& volume)
{

	BrkLib::Order order;

	//order.ID_m.generate();
	order.provider_m = rtDataProvider_m;
	order.symbol_m = symbol;
	order.orderType_m = BrkLib::OP_SELL_LIMIT;
	order.orderValidity_m = BrkLib::OV_DAY;
	order.orVolume_m = volume;
	order.orPrice_m =limitPrice; 
	order.context_m.setContext(ctx);


	if (!tradeParam_m.isDemoTrade_m) {
		BrkLib::BrokerOperation oper(*brokerConnect_m);
		oper.unblockingIssueOrder(order);
	} else {
		BrkLib::BrokerOperation oper(*brokerConnectDemo_m);
		oper.unblockingIssueOrder(order);
	}
}

void CAlgMisha::issueSellOrder(MyBrokerContext const& ctx, CppUtils::String const& symbol,double const& volume)
{

	BrkLib::Order order;

	//order.ID_m.generate();
	order.provider_m = rtDataProvider_m;
	order.symbol_m = symbol;
	order.orderType_m = BrkLib::OP_SELL;
	order.orderValidity_m = BrkLib::OV_DAY;
	order.orVolume_m = volume;
	order.context_m.setContext(ctx);


	if (!tradeParam_m.isDemoTrade_m) {
		BrkLib::BrokerOperation oper(*brokerConnect_m);
		oper.unblockingIssueOrder(order);
	} else {
		BrkLib::BrokerOperation oper(*brokerConnectDemo_m);
		oper.unblockingIssueOrder(order);
	}

}

void CAlgMisha::issueCloseLongOrder(MyBrokerContext const& ctx, CppUtils::String const& symbol, double const& volume, CppUtils::String const& brokerPositionId)
{

	BrkLib::Order order;

	//order.ID_m.generate();
	order.brokerPositionID_m = brokerPositionId;
	order.provider_m = rtDataProvider_m;
	order.symbol_m = symbol;
	order.orderType_m = (tradeParam_m.onlyOpenPosOpes_m ? BrkLib::OP_SELL : BrkLib::OP_CLOSE_LONG);
	order.orderValidity_m = BrkLib::OV_DAY;
	order.context_m.setContext(ctx);

	order.orVolume_m = volume;

	if (tradeParam_m.onlyOpenPosOpes_m && order.brokerPositionID_m.empty()) {
		THROW(CppUtils::OperationFailed, "exc_NeedToHaveBrokerPositionID", "issueCloseLongOrder", BrkLib::getOrderSmallDescription(order));
	}

	if (!tradeParam_m.isDemoTrade_m) {
		BrkLib::BrokerOperation oper(*brokerConnect_m);
		oper.unblockingIssueOrder(order);
	} else {
		BrkLib::BrokerOperation oper(*brokerConnectDemo_m);
		oper.unblockingIssueOrder(order);
	}
}

void CAlgMisha::issueCloseShortOrder(MyBrokerContext const& ctx, CppUtils::String const& symbol, double const& volume, CppUtils::String const& brokerPositionId)
{

	BrkLib::Order order;

	//order.ID_m.generate();
	order.brokerPositionID_m = brokerPositionId;
	order.provider_m = rtDataProvider_m;
	order.symbol_m = symbol;
	order.orderType_m = (tradeParam_m.onlyOpenPosOpes_m ? BrkLib::OP_BUY : BrkLib::OP_CLOSE_SHORT);
	order.orderValidity_m = BrkLib::OV_DAY;
	order.context_m.setContext(ctx);

	order.orVolume_m = volume;

	if (tradeParam_m.onlyOpenPosOpes_m && order.brokerPositionID_m.empty()) {
		THROW(CppUtils::OperationFailed, "exc_NeedToHaveBrokerPositionID", "issueCloseShortOrder", BrkLib::getOrderSmallDescription(order));
	}


	if (!tradeParam_m.isDemoTrade_m) {
		BrkLib::BrokerOperation oper(*brokerConnect_m);
		oper.unblockingIssueOrder(order);
	} else {
		BrkLib::BrokerOperation oper(*brokerConnectDemo_m);
		oper.unblockingIssueOrder(order);
	}

}

void CAlgMisha::issueCloseLongOrder_Limit(MyBrokerContext const& ctx, double const& limitPrice, CppUtils::String const& symbol, double const& volume, CppUtils::String const& brokerPositionId)
{

	BrkLib::Order order;

	//order.ID_m.generate();
	order.brokerPositionID_m = brokerPositionId;
	order.provider_m = rtDataProvider_m;
	order.symbol_m = symbol;
	order.orderType_m = (tradeParam_m.onlyOpenPosOpes_m ? BrkLib::OP_SELL_LIMIT : BrkLib::OP_CLOSE_LONG_LIMIT);
	order.orderValidity_m = BrkLib::OV_DAY;
	order.context_m.setContext(ctx);

	order.orVolume_m = volume;
	order.orPrice_m = limitPrice;

	if (tradeParam_m.onlyOpenPosOpes_m && order.brokerPositionID_m.empty()) {
		THROW(CppUtils::OperationFailed, "exc_NeedToHaveBrokerPositionID", "issueCloseLongOrder_Limit", BrkLib::getOrderSmallDescription(order));
	}

	if (!tradeParam_m.isDemoTrade_m) {
		BrkLib::BrokerOperation oper(*brokerConnect_m);
		oper.unblockingIssueOrder(order);
	} else {
		BrkLib::BrokerOperation oper(*brokerConnectDemo_m);
		oper.unblockingIssueOrder(order);
	}
}

void CAlgMisha::issueCloseShortOrder_Limit(MyBrokerContext const& ctx, double const& limitPrice, CppUtils::String const& symbol, double const& volume,  CppUtils::String const& brokerPositionId)
{

	BrkLib::Order order;

	//order.ID_m.generate();
	order.brokerPositionID_m = brokerPositionId;
	order.provider_m = rtDataProvider_m;
	order.symbol_m = symbol;
	order.orderType_m = (tradeParam_m.onlyOpenPosOpes_m ? BrkLib::OP_BUY_LIMIT : BrkLib::OP_CLOSE_SHORT_LIMIT);
	order.orderValidity_m = BrkLib::OV_DAY;
	order.context_m.setContext(ctx);

	order.orVolume_m = volume;
	order.orPrice_m = limitPrice;

	if (tradeParam_m.onlyOpenPosOpes_m && order.brokerPositionID_m.empty()) {
		THROW(CppUtils::OperationFailed, "exc_NeedToHaveBrokerPositionID", "issueCloseShortOrder_Limit", BrkLib::getOrderSmallDescription(order));
	}

	if (!tradeParam_m.isDemoTrade_m) {
		BrkLib::BrokerOperation oper(*brokerConnect_m);
		oper.unblockingIssueOrder(order);
	} else {
		BrkLib::BrokerOperation oper(*brokerConnectDemo_m);
		oper.unblockingIssueOrder(order);
	}
}

void CAlgMisha::issueCloseLongOrder_Stop(MyBrokerContext const& ctx, double const& stopPrice, CppUtils::String const& symbol, double const& volume,  CppUtils::String const& brokerPositionId)
{

	BrkLib::Order order;

	//order.ID_m.generate();
	order.brokerPositionID_m = brokerPositionId;
	order.provider_m = rtDataProvider_m;
	order.symbol_m = symbol;
	order.orderType_m = (tradeParam_m.onlyOpenPosOpes_m ? BrkLib::OP_SELL_STOP : BrkLib::OP_CLOSE_LONG_STOP );
	order.orderValidity_m = BrkLib::OV_DAY;
	order.context_m.setContext(ctx);

	order.orVolume_m = volume;
	order.orStopPrice_m = stopPrice;

	if (tradeParam_m.onlyOpenPosOpes_m && order.brokerPositionID_m.empty()) {
		THROW(CppUtils::OperationFailed, "exc_NeedToHaveBrokerPositionID", "issueCloseLongOrder_Stop", BrkLib::getOrderSmallDescription(order));
	}

	if (!tradeParam_m.isDemoTrade_m) {
		BrkLib::BrokerOperation oper(*brokerConnect_m);
		oper.unblockingIssueOrder(order);
	} else {
		BrkLib::BrokerOperation oper(*brokerConnectDemo_m);
		oper.unblockingIssueOrder(order);
	}
}

void CAlgMisha::issueCloseShortOrder_Stop(MyBrokerContext const& ctx, double const& stopPrice, CppUtils::String const& symbol, double const& volume, CppUtils::String const& brokerPositionId)
{

	BrkLib::Order order;

	//order.ID_m.generate();
	order.brokerPositionID_m = brokerPositionId;
	order.provider_m = rtDataProvider_m;

	order.symbol_m = symbol;
	order.orderType_m = (tradeParam_m.onlyOpenPosOpes_m ? BrkLib::OP_BUY_STOP : BrkLib::OP_CLOSE_SHORT_STOP );
	order.orderValidity_m = BrkLib::OV_DAY;
	order.context_m.setContext(ctx);

	order.orVolume_m = volume;
	order.orStopPrice_m = stopPrice;

	if (tradeParam_m.onlyOpenPosOpes_m && order.brokerPositionID_m.empty()) {
		THROW(CppUtils::OperationFailed, "exc_NeedToHaveBrokerPositionID", "issueCloseShortOrder_Stop", BrkLib::getOrderSmallDescription(order));
	}

	if (!tradeParam_m.isDemoTrade_m) {
		BrkLib::BrokerOperation oper(*brokerConnect_m);
		oper.unblockingIssueOrder(order);
	} else {
		BrkLib::BrokerOperation oper(*brokerConnectDemo_m);
		oper.unblockingIssueOrder(order);
	}
}

void CAlgMisha::issueCloseLongOrder_StopLimit(MyBrokerContext const& ctx, double const& stopPrice, double const& limitPrice, CppUtils::String const& symbol, double const& volume, CppUtils::String const& brokerPositionId)
{

	BrkLib::Order order;

	//order.ID_m.generate();
	order.brokerPositionID_m = brokerPositionId;
	order.provider_m = rtDataProvider_m;
	order.symbol_m = symbol;
	order.orderType_m = (tradeParam_m.onlyOpenPosOpes_m ? BrkLib::OP_SELL_STOP_LIMIT : BrkLib::OP_CLOSE_LONG_STOP_LIMIT );
	order.orderValidity_m = BrkLib::OV_DAY;
	order.context_m.setContext(ctx);

	order.orVolume_m = volume;
	order.orStopPrice_m = stopPrice;
	order.orPrice_m = limitPrice;

	if (tradeParam_m.onlyOpenPosOpes_m && order.brokerPositionID_m.empty()) {
		THROW(CppUtils::OperationFailed, "exc_NeedToHaveBrokerPositionID", "issueCloseLongOrder_StopLimit", BrkLib::getOrderSmallDescription(order));
	}

	if (!tradeParam_m.isDemoTrade_m) {
		BrkLib::BrokerOperation oper(*brokerConnect_m);
		oper.unblockingIssueOrder(order);
	} else {
		BrkLib::BrokerOperation oper(*brokerConnectDemo_m);
		oper.unblockingIssueOrder(order);
	}
}

void CAlgMisha::issueCloseShortOrder_StopLimit(MyBrokerContext const& ctx, double const& stopPrice, double const& limitPrice, CppUtils::String const& symbol, double const& volume, CppUtils::String const& brokerPositionId)
{

	BrkLib::Order order;

	//order.ID_m.generate();
	order.brokerPositionID_m = brokerPositionId;
	order.provider_m = rtDataProvider_m;
	order.symbol_m = symbol;
	order.orderType_m = (tradeParam_m.onlyOpenPosOpes_m ? BrkLib::OP_BUY_STOP_LIMIT :  BrkLib::OP_CLOSE_SHORT_STOP_LIMIT );
	order.orderValidity_m = BrkLib::OV_DAY;
	order.context_m.setContext(ctx);

	order.orVolume_m = volume;
	order.orStopPrice_m = stopPrice;
	order.orPrice_m = limitPrice;

	if (tradeParam_m.onlyOpenPosOpes_m && order.brokerPositionID_m.empty()) {
		THROW(CppUtils::OperationFailed, "exc_NeedToHaveBrokerPositionID", "ctx_issueCloseShortOrder_StopLimit", BrkLib::getOrderSmallDescription(order));
	}

	if (!tradeParam_m.isDemoTrade_m) {
		BrkLib::BrokerOperation oper(*brokerConnect_m);
		oper.unblockingIssueOrder(order);
	} else {
		BrkLib::BrokerOperation oper(*brokerConnectDemo_m);
		oper.unblockingIssueOrder(order);
	}
}

void CAlgMisha::issueCancelOrder(MyBrokerContext const& ctx, CppUtils::String const& symbol, CppUtils::String const& brokerPositionId, CppUtils::String const& brokerOrderId)
{

	BrkLib::Order order;

	//order.ID_m.generate();
	order.provider_m = rtDataProvider_m;
	order.brokerPositionID_m = brokerPositionId;
	order.brokerOrderID_m = brokerOrderId;
	order.symbol_m = symbol;
	order.orderType_m = BrkLib::OP_ORDER_CANCEL;
	order.orderValidity_m = BrkLib::OV_DAY;
	order.context_m.setContext(ctx);

	if (!tradeParam_m.isDemoTrade_m) {
		BrkLib::BrokerOperation oper(*brokerConnect_m);
		oper.unblockingIssueOrder(order);
	} else {
		BrkLib::BrokerOperation oper(*brokerConnectDemo_m);
		oper.unblockingIssueOrder(order);
	}
}

void CAlgMisha::issueCloseLongOrder_Delayed_TP(MyBrokerContext const& ctx, double const& executionPrice, CppUtils::String const& symbol,double const& volume, CppUtils::String const& brokerPositionId)
{
	// this is intended to close any long order using market order

	BrkLib::Order order;

	//order.ID_m.generate();
	order.brokerPositionID_m = brokerPositionId;
	order.provider_m = rtDataProvider_m;
	order.symbol_m = symbol;
	order.orderType_m = (tradeParam_m.onlyOpenPosOpes_m ? BrkLib::OP_SELL : BrkLib::OP_CLOSE_LONG);
	order.orderValidity_m = BrkLib::OV_DAY;
	order.context_m.setContext(ctx);

	order.orVolume_m = volume;

	if (!tradeParam_m.isDemoTrade_m) {
		// real broker
		getAlgHolder().getBroker()->registerDelayedOrder(
			brokerConnect_m->getSession(), 
			BrkLib::DO_EXEC_PRICE_MORE,
			executionPrice,
			order,
			"issueCloseLongOrder_Delayed_TP"
			);
	}
	else {
		// demo broker

		getAlgHolder().getBroker2()->registerDelayedOrder(
			brokerConnectDemo_m->getSession(), 
			BrkLib::DO_EXEC_PRICE_MORE,
			executionPrice,
			order,
			"issueCloseLongOrder_Delayed_TP"
			);
	}


}

void CAlgMisha::issueCloseShortOrder_Delayed_TP(MyBrokerContext const& ctx, double const& executionPrice, CppUtils::String const& symbol, double const& volume, CppUtils::String const& brokerPositionId)
{

	BrkLib::Order order;

	//order.ID_m.generate();
	order.brokerPositionID_m = brokerPositionId;
	order.provider_m = rtDataProvider_m;
	order.symbol_m = symbol;
	order.orderType_m = (tradeParam_m.onlyOpenPosOpes_m ? BrkLib::OP_BUY : BrkLib::OP_CLOSE_SHORT );
	order.orderValidity_m = BrkLib::OV_DAY;
	order.context_m.setContext(ctx);

	order.orVolume_m = volume;

	if (!tradeParam_m.isDemoTrade_m) {
		// real broker
		getAlgHolder().getBroker()->registerDelayedOrder(
			brokerConnect_m->getSession(), 
			BrkLib::DO_EXEC_PRICE_LESS,
			executionPrice,
			order,
			"issueCloseShortOrder_Delayed_TP"
			);
	}
	else {
		// demo broker

		getAlgHolder().getBroker2()->registerDelayedOrder(
			brokerConnectDemo_m->getSession(), 
			BrkLib::DO_EXEC_PRICE_LESS,
			executionPrice,
			order,
			"issueCloseShortOrder_Delayed_TP"
			);
	}

}


void CAlgMisha::issueCloseLongOrder_Delayed_SL(MyBrokerContext const& ctx, double const& executionPrice, CppUtils::String const& symbol,double const& volume, CppUtils::String const& brokerPositionId)
{
	// this is intended to close any long order using market order

	BrkLib::Order order;

	//order.ID_m.generate();
	order.brokerPositionID_m = brokerPositionId;
	order.provider_m = rtDataProvider_m;
	order.symbol_m = symbol;
	order.orderType_m = (tradeParam_m.onlyOpenPosOpes_m ? BrkLib::OP_SELL : BrkLib::OP_CLOSE_LONG);
	order.orderValidity_m = BrkLib::OV_DAY;
	order.context_m.setContext(ctx);

	order.orVolume_m = volume;

	if (!tradeParam_m.isDemoTrade_m) {
		// real broker
		getAlgHolder().getBroker()->registerDelayedOrder(
			brokerConnect_m->getSession(), 
			BrkLib::DO_EXEC_PRICE_LESS,
			executionPrice,
			order,
			"issueCloseLongOrder_Delayed_SL"
			);
	}
	else {
		// demo broker

		getAlgHolder().getBroker2()->registerDelayedOrder(
			brokerConnectDemo_m->getSession(), 
			BrkLib::DO_EXEC_PRICE_LESS,
			executionPrice,
			order,
			"issueCloseLongOrder_Delayed_SL"
			);
	}


}

void CAlgMisha::issueCloseShortOrder_Delayed_SL(MyBrokerContext const& ctx, double const& executionPrice, CppUtils::String const& symbol, double const& volume, CppUtils::String const& brokerPositionId)
{

	BrkLib::Order order;

	//order.ID_m.generate();
	order.brokerPositionID_m = brokerPositionId;
	order.provider_m = rtDataProvider_m;
	order.symbol_m = symbol;
	order.orderType_m = (tradeParam_m.onlyOpenPosOpes_m ? BrkLib::OP_BUY :BrkLib::OP_CLOSE_SHORT);
	order.orderValidity_m = BrkLib::OV_DAY;
	order.context_m.setContext(ctx);

	order.orVolume_m = volume;

	if (!tradeParam_m.isDemoTrade_m) {
		// real broker
		getAlgHolder().getBroker()->registerDelayedOrder(
			brokerConnect_m->getSession(), 
			BrkLib::DO_EXEC_PRICE_MORE,
			executionPrice,
			order,
			"issueCloseShortOrder_Delayed_SL"
			);
	}
	else {
		// demo broker

		getAlgHolder().getBroker2()->registerDelayedOrder(
			brokerConnectDemo_m->getSession(), 
			BrkLib::DO_EXEC_PRICE_MORE,
			executionPrice,
			order,
			"issueCloseShortOrder_Delayed_SL"
			);
	}

}

void CAlgMisha::checkDelayedOrderExecution(HlpStruct::RtData const& rtdata)
{

	if (!tradeParam_m.isDemoTrade_m) {
		// real
		if (getAlgHolder().getBroker()) {
			getAlgHolder().getBroker()->onDelayedOrderExecutionCheck(brokerConnect_m->getSession(), rtdata.getRtData().bid_m, rtdata.getProvider2(), rtdata.getSymbol2() );
		}
	}
	else {
		// demo
		if (getAlgHolder().getBroker2()) {
			getAlgHolder().getBroker2()->onDelayedOrderExecutionCheck(brokerConnectDemo_m->getSession(), rtdata.getRtData().bid_m, rtdata.getProvider2(), rtdata.getSymbol2() );
		}
	}
}

void CAlgMisha::cancelAllDelayedOrders()
{
	if (!tradeParam_m.isDemoTrade_m) {
		// real
		if (getAlgHolder().getBroker()) {
			getAlgHolder().getBroker()->removeAllDelayedOrders(brokerConnect_m->getSession());
		}
	}
	else {
		// demo
		if (getAlgHolder().getBroker2()) {
			getAlgHolder().getBroker2()->removeAllDelayedOrders(brokerConnectDemo_m->getSession());
		}
	}
}

void CAlgMisha::exportSymbol(Inqueue::ContextAccessor& historyaccessor, CppUtils::String const& symbol, int const cache_id)
{
	CppUtils::String fileExportMame = baseExportPath_m + "\\"+ symbol + ".txt";

	Math::ExporterEngine<Math::PriceSeriesProvider> exporter(fileExportMame, false);
	bool result = historyaccessor.callMathEngine<bool, Math::P_CloseBid, Math::ExporterEngine<Math::PriceSeriesProvider> >(cache_id, exporter);

	if (!result) {
		LOG_SYMBOL_PROBLEM(connectRunName_m,rtDataProvider_m, symbol,"Cannot export symbol to: " << fileExportMame );
	}
}

void CAlgMisha::exportSymbol(Inqueue::ContextAccessor& historyaccessor, CppUtils::String const& symbol)
{
	int cache_symbol_id = historyaccessor.cacheSymbol(barDataProvider_m, symbol, HlpStruct::AP_Minute, tradeParam_m.tradePeriodMinutes_m);

	// need to recalculate max & mins
	if (cache_symbol_id <= 0) {
		LOG_SYMBOL_PROBLEM(connectRunName_m,rtDataProvider_m, symbol,"Cannot cache symbol upon request: "  << symbol);
		return;
	}

	CppUtils::String fileExportMame = baseExportPath_m + "\\"+ symbol + ".txt";

	Math::ExporterEngine<Math::PriceSeriesProvider> exporter(fileExportMame, false);
	bool result = historyaccessor.callMathEngine<bool, Math::P_CloseBid, Math::ExporterEngine<Math::PriceSeriesProvider> >(cache_symbol_id, exporter);

	if (!result) {
		LOG_SYMBOL_PROBLEM(connectRunName_m,rtDataProvider_m, symbol,"Cannot export symbol to: " << fileExportMame );
	}
}

void CAlgMisha::helperThreadFun()
{
	threadStarted_m.signalEvent();
	LOG(MSG_INFO, SMARTCOM_ALG_MISHA, CAlgMisha::getName() << " - helper thread started");
	LOG_COMMON(connectRunName_m, rtDataProvider_m, BrkLib::AP_MEDIUM,  "Helper thread started");




	while(true) {

		// lock
		bool res = shutDownThread_m.lock(1000);
		if (res)
			break;

		try {
			// check if we are in timeout
			for(CppUtils::StringSet::const_iterator it = symbols_m.begin(); it != symbols_m.end(); it++) {
				// timer event
				TE_On_Timer event_i;
				tradingMachines_m.processEvent<TE_On_Timer>(*it, event_i);
			}

		}
		catch(CppUtils::Exception& e) {
			LOG_COMMON(connectRunName_m, rtDataProvider_m, BrkLib::AP_MEDIUM, "Error in helper thread: " << e.message());
		}
#ifdef HANDLE_NATIVE_EXCEPTIONS
		catch(Testframe::Exception& e) {
			LOG_COMMON(connectRunName_m, rtDataProvider_m, BrkLib::AP_MEDIUM, "MSVC error in helper thread: " << e.message());
		}
#endif
		catch(...) {
			LOG_COMMON(connectRunName_m, rtDataProvider_m, BrkLib::AP_MEDIUM,  "Unknown error in helper thread");
		}

	}

	LOG_COMMON(connectRunName_m, rtDataProvider_m, BrkLib::AP_MEDIUM,  "Helper thread finished");
	LOG(MSG_INFO, SMARTCOM_ALG_MISHA, CAlgMisha::getName() << " - helper thread finished");
}


} // end of namespace
