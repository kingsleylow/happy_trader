#include "implement.hpp"

 // Many thanks to Zoubok Victor for programming this





namespace AlgLib {


	// -----------------------------


	int bootProcessingThread (void *ptr)
	{
		// handle run-time exceptions
#ifdef HANDLE_NATIVE_EXCEPTIONS
		Testframe::init();
#endif


		CAlgMisha2* pThis = (CAlgMisha2*)ptr; 
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
	return new CAlgMisha2(*algholder);
}

void terminator (AlgorithmBase *pthis)
{
	assert(pthis);
	delete pthis;
}

AlgorithmBase::CreatorAlgorithmFun createAlgorithm = creator;

AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm = terminator;

// intreface to be implemented


CAlgMisha2::CAlgMisha2( Inqueue::AlgorithmHolder& algHolder):
AlgorithmBase(algHolder),
robot_m(*this)
{
	 	thread_m = new CppUtils::Thread(bootProcessingThread, this);

		// if cannot start thread for 10 seconds
		if (!threadStarted_m.lock(10000))
			THROW(CppUtils::OperationFailed, "exc_CannorStartProcessingThread", "ctx_CAlgMisha2Ctor", "");

}
										 
CAlgMisha2::~CAlgMisha2()
{
		shutDownThread_m.signalEvent();
		thread_m->waitForThread();
		// Delete thread
		delete thread_m;
		thread_m = NULL;

		LOG(MSG_INFO, SMARTCOM_ALG_MISHA, CAlgMisha2::getName() << " - deinitialized");
}

void CAlgMisha2::onLibraryInitialized(
	HlpStruct::AlgBrokerDescriptor const& descriptor, 
	BrkLib::BrokerBase* brokerlib, 
	BrkLib::BrokerBase* brokerlib2,
	CppUtils::String const& runName,
	CppUtils::String const& comment
)
{
		connectRunName_m = runName;

		allowTrading_m = BrkLib::TD_NONE;

		// read params
		historyDataProfile_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "HISTORY_DATA_PROFILE");

		rtDataProvider_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "RT_PROVIDER");

		barDataProvider_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "RT_BAR_PROVIDER");


		TradeParam tradeParam;

		tradeParam.cutoffVolume_m = GET_ALG_TRADE_DOUBLE(descriptor.initialAlgBrkParams_m, "CUTOFF_VOLUME");

		tradeParam.maxPriceDifference_m =	 GET_ALG_TRADE_DOUBLE(descriptor.initialAlgBrkParams_m, "MAX_PRICE_DIFFERENCE");

		tradeParam.maxMinutesLevelTrack_m = GET_ALG_TRADE_DOUBLE(descriptor.initialAlgBrkParams_m, "MAX_MINUTES_LEVEL_TRACK");

		tradeParam.workTimeHolder_m.get().setUp(descriptor.initialAlgBrkParams_m.algParams_m, "BEGIN_WORK_TIME", "END_WORK_TIME");
		
		tradeParam.tradeVolume_m = GET_ALG_TRADE_DOUBLE(descriptor.initialAlgBrkParams_m, "TRADE_VOLUME");

		tradeParam.tp_relativeLevel_m = GET_ALG_TRADE_DOUBLE(descriptor.initialAlgBrkParams_m, "TP_RELATIVE_LEVEL");

		tradeParam.sl_relativeLevel_m = GET_ALG_TRADE_DOUBLE(descriptor.initialAlgBrkParams_m, "SL_RELATIVE_LEVEL");

		tradeParam.isDemoTrade_m = GET_ALG_TRADE_BOOL(descriptor.initialAlgBrkParams_m, "IS_DEMO_TRADE");
		
		CppUtils::String symbolsFileName = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "SYMBOL_FILE");

		//CppUtils::String tradeMode_s = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "TRADE_MODE");
		//CppUtils::toupper(tradeMode_s);

		
		tradeParam.tradePeriodMinutes_m = GET_ALG_TRADE_INT(descriptor.initialAlgBrkParams_m, "TRADE_PERIOD_MINUTES");
		if (tradeParam.tradePeriodMinutes_m <=0)
			tradeParam.tradePeriodMinutes_m = 15;

		baseExportPath_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "BASE_EXPORT_PATH");
		// need to export - when new period comes
		baseExportPath_m = baseExportPath_m + "\\" + CppUtils::long2String(tradeParam.tradePeriodMinutes_m);
		CppUtils::makeDir(baseExportPath_m);


		if (CppUtils::fileExists(symbolsFileName)) {

			symbols_m.clear();
			CppUtils::parseSymbolListFile(symbols_m, symbolsFileName);
			
		}
		else
			THROW(CppUtils::OperationFailed, "exc_SymbolFileNotExists", "ctx_onLibraryInitialized", symbolsFileName );

		// init state machine
		robot_m.init(	descriptor, brokerlib, runName, comment, tradeParam, symbols_m);
	
}

void CAlgMisha2::onLibraryFreed()
{
	robot_m.deinit();
}



void CAlgMisha2::onBrokerResponseArrived(
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
)
{
	

	MyBrokerContext const* myCtxPtr = (MyBrokerContext const*)bresponse.context_m.getContext();
	MyBrokerContext const* myCauseCtxPtr = (MyBrokerContext const*)bresponse.causeContext_m.getContext();

	if (myCauseCtxPtr != NULL)
		// in case of cancellations
		myCtxPtr = myCauseCtxPtr;

	if (myCtxPtr != NULL) {
		// context is const but we want actually change this
		CppUtils::BaseAction<MachineContext>& action =  const_cast<CppUtils::BaseAction<MachineContext>&>(myCtxPtr->getAction());

		BrokerResponseRecipient* rp = dynamic_cast<BrokerResponseRecipient*>(&action);
		if (rp != NULL) {
			CppUtils::BaseAction<MachineContext>* action = dynamic_cast< CppUtils::BaseAction<MachineContext>* >(rp);

			LOG_COMMON( getRunName(), getRtDataProvider(), BrkLib::AP_MEDIUM, "Broker response delegated to: " << action->toString());
			rp->onCallbackReceived(brkConnect, bresponse);
		}
		else {
			// unknown target
			LOG_COMMON( getRunName(), getRtDataProvider(), BrkLib::AP_MEDIUM, "Broker response delegated to default handler - unknown target");

			BrokerResponseRecipient::onUnknownCallbackreceived(brkConnect, bresponse);
		}
		
	} else {
		// unknown target
		LOG_COMMON( getRunName(), getRtDataProvider(), BrkLib::AP_MEDIUM, "Broker response delegated to default handler - NULL context");

		BrokerResponseRecipient::onUnknownCallbackreceived(brkConnect, bresponse);
	}

	//MyBrokerContext const* myCauseCtxPtr = (MyBrokerContext const*)bresponse.causeContext_m.getContext();

	// due to the nature of actionswe are going to delegate these events to action objects

	/*
	if (bresponse.type_m == BrkLib::RT_BrokerResponseOrder) {
		
		BrkLib::BrokerResponseOrder const& bRespOrder = (BrkLib::BrokerResponseOrder const&)bresponse;

		if (bRespOrder.resultCode_m ==  BrkLib::PT_ORDER_ESTABLISHED)	{
			if (bRespOrder.resultDetailCode_m & BrkLib::ODR_ORDER_PENDING) {

			}
			else if (bRespOrder.resultDetailCode_m & BrkLib::ODR_ORDER_INSTALLED) {
				
			}
			else if (bRespOrder.resultDetailCode_m & BrkLib::ODR_ORDER_EXECUTED) {
				
			}
		}
		else if (bRespOrder.resultCode_m == BrkLib::PT_ORDER_CANCELLED) {
			if (bRespOrder.resultDetailCode_m & BrkLib::ODR_OK) {

			}
		}
		else if (bRespOrder.resultCode_m==BrkLib::PT_GENERAL_ERROR_HAPPENED) {

		}
	}
	*/
	
}

void CAlgMisha2::onDataArrived(
	HlpStruct::RtData const& rtdata, 
	Inqueue::ContextAccessor& historyaccessor, 
	HlpStruct::TradingParameters const& descriptor,
	BrkLib::BrokerBase* brokerlib,
	CppUtils::String const& runName,
	CppUtils::String const& comment
)
{
	
	robot_m.onDataArrived(rtdata, historyaccessor, descriptor, brokerlib, runName, comment);

}
		
void CAlgMisha2::onRtProviderSynchEvent (
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
		/*
		else if (synchFlg==HlpStruct::RtData::SYNC_EVENT_PROVIDER_FINISH) {
			LOG_COMMON(connectRunName_m,rtDataProvider_m,BrkLib::AP_MEDIUM,"RT Provider finished: " << providerName);

			// reset everything
			bool result;
			CppUtils::String log;
			contextaccessor.requestRunExternalPackage("german_starter.jar", true, result, log);

		}
		*/
		else if (synchFlg==HlpStruct::RtData::SYNC_EVENT_PROVIDER_TRANSMIT_ERROR) {
			LOG_COMMON(connectRunName_m,rtDataProvider_m,BrkLib::AP_HIGH,"RT Provider transmit error: " << providerName << " - " << message);

			
		}
		else if (synchFlg==HlpStruct::RtData::SYNC_EVENT_CUSTOM_MESSAGE) {
			LOG_COMMON(connectRunName_m,rtDataProvider_m,BrkLib::AP_HIGH,"RT Provider custom message: " << providerName << " - " << message);
		}
}
   
void CAlgMisha2::onLevel2DataArrived(
		HlpStruct::RtLevel2Data const& level2data,
		Inqueue::ContextAccessor& historyaccessor, 
		HlpStruct::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
	)
{
	// 
	robot_m.onLevel2DataArrived(level2data, historyaccessor, descriptor, brokerlib, runName, comment);
}

void CAlgMisha2::onLevel1DataArrived(
		HlpStruct::RtLevel1Data const& level1data,
		Inqueue::ContextAccessor& historyaccessor, 
		HlpStruct::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
	)
{

}

void CAlgMisha2::onEventArrived(
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
		else if (command=="dump_all") {
			exportAll(contextaccessor);

			LOG_COMMON(connectRunName_m,rtDataProvider_m, BrkLib::AP_MEDIUM, "All symbols dumped");
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
			
		
			LOG_COMMON(connectRunName_m,rtDataProvider_m, BrkLib::AP_MEDIUM, "Loaded history OK, working with minutes period: " << robot_m.getTradeParam().tradePeriodMinutes_m);
		}
		else if (command=="manual_fix") {
			//TE_Manual_Fix event_i;
			//tradingMachines_m.processEvent<TE_Manual_Fix>(xmlparam_data.getParameter("symbol").getAsString(), event_i);
		}
		else if (command=="dump_cache") {
			// this command dump cache file to see what we have in memory
			CppUtils::String const& fileTmp = CppUtils::getTempPath() + "\\dump_" + CppUtils::createValidFileName(getRunName()) + "_" + CppUtils::long2String(CppUtils::getTime()) + ".txt";
			CppUtils::saveContentInFile2(fileTmp,  contextaccessor.makeDebugDump(true));
		}
		else
			THROW(CppUtils::OperationFailed, "exc_InvalidCommand", "ctx_onEventArrived", command);

		// no exceptions
		customOutputData = command + " executed OK";
}
	
	void CAlgMisha2::onThreadStarted(
		Inqueue::ContextAccessor& contextaccessor, bool const firstLib, bool const lastLib
	)
{

}

void CAlgMisha2::onThreadFinished(
	Inqueue::ContextAccessor& contextaccessor, HlpStruct::TradingParameters const& tradeParams, bool const firstLib, bool const lastLib
)
{

}

CppUtils::String const& CAlgMisha2::getName() const
{
	static const CppUtils::String name("[ alg lib: CAlgMisha2 ]");
	return name;
}

	void CAlgMisha2::exportAll(Inqueue::ContextAccessor& historyaccessor)	
	{
		for(CppUtils::StringSet::const_iterator it = symbols_m.begin(); it != symbols_m.end(); it++) {
			 exportSymbol(historyaccessor, *it);
		}
	}

	void CAlgMisha2::exportSymbol(Inqueue::ContextAccessor& historyaccessor, CppUtils::String const& symbol, int const cache_id)
	{
			CppUtils::String fileExportMame = baseExportPath_m + "\\"+ symbol + ".txt";
				
			Math::ExporterEngine<Math::PriceSeriesProvider> exporter(fileExportMame, false);
			bool result = historyaccessor.callMathEngine<bool, Math::P_CloseBid, Math::ExporterEngine<Math::PriceSeriesProvider> >(cache_id, exporter);

			if (!result) {
				LOG_SYMBOL_PROBLEM(connectRunName_m,rtDataProvider_m, symbol,"Cannot export symbol to: " << fileExportMame );
			}
	}

	void CAlgMisha2::exportSymbol(Inqueue::ContextAccessor& historyaccessor, CppUtils::String const& symbol)
	{
			int cache_symbol_id = historyaccessor.cacheSymbol(barDataProvider_m, symbol, HlpStruct::AP_Minute, robot_m.getTradeParam().tradePeriodMinutes_m);

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

	void CAlgMisha2::helperThreadFun()
	{
		threadStarted_m.signalEvent();
		LOG(MSG_INFO, SMARTCOM_ALG_MISHA, CAlgMisha2::getName() << " - helper thread started");
		LOG_COMMON(connectRunName_m, rtDataProvider_m, BrkLib::AP_MEDIUM,  "Helper thread started");


		

		while(true) {

			// lock
			bool res = shutDownThread_m.lock(1000);
			if (res)
				break;

			try {
				// check if we are in timeout
				//for(CppUtils::StringSet::const_iterator it = symbols_m.begin(); it != symbols_m.end(); it++) {
					// timer event
					//TE_On_Timer event_i;
					//tradingMachines_m.processEvent<TE_On_Timer>(*it, event_i);
				//}

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
		LOG(MSG_INFO, SMARTCOM_ALG_MISHA, CAlgMisha2::getName() << " - helper thread finished");
	}


} // end of namespace
