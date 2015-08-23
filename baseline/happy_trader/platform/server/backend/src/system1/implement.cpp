#include "implement.hpp"


namespace AlgLib {
AlgorithmBase *creator (Inqueue::AlgorithmHolder* algholder)
{
	return new System1(*algholder);
}

void terminator (AlgorithmBase *pthis)
{
	assert(pthis);
	delete pthis;
}

AlgorithmBase::CreatorAlgorithmFun createAlgorithm = creator;

AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm = terminator;

// intreface to be implemented
System1::System1( Inqueue::AlgorithmHolder& algHolder):
	AlgorithmBase(algHolder)
{

}

System1::~System1()
{

}

void System1::onLibraryInitialized(
	Inqueue::AlgBrokerDescriptor const& descriptor, 
	BrkLib::BrokerBase* brokerlib, 
	CppUtils::String const& runName,
	CppUtils::String const& comment
)
{
	internalRun_name_m = runName;
	readInitialParameters(descriptor);
}

void System1::onLibraryFreed()
{
	LOG_SIGNAL(internalRun_name_m, commonparams_m.providerForRTSymbols_m, BrkLib::AP_CRITICAL, "DUMMY",
		"Finishing..."		 
	);
}

void System1::onBrokerResponseArrived(
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
)
{

}

void System1::onDataArrived(
	HlpStruct::RtData const& rtdata, 
	Inqueue::ContextAccessor& historyaccessor, 
	Inqueue::TradingParameters const& descriptor,
	BrkLib::BrokerBase* brokerlib,
	CppUtils::String const& runName,
	CppUtils::String const& comment
)
{
	//	5 minutes
	//	commonparams_m.providerForRTSymbols_m
	//	commonparams_m.workSymbols_m
	//	int cache_symbol_id = historyaccessor.cacheSymbol(provider_m, symbol_i, Inqueue::AP_Minute, 5);

	if (rtdata.getProvider() != commonparams_m.providerForRTSymbols_m) 
		return;
	
	map<CppUtils::String, SymbolContext>::iterator it = symbolContextMap_m.find(rtdata.getSymbol());
	
	
	if (it == symbolContextMap_m.end()) 
		return;

	SymbolContext& smbcontext = it->second;
	if (smbcontext.dateLastDayClose_m < 0) {
		LOG_SYMBOL(internalRun_name_m, commonparams_m.providerForRTSymbols_m, BrkLib::AP_HIGH,rtdata.getSymbol(),
			"For symbol: " << rtdata.getSymbol() << " invalid last day close price" );
		return;
	}

	if (!commonparams_m.isWithinWorkingTime(rtdata.getTime())) 
		return;

	int cache_symbol_id = historyaccessor.cacheSymbolWithDateFilteringHours(
		commonparams_m.hoursBegin_m*100+commonparams_m.minutesBegin_m,
		commonparams_m.hourEnd_m *100 + commonparams_m.minutesEnd_m,
		rtdata.getProvider2(), rtdata.getSymbol2(), Inqueue::AP_Minute, 5);
	
	if (cache_symbol_id <0 ) 
		return;

			
		
	// get last unclosed symbol
	HlpStruct::PriceData const& unclosed = historyaccessor.getUnclosedCandle(cache_symbol_id);

	bool price_signal_threshold_short = false, 
		price_signal_threshold_long = false, 
		price_high_threshold_long = false,
		price_low_threshold_short = false;

	if (unclosed.isNewPeriod()) {
		
		// reset this each 5 minutes
		smbcontext.volume_signal_5MIN_m = false;
		

		LOG_SYMBOL(internalRun_name_m, commonparams_m.providerForRTSymbols_m, BrkLib::AP_MEDIUM,rtdata.getSymbol(),
					"5MIN For symbol: " << rtdata.getSymbol() << "[ " << CppUtils::getGmtTime(rtdata.getTime())<< " ] we reset SIG1 flag"	)


		// 
		// -------------------------------------
		// last
		Inqueue::DataPtr ptr= historyaccessor.dataLast( cache_symbol_id );
		historyaccessor.dataPrev(cache_symbol_id,ptr );

		
		if (!ptr.isValid())
			return;

			// last candle
		HlpStruct::PriceData const& pdata = historyaccessor.getCurrentPriceData(cache_symbol_id, ptr);

		CppUtils::String hintStr;

		// calc max volume
		if (smbcontext.maxDay5minVolume_m <0) {
			// need to go through history and find max volume
			Inqueue::DataPtr ptr_i = historyaccessor.dataFirst(cache_symbol_id);

			while(historyaccessor.dataNext(cache_symbol_id, ptr_i)) {
				HlpStruct::PriceData const& pdata_i = historyaccessor.getCurrentPriceData(cache_symbol_id, ptr_i);
				hintStr += CppUtils::getGmtTime(pdata_i.time_m) + ", ";


				if (smbcontext.maxDay5minVolume_m < 0) {
					smbcontext.maxDay5minVolume_m = pdata_i.volume_m;
				
				}
				else {
					if (smbcontext.maxDay5minVolume_m < pdata_i.volume_m) 
						smbcontext.maxDay5minVolume_m = pdata_i.volume_m;
				}

				if (smbcontext.currentHigh_m < 0 ) {
					smbcontext.currentHigh_m = pdata_i.high2_m;
				}
				else {
					if (smbcontext.currentHigh_m < pdata_i.high2_m) 
						smbcontext.currentHigh_m = pdata_i.high2_m;
				}

				if (smbcontext.currentLow_m < 0 ) {
					smbcontext.currentLow_m = pdata_i.low2_m;
				}
				else {
					if (smbcontext.currentLow_m > pdata_i.low2_m) 
						smbcontext.currentLow_m = pdata_i.low2_m;
				}


			}

			
			LOG_SYMBOL(internalRun_name_m, commonparams_m.providerForRTSymbols_m, BrkLib::AP_MEDIUM,rtdata.getSymbol(),
					"5MIN - Initial detection for symbol: " << rtdata.getSymbol() << "[ " << CppUtils::getGmtTime(rtdata.getTime()) << " ]" << "\n" << 
					" volume: " << smbcontext.maxDay5minVolume_m << "\n" <<
					" high: " << smbcontext.currentHigh_m << "\n" <<
					" low: " << smbcontext.currentLow_m << "\n" <<
					" candles used: " + hintStr
			);
			


		}
		else { // maxDay5minVolume_m > 0
			
			//7) current 5 minutes volume > for this day all 5 minutes volumes excepting the 5st
			if (pdata.volume_m > smbcontext.maxDay5minVolume_m) {
				smbcontext.volume_signal_5MIN_m = true;

				
				
				LOG_SYMBOL(internalRun_name_m, commonparams_m.providerForRTSymbols_m, BrkLib::AP_MEDIUM,rtdata.getSymbol(),
					"5MIN - SIG1 - For symbol: " << rtdata.getSymbol() << "[ " << CppUtils::getGmtTime(rtdata.getTime())<< " ] current volume exceeds all other volumes\n" <<
					" current volume=" << pdata.volume_m << "\n" <<
					" max 5min day volume=" << smbcontext.maxDay5minVolume_m << "\n" <<
					" candle time=" << CppUtils::getGmtTime(pdata.time_m)
				);
				

				smbcontext.maxDay5minVolume_m = pdata.volume_m;

			}


		}


	} // end of 5 min period
	


	// 6) current print price ~ current high (-0.002 )
	// TICK!!!
	if (smbcontext.currentHigh_m > 0) {
		if (smbcontext.currentHigh_m < rtdata.getRtData().bid_m) {
			smbcontext.currentHigh_m = rtdata.getRtData().bid_m;

			//LOG_SYMBOL(internalRun_name_m, commonparams_m.providerForRTSymbols_m, BrkLib::AP_MEDIUM,rtdata.getSymbol(),
			//	"TICK - For symbol: " << rtdata.getSymbol() << "[ " << CppUtils::getGmtTime(rtdata.getTime())<< " ] high detected: " << smbcontext.currentHigh_m);

		}

		if (rtdata.getRtData().bid_m > (smbcontext.currentHigh_m - commonparams_m.highLowThreshold_m)) {
			price_high_threshold_long = true;

			
			LOG_SYMBOL(internalRun_name_m, commonparams_m.providerForRTSymbols_m, BrkLib::AP_HIGH,rtdata.getSymbol(),
				"TICK - SIG2 - For symbol: " << rtdata.getSymbol() << "[ " << CppUtils::getGmtTime(rtdata.getTime())<< 
				" ] price exceeds up the High for LONG\n" << 
				" current price=" << rtdata.getRtData().bid_m << "\n" <<
				" current high=" << smbcontext.currentHigh_m << "\n" <<
				" threshold=" << commonparams_m.highLowThreshold_m
			);
			
		}
	}

	if (smbcontext.currentLow_m >0) {
		if (smbcontext.currentLow_m > rtdata.getRtData().bid_m) {
			smbcontext.currentLow_m = rtdata.getRtData().bid_m;

			//LOG_SYMBOL(internalRun_name_m, commonparams_m.providerForRTSymbols_m, BrkLib::AP_MEDIUM,rtdata.getSymbol(),
			//	"TICK -For symbol: " << rtdata.getSymbol() << "[ " << CppUtils::getGmtTime(rtdata.getTime())<< " ] low detected: " << smbcontext.currentLow_m);

		}

		if (rtdata.getRtData().bid_m < (smbcontext.currentLow_m + commonparams_m.highLowThreshold_m) ) {
			price_low_threshold_short = true;
			
			LOG_SYMBOL(internalRun_name_m, commonparams_m.providerForRTSymbols_m, BrkLib::AP_HIGH,rtdata.getSymbol(),
				"TICK - SIG2 - For symbol: " << rtdata.getSymbol() << "[ " << CppUtils::getGmtTime(rtdata.getTime())<< 
				" ] price exceeds down the Low for SHORT\n" << 
				" current price=" << rtdata.getRtData().bid_m << "\n" <<
				" current low=" << smbcontext.currentLow_m << "\n" <<
				" threshold=" << commonparams_m.highLowThreshold_m
			);
			
		}
	}

	// other signals
	// -----------------------------
	// price net = current print price - close prev day; 
  // price net >= 2.49
	// signal price 
	// TICK!!!
	double price_net_long = rtdata.getRtData().bid_m - smbcontext.lastDayClose_m;
	if (price_net_long > commonparams_m.signalPriceThreshold_m) {
		price_signal_threshold_long = true;

		
		LOG_SYMBOL(internalRun_name_m, commonparams_m.providerForRTSymbols_m, BrkLib::AP_HIGH,rtdata.getSymbol(),
			"TICK - SIG3 - For symbol: " << rtdata.getSymbol() << "[ " << CppUtils::getGmtTime(rtdata.getTime())<< 
			" ] price net=(current price-day close price) exceeds up the threshold for LONG\n" << 
			" current price=" << rtdata.getRtData().bid_m << "\n" <<
			" last close price=" << smbcontext.lastDayClose_m << "\n" <<
			" threshold=" << commonparams_m.signalPriceThreshold_m
		);
		
	}

	if ((-price_net_long) > commonparams_m.signalPriceThreshold_m) {
		price_signal_threshold_short = true;

		
		LOG_SYMBOL(internalRun_name_m, commonparams_m.providerForRTSymbols_m, BrkLib::AP_HIGH,rtdata.getSymbol(),
			"TICK - SIG3 - For symbol: " << rtdata.getSymbol() << "[ " << CppUtils::getGmtTime(rtdata.getTime())<< 
			" ] price net=(current price-day close price) exceeds down the threshold for SHORT\n" << 
			" current price=" << rtdata.getRtData().bid_m << "\n" <<
			" last close price=" << smbcontext.lastDayClose_m << "\n" <<
			" threshold=" << commonparams_m.signalPriceThreshold_m
		);
		
	}

	// time to raise signals
	if (!commonparams_m.isWithingSignalRaising(rtdata.getTime())) 
		return;

	// just summarize
	if (price_signal_threshold_short && price_low_threshold_short &&  smbcontext.volume_signal_5MIN_m) {
		LOG_SIGNAL(internalRun_name_m, commonparams_m.providerForRTSymbols_m, BrkLib::AP_CRITICAL,rtdata.getSymbol(),
			"TICK - For symbol: " << rtdata.getSymbol() << "[ " << CppUtils::getGmtTime(rtdata.getTime())<< 
			" SHORT SIGNAL" 
		);
	}

	if (price_signal_threshold_long && price_high_threshold_long && smbcontext.volume_signal_5MIN_m) {
		LOG_SIGNAL(internalRun_name_m, commonparams_m.providerForRTSymbols_m, BrkLib::AP_CRITICAL,rtdata.getSymbol(),
			"TICK - For symbol: " << rtdata.getSymbol() << "[ " << CppUtils::getGmtTime(rtdata.getTime())<< 
			" LONG SIGNAL" 
		);
	}
	

}

void System1::onRtProviderSynchEvent (
			Inqueue::ContextAccessor& contextaccessor,
			HlpStruct::	RtData::SynchType const synchFlg,
			double const& synchTime,
			CppUtils::String const& providerName,
			CppUtils::String const& message
		)
{
	// notifications
	if (HlpStruct::RtData::SYNC_EVENT_PROVIDER_TRANSMIT_ERROR == synchFlg) {
		LOG_SIGNAL(internalRun_name_m, commonparams_m.providerForRTSymbols_m, BrkLib::AP_CRITICAL, "DUMMY", "Quote transmition failure" );
	}
	else if (HlpStruct::RtData::SYNC_EVENT_PROVIDER_START == synchFlg) {
		LOG_SIGNAL(internalRun_name_m, commonparams_m.providerForRTSymbols_m, BrkLib::AP_CRITICAL, "DUMMY", "Quote transmition OK" );
	}
}

   
void System1::onLevel2DataArrived(
		HlpStruct::RtLevel2Data const& level2data,
		Inqueue::ContextAccessor& historyaccessor, 
		Inqueue::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
	)
{

}

void System1::onLevel1DataArrived(
		HlpStruct::RtLevel1Data const& level1data,
		Inqueue::ContextAccessor& historyaccessor, 
		Inqueue::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
	)
{

}

void System1::onEventArrived(
	Inqueue::ContextAccessor& contextaccessor, 
	HlpStruct::CallingContext& callContext,
	bool& result,
	CppUtils::String &customOutputData
)
{
	result = false;
}
	
void System1::onThreadStarted(Inqueue::ContextAccessor& contextaccessor, bool const firstLib, bool const lastLib) 
{
	
	double begin_time = CppUtils::roundToBeginningOfTheDay(CppUtils::getTime()) - 3*24*60*60;
	LOG_COMMON(internalRun_name_m, commonparams_m.providerForRTSymbols_m, BrkLib::AP_MEDIUM, "Try to load history dtata from profile: " << commonparams_m.profileForEndDaySymbols_m << " from: " << CppUtils::getGmtTime(begin_time));

	bool result = contextaccessor.loadDataFromMaster(commonparams_m.profileForEndDaySymbols_m, begin_time,-1);
	if (!result) {
		LOG_COMMON(internalRun_name_m, commonparams_m.providerForRTSymbols_m, BrkLib::AP_HIGH, "Error - cannot load history" );
	}

	initSymbolContext(contextaccessor);
	LOG_COMMON(internalRun_name_m, commonparams_m.providerForRTSymbols_m, BrkLib::AP_MEDIUM, "^^^^^^^^^^^^^^^^^^^^^^^^^^^^ Starting ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");

	LOG_SIGNAL(internalRun_name_m, commonparams_m.providerForRTSymbols_m, BrkLib::AP_CRITICAL, "DUMMY",
		"Pay attention to that kind of signals, we are starting..."		 
	);

};

CppUtils::String const& System1::getName() const
{
	static const CppUtils::String name("[ System1 ]");
	return name;
}

/**
* Helpers
*/

void System1::initSymbolContext(Inqueue::ContextAccessor& historyaccessor)
{

	
	
	double work_day_begin_yesterday_t = -1;
	double now_t = CppUtils::getTime();
	double day_begin_t = CppUtils::roundToBeginningOfTheDay(now_t);
	
	if (commonparams_m.yesterdayDate_m.size() == 0) {
		

		// if today is monday
		tm tms;
		CppUtils::crackGmtTime(day_begin_t, tms);
		
		
		if (tms.tm_wday==1)	{
			// is monday ?
			work_day_begin_yesterday_t = day_begin_t - 3*24*60*60;
			
		}
		else if (tms.tm_wday==0) {  
			// is sunday
			work_day_begin_yesterday_t = day_begin_t - 2*24*60*60;
			LOG_COMMON(internalRun_name_m, commonparams_m.providerForRTSymbols_m, BrkLib::AP_HIGH, "Today is Sunday!!!");
		}
		else if (tms.tm_wday==6) {
			// saturday
			work_day_begin_yesterday_t = day_begin_t - 24*60*60;
			LOG_COMMON(internalRun_name_m, commonparams_m.providerForRTSymbols_m, BrkLib::AP_HIGH, "Today is Saturday!!!");
		}
		else {
			work_day_begin_yesterday_t = day_begin_t - 24*60*60;
		}

	}
	else {
		work_day_begin_yesterday_t = CppUtils::roundToBeginningOfTheDay(CppUtils::parseFormattedDateTime("d/M/Y", commonparams_m.yesterdayDate_m, 0));
	}

	LOG_COMMON(internalRun_name_m,commonparams_m.providerForRTSymbols_m, BrkLib::AP_MEDIUM, "Now is: " << CppUtils::getGmtTime2(now_t) <<
		" Today day: " << CppUtils::getGmtTime2(day_begin_t) <<
		" last close day: " << CppUtils::getGmtTime2(work_day_begin_yesterday_t)
	);


	for(CppUtils::StringSet::const_iterator it = commonparams_m.workSymbols_m.begin(); it != commonparams_m.workSymbols_m.end(); it++) {
		CppUtils::String const& symbol_i = *it;
		int cache_symbol_id = historyaccessor.cacheSymbol(commonparams_m.providerForEndDaySymbols_m, symbol_i, Inqueue::AP_Day, 1);
		if (cache_symbol_id >0)	{

				HlpStruct::PriceData const& unclosed_symbol = historyaccessor.getUnclosedCandle(cache_symbol_id);

				if (unclosed_symbol.time_m >0) {
				
					// check the date
					tm tms_i;
					CppUtils::crackGmtTime(unclosed_symbol.time_m, tms_i);

					tm tms_work;
					CppUtils::crackGmtTime(work_day_begin_yesterday_t, tms_work);

					if (tms_i.tm_mday == tms_work.tm_mday && tms_i.tm_year == tms_work.tm_year && tms_i.tm_mon == tms_work.tm_mon) {
						// our date
						symbolContextMap_m[symbol_i].lastDayClose_m = unclosed_symbol.close2_m;
						symbolContextMap_m[symbol_i].dateLastDayClose_m = unclosed_symbol.time_m;

						LOG_SYMBOL(internalRun_name_m, commonparams_m.providerForRTSymbols_m, BrkLib::AP_MEDIUM, symbol_i, "History data date OK: " << symbol_i << " dated " << CppUtils::getGmtTime2(unclosed_symbol.time_m));


					}
					else {
						LOG_SYMBOL(internalRun_name_m, commonparams_m.providerForRTSymbols_m, BrkLib::AP_HIGH, symbol_i, "Wrong history data date: " << symbol_i << " dated " << CppUtils::getGmtTime2(unclosed_symbol.time_m));
					}
				} else {
					LOG_SYMBOL(internalRun_name_m, commonparams_m.providerForRTSymbols_m, BrkLib::AP_HIGH, symbol_i, "For symbol: " << symbol_i << " data where obsolete");
				}
		}
		else {
			LOG_SYMBOL(internalRun_name_m, commonparams_m.providerForRTSymbols_m, BrkLib::AP_HIGH, symbol_i, "History data not found for symbol: " << symbol_i);
		}
	}

	LOG_COMMON(internalRun_name_m, commonparams_m.providerForRTSymbols_m, BrkLib::AP_MEDIUM, "History data were succesfully read for: " << symbolContextMap_m.size() << " symbols" << ", expected for: " << commonparams_m.workSymbols_m.size() );

	if (commonparams_m.workSymbols_m.size() != symbolContextMap_m.size()) {
		LOG_COMMON(internalRun_name_m, commonparams_m.providerForRTSymbols_m, BrkLib::AP_HIGH, "Some symbol were not initialized!");
	}
}

void System1::readInitialParameters(Inqueue::AlgBrokerDescriptor const& descriptor)
{
	LOG_COMMON(internalRun_name_m, commonparams_m.providerForRTSymbols_m, BrkLib::AP_MEDIUM, "^^^^^^^^^^^^^^^^^^^^^^^^^^^^ Initializing ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");

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

	// raise signals time

	CppUtils::String const& time_raise_signals_s = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "TIME_RAISE_SIGNALS_BEGIN");
	CppUtils::tokenize(time_raise_signals_s,reslst,":");
	
	if (reslst.size() ==2) {
		commonparams_m.hourRaiseSignalsBegin_m=atoi(reslst[0].c_str());
		commonparams_m.minuteRaiseSignalsBegin_m = atoi(reslst[1].c_str());
	}


	commonparams_m.unixTimeBeginTrade_m = commonparams_m.hoursBegin_m * 60 * 60 + commonparams_m.minutesBegin_m*60;
	commonparams_m.unixTimeEndTrade_m = commonparams_m.hourEnd_m * 60 * 60 + commonparams_m.minutesEnd_m*60;
	commonparams_m.unixTimeBeginSignals_m = commonparams_m.hourRaiseSignalsBegin_m * 60 * 60 + commonparams_m.minuteRaiseSignalsBegin_m*60;
	
	commonparams_m.profileForEndDaySymbols_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "PROFILE_FOR_ENDDAY_SYMBOLS");
	commonparams_m.providerForRTSymbols_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "PROVIDER_FOR_RT_SYMBOLS"); // this will be used for drawable objects
	commonparams_m.providerForEndDaySymbols_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "PROVIDER_FOR_ENDDAY_SYMBOLS"); // this will be used for drawable objects

	commonparams_m.signalPriceThreshold_m = GET_ALG_TRADE_DOUBLE(descriptor.initialAlgBrkParams_m, "SIGNAL_PRICE_THRESHOLD");
	commonparams_m.highLowThreshold_m = GET_ALG_TRADE_DOUBLE(descriptor.initialAlgBrkParams_m, "HI_LOW_THRESHOLD");


	CppUtils::String const& symbol_rt_file = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "SYMBOL_FILE");
	if (CppUtils::fileExists(symbol_rt_file)) {
	
		CppUtils::parseSymbolListFile(commonparams_m.workSymbols_m, symbol_rt_file);

		// send available symbols
		for(CppUtils::StringSet::const_iterator it = commonparams_m.workSymbols_m.begin(); it != commonparams_m.workSymbols_m.end(); it++) {
				//LOG_SYMBOL(internalRun_name_m,  providerForRTSymbols_m, SLogger::SL_INFO, *it, "symbol will be processed" );
		}
  
	};

	// if set this is a parameter requiriing when yesterday happen
	commonparams_m.yesterdayDate_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "LAST_CLOSE_DATE");

	LOG_COMMON(internalRun_name_m, commonparams_m.providerForRTSymbols_m, BrkLib::AP_MEDIUM, 
			" Parameters were read, they are: \n" <<
			" Time signal start: " << commonparams_m.hoursBegin_m << ":" << commonparams_m.minutesBegin_m<< "\n" << 
			" Time signal end: " << commonparams_m.hourEnd_m << ":" << commonparams_m.minutesEnd_m<< "\n" << 
			" Time raise signals begin: " << commonparams_m.hourRaiseSignalsBegin_m << ":" << commonparams_m.minuteRaiseSignalsBegin_m<< "\n" << 
			" RT symbols Provider: " << commonparams_m.providerForRTSymbols_m << "\n" <<
			" End day symbols provider: " << commonparams_m.providerForEndDaySymbols_m << "\n" <<
			" Signal price threshold: " << commonparams_m.signalPriceThreshold_m << "\n" <<
			" Hi Low threshold: " << commonparams_m.highLowThreshold_m << "\n" <<
			" Last close date(optional): " << commonparams_m.yesterdayDate_m << "\n" <<
			" The number of symbols passed for operation: " << commonparams_m.workSymbols_m.size()
		);

	
}

} // end of namespace
