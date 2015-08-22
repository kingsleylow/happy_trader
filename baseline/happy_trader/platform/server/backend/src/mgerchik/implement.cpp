#include "implement.hpp"
#include "../brkliblaser/brkliblaser.hpp"

namespace AlgLib {

#define ALG_M_GERCHIK "ALG_M_GERCHIK"
//#define PROVIDER "laser_provider_history"
// ------------------------

int bootProcssingThread (void *ptr)
{
		// handle run-time exceptions
#ifdef HANDLE_NATIVE_EXCEPTIONS
			Testframe::init();
#endif

			
		MGerchik* pThis = (MGerchik*)ptr; 
		try {
			pThis->threadProcessingThread();
		}
		catch(CppUtils::Exception& e) {
			LOG(MSG_FATAL, ALG_M_GERCHIK, "Fatal exception: " << e.message());
		}
#ifdef HANDLE_NATIVE_EXCEPTIONS
		catch(Testframe::Exception& e) {
			LOG(MSG_FATAL, ALG_M_GERCHIK, "Fatal MSVC exception: " << e.message());
		}
#endif
		catch(...) {
			LOG(MSG_FATAL, ALG_M_GERCHIK, "Unknown fatal exception");
		}
		return 42;

}

// ------------------------

AlgorithmBase *creator (Inqueue::AlgorithmHolder* algholder)
{
		return new MGerchik(*algholder);
}

void terminator (AlgorithmBase *pthis)
{
		assert(pthis);
		delete pthis;
}

AlgorithmBase::CreatorAlgorithmFun createAlgorithm = creator;

AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm = terminator;


// ------------------------

MGerchik::MGerchik( Inqueue::AlgorithmHolder& algHolder):
	AlgorithmBase(algHolder),
	thread_m(NULL)
{
	thread_m = new CppUtils::Thread(bootProcssingThread, this);

	// if cannot start thread for 10 seconds
	if (!threadStarted_m.lock(10000))
		THROW(CppUtils::OperationFailed, "exc_CannorStartProcessingThread", "ctx_MGerchikCtor", "");
}

MGerchik::~MGerchik()
{  
	shutDownThread_m.signalEvent();

	thread_m->waitForThread();

	// Delete thread
	delete thread_m;
	thread_m = NULL;

	LOG(MSG_INFO, ALG_M_GERCHIK, MGerchik::getName() << " - deinitialized");
	
}

void MGerchik::onLibraryInitialized(
	Inqueue::AlgBrokerDescriptor const& descriptor, 
	BrkLib::BrokerBase* brokerlib, 
	CppUtils::String const& runName,
	CppUtils::String const& comment  
)
{
	
	// will be used for drawable objects
	internalRun_name_m = runName;

	// defaults
	precalcOnly_m = true;
	SBA_m = 2;

	// --------------------
	logBasePath_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "LOG_BASE_PATH");
	
	if (logBasePath_m.size() <=0)
		THROW(CppUtils::OperationFailed, "exc_InvalidLogBasePath", "ctx_AlgInit", logBasePath_m);


	logger_m.initialize(logBasePath_m, SLogger::SL_DEBUG);


	precalcDataFile_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "FILE_DATA_PRECALC_OUT");

	// read list of stocks
	CppUtils::String const& symbols_level1_file = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "FILE_FOR_PRECALC_SYMBOLS");
	if (CppUtils::fileExists(symbols_level1_file)) {
	
      parseFileSymbolList(symbolsLevel1_precalc_m, symbols_level1_file);
	};
	
	CppUtils::String const& index_file = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "FILE_FOR_PRECALC_INDEXES");
	if (CppUtils::fileExists(index_file)) {
		parseFileSymbolList(indexList_precalc_m, index_file);
	}

	CppUtils::String const& symbol_rt_file = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "FILE_FOR_RT_SYMBOLS");
	if (CppUtils::fileExists(symbol_rt_file)) {
	
      parseFileSymbolList(symbolLevel1_rt_m, symbol_rt_file);
	};


	CppUtils::String const& index_rt_file = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "FILE_FOR_RT_INDEXES");
		
	if (CppUtils::fileExists(index_rt_file)) {
	
      parseFileSymbolList(indexList_rt_m, index_rt_file);
	};

	depthCorrelation_m = GET_ALG_TRADE_INT(descriptor.initialAlgBrkParams_m, "CORRELATION_DEPTH");
	
	providerForPrecalcHistory_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "PROVIDER_FOR_PRECALC");
	providerForRTSymbols_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "PROVIDER_FOR_RT_SYMBOLS"); // this will be used for drawable objects
	providerForRTIndexes_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "PROVIDER_FOR_RT_INDEXES");
	
	precalcOnly_m = GET_ALG_TRADE_BOOL(descriptor.initialAlgBrkParams_m, "IS_PRECALC_ONLY");

	SBA_m = GET_ALG_TRADE_DOUBLE(descriptor.initialAlgBrkParams_m, "SBA");
	PR_m = GET_ALG_TRADE_DOUBLE(descriptor.initialAlgBrkParams_m, "PR");
	LONG_ONLY_m = GET_ALG_TRADE_BOOL(descriptor.initialAlgBrkParams_m, "LONG_ONLY");
	TIMEOUT_AFTER_OPEN_m = GET_ALG_TRADE_INT(descriptor.initialAlgBrkParams_m, "TIMEOUT_AFTER_OPEN");

	// if we operate only with one security at a time
	SINGLETON_m = GET_ALG_TRADE_BOOL(descriptor.initialAlgBrkParams_m, "SINGLETON");

	SHARES_m = GET_ALG_TRADE_DOUBLE(descriptor.initialAlgBrkParams_m, "SHARES");

	OPEN_IOC_m = GET_ALG_TRADE_BOOL(descriptor.initialAlgBrkParams_m, "OPEN_IOC");

	// if IOC open - e.i. pass timeout - if valid
	OPEN_IOC_TIMEOUT_SEC_m = GET_ALG_TRADE_INT(descriptor.initialAlgBrkParams_m, "OPEN_IOC_TIMEOUT_SEC");

	SLIDE_m = GET_ALG_TRADE_DOUBLE(descriptor.initialAlgBrkParams_m, "SLIDE");

	RISK_MIN_m = GET_ALG_TRADE_DOUBLE(descriptor.initialAlgBrkParams_m, "RISK_MIN"); // min risk

	// begin
	CppUtils::String strContent;
	strContent = "List of pre-history symbols (provider: "+ providerForPrecalcHistory_m +") : <br>";

	for(CppUtils::StringSet::const_iterator it = symbolsLevel1_precalc_m.begin(); it != symbolsLevel1_precalc_m.end(); it++ ) {
		CppUtils::String const& symbol_i = *it;
		strContent += symbol_i + ", ";
	}

	strContent += "<br> list of pre-history indexes (provider: "+ providerForPrecalcHistory_m +") : <br>";

	for(CppUtils::StringSet::const_iterator it = indexList_precalc_m.begin(); it != indexList_precalc_m.end(); it++ ) {
			CppUtils::String const& index_i = *it;
			strContent += index_i + ", ";
	}

	strContent += "<br> list of RT symbols (provider: "+ providerForRTSymbols_m +") : <br>";
		for(CppUtils::StringSet::const_iterator it = symbolLevel1_rt_m.begin(); it != symbolLevel1_rt_m.end(); it++ ) {
			CppUtils::String const& symbols_rt_i = *it;
			strContent += symbols_rt_i + ", ";
	}

	strContent += "<br> list of RT indexes (provider: "+ providerForRTIndexes_m +") : <br>";
		for(CppUtils::StringSet::const_iterator it = indexList_rt_m.begin(); it != indexList_rt_m.end(); it++ ) {
			CppUtils::String const& index_rt_i = *it;
			strContent += index_rt_i + ", ";
	}

	strContent += "<br>";
	strContent += "OUT precalc file: "+ precalcDataFile_m;
	strContent += "<br>";
	strContent += "Base log path: "+ logBasePath_m;

	// ---------------
	// broker param
	order_method_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "ORDER_METHOD");
	order_place_m  = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m,  "ORDER_PLACE" );

	ALERTEVENTNC("",BrkLib::AP_MEDIUM,ALG_M_GERCHIK, strContent);
	LOG_COMMON( internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, "Gerchik library started");

		

}

void MGerchik::onLibraryFreed()
{
	LOG_COMMON( internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO,"Gerchik library freed");

	logger_m.deinitialize();
}

void MGerchik::onBrokerResponseArrived( 
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
)
{
	
	if (precalcOnly_m)
		return;


	// process all responses
	if (bresponse.type_m == BrkLib::RT_BrokerResponseEquity) {
		LOG_EQUITY(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, (BrkLib::BrokerResponseEquity const&)bresponse);
	}
	else if (bresponse.type_m == BrkLib::RT_BrokerResponseOrder) {
			BrkLib::BrokerResponseOrder const& bRespOrder = (BrkLib::BrokerResponseOrder const&)bresponse;

			// resolve context - here is only read only
			BrokerContext context = getBrokerContext(bRespOrder.symbolNum_m);
			

			TradingEventList elist;
			
			if (bRespOrder.resultCode_m==BrkLib::PT_ORDER_ESTABLISHED ) {
				if (bRespOrder.resultDetailCode_m & BrkLib::ODR_ORDER_INSTALLED) {
					
					// pending
					// select type of request
					ACTION_BEGIN(context.action_m.action_m)
						ACTION_ENTRY_BEGIN( Action::AS_IssueOpenLong) 
							TradingEvent te(TradingEvent::TE_BrokerConfirmPendingLong,  bRespOrder.symbolNum_m, bRespOrder.opOpenTime_m, true);
							elist.push_back(te);

							context.brokerInstallPrice_m = bRespOrder.opOpenPrice_m;
							context.posInstallTime_m = CppUtils::getTime(); // install local time
						

						ACTION_ENTRY_END()
						ACTION_ENTRY_BEGIN( Action::AS_IssueOpenShort) 
							TradingEvent te(TradingEvent::TE_BrokerConfirmPendingShort,  bRespOrder.symbolNum_m, bRespOrder.opOpenTime_m, true);
							elist.push_back(te);

							context.brokerInstallPrice_m = bRespOrder.opOpenPrice_m;
							context.posInstallTime_m = CppUtils::getTime(); // install local time
							

						ACTION_ENTRY_END()
						
						ACTION_ENTRY_BEGIN(Action::AS_IssueCloseLong) 
						ACTION_ENTRY_END()
						
						ACTION_ENTRY_BEGIN(Action::AS_IssueCloseShort) 
						ACTION_ENTRY_END()
						
						ACTION_ENTRY_BEGIN(Action::AS_IssueClose2Long) 
						ACTION_ENTRY_END()
						
						ACTION_ENTRY_BEGIN(Action::AS_IssueClose2Short) 
						ACTION_ENTRY_END()
						
						ACTION_ENTRY_BEGIN(Action::AS_IssueCancelLong) 
						ACTION_ENTRY_END()

						ACTION_ENTRY_BEGIN(Action::AS_IssueCancelShort) 
						ACTION_ENTRY_END()

						ACTION_ENTRY_BEGIN(Action::AS_IssueClose3Long) 
						ACTION_ENTRY_END()
						
						ACTION_ENTRY_BEGIN(Action::AS_IssueClose3Short) 
						ACTION_ENTRY_END()
					ACTION_END()

				}
				else if (bRespOrder.resultDetailCode_m & BrkLib::ODR_ORDER_EXECUTED) {
					
					// weh have rights return only final trades
					if (bRespOrder.RESPONSE_TAG_REMAIN_SHARES == 0 ) {
					
						// executed
						ACTION_BEGIN(context.action_m.action_m)
							ACTION_ENTRY_BEGIN(Action::AS_IssueOpenLong) 
								TradingEvent te(TradingEvent::TE_BrokerConfirmTradeLong,  bRespOrder.symbolNum_m, bRespOrder.opOpenTime_m, true);
								elist.push_back(te);

								context.brokerOpenPrice_m = bRespOrder.opOpenPrice_m;
								context.posOpenTime_m = CppUtils::getTime(); // open local time

							
							ACTION_ENTRY_END()
							ACTION_ENTRY_BEGIN( Action::AS_IssueOpenShort) 
								TradingEvent te(TradingEvent::TE_BrokerConfirmTradeShort,  bRespOrder.symbolNum_m, bRespOrder.opOpenTime_m, true);
								elist.push_back(te);

							
								context.brokerOpenPrice_m = bRespOrder.opOpenPrice_m;
								context.posOpenTime_m = CppUtils::getTime(); // open local time

							ACTION_ENTRY_END()
							ACTION_ENTRY_BEGIN(Action::AS_IssueCloseLong) 
								TradingEvent te(TradingEvent::TE_BrokerConfirmCloseLong,  bRespOrder.symbolNum_m, bRespOrder.opOpenTime_m, true);
								elist.push_back(te);
							
								context.brokerClosePrice_m = bRespOrder.opOpenPrice_m;

							ACTION_ENTRY_END()
							ACTION_ENTRY_BEGIN(Action::AS_IssueCloseShort) 
								TradingEvent te(TradingEvent::TE_BrokerConfirmCloseShort,  bRespOrder.symbolNum_m, bRespOrder.opOpenTime_m, true);
								elist.push_back(te);

								context.brokerClosePrice_m = bRespOrder.opOpenPrice_m;
								
							ACTION_ENTRY_END()

							ACTION_ENTRY_BEGIN(Action::AS_IssueClose2Long) 
								TradingEvent te(TradingEvent::TE_BrokerConfirmClose2Long,  bRespOrder.symbolNum_m, bRespOrder.opOpenTime_m, true);
								elist.push_back(te);

								context.brokerClosePrice_m = bRespOrder.opOpenPrice_m;
								
							ACTION_ENTRY_END()
							
							ACTION_ENTRY_BEGIN(Action::AS_IssueClose2Short) 
								TradingEvent te(TradingEvent::TE_BrokerConfirmClose2Short,  bRespOrder.symbolNum_m, bRespOrder.opOpenTime_m, true);
								elist.push_back(te);

								context.brokerClosePrice_m = bRespOrder.opOpenPrice_m;
								

							ACTION_ENTRY_END()

							ACTION_ENTRY_BEGIN(Action::AS_IssueCancelLong) 
							ACTION_ENTRY_END()
							
							ACTION_ENTRY_BEGIN(Action::AS_IssueCancelShort) 
							ACTION_ENTRY_END()

							ACTION_ENTRY_BEGIN(Action::AS_IssueClose3Long) 
								TradingEvent te(TradingEvent::TE_BrokerConfirmClose3Long,  bRespOrder.symbolNum_m, bRespOrder.opOpenTime_m, true);
								elist.push_back(te);

								context.brokerClosePrice_m = bRespOrder.opOpenPrice_m;
								
							ACTION_ENTRY_END()
						
							ACTION_ENTRY_BEGIN(Action::AS_IssueClose3Short) 
								TradingEvent te(TradingEvent::TE_BrokerConfirmClose3Short,  bRespOrder.symbolNum_m, bRespOrder.opOpenTime_m, true);
								elist.push_back(te);

								context.brokerClosePrice_m = bRespOrder.opOpenPrice_m;
								
							ACTION_ENTRY_END()

						ACTION_END()

					} 
					
				}
			}
			else if (bRespOrder.resultCode_m==BrkLib::PT_ORDER_CANCELLED) {
				// order cancelled
				ACTION_BEGIN(context.action_m.action_m)
					ACTION_ENTRY_BEGIN(Action::AS_IssueOpenLong) 
						TradingEvent te(TradingEvent::TE_BrokerRejectTradeLong,  bRespOrder.symbolNum_m, bRespOrder.opOpenTime_m, true);
						elist.push_back(te);
						

					ACTION_ENTRY_END()
					ACTION_ENTRY_BEGIN(Action::AS_IssueOpenShort) 
						TradingEvent te(TradingEvent::TE_BrokerRejectTradeShort,  bRespOrder.symbolNum_m, bRespOrder.opOpenTime_m, true);
						elist.push_back(te);
					

					ACTION_ENTRY_END()
					ACTION_ENTRY_BEGIN(Action::AS_IssueCloseLong) 
						TradingEvent te(TradingEvent::TE_BrokerRejectCloseLong,  bRespOrder.symbolNum_m, bRespOrder.opOpenTime_m, true);
						elist.push_back(te);
						
					ACTION_ENTRY_END()

					ACTION_ENTRY_BEGIN(Action::AS_IssueCloseShort) 
						TradingEvent te(TradingEvent::TE_BrokerRejectCloseShort,  bRespOrder.symbolNum_m, bRespOrder.opOpenTime_m, true);
						elist.push_back(te);
						

					ACTION_ENTRY_END()

					ACTION_ENTRY_BEGIN(Action::AS_IssueClose2Long) 
						TradingEvent te(TradingEvent::TE_BrokerRejectClose2Long,  bRespOrder.symbolNum_m, bRespOrder.opOpenTime_m, true);
						elist.push_back(te);
						
					ACTION_ENTRY_END()

					ACTION_ENTRY_BEGIN(Action::AS_IssueClose2Short) 
						TradingEvent te(TradingEvent::TE_BrokerRejectClose2Short,  bRespOrder.symbolNum_m, bRespOrder.opOpenTime_m, true);
						elist.push_back(te);
						
					ACTION_ENTRY_END()

					ACTION_ENTRY_BEGIN(Action::AS_IssueCancelLong) 
						TradingEvent te(TradingEvent::TE_BrokerConfirmCancelLong,  bRespOrder.symbolNum_m, bRespOrder.opOpenTime_m, true);
						elist.push_back(te);
						
					ACTION_ENTRY_END()

					ACTION_ENTRY_BEGIN(Action::AS_IssueCancelShort) 
						TradingEvent te(TradingEvent::TE_BrokerConfirmCancelShort,  bRespOrder.symbolNum_m, bRespOrder.opOpenTime_m, true);
						elist.push_back(te);
						
					ACTION_ENTRY_END()

					ACTION_ENTRY_BEGIN(Action::AS_IssueClose3Long) 
						TradingEvent te(TradingEvent::TE_BrokerRejectClose3Long,  bRespOrder.symbolNum_m, bRespOrder.opOpenTime_m, true);
						elist.push_back(te);
						
					ACTION_ENTRY_END()
				
					ACTION_ENTRY_BEGIN(Action::AS_IssueClose3Short) 
						TradingEvent te(TradingEvent::TE_BrokerRejectClose3Short,  bRespOrder.symbolNum_m, bRespOrder.opOpenTime_m, true);
						elist.push_back(te);
						
					ACTION_ENTRY_END()

				ACTION_END()

			}
			else if (bRespOrder.resultCode_m==BrkLib::PT_GENERAL_ERROR_HAPPENED) {
				// order errored
				ACTION_BEGIN(context.action_m.action_m)
					ACTION_ENTRY_BEGIN(Action::AS_IssueOpenLong) 
						TradingEvent te(TradingEvent::TE_BrokerRejectTradeLong,  bRespOrder.symbolNum_m, bRespOrder.opOpenTime_m, true);
						elist.push_back(te);
						

					ACTION_ENTRY_END()
					ACTION_ENTRY_BEGIN(Action::AS_IssueOpenShort) 
						TradingEvent te(TradingEvent::TE_BrokerRejectTradeShort,  bRespOrder.symbolNum_m, bRespOrder.opOpenTime_m, true);
						elist.push_back(te);
					

					ACTION_ENTRY_END()
					ACTION_ENTRY_BEGIN(Action::AS_IssueCloseLong) 
						TradingEvent te(TradingEvent::TE_BrokerRejectCloseLong,  bRespOrder.symbolNum_m, bRespOrder.opOpenTime_m, true);
						elist.push_back(te);
						
					ACTION_ENTRY_END()

					ACTION_ENTRY_BEGIN(Action::AS_IssueCloseShort) 
						TradingEvent te(TradingEvent::TE_BrokerRejectCloseShort,  bRespOrder.symbolNum_m, bRespOrder.opOpenTime_m, true);
						elist.push_back(te);
						

					ACTION_ENTRY_END()

					ACTION_ENTRY_BEGIN(Action::AS_IssueClose2Long) 
						TradingEvent te(TradingEvent::TE_BrokerRejectClose2Long,  bRespOrder.symbolNum_m, bRespOrder.opOpenTime_m, true);
						elist.push_back(te);
						
					ACTION_ENTRY_END()

					ACTION_ENTRY_BEGIN(Action::AS_IssueClose2Short) 
						TradingEvent te(TradingEvent::TE_BrokerRejectClose2Short,  bRespOrder.symbolNum_m, bRespOrder.opOpenTime_m, true);
						elist.push_back(te);
						
					ACTION_ENTRY_END()

					ACTION_ENTRY_BEGIN(Action::AS_IssueCancelLong) 
						TradingEvent te(TradingEvent::TE_BrokerRejectCancelLong,  bRespOrder.symbolNum_m, bRespOrder.opOpenTime_m, true);
						elist.push_back(te);
						
					ACTION_ENTRY_END()

					ACTION_ENTRY_BEGIN(Action::AS_IssueCancelShort) 
						TradingEvent te(TradingEvent::TE_BrokerRejectCancelShort,  bRespOrder.symbolNum_m, bRespOrder.opOpenTime_m, true);
						elist.push_back(te);
						
					ACTION_ENTRY_END()

					ACTION_ENTRY_BEGIN(Action::AS_IssueClose3Long) 
						TradingEvent te(TradingEvent::TE_BrokerRejectClose3Long,  bRespOrder.symbolNum_m, bRespOrder.opOpenTime_m, true);
						elist.push_back(te);
						
					ACTION_ENTRY_END()
				
					ACTION_ENTRY_BEGIN(Action::AS_IssueClose3Short) 
						TradingEvent te(TradingEvent::TE_BrokerRejectClose3Short,  bRespOrder.symbolNum_m, bRespOrder.opOpenTime_m, true);
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

void MGerchik::onRtProviderSynchEvent (
			Inqueue::ContextAccessor& contextaccessor,
			HlpStruct::RtData::SynchType const synchFlg,
			double const& synchTime,
			CppUtils::String const& providerName,
			CppUtils::String const& message
		)
	{
	}



void MGerchik::onDataArrived(
	HlpStruct::RtData const& rtdata, 
	Inqueue::ContextAccessor& historyaccessor, 
	Inqueue::TradingParameters const& descriptor,
	BrkLib::BrokerBase* brokerlib,
	CppUtils::String const& runName,
	CppUtils::String const& comment
)
{
	
	if (precalcOnly_m)
		return;

	BrkLib::BrokerConnect brkConnect(brokerlib, "gerchik", false, false, runName.c_str(), comment.c_str());
	brkConnect.connect();
		
	if (!brkConnect.getSession().isConnected()) {
		LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  ALG_M_GERCHIK, "onDataArrived(...) session cannot be established: " << brkConnect.getSession().getConnection());
		return;
	}

	{
		LOCK_FOR_SCOPE(priceRTDataMtx_m);

		// session
		map<CppUtils::String, HlpStruct::RtData >::iterator it = rtData_m.find(rtdata.getSymbol());

		if(it == rtData_m.end()) {
			rtData_m[rtdata.getSymbol()] = rtdata;
			return;
		}
	}

		
	if (rtdata.getProvider2()==providerForRTSymbols_m) {
		LOG_PRINT(internalRun_name_m, providerForRTSymbols_m, rtdata.getSymbol2(), rtdata.getRtData().bid_m, rtdata.getRtData().ask_m, rtdata.getRtData().volume_m, rtdata.getRtData().color_m );
	}

	// event
	TradingEventList elist;
		
	generateNewEvent(brkConnect, elist, rtdata);
	machineEventsReaction(brkConnect, elist);
		
	{
		LOCK_FOR_SCOPE(priceRTDataMtx_m);

		// update
		rtData_m[ rtdata.getSymbol() ] = rtdata;
	
	}
	
}

void MGerchik::onLevel2DataArrived(
		HlpStruct::RtLevel2Data const& level2data,
		Inqueue::ContextAccessor& historyaccessor, 
		Inqueue::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
	)
{

	if (precalcOnly_m)
		return;


}

void MGerchik::onLevel1DataArrived(
		HlpStruct::RtLevel1Data const& level1data,
		Inqueue::ContextAccessor& historyaccessor, 
		Inqueue::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
	)
{
	

	if (precalcOnly_m)
		return;

	

	/*
	if (level1data.best_ask_volume_m==0 || level1data.best_bid_volume_m ==0 || level1data.best_ask_price_m == 0 || level1data.best_bid_price_m == 0)
	{
		// wrong data
		LOG_SYMBOL_CTX( providerForRTSymbols_m, SLogger::SL_ERROR, level1data.getSymbol(), 
			"Level 1 data do not have all good fields",
			"level1data1=" << level1data.toString()
		)

		return;

	}
	*/

	// session
	BrkLib::BrokerConnect brkConnect(brokerlib, "gerchik", false, false, runName.c_str(), comment.c_str());
	brkConnect.connect();
		
	if (!brkConnect.getSession().isConnected()) {
		LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  ALG_M_GERCHIK, "onLevel1DataArrived(...) session cannot be established: " << brkConnect.getSession().getConnection());
		return;
	}

	HlpStruct::RtLevel1Data level1data_adopted = level1data;

	{
		LOCK_FOR_SCOPE(priceLevel1DataMtx_m);
	
		//
		map<CppUtils::String, HlpStruct::RtLevel1Data >::iterator it = level1Data_m.find(level1data_adopted.getSymbol());

		if(it == level1Data_m.end()) {
			level1Data_m[level1data_adopted.getSymbol()] = level1data_adopted;
			return;
		}

		// remove zeros but use the old price
		
		if (level1data_adopted.best_ask_price_m == 0 && level1data_adopted.best_ask_volume_m==0) {
			if (it->second.best_ask_price_m==0) {
				// if past iz zero - return
				return;
			}

			level1data_adopted.best_ask_price_m = it->second.best_ask_price_m;
			
		}
		// if volume only
		else if (level1data_adopted.best_ask_volume_m==0) {
			level1data_adopted.best_ask_volume_m = 1.0;
		}

		// bid //
		if (level1data_adopted.best_bid_price_m == 0 && level1data_adopted.best_bid_volume_m==0) {
			if (it->second.best_bid_price_m==0) {
				// if past iz zero - return
				return;
			}

			level1data_adopted.best_bid_price_m = it->second.best_bid_price_m;
			level1data_adopted.best_bid_volume_m = it->second.best_bid_volume_m;
		}
		else if (level1data_adopted.best_bid_volume_m==0) {
			level1data_adopted.best_bid_volume_m = 1.0;
		}

	}
	
	// log
	LOG_LEVEL1(internalRun_name_m, level1data_adopted.getProvider2(), level1data_adopted.getSymbol2(), 
		level1data_adopted.best_bid_price_m, level1data_adopted.best_bid_volume_m, 
		level1data_adopted.best_ask_price_m, level1data_adopted.best_ask_volume_m);


	// event
	TradingEventList elist;

	{
		LOCK_FOR_SCOPE(currentLevel1DataMtx_m);
		// current level 1 data
		currentLevel1Data_m[level1data_adopted.getSymbol()] = level1data_adopted;
	}

	generateNewEvent(brkConnect, elist, level1data_adopted);
	machineEventsReaction(brkConnect, elist);
		
	{
		LOCK_FOR_SCOPE(priceLevel1DataMtx_m);

		// update
		level1Data_m[level1data.getSymbol()] = level1data_adopted;
	}


}


void MGerchik::onEventArrived(
				Inqueue::ContextAccessor& contextaccessor, 
				HlpStruct::CallingContext& callContext,
				bool& result,
				CppUtils::String &customOutputData
		)
{
	result = false;
}



void MGerchik::onThreadStarted(Inqueue::ContextAccessor& contextaccessor, bool const firstLib, bool const lastLib) {
	LOGEVENT(HlpStruct::CommonLog::LL_INFO,  ALG_M_GERCHIK, "I'm started, precalc only=" << precalcOnly_m);
	calculateCorrelation(depthCorrelation_m, contextaccessor);

}


CppUtils::String const& MGerchik::getName() const
{
	static const CppUtils::String build_date(__TIMESTAMP__);
	static const CppUtils::String name( "[ gerchik algorithm ] - build: " + build_date );
	return name;
}

/**
* Helpers
*/

void MGerchik::parseFileSymbolList(CppUtils::StringSet& symbols, CppUtils::String const& fileName)
{
	CppUtils::String content;
	if (!CppUtils::readContentFromFile2(fileName, content))
		THROW(CppUtils::OperationFailed, "exc_CannotReadFile", "ctx_parseSymbolFile", fileName);

	CppUtils::StringList slist;
	CppUtils::tokenize(content, slist, "\n");

	symbols.clear();
	for(int i = 0; i < slist.size(); i++) {
		symbols.insert(slist[i]);
	}

} 

void MGerchik::calculateCorrelation(int const depth, Inqueue::ContextAccessor& historyaccessor)
{

	
	CppUtils::String logCorrelContent;

	double now_t = CppUtils::getTime();
	double day_begin_t = CppUtils::roundToBeginningOfTheDay(now_t);
	double day_begin_yesterday_t = day_begin_t - 24*60*60;

	ALERTEVENTNC("",BrkLib::AP_MEDIUM,ALG_M_GERCHIK, "Now is: " << CppUtils::getGmtTime2(now_t) <<
		" Day begin: " << CppUtils::getGmtTime2(day_begin_t) <<
		" Day begin yesterday: " << CppUtils::getGmtTime2(day_begin_yesterday_t)
	);

	

	CppUtils::String strContent("SYMBOL;");

	for(CppUtils::StringSet::const_iterator it = indexList_precalc_m.begin(); it != indexList_precalc_m.end(); it++ ) {
			CppUtils::String const& index_i = *it;
			strContent += "CORREL "+ index_i + ";";

	}
	strContent +="RANGE VALUE($);THIS OPEN;LAST CLOSE;LAST DATE;";
	strContent.resize(strContent.size()-1);
	strContent += "\n";
	
	// prices
	HlpStruct::PriceDataList symbol_list, index_list;

	int i = 0;
	for(CppUtils::StringSet::const_iterator it = symbolsLevel1_precalc_m.begin(); it != symbolsLevel1_precalc_m.end(); it++ ) {
		CppUtils::String const& symbol_i = *it;
		int cache_symbol_id = historyaccessor.cacheSymbol(providerForPrecalcHistory_m, symbol_i, Inqueue::AP_Day, 1);

		if (cache_symbol_id >0)	{

			HlpStruct::PriceData const& unclosed_symbol = historyaccessor.getUnclosedCandle(cache_symbol_id);

						
			

			//symbolHelperData_m[symbol_i].thisDayOpen_m = unclosed_symbol.open2_m;

						

			strContent += "\"" + symbol_i+"\";";
			
			
			for(CppUtils::StringSet::const_iterator it = indexList_precalc_m.begin(); it != indexList_precalc_m.end(); it++ ) {
				CppUtils::String const& index_i = *it;
				
			
			
				int cache_index_id = historyaccessor.cacheSymbol(providerForPrecalcHistory_m, index_i, Inqueue::AP_Day, 1);

				if (cache_index_id > 0) {
				
					// get data

					// dumps
					//CppUtils::String dumped_symbols = historyaccessor.dump_reverse(cache_symbol_id, day_begin_yesterday_t);
					//CppUtils::String dumped_indexes = historyaccessor.dump_reverse(cache_index_id, day_begin_yesterday_t);
					

					//CppUtils::saveContentInFile2("c:\\temp\\d\\"+symbol_i+"_dumped_symbols_"+CppUtils::long2String(i)+".txt", dumped_symbols);
					//CppUtils::saveContentInFile2("c:\\temp\\d\\"+index_i+"_dumped_indexes_"+CppUtils::long2String(i)+".txt", dumped_indexes);
										

					
					HlpStruct::PriceData const& unclosed_index = historyaccessor.getUnclosedCandle(cache_index_id);

					Inqueue::DataPtr ptr_symbol = historyaccessor.dataLast(cache_symbol_id);
					Inqueue::DataPtr ptr_index = historyaccessor.dataLast(cache_index_id);

					symbol_list.clear();

					if (unclosed_symbol.time_m <= day_begin_yesterday_t) {
						symbol_list.push_back(unclosed_symbol);

						//actually our date must be contained here
						// bid
						symbolHelperData_m[symbol_i].lastDayClose_m = unclosed_symbol.close2_m;
						symbolHelperData_m[symbol_i].dateLastDayClose_m = unclosed_symbol.time_m;
						symbolHelperData_m[symbol_i].symbol_m = symbol_i;
  
						LOG_SYMBOL(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO,  symbol_i, "Symbol helper data: \n" << symbolHelperData_m[symbol_i].toString() );

						LOG_COMMON(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, "Found symbol from history provider, cached last date data: " << unclosed_symbol.toString());
					}

				
  
					// find only before the previous date
					historyaccessor.dataTimeFind(day_begin_yesterday_t, cache_symbol_id, ptr_symbol);

				
					if (historyaccessor.getBackWardPriceList(cache_symbol_id, symbol_list, ptr_symbol, depth)) {
						// symbol

						

						index_list.clear();
						if (unclosed_index.time_m <= day_begin_yesterday_t) {
							index_list.push_back(unclosed_index);

							//actually our date must be contained here
							// bid
							symbolHelperData_m[index_i].lastDayClose_m = unclosed_index.close2_m;
							symbolHelperData_m[index_i].dateLastDayClose_m = unclosed_index.time_m;
							symbolHelperData_m[index_i].symbol_m = index_i;

							LOG_COMMON(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, "Found index from history provider, cached last date data: " << unclosed_index.toString());
						}

						// find what is before the previous date
						historyaccessor.dataTimeFind(day_begin_yesterday_t, cache_index_id, ptr_index);
						if (historyaccessor.getBackWardPriceList(cache_index_id, index_list, ptr_index, depth)) {
							// index
							// all is here

						

						
						
							double correl = Math::calcCorrelation<Math::P_CloseBid>(symbol_list, index_list);
							strContent += "\"" + CppUtils::float2String(correl, -1,2)+"\";";

							// log
							logCorrelContent += "CORRELATION CALCULATED (VIA CLOSE): " + CppUtils::float2String(correl,-1, 3) + " >>>>\n";
							logCorrelContent += "SYMBOL: " + symbol_i+ "\n";

							for(int g = 0; g < symbol_list.size(); g++) {
								logCorrelContent += "#"+CppUtils::long2String(g) + ": " +  symbol_list[g].toCSVBidString() + "\n";
							}

							logCorrelContent += "\n";

							//
							logCorrelContent += "INDEX: " + index_i+ "\n";

							for(int g = 0; g < index_list.size(); g++) {
								logCorrelContent += "#"+CppUtils::long2String(g) + ": " +  index_list[g].toCSVBidString() + "\n";
							}

							logCorrelContent += "\n";
							
						}
					}



					
				
				} // end cache index
			

				// advance index
				i++; 

			} // end loop on index 
		
			// range on unclosed date
			// bid
			double range = unclosed_symbol.high2_m - unclosed_symbol.low2_m;
			
			strContent += "\"" + CppUtils::float2String(range, -1, 4) + "\";";
			strContent += "\"" + CppUtils::float2String(symbolHelperData_m[symbol_i].thisDayOpen_m, -1, 4) + "\";";
			strContent += "\"" + CppUtils::float2String(symbolHelperData_m[symbol_i].lastDayClose_m, -1, 4) + "\";";
			strContent += "\"" + CppUtils::getGmtTime(unclosed_symbol.time_m)+"\";";
			

			// report
			strContent.resize(strContent.size()-1);
			strContent += "\n";

		} // end cache symbol

	} // end loop on symbol 

	//ALERTEVENTNC("",BrkLib::AP_MEDIUM,ALG_M_GERCHIK, strContent);
	
	

	CppUtils::saveContentInFile2(precalcDataFile_m, strContent);
	CppUtils::saveContentInFile2(precalcDataFile_m+".clog", logCorrelContent);

	
	// need to fill helper structure with symbols and indexes

}


void MGerchik::generateNewEvent(BrkLib::BrokerConnect& brconnect, TradingEventList& elist, HlpStruct::RtLevel1Data const& level1data)
{
	
	double risk = (level1data.best_ask_price_m - level1data.best_bid_price_m)+0.02;
	
	// here we shall check 
	if (providerForRTSymbols_m == level1data.getProvider()) {
		if (symbolLevel1_rt_m.find(level1data.getSymbol2()) != symbolLevel1_rt_m.end()) {

				
			double lastBid  =-1;
			double lastAsk  = -1;


			{ 
				LOCK_FOR_SCOPE(priceLevel1DataMtx_m);

				// check last beid and ask prices - must exist
				map<CppUtils::String, HlpStruct::RtLevel1Data >::const_iterator it = level1Data_m.find(level1data.getSymbol2());
				if (it==level1Data_m.end()) {

					LOG_COMMON(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_ERROR, "It is error - the past Level1 data must already exist: " << level1data.getSymbol2() );
					return;
				}

				// SHALL NOT HAPPEN
				if (level1data.best_ask_volume_m==0 || level1data.best_bid_volume_m ==0 || level1data.best_ask_price_m == 0 || level1data.best_bid_price_m == 0)
				{
					LOG_SYMBOL_CTX( internalRun_name_m, providerForRTSymbols_m, SLogger::SL_ERROR, level1data.getSymbol(), 
						"Level 1 data do not have all good fields (SHALL NOT HAPPEN HERE)",
						"level1data1=" << level1data.toString()
					)

					return;
				}

				// found
				lastBid = it->second.best_bid_price_m;
				lastAsk = it->second.best_ask_price_m;

			}

			// check for risk threashold
			if (risk > RISK_MIN_m) {
				TradingEvent te(TradingEvent::TE_CalculatedRiskMoreThreashold, level1data.getSymbol2(), level1data.getTime() , true);
							
				elist.push_back(te);

				LOG_SYMBOL_CTX(internalRun_name_m,  providerForRTSymbols_m, SLogger::SL_INFO, level1data.getSymbol(), 
					"New event: " << te.toString(),
					" risk=" << CppUtils::float2String(risk, -1, 4) << endl << 
					" current bid=" << CppUtils::float2String(level1data.best_bid_price_m, -1, 4) << endl << 
					" current ask=" << CppUtils::float2String(level1data.best_ask_price_m, -1, 4)
				)

			}
			else {
				TradingEvent te(TradingEvent::TE_NotCalculatedRiskMoreThreashold, level1data.getSymbol2(), level1data.getTime() , true);
							
				elist.push_back(te);

				LOG_SYMBOL_CTX(internalRun_name_m,  providerForRTSymbols_m, SLogger::SL_INFO, level1data.getSymbol(), 
					"New event: " << te.toString(),
					" risk=" << CppUtils::float2String(risk, -1, 4) << endl << 
					" current bid=" << CppUtils::float2String(level1data.best_bid_price_m, -1, 4) << endl << 
					" current ask=" << CppUtils::float2String(level1data.best_ask_price_m, -1, 4)
				)
			}


						
			// check for events
			double factor_long = level1data.best_bid_volume_m/level1data.best_ask_volume_m;

			if (factor_long < SBA_m) {
				TradingEvent te(TradingEvent::TE_SizeBidSizeAsk_Long, level1data.getSymbol2(), level1data.getTime() , true);
							
				elist.push_back(te);

				LOG_SYMBOL_CTX(internalRun_name_m,  providerForRTSymbols_m, SLogger::SL_INFO, level1data.getSymbol(), 
					"New event: " << te.toString(),
					" SBA_m=" << CppUtils::float2String(SBA_m, -1, 4) << endl << 
					" factor_long=" << CppUtils::float2String(factor_long, -1, 4) << endl << 
					" level1data1=" << level1data.toString()
				)
			}
			else {
				TradingEvent te(TradingEvent::TE_NotSizeBidSizeAsk_Long,  level1data.getSymbol2(), level1data.getTime(), true);
			
				elist.push_back(te);

				LOG_SYMBOL_CTX( internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, level1data.getSymbol(), 
					"New event: " << te.toString(),
					" SBA_m=" << CppUtils::float2String(SBA_m, -1, 4) << endl << 
					" factor_long=" << CppUtils::float2String(factor_long, -1, 4) << endl << 
					" level1data1=" << level1data.toString()
				)

			}

			// short
			// check for events
			double factor_short = level1data.best_ask_volume_m/level1data.best_bid_volume_m;

			if (factor_short < SBA_m) {
				TradingEvent te(TradingEvent::TE_SizeBidSizeAsk_Short, level1data.getSymbol2(), level1data.getTime() , true);
							
				elist.push_back(te);

				LOG_SYMBOL_CTX( internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, level1data.getSymbol(), 
					"New event: " << te.toString(),
					" SBA_m=" << CppUtils::float2String(SBA_m, -1, 4) << endl << 
					" factor_short=" << CppUtils::float2String(factor_short, -1, 4) << endl << 
					" level1data1=" << level1data.toString()
				)
			}
			else {
				TradingEvent te(TradingEvent::TE_NotSizeBidSizeAsk_Short,  level1data.getSymbol2(), level1data.getTime(), true);
			
				elist.push_back(te);

				LOG_SYMBOL_CTX( internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, level1data.getSymbol(), 
					"New event: " << te.toString(),
					" SBA_m=" << CppUtils::float2String(SBA_m, -1, 4) << endl << 
					" factor_short=" << CppUtils::float2String(factor_short, -1, 4) << endl << 
					" level1data1=" << level1data.toString()
				)

			}

			//
			
			double const& dayOpen = symbolHelperData_m[level1data.getSymbol2()].thisDayOpen_m;
			double const& dayClose = symbolHelperData_m[level1data.getSymbol2()].lastDayClose_m;

			if (lastBid  > 0 && /*dayOpen > 0 &&*/ dayClose > 0) {
				
				if (level1data.best_bid_price_m > lastBid && /*level1data.best_bid_price_m > dayOpen &&*/ level1data.best_bid_price_m > dayClose) {
					TradingEvent te(TradingEvent::TE_PriceIsRising,  level1data.getSymbol2(), level1data.getTime(), true);
				
					elist.push_back(te);

					LOG_SYMBOL_CTX(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO,  level1data.getSymbol(), 
						"New event: " << te.toString(),
						" lastbid=" << CppUtils::float2String(lastBid, -1, 4) << endl << 
						" dayClose=" << CppUtils::float2String(dayClose, -1, 4) << endl << 
						" level1data1=" << level1data.toString()
					);

				}
				else {
					TradingEvent te(TradingEvent::TE_NotPriceIsRising,  level1data.getSymbol2(), level1data.getTime(), true);
			
					elist.push_back(te);

					LOG_SYMBOL_CTX( internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO,level1data.getSymbol(), 
						"New event: " << te.toString(),
						" lastbid=" << CppUtils::float2String(lastBid, -1, 4) << endl << 
						" dayClose=" << CppUtils::float2String(dayClose, -1, 4) << endl << 
						" level1data1=" << level1data.toString()
					);

				}
				
			}

			if (lastAsk  > 0 && /*dayOpen > 0 &&*/ dayClose > 0) {
				
				if (level1data.best_ask_price_m < lastAsk && /*level1data.best_ask_price_m < dayOpen &&*/ level1data.best_ask_price_m < dayClose) {

					TradingEvent te(TradingEvent::TE_PriceIsFalling,  level1data.getSymbol2(), level1data.getTime(), true);
				
					elist.push_back(te);

					LOG_SYMBOL_CTX( internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO,level1data.getSymbol(), 
						"New event: " << te.toString(),
						" lastAsk=" << CppUtils::float2String(lastAsk, -1, 4) << endl << 
						" dayClose=" << CppUtils::float2String(dayClose, -1, 4) << endl << 
						" level1data1=" << level1data.toString()
					);
				}
				else {
					TradingEvent te(TradingEvent::TE_NotPriceIsFalling,  level1data.getSymbol2(), level1data.getTime(), true);
				
					elist.push_back(te);

					LOG_SYMBOL_CTX(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, level1data.getSymbol(), 
						"New event: " << te.toString(),
						" lastAsk=" << CppUtils::float2String(lastAsk, -1, 4) << endl << 
						" dayClose=" << CppUtils::float2String(dayClose, -1, 4) << endl << 
						" level1data1=" << level1data.toString()
					);
				}
				
			}

			// for exit events
			BrokerContext ctx = getBrokerContext(level1data.getSymbol2());

			double const& opBuy  = ctx.opBuyLimit_m;
			double const& opSell  = ctx.opSellLimit_m;
			


			// exit from buy
			if (opBuy > 0) {
				//  (OP + (risk*PR)) < Bid
				if (lastBid > ( opBuy + risk*PR_m ) ) {

					// pos is opened and at least timeout elapsed
					if (ctx.posOpenTime_m > 0 && (CppUtils::getTime()-ctx.posOpenTime_m) >= TIMEOUT_AFTER_OPEN_m ) {
					
						TradingEvent te(TradingEvent::TE_Exit1FromLong,  level1data.getSymbol2(), level1data.getTime(), true);
						
						elist.push_back(te);

						LOG_SYMBOL_CTX(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO,  level1data.getSymbol(), 
							"New event: " << te.toString(),
							" opBuy=" << CppUtils::float2String(opBuy, -1, 4) << endl << 
							" lastBid=" << CppUtils::float2String(lastBid, -1, 4) << endl << 
							" risk=" << CppUtils::float2String(risk, -1, 4) << endl << 
							" PR_m=" << CppUtils::float2String(PR_m, -1, 4) << endl << 
							" level1data1=" << level1data.toString()
						);
					} 
					else {
						LOG_SYMBOL_CTX(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO,  level1data.getSymbol(), 
							"TE_Exit1FromLong cannot be raised as timeout still have not elapsed or position is not opened",
							" open time (empty means no open done) =" << CppUtils::getGmtTime(ctx.posOpenTime_m) << endl << 
							" TIMEOUT_AFTER_OPEN_m=" << TIMEOUT_AFTER_OPEN_m
						);
					}
				}

				// (OP + risk) < lastBid
				if (lastBid > (opBuy + risk)) {
					// pos is opened and at least timeout elapsed
					if (ctx.posOpenTime_m > 0 && (CppUtils::getTime()-ctx.posOpenTime_m) >= TIMEOUT_AFTER_OPEN_m ) {

						TradingEvent te(TradingEvent::TE_Exit2FromLong,  level1data.getSymbol2(), level1data.getTime(), true);
					
						elist.push_back(te);

						LOG_SYMBOL_CTX(internalRun_name_m,  providerForRTSymbols_m, SLogger::SL_INFO,level1data.getSymbol(), 
							"New event: " << te.toString(),
							" opBuy=" << CppUtils::float2String(opBuy, -1, 4) << endl << 
							" lastAsk=" << CppUtils::float2String(lastAsk, -1, 4) << endl << 
							" risk=" << CppUtils::float2String(risk, -1, 4) << endl << 
							" level1data1=" << level1data.toString()
						);
					}
					else {
						LOG_SYMBOL_CTX(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO,  level1data.getSymbol(), 
							"TE_Exit2FromLong cannot be raised as timeout still have not elapsed or position is not opened",
							" open time (empty means no open done) =" << CppUtils::getGmtTime(ctx.posOpenTime_m) <<  endl << 
							" TIMEOUT_AFTER_OPEN_m=" << TIMEOUT_AFTER_OPEN_m
						);
					}
				}

				// cancel event
				if ( lastBid <= (opBuy + risk*PR_m)) {

					if (ctx.posInstallTime_m > 0 && (CppUtils::getTime()-ctx.posInstallTime_m) >= TIMEOUT_AFTER_OPEN_m ) {
				
						TradingEvent te(TradingEvent::TE_CancelFromLong,  level1data.getSymbol2(), level1data.getTime(), true);
						
						elist.push_back(te);

						LOG_SYMBOL_CTX( internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO,level1data.getSymbol(), 
							"New event: " << te.toString(),
							" opBuy=" << CppUtils::float2String(opBuy, -1, 4) << endl << 
							" lastBid=" << CppUtils::float2String(lastBid, -1, 4) << endl << 
							" risk=" << CppUtils::float2String(risk, -1, 4) << endl << 
							" PR_m=" << CppUtils::float2String(PR_m, -1, 4) << endl << 
							" level1data1=" << level1data.toString()
						);

					}
					else {
						LOG_SYMBOL_CTX(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO,  level1data.getSymbol(), 
							"TE_CancelFromLong cannot be raised as timeout still have not elapsed or position is not installed",
							" install time (empty means no install done) =" << CppUtils::getGmtTime(ctx.posInstallTime_m) <<  endl << 
							" TIMEOUT_AFTER_OPEN_m=" << TIMEOUT_AFTER_OPEN_m
						);
					}


				}

				// urgent close
				if (lastBid <= (opBuy + risk)) {
					TradingEvent te(TradingEvent::TE_Exit3FromLong,  level1data.getSymbol2(), level1data.getTime(), true);
					elist.push_back(te);

					LOG_SYMBOL_CTX( internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO,level1data.getSymbol(), 
						"New event: " << te.toString(),
						" opBuy=" << CppUtils::float2String(opBuy, -1, 4) << endl << 
						" lastBid=" << CppUtils::float2String(lastBid, -1, 4)
					);
				}


			}

			// exit from sell
			if (opSell > 0) {
				//  (OP + (risk*PR)) < Bid
				if (lastAsk < ( opSell - risk*PR_m ) ) {
					if (ctx.posOpenTime_m > 0 && (CppUtils::getTime()-ctx.posOpenTime_m) >= 1.0 ) {
						TradingEvent te(TradingEvent::TE_Exit1FromShort,  level1data.getSymbol2(), level1data.getTime(), true);
				
						elist.push_back(te);

						LOG_SYMBOL_CTX( internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, level1data.getSymbol(), 
							"New event: " << te.toString(),
							" opSell=" << CppUtils::float2String(opSell, -1, 4) << endl << 
							" lastAsk=" << CppUtils::float2String(lastAsk, -1, 4) << endl << 
							" risk=" << CppUtils::float2String(risk, -1, 4) << endl << 
							" PR_m=" << CppUtils::float2String(PR_m, -1, 4) << endl << 
							" level1data1=" << level1data.toString()
						);
					}
					else {
						LOG_SYMBOL_CTX(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO,  level1data.getSymbol(), 
							"TE_Exit1FromShort cannot be raised as timeout still have not elapsed or position is not opened",
							" open time (empty means no open done) =" << CppUtils::getGmtTime(ctx.posOpenTime_m) <<  endl << 
							" TIMEOUT_AFTER_OPEN_m=" << TIMEOUT_AFTER_OPEN_m
						);
					}
				}

				// (OP + risk) < lastBid
				if (lastAsk < (opSell - risk) ) {
					// pos is opened and at least timeout elapsed
					if (ctx.posOpenTime_m > 0 && (CppUtils::getTime()-ctx.posOpenTime_m) >= 1.0 ) {

						TradingEvent te(TradingEvent::TE_Exit2FromShort,  level1data.getSymbol2(), level1data.getTime(), true);
					
						elist.push_back(te);

						LOG_SYMBOL_CTX(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, level1data.getSymbol(), 
							"New event: " << te.toString(),
							" opSell=" << CppUtils::float2String(opSell, -1, 4) << endl << 
							" lastBid=" << CppUtils::float2String(lastBid, -1, 4) << endl << 
							" risk=" << CppUtils::float2String(risk, -1, 4) << endl << 
							" level1data1=" << level1data.toString()
						);

					}
					else {
						LOG_SYMBOL_CTX(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO,  level1data.getSymbol(), 
							"TE_Exit2FromShort cannot be raised as timeout still have not elapsed or position is not opened",
							" open time (empty means no open done) =" << CppUtils::getGmtTime(ctx.posOpenTime_m) <<  endl << 
							" TIMEOUT_AFTER_OPEN_m=" << TIMEOUT_AFTER_OPEN_m
						);
					}
				}

				// cancel event
				if ( lastAsk  >= (opSell- risk*PR_m) ) {

					if (ctx.posInstallTime_m > 0 && (CppUtils::getTime()-ctx.posInstallTime_m) >= TIMEOUT_AFTER_OPEN_m ) {
						TradingEvent te(TradingEvent::TE_CancelFromShort,  level1data.getSymbol2(), level1data.getTime(), true);
						elist.push_back(te);

						LOG_SYMBOL_CTX(internalRun_name_m,  providerForRTSymbols_m, SLogger::SL_INFO,level1data.getSymbol(), 
							"New event: " << te.toString(),
							" opSell=" << CppUtils::float2String(opSell, -1, 4) << endl << 
							" lastAsk=" << CppUtils::float2String(lastAsk, -1, 4) << endl << 
							" risk=" << CppUtils::float2String(risk, -1, 4) << endl << 
							" PR_m=" << CppUtils::float2String(PR_m, -1, 4) << endl << 
							" level1data1=" << level1data.toString()
						);  

					} else {
						LOG_SYMBOL_CTX(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO,  level1data.getSymbol(), 
							"TE_CancelFromShort cannot be raised as timeout still have not elapsed or position is not installed",
							" install time (empty means no install done) =" << CppUtils::getGmtTime(ctx.posInstallTime_m) <<  endl << 
							" TIMEOUT_AFTER_OPEN_m=" << TIMEOUT_AFTER_OPEN_m
						);
					}

				}

				// urgent close
				if (lastAsk >= (opSell-risk) ) {
					TradingEvent te(TradingEvent::TE_Exit3FromShort,  level1data.getSymbol2(), level1data.getTime(), true);
					elist.push_back(te);

					LOG_SYMBOL_CTX( internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO,level1data.getSymbol(), 
						"New event: " << te.toString(),
						" opSell=" << CppUtils::float2String(opSell, -1, 4) << endl << 
						" lastAsk=" << CppUtils::float2String(lastAsk, -1, 4)
					);
				}

			}



			// end check if it ours
		}
	}



}


void MGerchik::generateNewEvent(BrkLib::BrokerConnect& brconnect, TradingEventList& elist, HlpStruct::RtData const& rtdata)
{
	
	

	// check if indexes are rising
	
	// check our index list
	// indexList_rt_m
	// and provider
	// providerForRTIndexes_m

	if (providerForRTIndexes_m == rtdata.getProvider()) {
		if (indexList_rt_m.find(rtdata.getSymbol2()) != indexList_rt_m.end()) {

			double lastBid = -1;
			double lastAsk = -1;

			{ 
				LOCK_FOR_SCOPE(priceRTDataMtx_m);
				// check last beid and ask prices - must exist
				map<CppUtils::String, HlpStruct::RtData >::const_iterator it = rtData_m.find(rtdata.getSymbol2());
				if (it==rtData_m.end()) {

					ALERTEVENTNC("",BrkLib::AP_HIGH,ALG_M_GERCHIK, "It is error - the past index RT data must already exist: " << rtdata.getSymbol2() );
					LOG_COMMON(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_ERROR,"It is error - the past index RT data must already exist: " << rtdata.getSymbol2());
					return;
				}

		

				// found
				lastBid = it->second.getRtData().bid_m;
				lastAsk = it->second.getRtData().ask_m;
			
			}
			
			CppUtils::String symbol_as_history = convertRtIndexNameToPrecalcHistoryName(rtdata.getSymbol2());

			// enforce check
			if (symbol_as_history.size() <=0) {
				ALERTEVENTNC("",BrkLib::AP_HIGH,ALG_M_GERCHIK, "It is error - the index RT data must be presented as history symbol: " << rtdata.getSymbol2() );
				LOG_COMMON(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_ERROR, "It is error - the index RT data must be presented as history symbol: " << rtdata.getSymbol2());
				return;
			}


			map<CppUtils::String, SymbolHelperHolder>::const_iterator ithlp = symbolHelperData_m.find(symbol_as_history);
			if (ithlp==symbolHelperData_m.end()) {
				ALERTEVENTNC("",BrkLib::AP_HIGH,ALG_M_GERCHIK, "It is error - Can't find the RT index in history helper list: " << symbol_as_history );
				LOG_COMMON(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_ERROR, "It is error - Can't find the RT index in history helper list: " << symbol_as_history);
				return;
			}

			
			double const& dayOpen = ithlp->second.thisDayOpen_m;
			double const& dayClose = ithlp->second.lastDayClose_m;

			if (lastBid  > 0 && /*dayOpen > 0 &&*/ dayClose > 0) {
				
				// event with symbols as history
				if (rtdata.getRtData().bid_m > lastBid && /*rtdata.getRtData().bid_m > dayOpen &&*/ rtdata.getRtData().bid_m > dayClose) {
					TradingEvent te(TradingEvent::TE_IndexIsRising,  rtdata.getSymbol2(), rtdata.getTime(), false );

					// list of target symbols
					CppUtils::assign<CppUtils::String>(te.targetSymbols_m, symbolLevel1_rt_m );


					elist.push_back(te);

					LOG_SYMBOL_CTX( internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, rtdata.getSymbol2(), 
						"New event: " << te.toString(),
						" lastBid=" << CppUtils::float2String(lastBid, -1, 4) << endl << 
						" dayClose=" << CppUtils::float2String(dayClose, -1, 4) << endl << 
						" rtdata=" << rtdata.toString()
					);
				}
				else {
					TradingEvent te(TradingEvent::TE_NotIndexIsRising,  rtdata.getSymbol2(), rtdata.getTime(), false);
					// list of target symbols
					CppUtils::assign<CppUtils::String>(te.targetSymbols_m, symbolLevel1_rt_m );


					elist.push_back(te);

					LOG_SYMBOL_CTX( internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, rtdata.getSymbol2(), 
						"New event: " << te.toString(),
						" lastBid=" << CppUtils::float2String(lastBid, -1, 4) << endl << 
						" dayClose=" << CppUtils::float2String(dayClose, -1, 4) << endl << 
						" rtdata=" << rtdata.toString()
					);

				}
				
			}

			if (lastAsk  > 0 && /*dayOpen > 0 &&*/ dayClose > 0) {
				
				if (rtdata.getRtData().ask_m < lastAsk && /*rtdata.getRtData().ask_m < dayOpen &&*/ rtdata.getRtData().ask_m < dayClose) {

					TradingEvent te(TradingEvent::TE_IndexIsFalling,  rtdata.getSymbol2(), rtdata.getTime(), false);
					// list of target symbols
					CppUtils::assign<CppUtils::String>(te.targetSymbols_m, symbolLevel1_rt_m );


					elist.push_back(te);

					LOG_SYMBOL_CTX(internalRun_name_m,  providerForRTSymbols_m, SLogger::SL_INFO, rtdata.getSymbol2(), 
						"New event: " << te.toString(),
						" lastAsk=" << CppUtils::float2String(lastAsk, -1, 4) << endl << 
						" dayClose=" << CppUtils::float2String(dayClose, -1, 4) << endl << 
						" rtdata=" << rtdata.toString()
					);
				}
				else {
					TradingEvent te(TradingEvent::TE_NotIndexIsFalling,  rtdata.getSymbol2(), rtdata.getTime(), false);
					// list of target symbols
					CppUtils::assign<CppUtils::String>(te.targetSymbols_m, symbolLevel1_rt_m );


					elist.push_back(te);

					LOG_SYMBOL_CTX(internalRun_name_m,  providerForRTSymbols_m, SLogger::SL_INFO, rtdata.getSymbol2(), 
						"New event: " << te.toString(),
						" lastAsk=" << CppUtils::float2String(lastAsk, -1, 4) << endl << 
						" dayClose=" << CppUtils::float2String(dayClose, -1, 4) << endl << 
						" rtdata=" << rtdata.toString()
					);

				}
				
			}
					
			// 
		}
	}




}

void MGerchik::machineEventsReaction(BrkLib::BrokerConnect& brconnect, TradingEventList const& elist)
{
	LOCK_FOR_SCOPE(jobMutex_m);
	
	for(int i = 0; i < elist.size(); i++) {
		TradingEvent const& event_i = elist[i];

		// target symbols
		for(int k = 0; k < event_i.targetSymbols_m.size(); k++) {
			CppUtils::String const& target_i = event_i.targetSymbols_m[k];

			// create new state
			TradingState & oldState = state_m[target_i];
			TradingState newState = oldState;
	
			// store symbol
			newState.symbol_m = target_i;
		
			
			// initial state
			OLD_STATE_BEGIN(oldState.state_m)
				
				OLD_STATE_ENTRY_BEGIN(TradingState::TS_Initial)
				
				NEW_EVENT_BEGIN(event_i.event_m)
				
				// old state - state_m
				NEW_EVENT_ENTRY_BEGIN(TradingEvent::TE_SizeBidSizeAsk_Long) 
					
					newState.ch_SizeBidSizeAsk_On_Long_m = true;
				
				NEW_EVENT_ENTRY_END()
				NEW_EVENT_ENTRY_BEGIN(TradingEvent::TE_NotSizeBidSizeAsk_Long) 
				
					newState.ch_SizeBidSizeAsk_On_Long_m = false;
				
				NEW_EVENT_ENTRY_END()

				NEW_EVENT_ENTRY_BEGIN(TradingEvent::TE_SizeBidSizeAsk_Short) 
					
					newState.ch_SizeBidSizeAsk_On_Short_m = true;
				
				NEW_EVENT_ENTRY_END()
				NEW_EVENT_ENTRY_BEGIN(TradingEvent::TE_NotSizeBidSizeAsk_Short) 
				
					newState.ch_SizeBidSizeAsk_On_Short_m = false;
				
				NEW_EVENT_ENTRY_END()

				//
				NEW_EVENT_ENTRY_BEGIN(TradingEvent::TE_CalculatedRiskMoreThreashold) 
					
					newState.ch_Risk_More_Threashold_m = true;
				
				NEW_EVENT_ENTRY_END()
				NEW_EVENT_ENTRY_BEGIN(TradingEvent::TE_NotCalculatedRiskMoreThreashold) 
				
					newState.ch_Risk_More_Threashold_m = false;
				
				NEW_EVENT_ENTRY_END()



				NEW_EVENT_ENTRY_BEGIN(TradingEvent::TE_IndexIsFalling) 
									
					newState.indexesFalling_On_m[event_i.symbol_m] = true;
				NEW_EVENT_ENTRY_END()
				NEW_EVENT_ENTRY_BEGIN(TradingEvent::TE_NotIndexIsFalling) 

					
					newState.indexesFalling_On_m[event_i.symbol_m] = false;
				NEW_EVENT_ENTRY_END()
				NEW_EVENT_ENTRY_BEGIN(TradingEvent::TE_IndexIsRising) 
					

					newState.indexesRising_On_m[event_i.symbol_m] = true;
				NEW_EVENT_ENTRY_END()
				NEW_EVENT_ENTRY_BEGIN(TradingEvent::TE_NotIndexIsRising) 
					

					newState.indexesRising_On_m[event_i.symbol_m] = false;
				NEW_EVENT_ENTRY_END()
				NEW_EVENT_ENTRY_BEGIN(TradingEvent::TE_PriceIsFalling)
					newState.ch_PriceIsFalling_On_m = true;
				NEW_EVENT_ENTRY_END()
				NEW_EVENT_ENTRY_BEGIN(TradingEvent::TE_NotPriceIsFalling) 
					newState.ch_PriceIsFalling_On_m = false;
				NEW_EVENT_ENTRY_END()
				NEW_EVENT_ENTRY_BEGIN(TradingEvent::TE_PriceIsRising)
					newState.ch_PriceIsRising_On_m = true;
				NEW_EVENT_ENTRY_END()
				NEW_EVENT_ENTRY_BEGIN(TradingEvent::TE_NotPriceIsRising) 
					newState.ch_PriceIsRising_On_m = false;
				NEW_EVENT_ENTRY_END()

				NEW_EVENT_END()


				// check if state changed
				if (newState.ch_SizeBidSizeAsk_On_Long_m && newState.ch_PriceIsRising_On_m && newState.ch_Risk_More_Threashold_m) {

					bool all_fired = true;
					for(CppUtils::StringSet::const_iterator  it = indexList_rt_m.begin(); it != indexList_rt_m.end(); it++ ) {
						map<CppUtils::String, bool>::const_iterator it_bool = newState.indexesRising_On_m.find(*it);

						if (it_bool!= newState.indexesRising_On_m.end()) {
							if (!it_bool->second) {
								all_fired = false;
								break;
							}
						}
						else {
							all_fired = false;
							break;
						}
					}
					if (all_fired) {
						// check if not singleton
						if (!SINGLETON_m || getSecuritiesNumberInProgress() <= 0) {
							newState.state_m = TradingState::TS_Ready_Long;
						}
						else {
							LOG_COMMON( internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, "Will not open new LONG pos because SINGLETON=true - " << target_i );
						}
					}
				}

				if (newState.ch_SizeBidSizeAsk_On_Short_m && newState.ch_PriceIsFalling_On_m && newState.ch_Risk_More_Threashold_m ) {

		
					bool all_fired = true;
					for(CppUtils::StringSet::const_iterator  it = indexList_rt_m.begin(); it != indexList_rt_m.end(); it++ ) {
						map<CppUtils::String, bool>::const_iterator it_bool = newState.indexesFalling_On_m.find(*it);

						if (it_bool!= newState.indexesFalling_On_m.end()) {
							if (!it_bool->second) {
								all_fired = false;
								break;
							}
						}
						else {
							all_fired = false;
							break;
						}
					}

					if (all_fired) {
						if (!LONG_ONLY_m) {
							if (!SINGLETON_m || getSecuritiesNumberInProgress() <= 0) {
								newState.state_m = TradingState::TS_Ready_Short;
							}
							else {
								LOG_COMMON( internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, "Will not open new SHORT pos because SINGLETON=true (busy with others) - " << target_i );
							}
						}
						else {
							LOG_COMMON( internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, "Will not open new SHORT pos because LONG_ONLY_m=true - " << target_i );
						}
						
					}
				}

				 

			OLD_STATE_ENTRY_END()
			//
			OLD_STATE_ENTRY_BEGIN(TradingState::TS_Ready_Long) 
				NEW_EVENT_BEGIN(event_i.event_m)
							
					NEW_EVENT_ENTRY_BEGIN(TradingEvent::TE_BrokerConfirmPendingLong) 
						newState.state_m = TradingState::TS_Pending_Long;
					NEW_EVENT_ENTRY_END()

					NEW_EVENT_ENTRY_BEGIN(TradingEvent::TE_BrokerRejectTradeLong) 
						newState.state_m = TradingState::TS_Initial;
					NEW_EVENT_ENTRY_END()


				NEW_EVENT_END()

			OLD_STATE_ENTRY_END()
			OLD_STATE_ENTRY_BEGIN(TradingState::TS_Ready_Short)
				
				NEW_EVENT_BEGIN(event_i.event_m)
					NEW_EVENT_ENTRY_BEGIN(TradingEvent::TE_BrokerConfirmPendingShort) 
						newState.state_m = TradingState::TS_Pending_Short;
					NEW_EVENT_ENTRY_END()

					NEW_EVENT_ENTRY_BEGIN(TradingEvent::TE_BrokerRejectTradeShort) 
						newState.state_m = TradingState::TS_Initial;
					NEW_EVENT_ENTRY_END()
				NEW_EVENT_END()

			OLD_STATE_ENTRY_END()
			//
			OLD_STATE_ENTRY_BEGIN(TradingState::TS_Pending_Long) 
				
				NEW_EVENT_BEGIN(event_i.event_m)
					NEW_EVENT_ENTRY_BEGIN(TradingEvent::TE_BrokerConfirmTradeLong) 
						newState.state_m = TradingState::TS_Open_Long;
					NEW_EVENT_ENTRY_END()

					NEW_EVENT_ENTRY_BEGIN(TradingEvent::TE_BrokerRejectTradeLong) 
						newState.state_m = TradingState::TS_Initial;
					NEW_EVENT_ENTRY_END()

					NEW_EVENT_ENTRY_BEGIN(TradingEvent::TE_CancelFromLong) 
						newState.state_m = TradingState::TS_Ready_Cancel_Long;
					NEW_EVENT_ENTRY_END()

				

				NEW_EVENT_END()

			OLD_STATE_ENTRY_END()

			//
			OLD_STATE_ENTRY_BEGIN(TradingState::TS_Pending_Short) 
				NEW_EVENT_BEGIN(event_i.event_m)
					NEW_EVENT_ENTRY_BEGIN(TradingEvent::TE_BrokerConfirmTradeShort) 
						newState.state_m = TradingState::TS_Open_Short;
					NEW_EVENT_ENTRY_END()

					NEW_EVENT_ENTRY_BEGIN(TradingEvent::TE_BrokerRejectTradeShort) 
						newState.state_m = TradingState::TS_Initial;
					NEW_EVENT_ENTRY_END()

					NEW_EVENT_ENTRY_BEGIN(TradingEvent::TE_CancelFromShort) 
						newState.state_m = TradingState::TS_Ready_Cancel_Short;
					NEW_EVENT_ENTRY_END()

					
				NEW_EVENT_END()

			OLD_STATE_ENTRY_END()
			
			//
			OLD_STATE_ENTRY_BEGIN(TradingState::TS_Ready_Cancel_Long) 
				NEW_EVENT_BEGIN(event_i.event_m)
					
					NEW_EVENT_ENTRY_BEGIN(TradingEvent::TE_BrokerConfirmCancelLong) 
						newState.state_m = TradingState::TS_Initial;
					NEW_EVENT_ENTRY_END()

					NEW_EVENT_ENTRY_BEGIN(TradingEvent::TE_BrokerRejectCancelLong) 
						newState.state_m = TradingState::TS_Inconsistent;
					NEW_EVENT_ENTRY_END()

				NEW_EVENT_END()

			OLD_STATE_ENTRY_END()

			OLD_STATE_ENTRY_BEGIN(TradingState::TS_Ready_Cancel_Short) 
				NEW_EVENT_BEGIN(event_i.event_m)
					
					NEW_EVENT_ENTRY_BEGIN(TradingEvent::TE_BrokerConfirmCancelShort) 
						newState.state_m = TradingState::TS_Initial;
					NEW_EVENT_ENTRY_END()

					NEW_EVENT_ENTRY_BEGIN(TradingEvent::TE_BrokerRejectCancelShort) 
						newState.state_m = TradingState::TS_Inconsistent;
					NEW_EVENT_ENTRY_END()

				NEW_EVENT_END()
			OLD_STATE_ENTRY_END()

			//
			
			OLD_STATE_ENTRY_BEGIN(TradingState::TS_Open_Long) 
				NEW_EVENT_BEGIN(event_i.event_m)
					
					NEW_EVENT_ENTRY_BEGIN(TradingEvent::TE_Exit1FromLong) 
						newState.state_m = TradingState::TS_Ready_Close_2_Long;
					NEW_EVENT_ENTRY_END()

					NEW_EVENT_ENTRY_BEGIN(TradingEvent::TE_Exit3FromLong) 
						newState.state_m = TradingState::TS_Ready_Close_3_Long;
					NEW_EVENT_ENTRY_END()

				NEW_EVENT_END()

			OLD_STATE_ENTRY_END()

			OLD_STATE_ENTRY_BEGIN(TradingState::TS_Open_Short) 
				NEW_EVENT_BEGIN(event_i.event_m)
					
					NEW_EVENT_ENTRY_BEGIN(TradingEvent::TE_Exit1FromShort) 
						newState.state_m = TradingState::TS_Ready_Close_2_Short;
					NEW_EVENT_ENTRY_END()

					NEW_EVENT_ENTRY_BEGIN(TradingEvent::TE_Exit3FromShort) 
						newState.state_m = TradingState::TS_Ready_Close_3_Short;
					NEW_EVENT_ENTRY_END()

				NEW_EVENT_END()

			OLD_STATE_ENTRY_END()
			

			//
			OLD_STATE_ENTRY_BEGIN(TradingState::TS_Ready_Close_2_Long) 
				NEW_EVENT_BEGIN(event_i.event_m)
					NEW_EVENT_ENTRY_BEGIN(TradingEvent::TE_BrokerConfirmClose2Long)
							newState.state_m = TradingState::TS_Initial;
					NEW_EVENT_ENTRY_END()

					NEW_EVENT_ENTRY_BEGIN(TradingEvent::TE_BrokerRejectClose2Long)
						newState.state_m = TradingState::TS_Ready_Close_3_Long;
					NEW_EVENT_ENTRY_END()

				NEW_EVENT_END()
				
			OLD_STATE_ENTRY_END()

			OLD_STATE_ENTRY_BEGIN(TradingState::TS_Ready_Close_2_Short)
				NEW_EVENT_BEGIN(event_i.event_m)
					NEW_EVENT_ENTRY_BEGIN(TradingEvent::TE_BrokerConfirmClose2Short)
							newState.state_m = TradingState::TS_Initial;
					NEW_EVENT_ENTRY_END()
					NEW_EVENT_ENTRY_BEGIN(TradingEvent::TE_BrokerRejectClose2Short)
							newState.state_m = TradingState::TS_Ready_Close_3_Short;
					NEW_EVENT_ENTRY_END()
				NEW_EVENT_END()

			OLD_STATE_ENTRY_END()
			//
			
			OLD_STATE_ENTRY_BEGIN(TradingState::TS_Ready_Close_3_Long) 
				NEW_EVENT_BEGIN(event_i.event_m)
					NEW_EVENT_ENTRY_BEGIN(TradingEvent::TE_BrokerConfirmClose3Long)
							newState.state_m = TradingState::TS_Initial;
					NEW_EVENT_ENTRY_END()

					NEW_EVENT_ENTRY_BEGIN(TradingEvent::TE_BrokerRejectClose3Long)
						newState.state_m = TradingState::TS_Inconsistent;
					NEW_EVENT_ENTRY_END()

				NEW_EVENT_END()
				
			OLD_STATE_ENTRY_END()

			OLD_STATE_ENTRY_BEGIN(TradingState::TS_Ready_Close_3_Short)
				NEW_EVENT_BEGIN(event_i.event_m)
					NEW_EVENT_ENTRY_BEGIN(TradingEvent::TE_BrokerConfirmClose3Short)
							newState.state_m = TradingState::TS_Initial;
					NEW_EVENT_ENTRY_END()
					NEW_EVENT_ENTRY_BEGIN(TradingEvent::TE_BrokerRejectClose3Short)
							newState.state_m = TradingState::TS_Inconsistent;
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

				onStateChanged(brconnect, oldState, newState, event_i);
				// state changed
			}
		
			// store  
			oldState = newState;


		} // end target symbols
	} // end events

		
}


void MGerchik::onStateChanged(BrkLib::BrokerConnect& brconnect, TradingState const& oldState, TradingState& newState, TradingEvent const& cause_event)
{
	// reaction if satte changed
	// cause event - who caused this change
	
	//double const& lastAsk = level1Data_m[newState.symbol_m].best_ask_price_m;
	//double const& lastBid = level1Data_m[newState.symbol_m].best_bid_price_m;

	double curAsk = -1;
	double curBid = -1;

	{
		
		LOCK_FOR_SCOPE(currentLevel1DataMtx_m);
		curAsk = currentLevel1Data_m[newState.symbol_m].best_ask_price_m;
		curBid = currentLevel1Data_m[newState.symbol_m].best_bid_price_m;
	}

	
	if (curBid <= 0 || curAsk <= 0) {
		LOG_COMMON(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_DEBUG, "For symbol: " << newState.symbol_m << " cannot find CURRENT bid or ask prices, returning...");
		return;
	}

	double risk = (curAsk - curBid)+0.02;


	// handler for all ols states
	OLD_STATE_ENTRY_ALL()

		NEW_STATE_BEGIN(newState.state_m)
			NEW_STATE_ENTRY_BEGIN(TradingState::TS_Initial)

				// before clearing context some logging
				BrokerContext context = getBrokerContext(newState.symbol_m);

				double profit = -1;
				if (context.brokerClosePrice_m > 0 && context.brokerOpenPrice_m >0) {
					if (context.isLong_m) {
						profit = context.brokerClosePrice_m - context.brokerOpenPrice_m;
					}
					else {
						profit = context.brokerOpenPrice_m - context.brokerClosePrice_m;
					}
				}

				if (context.brokerOpenPrice_m >0) {
					LOG_COMMON(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, "TRADE RESULT: " << newState.symbol_m << 
						( context.isLong_m ? " LONG " : " SHORT " ) <<
						" PROFIT=" << CppUtils::float2String(profit, -1, 4) << 
						" open price=" << CppUtils::float2String(context.brokerOpenPrice_m, -1, 4) << 
						" close price=" << CppUtils::float2String(context.brokerClosePrice_m, -1, 4) << 
						" install price=" << CppUtils::float2String(context.brokerInstallPrice_m, -1, 4) 
					);
				}
				else {
					LOG_COMMON(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, "NO DEAL FOR: " << newState.symbol_m << " WAS CANCELLED (REJECTED)" );
				}
				
				// clear context
				Action action(Action::AS_ClearBrokerContext, newState.symbol_m);
				doAction(brconnect, action);
			NEW_STATE_ENTRY_END();
		NEW_STATE_END();
	OLD_STATE_ENTRY_ALL_END()
	//

	OLD_STATE_BEGIN(oldState.state_m)
		OLD_STATE_ENTRY_BEGIN(TradingState::TS_Initial)

				
			
		NEW_STATE_BEGIN(newState.state_m)
		NEW_STATE_ENTRY_BEGIN(TradingState::TS_Ready_Short)
		
			Action action(Action::AS_IssueOpenShort, newState.symbol_m);
			action.opPrice_m = curBid + (risk* PR_m - SLIDE_m);
			//action.opPrice_m = curAsk + (risk* PR_m - SLIDE_m);
			action.volume_m = SHARES_m;

			//ALERTEVENTNC("",BrkLib::AP_HIGH,ALG_M_GERCHIK, "SHORT SIGNAL FOR " << newState.symbol_m );
			LOG_COMMON(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, "OPEN SHORT SIGNAL " << ((action.opPrice_m <= curAsk && action.opPrice_m >= curBid) ? "INSIDE SPREAD" : "OUTSIDE SPREAD") <<
				" FOR " << action.symbol_m << 
				" open price( curBid + (risk* PR_m - SLIDE_m) )=" << action.opPrice_m <<
				" curAsk=" << curAsk <<
				" curBid=" << curBid <<
				" risk=" << risk <<
				" SLIDE_m=" << SLIDE_m <<
				" caused event id=" << cause_event.uid_m.toString()
				
				);

 
			doAction(brconnect, action);

			// need to set thsi when order is pending
			//brokerContext_m[newState.symbol_m].opSellLimit_m = action.opSellLimit_m;

			
		NEW_STATE_ENTRY_END()
		NEW_STATE_ENTRY_BEGIN(TradingState::TS_Ready_Long)

			Action action(Action::AS_IssueOpenLong, newState.symbol_m);
			action.opPrice_m = curAsk - (risk* PR_m + SLIDE_m);
			//action.opPrice_m = curBid - (risk* PR_m + SLIDE_m);
			action.volume_m = SHARES_m;

			//ALERTEVENTNC("",BrkLib::AP_HIGH,ALG_M_GERCHIK, "LONG SIGNAL FOR " << newState.symbol_m );
			
			LOG_COMMON(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, "OPEN LONG SIGNAL " << ((action.opPrice_m <= curAsk && action.opPrice_m >= curBid) ? "INSIDE SPREAD" : "OUTSIDE SPREAD") << 
				" FOR " << action.symbol_m << 
				" open price( curAsk - (risk* PR_m + SLIDE_m) )=" << action.opPrice_m <<
				" curBid=" << curBid <<
				" curAsk=" << curAsk <<
				" risk=" << risk <<
				" SLIDE_m=" << SLIDE_m <<
				" caused event id=" << cause_event.uid_m.toString()
				);

			doAction(brconnect, action);

			// need to set this when order is pending
			//brokerContext_m[newState.symbol_m].opBuyLimit_m = action.opBuyLimit_m;
		
			
		

		NEW_STATE_ENTRY_END();
		NEW_STATE_END();

	OLD_STATE_ENTRY_END()
	

	
	OLD_STATE_ENTRY_BEGIN(TradingState::TS_Open_Long)
		NEW_STATE_BEGIN(newState.state_m)
			
			NEW_STATE_ENTRY_BEGIN(TradingState::TS_Ready_Close_2_Long)
				
				// close
				Action action(Action::AS_IssueClose2Long, newState.symbol_m);
				action.opPrice_m = curAsk - 0.01;

				//ALERTEVENTNC("",BrkLib::AP_HIGH,ALG_M_GERCHIK, "CLOSE SIGNAL FOR " << newState.symbol_m );
			
				LOG_COMMON(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, "CLOSE 2 LONG SIGNAL FOR " << action.symbol_m << 
					" close price( curAsk - 0.01 )=" << action.opPrice_m <<
					" caused event id=" << cause_event.uid_m.toString()
				);

				doAction(brconnect, action);
				
			NEW_STATE_ENTRY_END();

			NEW_STATE_ENTRY_BEGIN(TradingState::TS_Ready_Close_3_Long)
				
				// close
				Action action(Action::AS_IssueClose3Long, newState.symbol_m);
				//ALERTEVENTNC("",BrkLib::AP_HIGH,ALG_M_GERCHIK, "CLOSE SIGNAL FOR " << newState.symbol_m );
			
				LOG_COMMON(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, "CLOSE 3 (URGENT) LONG SIGNAL FOR " << action.symbol_m << 
					" caused event id=" << cause_event.uid_m.toString()
				);

				doAction(brconnect, action);
				
			NEW_STATE_ENTRY_END();


		NEW_STATE_END();
	OLD_STATE_ENTRY_END()

	OLD_STATE_ENTRY_BEGIN(TradingState::TS_Open_Short)
		NEW_STATE_BEGIN(newState.state_m)
			
			

			NEW_STATE_ENTRY_BEGIN(TradingState::TS_Ready_Close_2_Short)
				
				// close
				Action action(Action::AS_IssueClose2Short, newState.symbol_m);
				action.opPrice_m = curBid + 0.01;

				//ALERTEVENTNC("",BrkLib::AP_HIGH,ALG_M_GERCHIK, "CLOSE SIGNAL FOR " << newState.symbol_m );
			
				LOG_COMMON(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, "CLOSE 2 SHORT SIGNAL FOR " << action.symbol_m << 
					" close price( curBid + 0.01 )=" << action.opPrice_m <<
					" caused event id=" << cause_event.uid_m.toString()
				);

				doAction(brconnect, action);
				
			NEW_STATE_ENTRY_END();

			NEW_STATE_ENTRY_BEGIN(TradingState::TS_Ready_Close_3_Short)
				// close
				Action action(Action::AS_IssueClose3Short, newState.symbol_m);
				

				//ALERTEVENTNC("",BrkLib::AP_HIGH,ALG_M_GERCHIK, "CLOSE SIGNAL FOR " << newState.symbol_m );
			
				LOG_COMMON(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, "CLOSE 3 (URGENT) SHORT SIGNAL FOR " << action.symbol_m << 
					" caused event id=" << cause_event.uid_m.toString()
				);

				doAction(brconnect, action);

			NEW_STATE_ENTRY_END();
		
		NEW_STATE_END();
	OLD_STATE_ENTRY_END()
	//
	

  // pending

	OLD_STATE_ENTRY_BEGIN(TradingState::TS_Pending_Long)
		NEW_STATE_BEGIN(newState.state_m)
			
			NEW_STATE_ENTRY_BEGIN(TradingState::TS_Ready_Cancel_Long)
				
				// close 2 - market
				Action action(Action::AS_IssueCancelLong, newState.symbol_m);
				
				LOG_COMMON(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, "CANCEL PENDING LONG SIGNAL FOR " << action.symbol_m << 
					" caused event id=" << cause_event.uid_m.toString()
				);

				doAction(brconnect, action);
				
			NEW_STATE_ENTRY_END();
			

		NEW_STATE_END();
	OLD_STATE_ENTRY_END()
	
	
	OLD_STATE_ENTRY_BEGIN(TradingState::TS_Pending_Short)
		NEW_STATE_BEGIN(newState.state_m)
			
			NEW_STATE_ENTRY_BEGIN(TradingState::TS_Ready_Cancel_Short)
				
				// close 2 - market
				Action action(Action::AS_IssueCancelShort, newState.symbol_m);
				
				LOG_COMMON(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, "CANCEL PENDING SHORT SIGNAL FOR " << action.symbol_m << 
					" caused event id=" << cause_event.uid_m.toString()
				);

				doAction(brconnect, action);

			NEW_STATE_ENTRY_END();
			
		NEW_STATE_END();
	OLD_STATE_ENTRY_END()

	//

	OLD_STATE_ENTRY_BEGIN(TradingState::TS_Ready_Close_2_Long)
		NEW_STATE_BEGIN(newState.state_m)

			NEW_STATE_ENTRY_BEGIN(TradingState::TS_Ready_Close_3_Long)
				
				// close
				Action action(Action::AS_IssueClose3Long, newState.symbol_m);
				//ALERTEVENTNC("",BrkLib::AP_HIGH,ALG_M_GERCHIK, "CLOSE SIGNAL FOR " << newState.symbol_m );
			
				LOG_COMMON(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, "CLOSE 3 (URGENT) LONG SIGNAL FOR " << action.symbol_m << 
					" caused event id=" << cause_event.uid_m.toString()
				);

				doAction(brconnect, action);
				
			NEW_STATE_ENTRY_END();


		NEW_STATE_END();
	OLD_STATE_ENTRY_END()

	//
	OLD_STATE_ENTRY_BEGIN(TradingState::TS_Ready_Close_2_Short)
		NEW_STATE_BEGIN(newState.state_m)

			NEW_STATE_ENTRY_BEGIN(TradingState::TS_Ready_Close_3_Short)
				// close
				Action action(Action::AS_IssueClose3Short, newState.symbol_m);
			
				//ALERTEVENTNC("",BrkLib::AP_HIGH,ALG_M_GERCHIK, "CLOSE SIGNAL FOR " << newState.symbol_m );
			
				LOG_COMMON(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_INFO, "CLOSE 3 (URGENT) SHORT SIGNAL FOR " << action.symbol_m << 
					" caused event id=" << cause_event.uid_m.toString()
				);

				doAction(brconnect, action);

			NEW_STATE_ENTRY_END();
		
		NEW_STATE_END();
	OLD_STATE_ENTRY_END()

 

	OLD_STATE_END()


			
}

CppUtils::String MGerchik::convertRtIndexNameToPrecalcHistoryName(CppUtils::String const& name)
{
	if (name=="SP500") return "^GSPC";
	if (name=="ES_CONT") return "^GSPC";

	return "";
}

void MGerchik::doAction(BrkLib::BrokerConnect& brconnect, Action const& action)
{
	
	LOG_SYMBOL(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_DEBUG, action.symbol_m, "I'm going to execute the action: " << action.toString());



	// context
	BrokerContext context = getBrokerContext(action.symbol_m);
	context.action_m = action;

	BrkLib::BrokerOperation oper(brconnect);
	BrkLib::Order order;

	order.symbolNum_m = action.symbol_m;
	order.method_m = order_method_m;
	order.place_m = order_place_m;
	
	// TODO add volume

	//LOG_COMMON(providerForRTSymbols_m, SLogger::SL_DEBUG, "Order id: " << order.ID_m.toString());
	
	// check for laser exception
	try {

		switch(action.action_m) {
				case Action::AS_Dummy:
					break;
				case Action::AS_ClearBrokerContext:
					{
						context.clear();
						updateBrokerContext(action.symbol_m, context);
						
					}
					break;

				case Action::AS_IssueCloseLong: 
					{
						// open opposite order the same volume
						context.close1LongPrice_m = action.opPrice_m;

						// do not forget to update context!!!
						updateBrokerContext(action.symbol_m, context);
						
						order.additionalInfo_m = "TFORCE=TIF_IOC";
						order.orPriceType_m = BrkLib::PC_INPUT;
						order.orderType_m = BrkLib::OP_SELL_LIMIT;
						order.orPrice_m = action.opPrice_m;
						order.orVolume_m = context.volume_m;

						LOG_SYMBOL(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_DEBUG, action.symbol_m, "Closing LONG with volume: " << order.orVolume_m );
						oper.unblockingIssueOrder(order);
																				
					}
					break;
				case Action::AS_IssueCloseShort: 
					{
						// open opposite order the same volume
						context.close1ShortPrice_m = action.opPrice_m;

						// do not forget to update context!!!
						updateBrokerContext(action.symbol_m, context);
						
						order.additionalInfo_m = "TFORCE=TIF_IOC";
						order.orPriceType_m = BrkLib::PC_INPUT;
						order.orderType_m = BrkLib::OP_BUY_LIMIT;
						order.orPrice_m = action.opPrice_m;
						order.orVolume_m = context.volume_m;

						LOG_SYMBOL(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_DEBUG, action.symbol_m, "Closing SHORT with volume: " << order.orVolume_m );
						oper.unblockingIssueOrder(order);
																				 
					}
					break;
				case Action::AS_IssueCancelLong: 
					{
					
						// do not forget to update context!!!
						updateBrokerContext(action.symbol_m, context);

						// cancel pending
						order.orderType_m = BrkLib::OP_CANCEL;
						oper.unblockingIssueOrder(order);
					}
					break;
				case Action::AS_IssueCancelShort: 
					{
					
						// do not forget to update context!!!
						updateBrokerContext(action.symbol_m, context);

						// cancel pending
						order.orderType_m = BrkLib::OP_CANCEL;
						oper.unblockingIssueOrder(order);
					}
					break;
				case Action::AS_IssueClose2Long:
					{
						
						context.close2LongPrice_m = action.opPrice_m;
						// do not forget to update context!!!
						updateBrokerContext(action.symbol_m, context);

						order.additionalInfo_m = "TFORCE=TIF_IOC";
						order.orPriceType_m = BrkLib::PC_INPUT;
						order.orderType_m = BrkLib::OP_SELL_LIMIT;
						order.orPrice_m = action.opPrice_m;
						order.orVolume_m = context.volume_m;
						oper.unblockingIssueOrder(order);
						
					}
					break;
				case Action::AS_IssueClose2Short:
					{
						context.close2LongPrice_m = action.opPrice_m;
						// do not forget to update context!!!
						updateBrokerContext(action.symbol_m, context);
											
						order.additionalInfo_m = "TFORCE=TIF_IOC";
						order.orPriceType_m = BrkLib::PC_INPUT;
						order.orderType_m = BrkLib::OP_BUY_LIMIT;
						order.orPrice_m = action.opPrice_m;
						order.orVolume_m = context.volume_m;
						oper.unblockingIssueOrder(order);
					}
					break;
				case Action::AS_IssueOpenLong: 
					{
						
						context.opBuyLimit_m = action.opPrice_m;
						context.volume_m = action.volume_m;
						context.isLong_m = true;
						// do not forget to update context!!!
						updateBrokerContext(action.symbol_m, context);
						
						

						// issue order
						if (OPEN_IOC_m) {
							if (OPEN_IOC_TIMEOUT_SEC_m <= 0)
								order.additionalInfo_m = "TFORCE=TIF_IOC";
							else
								order.additionalInfo_m = "TFORCE=" + CppUtils::long2String(OPEN_IOC_TIMEOUT_SEC_m);
						}
						else
							order.additionalInfo_m = "TFORCE=TIF_DAY"; 

						order.orPriceType_m = BrkLib::PC_INPUT;
						order.orderType_m = BrkLib::OP_BUY_LIMIT; 
						order.orPrice_m = action.opPrice_m;
						order.orVolume_m = action.volume_m;

						oper.unblockingIssueOrder(order);

																				
					}
					break;
				case Action::AS_IssueOpenShort: 
					{
						
						context.opSellLimit_m = action.opPrice_m;
						context.volume_m = action.volume_m;
						context.isLong_m = false;
						// do not forget to update context!!!
						updateBrokerContext(action.symbol_m, context);
						

						// issue order
						if (OPEN_IOC_m) {
							if (OPEN_IOC_TIMEOUT_SEC_m <= 0)
								order.additionalInfo_m = "TFORCE=TIF_IOC";
							else
								order.additionalInfo_m = "TFORCE=" + CppUtils::long2String(OPEN_IOC_TIMEOUT_SEC_m);
						}
						else
							order.additionalInfo_m = "TFORCE=TIF_DAY"; 

						order.orPriceType_m = BrkLib::PC_INPUT;
						order.orderType_m = BrkLib::OP_SELL_LIMIT;
						order.orPrice_m = action.opPrice_m;
						order.orVolume_m = action.volume_m;

						oper.unblockingIssueOrder(order);
																		
					}
					break;
				case Action::AS_IssueClose3Long: 
					{
						// do not forget to update context!!!
						updateBrokerContext(action.symbol_m, context);

						// market urgent close
						order.orPriceType_m = BrkLib::PC_ZERO;
						order.orderType_m = BrkLib::OP_SELL;
						order.orVolume_m = context.volume_m;

						oper.unblockingIssueOrder(order);
																		
					}
					break;
				case Action::AS_IssueClose3Short: 
					{
						// do not forget to update context!!!
						updateBrokerContext(action.symbol_m, context);

						// market urgent close

						order.orPriceType_m = BrkLib::PC_ZERO;
						order.orderType_m = BrkLib::OP_BUY;
						order.orVolume_m = context.volume_m;

						oper.unblockingIssueOrder(order);
						
																		
					}
					break;
				default: 
					THROW(CppUtils::OperationFailed, "exc_InvalidAction", "ctx_doAction", action.toString())
			}



			LOG_SYMBOL_CTX(internalRun_name_m, providerForRTSymbols_m, SLogger::SL_DEBUG, action.symbol_m, "Normally executed action: " << action.toString(),
				context.toString()
			);

	}
	catch(BrkLib::LaserFailed &e) {

		TradingEventList elist;

		// generate error event
		ACTION_BEGIN(action.action_m)
			ACTION_ENTRY_BEGIN(Action::AS_IssueOpenLong) 
				TradingEvent te(TradingEvent::TE_BrokerRejectTradeLong,  action.symbol_m, -1, true);
				elist.push_back(te);
				
			ACTION_ENTRY_END()

			ACTION_ENTRY_BEGIN(Action::AS_IssueOpenShort) 
				TradingEvent te(TradingEvent::TE_BrokerRejectTradeShort,  action.symbol_m, -1, true);
				elist.push_back(te);
				
			ACTION_ENTRY_END()

			ACTION_ENTRY_BEGIN(Action::AS_IssueCloseLong) 
				TradingEvent te(TradingEvent::TE_BrokerRejectCloseLong,  action.symbol_m, -1, true);
				elist.push_back(te);
				
			ACTION_ENTRY_END()

			ACTION_ENTRY_BEGIN(Action::AS_IssueCloseShort) 
				TradingEvent te(TradingEvent::TE_BrokerRejectCloseShort,  action.symbol_m, -1, true);
				elist.push_back(te);
				
			ACTION_ENTRY_END()

			ACTION_ENTRY_BEGIN(Action::AS_IssueClose2Long) 
				TradingEvent te(TradingEvent::TE_BrokerRejectClose2Long,  action.symbol_m, -1, true);
				elist.push_back(te);
				
			ACTION_ENTRY_END()

			ACTION_ENTRY_BEGIN(Action::AS_IssueClose2Short) 
				TradingEvent te(TradingEvent::TE_BrokerRejectClose2Short,  action.symbol_m, -1, true);
				elist.push_back(te);
			
			ACTION_ENTRY_END()

			ACTION_ENTRY_BEGIN(Action::AS_IssueCancelLong) 
			TradingEvent te(TradingEvent::TE_BrokerRejectCancelLong,  action.symbol_m, -1, true);
				elist.push_back(te);
				
			ACTION_ENTRY_END()

			ACTION_ENTRY_BEGIN(Action::AS_IssueCancelShort) 
				TradingEvent te(TradingEvent::TE_BrokerRejectCancelShort,  action.symbol_m, -1, true);
				elist.push_back(te);
				
			ACTION_ENTRY_END()

				ACTION_ENTRY_BEGIN(Action::AS_IssueClose3Long) 
					TradingEvent te(TradingEvent::TE_BrokerRejectClose3Long,  action.symbol_m, -1, true);
					elist.push_back(te);
				ACTION_ENTRY_END()

				ACTION_ENTRY_BEGIN(Action::AS_IssueClose3Short) 
					TradingEvent te(TradingEvent::TE_BrokerRejectClose3Short,  action.symbol_m, -1, true);
					elist.push_back(te);
				ACTION_ENTRY_END()

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

BrokerContext& MGerchik::getBrokerContext(CppUtils::String const& symbol)
{
	LOCK_FOR_SCOPE(brokerContextMtx_m);
	return brokerContext_m[symbol];
}

void MGerchik::updateBrokerContext(CppUtils::String const& symbol, BrokerContext const& context)
{
	LOCK_FOR_SCOPE(brokerContextMtx_m);
	brokerContext_m[symbol] = context;
}

void MGerchik::pushProcessingEvent(BrkLib::BrokerConnect const& brkConnect, TradingEventList const& eventList)
{
	// this will push event data that must be processed in the algorithm/thread
	EventProcessingStruct procStruct(brkConnect, eventList);

	{
		LOCK_FOR_SCOPE(receivedProcessingDataMtx_m);
		
		receivedProcessingData_m.push_back(procStruct);
		receivedProcessingDataArrived_m.signalEvent();
	}
}

void MGerchik::threadProcessingThread()
{
	threadStarted_m.signalEvent();
	LOG(MSG_INFO, ALG_M_GERCHIK, MGerchik::getName() << " - working thread started");


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

	LOG(MSG_INFO, ALG_M_GERCHIK, MGerchik::getName() << " - working thread finished");
}

int MGerchik::getSecuritiesNumberInProgress() const
{
	int cnt = 0;
	for(map<CppUtils::String,TradingState>::const_iterator it = state_m.begin(); it != state_m.end(); it++) {
		TradingState const& state_i = it->second;
		
		if (!state_i.isIdle() )
			cnt++;

	}

	return cnt;
}

}; // end of namespace