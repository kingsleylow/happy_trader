#include "implement.hpp"

// -------------------------------
// constants


#define SENSITIV 2
#define RSI_TRUE_RANGE_PER 15
#define USECLOSE true
// ------------------------------

#define TP_POS_TAG	 2
#define FREE_POS_TAG  3


#define LOG_SYMBOL(RUN_NAME,PROVIDER, L,S,X) {	\
	DRAWOBJ_TEXT(RUN_NAME,CppUtils::getTime(),"SYSTEM 2", S, PROVIDER, L, "LOG", X );	\
}

#define LOG_SIGNAL(RUN_NAME,PROVIDER, L,S,X) {	\
	DRAWOBJ_TEXT(RUN_NAME,CppUtils::getTime(),"SYSTEM 2", S, PROVIDER, L, "SIGNAL", X );	\
}

#define LOG_COMMON(RUN_NAME,PROVIDER, L,X)	{		\
	DRAWOBJ_TEXT(RUN_NAME,CppUtils::getTime(),"SYSTEM 2", "-ALL-", PROVIDER, L, "LOG", X );	\
}


namespace AlgLib {
	AlgorithmBase *creator (Inqueue::AlgorithmHolder* algholder)
	{
		return new CSystem2(*algholder);
	}

	void terminator (AlgorithmBase *pthis)
	{
		assert(pthis);
		delete pthis;
	}

	AlgorithmBase::CreatorAlgorithmFun createAlgorithm = creator;

	AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm = terminator;

	// intreface to be implemented
	CSystem2::CSystem2( Inqueue::AlgorithmHolder& algHolder):
	AlgorithmBase(algHolder)
	{

	}

	CSystem2::~CSystem2()
	{

	}

	void CSystem2::onLibraryInitialized(
		Inqueue::AlgBrokerDescriptor const& descriptor, 
		BrkLib::BrokerBase* brokerlib, 
		CppUtils::String const& runName,
		CppUtils::String const& comment
		)
	{
		internalRun_name_m = runName;
		readInitialParameters(descriptor);
	}

	void CSystem2::onLibraryFreed()
	{
		LOG_SIGNAL(internalRun_name_m, commonparams_m.providerForRTSymbols_m, BrkLib::AP_CRITICAL, "DUMMY",
			"Finishing..."		 
			);
	}

	void CSystem2::onBrokerResponseArrived(
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
		)
	{


		if (bresponse.type_m==BrkLib::RT_BrokerResponseOrder) {
			BrkLib::BrokerResponseOrder const& bRespOrder = (BrkLib::BrokerResponseOrder const&)bresponse;

			if (bRespOrder.provider_m != commonparams_m.providerForRTSymbols_m) 
				return;

			map<CppUtils::String, SymbolContext>::iterator it = symbolContextMap_m.find(bRespOrder.symbolNum_m);


			if (it == symbolContextMap_m.end()) 
				return;


			SymbolContext& smbcontext = it->second;



			if (bRespOrder.resultCode_m == BrkLib::PT_ORDER_ESTABLISHED) {
				if (bRespOrder.positionType_m == BrkLib::POS_BUY) {
				
					if (bRespOrder.tag_m == TP_POS_TAG) {
						smbcontext.openPosId1_m = bRespOrder.brokerPositionID_m;
						
					}
					else if (bRespOrder.tag_m == FREE_POS_TAG) {
						smbcontext.openPosId2_m = bRespOrder.brokerPositionID_m;
					}
					else
						THROW(CppUtils::OperationFailed, "msg_InvalidPosTag", "ctx_onBrokerRespArrived_OrderBuyEstablished", "" );
					
					
					


					
				}
				else if (bRespOrder.positionType_m == BrkLib::POS_SELL) {
					
					if (bRespOrder.tag_m == TP_POS_TAG) {
						smbcontext.openPosId1_m = bRespOrder.brokerPositionID_m;

						
					}
					else if (bRespOrder.tag_m == FREE_POS_TAG) {
						smbcontext.openPosId2_m = bRespOrder.brokerPositionID_m;
					}
					else
						THROW(CppUtils::OperationFailed, "msg_InvalidPosTag", "ctx_onBrokerRespArrived_OrderSellEstablished", "" );

					// short pos

				}

			}
			


		}	
	}

	void CSystem2::onDataArrived(
		HlpStruct::RtData const& rtdata, 
		Inqueue::ContextAccessor& historyaccessor, 
		Inqueue::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
		)
	{ 

		// data context
		if (rtdata.getProvider() != commonparams_m.providerForRTSymbols_m) 
			return;

		map<CppUtils::String, SymbolContext>::iterator it = symbolContextMap_m.find(rtdata.getSymbol());


		if (it == symbolContextMap_m.end()) 
			return;


		SymbolContext& smbcontext = it->second;

		if (!commonparams_m.isWithinWorkingTime(rtdata.getTime())) 
			return;

		int cache_symbol_id = historyaccessor.cacheSymbolWithDateFilteringHours(
			commonparams_m.hoursBegin_m*100+commonparams_m.minutesBegin_m,
			commonparams_m.hourEnd_m *100 + commonparams_m.minutesEnd_m,
			rtdata.getProvider2(), rtdata.getSymbol2(), Inqueue::AP_Minute, 5);

		if (cache_symbol_id <0 ) 
			return;


		// 
		// get last unclosed symbol
		HlpStruct::PriceData const& unclosed = historyaccessor.getUnclosedCandle(cache_symbol_id);

		if (unclosed.isNewPeriod()) {

			Inqueue::DataPtr ptr= historyaccessor.dataLast( cache_symbol_id );
			historyaccessor.dataPrev(cache_symbol_id,ptr );


			if (!ptr.isValid())
				return;

			// last candle
			HlpStruct::PriceData const& pdata = historyaccessor.getCurrentPriceData(cache_symbol_id, ptr);



			// export (temprartly debug mode)

			Math::ExporterEngine<Math::PriceSeriesProvider> exporter(smbcontext.exportPath_m, false);
			historyaccessor.callMathEngine<Math::P_CloseBid, Math::ExporterEngine<Math::PriceSeriesProvider> >(cache_symbol_id, exporter);


			// executed each new period
			if (!smbcontext.initialized_m) {
				smbcontext.initialized_m = true;

				// initial values

				smbcontext.SDR_m = 0;
				smbcontext.ADR_m = pdata.high2_m - pdata.low2_m;
				smbcontext.ADM_m = smbcontext.ADR_m /2.0;

				smbcontext.rsiSeries_m.clear();

			}
			else {
				// we are sure that we have at least 2 bars

				Math::TrueRangeEngine<Math::PriceSeriesProvider> truerange;
				double TR = historyaccessor.callMathEngine<Math::P_CloseBid, Math::TrueRangeEngine<Math::PriceSeriesProvider> >(cache_symbol_id, truerange);
				smbcontext.ADR_m = smbcontext.ADR_m + 0.13*(TR - smbcontext.ADR_m);  // low frequency IIR filter of truerange


				double M = (pdata.high2_m + pdata.low2_m) / 2.0;
				smbcontext.ADM_m = smbcontext.ADM_m + 0.4*(M - smbcontext.ADM_m); // low frequency IIR filter of avarage price (momentum ???)

				HlpStruct::PriceData pdata_rsi;
				pdata_rsi.close2_m = TR;
				pdata_rsi.close_m = TR;

								
				Math::STDEngine<Math::ArbitrarySeriesProvider> stdengine(RSI_TRUE_RANGE_PER);
				Math::ArbitrarySeriesProvider::appendToPriceList(smbcontext.rsiSeries_m, pdata_rsi, stdengine.returnNecessaryCacheSize());


				smbcontext.SDR_m = historyaccessor.callMathEngineWithArbitrarySeries<Math::P_CloseBid, Math::STDEngine<Math::ArbitrarySeriesProvider> >( stdengine, smbcontext.rsiSeries_m );
				
				if (smbcontext.SDR_m != Math::RESULT_INVALID) {
					
					int sig_s =0;

					
					// prev data 
					HlpStruct::PriceData const& pdata_1 =historyaccessor.getBackwardPriceData(cache_symbol_id, ptr, 1);

					// continue here
					double stopincr =  smbcontext.ADR_m/2.0 + smbcontext.SDR_m*(double)SENSITIV; 

					//  
   

					if ((smbcontext.UPDN_m == 1 && smbcontext.UPDN_PREV_m == -1)  ||  smbcontext.UPDN_m == -1 ) {

						smbcontext.LONGSTOP_m = smbcontext.ADM_m - stopincr;
					} else {
						// io a anee ia oae, oi aeou eaai aaa?o oi oieuei iia?eiaai oieuei aaa?o
						smbcontext.LONGSTOP_m = max(smbcontext.LONGSTOP_m, smbcontext.ADM_m - stopincr);
					}

					if ((smbcontext.UPDN_m == -1 && smbcontext.UPDN_PREV_m == 1)  ||  smbcontext.UPDN_m == 1 ) {

						smbcontext.SHORTSTOP_m = smbcontext.ADM_m + stopincr;
					} else {
						smbcontext.SHORTSTOP_m = min(smbcontext.SHORTSTOP_m, smbcontext.ADM_m + stopincr);
					}
					//
					if (smbcontext.LONGSTOP_PREV_m > 0 && smbcontext.SHORTSTOP_PREV_m > 0) {

						/*
						if (smbcontext.UPDN_m == -1) {


						// UP CHECK
						if (
						( !USECLOSE && Math::GetPrice2<Math::P_CloseBid>::getHighPrice(pdata) > smbcontext.SHORTSTOP_m && Math::GetPrice2<Math::P_CloseBid>::getHighPrice(pdata_1) <= smbcontext.SHORTSTOP_PREV_m) ||
						(USECLOSE && Math::GetPrice2<Math::P_CloseBid>::getClosePrice(pdata) > smbcontext.SHORTSTOP_m && Math::GetPrice2<Math::P_CloseBid>::getClosePrice(pdata_1) <= smbcontext.SHORTSTOP_PREV_m )
						)
						{


						smbcontext.UPDN_m = 1;
						// signal up
						} else {
						}
						}


						// DOWN CHECK
						//
						if (smbcontext.UPDN_m == 1) {


						// oaia aueaoaao aiec ca LONGSTOP eeie?
						if (
						( !USECLOSE && Math::GetPrice2<Math::P_CloseBid>::getLowPrice(pdata) < smbcontext.LONGSTOP_m && Math::GetPrice2<Math::P_CloseBid>::getLowPrice(pdata_1) >= smbcontext.LONGSTOP_PREV_m) ||
						(USECLOSE && Math::GetPrice2<Math::P_CloseBid>::getClosePrice(pdata) < smbcontext.LONGSTOP_m && Math::GetPrice2<Math::P_CloseBid>::getClosePrice(pdata_1) >= smbcontext.LONGSTOP_PREV_m )
						)
						{

						smbcontext.UPDN_m = -1;
						// SIGNAL DOWN
						} else {

						}
						}

						// 
						*/





						if (smbcontext.UPDN_m==-1) {
							if (
								( !USECLOSE && Math::GetPrice2<Math::P_CloseBid>::getHighPrice(pdata) > smbcontext.SHORTSTOP_m) ||
								(USECLOSE && Math::GetPrice2<Math::P_CloseBid>::getClosePrice(pdata) > smbcontext.SHORTSTOP_m )
								) 
							{
								smbcontext.UPDN_m = 1;
								// SIGNAL UP, OPEN LONG POS
									
								LOG_SIGNAL(internalRun_name_m, commonparams_m.providerForRTSymbols_m, BrkLib::AP_CRITICAL,rtdata.getSymbol(),
									"TICK - For symbol: " << rtdata.getSymbol() << "[ " << CppUtils::getGmtTime(rtdata.getTime())<< 
									" LONG SIGNAL" 
								);

								sig_s = 1;

								// long pos
								double tpPrice =rtdata.getRtData().ask_m + ( rtdata.getRtData().ask_m - smbcontext.LONGSTOP_m);

								
								
								closePos(brokerlib, runName, comment, rtdata.getProvider2(), rtdata.getSymbol2(), rtdata.getTime());

								openLongPos(brokerlib, runName, comment, rtdata.getProvider2(), rtdata.getSymbol2(), rtdata.getTime(), TP_POS_TAG, tpPrice);
								openLongPos(brokerlib, runName, comment, rtdata.getProvider2(), rtdata.getSymbol2(), rtdata.getTime(), FREE_POS_TAG, -1);

							}
						}

						//
						if (smbcontext.UPDN_m==1) {
							if (
								( !USECLOSE && Math::GetPrice2<Math::P_CloseBid>::getLowPrice(pdata) < smbcontext.LONGSTOP_m) ||
								(USECLOSE && Math::GetPrice2<Math::P_CloseBid>::getClosePrice(pdata) < smbcontext.LONGSTOP_m )
								) 
							{
								smbcontext.UPDN_m = -1;
								// SIGNAL DOWN
								LOG_SIGNAL(internalRun_name_m, commonparams_m.providerForRTSymbols_m, BrkLib::AP_CRITICAL,rtdata.getSymbol(),
									"TICK - For symbol: " << rtdata.getSymbol() << "[ " << CppUtils::getGmtTime(rtdata.getTime())<< 
									" SHORT SIGNAL" 
								);

								sig_s = -1;

								// short pos
								double tpPrice = rtdata.getRtData().bid_m - ( smbcontext.SHORTSTOP_m - rtdata.getRtData().bid_m);

								
								
								closePos(brokerlib, runName, comment, rtdata.getProvider2(), rtdata.getSymbol2(), rtdata.getTime());

								openShortPos(brokerlib, runName, comment, rtdata.getProvider2(), rtdata.getSymbol2(), rtdata.getTime(),TP_POS_TAG, tpPrice);
								openShortPos(brokerlib, runName, comment, rtdata.getProvider2(), rtdata.getSymbol2(), rtdata.getTime(),FREE_POS_TAG, -1);
							}
						}

					} // end block with signal check

					
					DRAWOBJ_LINEINDIC_7(internalRun_name_m, pdata.time_m, "MIG_INDIC_2", rtdata.getSymbol2(), rtdata.getProvider2(), 
						"LONGSTOP", smbcontext.LONGSTOP_m, 0, 
						"SHORTSTOP", smbcontext.SHORTSTOP_m, 1,
						"TRUE RANGE", TR, 2,
						"SDR", smbcontext.SDR_m, 3,  
						"ADM", smbcontext.ADM_m, 4, 
						"ADR", smbcontext.ADR_m, 5, 
						"SIGNAL", sig_s, 6
					); 


					//
					smbcontext.LONGSTOP_PREV_m = smbcontext.LONGSTOP_m;
					smbcontext.SHORTSTOP_PREV_m = smbcontext.SHORTSTOP_m;
					smbcontext.UPDN_PREV_m = smbcontext.UPDN_m;

				}  


			}
		}


	}

	void CSystem2::onRtProviderSynchEvent (
		Inqueue::ContextAccessor& contextaccessor, HlpStruct::RtData::SynchType const synchFlg,
		double const& synchTime,
		CppUtils::String const& providerName,
		CppUtils::String const& message
		)
	{
		// notifications
		if (HlpStruct::RtData::SYNC_EVENT_PROVIDER_TRANSMIT_ERROR == synchFlg) {
			LOG_SIGNAL(internalRun_name_m, commonparams_m.providerForRTSymbols_m, BrkLib::AP_CRITICAL, "TRANSMIT STATUS", "Transmition failure" );
		}
		else if (HlpStruct::RtData::SYNC_EVENT_PROVIDER_START == synchFlg) {
			LOG_SIGNAL(internalRun_name_m, commonparams_m.providerForRTSymbols_m, BrkLib::AP_CRITICAL, "TRANSMIT STATUS", "Transmition OK" );
		}
	}

	void CSystem2::onLevel2DataArrived(
		HlpStruct::RtLevel2Data const& level2data,
		Inqueue::ContextAccessor& historyaccessor, 
		Inqueue::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
		)
	{

	}

	void CSystem2::onLevel1DataArrived(
		HlpStruct::RtLevel1Data const& level1data,
		Inqueue::ContextAccessor& historyaccessor, 
		Inqueue::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
		)
	{

	}

	void CSystem2::onEventArrived(
		Inqueue::ContextAccessor& contextaccessor, 
		HlpStruct::CallingContext& callContext,
		bool& result,
		CppUtils::String& customOutputData
		)
	{
		result=false;
	}

	void CSystem2::onThreadStarted(Inqueue::ContextAccessor& contextaccessor, bool const firstLib, bool const lastLib) 
	{
		double begin_time = CppUtils::roundToBeginningOfTheDay(CppUtils::getTime()) - 3*24*60*60;

		initSymbolContext(contextaccessor);
	};

	CppUtils::String const& CSystem2::getName() const
	{
		static const CppUtils::String name("[ system 2 ]");
		return name;
	}

	// ------------------------------------------------
	// Helpers
	// ------------------------------------------------

	void CSystem2::initSymbolContext(Inqueue::ContextAccessor& historyaccessor)
	{
		double work_day_begin_yesterday_t = -1;
		double now_t = CppUtils::getTime();
		double day_begin_t = CppUtils::roundToBeginningOfTheDay(now_t);

		for(CppUtils::StringSet::const_iterator it = commonparams_m.workSymbols_m.begin(); it != commonparams_m.workSymbols_m.end(); it++) {
			CppUtils::String const& symbol_i = *it;
			SymbolContext ctx;

			CppUtils::String exportDir(commonparams_m.exportBasePath_m + "\\" + CppUtils::getAscTimeAsFileName(day_begin_t));
			CppUtils::makeDir(exportDir);

			ctx.exportPath_m = exportDir + "\\" + CppUtils::createValidFileName(symbol_i) + ".csv";
			symbolContextMap_m[symbol_i] = ctx;
		}


	}

	void CSystem2::readInitialParameters(Inqueue::AlgBrokerDescriptor const& descriptor)
	{

		commonparams_m.hoursBegin_m = 9;
		commonparams_m.minutesBegin_m  = 30;

		commonparams_m.hourEnd_m = 15;
		commonparams_m.minutesEnd_m = 59;


		// hours of trading
		CppUtils::StringList reslst;
		CppUtils::String const& time_trade_begin_s = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "TIME_TRADE_BEGIN");
		CppUtils::tokenize(time_trade_begin_s,reslst,":");

		if (reslst.size() ==2) {
			commonparams_m.hoursBegin_m=atoi(reslst[0].c_str());
			commonparams_m.minutesBegin_m = atoi(reslst[1].c_str());
		}

		// 16:40
		CppUtils::String const& time_trade_end_s = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "TIME_TRADE_END");
		CppUtils::tokenize(time_trade_end_s,reslst,":");

		if (reslst.size() ==2) {
			commonparams_m.hourEnd_m=atoi(reslst[0].c_str());
			commonparams_m.minutesEnd_m = atoi(reslst[1].c_str());
		}



		commonparams_m.unixTimeBeginTrade_m = commonparams_m.hoursBegin_m * 60 * 60 + commonparams_m.minutesBegin_m*60;
		commonparams_m.unixTimeEndTrade_m = commonparams_m.hourEnd_m * 60 * 60 + commonparams_m.minutesEnd_m*60;
		commonparams_m.providerForRTSymbols_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "PROVIDER_FOR_RT_SYMBOLS"); // this will be used for drawable objects


		CppUtils::String const& symbol_rt_file = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "SYMBOL_FILE");
		if (CppUtils::fileExists(symbol_rt_file)) {

			CppUtils::parseSymbolListFile(commonparams_m.workSymbols_m, symbol_rt_file);

			// send available symbols
			//for(CppUtils::StringSet::const_iterator it = commonparams_m.workSymbols_m.begin(); it != commonparams_m.workSymbols_m.end(); it++) {
			//LOG_SYMBOL(internalRun_name_m,  providerForRTSymbols_m, SLogger::SL_INFO, *it, "symbol will be processed" );
			//}

		};


		commonparams_m.exportBasePath_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "EXPORT_BASE_PATH");

		LOG_COMMON(internalRun_name_m, commonparams_m.providerForRTSymbols_m, BrkLib::AP_MEDIUM, 
			" Parameters were read, they are: \n" <<
			" Time signal start: " << commonparams_m.hoursBegin_m << ":" << commonparams_m.minutesBegin_m<< "\n" << 
			" Time signal end: " << commonparams_m.hourEnd_m << ":" << commonparams_m.minutesEnd_m<< "\n" << 
			" RT symbols Provider: " << commonparams_m.providerForRTSymbols_m << "\n" <<
			" The number of symbols passed for operation: " << commonparams_m.workSymbols_m.size() <<
			" Export base path: " << commonparams_m.exportBasePath_m
			);


	}

	void CSystem2::openLongPos(
		BrkLib::BrokerBase* brokerlib, 
		CppUtils::String const& runName, 
		CppUtils::String const& comment,
		CppUtils::String const& provider,
		CppUtils::String const& symbol,
		double const& orderTime,
		int const posTag,
		double const& tpPrice
	)
	{
		if (brokerlib== NULL)
			return;

		BrkLib::BrokerConnect brkConnect(brokerlib, "system_2", false, false, runName.c_str(), comment.c_str());
		brkConnect.connect();

		BrkLib::BrokerOperation oper(brkConnect);
		BrkLib::Order order;

		// buy
		BrkLib::initMarketOrder(order, provider, symbol, "", true, -1,tpPrice,-1,100,-1,orderTime);
		order.tag_m = posTag;
		oper.unblockingIssueOrder(order);
	}

	void CSystem2::openShortPos(BrkLib::BrokerBase* brokerlib, 
		CppUtils::String const& runName, 
		CppUtils::String const& comment,
		CppUtils::String const& provider,
		CppUtils::String const& symbol,
		double const& orderTime,
		int const posTag,
		double const& tpPrice
		)
	{
		if (brokerlib== NULL)
			return;

		BrkLib::BrokerConnect brkConnect(brokerlib, "system_2", false, false, runName.c_str(), comment.c_str());
		brkConnect.connect();

		BrkLib::BrokerOperation oper(brkConnect);
		BrkLib::Order order;

		// buy
		BrkLib::initMarketOrder(order, provider, symbol, "", false, -1,tpPrice,-1,100,-1,orderTime);
		order.tag_m  = posTag;
		oper.unblockingIssueOrder(order);

	}

	void CSystem2::closePos(
		BrkLib::BrokerBase* brokerlib, 
		CppUtils::String const& runName, 
		CppUtils::String const& comment,
		CppUtils::String const& provider,
		CppUtils::String const& symbol,
		double const& orderTime
		)
	{
		if (brokerlib== NULL)
			return;

		map<CppUtils::String, SymbolContext>::iterator it = symbolContextMap_m.find(symbol);


		if (it == symbolContextMap_m.end()) 
			return;


		SymbolContext& smbcontext = it->second;

		if (smbcontext.openPosId1_m.size() <=0 && smbcontext.openPosId2_m.size() <=0)
			return;

		

		BrkLib::BrokerConnect brkConnect(brokerlib, "system_2", false, false, runName.c_str(), comment.c_str());
		brkConnect.connect();


		BrkLib::BrokerOperation oper(brkConnect);
		BrkLib::Order order;

		if (smbcontext.openPosId1_m.size() >0) {
			BrkLib::initCloseOrder(order, provider,symbol, "", smbcontext.openPosId1_m,orderTime);
			oper.unblockingIssueOrder(order);
			smbcontext.openPosId1_m = "";
		}

		if (smbcontext.openPosId2_m.size() >0) {
			BrkLib::initCloseOrder(order, provider,symbol, "", smbcontext.openPosId2_m,orderTime);
			oper.unblockingIssueOrder(order);
			smbcontext.openPosId2_m = "";
		}

	}

	

} // end of namespace
