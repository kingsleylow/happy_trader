#include "impl.hpp"
#include "symbolstruct.hpp"

namespace AlgLib {


#define GAP_CALCULATOR "GAP_CALCULATOR"
// ------------------------

AlgorithmBase *creator (Inqueue::AlgorithmHolder* algholder)
{
		return new GapCalculator(*algholder);
}

void terminator (AlgorithmBase *pthis)
{
		HTASSERT(pthis);
		delete pthis;
}

AlgorithmBase::CreatorAlgorithmFun createAlgorithm = creator;

AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm = terminator;


// ------------------------


GapCalculator::GapCalculator( Inqueue::AlgorithmHolder& algHolder):
	AlgorithmBase(algHolder)
{
	
}

GapCalculator::~GapCalculator()
{  
	
}

void GapCalculator::onLibraryInitialized(
	HlpStruct::AlgBrokerDescriptor const& descriptor, 
	BrkLib::BrokerBase* brokerlib, 
	BrkLib::BrokerBase* brokerlib2,
	CppUtils::String const& runName,
	CppUtils::String const& comment  
)
{

	// list of symbols


	// symbol lits

	CppUtils::String const& trade_symbols_file = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "TRADE_SYMBOLS_FILE");
	if (CppUtils::fileExists(trade_symbols_file)) {
	
		if (!CppUtils::parseSymbolListFile(checkedSymbols_m, trade_symbols_file)) {
			LOGEVENT(HlpStruct::CommonLog::LL_ERROR, GAP_CALCULATOR, "Cannot read symbols from: " << trade_symbols_file);
			return;
		}

		
	};

	provider_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "TRADE_SYMBOL_PROVIDER");

	beginTestingDate_m = CppUtils::parseFormattedDateTime("M/d/Y", GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "BEGIN_TESTING_DATE"), 0 );

	endTestingDate_m = CppUtils::parseFormattedDateTime("M/d/Y", GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "END_TESTING_DATE"), 0 );


	LOGEVENT(HlpStruct::CommonLog::LL_INFO, GAP_CALCULATOR, "Library initialized");
	
	
}

void GapCalculator::onLibraryFreed()
{
	
	

	LOGEVENT(HlpStruct::CommonLog::LL_INFO, GAP_CALCULATOR, "Library freed");
	
}

void GapCalculator::onBrokerResponseArrived(
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
)
{
	

}

void GapCalculator::onRtProviderSynchEvent (
			Inqueue::ContextAccessor& contextaccessor,
			HlpStruct::RtData::SynchType const synchFlg,
			double const& synchTime,
			CppUtils::String const& providerName,
			CppUtils::String const& message
		)
	{
	}


void GapCalculator::onDataArrived(
	HlpStruct::RtData const& rtdata, 
	Inqueue::ContextAccessor& historyaccessor, 
	HlpStruct::TradingParameters const& descriptor,
	BrkLib::BrokerBase* brokerlib,
	CppUtils::String const& runName,
	CppUtils::String const& comment
)
{

	
}

void GapCalculator::onLevel2DataArrived(
		HlpStruct::RtLevel2Data const& level2data,
		Inqueue::ContextAccessor& historyaccessor, 
		HlpStruct::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
	)
{

	
}

void GapCalculator::onLevel1DataArrived(
		HlpStruct::RtLevel1Data const& level1data,
		Inqueue::ContextAccessor& historyaccessor, 
		HlpStruct::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
	)
{
	

}


void GapCalculator::onEventArrived(
				Inqueue::ContextAccessor& contextaccessor, 
				HlpStruct::CallingContext& callContext,
				bool& result,
				CppUtils::String &customOutputData
		)
{
	result = false;
}


void GapCalculator::onThreadStarted(Inqueue::ContextAccessor& contextaccessor, bool const firstLib, bool const lastLib) {
	

	/// cache symbols
	cachedIds_m.clear();

	for(CppUtils::StringSet::const_iterator it= checkedSymbols_m.begin(); it != checkedSymbols_m.end(); it++) {
		CppUtils::String const& symbol_i = *it;

		int cache_id_i = contextaccessor.cacheSymbol(provider_m, symbol_i, HlpStruct::AP_Day, 1);

		if (cache_id_i <= 0) {
			LOGEVENT(HlpStruct::CommonLog::LL_ERROR, GAP_CALCULATOR, "Cannot cache symbol: " << symbol_i);
			continue;
		}

		if (contextaccessor.getSize(cache_id_i) < 2) {
			LOGEVENT(HlpStruct::CommonLog::LL_ERROR, GAP_CALCULATOR, "Cache size is less then 2: " << symbol_i);
			continue;
		}

		cachedIds_m.push_back(cache_id_i);

		
	}
	// ----------

	double stopLossLevel, volumeFactorFilter, gapFilter;

	bool IS_SHORT = true;
	bool SINGLETON = true;
	double MIN_SL_LEVEL = 0.01;
	double MAX_SL_LEVEL = 0.06;
	double STEP_SL_LEVEl = 0.01;

	double MIN_VOLUME_FACTOR = 100000000.0;
	double MAX_VOLUME_FACTOR = 6.0*1000000000.0;
	double STEP_VOLUME_FACTOR = 250000000.0;

	double MIN_GAP_FILTER = 0.01;
	double MAX_GAP_FILTER = 0.15;
	double STEP_GAP_FILTER = 0.02;

	int totalcnt = 0;
	vector<CalcResult> resultlist;
	
	stopLossLevel = MIN_SL_LEVEL;
	while(stopLossLevel <=MAX_SL_LEVEL) {

		volumeFactorFilter = MIN_VOLUME_FACTOR;
		while(volumeFactorFilter <= MAX_VOLUME_FACTOR) {

			gapFilter = MIN_GAP_FILTER;
			while(gapFilter <= MAX_GAP_FILTER) {  
  					
				//
				
				 //if (gapFilter== 0.03 && volumeFactorFilter == 1100000000 && stopLossLevel == 0.02) {
					CalcResult result(totalcnt);
					returnProfitPerRun(IS_SHORT, SINGLETON,stopLossLevel, volumeFactorFilter, gapFilter, contextaccessor, result);
					LOGEVENT(HlpStruct::CommonLog::LL_INFO, GAP_CALCULATOR, "Simulation result #" << totalcnt  );

					resultlist.push_back( result );
				 //}

				totalcnt++;
				gapFilter+= STEP_GAP_FILTER;

			}

			volumeFactorFilter += STEP_VOLUME_FACTOR;
		}

		stopLossLevel += STEP_SL_LEVEl;
	}


	createReportToTheFile("c:\\temp\\gap_calk.txt", resultlist );
	LOGEVENT(HlpStruct::CommonLog::LL_INFO, GAP_CALCULATOR, "Simulation finished with total iterations: " << totalcnt );

	
}


CppUtils::String const& GapCalculator::getName() const
{
	static const CppUtils::String build_date(__TIMESTAMP__);
	static const CppUtils::String name( "[ GAP CALCULATOR ] - build: " + build_date );
	return name;
}
  
// ----------------------
// HELPERS
// ----------------------

void GapCalculator::returnProfitPerRun(
			bool const is_short,
			bool const singleton,
			double const& stolLossLevel,
			double const& volumeFactorFilter, 
			double const& gapFilter, 
			Inqueue::ContextAccessor& historyaccessor,
			CalcResult& result
		)
{
	result.stolLossLevel_m = stolLossLevel;
	result.volumeFactorFilter_m = volumeFactorFilter;
	result.gapFilter_m = gapFilter;

	result.beginTestingDate_m = beginTestingDate_m;
	result.endTestingDate_m = endTestingDate_m;
	result.isSingleton_m = singleton;

	result.fullPeriodCovered_m = true;

	
	
	
	// cached
	for(int l = 0; l < cachedIds_m.size(); l++) {

		int cache_id_i = cachedIds_m[l];


		// now iterate
		Inqueue::DataPtr ptr = historyaccessor.dataFirst(cache_id_i);
		

		// 0, 1, ... ->
		HlpStruct::PriceData data_0;
		HlpStruct::PriceData const* pdata_1 = 0;
		while(true) {
			bool has_next = historyaccessor.dataNext(cache_id_i, ptr);
			if (has_next)
				pdata_1 = &historyaccessor.getCurrentPriceData(cache_id_i, ptr);
			else 
				pdata_1 = &historyaccessor.getUnclosedCandle(cache_id_i);
			
			HlpStruct::PriceData const& data_1 = *pdata_1;
				
			if (!data_0.isValid()) {

				data_0 = data_1;

				if (data_1.time_m > beginTestingDate_m) {
					LOGEVENT(HlpStruct::CommonLog::LL_ERROR, GAP_CALCULATOR, "Begin testing date is " << CppUtils::getGmtTime(beginTestingDate_m) << 
						" but found first data is " << CppUtils::getGmtTime(data_1.time_m) << " for symbol: " << cache_id_i);

					return;
				}

				continue;
			}
			// calculations

			result.checkedDaysCount_m++;
		

			// TODO!!!
			double gapValue=(Math::GetPrice2<Math::P_CloseBid>::getCandlePrice(data_0) - Math::GetPrice2<Math::P_OpenBid>::getCandlePrice(data_1))/Math::GetPrice2<Math::P_CloseBid>::getCandlePrice(data_0);
			
			// gap value always positive
			if (!is_short)
				gapValue = -gapValue;
			
			Math::MovAverMathEngine<Math::PriceSeriesProvider> maEngine(52);
			double ma = historyaccessor.callMathEngine<double, Math::P_Volume, Math::MovAverMathEngine<Math::PriceSeriesProvider> >(cache_id_i, maEngine);
			double factor1 = ma * Math::GetPrice2<Math::P_CloseBid>::getCandlePrice(data_0);

			

			if (gapValue > gapFilter) {
				result.gapFilterHitCount_m++;
			}

			if (factor1 > volumeFactorFilter) {
				result.volumeFactorHitCount_m++;
			}

					
			// gap filter
			if (gapValue > gapFilter) {

				


				// volume factor filter
				if (factor1 > volumeFactorFilter) {

					

					// total deals this day - we restrict the number of deals
					if (singleton && (result.dailyEntries_m[data_1.time_m].totalDeals_m>=1)) {
						// need to skip that test

						if (!has_next)
							break;

						// prev
						data_0 = data_1;

						continue;
					}

					// volume factor
					double profit;  
					
					// open low only - all signals we have!!!!!!!!!!!!!!!
					result.pattTotalCount_m++;
					
					
					// otherwise GO ON
					result.dailyEntries_m[data_1.time_m].totalDeals_m++;

					double drawdown_abs;

					// this is our go to the negative side
					// muts be always positive
					if (is_short)
						drawdown_abs = Math::GetPrice2<Math::P_HighBid>::getCandlePrice(data_1) - Math::GetPrice2<Math::P_OpenBid>::getCandlePrice(data_1) ;
					else
						drawdown_abs = Math::GetPrice2<Math::P_OpenBid>::getCandlePrice(data_1) - Math::GetPrice2<Math::P_LowBid>::getCandlePrice(data_1) ;

					double drawdown = drawdown_abs / Math::GetPrice2<Math::P_OpenBid>::getCandlePrice(data_1);
					
					SymbolEntry& symbol_day_entry_i = result.dailyEntries_m[data_1.time_m].daySymbols_m[cache_id_i];
					symbol_day_entry_i.prevDayClose_m = Math::GetPrice2<Math::P_CloseBid>::getCandlePrice(data_0);
					symbol_day_entry_i.symbol_m = historyaccessor.getSymbol(cache_id_i);

					symbol_day_entry_i.is_short_m = is_short;
					

					if (  drawdown > stolLossLevel ) {
						// stop loss executed
						profit = -drawdown_abs;
						result.pattStopLossCount_m++;

						// no profit
						symbol_day_entry_i.absoluteMaxIncomePriceGo_m = -1;
						symbol_day_entry_i.relativeMaxIncomePriceGo_m = -1;

					} else {
						if (is_short)
							profit = Math::GetPrice2<Math::P_OpenBid>::getCandlePrice(data_1) - Math::GetPrice2<Math::P_CloseBid>::getCandlePrice(data_1);
						else
							profit = Math::GetPrice2<Math::P_CloseBid>::getCandlePrice(data_1) - Math::GetPrice2<Math::P_OpenBid>::getCandlePrice(data_1);

						result.pattExecCount_m++;

						// profit
						if (is_short)
							symbol_day_entry_i.absoluteMaxIncomePriceGo_m = Math::GetPrice2<Math::P_OpenBid>::getCandlePrice(data_1) - Math::GetPrice2<Math::P_LowBid>::getCandlePrice(data_1); // low price
						else
							symbol_day_entry_i.absoluteMaxIncomePriceGo_m = Math::GetPrice2<Math::P_HighBid>::getCandlePrice(data_1) - Math::GetPrice2<Math::P_OpenBid>::getCandlePrice(data_1);

						//
						symbol_day_entry_i.relativeMaxIncomePriceGo_m = symbol_day_entry_i.absoluteMaxIncomePriceGo_m / symbol_day_entry_i.prevDayClose_m;


					}

					symbol_day_entry_i.absoluteProfit_m = profit;
					symbol_day_entry_i.relativeProfit_m = symbol_day_entry_i.absoluteProfit_m / symbol_day_entry_i.prevDayClose_m;

					
					//NOW calculate PL RATIO - the only parameter to optimize

					
					


				} // end of patter detect

		
			}
			


			
			// 
			if (!has_next)
				break;

			// prev
			data_0 = data_1;

		};


		// data_1 - the last unclosed candle
		if (pdata_1->time_m < this->endTestingDate_m) {
			LOGEVENT(HlpStruct::CommonLog::LL_ERROR, GAP_CALCULATOR, "End testing date is " << CppUtils::getGmtTime2(endTestingDate_m) << 
						" but found last data is " << CppUtils::getGmtTime2(pdata_1->time_m) << " for symbol: " << cache_id_i);
			result.fullPeriodCovered_m = false;

			
		} 		

		


	} // end symbol loop

	
}


void GapCalculator::createReportToTheFile(CppUtils::String const& filename, vector<CalcResult> const& resultlist)
{
	CppUtils::String data;

	data = "ID,"
			"BEGIN DATE,"
			"END DATE,"
			"PARAMETER STOP LOSS,"
			"PARAMETER VOLUME FACTOR FILTER,"
			"PARAMETER GAP FILTER,"

			"GAP FILTER HITS,"
			"VOLUME FACTOR FILTER HITS,"

			"TOTAL TEST COUNT,"
			"TOTAL PATTERN COUNT,"
			"TOTAL CLOSED ENDOFDAY,"
			"TOTAL STOP LOSS COUNT,"

		
			"MAX DEALS PDAY,"
			"AVR DEALS PDAY,"
			"AVR PCT PROFIT MAX PRICE GO,"
			"STD PCT PROFIT MAX PRICE GO,"
			
			"PL RATIO,"
			"AVERAGE ABS PROFIT,"
			"STD PCT PROFIT\n";
	for(int i = 0; i < resultlist.size(); i++) {

		
		CalcResult const& r = resultlist[i];
	
		data+=
			CppUtils::long2String(r.ID_m)+","+
			CppUtils::getGmtTime2(r.beginTestingDate_m)+","+
			CppUtils::getGmtTime2(r.endTestingDate_m)+","+
			CppUtils::float2String(r.stolLossLevel_m, -1, 4)+","+
			CppUtils::float2String(r.volumeFactorFilter_m, -1, 4)+","+
			CppUtils::float2String(r.gapFilter_m, -1, 4)+","+

			CppUtils::long2String(r.gapFilterHitCount_m)+","+
			CppUtils::long2String(r.volumeFactorHitCount_m)+","+

			CppUtils::long2String(r.checkedDaysCount_m)+","+
			CppUtils::long2String(r.pattTotalCount_m)+","+
			CppUtils::long2String(r.pattExecCount_m)+","+
			CppUtils::long2String(r.pattStopLossCount_m)+","+

		
			CppUtils::long2String(r.getMaxDealsPerDay())+","+
			CppUtils::float2String(r.getAverageDealsPerDay(), -1,4)+","+

			CppUtils::float2String(r.getAverageRelativeMaxIncomePriceGo()*100, -1,2)+","+
			CppUtils::float2String(r.getStdRelativeMaxIncomePriceGo()*100, -1,2)+","+

			CppUtils::float2String(r.getPLRatio(), -1, 4)+","+
			CppUtils::float2String(r.getAverageProfit(), -1, 4)+","+
			CppUtils::float2String(r.getStdProfit(), -1, 4)+"\n";

			
		
			// list of symbols

			//data += "\n--------------------------------------\n";
			//data += r.getSymbolList();
			
		
	}

	CppUtils::saveContentInFile2(filename, data );
}


}; // end of namespace