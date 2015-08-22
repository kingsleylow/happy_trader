#include "implement.hpp"


#define GERMAN "GERMAN"
namespace AlgLib {


// ------------------------

	void writeFileEntry(bool const is_clear, CppUtils::String const& basedir, CppUtils::String const& symbol, CppUtils::String const& serName, double const& time, double const& value)
	{
		CppUtils::String fileName = basedir + "\\" + symbol+ "_"+serName+"_ser.txt";

		ofstream f(fileName.c_str(), ios::out | (is_clear ? ios::trunc : ios::app));

		f << CppUtils::getGmtTime3(time)<< "->" << CppUtils::float2String(value, -1, 4) << endl;

		f.flush();
		f.close();

	}

	void writeFileEntry(bool const is_clear, CppUtils::String const& basedir, CppUtils::String const& symbol, CppUtils::String const& line)
	{
		CppUtils::String fileName = basedir + "\\" + symbol+ ".txt";

		ofstream f(fileName.c_str(), ios::out | (is_clear ? ios::trunc : ios::app));

		f << line << endl;

		f.flush();
		f.close();


	}


// ------------------------

AlgorithmBase *creator (Inqueue::AlgorithmHolder* algholder)
{
		return new MGerman(*algholder);
}

void terminator (AlgorithmBase *pthis)
{
		assert(pthis);
		delete pthis;
}

AlgorithmBase::CreatorAlgorithmFun createAlgorithm = creator;

AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm = terminator;


// ------------------------

MGerman::MGerman( Inqueue::AlgorithmHolder& algHolder):
	AlgorithmBase(algHolder)
	//pEngineRSI_m(NULL)
{
	
}

MGerman::~MGerman()
{  
	
}

void MGerman::onLibraryInitialized(
	Inqueue::AlgBrokerDescriptor const& descriptor, 
	BrkLib::BrokerBase* brokerlib, 
	CppUtils::String const& runName,
	CppUtils::String const& comment  
)
{
	//runName_m = runName;
	

	// read trade params
	tradeParams_m.tradeAggrPeriod_m = Inqueue::aggrPeriodFromString()(GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "TRADE_AGGR_PERIOD" ).c_str());
	tradeParams_m.tradeMultFactor_m = GET_ALG_TRADE_INT(descriptor.initialAlgBrkParams_m, "TRADE_MULT_FACTOR");
	tradeParams_m.RSI_PERIOD_G_m = GET_ALG_TRADE_INT(descriptor.initialAlgBrkParams_m, "RSI_PERIOD_G");

	tradeParams_m.PARABOLIC_ACEL_DOWN_m = GET_ALG_TRADE_DOUBLE(descriptor.initialAlgBrkParams_m, "PARABOLIC_ACEL_DOWN");
	tradeParams_m.PARABOLIC_ACEL_UP_m = GET_ALG_TRADE_DOUBLE(descriptor.initialAlgBrkParams_m, "PARABOLIC_ACEL_UP");
	tradeParams_m.PARABOLIC_ACEL_MAX_m = GET_ALG_TRADE_DOUBLE(descriptor.initialAlgBrkParams_m, "PARABOLIC_ACEL_MAX");

	path_wl_ds_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "PATH_FOR_WL_DS");

	//pEngineRSI_m = new Math::WLDRSIMathEngine(tradeParams_m.RSI_PERIOD_G_m);

	tradeState_m.clear();
	openPosId_m.clear();


	// metainfo
	// borker connect
	/*
	BrkLib::BrokerConnect brkConnect(brokerlib, "german_tst", false, false, runName.c_str(), comment.c_str());
	brkConnect.connect();

	BrkLib::BrokerOperation oper(brkConnect);
	BrkLib::Order order;

	// symbol metainfo
	order.orderType_m = BrkLib::OP_GET_SYMBOL_METAINFO;
	oper.unblockingIssueOrder(order);
	*/
	
	LOGEVENT(HlpStruct::CommonLog::LL_INFO, GERMAN, "Library initialized");
	
}

void MGerman::onLibraryFreed()
{
	
	/*
	if (pEngineRSI_m) {
		delete pEngineRSI_m;
		pEngineRSI_m= NULL;
	}
	*/
	//DRAWOBJ_TEXT(CppUtils::getTime(),"GERMAN_INDIC_TEXT", "", "", BrkLib::AP_CRITICAL, "DEINITIALIZED", "" );

	LOGEVENT(HlpStruct::CommonLog::LL_INFO, GERMAN, "Library freed");
}

void MGerman::onBrokerResponseArrived(
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
)
{
	if (bresponse.type_m==BrkLib::RT_BrokerResponseMetaInfo) {
		BrkLib::BrokerResponseMetaInfo const& brespMinfo = (BrkLib::BrokerResponseMetaInfo const&)bresponse;

		
	}
	else if (bresponse.type_m==BrkLib::RT_BrokerResponseOrder) {
		BrkLib::BrokerResponseOrder const& bRespOrder = (BrkLib::BrokerResponseOrder const&)bresponse;
				

		if (bRespOrder.resultCode_m == BrkLib::PT_ORDER_ESTABLISHED) {
			if (bRespOrder.positionType_m == BrkLib::POS_BUY) {
				tradeState_m[bRespOrder.symbolNum_m] = AlgLib::TSG_OpenLong;
				openPosId_m[bRespOrder.symbolNum_m] = bRespOrder.brokerPositionID_m;
				//DRAWOBJ_TEXT(runName_m, bRespOrder.opOpenTime_m,"GERMAN_INDIC_TEXT", bRespOrder.symbolNum_m, bRespOrder.provider_m, BrkLib::AP_HIGH, "BROKER OPEN BUY", 
				//	" POS ID: " <<  openPosId_m << 
				//	" OPEN PRICE: " << CppUtils::float2String(bRespOrder.opOpenPrice_m, -1, 4)
				//	);
			}
			else if (bRespOrder.positionType_m == BrkLib::POS_SELL) {
				tradeState_m[bRespOrder.symbolNum_m] = AlgLib::TSG_OpenShort;
				openPosId_m[bRespOrder.symbolNum_m] = bRespOrder.brokerPositionID_m;
				//DRAWOBJ_TEXT(runName_m, bRespOrder.opOpenTime_m,"GERMAN_INDIC_TEXT", bRespOrder.symbolNum_m, bRespOrder.provider_m, BrkLib::AP_HIGH, "BROKER OPEN SELL", 
				//	" POS ID: " <<  openPosId_m <<
				//	" OPEN PRICE: " << CppUtils::float2String(bRespOrder.opOpenPrice_m, -1, 4)
				//	);
			}
			
		}
		else if (bRespOrder.resultCode_m == BrkLib::PT_ORDER_CLOSED) {
			//DRAWOBJ_TEXT(runName_m, bRespOrder.opCloseTime_m,"GERMAN_INDIC_TEXT", bRespOrder.symbolNum_m, bRespOrder.provider_m, BrkLib::AP_HIGH, "BROKER CLOSE", 
			//	"POS ID: " <<  openPosId_m <<
			//	" CLOSE PRICE: " << CppUtils::float2String(bRespOrder.opClosePrice_m, -1, 4)
			//	);

			tradeState_m[bRespOrder.symbolNum_m] = AlgLib::TSG_Initial;
			openPosId_m[bRespOrder.symbolNum_m] = "";

			
		}


	}	
	

}

void MGerman::onRtProviderSynchEvent (
			Inqueue::ContextAccessor& contextaccessor,
			HlpStruct::RtData::SynchType const synchFlg,
			double const& synchTime,
			CppUtils::String const& providerName,
			CppUtils::String const& message
		)
	{
	}


void MGerman::onDataArrived(
	HlpStruct::RtData const& rtdata, 
	Inqueue::ContextAccessor& historyaccessor, 
	Inqueue::TradingParameters const& descriptor,
	BrkLib::BrokerBase* brokerlib,
	CppUtils::String const& runName,
	CppUtils::String const& comment
)
{
	
	if (historyaccessor.isFirst() )
	{
		// clear data files
		writeFileEntry(true, path_wl_ds_m, rtdata.getSymbol2(), "");

	}


	// borker connect
	BrkLib::BrokerConnect brkConnect(brokerlib, "german_tst", false, false, runName.c_str(), comment.c_str());
	brkConnect.connect();

	//DRAWOBJ_LINEINDIC_3(runName, rtdata.getTime(), "GERMAN_INDIC", rtdata.getSymbol2(), rtdata.getProvider2(), "RT DATA BID", rtdata.getRtData().bid_m, 1,  "RT DATA ASK", rtdata.getRtData().bid_m, 2,  "RT DATA VOLUME", rtdata.getRtData().volume_m,3);


	// here we can start our calcs
	int cache_symbol_id = historyaccessor.cacheSymbol(rtdata.getProvider(), rtdata.getSymbol2(), tradeParams_m.tradeAggrPeriod_m, tradeParams_m.tradeMultFactor_m);
	if (cache_symbol_id <= 0) {
		LOGEVENT(HlpStruct::CommonLog::LL_ERROR, GERMAN, "Cannot cache symbol: " << rtdata.getSymbol2());
		return;
	}

	

	// get last unclosed symbol
	HlpStruct::PriceData const& unclosed = historyaccessor.getUnclosedCandle(cache_symbol_id);

	
	if (unclosed.isNewPeriod()) {

		
			// last
		Inqueue::DataPtr ptr= historyaccessor.dataLast( cache_symbol_id );
		historyaccessor.dataPrev(cache_symbol_id,ptr );

		if (!ptr.isValid())
			return;
  
		
		HlpStruct::PriceData const& pdata = historyaccessor.getCurrentPriceData(cache_symbol_id, ptr);


		// please note that pdatas - this nis the LAST CLOSED candle - e.i. 15.30
		// but rtticks - this is a new tick in a new period - thus - it will be 14.45.00.001-

		

		//CppUtils::String pdatats = CppUtils::getGmtTime(pdata.time_m);
		//CppUtils::String rtticks = CppUtils::getGmtTime(rtdata.getTime());
		
		
		
		// need to create the file with data
		CppUtils::String line_s;
		line_s = CppUtils::getGmtTime3(pdata.time_m) + "," + 
			CppUtils::float2String(pdata.open2_m, -1, 4) + "," +
			CppUtils::float2String(pdata.high2_m, -1, 4) + "," +
			CppUtils::float2String(pdata.low2_m, -1, 4) + "," +
			CppUtils::float2String(pdata.close2_m, -1, 4)+ "," +
			CppUtils::float2String(pdata.volume_m, -1, 4);

		
		 
		
		writeFileEntry(false, path_wl_ds_m, rtdata.getSymbol2(), line_s);
		
		/*
		writeFileEntry(false, path_wl_ds_m, rtdata.getSymbol2(), "Close", pdata.time_m, pdata.close2_m);
		writeFileEntry(false, path_wl_ds_m, rtdata.getSymbol2(), "High", pdata.time_m, pdata.high2_m);
		writeFileEntry(false, path_wl_ds_m, rtdata.getSymbol2(), "Open", pdata.time_m, pdata.open2_m);
		writeFileEntry(false, path_wl_ds_m, rtdata.getSymbol2(), "Low", pdata.time_m, pdata.low2_m);
		*/
		

		//if (historyaccessor.getSize(cache_symbol_id) <= tradeParams_m.getMaxDepth()) {
		//	return;
		//}    

		TradeFactors currentSignals;
		
		//double rsi = historyaccessor.callMathEngine<Math::P_CloseBid, Math::WLDRSIMathEngine>(cache_symbol_id, *pEngineRSI_m);

		Math::PivotMathEngine<Math::PriceSeriesProvider> enginePivot1(6,2);
		Math::PivotMathEngine<Math::PriceSeriesProvider> enginePivot2(2,1);
		Math::KpVolEngine<Math::PriceSeriesProvider> engineKpVol;   

		Math::MetastockParabolicMathEngine<Math::PriceSeriesProvider> engineParabolic(tradeParams_m.PARABOLIC_ACEL_UP_m, tradeParams_m.PARABOLIC_ACEL_DOWN_m, tradeParams_m.PARABOLIC_ACEL_MAX_m, true);
		currentSignals.pivot1_m = historyaccessor.callMathEngine<Math::P_CloseBid, Math::PivotMathEngine<Math::PriceSeriesProvider>>(cache_symbol_id, enginePivot1);
		currentSignals.pivot2_m = historyaccessor.callMathEngine<Math::P_CloseBid, Math::PivotMathEngine<Math::PriceSeriesProvider>>(cache_symbol_id, enginePivot2);
		currentSignals.parab_m = historyaccessor.callMathEngine<Math::P_CloseBid, Math::MetastockParabolicMathEngine<Math::PriceSeriesProvider>>(cache_symbol_id, engineParabolic);
		currentSignals.kPvol_m = historyaccessor.callMathEngine<Math::P_CloseBid, Math::KpVolEngine<Math::PriceSeriesProvider>>(cache_symbol_id, engineKpVol);
	
		/*
		writeFileEntry(false, path_wl_ds_m, rtdata.getSymbol2(), "pivot2Ser", pdata.time_m, pivot2);
		writeFileEntry(false, path_wl_ds_m, rtdata.getSymbol2(), "pivotSer", pdata.time_m, pivot);
		writeFileEntry(false, path_wl_ds_m, rtdata.getSymbol2(), "parabSer", pdata.time_m, parabolic);
		writeFileEntry(false, path_wl_ds_m, rtdata.getSymbol2(), "rsiSer", pdata.time_m, rsi);
		writeFileEntry(false, path_wl_ds_m, rtdata.getSymbol2(), "KPvolSer", pdata.time_m, kpvol);
		*/
		
		// dump
		//CppUtils::String dmp = historyaccessor.very_detailed_dump(cache_symbol_id);
		//CppUtils::saveContentInFile2("c:\\work\\German_work\\wl\\"+rtdata.getSymbol2()+".log", dmp );

		//CppUtils::String foo = rtdata.toString();  

  
		//DRAWOBJ_LEVEL1(runName, pdata.time_m,"GERMAN_INDIC_LEVEL1", rtdata.getSymbol2(), rtdata.getProvider2(),  1,2,3,4 );  
		//DRAWOBJ_PRINT(runName, pdata.time_m,"GERMAN_INDIC_PRINT", rtdata.getSymbol2(), rtdata.getProvider2(),  1,2,3, HlpStruct::RtData::CT_NA );  


		DRAWOBJ_LINEINDIC_4(runName, pdata.time_m, "GERMAN_INDIC", rtdata.getSymbol2(), rtdata.getProvider2(), "KPVOL", currentSignals.kPvol_m, 1, "PIVOT1", currentSignals.pivot1_m, 2, "PIVOT2", currentSignals.pivot2_m, 3, "PARAB", currentSignals.parab_m, 4);
		DRAWOBJ_OHCL(runName, pdata.time_m,"GERMAN_INDIC_OHCL", rtdata.getSymbol2(), rtdata.getProvider2(), pdata.open2_m, pdata.high2_m, pdata.low2_m, pdata.close2_m, pdata.volume_m );  
		
		//DRAWOBJ_TEXT(pdata.time_m,"GERMAN_INDIC_TEXT", rtdata.getSymbol2(), rtdata.getProvider2(), BrkLib::AP_HIGH, "сцуко...", "сцуко1 " << " сцуко2 " << 25 );

		TradeFactors &prevSignal = prevSignals_m[rtdata.getSymbol2()];
		if (prevSignal.isValid() && currentSignals.isValid()) {
			// check for signals here

			// KPvolSer - сверху вних pivot пересекает
			// close > parabolic ???
			if (prevSignal.kPvol_m <= prevSignal.pivot2_m && currentSignals.kPvol_m >= currentSignals.pivot2_m && pdata.close2_m > currentSignals.parab_m) {
				
				DRAWOBJ_TEXT(runName, rtdata.getTime(),"GERMAN_INDIC_TEXT", rtdata.getSymbol2(), rtdata.getProvider2(), BrkLib::AP_HIGH, "SIGNAL OPEN LONG", "" );
				DRAWOBJ_TEXT(runName, rtdata.getTime(),"GERMAN_INDIC_TEXT", rtdata.getSymbol2(), rtdata.getProvider2(), BrkLib::AP_HIGH, getTradeState(tradeState_m[rtdata.getSymbol2()]), "" );


				if (tradeState_m.size()==0 || tradeState_m[rtdata.getSymbol2()]==TSG_Initial) {
					BrkLib::BrokerOperation oper(brkConnect);
					BrkLib::Order order;

					// buy
					BrkLib::initMarketOrder(order, rtdata.getProvider2(), rtdata.getSymbol2(), "", true, -1,-1,-1,100,-1,rtdata.getTime());
					oper.unblockingIssueOrder(order);

					// open long
					DRAWOBJ_TEXT(runName, rtdata.getTime(),"GERMAN_INDIC_TEXT", rtdata.getSymbol2(), rtdata.getProvider2(), BrkLib::AP_HIGH, "OPEN LONG", "" );
				}
			}

			if (prevSignal.kPvol_m >= prevSignal.pivot2_m && currentSignals.kPvol_m <= currentSignals.pivot2_m) {
				
				DRAWOBJ_TEXT(runName, rtdata.getTime(),"GERMAN_INDIC_TEXT", rtdata.getSymbol2(), rtdata.getProvider2(), BrkLib::AP_HIGH, "SIGNAL CLOSE LONG", "" );
				DRAWOBJ_TEXT(runName, rtdata.getTime(),"GERMAN_INDIC_TEXT", rtdata.getSymbol2(), rtdata.getProvider2(), BrkLib::AP_HIGH, getTradeState(tradeState_m[rtdata.getSymbol2()]), "" );



				if (tradeState_m.size()>0 && tradeState_m[rtdata.getSymbol2()]==TSG_OpenLong) { 
					BrkLib::BrokerOperation oper(brkConnect);
					BrkLib::Order order;

					// sell
					BrkLib::initCloseOrder(order, rtdata.getProvider2(), rtdata.getSymbol2(), "", openPosId_m[rtdata.getSymbol2()], rtdata.getTime());
					oper.unblockingIssueOrder(order);

					// close long
					DRAWOBJ_TEXT(runName, rtdata.getTime(),"GERMAN_INDIC_TEXT", rtdata.getSymbol2(), rtdata.getProvider2(), BrkLib::AP_HIGH, "CLOSE LONG", "" );
				}

			}

			if (
				(prevSignal.kPvol_m >= prevSignal.pivot1_m && currentSignals.kPvol_m <= currentSignals.pivot1_m) ||
				(prevSignal.kPvol_m >= prevSignal.parab_m && currentSignals.kPvol_m <= currentSignals.parab_m)
			)
			{
				
				
				DRAWOBJ_TEXT(runName, rtdata.getTime(),"GERMAN_INDIC_TEXT", rtdata.getSymbol2(), rtdata.getProvider2(), BrkLib::AP_HIGH, "SIGNAL OPEN SHORT", "" );
				DRAWOBJ_TEXT(runName, rtdata.getTime(),"GERMAN_INDIC_TEXT", rtdata.getSymbol2(), rtdata.getProvider2(), BrkLib::AP_HIGH, getTradeState(tradeState_m[rtdata.getSymbol2()]), "" );


				if (tradeState_m.size()==0 || tradeState_m[rtdata.getSymbol2()]==TSG_Initial) {
					BrkLib::BrokerOperation oper(brkConnect);
					BrkLib::Order order;

					// sell
					BrkLib::initMarketOrder(order, rtdata.getProvider2(), rtdata.getSymbol2(), "", false, -1,-1,-1,100,-1,rtdata.getTime());
					oper.unblockingIssueOrder(order);

					// open short
					DRAWOBJ_TEXT(runName, rtdata.getTime(),"GERMAN_INDIC_TEXT", rtdata.getSymbol2(), rtdata.getProvider2(), BrkLib::AP_HIGH, "OPEN SHORT", "" );
				}
			}

			if (
				(prevSignal.kPvol_m <= prevSignal.pivot2_m && currentSignals.kPvol_m >= currentSignals.pivot2_m) ||
				(prevSignal.kPvol_m <= prevSignal.pivot1_m && currentSignals.kPvol_m >= currentSignals.pivot1_m) ||
				(prevSignal.kPvol_m <= prevSignal.parab_m && currentSignals.kPvol_m >= currentSignals.parab_m)
			) 
			{
				// close short
				DRAWOBJ_TEXT(runName, rtdata.getTime(),"GERMAN_INDIC_TEXT", rtdata.getSymbol2(), rtdata.getProvider2(), BrkLib::AP_HIGH, "SIGNAL CLOSE SHORT", "" );
				DRAWOBJ_TEXT(runName, rtdata.getTime(),"GERMAN_INDIC_TEXT", rtdata.getSymbol2(), rtdata.getProvider2(), BrkLib::AP_HIGH, getTradeState(tradeState_m[rtdata.getSymbol2()]), "" );


				if (tradeState_m.size()>0 && tradeState_m[rtdata.getSymbol2()]==TSG_OpenShort) { 
					BrkLib::BrokerOperation oper(brkConnect);
					BrkLib::Order order;  

					 
					// sell
					BrkLib::initCloseOrder(order, rtdata.getProvider2(), rtdata.getSymbol2(), "", openPosId_m[rtdata.getSymbol2()], rtdata.getTime());
					oper.unblockingIssueOrder(order);

					// close short
					DRAWOBJ_TEXT(runName, rtdata.getTime(),"GERMAN_INDIC_TEXT", rtdata.getSymbol2(), rtdata.getProvider2(), BrkLib::AP_HIGH, "CLOSE SHORT", "" );
				}
			}

		}
		

		//
		prevSignal = currentSignals;
		
	}  

	
}

void MGerman::onLevel2DataArrived(
		HlpStruct::RtLevel2Data const& level2data,
		Inqueue::ContextAccessor& historyaccessor, 
		Inqueue::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
	)
{

	
}

void MGerman::onLevel1DataArrived(
		HlpStruct::RtLevel1Data const& level1data,
		Inqueue::ContextAccessor& historyaccessor, 
		Inqueue::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
	)
{
	

}

void MGerman::onEventArrived(
				Inqueue::ContextAccessor& contextaccessor, 
				HlpStruct::CallingContext& callContext,
				bool& result,
				CppUtils::String &customOutputData
		)
{
	result=false;
}


void MGerman::onThreadStarted(Inqueue::ContextAccessor& contextaccessor, bool const firstLib, bool const lastLib) {

}


CppUtils::String const& MGerman::getName() const
{
	static const CppUtils::String build_date(__TIMESTAMP__);
	static const CppUtils::String name( "[ german algorithm ] - build: " + build_date );
	return name;
}

/**
* Helpers
*/



}; // end of namespace