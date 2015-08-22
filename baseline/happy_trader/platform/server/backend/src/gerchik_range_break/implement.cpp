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
			LOG(MSG_FATAL, ALG_M_RANGE_BREAK, "Fatal exception: " << e.message());
		}
#ifdef HANDLE_NATIVE_EXCEPTIONS
		catch(Testframe::Exception& e) {
			LOG(MSG_FATAL, ALG_M_RANGE_BREAK, "Fatal MSVC exception: " << e.message());
		}
#endif
		catch(...) {
			LOG(MSG_FATAL, ALG_M_RANGE_BREAK, "Unknown fatal exception");
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
	thread_m(NULL)
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

	LOG(MSG_INFO, ALG_M_RANGE_BREAK, MRangeBreak::getName() << " - deinitialized");
	
}

void MRangeBreak::onLibraryInitialized(
	Inqueue::AlgBrokerDescriptor const& descriptor, 
	BrkLib::BrokerBase* brokerlib, 
	CppUtils::String const& runName,
	CppUtils::String const& comment  
)
{
	
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
				LOG_SYMBOL(internalRun_name_m,  providerForRTSymbols_m, SLogger::SL_INFO, *it, "symbol will be processed" );
				TradingState& oldState = state_m[*it];
				LOG_SYMBOL(internalRun_name_m,  providerForRTSymbols_m, SLogger::SL_INFO, *it,  "EVENT_X: " << oldState.toString());
		}
      
	};

	  
	// ---------------
	// broker param
	order_method_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "ORDER_METHOD");
	order_place_m  = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m,  "ORDER_PLACE" );


	// trade param = 
	//SHARES_m = GET_ALG_TRADE_DOUBLE(descriptor.initialAlgBrkParams_m, "SHARES");

	// the maximum nuber of concurrently traded securities
	// -1 means no restriction
	CONCURRENT_TRADE_SECURITIES_m = GET_ALG_TRADE_INT(descriptor.initialAlgBrkParams_m, "CONCURRENT_TRADE_SECURITIES");

	/*
	pathToExport_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "EXPORT_PATH");
	if (pathToExport_m.size() <=0) {
		THROW(CppUtils::OperationFailed, "exc_InvalidExportPath", "ctx_InitializeHooker", "");
	}

	CppUtils::makeDir(pathToExport_m);
	*/


	// bollinger
	BOLLINGER_PERIOD_m = GET_ALG_TRADE_DOUBLE(descriptor.initialAlgBrkParams_m, "BOLLINGER_PERIOD");

	BOLLINGER_STDDEV_MULT_m = GET_ALG_TRADE_DOUBLE(descriptor.initialAlgBrkParams_m, "BOLLINGER_STDDEV_MULT");

	//

	BOLLINGER_ENTER_CRITERIA_m = GET_ALG_TRADE_DOUBLE(descriptor.initialAlgBrkParams_m, "BOLLINGER_ENTER_CRITERIA");
	
	LOG_COMMON( internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, 
			" Gerchik range break library started " << "\n" <<
			" Symbols totally: " << symbolLevel1_rt_m.size() << "\n" <<
			" Order method: " << order_method_m << "\n" <<
			" Order place: " << order_place_m << "\n" <<
			" time work begin: " << hoursBegin_m << ":" << minutesBegin_m << "\n" <<
			" time work end: " << hourEnd_m << ":" << minutesEnd_m << "\n" <<
			" bollinger period: " << BOLLINGER_PERIOD_m << "\n" <<
			" bollinger stddev mult: " << BOLLINGER_STDDEV_MULT_m << "\n" <<
			" bollinger enter criteria: " << BOLLINGER_ENTER_CRITERIA_m
		);

	ALERTEVENTNC(internalRun_name_m,BrkLib::AP_MEDIUM, ALG_M_RANGE_BREAK, "Initialized" );


}

void MRangeBreak::onLibraryFreed()
{
	LOG_COMMON( internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO,"Gerchik range break library freed");

	
}

void MRangeBreak::onBrokerResponseArrived( 
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
)
{
	
	// process all responses

	if (bresponse.type_m == BrkLib::RT_BrokerResponseDelayedOrder) {
		BrkLib::BrokerResponseDelayedOrder const& brespDO = (BrkLib::BrokerResponseDelayedOrder const&)bresponse;

		// resolve context - here is only read only
		BrokerContext context = getBrokerContext(brespDO.order_m.symbolNum_m);
		TradingEventList elist;

		if (brespDO.doType_m == BrkLib::DORT_REGISTER_ORDER) {
			ACTION_BEGIN(context.action_m.action_m)
				ACTION_ENTRY_BEGIN(AS_IssueEnterBothOrder) 
							// local
						TradingEvent te(TE_BrokerConfirm_EnterBoth,  brespDO.order_m.symbolNum_m, CppUtils::getTime(), true);
						elist.push_back(te);

				ACTION_ENTRY_END()
				ACTION_ENTRY_BEGIN(AS_IssueSLTPOrder) 
					// local
						TradingEvent te(TE_BrokerConfirmTPSLPending,  brespDO.order_m.symbolNum_m, CppUtils::getTime(), true);
						elist.push_back(te);
			ACTION_ENTRY_END()
			ACTION_END()

		}

		// delegate event
			// log
			for(int i =0; i < elist.size(); i++) {
				LOG_SYMBOL_CTX_BRK( internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, brespDO.order_m.symbolNum_m, 
							"New event (will be delegated to the processing thread): " << elist[i].toString(),
							"Broker response: " << brespDO.toString()
				);
			}

			pushProcessingEvent(brkConnect, elist);


	}
	else if (bresponse.type_m == BrkLib::RT_BrokerResponseEquity) {
		LOG_EQUITY(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, (BrkLib::BrokerResponseEquity const&)bresponse);
	}
	else if (bresponse.type_m == BrkLib::RT_BrokerResponseOrder) {
			BrkLib::BrokerResponseOrder const& bRespOrder = (BrkLib::BrokerResponseOrder const&)bresponse;

			// resolve context - here is only read only
			BrokerContext context = getBrokerContext(bRespOrder.symbolNum_m);
			TradingEventList elist;

			CppUtils::EnumerationHelper isLong = DIRECTION_DUMMY;
			if (bRespOrder.positionType_m == BrkLib::POS_LIMIT_BUY || bRespOrder.positionType_m == BrkLib::POS_STOP_BUY)
				isLong = DIRECTION_LONG;
			else if (bRespOrder.positionType_m == BrkLib::POS_LIMIT_SELL || bRespOrder.positionType_m == BrkLib::POS_STOP_SELL)
				isLong = DIRECTION_SHORT;

			// this must match order ID
			
			
			if (bRespOrder.resultCode_m==BrkLib::PT_ORDER_ESTABLISHED ) {
				if (bRespOrder.resultDetailCode_m & BrkLib::ODR_ORDER_INSTALLED) {
					
									
					// pending confirmation
					// select type of request
					ACTION_BEGIN(context.action_m.action_m)
						ACTION_ENTRY_BEGIN(AS_IssueEnterBothOrder) 
							
							OrderDescriptor& descr = context.resolveWorkingOrderViaId(bRespOrder.parentID_m);
							descr.orderInstallPrice_m = bRespOrder.opOpenPrice_m;
							descr.orderInstallTime_m = CppUtils::getTime();

							
							if (context.orderWorkShort_m.isInstalled() && context.orderWorkLong_m.isInstalled()) {
								// event
								TradingEvent te(TE_BrokerConfirm_EnterBoth,  bRespOrder.symbolNum_m, isLong, bRespOrder.opOpenTime_m, true);
								elist.push_back(te);
							}

							
						ACTION_ENTRY_END()
						

						ACTION_ENTRY_BEGIN(AS_IssueSLTPOrder)

							OrderDescriptor& descr = context.resolveSLTPOrderViaId(bRespOrder.parentID_m);
							descr.orderInstallPrice_m = bRespOrder.opOpenPrice_m;
							descr.orderInstallTime_m = CppUtils::getTime();

							
							if (context.orderTP_m.isInstalled() && context.orderSL_m.isInstalled()) {

								TradingEvent te(TE_BrokerConfirmTPSLPending,  bRespOrder.symbolNum_m, isLong, bRespOrder.opOpenTime_m, true);
								elist.push_back(te);
							}


						ACTION_ENTRY_END()
					ACTION_END()
					
					
				

				}
				else if (bRespOrder.resultDetailCode_m & BrkLib::ODR_ORDER_EXECUTED) {
					
					// we have rights return only final trades
					if (bRespOrder.RESPONSE_TAG_REMAIN_SHARES == 0 ) {
						

						//
						ACTION_BEGIN(context.action_m.action_m)
							ACTION_ENTRY_BEGIN(AS_IssueEnterBothOrder) 
								
								OrderDescriptor& descr = context.resolveWorkingOrderViaId(bRespOrder.parentID_m);
								descr.orderExecutePrice_m = bRespOrder.opOpenPrice_m;
								descr.orderExecuteTime_m = CppUtils::getTime();

								

								if (context.orderWorkShort_m.isExecuted() ) {
									context.directionOfTrade_m = DIRECTION_SHORT;
									TradingEvent te(TE_BrokerConfirmEnterOpened,  bRespOrder.symbolNum_m, isLong, bRespOrder.opOpenTime_m, true);
									elist.push_back(te);
								}
									
								if (context.orderWorkLong_m.isExecuted()) {
									// event
									context.directionOfTrade_m = DIRECTION_LONG;
									TradingEvent te(TE_BrokerConfirmEnterOpened,  bRespOrder.symbolNum_m, isLong, bRespOrder.opOpenTime_m, true);
									elist.push_back(te);
								}
							ACTION_ENTRY_END()
							
							ACTION_ENTRY_BEGIN(AS_IssueSLTPOrder) 
								OrderDescriptor& descr = context.resolveSLTPOrderViaId(bRespOrder.parentID_m);
								descr.orderExecutePrice_m = bRespOrder.opOpenPrice_m;
								descr.orderExecuteTime_m = CppUtils::getTime();

								
								if (context.orderTP_m.isExecuted()) {
									TradingEvent te(TE_BrokerConfirmTPOpened,  bRespOrder.symbolNum_m, isLong, bRespOrder.opOpenTime_m, true);
									elist.push_back(te);
								}

								if (context.orderSL_m.isExecuted()) {
									TradingEvent te(TE_BrokerConfirmSLOpened,  bRespOrder.symbolNum_m, isLong, bRespOrder.opOpenTime_m, true);
									elist.push_back(te);
								}



							ACTION_ENTRY_END()

							
						ACTION_END()

					} 
					
				}
			}
			else if (bRespOrder.resultCode_m==BrkLib::PT_ORDER_CANCELLED) {
				// order cancelled

				
				ACTION_BEGIN(context.action_m.action_m)

					ACTION_ENTRY_BEGIN(AS_IssueEnterBothOrder) 
						TradingEvent te(TE_BrokerReject_EnterBoth,  bRespOrder.symbolNum_m, isLong, bRespOrder.opOpenTime_m, true);
						elist.push_back(te);
						
					ACTION_ENTRY_END()
					
					ACTION_ENTRY_BEGIN(AS_IssueCancelEnterOtherSideOrder) 
						TradingEvent te(TE_BrokerConfirmOtherSideCancelled,  bRespOrder.symbolNum_m, isLong, bRespOrder.opOpenTime_m, true);
						elist.push_back(te);
					ACTION_ENTRY_END()

					ACTION_ENTRY_BEGIN(AS_IssueSLTPOrder) 
						TradingEvent te(TE_BrokerRejectTPSLPending,  bRespOrder.symbolNum_m, isLong, bRespOrder.opOpenTime_m, true);
						elist.push_back(te);
					ACTION_ENTRY_END()

					// cancellations
					ACTION_ENTRY_BEGIN(AS_IssueCancelSLOrder) 
						context.orderSL_m.orderCancelTime_m = CppUtils::getTime();
						

						if (context.orderSL_m.isCancelled()) {
							TradingEvent te(TE_BrokerConfirmSLCancelled,  bRespOrder.symbolNum_m, isLong, bRespOrder.opOpenTime_m, true);
							elist.push_back(te);
						}
					ACTION_ENTRY_END()

					ACTION_ENTRY_BEGIN(AS_IssueCancelBothOrder) 
						// must arrive 2 times
						if (isLong == DIRECTION_SHORT) {
						
							context.orderWorkShort_m.orderCancelTime_m = CppUtils::getTime();
						
						}
						else if (isLong == DIRECTION_LONG) {
							context.orderWorkLong_m.orderCancelTime_m = CppUtils::getTime();
							
						}

						if (context.orderWorkLong_m.isCancelled() && context.orderWorkShort_m.isCancelled()) {
							TradingEvent te(TE_BrokerConfirmBothCancelled,  bRespOrder.symbolNum_m, isLong, bRespOrder.opOpenTime_m, true);
							elist.push_back(te);
						}


					ACTION_ENTRY_END()

					ACTION_ENTRY_BEGIN(AS_IssueCancelTPOrder) 
						context.orderTP_m.orderCancelTime_m = CppUtils::getTime();
						

						if (context.orderSL_m.isCancelled()) {
							TradingEvent te(TE_BrokerConfirmTPCancelled,  bRespOrder.symbolNum_m, isLong, bRespOrder.opOpenTime_m, true);
							elist.push_back(te);
						}
					ACTION_ENTRY_END()


				ACTION_END()


				

			}
			else if (bRespOrder.resultCode_m==BrkLib::PT_GENERAL_ERROR_HAPPENED) {
				// order errored
				// return errors everywhere

				

				ACTION_BEGIN(context.action_m.action_m)

					ACTION_ENTRY_BEGIN(AS_IssueEnterBothOrder) 
						OrderDescriptor& descr = context.resolveWorkingOrderViaId(bRespOrder.parentID_m);
						descr.orderErrorTime_m = CppUtils::getTime();

						TradingEvent te(TE_BrokerReject_EnterBoth,  bRespOrder.symbolNum_m, isLong, bRespOrder.opOpenTime_m, true);
						elist.push_back(te);
					ACTION_ENTRY_END()
					
					ACTION_ENTRY_BEGIN(AS_IssueCancelEnterOtherSideOrder) 
						TradingEvent te(TE_BrokerRejectOtherSideCancelled,  bRespOrder.symbolNum_m, isLong, bRespOrder.opOpenTime_m, true);
						elist.push_back(te);
					ACTION_ENTRY_END()

					ACTION_ENTRY_BEGIN(AS_IssueSLTPOrder) 
						OrderDescriptor& descr = context.resolveSLTPOrderViaId(bRespOrder.parentID_m);
						descr.orderErrorTime_m = CppUtils::getTime();

						TradingEvent te(TE_BrokerRejectTPSLPending,  bRespOrder.symbolNum_m, isLong, bRespOrder.opOpenTime_m, true);
						elist.push_back(te);
					ACTION_ENTRY_END()

					// cancellations
					ACTION_ENTRY_BEGIN(AS_IssueCancelSLOrder) 
						TradingEvent te(TE_BrokerRejectSLCancelled,  bRespOrder.symbolNum_m, isLong, bRespOrder.opOpenTime_m, true);
						elist.push_back(te);
					ACTION_ENTRY_END()

					ACTION_ENTRY_BEGIN(AS_IssueCancelBothOrder)
						TradingEvent te(TE_BrokerRejectBothCancelled,  bRespOrder.symbolNum_m, isLong, bRespOrder.opOpenTime_m, true);
						elist.push_back(te);
					ACTION_ENTRY_END()

					ACTION_ENTRY_BEGIN(AS_IssueCancelTPOrder) 
						TradingEvent te(TE_BrokerRejectTPCancelled,  bRespOrder.symbolNum_m, isLong, bRespOrder.opOpenTime_m, true);
						elist.push_back(te);
					ACTION_ENTRY_END()


				ACTION_END()

			}

			// delegate event
			// log
			for(int i =0; i < elist.size(); i++) {
				LOG_SYMBOL_CTX_BRK( internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, bRespOrder.symbolNum_m, 
							"New event (will be delegated to the processing thread): " << elist[i].toString(),
							"Broker response: " << bRespOrder.toString()
				);
			}

			// update context
			updateBrokerContext(bRespOrder.symbolNum_m, context);
			pushProcessingEvent(brkConnect, elist);

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
	Inqueue::TradingParameters const& descriptor,
	BrkLib::BrokerBase* brokerlib,
	CppUtils::String const& runName,
	CppUtils::String const& comment
)
{
	
	

	BrkLib::BrokerConnect brkConnect(brokerlib, "gerchik_range_break", false, false, internalRun_name_m.c_str(), comment.c_str());
	brkConnect.connect();
		
	if (!brkConnect.getSession().isConnected()) {
		LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  ALG_M_RANGE_BREAK, "onDataArrived(...) session cannot be established: " << brkConnect.getSession().getConnection());
		return;
	}

	/*
	{
		LOCK_FOR_SCOPE(priceRTDataMtx_m);

		// session
		map<CppUtils::String, HlpStruct::RtData >::iterator it = rtData_m.find(rtdata.getSymbol());

		if(it == rtData_m.end()) {
			rtData_m[rtdata.getSymbol()] = rtdata;
			return;
		}
	}
	*/

		
	//if (rtdata.getProvider2()==providerForRTSymbols_m) {
		 //LOG_PRINT(internalRun_name_m, providerForRTSymbols_m, rtdata.getSymbol2(), rtdata.getRtData().bid_m, rtdata.getRtData().ask_m, rtdata.getRtData().volume_m, rtdata.getRtData().color_m );
	//}


	// before we check if we can go on with our delayed orders
	// but need to react on exceptions
	BrokerContext context = getBrokerContext(rtdata.getSymbol());

	// in case of exception 
	try {
		brokerlib->onDelayedOrderExecutionCheck(brkConnect.getSession(), rtdata.getRtData().bid_m, rtdata.getProvider2(), rtdata.getSymbol2(), "" );
	}
	catch(BrkLib::LaserFailed &e) {

		TradingEventList elist_err;

	
		// generate error event
		ACTION_BEGIN(context.action_m.action_m)
			ACTION_ENTRY_BEGIN(AS_IssueEnterBothOrder) 
				TradingEvent te(TE_BrokerReject_EnterBoth,  context.action_m.symbol_m, -1, true);
				elist_err.push_back(te);
				
			ACTION_ENTRY_END()

			ACTION_ENTRY_BEGIN(AS_IssueCancelEnterOtherSideOrder) 
				TradingEvent te(TE_BrokerRejectOtherSideCancelled,  context.action_m.symbol_m, -1, true);
				elist_err.push_back(te);
				
			ACTION_ENTRY_END()

			ACTION_ENTRY_BEGIN(AS_IssueSLTPOrder) 
				TradingEvent te(TE_BrokerRejectTPSLPending,  context.action_m.symbol_m, -1, true);
				elist_err.push_back(te);
				
			ACTION_ENTRY_END()

			ACTION_ENTRY_BEGIN(AS_IssueCancelSLOrder) 
				TradingEvent te(TE_BrokerRejectSLCancelled,  context.action_m.symbol_m, -1, true);
				elist_err.push_back(te);
				
			ACTION_ENTRY_END()

			ACTION_ENTRY_BEGIN(AS_IssueCancelBothOrder) 
				TradingEvent te(TE_BrokerRejectBothCancelled,  context.action_m.symbol_m, -1, true);
				elist_err.push_back(te);
				
			ACTION_ENTRY_END()

			ACTION_ENTRY_BEGIN(AS_IssueCancelTPOrder) 
				TradingEvent te(TE_BrokerRejectTPCancelled,  context.action_m.symbol_m, -1, true);
				elist_err.push_back(te);
			
			ACTION_ENTRY_END()

			// 
		ACTION_END()

	

		LOG_SYMBOL_CTX(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_ERROR, context.action_m.symbol_m, "Error on action: " << context.action_m.toString(),
				context.toString()
			);

		for(int i =0; i < elist_err.size(); i++) {
				LOG_SYMBOL_CTX(internalRun_name_m,  providerForRTSymbols_m, SLogger::SL_ERROR, context.action_m.symbol_m, 
							"Error event (will be delegated to the processing thread): " << elist_err[i].toString(),
							"Error: " << e.message() << " - " << e.arg() << " - " << e.context()
				);
		}

		// propagate
		// delegate error event
		machineEventsReaction(brkConnect, elist_err);
	}



	// event
	TradingEventList elist;
		
	//generateNewEvent(brkConnect, historyaccessor,descriptor, elist, rtdata);
	machineEventsReaction(brkConnect, elist);
		
	/*
	{
		LOCK_FOR_SCOPE(priceRTDataMtx_m);

		// update
		rtData_m[ rtdata.getSymbol() ] = rtdata;
	
	}
	*/
	
}

void MRangeBreak::onLevel2DataArrived(
		HlpStruct::RtLevel2Data const& level2data,
		Inqueue::ContextAccessor& historyaccessor, 
		Inqueue::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
	)
{

	

}

void MRangeBreak::onLevel1DataArrived(
		HlpStruct::RtLevel1Data const& level1data,
		Inqueue::ContextAccessor& historyaccessor, 
		Inqueue::TradingParameters const& descriptor,
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

	BrkLib::BrokerConnect brkConnect(getAlgHolder().getBroker(), "gerchik_range_break", false, false, internalRun_name_m.c_str(), "");
	brkConnect.connect();
		
	if (!brkConnect.getSession().isConnected()) {
		LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  ALG_M_RANGE_BREAK, "onEventArrived(...) session cannot be established: " << brkConnect.getSession().getConnection());
		return;
	}

	Signaller signaller;
	TradingEventList elist;

	CppUtils::String const& data = callContext.getParameter<BrkLib::String_Proxy>("custom_data").get();

	if (callContext.getCallCustomName()=="Force_TE_Manually_Fixed") {
		
		signaller.fromString( data);
		
		TradingEvent te(TE_Manually_Fixed, signaller.symbol_m, CppUtils::getTime(), true );
		elist.push_back(te);

		customOutputData = "command accepted";

	}
	else if (callContext.getCallCustomName()=="Force_TE_BollingerTradeSignal") {
		
		// this enforce trading
		
		signaller.fromString( data);
		
		TradingEvent te(TE_BollingerTradeSignal, signaller.symbol_m, CppUtils::getTime(), true );
		elist.push_back(te);

		customOutputData = "command accepted";
		  
	}
	else if (callContext.getCallCustomName()=="Force_Ping") {
		
		// symbolLevel1_rt_m - holds all symbols
		HlpStruct::XmlParameter xmlparameter;
		for(CppUtils::StringSet::const_iterator it = symbolLevel1_rt_m.begin(); it != symbolLevel1_rt_m.end(); it++) {
			xmlparameter.getParameter("symbols").getAsStringList(true).push_back(*it);
		}
		
		CppUtils::String content;
		HlpStruct::serializeXmlParameter(xmlparameter, content );


		// get symbol list
		customOutputData = CppUtils::wrapToCDataTags(content);
	}
	else 
		return;
	
	
	LOCK_FOR_SCOPE(signalStructureMtx_m) {
		signalStructure_m[signaller.symbol_m] = signaller;
	}

	machineEventsReaction(brkConnect, elist);

	result = true;
}

void MRangeBreak::onThreadStarted(Inqueue::ContextAccessor& contextaccessor, bool const firstLib, bool const lastLib) {
	LOGEVENT(HlpStruct::CommonLog::LL_INFO,  ALG_M_RANGE_BREAK, "RT Thread started");
	

}


CppUtils::String const& MRangeBreak::getName() const
{
	static const CppUtils::String build_date(__TIMESTAMP__);
	static const CppUtils::String name( "[ gerchik range break ]: " + build_date );
	return name;
}

/**
* Helpers
*/



/*
void MRangeBreak::generateNewEvent(
		BrkLib::BrokerConnect& brconnect, 
		Inqueue::ContextAccessor& historyaccessor,
		Inqueue::TradingParameters const& descriptor,
		TradingEventList& elist, 
		HlpStruct::RtData const& rtdata
	)
{
	


}
*/


void MRangeBreak::machineEventsReaction(BrkLib::BrokerConnect& brconnect, TradingEventList const& elist)
{
	LOCK_FOR_SCOPE(jobMutex_m);
	
	for(int i = 0; i < elist.size(); i++) {
		TradingEvent const& event_i = elist[i];

		// target symbols
		for(int k = 0; k < event_i.targetSymbols_m.size(); k++) {
			CppUtils::String const& target_i = event_i.targetSymbols_m[k];

			// create new state
			TradingState& oldState = state_m[target_i];
			if (oldState.symbol_m.size() <=0)
				oldState.symbol_m = target_i;
			
			TradingState newState = oldState;
	
			// store symbol
			newState.symbol_m = target_i;
		
			// large map of our event processing
			OLD_STATE_BEGIN(oldState.state_m)
				OLD_STATE_ENTRY_BEGIN(TS_Initial)
					NEW_EVENT_BEGIN(event_i.event_m)
						NEW_EVENT_ENTRY_BEGIN(TE_BollingerTradeSignal) 

							// here how we are adjusting that we are going to trade only with one security per time

							if (CONCURRENT_TRADE_SECURITIES_m >=0 && getSecuritiesNumberInProgress() < CONCURRENT_TRADE_SECURITIES_m) {
								newState.state_m = TS_Ready_Enter_Both;
							}
							else if (CONCURRENT_TRADE_SECURITIES_m < 0) {
								newState.state_m = TS_Ready_Enter_Both;
							}
						NEW_EVENT_ENTRY_END()
					NEW_EVENT_END()
				OLD_STATE_ENTRY_END()

				//
				OLD_STATE_ENTRY_BEGIN(TS_Ready_Enter_Both)
					NEW_EVENT_BEGIN(event_i.event_m)
						NEW_EVENT_ENTRY_BEGIN(TE_BrokerConfirm_EnterBoth) 
							newState.state_m = TS_Pending_Both;
						NEW_EVENT_ENTRY_END()
						NEW_EVENT_ENTRY_BEGIN(TE_BrokerReject_EnterBoth) 
							newState.state_m = TS_Inconsistent;
						NEW_EVENT_ENTRY_END()

					NEW_EVENT_END()

				OLD_STATE_ENTRY_END()
				////
				OLD_STATE_ENTRY_BEGIN(TS_Pending_Both)
					NEW_EVENT_BEGIN(event_i.event_m)
						NEW_EVENT_ENTRY_BEGIN(TE_BrokerConfirmEnterOpened) 
							newState.state_m = TS_Ready_Cancel_OtherSide;
						NEW_EVENT_ENTRY_END()  

						NEW_EVENT_ENTRY_BEGIN(TE_BollingerNotTradeSignal) 
							newState.state_m = TS_Ready_Cancel_Both;
						NEW_EVENT_ENTRY_END()  
						
						NEW_EVENT_ENTRY_BEGIN(TE_BrokerRejectEnterOpened) 
							newState.state_m = TS_Inconsistent;
						NEW_EVENT_ENTRY_END() 

						NEW_EVENT_ENTRY_BEGIN(TE_BrokerReject_EnterBoth) 
							newState.state_m = TS_Inconsistent;
						NEW_EVENT_ENTRY_END()

					NEW_EVENT_END()
				OLD_STATE_ENTRY_END()
				
				////
				OLD_STATE_ENTRY_BEGIN(TS_Ready_Cancel_OtherSide)
					NEW_EVENT_BEGIN(event_i.event_m)
						NEW_EVENT_ENTRY_BEGIN(TE_BrokerConfirmOtherSideCancelled) 
							newState.state_m = TS_Ready_Enter_TP_SL;
						NEW_EVENT_ENTRY_END()  
						
						NEW_EVENT_ENTRY_BEGIN(TE_BrokerRejectOtherSideCancelled) 
							newState.state_m = TS_Inconsistent;
						NEW_EVENT_ENTRY_END()

					NEW_EVENT_END()
				OLD_STATE_ENTRY_END()
				
				////
				OLD_STATE_ENTRY_BEGIN(TS_Ready_Enter_TP_SL)
					NEW_EVENT_BEGIN(event_i.event_m)
						NEW_EVENT_ENTRY_BEGIN(TE_BrokerConfirmTPSLPending) 
							newState.state_m = TS_Pending_TP_SL;
						NEW_EVENT_ENTRY_END() 
						NEW_EVENT_ENTRY_BEGIN(TE_BrokerRejectTPSLPending) 
							newState.state_m = TS_Inconsistent;
						NEW_EVENT_ENTRY_END()  
					NEW_EVENT_END()
				OLD_STATE_ENTRY_END()

				//
				OLD_STATE_ENTRY_BEGIN(TS_Pending_TP_SL)
					NEW_EVENT_BEGIN(event_i.event_m)
						NEW_EVENT_ENTRY_BEGIN(TE_BrokerConfirmTPOpened) 
							newState.state_m = TS_Ready_Cancel_SL;
						NEW_EVENT_ENTRY_END()  
						NEW_EVENT_ENTRY_BEGIN(TE_BrokerConfirmSLOpened) 
							newState.state_m = TS_Ready_Cancel_TP;
						NEW_EVENT_ENTRY_END()  
						NEW_EVENT_ENTRY_BEGIN(TE_BrokerRejectTPOpened) 
							newState.state_m = TS_Inconsistent;
						NEW_EVENT_ENTRY_END()  
						NEW_EVENT_ENTRY_BEGIN(TE_BrokerRejectSLOpened) 
							newState.state_m = TS_Inconsistent;
						NEW_EVENT_ENTRY_END() 
					NEW_EVENT_END()
				OLD_STATE_ENTRY_END()
				
				//
				OLD_STATE_ENTRY_BEGIN(TS_Ready_Cancel_SL)
					NEW_EVENT_BEGIN(event_i.event_m)
						NEW_EVENT_ENTRY_BEGIN(TE_BrokerConfirmSLCancelled) 
							newState.state_m = TS_Initial;
						NEW_EVENT_ENTRY_END() 
						NEW_EVENT_ENTRY_BEGIN(TE_BrokerRejectSLCancelled) 
							newState.state_m = TS_Inconsistent;
						NEW_EVENT_ENTRY_END() 
					NEW_EVENT_END()
				OLD_STATE_ENTRY_END()

				//
				OLD_STATE_ENTRY_BEGIN(TS_Ready_Cancel_TP)
					NEW_EVENT_BEGIN(event_i.event_m)
						NEW_EVENT_ENTRY_BEGIN(TE_BrokerConfirmTPCancelled) 
							newState.state_m = TS_Initial;
						NEW_EVENT_ENTRY_END() 
						NEW_EVENT_ENTRY_BEGIN(TE_BrokerRejectTPCancelled) 
							newState.state_m = TS_Inconsistent;
						NEW_EVENT_ENTRY_END() 
					NEW_EVENT_END()
				OLD_STATE_ENTRY_END()

				//
				OLD_STATE_ENTRY_BEGIN(TS_Ready_Cancel_Both)
					NEW_EVENT_BEGIN(event_i.event_m)
						NEW_EVENT_ENTRY_BEGIN(TE_BrokerConfirmBothCancelled) 
							newState.state_m = TS_Initial;
						NEW_EVENT_ENTRY_END()  
						NEW_EVENT_ENTRY_BEGIN(TE_BrokerRejectBothCancelled) 
							newState.state_m = TS_Inconsistent;
						NEW_EVENT_ENTRY_END()
					NEW_EVENT_END()
				OLD_STATE_ENTRY_END()


				OLD_STATE_ENTRY_BEGIN(TS_Inconsistent)
					NEW_EVENT_BEGIN(event_i.event_m)
						NEW_EVENT_ENTRY_BEGIN(TE_Manually_Fixed) 
							newState.state_m = TS_Initial;
						NEW_EVENT_ENTRY_END()  
					NEW_EVENT_END()

				OLD_STATE_ENTRY_END()
			
			OLD_STATE_END()

			// 
			// if state changed
			if (newState.state_m != oldState.state_m) {
				
				// logging that state has changed
				LOG_SYMBOL(internalRun_name_m,  providerForRTSymbols_m, SLogger::SL_INFO, oldState.symbol_m, 
					"State changed - Old state: \n" + oldState.toString() + " New State: \n" + newState.toString() + "\n" +
					"Caused last event: \n" + event_i.toString()
				);

				LOG_SYMBOL(internalRun_name_m,  providerForRTSymbols_m, SLogger::SL_INFO, oldState.symbol_m,  "EVENT_X: "<< newState.toString());

				
				onStateChanged(brconnect, oldState, newState, event_i);
				// state changed
			}
		
			// store  
			oldState = newState;


		} // end target symbols
	} // end events

		
}

/*
HERE WE IMPLEMENT WHAT ACTION WE WILL HAVE WHEN THE STATE IS CHANGED
*/
// this is called from machineEventsReaction
// thus all calls here are secured by jobMutex_m
void MRangeBreak::onStateChanged(BrkLib::BrokerConnect& brconnect, TradingState const& oldState, TradingState& newState, TradingEvent const& cause_event)
{
	
	Signaller signal_i;

	LOCK_FOR_SCOPE(signalStructureMtx_m) {

		map<CppUtils::String, Signaller>::const_iterator it = signalStructure_m.find(newState.symbol_m);
		if (it == signalStructure_m.end()) {
			LOG_COMMON(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_DEBUG, "For symbol: " << newState.symbol_m << " cannot find Signaller structure, returning...");
			return;
		}

		signal_i = it->second;
	}


	//If we are returning to initial state
	OLD_STATE_ENTRY_ALL()

		NEW_STATE_BEGIN(newState.state_m)
			NEW_STATE_ENTRY_BEGIN(TS_Initial)

				// before clearing context some logging
				// context here contains only 
				//BrokerContext context = getBrokerContext(newState.symbol_m);

				/*
				double profit_tp, profit_sl;
				bool closed_tp = false;
				bool closed_sl = false;

				// if stop loss
				if (context.broker_EnterOrder_OpenPrice_m > 0 && context.broker_StopOrder_OpenPrice_m > 0  ) {
					if (context.isLong_m == DIRECTION_LONG) {
						profit_sl = context.broker_EnterOrder_OpenPrice_m - context.broker_StopOrder_OpenPrice_m;
						closed_sl = true;
					}
					else if (context.isLong_m == DIRECTION_SHORT){
						profit_sl = context.broker_StopOrder_OpenPrice_m - context.broker_EnterOrder_OpenPrice_m;
						closed_sl = true;
					}
				}

				// if take profit
				if (context.broker_EnterOrder_OpenPrice_m > 0 && context.broker_TPOrder_OpenPrice_m > 0  ) {
					if (context.isLong_m == DIRECTION_LONG) {
						profit_tp = context.broker_EnterOrder_OpenPrice_m - context.broker_TPOrder_OpenPrice_m;
						closed_tp = true;
					}
					else if (context.isLong_m == DIRECTION_SHORT){
						profit_tp = context.broker_TPOrder_OpenPrice_m - context.broker_EnterOrder_OpenPrice_m;
						closed_tp = true;
					}
				}

				
				CppUtils::String time_install_s = context.broker_EnterOrder_InstallTime_m > 0 ? CppUtils::getGmtTime(context.broker_EnterOrder_InstallTime_m) : "N/A";
				CppUtils::String time_execute_s = context.broker_EnterOrder_InstallTime_m > 0 ? CppUtils::getGmtTime(context.broker_EnterOrder_ExecuteTime_m) : "N/A";
				CppUtils::String time_close_tp_s = context.broker_EnterOrder_InstallTime_m > 0 ? CppUtils::getGmtTime(context.broker_TPOrder_ExecuteTime_m) : "N/A";
				CppUtils::String time_close_sl_s = context.broker_EnterOrder_InstallTime_m > 0 ? CppUtils::getGmtTime(context.broker_StopOrder_ExecuteTime_m) : "N/A";


				if (closed_tp) {
					LOG_COMMON(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, "(!) TRADE RESULT TP EXECUTED: " << newState.symbol_m << 
						( resolveDirectionConstant(context.isLong_m) ) <<
						" PROFIT=" << CppUtils::float2String(profit_tp, -1, 4) << 
						" open price=" << CppUtils::float2String(context.broker_EnterOrder_OpenPrice_m, -1, 4) << 
						" close TP price=" << CppUtils::float2String(context.broker_TPOrder_OpenPrice_m, -1, 4) << 
						" install TP price=" << CppUtils::float2String(context.broker_EnterOrder_InstallPrice_m, -1, 4) <<
						" install order time=" << time_install_s <<
						" open order time=" << time_execute_s <<
						" close TP time=" << time_close_tp_s

					);
				} else if (closed_sl) {
					LOG_COMMON(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, "(:-() TRADE RESULT SL EXECUTED: " << newState.symbol_m << 
						( resolveDirectionConstant(context.isLong_m) ) <<
						" PROFIT=" << CppUtils::float2String(profit_tp, -1, 4) << 
						" open price=" << CppUtils::float2String(context.broker_EnterOrder_OpenPrice_m, -1, 4) << 
						" close SL price=" << CppUtils::float2String(context.broker_StopOrder_OpenPrice_m, -1, 4) << 
						" install SL price=" << CppUtils::float2String(context.broker_StopOrder_InstallPrice_m, -1, 4) <<
						" install order time=" << time_install_s <<
						" open order time=" << time_execute_s <<
						" close TP time=" << time_close_sl_s
					);
				}
				else {
					LOG_COMMON(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, "NO DEAL FOR: " << newState.symbol_m  );
				}
				*/
				
				// clear context
				Action action(AS_ClearBrokerContext, newState.symbol_m);
				doAction(brconnect, action);

			NEW_STATE_ENTRY_END();
		NEW_STATE_END();
	OLD_STATE_ENTRY_ALL_END()
	//

	// other states
	OLD_STATE_BEGIN(oldState.state_m)
		
		OLD_STATE_ENTRY_BEGIN(TS_Initial)
			
			NEW_STATE_BEGIN(newState.state_m)
				NEW_STATE_ENTRY_BEGIN(TS_Ready_Enter_Both)
				
					Action action(AS_IssueEnterBothOrder, newState.symbol_m);
					action.opBothSideLongInstallPrice_m  = signal_i.opBothSideLongInstallPrice_m;
					action.opBothSideShortInstallPrice_m = signal_i.opBothSideShortInstallPrice_m;
					action.volume_m = signal_i.volume_m;
				

					LOG_COMMON(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, 
						"ISSUE WORK ORDERS (LONG & SHORT) ( " << action.symbol_m << " ) " <<
						" Long price: " << action.opBothSideLongInstallPrice_m <<
						" Short price: " << action.opBothSideShortInstallPrice_m
					);

					doAction(brconnect, action);
					
				NEW_STATE_ENTRY_END()
			NEW_STATE_END();
		
		OLD_STATE_ENTRY_END()
		////

		OLD_STATE_ENTRY_BEGIN(TS_Pending_Both)
			
			NEW_STATE_BEGIN(newState.state_m)
				NEW_STATE_ENTRY_BEGIN(TS_Ready_Cancel_OtherSide)
				
					BrokerContext context = getBrokerContext(newState.symbol_m);
					Action action(AS_IssueCancelEnterOtherSideOrder, newState.symbol_m);
					
					
					LOG_COMMON(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, 
						"ISSUE CANCEL OTHER SIDE CAUSE OUR DIRECTION ( " << action.symbol_m << " ) " << context.directionOfTrade_m.getName() 
					);

					doAction(brconnect, action);
					
				NEW_STATE_ENTRY_END()
			NEW_STATE_END();

			NEW_STATE_BEGIN(newState.state_m)
				NEW_STATE_ENTRY_BEGIN(TS_Ready_Cancel_Both)
				
					Action action(AS_IssueCancelBothOrder, newState.symbol_m);
					
					LOG_COMMON(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, 
						"ISSUE CANCEL BOTH ( " << action.symbol_m << " ) "
					);

					doAction(brconnect, action);
					
				NEW_STATE_ENTRY_END()
			NEW_STATE_END();
		
		OLD_STATE_ENTRY_END()

		////

		OLD_STATE_ENTRY_BEGIN(TS_Ready_Cancel_OtherSide)
			
			NEW_STATE_BEGIN(newState.state_m)
				NEW_STATE_ENTRY_BEGIN(TS_Ready_Enter_TP_SL)
					
					// at this stage we may have the direction of our trade
					BrokerContext context = getBrokerContext(newState.symbol_m);

					Action action(AS_IssueSLTPOrder, newState.symbol_m);

					bool go_on  = true;
					if (context.directionOfTrade_m == DIRECTION_SHORT) {
						action.opSLInstallPrice_m = signal_i.opSLInstallPrice_ShortTrade_m;
						action.opTPInstallPrice_m = signal_i.opTPInstallPrice_ShortTrade_m;
					}
					else if (context.directionOfTrade_m == DIRECTION_LONG) {
						action.opSLInstallPrice_m = signal_i.opSLInstallPrice_LongTrade_m;
						action.opTPInstallPrice_m = signal_i.opTPInstallPrice_LongTrade_m;
					}
					else {
						LOG_COMMON(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_ERROR, 
							"CANNOT ISSUE TP & SL ORDERS ( " << action.symbol_m << " ) BECAUSE INVALID TRADE DIRECTION" 
						);

						go_on = false;
					}
					
					if (go_on) {
						LOG_COMMON(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, 
							"ISSUE TP & SL ORDERS ( " << action.symbol_m << " ) " 
						);

						doAction(brconnect, action);
					}
					
				NEW_STATE_ENTRY_END()
			NEW_STATE_END();
		
		OLD_STATE_ENTRY_END()
		//
		
		//
		OLD_STATE_ENTRY_BEGIN(TS_Pending_TP_SL)
			
			NEW_STATE_BEGIN(newState.state_m)
				NEW_STATE_ENTRY_BEGIN(TS_Ready_Cancel_SL)
				
					Action action(AS_IssueCancelSLOrder, newState.symbol_m);
				

					LOG_COMMON(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, 
						"CANCEL SL ORDER ( " << action.symbol_m << " ) " 
					);

					doAction(brconnect, action);
					
				NEW_STATE_ENTRY_END()
			NEW_STATE_END();

			NEW_STATE_BEGIN(newState.state_m)
				NEW_STATE_ENTRY_BEGIN(TS_Ready_Cancel_TP)
				
					Action action(AS_IssueCancelTPOrder, newState.symbol_m);
			

					LOG_COMMON(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, 
						"CANCEL TP ORDER ( " << action.symbol_m << " ) " 
					);

					doAction(brconnect, action);
					
				NEW_STATE_ENTRY_END()
			NEW_STATE_END();

		
		OLD_STATE_ENTRY_END()


	OLD_STATE_END()

	
			
}



void MRangeBreak::doAction(BrkLib::BrokerConnect& brconnect, Action const& action)
{
	
	LOG_SYMBOL(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_DEBUG, action.symbol_m, "Will execute the action: " << action.toString());
	
	// context
	BrokerContext context = getBrokerContext(action.symbol_m);
	context.action_m = action;
	
	BrkLib::BrokerOperation oper(brconnect);

					
	
	try {

		
			switch(action.action_m) {
				case GET_ENUM_ENTRY(AS_Dummy):
					break;
				case GET_ENUM_ENTRY(AS_ClearBrokerContext):
					{
						context.clear();
						updateBrokerContext(action.symbol_m, context);

						// clear all delayed orders
						oper.removeAllDelayedOrders();

						
					}
					break;

					

				case GET_ENUM_ENTRY(AS_IssueEnterBothOrder): 
					{
					
						// initialize it here
						// install 2 orders!
						BrkLib::Order order_long, order_short;

						order_long.provider_m =  providerForRTSymbols_m;
						order_short.provider_m = providerForRTSymbols_m;
						
  
						// do not forget to update context!!!
						context.orderWorkLong_m.orderID_m = order_long.ID_m;
						context.orderWorkShort_m.orderID_m = order_short.ID_m;

						// TODO
						// because of local
						context.orderWorkLong_m.orderExecuteTime_m = CppUtils::getTime();
						context.orderWorkLong_m.orderInstallPrice_m = action.opBothSideLongInstallPrice_m;

						// TODO
						// because of local
						context.orderWorkShort_m.orderExecuteTime_m = CppUtils::getTime();
						context.orderWorkShort_m.orderInstallPrice_m = action.opBothSideShortInstallPrice_m;


						updateBrokerContext(action.symbol_m, context);
						
						

						order_long.symbolNum_m = action.symbol_m;
						order_long.method_m = order_method_m;
						order_long.place_m = order_place_m;

						order_long.additionalInfo_m = "TFORCE=TIF_DAY";

					
						order_long.orderType_m = BrkLib::OP_BUY_LIMIT;
						
						
						order_long.orPrice_m = action.opBothSideLongInstallPrice_m;
						order_long.orVolume_m = action.volume_m;
						order_long.orPriceType_m = BrkLib::PC_INPUT;

						//oper.unblockingIssueOrder(order_long);
						oper.registerDelayedOrder(BrkLib::DO_EXEC_PRICE_MORE, order_long.orPrice_m, order_long );

						//
						order_short.symbolNum_m = action.symbol_m;
						order_short.method_m = order_method_m;
						order_short.place_m = order_place_m;

						order_short.additionalInfo_m = "TFORCE=TIF_DAY";

					
						order_short.orderType_m = BrkLib::OP_SELL_LIMIT;
						
						
						order_short.orPrice_m = action.opBothSideShortInstallPrice_m;
						order_short.orVolume_m = action.volume_m;
						order_short.orPriceType_m = BrkLib::PC_INPUT;

						//oper.unblockingIssueOrder(order_short);
						oper.registerDelayedOrder(BrkLib::DO_EXEC_PRICE_LESS, order_short.orPrice_m, order_short );
	
						

																				
					}
					break;
				case GET_ENUM_ENTRY(AS_IssueCancelEnterOtherSideOrder): 
					{
						
						// do not forget to update context!!!
						BrkLib::Order order;

						updateBrokerContext(action.symbol_m, context);
												
											
						order.provider_m = providerForRTSymbols_m;
						order.symbolNum_m = action.symbol_m;
						order.method_m = order_method_m;
						order.place_m = order_place_m;
						
						order.orderType_m = BrkLib::OP_CANCEL;
						
						oper.unblockingIssueOrder(order);

						// and then
						oper.removeAllDelayedOrders();
																				 
					}
					break;
				case GET_ENUM_ENTRY(AS_IssueSLTPOrder): 
					{
						
						// do not forget to update context!!!
						// install 2 orders!
						BrkLib::Order order_sl, order_tp;
						context.orderSL_m.orderID_m = order_sl.ID_m;
						context.orderTP_m.orderID_m = order_tp.ID_m;

						// TODO
						// because of local
						context.orderSL_m.orderExecuteTime_m = CppUtils::getTime();
						context.orderSL_m.orderInstallPrice_m = action.opSLInstallPrice_m;

						// TODO
						// because of local
						context.orderTP_m.orderExecuteTime_m = CppUtils::getTime();
						context.orderTP_m.orderInstallPrice_m = action.opTPInstallPrice_m;


						updateBrokerContext(action.symbol_m, context);

						// init 2 orders 

						
						order_sl.provider_m = providerForRTSymbols_m;
						order_sl.symbolNum_m = action.symbol_m;
						order_sl.method_m = order_method_m;
						order_sl.place_m = order_place_m;

						order_sl.additionalInfo_m = "TFORCE=TIF_DAY";

						if (context.directionOfTrade_m == DIRECTION_LONG)
							order_sl.orderType_m = BrkLib::OP_SELL_LIMIT;
						else if (context.directionOfTrade_m == DIRECTION_SHORT)
							order_sl.orderType_m = BrkLib::OP_BUY_LIMIT;
						else
							THROW(CppUtils::OperationFailed, "exc_InvalidSLOperationDirection", "ctx_doAction", action.toString())
							
						
						
						order_sl.orPrice_m = action.opSLInstallPrice_m;
						order_sl.orVolume_m = action.volume_m;
						order_sl.orPriceType_m = BrkLib::PC_INPUT;

						// oper.unblockingIssueOrder(order_sl);

						if (context.directionOfTrade_m == DIRECTION_LONG) 
						{
							oper.registerDelayedOrder( BrkLib::DO_EXEC_PRICE_MORE, action.opSLInstallPrice_m, order_sl );
						}
						else if (context.directionOfTrade_m == DIRECTION_SHORT) 
						{
							oper.registerDelayedOrder( BrkLib::DO_EXEC_PRICE_LESS, action.opSLInstallPrice_m, order_sl );
						}


						//
						order_tp.provider_m = providerForRTSymbols_m;
						order_tp.symbolNum_m = action.symbol_m;
						order_tp.method_m = order_method_m;
						order_tp.place_m = order_place_m;

						order_tp.additionalInfo_m = "TFORCE=TIF_DAY";
					
						if (context.directionOfTrade_m== DIRECTION_LONG)
							order_tp.orderType_m = BrkLib::OP_BUY_LIMIT;
						else if (context.directionOfTrade_m == DIRECTION_SHORT)
							order_tp.orderType_m = BrkLib::OP_SELL_LIMIT;
						else
							THROW(CppUtils::OperationFailed, "exc_InvalidTPOperationDirection", "ctx_doAction", action.toString())
						
						order_tp.orPrice_m = action.opTPInstallPrice_m;
						order_tp.orVolume_m = action.volume_m;
						order_tp.orPriceType_m = BrkLib::PC_INPUT;

						//oper.unblockingIssueOrder(order_tp);

						if (context.directionOfTrade_m == DIRECTION_LONG) 
						{
							oper.registerDelayedOrder( BrkLib::DO_EXEC_PRICE_MORE, action.opTPInstallPrice_m, order_tp );
						}
						else if (context.directionOfTrade_m == DIRECTION_SHORT) 
						{
							oper.registerDelayedOrder( BrkLib::DO_EXEC_PRICE_LESS, action.opTPInstallPrice_m, order_tp );
						}

						


					}
					break;
				case GET_ENUM_ENTRY(AS_IssueCancelSLOrder):
					{
						
						// do not forget to update context!!!
						BrkLib::Order order;
						updateBrokerContext(action.symbol_m, context);

						order.provider_m = providerForRTSymbols_m;
						order.symbolNum_m = action.symbol_m;
						order.method_m = order_method_m;
						order.place_m = order_place_m;
						
						// cancel pending
						// here this is a single by that symbol
						order.orderType_m = BrkLib::OP_CANCEL;
						oper.unblockingIssueOrder(order);

						// and
						oper.removeAllDelayedOrders();
						
					}
					break;
				case GET_ENUM_ENTRY(AS_IssueCancelTPOrder):
					{
						// do not forget to update context!!!
						BrkLib::Order order;
						updateBrokerContext(action.symbol_m, context);

						order.provider_m = providerForRTSymbols_m;
						order.symbolNum_m = action.symbol_m;
						order.method_m = order_method_m;
						order.place_m = order_place_m;

						// cancel pending
						// here this is a single by that symbol
						order.orderType_m = BrkLib::OP_CANCEL;
						oper.unblockingIssueOrder(order);

						//
						// and
						oper.removeAllDelayedOrders();
					}
					break;

				case GET_ENUM_ENTRY(AS_IssueCancelBothOrder):
					{
						// do not forget to update context!!!
						BrkLib::Order order;
						updateBrokerContext(action.symbol_m, context);

						order.provider_m = providerForRTSymbols_m;
						order.symbolNum_m = action.symbol_m;
						order.method_m = order_method_m;
						order.place_m = order_place_m;

						
						// cancel pending
						// by symbol name we will cancel everything
						order.orderType_m = BrkLib::OP_CANCEL;
						oper.unblockingIssueOrder(order);

						//
						// and
						oper.removeAllDelayedOrders();
					}
					break;
				
				default: 
					THROW(CppUtils::OperationFailed, "exc_InvalidAction", "ctx_doAction", action.toString())
			}


			//LOG_SYMBOL_CTX(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_DEBUG, action.symbol_m, "Normally executed action: " << action.toString(),
			//	context.toString()
			//);

			// TODO
			//LOGEVENT(HlpStruct::CommonLog::LL_INFO,  ALG_M_RANGE_BREAK, action.symbol_m << "\n" << "Normally executed action: " << action.toString() );

	}
	catch(BrkLib::LaserFailed &e) {

		TradingEventList elist;

	
		// generate error event
		ACTION_BEGIN(action.action_m)
			ACTION_ENTRY_BEGIN(AS_IssueEnterBothOrder) 
				TradingEvent te(TE_BrokerReject_EnterBoth,  action.symbol_m, -1, true);
				elist.push_back(te);
				
			ACTION_ENTRY_END()

			ACTION_ENTRY_BEGIN(AS_IssueCancelEnterOtherSideOrder) 
				TradingEvent te(TE_BrokerRejectOtherSideCancelled,  action.symbol_m, -1, true);
				elist.push_back(te);
				
			ACTION_ENTRY_END()

			ACTION_ENTRY_BEGIN(AS_IssueSLTPOrder) 
				TradingEvent te(TE_BrokerRejectTPSLPending,  action.symbol_m, -1, true);
				elist.push_back(te);
				
			ACTION_ENTRY_END()

			ACTION_ENTRY_BEGIN(AS_IssueCancelSLOrder) 
				TradingEvent te(TE_BrokerRejectSLCancelled,  action.symbol_m, -1, true);
				elist.push_back(te);
				
			ACTION_ENTRY_END()

			ACTION_ENTRY_BEGIN(AS_IssueCancelBothOrder) 
				TradingEvent te(TE_BrokerRejectBothCancelled,  action.symbol_m, -1, true);
				elist.push_back(te);
				
			ACTION_ENTRY_END()

			ACTION_ENTRY_BEGIN(AS_IssueCancelTPOrder) 
				TradingEvent te(TE_BrokerRejectTPCancelled,  action.symbol_m, -1, true);
				elist.push_back(te);
			
			ACTION_ENTRY_END()

			// 
		ACTION_END()

	

		LOG_SYMBOL_CTX(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_ERROR, action.symbol_m, "Error on action: " << action.toString(),
				context.toString()
			);

		for(int i =0; i < elist.size(); i++) {
				LOG_SYMBOL_CTX(internalRun_name_m,  providerForRTSymbols_m, SLogger::SL_ERROR, action.symbol_m, 
							"Error event (will be delegated to the processing thread): " << elist[i].toString(),
							"Error: " << e.message() << " - " << e.arg() << " - " << e.context()
				);
		}

		// propagate
		// delegate event
		pushProcessingEvent(brconnect, elist);

	}
	catch(CppUtils::OperationFailed &op) {
		LOG_SYMBOL_CTX(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_ERROR,  action.symbol_m, "Unexpected error on doAction(...): " << op.message(), 
			"Action: " << action.toString());
	}
	catch(...) {
		LOG_SYMBOL_CTX(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_ERROR,  action.symbol_m, "Unknown unexpected error on doAction(...)", 
			"Action: " << action.toString());
	}

	
}

BrokerContext& MRangeBreak::getBrokerContext(CppUtils::String const& symbol)
{
	LOCK_FOR_SCOPE(brokerContextMtx_m);
	return brokerContext_m[symbol];
}

void MRangeBreak::updateBrokerContext(CppUtils::String const& symbol, BrokerContext const& context)
{
	LOCK_FOR_SCOPE(brokerContextMtx_m);
	brokerContext_m[symbol] = context;
}

void MRangeBreak::pushProcessingEvent(BrkLib::BrokerConnect const& brkConnect, TradingEventList const& eventList)
{
	// this will push event data that must be processed in the algorithm/thread
	EventProcessingStruct procStruct(brkConnect, eventList);

	{
		LOCK_FOR_SCOPE(receivedProcessingDataMtx_m);
		
		receivedProcessingData_m.push_back(procStruct);
		receivedProcessingDataArrived_m.signalEvent();
	}
}

void MRangeBreak::threadProcessingThread()
{
	threadStarted_m.signalEvent();
	LOG(MSG_INFO, ALG_M_RANGE_BREAK, MRangeBreak::getName() << " - working thread started");


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
				
				machineEventsReaction(procstruct.brokerConnect_m, procstruct.eventList_m);
				
			}

			
		}
		catch(CppUtils::Exception& e) {
				LOG_COMMON(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, "Error in event processing thread: " << e.message());
		}
	#ifdef HANDLE_NATIVE_EXCEPTIONS
		catch(Testframe::Exception& e) {
				LOG_COMMON(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, "MSVC error in event processing thread: " << e.message());
		}
	#endif
		catch(...) {
				LOG_COMMON(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO,  "Unknown error in event processing thread");
		}

	}

	LOG(MSG_INFO, ALG_M_RANGE_BREAK, MRangeBreak::getName() << " - working thread finished");
}

int MRangeBreak::getSecuritiesNumberInProgress() const
{
	int cnt = 0;
	for(map<CppUtils::String,TradingState>::const_iterator it = state_m.begin(); it != state_m.end(); it++) {
		TradingState const& state_i = it->second;
		
		if (!state_i.isIdle() )
			cnt++;

	}

	return cnt;
}

bool MRangeBreak::isWithinWorkingTime(double const &ttime) const
{
	double diffFromDayBegin = ttime - CppUtils::roundToBeginningOfTheDay(ttime);

	if (diffFromDayBegin < unixTimeBeginTrade_m || diffFromDayBegin > unixTimeEndTrade_m)
		return false;
	
	return true;

}



}; // end of namespace