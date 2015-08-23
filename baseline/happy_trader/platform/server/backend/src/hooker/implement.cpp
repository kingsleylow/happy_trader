#include "implement.hpp"


#define HOOKER "HOOKER"

namespace AlgLib {

// ------------------------

AlgorithmBase *creator (Inqueue::AlgorithmHolder* algholder)
{
		return new GerhikHooker(*algholder);
}

void terminator (AlgorithmBase *pthis)
{
		HTASSERT(pthis);
		delete pthis;
}

AlgorithmBase::CreatorAlgorithmFun createAlgorithm = creator;

AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm = terminator;


// ------------------------


GerhikHooker::GerhikHooker( Inqueue::AlgorithmHolder& algHolder):
	AlgorithmBase(algHolder)
{
	
}

GerhikHooker::~GerhikHooker()
{  
	
}

void GerhikHooker::onLibraryInitialized(
	HlpStruct::AlgBrokerDescriptor const& descriptor, 
	BrkLib::BrokerBase* brokerlib, 
	CppUtils::String const& runName,
	CppUtils::String const& comment  
)
{

	int tmp;

	hiLowHookerCriteriaPct_m = 1;
	maPeriod_m = 13;
	bodyLehgthHookerCriteriaPct_m = 80;


	// by default:
	hoursBeginMonitor_m=9;
	minutesBeginMonitor_m =30;

	hoursBegin_m = 9;
	minutesBegin_m = 50;

	hourEnd_m = 15;
	minutesEnd_m = 59;


	// ma
	tmp = GET_ALG_TRADE_INT(descriptor.initialAlgBrkParams_m, "MA_PERIOD");
	if (tmp != -1)
		maPeriod_m = tmp;

	// hooker candle
	double tmpd = GET_ALG_TRADE_DOUBLE(descriptor.initialAlgBrkParams_m, "HI_LOW_HOOKER_CRITERIA");
	if (tmpd != -1)
		hiLowHookerCriteriaPct_m = tmpd;


	tmpd = GET_ALG_TRADE_DOUBLE(descriptor.initialAlgBrkParams_m, "BODY_LENGTH_HOOKER_CRITERIA");
	if (tmpd != -1)
		bodyLehgthHookerCriteriaPct_m = tmpd;
	
	// symbol lits
	CppUtils::String const& trade_symbols_file = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "TRADE_SYMBOLS_FILE");
	if (CppUtils::fileExists(trade_symbols_file)) {
	
		if (!CppUtils::parseSymbolListFile(tradedSymbols_m, trade_symbols_file)) {
			LOGEVENT(HlpStruct::CommonLog::LL_ERROR, HOOKER, "Cannot read symbols from: " << trade_symbols_file);
		}
	};

	pathToExport_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "EXPORT_PATH");
	if (pathToExport_m.size() <=0) {
		THROW(CppUtils::OperationFailed, "exc_InvalidExportPath", "ctx_InitializeHooker", "");
	}

	CppUtils::makeDir(pathToExport_m);


	// time of trading
	// 09:30
	CppUtils::String const& time_monitor_begin_s = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "TIME_MONITOR_BEGIN");

	CppUtils::StringList reslst;
	CppUtils::tokenize(time_monitor_begin_s,reslst,":");

	if (reslst.size() ==2) {
		hoursBeginMonitor_m=atoi(reslst[0].c_str());
		minutesBeginMonitor_m = atoi(reslst[1].c_str());
	}


	// 16:40
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

	PERIOD_MIN_m = GET_ALG_TRADE_INT(descriptor.initialAlgBrkParams_m, "PERIOD_MIN");
	if (PERIOD_MIN_m < 0 )
		PERIOD_MIN_m = 5;

	RAISE_ONLY_BOTH_m = false;
	RAISE_ONLY_BOTH_m = GET_ALG_TRADE_BOOL(descriptor.initialAlgBrkParams_m, "RAISE_ONLY_BOTH");



	LOGEVENT(HlpStruct::CommonLog::LL_INFO, HOOKER, "Library initialized: start monitor time: " << hoursBeginMonitor_m << ":" << minutesBeginMonitor_m <<
		" start trade: " << hoursBegin_m << ":" << minutesBegin_m <<
		" end trade: " << hourEnd_m << ":" << minutesEnd_m <<
		" minutes in period: " << PERIOD_MIN_m <<
		" raise only both signals: " << RAISE_ONLY_BOTH_m
		);

	unixTimeBeginMonitor_m = hoursBeginMonitor_m * 60 * 60 + minutesBeginMonitor_m*60;
	unixTimeBeginTrade_m = hoursBegin_m * 60 * 60 + minutesBegin_m*60;
	unixTimeEndTrade_m = hourEnd_m * 60 * 60 + minutesEnd_m*60;

	RAISE_CONSOLIDATION_m = false;
	RAISE_CONSOLIDATION_m = GET_ALG_TRADE_BOOL(descriptor.initialAlgBrkParams_m, "RAISE_CONSOLIDATION");


	EVEN_BARS_COUNT_m = GET_ALG_TRADE_INT(descriptor.initialAlgBrkParams_m, "EVEN_BARS_COUNT");
	if (EVEN_BARS_COUNT_m < 0)
		EVEN_BARS_COUNT_m = 4;



	ALERTEVENTNC("",BrkLib::AP_HIGH, HOOKER, "Hooker initialized" );
	
	DRAWOBJ_TEXT(runName,CppUtils::getTime(),HOOKER, "-ALL-", "dummy", BrkLib::AP_MEDIUM, "LOG", "Hooker initialized" );	

	
	
	
}

void GerhikHooker::onLibraryFreed()
{
	
	

	LOGEVENT(HlpStruct::CommonLog::LL_INFO, HOOKER, "Library freed");
	ALERTEVENTNC("",BrkLib::AP_HIGH, HOOKER, "Hooker freed" );
}

void GerhikHooker::onBrokerResponseArrived(
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
)
{
	if (bresponse.type_m==BrkLib::RT_BrokerResponseMetaInfo) {
		BrkLib::BrokerResponseMetaInfo const& brespMinfo = (BrkLib::BrokerResponseMetaInfo const&)bresponse;

		
	}
	else if (bresponse.type_m==BrkLib::RT_BrokerResponseOrder) {
		BrkLib::BrokerResponseOrder const& bRespOrder = (BrkLib::BrokerResponseOrder const&)bresponse;
				



	}	
	

}

void GerhikHooker::onRtProviderSynchEvent (
			Inqueue::ContextAccessor& contextaccessor,
			HlpStruct::RtData::SynchType const synchFlg,
			double const& synchTime,
			CppUtils::String const& providerName,
			CppUtils::String const& message
		)
	{
	}


void GerhikHooker::onDataArrived(
	HlpStruct::RtData const& rtdata, 
	Inqueue::ContextAccessor& historyaccessor, 
	HlpStruct::TradingParameters const& descriptor,
	BrkLib::BrokerBase* brokerlib,
	CppUtils::String const& runName,
	CppUtils::String const& comment
)
{

	double diffFromDayBegin = rtdata.getTime() - CppUtils::roundToBeginningOfTheDay(rtdata.getTime());

	bool monitor_only;
	if (diffFromDayBegin < unixTimeBeginMonitor_m || diffFromDayBegin > unixTimeEndTrade_m)
		return;
	else if (diffFromDayBegin >= unixTimeBeginMonitor_m && diffFromDayBegin < unixTimeBeginTrade_m )
		monitor_only = true;
	else if (diffFromDayBegin > unixTimeBeginTrade_m)
		monitor_only = false;

	

	// will trade only the single provider - e.i. the only provider

	
	// here we can start our calcs
	//int cache_symbol_id = historyaccessor.cacheSymbol(rtdata.getProvider(), rtdata.getSymbol2(), Inqueue::AP_Minute, 5);
	int cache_symbol_id = historyaccessor.cacheSymbolWithDateFilteringHours(
		hoursBegin_m*100+minutesBegin_m,
		hourEnd_m *100 + minutesEnd_m,
		rtdata.getProvider2(), rtdata.getSymbol2(), HlpStruct::AP_Minute, PERIOD_MIN_m);

	if (cache_symbol_id <= 0) {
		LOGEVENT(HlpStruct::CommonLog::LL_ERROR, HOOKER, "Cannot cache symbol: " << rtdata.getSymbol2());
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


		// all signals here
		// последн€€ свеча закрыта€ должна быть ниже 13 ≈ћј
		// а перед этим свеча удовлетвор€ть критери€м
		// вот  и все

		// here we will export our cache

		
		CppUtils::String fileName = pathToExport_m + "\\" + rtdata.getSymbol2() + "_" + CppUtils::long2String(PERIOD_MIN_m) + ".txt";
		Math::ExporterEngine<Math::PriceSeriesProvider> exporter(fileName, false, false);
		historyaccessor.callMathEngine<double, Math::P_CloseBid, Math::ExporterEngine<Math::PriceSeriesProvider> >(cache_symbol_id, exporter);

  
		// -----------
	
		
		// pre prev
		//HlpStruct::PriceData const& pdata_1 = historyaccessor.getCurrentPriceData(cache_symbol_id, ptr);


		Math::MovAverMathEngine<Math::PriceSeriesProvider> engineSMA(maPeriod_m);
		double maVal = historyaccessor.callMathEngine<double, Math::P_CloseBid, Math::MovAverMathEngine<Math::PriceSeriesProvider> >(cache_symbol_id, engineSMA);
		//if (maVal <=0)
		//	return;

		// our conditions
		//double relative_hi_low_range = 100.0*(pdata_1.high2_m - pdata_1.low2_m)/pdata_1.close2_m;
		//double relative_body = (abs(pdata_1.open2_m - pdata_1.close2_m))/(pdata_1.high2_m - pdata_1.low2_m) * 100.0;


		double diff = pdata.high2_m - pdata.low2_m;
		if (diff < 0.00001) {
			//LOGEVENT(HlpStruct::CommonLog::LL_INFO, HOOKER, " BAD CANDLE SYMBOL:" <<  rtdata.getSymbol2() << " ASK CANDLE: " << pdata.toCSVAskString() << " BID: " << pdata.toCSVBidString());
			//DRAWOBJ_TEXT(runName,CppUtils::getTime(),HOOKER, rtdata.getSymbol2(), rtdata.getProvider2(), BrkLib::AP_MEDIUM, "LOG",
			//	" BAD CANDLE SYMBOL:" <<  rtdata.getSymbol2() << " ASK CANDLE: " << pdata.toCSVAskString() << " BID: " << pdata.toCSVBidString() );	

			return;
		}
		double relative_hi_low_range = 100.0*(pdata.high2_m - pdata.low2_m)/pdata.close2_m;
		double relative_body = ((abs(pdata.open2_m - pdata.close2_m))/diff) * 100.0;

		
		CppUtils::String prefix = rtdata.getSymbol2() + ":" + 
				" TIME:" + CppUtils::getGmtTime2(pdata.time_m) + 
				" OPEN:"+ CppUtils::float2String(pdata.open2_m, -1, 4) + 
				" HIGH:"+ CppUtils::float2String(pdata.high2_m, -1, 4) +
				" LOW:"+ CppUtils::float2String(pdata.low2_m, -1, 4) + 
				" CLOSE:"+ CppUtils::float2String(pdata.close2_m, -1, 4) + 
				" HI-LOW RANGE:" + CppUtils::float2String(relative_hi_low_range, -1, 2) +
				" BODY:"+CppUtils::float2String(relative_body, -1, 2)+
				" MA:"+CppUtils::float2String(maVal, -1,2);   
        
		if ( relative_hi_low_range >= hiLowHookerCriteriaPct_m && relative_body < bodyLehgthHookerCriteriaPct_m) {
			if (!RAISE_ONLY_BOTH_m) {
				//ALERTEVENTNC("",BrkLib::AP_HIGH, CppUtils::String("HOOKER HI-LOW CRITERIA: ") + prefix, "");
				//DRAWOBJ_TEXT(runName,CppUtils::getTime(),HOOKER, rtdata.getSymbol2(), rtdata.getProvider2(), BrkLib::AP_MEDIUM, "LOG", 
				//	CppUtils::String("HOOKER HI-LOW CRITERIA: ") + prefix );	

			}
		}
		else if ( relative_hi_low_range >= hiLowHookerCriteriaPct_m && relative_body >= bodyLehgthHookerCriteriaPct_m) {
			//ALERTEVENTNC("",BrkLib::AP_HIGH, CppUtils::String("AND BODY LENGTH CRITERIA: ") + prefix, "");
			//DRAWOBJ_TEXT(runName,CppUtils::getTime(),HOOKER, rtdata.getSymbol2(), rtdata.getProvider2(), BrkLib::AP_MEDIUM, "LOG", 
			//		CppUtils::String("AND BODY LENGTH CRITERIA: ") + prefix );	
		}
				
			

		
	

		// here all we have data

		// another stuff to detect consolidation - let's get 3 candles and see if high or lows are close
		// round to 1 cent
		if (RAISE_CONSOLIDATION_m) {
			// pdata
			int i = 0;


			// even check
			/*
			for( i = 20; i >= 3; i--) {
				Math::ConvergentCloseEngine cengine(i);
				double result_conv = historyaccessor.callMathEngine<Math::P_CloseBid, Math::ConvergentCloseEngine>(cache_symbol_id, cengine);
				

				if (result_conv != Math::ConvergentCloseEngine::NO_CONVERGENT && result_conv != 0) {
					DRAWOBJ_TEXT(runName,CppUtils::getTime(),HOOKER, rtdata.getSymbol2(), rtdata.getProvider2(), BrkLib::AP_MEDIUM, "LOG", 
						"Result from convergion: #" << i << " - "<< Math::ConvergentCloseEngine::getConversionStringResult((int)result_conv) );	

					break;
				}
			}
			*/


			bool high_even = false, low_even = false;
			double result_conv = -1.0;
			for( i = 20; i >= 3; i--) {

				// 1 centes price range
				Math::EqualHighLowEngine<Math::PriceSeriesProvider> ehlengine(i, 0.01);
				double result = historyaccessor.callMathEngine<double, Math::P_CloseBid, Math::EqualHighLowEngine<Math::PriceSeriesProvider> >(cache_symbol_id, ehlengine);

				// convergent
				Math::ConvergentCloseEngine<Math::PriceSeriesProvider> cengine(i);
				result_conv = historyaccessor.callMathEngine<double, Math::P_CloseBid, Math::ConvergentCloseEngine<Math::PriceSeriesProvider> >(cache_symbol_id, cengine);
				

				if (!Math::EqualHighLowEngine<Math::PriceSeriesProvider>::isHighsOut(result) && !Math::EqualHighLowEngine<Math::PriceSeriesProvider>::isLowsOut(result)) {
					
					// no signal
					break;
				}

				if (!Math::EqualHighLowEngine<Math::PriceSeriesProvider>::isHighsOut(result) && (result_conv == Math::ConvergentEngine<Math::PriceSeriesProvider>::HIGHS_LOWS_CONVERGENT || result_conv == Math::ConvergentEngine<Math::PriceSeriesProvider>::LOWS_CONVERGENT) )
				{
					//ALERTEVENTNC("",BrkLib::AP_HIGH, CppUtils::long2String(i) + CppUtils::String(" !!! - HOOKER HIGHS EVEN SURFACE: ") + prefix, "");
					//DRAWOBJ_TEXT(runName,CppUtils::getTime(),HOOKER, rtdata.getSymbol2(), rtdata.getProvider2(), BrkLib::AP_MEDIUM, "LOG", 
					//	CppUtils::long2String(i) + CppUtils::String(" !!! - HOOKER HIGHS EVEN SURFACE: ") + prefix );	

					high_even = true;

					break;
				}

				if (!Math::EqualHighLowEngine<Math::PriceSeriesProvider>::isLowsOut(result) && (result_conv == Math::ConvergentEngine<Math::PriceSeriesProvider>::HIGHS_LOWS_CONVERGENT || result_conv == Math::ConvergentEngine<Math::PriceSeriesProvider>::HIGHS_CONVERGENT))
				{
					//ALERTEVENTNC("",BrkLib::AP_HIGH, CppUtils::long2String(i) + CppUtils::String(" !!! - HOOKER LOWS EVEN SURFACE: ") + prefix, "");
					//DRAWOBJ_TEXT(runName,CppUtils::getTime(),HOOKER, rtdata.getSymbol2(), rtdata.getProvider2(), BrkLib::AP_MEDIUM, "LOG", 
					//	CppUtils::long2String(i) + CppUtils::String(" !!! - HOOKER LOWS EVEN SURFACE: ") + prefix );	

					low_even = true;

					break;
				}

			}; // end loop

			// check everything
			if (low_even && (result_conv==Math::ConvergentCloseEngine<Math::PriceSeriesProvider>::HIGHS_CONVERGENT || result_conv==Math::ConvergentCloseEngine<Math::PriceSeriesProvider>::HIGHS_LOWS_CONVERGENT))
			{
				DRAWOBJ_TEXT(runName,CppUtils::getTime(),HOOKER, rtdata.getSymbol2(), rtdata.getProvider2(), BrkLib::AP_MEDIUM, "LOG", 
					CppUtils::long2String(i) + CppUtils::String(" !!! - HOOKER LOWS EVEN SURFACE AND HIGH CONVERGENT ") + prefix );	

			}

			if (high_even && (result_conv==Math::ConvergentCloseEngine<Math::PriceSeriesProvider>::LOWS_CONVERGENT || result_conv==Math::ConvergentCloseEngine<Math::PriceSeriesProvider>::HIGHS_LOWS_CONVERGENT))
			{
				DRAWOBJ_TEXT(runName,CppUtils::getTime(),HOOKER, rtdata.getSymbol2(), rtdata.getProvider2(), BrkLib::AP_MEDIUM, "LOG", 
					CppUtils::long2String(i) + CppUtils::String(" !!! - HOOKER HIGH EVEN SURFACE AND LOW CONVERGENT ") + prefix );	

			}


			if (brokerlib==NULL)
				return;
			

			// consolidation - good thing to check broker library
			BrkLib::BrokerConnect brkConnect(brokerlib, "hooker_rt_test", false, false, runName.c_str(), comment.c_str());
			brkConnect.connect();

			BrkLib::BrokerOperation oper(brkConnect);
			BrkLib::Order order;
			
			// system is simple 
			// if signal and no orders just if we have 4 consolidations
			if (i>=EVEN_BARS_COUNT_m) {

				BrkLib::PositionList const& poslist = oper.getBrokerPositionList(BrkLib::QUERY_PENDING | BrkLib::QUERY_OPEN);

				int cnt_open = oper.getPositionCount(BrkLib::QUERY_PENDING | BrkLib::QUERY_OPEN, rtdata.getProvider2(), rtdata.getSymbol2(), "");

				// open
				if (cnt_open==0) {
					if (low_even) {
						// install STOP with SL

						// 2 centes
						double sell_price = rtdata.getRtData().bid_m - 0.02;

						// 10 centes
						double tp_price = rtdata.getRtData().bid_m - 0.10;

						// 3 centes
						double sl_price = rtdata.getRtData().bid_m + 0.03;

						/*initStopOrder(
							order,
							rtdata.getProvider2(),
							rtdata.getSymbol2(), 
							"",
							false , // sell
							sl_price,
							tp_price,
							sell_price,
							100,
							-1,
							-1
						);
						*/

						oper.unblockingIssueOrder(order);

					}
					else if (high_even) {
						// install STOP with SL

						// 2 centes
						double buy_price = rtdata.getRtData().bid_m + 0.02;

						// 10 centes
						double tp_price = rtdata.getRtData().bid_m + 0.10;

						// 3 centes
						double sl_price = rtdata.getRtData().bid_m - 0.03;
						
						/*
						initStopOrder(
							order,
							rtdata.getProvider2(),
							rtdata.getSymbol2(), 
							"",
							true , // sell
							sl_price,
							tp_price,
							buy_price,
							100,
							-1,
							-1
						);
						*/

						oper.unblockingIssueOrder(order);
					
					}
				}

			}

		}

		

	

		
	};	
	
}

void GerhikHooker::onLevel2DataArrived(
		HlpStruct::RtLevel2Data const& level2data,
		Inqueue::ContextAccessor& historyaccessor, 
		HlpStruct::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
	)
{

	
}

void GerhikHooker::onLevel1DataArrived(
		HlpStruct::RtLevel1Data const& level1data,
		Inqueue::ContextAccessor& historyaccessor, 
		HlpStruct::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
	)
{
	

}


void GerhikHooker::onEventArrived(
				Inqueue::ContextAccessor& contextaccessor, 
				HlpStruct::CallingContext& callContext,
				bool& result,
				CppUtils::String &customOutputData
		)
{
	result = false;
}

void GerhikHooker::onThreadStarted(Inqueue::ContextAccessor& contextaccessor, bool const firstLib, bool const lastLib) {

}


CppUtils::String const& GerhikHooker::getName() const
{
	static const CppUtils::String build_date(__TIMESTAMP__);
	static const CppUtils::String name( "[ HOOKER algorithm ] - build: " + build_date );
	return name;
}

/**
* Helpers
*/


}; // end of namepsace