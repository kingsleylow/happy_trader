#include "impl.hpp"


namespace AlgLib {


#define GAP_DETECTOR "GAP_DETECTOR"
// ------------------------

AlgorithmBase *creator (Inqueue::AlgorithmHolder* algholder)
{
		return new GapDetector(*algholder);
}

void terminator (AlgorithmBase *pthis)
{
		HTASSERT(pthis);
		delete pthis;
}

AlgorithmBase::CreatorAlgorithmFun createAlgorithm = creator;

AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm = terminator;


// ------------------------


GapDetector::GapDetector( Inqueue::AlgorithmHolder& algHolder):
	AlgorithmBase(algHolder)
{
	
}

GapDetector::~GapDetector()
{  
	
}

void GapDetector::onLibraryInitialized(
	HlpStruct::AlgBrokerDescriptor const& descriptor, 
	BrkLib::BrokerBase* brokerlib, 
	BrkLib::BrokerBase* brokerlib2,
	CppUtils::String const& runName,
	CppUtils::String const& comment  
)
{

	// list of symbols
	CppUtils::String const& trade_symbols_file = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "TRADE_SYMBOLS_FILE");
	if (CppUtils::fileExists(trade_symbols_file)) {
	
		if (!CppUtils::parseSymbolListFile(checkedSymbols_m, trade_symbols_file)) {
			LOGEVENT(HlpStruct::CommonLog::LL_ERROR, GAP_DETECTOR, "Cannot read symbols from: " << trade_symbols_file);
			return;
		}
		
	};

	// provider
	provider_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "TRADE_SYMBOL_PROVIDER");


	// symbol lits

	
	// by default:
	hoursBeginMonitor_m=9;
	minutesBeginMonitor_m =30;
	
	// time of trading
	// 09:30
	CppUtils::String const& time_monitor_begin_s = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "TIME_MONITOR_BEGIN");

	CppUtils::StringList reslst;
	CppUtils::tokenize(time_monitor_begin_s,reslst,":");

	if (reslst.size() ==2) {
		hoursBeginMonitor_m=atoi(reslst[0].c_str());
		minutesBeginMonitor_m = atoi(reslst[1].c_str());
	}

	unixTimeBeginMonitor_m = hoursBeginMonitor_m * 60 * 60 + minutesBeginMonitor_m*60;

	

	LOGEVENT(HlpStruct::CommonLog::LL_INFO, GAP_DETECTOR, "Library initialized: start monitor time: " << hoursBeginMonitor_m << ":" << minutesBeginMonitor_m 	);

	DRAWOBJ_TEXT(runName, CppUtils::getTime(),GAP_DETECTOR, "-ALL-", provider_m, BrkLib::AP_HIGH, "INITIALIZED", "" );
	
	
}

void GapDetector::onLibraryFreed()
{
	
	

	LOGEVENT(HlpStruct::CommonLog::LL_INFO, GAP_DETECTOR, "Library freed");
	
}

void GapDetector::onBrokerResponseArrived(
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
)
{
	

}

void GapDetector::onRtProviderSynchEvent (
			Inqueue::ContextAccessor& contextaccessor,
			HlpStruct::RtData::SynchType const synchFlg,
			double const& synchTime,
			CppUtils::String const& providerName,
			CppUtils::String const& message
		)
	{
	}

void GapDetector::onDataArrived(
	HlpStruct::RtData const& rtdata, 
	Inqueue::ContextAccessor& historyaccessor, 
	HlpStruct::TradingParameters const& descriptor,
	BrkLib::BrokerBase* brokerlib,
	CppUtils::String const& runName,
	CppUtils::String const& comment
)
{

	// here we may dinamically change parameters
	//
	

	double diffFromDayBegin = rtdata.getTime() - CppUtils::roundToBeginningOfTheDay(rtdata.getTime());
	

	double gap_detect_parameter = GET_ALG_TRADE_DOUBLE(descriptor, "GAP_DETECT_PARAM");
	double volume_factor = GET_ALG_TRADE_DOUBLE(descriptor, "VOLUME_FACTOR");

	if (gap_detect_parameter >= 0) {
		map<CppUtils::String, Symbol>::iterator it = symbolEntries_m.find(rtdata.getSymbol2());
		
		if (it!=symbolEntries_m.end()) {

			if (diffFromDayBegin >= unixTimeBeginMonitor_m && it->second.openToday_m < 0) {
				// time to init
				LOGEVENT(HlpStruct::CommonLog::LL_INFO, GAP_DETECTOR, "Symbol: " << rtdata.getSymbol() << " open initializing: " << rtdata.getRtData().bid_m <<
					" gap_detect_parameter: " << gap_detect_parameter <<
					" volume_factor: " << volume_factor
					);

				it->second.openToday_m = rtdata.getRtData().bid_m;
			}


			if (it->second.openToday_m <0) {
				// not initialized
				LOGEVENT(HlpStruct::CommonLog::LL_INFO, GAP_DETECTOR, "Symbol: " << rtdata.getSymbol() << " not initialized open price");

				return;
			}

			
			if ((volume_factor >0 && it->second.volumeFactor_m >= volume_factor) || volume_factor <=0 ) {

				double relativeDiff = (it->second.openToday_m  - it->second.closePrevDay_m ) / it->second.closePrevDay_m;

				//DRAWOBJ_TEXT(runName, rtdata.getTime(),GAP_DETECTOR, rtdata.getSymbol2(), rtdata.getProvider2(), BrkLib::AP_HIGH, "INFO", "Relative diff: " << relativeDiff << 
				//	" Volume factor: " << it->second.volumeFactor_m );

				if (relativeDiff >= gap_detect_parameter) {

					
					// up
					DRAWOBJ_TEXT(runName, rtdata.getTime(),GAP_DETECTOR, rtdata.getSymbol2(), rtdata.getProvider2(), BrkLib::AP_HIGH, "GAP UP", 
						"SYMBOL: " << rtdata.getSymbol2() <<
						" GAP VALUE: " << relativeDiff << 
						" VOLUME FACTOR: " << it->second.volumeFactor_m);

				

				} else if(relativeDiff<=-gap_detect_parameter) {
					// down
					DRAWOBJ_TEXT(runName, rtdata.getTime(),GAP_DETECTOR, rtdata.getSymbol2(), rtdata.getProvider2(), BrkLib::AP_HIGH, "GAP DOWN",
						"SYMBOL: " << rtdata.getSymbol2() <<
						" GAP VALUE: " << relativeDiff <<
						" VOLUME FACTOR: " << it->second.volumeFactor_m);

				}

			}
		} 
		
	}

}

void GapDetector::onLevel2DataArrived(
		HlpStruct::RtLevel2Data const& level2data,
		Inqueue::ContextAccessor& historyaccessor, 
		HlpStruct::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
	)
{

	
}

void GapDetector::onLevel1DataArrived(
		HlpStruct::RtLevel1Data const& level1data,
		Inqueue::ContextAccessor& historyaccessor, 
		HlpStruct::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
	)
{
	

}

void GapDetector::onEventArrived(
				Inqueue::ContextAccessor& contextaccessor, 
				HlpStruct::CallingContext& callContext,
				bool& result,
				CppUtils::String &customOutputData
		)
{
	result = false;
}



void GapDetector::onThreadStarted(Inqueue::ContextAccessor& contextaccessor, bool const firstLib, bool const lastLib) {
	

	// check that we have yesterdays price for all
	// otherwise we

	double now_t = CppUtils::getTime();
	double day_begin_t = CppUtils::roundToBeginningOfTheDay(now_t);
	double day_begin_yesterday_t = day_begin_t - 24*60*60;

	LOGEVENT(HlpStruct::CommonLog::LL_INFO,GAP_DETECTOR, "Now is: " << CppUtils::getGmtTime2(now_t) <<
		" Day begin: " << CppUtils::getGmtTime2(day_begin_t) <<
		" Day begin yesterday: " << CppUtils::getGmtTime2(day_begin_yesterday_t)
	);

	for(CppUtils::StringSet::const_iterator it = checkedSymbols_m.begin(); it != checkedSymbols_m.end(); it++ ) {
		CppUtils::String const& symbol_i = *it;
		int cache_symbol_id = contextaccessor.cacheSymbol(provider_m, symbol_i, HlpStruct::AP_Day, 1);

		if (cache_symbol_id >0)	{
			HlpStruct::PriceData const& unclosed_symbol = contextaccessor.getUnclosedCandle(cache_symbol_id);

			if (!unclosed_symbol.isValid()) {
				LOGEVENT(HlpStruct::CommonLog::LL_INFO, GAP_DETECTOR, "Symbol: " << symbol_i << " is invalid, cache size: " << contextaccessor.getSize(cache_symbol_id) );
				continue;
			}
   
			// this must match
			symbolEntries_m[symbol_i].closePrevDay_m = Math::GetPrice2<Math::P_CloseBid>::getCandlePrice(unclosed_symbol);

			// ma factor
			Math::MovAverMathEngine<Math::PriceSeriesProvider> maEngine(52);
			double ma = contextaccessor.callMathEngine<double, Math::P_Volume, Math::MovAverMathEngine<Math::PriceSeriesProvider> >(cache_symbol_id, maEngine);

			if (ma <0) {
				continue;
			}

			double factor1 = ma * Math::GetPrice2<Math::P_CloseBid>::getCandlePrice(unclosed_symbol);


			symbolEntries_m[symbol_i].volumeFactor_m = factor1;

			LOGEVENT(HlpStruct::CommonLog::LL_INFO, GAP_DETECTOR, "Symbol: " << symbol_i << " yesterday close price: " << symbolEntries_m[symbol_i].closePrevDay_m <<
				" yesterday time: " << CppUtils::getGmtTime2(unclosed_symbol.time_m) <<
				" volume factor: " << factor1
			);



		}
	};

	if (symbolEntries_m.size() != checkedSymbols_m.size()) {
		LOGEVENT(HlpStruct::CommonLog::LL_INFO,GAP_DETECTOR, "Not all symbols has yesterday price, symbol size: " << symbolEntries_m.size());
	} else {
		LOGEVENT(HlpStruct::CommonLog::LL_INFO,GAP_DETECTOR, "Yesterday close price acquired OK, symbol size: " << symbolEntries_m.size());
	}



}


CppUtils::String const& GapDetector::getName() const
{
	static const CppUtils::String build_date(__TIMESTAMP__);
	static const CppUtils::String name( "[ GAP DETECTOR ] - build: " + build_date );
	return name;
}
  
// ----------------------
// HELPERS
// ----------------------



}; // end of namespace