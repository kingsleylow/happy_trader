#include "implement.hpp"
// Many thanks to Zoubok Victor for programming this



namespace AlgLib {
	AlgorithmBase *creator (Inqueue::AlgorithmHolder* algholder)
	{
		return new Grm2ChannelMM(*algholder);
	}

	void terminator (AlgorithmBase *pthis)																				
	{
		HTASSERT(pthis);
		delete pthis;
	}

	AlgorithmBase::CreatorAlgorithmFun createAlgorithm = creator;

	AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm = terminator;

	// intreface to be implemented
	Grm2ChannelMM::Grm2ChannelMM( Inqueue::AlgorithmHolder& algHolder):
	AlgorithmBase(algHolder),
		allowTrading_m(TDNONE)
	{

	}

	Grm2ChannelMM::~Grm2ChannelMM()
	{

	}

	void Grm2ChannelMM::onLibraryInitialized(
		HlpStruct::AlgBrokerDescriptor const& descriptor, 
		BrkLib::BrokerBase* brokerlib, 
		CppUtils::String const& runName,
		CppUtils::String const& comment
		)
	{	
		brokerConnect_m = NULL;
		connectRunName_m = runName;

		allowTrading_m = TDNONE;

		// read params
		historyDataProfile_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "HISTORY_DATA_PROFILE");

		rtDataProvider_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "RT_PROVIDER");

		barDataProvider_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "RT_BAR_PROVIDER");

		tradeParam_m.workTimeHolder_m.setUp(descriptor.initialAlgBrkParams_m.algParams_m, "BEGIN_WORK_TIME", "END_WORK_TIME");

		tradeParam_m.period_m = GET_ALG_TRADE_INT(descriptor.initialAlgBrkParams_m, "PERIOD");

		tradeParam_m.periodStop_m= GET_ALG_TRADE_INT(descriptor.initialAlgBrkParams_m, "PERIOD_STOP");

		tradeParam_m.isReverse_m = GET_ALG_TRADE_BOOL(descriptor.initialAlgBrkParams_m, "IS_REVERSE");

		tradeParam_m.koeff_m = GET_ALG_TRADE_DOUBLE(descriptor.initialAlgBrkParams_m, "KOEFF");
		if (tradeParam_m.koeff_m <=0)
			tradeParam_m.koeff_m = 1.0;

		tradeParam_m.minLots_m = GET_ALG_TRADE_DOUBLE(descriptor.initialAlgBrkParams_m, "MIN_LOTS");
		if (tradeParam_m.minLots_m <=0)
			tradeParam_m.minLots_m = 1.0;

		tradeParam_m.maxLots_m = GET_ALG_TRADE_DOUBLE(descriptor.initialAlgBrkParams_m, "MAX_LOTS");
		if (tradeParam_m.maxLots_m <=0)
			tradeParam_m.maxLots_m = 5.0;

		tradeParam_m.risk_m =  GET_ALG_TRADE_DOUBLE(descriptor.initialAlgBrkParams_m, "RISK");
		if (tradeParam_m.risk_m <=0)
			tradeParam_m.risk_m = 0.02;

		tradeParam_m.accountSize_m = GET_ALG_TRADE_DOUBLE(descriptor.initialAlgBrkParams_m, "ACCOUNT_SIZE");
		if (tradeParam_m.accountSize_m <=0)
			tradeParam_m.accountSize_m = 10000;

		tradeParam_m.tradePeriodMinutes_m = GET_ALG_TRADE_INT(descriptor.initialAlgBrkParams_m, "TRADE_PERIOD_MINUTES");
		if (tradeParam_m.tradePeriodMinutes_m <=0)
			tradeParam_m.tradePeriodMinutes_m = 15;

		tradeParam_m.schift_m = GET_ALG_TRADE_DOUBLE(descriptor.initialAlgBrkParams_m, "SHIFT");

		baseExportPath_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "BASE_EXPORT_PATH");
		// need to export - when new period comes
		baseExportPath_m = baseExportPath_m + "\\" + CppUtils::long2String(tradeParam_m.tradePeriodMinutes_m);
		CppUtils::makeDir(baseExportPath_m);


		// --------------------- 

		// load trading symbols

		CppUtils::String symbolsFileName = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "SYMBOL_FILE");

		if (CppUtils::fileExists(symbolsFileName)) {

			CppUtils::StringSet symbols;
			CppUtils::parseSymbolListFile(symbols, symbolsFileName);

			// init state machine
			initStateMachine(symbols);
		}
		else
			THROW(CppUtils::OperationFailed, "exc_SymbolFileNotExists", "ctx_onLibraryInitialized", symbolsFileName );

		// connect to broker
		if (!brokerConnect_m) {
			brokerConnect_m = new BrkLib::BrokerConnect(brokerlib, GERM_2CHANNEL_MM, false, false, connectRunName_m.c_str(), comment.c_str());
			brokerConnect_m->connect();
		}

		if (!brokerConnect_m->getSession().isConnected()) {
			LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  GERM_2CHANNEL_MM, "onLibraryInitialized(...) session cannot be established: " << brokerConnect_m->getSession().getConnection());
			return;
		}

		if (tradeParam_m.isInvalid())
			THROW(CppUtils::OperationFailed, "exc_TradeParamsInvalid", "ctx_onLibraryInitialized", tradeParam_m.toString() );

		LOG_COMMON(connectRunName_m, rtDataProvider_m, BrkLib::AP_MEDIUM, "Trading parameters: " << tradeParam_m.toString());

	}

	void Grm2ChannelMM::onLibraryFreed()
	{
		deinitStateMachine();

		if (brokerConnect_m) {
			delete brokerConnect_m;
			brokerConnect_m = NULL;
		}
	}

	void Grm2ChannelMM::onBrokerResponseArrived(
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
		)
	{

	    

		if (bresponse.type_m == BrkLib::RT_BrokerResponseOrder) {
			// responses - use very simple technique
			BrkLib::BrokerResponseOrder const& bRespOrder = (BrkLib::BrokerResponseOrder const&)bresponse;

			if (bRespOrder.resultCode_m==BrkLib::PT_ORDER_ESTABLISHED ) {

				if (bRespOrder.resultDetailCode_m & BrkLib::ODR_ORDER_INSTALLED) {

				}
				else if (bRespOrder.resultDetailCode_m & BrkLib::ODR_ORDER_EXECUTED) {
					if (bRespOrder.opRemainingVolume_m == 0) {
						// assume this is executed
						if (bRespOrder.opOrderType_m== BrkLib::POS_BUY) {


							// allow trading
							//TE_Close_Long_Signal - will have all data in TE_Close_Long_Signal
							TE_Broker_Confirm_Buy event_i(bRespOrder.brokerOrderID_m);
							processEvent<TE_Broker_Confirm_Buy,TE_Close_Long_Signal>(bRespOrder.symbol_m, event_i);

						}

						if (bRespOrder.opOrderType_m== BrkLib::POS_SELL) {

							// allow trading
							TE_Broker_Confirm_Sell event_i(bRespOrder.brokerOrderID_m);
							processEvent<TE_Broker_Confirm_Sell,TE_Close_Short_Signal>(bRespOrder.symbol_m, event_i);

						}

						if (bRespOrder.opOrderType_m== BrkLib::POS_CLOSE_LONG) {


							// allow trading
							TE_Broker_Confirm_Close_Long event_i;
							processEvent<TE_Broker_Confirm_Close_Long>(bRespOrder.symbol_m, event_i);

						}

						if (bRespOrder.opOrderType_m== BrkLib::POS_CLOSE_SHORT) {

							// allow trading
							TE_Broker_Confirm_Close_Short event_i;
							processEvent<TE_Broker_Confirm_Close_Short>(bRespOrder.symbol_m, event_i);

						}

					}

				}

			}
			else if (bRespOrder.resultCode_m==BrkLib::PT_ORDER_CANCELLED) {
			}
			else if (bRespOrder.resultCode_m==BrkLib::PT_GENERAL_ERROR_HAPPENED) {
				if (bRespOrder.opOrderType_m== BrkLib::POS_BUY) {


					// allow trading
					TE_Broker_Error_Buy event_i;
					processEvent<TE_Broker_Error_Buy>(bRespOrder.symbol_m, event_i);

				}

				if (bRespOrder.opOrderType_m== BrkLib::POS_SELL) {

					// allow trading
					TE_Broker_Error_Sell event_i;
					processEvent<TE_Broker_Error_Sell>(bRespOrder.symbol_m, event_i);

				}

				if (bRespOrder.opOrderType_m== BrkLib::POS_CLOSE_LONG) {


					// allow trading
					TE_Broker_Error_Close_Long event_i;
					processEvent<TE_Broker_Error_Close_Long>(bRespOrder.symbol_m, event_i);

				}

				if (bRespOrder.opOrderType_m== BrkLib::POS_CLOSE_SHORT) {

					// allow trading
					TE_Broker_Error_Close_Short event_i;
					processEvent<TE_Broker_Error_Close_Short>(bRespOrder.symbol_m, event_i);

				}

			}

		}
	}

	void Grm2ChannelMM::onDataArrived(
		HlpStruct::RtData const& rtdata, 
		Inqueue::ContextAccessor& historyaccessor, 
		HlpStruct::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
		)
	{
			

		// cache bar data

		if (rtdata.getProvider2() == barDataProvider_m) {
			int cache_symbol_id = historyaccessor.cacheSymbol(rtdata.getProvider2(), rtdata.getSymbol2(), HlpStruct::AP_Minute, tradeParam_m.tradePeriodMinutes_m);

			// need to recalculate max & mins
			if (cache_symbol_id <= 0) {
				LOGEVENT(HlpStruct::CommonLog::LL_ERROR, GERM_2CHANNEL_MM, "Cannot cache symbol: " << rtdata.getSymbol2());
				return;
			}

			// get last unclosed symbol
			HlpStruct::PriceData const& unclosed = historyaccessor.getUnclosedCandle(cache_symbol_id);


			if (unclosed.isNewPeriod()) {

				// 
				// -------------------------------------
				// last
				Inqueue::DataPtr ptr= historyaccessor.dataLast( cache_symbol_id );
				historyaccessor.dataPrev(cache_symbol_id,ptr );


				if (!ptr.isValid())
					return;

				// last candle
				HlpStruct::PriceData const& pdata = historyaccessor.getCurrentPriceData(cache_symbol_id, ptr);


				// here we perform calculations
				Math::MinMaxChannel<Math::PriceSeriesProvider> channel(tradeParam_m.period_m);
				Math::MinMaxChannel<Math::PriceSeriesProvider>::Result res_channel = historyaccessor.callMathEngine<Math::MinMaxChannel<Math::PriceSeriesProvider>::Result, Math::P_Bid, Math::MinMaxChannel<Math::PriceSeriesProvider> >(cache_symbol_id, channel);


				Math::MinMaxChannel<Math::PriceSeriesProvider> stop_channel(tradeParam_m.periodStop_m);
				Math::MinMaxChannel<Math::PriceSeriesProvider>::Result res_channel_stop = historyaccessor.callMathEngine<Math::MinMaxChannel<Math::PriceSeriesProvider>::Result, Math::P_Bid, Math::MinMaxChannel<Math::PriceSeriesProvider> >(cache_symbol_id, stop_channel);

				if (res_channel_stop.lower_m <=0 || res_channel_stop.upper_m<=0) {
					return;
				}

				if (res_channel.lower_m <=0 || res_channel.upper_m<=0) {
					return;
				}

				// have 2 channels
				SymbolContext new_ctx;
				new_ctx.highChannel_m = res_channel.upper_m;
				new_ctx.lowChannel_m =  res_channel.lower_m;

				new_ctx.highStopChannel_m = res_channel_stop.upper_m + tradeParam_m.schift_m;
				new_ctx.lowStopChannel_m = res_channel_stop.lower_m - tradeParam_m.schift_m ;
				new_ctx.time_m = pdata.time_m;




				// log what we have
				DRAWOBJ_LINEINDIC_7(connectRunName_m, pdata.time_m, GERM_2CHANNEL_MM, rtdata.getSymbol2(), rtDataProvider_m, 
					"Low", new_ctx.lowChannel_m, 1, 
					"High", new_ctx.highChannel_m, 2, 
					"Stop Low", new_ctx.lowStopChannel_m, 3, 
					"Stop High", new_ctx.highStopChannel_m, 4,
					"High Price", pdata.high2_m, 5,
					"Low Price", pdata.low2_m, 6,
					"Close Price", pdata.close2_m, 7
					);



				map<CppUtils::String, SymbolContext>::const_iterator it = contextMap_m.find( rtdata.getSymbol2() );
				if (it != contextMap_m.end()) {
					// found
					SymbolContext const& old_ctx = it->second;

					// here is need to check for open signals
					if (Math::GetPrice2<Math::P_Bid>::getClosePrice(pdata) > old_ctx.highChannel_m) {
						// long signal enter
						
						// allow trading
						double tradeVolume = calculateTradingVolume(old_ctx.highChannel_m, old_ctx.lowChannel_m);

						if (allowTrading_m == TDLONG || allowTrading_m== TDBOTH) {
							TE_Enter_Long_Signal event_i(rtdata.getSymbol2(), tradeVolume);
							processEvent<TE_Enter_Long_Signal, TE_Close_Long_Signal>(rtdata.getSymbol2(), event_i);
						}
						

						LOG_SYMBOL(connectRunName_m,rtDataProvider_m, rtdata.getSymbol2(),
							"Long signal, previous [" << CppUtils::getGmtTime2(old_ctx.time_m) << 
							"] high channel: " << old_ctx.highChannel_m << 
							" current [" << CppUtils::getGmtTime2(pdata.time_m) << "] close: " <<
							Math::GetPrice2<Math::P_Bid>::getClosePrice(pdata)  );
					}

					if (Math::GetPrice2<Math::P_Bid>::getClosePrice(pdata) < old_ctx.lowChannel_m) {

						
						// allow trading
						double tradeVolume = calculateTradingVolume(old_ctx.highChannel_m, old_ctx.lowChannel_m);

						if (allowTrading_m == TDBOTH || allowTrading_m== TDSHORT) {
							TE_Enter_Short_Signal event_i(rtdata.getSymbol2(), tradeVolume);
							processEvent<TE_Enter_Short_Signal, TE_Close_Short_Signal>(rtdata.getSymbol2(), event_i);
						}
						


						LOG_SYMBOL(connectRunName_m,rtDataProvider_m, rtdata.getSymbol2(),
							"Short signal, previous [" << CppUtils::getGmtTime2(old_ctx.time_m) << 
							"] high channel: " << old_ctx.lowChannel_m << 
							" current [" << CppUtils::getGmtTime2(pdata.time_m) << "] close: " <<
							Math::GetPrice2<Math::P_Bid>::getClosePrice(pdata)  );
					}
				}

				contextMap_m[rtdata.getSymbol2()] = new_ctx;

				// at this stage we can export
				exportSymbol(historyaccessor, rtdata.getSymbol2(), cache_symbol_id );
				
			

			}






		} // bar data provider


		if (rtdata.getProvider2() == rtDataProvider_m) {
			// ticks

			map<CppUtils::String, SymbolContext>::const_iterator it = contextMap_m.find( rtdata.getSymbol2() );
			if (it != contextMap_m.end()) {
				// found
				SymbolContext const& old_ctx = it->second;

				// here is need to check for open signals
				if (rtdata.getRtData().bid_m > old_ctx.highStopChannel_m) {
					
					// allow trading
					TE_Close_Short_Signal event_i(rtdata.getSymbol2());
					processEvent<TE_Close_Short_Signal>(rtdata.getSymbol2(), event_i);
					

					LOG_SYMBOL(connectRunName_m,rtDataProvider_m, rtdata.getSymbol2(),
						"Close short signal, previous [" << CppUtils::getGmtTime2(old_ctx.time_m) << 
						"] high stop channel: " << old_ctx.highChannel_m << 
						" current [" << CppUtils::getGmtTime2(rtdata.getTime()) << "] price: " <<
						rtdata.getRtData().bid_m  );
				}

				if (rtdata.getRtData().bid_m < old_ctx.lowStopChannel_m) {

					
						
					// allow trading
					TE_Close_Long_Signal event_i(rtdata.getSymbol2());
					processEvent<TE_Close_Long_Signal>(rtdata.getSymbol2(), event_i);
					

					LOG_SYMBOL(connectRunName_m,rtDataProvider_m, rtdata.getSymbol2(),
						"Close long signal, previous [" << CppUtils::getGmtTime2(old_ctx.time_m) << 
						"] low stop channel: " << old_ctx.lowStopChannel_m << 
						" current [" << CppUtils::getGmtTime2(rtdata.getTime()) << "] price: " <<
						rtdata.getRtData().bid_m  );
				}
			}

		}


	}

	void Grm2ChannelMM::onRtProviderSynchEvent (
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
			bool result;
			CppUtils::String log;
			contextaccessor.requestRunExternalPackage("german_starter.jar", true, result, log);

		}
		else if (synchFlg==HlpStruct::RtData::SYNC_EVENT_PROVIDER_TRANSMIT_ERROR) {
			LOG_COMMON(connectRunName_m,rtDataProvider_m,BrkLib::AP_HIGH,"RT Provider transmit error: " << providerName << " - " << message);

			
		}
		else if (synchFlg==HlpStruct::RtData::SYNC_EVENT_CUSTOM_MESSAGE) {
			LOG_COMMON(connectRunName_m,rtDataProvider_m,BrkLib::AP_HIGH,"RT Provider custom message: " << providerName << " - " << message);
		}
	}

	void Grm2ChannelMM::onLevel2DataArrived(
		HlpStruct::RtLevel2Data const& level2data,
		Inqueue::ContextAccessor& historyaccessor, 
		HlpStruct::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
		)
	{

	}

	void Grm2ChannelMM::onLevel1DataArrived(
		HlpStruct::RtLevel1Data const& level1data,
		Inqueue::ContextAccessor& historyaccessor, 
		HlpStruct::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
		)
	{

	}

	void Grm2ChannelMM::onEventArrived(
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
					// n/a
		}
		else if (command=="dump_symbol") {
			CppUtils::String const& symbol = xmlparam_data.getParameter("symbol").getAsString();

			exportSymbol(contextaccessor, symbol);

			LOG_COMMON(connectRunName_m,rtDataProvider_m, BrkLib::AP_MEDIUM, "Symbol dumped");
		}  
		else if (command=="start_trade") {

			allowTrading_m = TDBOTH;
			if ( xmlparam_data.isKeyExists("direction")) {
				CppUtils::String  direction = xmlparam_data.getParameter("direction").getAsString();
				
				CppUtils::toupper( direction );
				if (direction == "SHORT") {
					allowTrading_m = TDSHORT;
					LOG_COMMON(connectRunName_m,rtDataProvider_m, BrkLib::AP_MEDIUM, "Allowed SHORT broker operations");
				}
				else if (direction == "LONG") {
					allowTrading_m = TDLONG;
					LOG_COMMON(connectRunName_m,rtDataProvider_m, BrkLib::AP_MEDIUM, "Allowed LONG broker operations");
				}
				else if (direction == "BOTH") {
					allowTrading_m = TDBOTH;
					LOG_COMMON(connectRunName_m,rtDataProvider_m, BrkLib::AP_MEDIUM, "Allowed LONG/SHORT broker operations");
				}
				
			}
			else	{
				allowTrading_m = TDBOTH;
				LOG_COMMON(connectRunName_m,rtDataProvider_m, BrkLib::AP_MEDIUM, "Allowed LONG/SHORT broker operations");
			}

			
			
			
		}
		else if (command=="stop_trade") {
			allowTrading_m = TDNONE;
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
			processEvent<TE_Manual_Fix>(xmlparam_data.getParameter("symbol").getAsString(), event_i);
		}
		else if (command=="buy_signal") {
			CppUtils::String const& symbol = xmlparam_data.getParameter("symbol").getAsString();
			TE_Enter_Long_Signal event_i(
				symbol, 
				CppUtils::string2Float(xmlparam_data.getParameter("volume").getAsString().c_str())
			);
			processEvent<TE_Enter_Long_Signal, TE_Close_Long_Signal>(symbol, event_i);
		}
		else if (command=="sell_signal") {
			CppUtils::String const& symbol = xmlparam_data.getParameter("symbol").getAsString();
			TE_Enter_Short_Signal event_i(
				symbol, 
				CppUtils::string2Float(xmlparam_data.getParameter("volume").getAsString().c_str())
			);
			processEvent<TE_Enter_Short_Signal, TE_Close_Short_Signal>(symbol, event_i);
		}
		else if (command=="close_long_signal") {
			CppUtils::String const& symbol = xmlparam_data.getParameter("symbol").getAsString();
			TE_Close_Long_Signal event_i(symbol);
			processEvent<TE_Close_Long_Signal>(symbol, event_i);
		}
		else if (command=="close_short_signal") {
			CppUtils::String const& symbol = xmlparam_data.getParameter("symbol").getAsString();
			TE_Close_Short_Signal event_i(symbol);
			processEvent<TE_Close_Short_Signal>(symbol, event_i);
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

	void Grm2ChannelMM::onThreadStarted(
		Inqueue::ContextAccessor& contextaccessor, bool const firstLib, bool const lastLib
		)
	{
	}

	void Grm2ChannelMM::onThreadFinished(
		Inqueue::ContextAccessor& contextaccessor, HlpStruct::TradingParameters const& tradeParams, bool const firstLib, bool const lastLib
		)
	{

	}

	CppUtils::String const& Grm2ChannelMM::getName() const
	{
		static const CppUtils::String name("[ alg lib: Grm2ChannelMM ]");
		return name;
	}

	// ---------------------------------
	// Helpers

	void Grm2ChannelMM::initStateMachine(CppUtils::StringSet const& symbols)
	{
		LOCK_FOR_SCOPE(stateMachineMtx_m);


		for(CppUtils::StringSet::const_iterator it = symbols.begin(); it != symbols.end(); it++) {
			MachineContext::MachineContextStructure ctx;
			ctx.base_m = this;
			ctx.symbol_m = *it;

			tradingMachines_m[*it] = new CppUtils::StateMachine<MachineContext>(&ctx);

			LOG_SYMBOL(getRunName(),getRtdataProvider(), *it, "State machine created");
		}

		for(map<CppUtils::String, CppUtils::StateMachine<MachineContext>* >::iterator it = tradingMachines_m.begin(); it != tradingMachines_m.end(); it++) {

			
			// register states
			CppUtils::BaseState<MachineContext> *s_0 = it->second->add< CppUtils::InitialState<MachineContext> >(  );
			CppUtils::BaseState<MachineContext> *s_TS_Initial = it->second->add<TS_Initial>(  );
			CppUtils::BaseState<MachineContext> *s_TS_Inconsistent = it->second->add<TS_Inconsistent>(  );
			CppUtils::BaseState<MachineContext> *s_TS_IssueBuy =  it->second->add<TS_IssueBuy>(  );
			CppUtils::BaseState<MachineContext> *s_TS_IssueSell = it->second->add<TS_IssueSell>(  );
			CppUtils::BaseState<MachineContext> *s_TS_Long = it->second->add<TS_Long>(  );
			CppUtils::BaseState<MachineContext> *s_TS_Short = it->second->add<TS_Short>(  );
			CppUtils::BaseState<MachineContext> *s_TS_IssueCloseLong = it->second->add<TS_IssueCloseLong>(  );
			CppUtils::BaseState<MachineContext> *s_TS_IssueCloseShort =  it->second->add<TS_IssueCloseShort>(  );
			CppUtils::BaseState<MachineContext> *s_TS_If_Reverse_To_Short = it->second->add<TS_If_Reverse_To_Short>(  );
			CppUtils::BaseState<MachineContext> *s_TS_If_Reverse_To_Long = it->second->add<TS_If_Reverse_To_Long>(  );

			// matrix
			s_0->connect(s_TS_Initial, CppUtils::Unconditional());
			s_TS_Initial->connect<TE_Enter_Long_Signal>(s_TS_IssueBuy);
			s_TS_Initial->connect<TE_Enter_Short_Signal>(s_TS_IssueSell);

			s_TS_IssueBuy->connect<TE_Broker_Confirm_Buy>(s_TS_Long);
			s_TS_IssueSell->connect<TE_Broker_Confirm_Sell>(s_TS_Short);

			s_TS_IssueBuy->connect<TE_Broker_Error_Buy>(s_TS_Inconsistent);
			s_TS_IssueSell->connect<TE_Broker_Error_Sell>(s_TS_Inconsistent);

			s_TS_Long->connect<TE_Close_Long_Signal>(s_TS_IssueCloseLong);
			s_TS_Short->connect<TE_Close_Short_Signal>(s_TS_IssueCloseShort);

			s_TS_IssueCloseLong->connect<TE_Broker_Confirm_Close_Long>(s_TS_If_Reverse_To_Short);
			s_TS_IssueCloseShort->connect<TE_Broker_Confirm_Close_Short>(s_TS_If_Reverse_To_Long);


			s_TS_IssueCloseLong->connect<TE_Broker_Error_Close_Long>(s_TS_Inconsistent);
			s_TS_IssueCloseShort->connect<TE_Broker_Error_Close_Short>(s_TS_Inconsistent);


			// if reverse
			s_TS_If_Reverse_To_Long->connect(s_TS_IssueBuy, mem_fun(&MachineContext::isReverse));
			s_TS_If_Reverse_To_Short->connect(s_TS_IssueSell,  mem_fun(&MachineContext::isReverse));

			// if to initial
			s_TS_If_Reverse_To_Long->connect(s_TS_Initial, not1(mem_fun(&MachineContext::isReverse)));
			s_TS_If_Reverse_To_Short->connect(s_TS_Initial, not1(mem_fun(&MachineContext::isReverse)));

			// manual fix
			s_TS_Inconsistent->connect<TE_Manual_Fix>(s_TS_Initial);

			// initialize
			it->second->init(); 


		}

 

	}

	void Grm2ChannelMM::deinitStateMachine()
	{
		LOCK_FOR_SCOPE(stateMachineMtx_m);

		for(map<CppUtils::String, CppUtils::StateMachine<MachineContext>* >::iterator it = tradingMachines_m.begin(); it != tradingMachines_m.end();it++)
		{
			delete it->second;
		}

		tradingMachines_m.clear();

	}
	
	double Grm2ChannelMM::calculateTradingVolume(
		double const& highChannel, 
		double const& lowChannel
	)
	{
		HTASSERT(!tradeParam_m.isInvalid());

		double result= tradeParam_m.accountSize_m*tradeParam_m.koeff_m * (tradeParam_m.risk_m /(abs(highChannel - lowChannel)+tradeParam_m.schift_m));
		if (result > tradeParam_m.maxLots_m)
			result = tradeParam_m.maxLots_m;

		if (result < tradeParam_m.minLots_m)
			result = tradeParam_m.minLots_m;


		return result;
	}




	template<class E_TYPE>
	void Grm2ChannelMM::processEvent(CppUtils::String const& symbol, CppUtils::BaseEvent& pev)
	{
		LOCK_FOR_SCOPE(stateMachineMtx_m);

		if ( getAlgHolder().getBroker() == NULL) 
			return;
		
		map<CppUtils::String, CppUtils::StateMachine<MachineContext>* >::iterator it = tradingMachines_m.find(symbol);

		if (it != tradingMachines_m.end()) {
			CppUtils::StateMachine<MachineContext>& machine = *it->second;
			
			machine.do_full_process<E_TYPE>(pev);
			
		}
	}

	template<class E_TYPE, class E_TARGET_TYPE>
	void Grm2ChannelMM::processEvent(CppUtils::String const& symbol, CppUtils::BaseEvent& pev)
	{
		LOCK_FOR_SCOPE(stateMachineMtx_m);

		if ( getAlgHolder().getBroker() == NULL) 
			return;
		
		map<CppUtils::String, CppUtils::StateMachine<MachineContext>* >::iterator it = tradingMachines_m.find(symbol);

		if (it != tradingMachines_m.end()) {
			CppUtils::StateMachine<MachineContext>& machine = *it->second;
			
			machine.register_event_to_recipient<E_TYPE, E_TARGET_TYPE>(pev);
			
			machine.do_full_process<E_TYPE>(pev);
			
		}
	}

	void Grm2ChannelMM::issueBuyOrder(CppUtils::String const& symbol,double const& volume)
	{
		BrkLib::BrokerOperation oper(*brokerConnect_m);
		BrkLib::Order order;

		order.ID_m.generate();
		order.symbol_m = symbol;
		order.orderType_m = BrkLib::OP_BUY;
		order.orderValidity_m = BrkLib::OV_DAY;
		order.orVolume_m = volume;
		
		LOG_SYMBOL(connectRunName_m,rtDataProvider_m, symbol, "Sending buy order volume: " << order.orVolume_m );


		oper.unblockingIssueOrder(order);

	}

	void Grm2ChannelMM::issueSellOrder(CppUtils::String const& symbol,double const& volume)
	{
		BrkLib::BrokerOperation oper(*brokerConnect_m);
		BrkLib::Order order;

		order.ID_m.generate();
		order.symbol_m = symbol;
		order.orderType_m = BrkLib::OP_SELL;
		order.orderValidity_m = BrkLib::OV_DAY;
		order.orVolume_m = volume;

		LOG_SYMBOL(connectRunName_m,rtDataProvider_m, symbol, "Sending sell order volume: " << order.orVolume_m );
		
		oper.unblockingIssueOrder(order);

	}

	void Grm2ChannelMM::issueCloseLongOrder(CppUtils::String const& symbol, double const& volume, CppUtils::String const& brokerOrderId)
	{
		BrkLib::BrokerOperation oper(*brokerConnect_m);
		BrkLib::Order order;

		order.ID_m.generate();
		order.symbol_m = symbol;
		order.orderType_m = BrkLib::OP_CLOSE_LONG;
		order.orderValidity_m = BrkLib::OV_DAY;
		order.brokerOrderID_m = brokerOrderId;
		order.orVolume_m = volume;
		
		LOG_SYMBOL(connectRunName_m,rtDataProvider_m, symbol, "Sending closing order for long position: " << order.brokerOrderID_m << ", volume: " << order.orVolume_m );
		
		oper.unblockingIssueOrder(order);

	}

	void Grm2ChannelMM::issueCloseShortOrder(CppUtils::String const& symbol, double const& volume, CppUtils::String const& brokerOrderId)
	{
		BrkLib::BrokerOperation oper(*brokerConnect_m);
		BrkLib::Order order;

		order.ID_m.generate();
		order.symbol_m = symbol;
		order.orderType_m = BrkLib::OP_CLOSE_SHORT;
		order.orderValidity_m = BrkLib::OV_DAY;
		order.brokerOrderID_m = brokerOrderId;
		order.orVolume_m = volume;

		LOG_SYMBOL(connectRunName_m,rtDataProvider_m, symbol, "Sending closing order for short position: " << order.brokerOrderID_m << ", volume: " << order.orVolume_m );
		
		oper.unblockingIssueOrder(order);

	}

	void Grm2ChannelMM::exportSymbol(Inqueue::ContextAccessor& historyaccessor, CppUtils::String const& symbol, int const cache_id)
	{
			CppUtils::String fileExportMame = baseExportPath_m + "\\"+ symbol + ".txt";
				
			Math::ExporterEngine<Math::PriceSeriesProvider> exporter(fileExportMame, false);
			bool result = historyaccessor.callMathEngine<bool, Math::P_CloseBid, Math::ExporterEngine<Math::PriceSeriesProvider> >(cache_id, exporter);

			if (!result) {
				LOG_SYMBOL_PROBLEM(connectRunName_m,rtDataProvider_m, symbol,"Cannot export symbol to: " << fileExportMame );
			}
	}

	void Grm2ChannelMM::exportSymbol(Inqueue::ContextAccessor& historyaccessor, CppUtils::String const& symbol)
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


} // end of namespace
