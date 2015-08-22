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


		CPairTrade* pThis = (CPairTrade*)ptr; 
		try {
			pThis->helperThreadFun();
		}
		catch(CppUtils::Exception& e) {
			LOG(MSG_FATAL, PAIR_TRADE, "Fatal exception: " << e.message());
		}
#ifdef HANDLE_NATIVE_EXCEPTIONS
		catch(Testframe::Exception& e) {
			LOG(MSG_FATAL, PAIR_TRADE, "Fatal MSVC exception: " << e.message());
		}
#endif
		catch(...) {
			LOG(MSG_FATAL, PAIR_TRADE, "Unknown fatal exception");
		}
		return 42;

	}


AlgorithmBase *creator (Inqueue::AlgorithmHolder* algholder)
{
	return new CPairTrade(*algholder);
}

void terminator (AlgorithmBase *pthis)
{
	assert(pthis);
	delete pthis;
}

AlgorithmBase::CreatorAlgorithmFun createAlgorithm = creator;

AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm = terminator;

// intreface to be implemented
// -----------------------------

CPairTrade::CPairTrade( Inqueue::AlgorithmHolder& algHolder):
AlgorithmBase(algHolder),
robot_m(*this)
{
	 	thread_m = new CppUtils::Thread(bootProcessingThread, this);

		// if cannot start thread for 10 seconds
		if (!threadStarted_m.lock(10000))
			THROW(CppUtils::OperationFailed, "exc_CannorStartProcessingThread", "ctx_CAlgMisha2Ctor", "");

}
										 
CPairTrade::~CPairTrade()
{
		shutDownThread_m.signalEvent();
		thread_m->waitForThread();
		// Delete thread
		delete thread_m;
		thread_m = NULL;

		LOG(MSG_INFO, PAIR_TRADE, CPairTrade::getName() << " - deinitialized");
}

void CPairTrade::onLibraryInitialized(
	HlpStruct::AlgBrokerDescriptor const& descriptor, 
	BrkLib::BrokerBase* brokerlib, 
	BrkLib::BrokerBase* brokerlib2,
	CppUtils::String const& runName,
	CppUtils::String const& comment
)
{
		
		//LOG(MSG_INFO, PAIR_TRADE, descriptor.dump());


		//brokerConnectDummy_m = NULL;

		
		connectRunName_m = runName;

	
		// read params
		historyDataProfile_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "HISTORY_DATA_PROFILE");

		rtDataProvider_Smartcom_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "RT_PROVIDER_SMARTCOM");

		barDataProvider_Smartcom_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "RT_PROVIDER_BAR_SMARTCOM");

		barDataProvider_Finam_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "RT_PROVIDER_BAR_FINAM");

		
		TradeParam tradeParam;

		tradeParam.isDemoTrade_m = GET_ALG_TRADE_BOOL(descriptor.initialAlgBrkParams_m, "IS_DEMO_TRADE");
		tradeParam.tradePeriodMinutes_m = GET_ALG_TRADE_INT(descriptor.initialAlgBrkParams_m, "TRADE_PERIOD_MINUTES");
		if (tradeParam.tradePeriodMinutes_m <=0)
			tradeParam.tradePeriodMinutes_m = 15;


		
		// so far hard coded
		tradeParam.tradeVolume_m = 1;

		tradeParam.brokerTimeoutSec_m =	 GET_ALG_TRADE_INT(descriptor.initialAlgBrkParams_m, "BROKER_TIMEOUT_SEC");

		tradeParam.maSlowPeriod_m = GET_ALG_TRADE_INT(descriptor.initialAlgBrkParams_m, "MA_SLOW_PERIOD");
		tradeParam.maFastPeriod_m = GET_ALG_TRADE_INT(descriptor.initialAlgBrkParams_m, "MA_FAST_PERIOD");

		CppUtils::String symbolsFileName_Finam = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "SYMBOL_FILE_FINAM");

		CppUtils::String symbolsFileName_Smartcom = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "SYMBOL_FILE_SMARTCOM");

		// synthetic symbols
		CppUtils::String symbolsFileSynthName_Smartcom = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "SYMBOL_SYNTH_FILE_SMARTCOM");

		
		baseExportPath_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "BASE_EXPORT_PATH");
		// need to export - when new period comes
		baseExportPath_m = baseExportPath_m + "\\";
		CppUtils::makeDir(baseExportPath_m);

		// it's map as may use a pseudonym
		CppUtils::StringMap symbolsSmartcom, symbolsSmartcomReverse;
		CppUtils::StringMap symbolsFinam;

		// synthetic is ony a set
		CppUtils::StringSet symbolsSyntheticSmartcom;
		CppUtils::StringMap symbolsSyntheticSmartcom_EnableMap;

		//
		if (CppUtils::fileExists(symbolsFileName_Finam)) {
			
			CppUtils::parseSymbolMapFile(symbolsFinam, symbolsFileName_Finam, true);
			for(CppUtils::StringMap::const_iterator it = symbolsFinam.begin(); it != symbolsFinam.end(); it++) {
				LOG_COMMON(connectRunName_m, getBarDataProvider_Smartcom(), BrkLib::AP_MEDIUM, "Finam symbol -> pseudonym: " << it->first << " -> " << it->second);
			}

		}
		else
			THROW(CppUtils::OperationFailed, "exc_SymbolFileNotExists", "ctx_onLibraryInitialized", symbolsFileName_Finam );

		//
		if (CppUtils::fileExists(symbolsFileName_Smartcom)) {
			
			CppUtils::parseSymbolMapFile(symbolsSmartcom, symbolsFileName_Smartcom, true);
			for(CppUtils::StringMap::const_iterator it = symbolsSmartcom.begin(); it != symbolsSmartcom.end(); it++) {
				symbolsSmartcomReverse[it->second] = it->first;
				LOG_COMMON(connectRunName_m, getBarDataProvider_Smartcom(), BrkLib::AP_MEDIUM, "Smartcom symbol -> pseudonym: " << it->first << " -> " << it->second);
			}
		}
		else
			THROW(CppUtils::OperationFailed, "exc_SymbolFileNotExists", "ctx_onLibraryInitialized", symbolsFileName_Smartcom );

		if (CppUtils::fileExists(symbolsFileSynthName_Smartcom)) {
			
			CppUtils::String content;
			if (!CppUtils::readContentFromFile2(symbolsFileSynthName_Smartcom, content))
				THROW(CppUtils::OperationFailed, "exc_CannotParseFile", "ctx_onLibraryInitialized", symbolsFileSynthName_Smartcom );

			CppUtils::StringList slist;
			CppUtils::tokenize(content, slist, ",\n");

			
			int i = 0;
			while(true ) {
				if ((i+2) < slist.size()) {
				
					CppUtils::String const& synth_symbol =	slist[i];
					CppUtils::String const& enable_flag =slist[i + 1];
					CppUtils::String const& portfolio =slist[i + 2];

					symbolsSyntheticSmartcom.insert( synth_symbol );
					if (enable_flag == "1") {
						tradeParam.synthSymbolToTrade_m = synth_symbol;
						tradeParam.tradePrortfolio_m = portfolio;
					}
					
					i += 3;
				}
				else
					break;
			}

		
			if (tradeParam.synthSymbolToTrade_m.get().length() == 0 )
				THROW(CppUtils::OperationFailed, "exc_InvalidDefaultSyntheticSymbol", "ctx_onLibraryInitialized", symbolsFileSynthName_Smartcom );

			for(CppUtils::StringSet::const_iterator it = symbolsSyntheticSmartcom.begin(); it != symbolsSyntheticSmartcom.end(); it++) {
				LOG_COMMON(connectRunName_m, getBarDataProvider_Smartcom(), BrkLib::AP_MEDIUM, "Synthetic symbol: " << *it);
			}

			LOG_COMMON(connectRunName_m, getBarDataProvider_Smartcom(), BrkLib::AP_MEDIUM, "Synthetic symbol to trade: " << tradeParam.synthSymbolToTrade_m.get() <<
				" using portfolio: " << tradeParam.tradePrortfolio_m.get());

		}
		else
			THROW(CppUtils::OperationFailed, "exc_SymbolFileNotExists", "ctx_onLibraryInitialized", symbolsFileSynthName_Smartcom );

		
		robot_m.init(	descriptor, brokerlib, runName, comment, tradeParam, symbolsFinam, symbolsSmartcom, symbolsSmartcomReverse, symbolsSyntheticSmartcom);

		LOG_COMMON(connectRunName_m, getBarDataProvider_Smartcom(), BrkLib::AP_MEDIUM, "Trading parameters: " << tradeParam.toDescriptionString());

}

void CPairTrade::onLibraryFreed()
{
		robot_m.deinit();
}



void CPairTrade::onBrokerResponseArrived(
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
)
{
	if (bresponse.type_m != BrkLib::RT_BrokerResponseOrder)
		return;

	MyBrokerContext const* myCtxPtr = (MyBrokerContext const*)bresponse.context_m.getContext();


	if (myCtxPtr != NULL) {
		// context is const but we want actually change this
		CppUtils::BaseAction<MachineContext>& action =  const_cast<CppUtils::BaseAction<MachineContext>&>(myCtxPtr->getAction());

		BrokerResponseRecipient* rp = dynamic_cast<BrokerResponseRecipient*>(&action);
		if (rp != NULL) {
			CppUtils::BaseAction<MachineContext>* action = dynamic_cast< CppUtils::BaseAction<MachineContext>* >(rp);

			LOG_COMMON( getRunName(), getRtdataProvider_Smartcom(), BrkLib::AP_MEDIUM, "Broker response delegated to: " << action->toString());
			rp->onCallbackReceived(brkConnect, bresponse);
		}
		else {																							 
			// unknown target
			LOG_COMMON( getRunName(), getRtdataProvider_Smartcom(), BrkLib::AP_MEDIUM, "Broker response delegated to default handler - unknown target");

			//LOG(MSG_INFO, "Check - Unknown target", "Check: \n" << bresponse.toString());

			BrokerResponseRecipient::onUnknownCallbackreceived(brkConnect, bresponse);
		}
		
	} else {
		// unknown target
		LOG_COMMON( getRunName(), getRtdataProvider_Smartcom(), BrkLib::AP_MEDIUM, "Broker response delegated to default handler - NULL context");

		//LOG(MSG_INFO, "Check - NULL context", "Check: \n" << bresponse.toString());

		BrokerResponseRecipient::onUnknownCallbackreceived(brkConnect, bresponse);
	}
}

void CPairTrade::onDataArrived(
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
		
void CPairTrade::onRtProviderSynchEvent (
		Inqueue::ContextAccessor& contextaccessor, HlpStruct::RtData::SynchType const synchFlg,
		double const& synchTime,
		CppUtils::String const& providerName,
		CppUtils::String const& message
)
{
	 // RT provider events - must check for sisconnections
		if (synchFlg==HlpStruct::RtData::SYNC_EVENT_PROVIDER_START) {
			LOG_COMMON(connectRunName_m,getBarDataProvider_Smartcom(),BrkLib::AP_MEDIUM,"RT Provider started (connected): " << providerName);
		}
		else if (synchFlg==HlpStruct::RtData::SYNC_EVENT_PROVIDER_FINISH) {
			LOG_COMMON(connectRunName_m,getBarDataProvider_Smartcom(),BrkLib::AP_MEDIUM,"RT Provider finished (disconnected): " << providerName);

			// reset everything
			//bool result;
			//CppUtils::String log;
			//contextaccessor.requestRunExternalPackage("pair_trade.jar", true, result, log);

		}
		else if (synchFlg==HlpStruct::RtData::SYNC_EVENT_PROVIDER_TRANSMIT_ERROR) {
			LOG_COMMON(connectRunName_m,getBarDataProvider_Smartcom(),BrkLib::AP_HIGH,"RT Provider transmit error: " << providerName << " - " << message);

			
		}
		else if (synchFlg==HlpStruct::RtData::SYNC_EVENT_CUSTOM_MESSAGE) {
			LOG_COMMON(connectRunName_m,getBarDataProvider_Smartcom(),BrkLib::AP_HIGH,"RT Provider custom message: " << providerName << " - " << message);
		}
}
   
void CPairTrade::onLevel2DataArrived(
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

void CPairTrade::onLevel1DataArrived(
		HlpStruct::RtLevel1Data const& level1data,
		Inqueue::ContextAccessor& historyaccessor, 
		HlpStruct::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
	)
{

}

void CPairTrade::onEventArrived(
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
		HlpStruct::XmlParameter xmlparam_data_v;
		HlpStruct::HumanSerializer::deserializeXmlParameter(data, xmlparam_data_v);


		HlpStruct::XmlParameter const& xmlparam_data = (HlpStruct::XmlParameter const&)xmlparam_data_v;
		if (command=="hallo") {
				
		}
		else if (command=="dump_symbol") {
			CppUtils::String const& symbol = xmlparam_data.getStringParameter("symbol");

			exportSymbol(contextaccessor, symbol);

			LOG_COMMON(connectRunName_m,getBarDataProvider_Smartcom(), BrkLib::AP_MEDIUM, "Symbol dumped");
		}  
		else if (command=="load_data") {
			bool loaded = contextaccessor.loadDataFromMaster(historyDataProfile_m, false);

			if (!loaded)
				THROW(CppUtils::OperationFailed, "exc_CannotLoadHistoryData", "ctx_onEventArrived", "");
			
		
			LOG_COMMON(connectRunName_m,getBarDataProvider_Smartcom(), BrkLib::AP_MEDIUM, "Loaded history OK ");
		}
		else if (command=="dump_cache") {
			// this command dump cache file to see what we have in memory
			CppUtils::String const& fileTmp = CppUtils::getTempPath() + "\\dump_" + CppUtils::createValidFileName(getRunName()) + "_" + CppUtils::long2String(CppUtils::getTime()) + ".txt";
			CppUtils::saveContentInFile2(fileTmp,  contextaccessor.makeDebugDump(true));
		}
		else if (command=="simulate_signal_up") {

		
			robot_m.simulateUp();
		}
		else if (command=="simulate_signal_down") {

		
			robot_m.simulateDown();
		}
		else if (command=="start_trade") {

			
			if ( xmlparam_data.isKeyExists("allowed_direction")) {
				CppUtils::String  direction = xmlparam_data.getParameter("allowed_direction").getAsString();
				CppUtils::toupper( direction );
				
				robot_m.getGlobalTradeContext().get().allowTrading_m = BrkLib::TD_NONE;

				if (direction == "SHORT") {
					robot_m.getGlobalTradeContext().get().allowTrading_m =  BrkLib::TD_SHORT;
				}
				else if (direction == "LONG") {
					robot_m.getGlobalTradeContext().get().allowTrading_m =  BrkLib::TD_LONG;
				}
				else if (direction == "BOTH") {
					robot_m.getGlobalTradeContext().get().allowTrading_m = BrkLib::TD_BOTH;
				}
				
			}

			if ( xmlparam_data.isKeyExists("current_position")) {
				CppUtils::String  direction = xmlparam_data.getParameter("current_position").getAsString();
				CppUtils::toupper( direction );
				
				if (direction == "SHORT") {
					robot_m.sendInitialEvent( BrkLib::TD_SHORT );
				}
				else if (direction == "LONG") {
					robot_m.sendInitialEvent( BrkLib::TD_LONG );
				}
				else if (direction == "NONE") {
					robot_m.sendInitialEvent( BrkLib::TD_NONE);
				}
				
			}

			
			
		}
		else if (command=="stop_trade") {
			robot_m.sendStopTradeEvent();
			LOG_COMMON(connectRunName_m,getBarDataProvider_Smartcom(), BrkLib::AP_MEDIUM, "Prohibited broker operations broker operations");

		}
		else
			THROW(CppUtils::OperationFailed, "exc_InvalidCommand", "ctx_onEventArrived", command);

		// no exceptions
		customOutputData ="Command: [" +command+ "] done OK";
}
	
	void CPairTrade::onThreadStarted(
		Inqueue::ContextAccessor& contextaccessor, bool const firstLib, bool const lastLib
	)
{
	robot_m.onThreadStarted(contextaccessor, firstLib, lastLib);
}

void CPairTrade::onThreadFinished(
	Inqueue::ContextAccessor& contextaccessor, HlpStruct::TradingParameters const& tradeParams, bool const firstLib, bool const lastLib
)
{
	robot_m.onThreadFinished(contextaccessor, tradeParams, firstLib, lastLib);
}

CppUtils::String const& CPairTrade::getName() const
{
	static const CppUtils::String name("[ alg lib: CPairTrade - release 1.0.2 ]");
	return name;
}


	void CPairTrade::exportSymbol(Inqueue::ContextAccessor& historyaccessor, CppUtils::String const& symbol, int const cache_id)
	{
			CppUtils::String fileExportMame = baseExportPath_m + "\\"+ symbol + ".txt";
				
			Math::ExporterEngine<Math::PriceSeriesProvider> exporter(fileExportMame, false);
			bool result = historyaccessor.callMathEngine<bool, Math::P_CloseBid, Math::ExporterEngine<Math::PriceSeriesProvider> >(cache_id, exporter);

			if (!result) {
				LOG_SYMBOL_PROBLEM(connectRunName_m,getBarDataProvider_Smartcom(), symbol,"Cannot export symbol to: " << fileExportMame );
			}
	}

	void CPairTrade::exportSymbol(Inqueue::ContextAccessor& historyaccessor, CppUtils::String const& symbol)
	{
			int cache_symbol_id = historyaccessor.cacheSymbol(getBarDataProvider_Finam(), symbol, HlpStruct::AP_Minute, 1);

			// need to recalculate max & mins
			if (cache_symbol_id <= 0) {
				LOG_SYMBOL_PROBLEM(connectRunName_m,getBarDataProvider_Smartcom(), symbol,"Cannot cache symbol upon request: "  << symbol);
				return;
			}
																    
			CppUtils::String fileExportMame = baseExportPath_m + "\\"+ symbol + ".txt";
				
			Math::ExporterEngine<Math::PriceSeriesProvider> exporter(fileExportMame, false);
			bool result = historyaccessor.callMathEngine<bool, Math::P_CloseBid, Math::ExporterEngine<Math::PriceSeriesProvider> >(cache_symbol_id, exporter);

			if (!result) {
				LOG_SYMBOL_PROBLEM(connectRunName_m,getBarDataProvider_Smartcom(), symbol,"Cannot export symbol to: " << fileExportMame );
			}
	}															   

	void CPairTrade::helperThreadFun()
	{
		threadStarted_m.signalEvent();
		LOG(MSG_INFO, PAIR_TRADE, CPairTrade::getName() << " - helper thread started");
		LOG_COMMON(connectRunName_m, getBarDataProvider_Smartcom(), BrkLib::AP_MEDIUM,  "Helper thread started");


		

		while(true) {

			// lock
			bool res = shutDownThread_m.lock(1000);
			if (res)
				break; 

			try {
				getRobot().getLevelCalculator().heartBeat();

			}
			catch(CppUtils::Exception& e) {
				LOG_COMMON(connectRunName_m, getBarDataProvider_Smartcom(), BrkLib::AP_MEDIUM, "Error in helper thread: " << e.message());
			}
#ifdef HANDLE_NATIVE_EXCEPTIONS
			catch(Testframe::Exception& e) {
				LOG_COMMON(connectRunName_m, getBarDataProvider_Smartcom(), BrkLib::AP_MEDIUM, "MSVC error in helper thread: " << e.message());
			}
#endif
			catch(...) {
				LOG_COMMON(connectRunName_m, getBarDataProvider_Smartcom(), BrkLib::AP_MEDIUM,  "Unknown error in helper thread");
			}

		}

		LOG_COMMON(connectRunName_m, getBarDataProvider_Smartcom(), BrkLib::AP_MEDIUM,  "Helper thread finished");
		LOG(MSG_INFO, PAIR_TRADE, CPairTrade::getName() << " - helper thread finished");
	}


}; // end of namespace
