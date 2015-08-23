
#include "impl.hpp"
#include "../brklib/brklib.hpp"

// math stuff
#include "../math/math.hpp"

#define ALGSILVER2 "ALGSILVER2"



namespace AlgLib {
	
	static id_m = 0;
	
	AlgorithmBase *creator (Inqueue::AlgorithmHolder* algholder)
	{
		return new SilverImpl(*algholder);
	}

	void terminator (AlgorithmBase *pthis)
	{
		assert(pthis);
		delete pthis;
	}

	AlgorithmBase::CreatorAlgorithmFun createAlgorithm = creator;

	AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm = terminator;


	// intreface to be implemented

	SilverImpl::SilverImpl( Inqueue::AlgorithmHolder& algHolder):
		AlgorithmBase(algHolder),
		spread_m(-1)
	{
		
	}

	SilverImpl::~SilverImpl()
	{
	}

	void SilverImpl::onLibraryInitialized(
		Inqueue::AlgBrokerDescriptor const& descriptor, 
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
	)
	{
	
		justStarted_m  = true;
		

		// mult factor and aggregation period
		
		aggrPeriod_m = Inqueue::aggrPeriodFromString()(descriptor.initialAlgBrkParams_m.algParams_m.getParameter("AGGRPER").getAsString().c_str());
		
		multFactor_m = atoi(descriptor.initialAlgBrkParams_m.algParams_m.getParameter("MULTFACT").getAsString().c_str());
		
		//
		tradeSymbol_m = descriptor.initialAlgBrkParams_m.algParams_m.getParameter("TRADE_SYMBOL").getAsString();

		tradeProvider_m = descriptor.initialAlgBrkParams_m.algParams_m.getParameter("TRADE_PROVIDER").getAsString();

		// silver channel trade parameters
		tenkan_m = atof(descriptor.initialAlgBrkParams_m.algParams_m.getParameter("TENKAN").getAsString().c_str());
		kijun_m = atof(descriptor.initialAlgBrkParams_m.algParams_m.getParameter("KIJUN").getAsString().c_str());
		senkou_m = atof(descriptor.initialAlgBrkParams_m.algParams_m.getParameter("SENKOU").getAsString().c_str());

		ssp_m = atoi(descriptor.initialAlgBrkParams_m.algParams_m.getParameter("SSP").getAsString().c_str());
		tsp_m = atoi(descriptor.initialAlgBrkParams_m.algParams_m.getParameter("TSP").getAsString().c_str());

		prof_loss_ratio_m = atof(descriptor.initialAlgBrkParams_m.algParams_m.getParameter("PROF_LOSS_RATIO").getAsString().c_str());

		stop_loss_pips_m = atof(descriptor.initialAlgBrkParams_m.algParams_m.getParameter("STOP_LOSS_PIPS").getAsString().c_str());

		posVolume_m = atof(descriptor.initialAlgBrkParams_m.algParams_m.getParameter("VOLUME").getAsString().c_str());

		priceCloseLevel_m = atof(descriptor.initialAlgBrkParams_m.algParams_m.getParameter("PRICE_CLOSE_LEVEL").getAsString().c_str());

		// on simualtion mode we may need this param
		if (descriptor.initialAlgBrkParams_m.algParams_m.isKeyExists("SPRED_SIMUL"))
			spread_m = atof(descriptor.initialAlgBrkParams_m.algParams_m.getParameter("SPRED_SIMUL").getAsString().c_str());

		if (tradeSymbol_m.size() >= 6) {
				tradeSymbolNum_m = tradeSymbol_m.substr(0,3);
				tradeSymbolDenom_m = tradeSymbol_m.substr(3,3);
		}

		// pre-load history bars number
		preloadHistoryData_m = max(ssp_m + tsp_m, senkou_m + kijun_m);

		
		LOGEVENTA(HlpStruct::CommonLog::LL_INFO,  ALGSILVER2, "Library Initialized" ,	
			"aggrPeriod" << Inqueue::aggrPeriodToString()(aggrPeriod_m) 	
			<< "multFactor" << multFactor_m 
			<< "symbol" << tradeSymbol_m 
			<< "tradeProvider" << tradeProvider_m
			<< "tenkan" << tenkan_m
			<< "kijun" << kijun_m
			<< "senkou" << senkou_m
			<< "ssp" << ssp_m
			<< "tsp" << tsp_m	
			<< "stop_loss_pips" << stop_loss_pips_m
			<< "tradeSymbolNum" << tradeSymbolNum_m
			<< "tradeSymbolDenom" << tradeSymbolDenom_m
			<< "posVolume" << posVolume_m
			<< "priceCloseLevel" << priceCloseLevel_m
			<< "preloadHistoryData" << preloadHistoryData_m
			);


		// alert
		ALERTEVENTNC(runName, BrkLib::AP_MEDIUM, "Silver Library initialized with parameters: " << ", " <<
			"aggrPeriod" << "=" << Inqueue::aggrPeriodToString()(aggrPeriod_m) << ", " 
			<< "multFactor" << "=" << multFactor_m << ", " 
			<< "symbol" << "=" << tradeSymbol_m  << ", " 
			<< "tradeProvider" << "=" << tradeProvider_m  << ", " 
			<< "tenkan" << "=" << tenkan_m << ", " 
			<< "kijun" << "=" << kijun_m << ", " 
			<< "senkou" << "=" << senkou_m << ", " 
			<< "ssp" << "=" << ssp_m << ", " 
			<< "tsp" << "=" << tsp_m	<< ", " 
			<< "stop_loss_pips" << "=" << stop_loss_pips_m << ", " 
			<< "tradeSymbolNum" << "=" << tradeSymbolNum_m << ", " 
			<< "tradeSymbolDenom" << "=" << tradeSymbolDenom_m << ", " 
			<< "posVolume" << "=" << posVolume_m << ", " 
			<< "priceCloseLevel" << "=" << priceCloseLevel_m << ", " 
			<< "preloadHistoryData" << "=" << preloadHistoryData_m
		);

		if (brokerlib==NULL) {
			LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  ALGSILVER2, "On initialization broker pointer is invalid");

			return;
		}

		
		//
		BrkLib::BrokerConnect brkConnect(brokerlib, "dummy", false, true, runName.c_str(), comment.c_str());
		brkConnect.connect();
		
		
		if (!brkConnect.getSession().isConnected()) {
			LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  ALGSILVER2, "On initialization session cannot be established: " << brkConnect.getSession().getConnection());
			return;
		}

		// set initial state
		brkConnect.getSession().setState(STATE_UNDEFINED);
	
	
		// handshake 
		sendHandshakeOrder(brkConnect);		

		// metainfo order
		sendMetainfoOrder(brkConnect);
									
				
	}

	void SilverImpl::onLibraryFreed()
	{
		LOGEVENT(HlpStruct::CommonLog::LL_INFO,  ALGSILVER2, "Library deinitialized" );
		ALERTEVENTNC("", BrkLib::AP_MEDIUM, "Silver Library deinitialized");
	}

	void SilverImpl::onBrokerResponseArrived(
			BrkLib::BrokerConnect &brkConnect,
			BrkLib::BrokerResponseBase const& bresponse
	)
	{
		

		// metainfo
		if (bresponse.type_m==BrkLib::RT_BrokerResponseMetaInfo) {
			BrkLib::BrokerResponseMetaInfo const& bRespMinfo = (BrkLib::BrokerResponseMetaInfo const&)bresponse;

			if ((bRespMinfo.resultCode_m == BrkLib::PT_GENERAL_INFO) && ((bRespMinfo.resultDetailCode_m & BrkLib::ODR_OK) > 0)) {
			
				// set up spread
				spread_m = bRespMinfo.spread_m / 1000.0;

				LOGEVENTA(HlpStruct::CommonLog::LL_INFO,  ALGSILVER2, "Metainfo arrived" ,	
					"symbol NUM" << bRespMinfo.symbolNum_m
					<< "symbol DENOM" << bRespMinfo.symbolDenom_m
					<< "spread" << spread_m);
			}
		}

		// order execution
		if (bresponse.type_m == BrkLib::RT_BrokerResponseOrder ) {

			// mtx this from data arrival
			LOCK_FOR_SCOPE(eventProcessingMtx_m);
			
			// here we mae need to change state of the machine
			BrkLib::BrokerResponseOrder const& brespOrder = (BrkLib::BrokerResponseOrder const&)bresponse;
			
			// generate event on broker response arrival
			int event = generateBrokerResponseSignal(brespOrder, brkConnect);

			//int oldstate = brkConnect.getSession().getState();

			processEvent(event, brkConnect);

			/*
			int newstate = brkConnect.getSession().getState();

			// this is to avoid MT4 - it send only close
			if (event==EVENT_BROKER_RESP_LONG_ORDER_CLOSED) {
				if (oldstate == newstate) {
					int newevent = EVENT_BROKER_RESP_LONG_ORDER_CANCELLED;
					processEvent(newevent, brkConnect);

				}
			}
			else if (event==EVENT_BROKER_RESP_SHORT_ORDER_CLOSED) {
				if (oldstate == newstate) {
					int newevent = EVENT_BROKER_RESP_SHORT_ORDER_CANCELLED;
					processEvent(newevent, brkConnect);

				}
			}
			*/

		}

	}  

	// will implement state machine
	void SilverImpl::onDataArrived(
		HlpStruct::RtData const& rtdata, 
		Inqueue::ContextAccessor& historyaccessor, 
		Inqueue::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
	)
	{
				
		// check symbol - return if not ours

		if (rtdata.getProvider() != tradeProvider_m)
			return;

		if (rtdata.getSymbol() != tradeSymbol_m)
			return;

		if (spread_m < 0) {
			LOGEVENT(HlpStruct::CommonLog::LL_DEBUG,  ALGSILVER2, "On data arrival spread is not initialized");
			return;
		}

		LOGEVENTA(HlpStruct::CommonLog::LL_DEBUGDETAIL,  ALGSILVER2, "*****On data arrival processing ticker" , rtdata.toString());
		
		int symbol_id = historyaccessor.cacheSymbol(rtdata.getProvider(), rtdata.getSymbol(), aggrPeriod_m, multFactor_m );
		if (symbol_id < 0) {
			LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  ALGSILVER2, "On data arrival cannot cache data");
			return;
		}

		long cursize = historyaccessor.getSize(symbol_id);
		if (cursize < preloadHistoryData_m) {
			LOGEVENT(HlpStruct::CommonLog::LL_DEBUG,  ALGSILVER2, "On data arrival not enough history data");
			return;
		}

  
		// get unclosed candle
		HlpStruct::PriceData const& unclosed = historyaccessor.getUnclosedCandle(symbol_id);

		// whether unclosed is invalid
		if (!unclosed.isValid()) {
			LOGEVENT(HlpStruct::CommonLog::LL_DEBUG,  ALGSILVER2, "On data arrival unclosed bar is not valid");
			return;
		}
		
		// try to recalculate silver parameters on start up
		if (justStarted_m) {
			LOGEVENT(HlpStruct::CommonLog::LL_DEBUG,  ALGSILVER2, "On data arrival we has started, recalculating silver parameters" );

			if (!recalculateSilverParameters(symbol_id, historyaccessor, rtdata)) {
				LOGEVENT(HlpStruct::CommonLog::LL_DEBUG,  ALGSILVER2, "On data arrival not enough history data");
				return;
			}

			// finish this attempt only on success
			justStarted_m = false;
		}
		
		
		// whether need to recalculate parameters
		if (unclosed.isNewPeriod()) {

			// logging
			LOGEVENTA(HlpStruct::CommonLog::LL_DEBUG,  ALGSILVER2, "On data arrival new period switch" , unclosed.toString() );
			

			// recalculate
			if (!recalculateSilverParameters(symbol_id, historyaccessor, rtdata)) {
				LOGEVENT(HlpStruct::CommonLog::LL_DEBUG,  ALGSILVER2, "On data arrival not enough history data");
				return;
			}

			// do alert each period
			doAlertEachPeriod(runName, historyaccessor, rtdata);	
			
		}
		

		// do alert each tick
		doAlertOnEachTick(runName, historyaccessor, rtdata);


		if (brokerlib==0) {
			// no broker library
			return;
		}

		if (!isSilverparametersInitialized(historyaccessor)){
			LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  ALGSILVER2, "On data arrival silver parameters are not initialized");
			return;
		}

		
		BrkLib::BrokerConnect brkConnect(brokerlib, "dummy", false, true, runName.c_str(), comment.c_str());
		brkConnect.connect();
	
		
		if (!brkConnect.getSession().isConnected()) {
			LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  ALGSILVER2,"On data arrival session cannot be established: " << brkConnect.getSession().getConnection() );
			return;
		}

		// do not change the state by broker response
		LOCK_FOR_SCOPE(eventProcessingMtx_m);
		
		// check the initial state 
		if (brkConnect.getSession().getState()==STATE_UNDEFINED)
			initialCheckState(historyaccessor, brkConnect);

		// time to check upon each tick what we have
		int tick_event = generateEachTickSignal(historyaccessor, rtdata, brkConnect);
		processEvent(tick_event, brkConnect);	

		
		// time to check upon period finish
		if (unclosed.isNewPeriod()) {
			int period_event = generatePeriodFinishedSignal(historyaccessor, brkConnect);
			processEvent(period_event, brkConnect);

			
		}


	}

	
	
	bool SilverImpl::onEventArrived(HlpStruct::EventData const &eventdata, HlpStruct::EventData& response)
	{
		// we may need to pass arbitrary data here
		return false;
	}

	CppUtils::String const& SilverImpl::getName() const
	{
		static const CppUtils::String name("Alex Silver 2");
		return name;
	}

	/**
	Helpers
	*/
	
	// checks for automat state at the initial stage
	void SilverImpl::initialCheckState(Inqueue::ContextAccessor& historyaccessor, BrkLib::BrokerConnect& brkConnect)
	{
		
		// here we shall check what about positions to determine the state of automat
		BrkLib::BrokerOperation oper(brkConnect);
		BrkLib::PositionList const& poslist = oper.getBrokerPositionList(BrkLib::QUERY_HISTORY | BrkLib::QUERY_OPEN | BrkLib::QUERY_PENDING);
		
		// look for our position taking into account broker comment - this should contain our string
		// we cannot track here whether order was asked because this initial check
		
		int state = STATE_NO_OPEN_POS;
		brkConnect.getSession().getSessionStorage().getGlobal("valid_order_id").getAsString(true) = "";
		brkConnect.getSession().getSessionStorage().getGlobal("valid_order_installprice").getAsDouble(true) = -1;
		brkConnect.getSession().getSessionStorage().getGlobal("valid_order_executeprice").getAsDouble(true) = -1;

		CppUtils::String posId;
		double installprice, executeprice;

		for(int i = 0; i < poslist.size(); i++) {
			BrkLib::Position const& pos = poslist[ i ];

			// need to discover open or 
			//if (pos.comment_m.find(ORDER_COMMENT) <0)
			//	continue;

			if (state != STATE_NO_OPEN_POS)
				// out not changing the state - this is undefined 
				return;

			if (pos.ID_m.isValid() && pos.posState_m == BrkLib::STATE_OPEN ) {
				if (pos.lastPosType_m==BrkLib::POS_BUY) {
					state = STATE_LONG_EXECUTED;
				}
				else if (pos.lastPosType_m==BrkLib::POS_SELL) {
					state = STATE_SHORT_EXECUTED;
				}
				
				
				posId = pos.brokerPositionID_m;
				executeprice = pos.executePrice_m;
			}
			else if (pos.ID_m.isValid() && pos.posState_m == BrkLib::STATE_PENDING ) {
				if (pos.lastPosType_m==BrkLib::POS_STOP_BUY) {
					state = STATE_LONG_INSTALLED;
				}
				else if (pos.lastPosType_m==BrkLib::POS_STOP_SELL) {
					state = STATE_SHORT_INSTALLED;
				}
				posId = pos.brokerPositionID_m;
				installprice = pos.installPrice_m;
			}

		}; // end loop
		
		// we found only one valid position or do not find at all - change the state
		brkConnect.getSession().setState(state);
		if (state != STATE_NO_OPEN_POS) {
			// set order ID if any
			// to the session context!!!
			brkConnect.getSession().getSessionStorage().getGlobal("valid_order_id").getAsString(true) = posId;
			brkConnect.getSession().getSessionStorage().getGlobal("valid_order_installprice").getAsDouble(true)  = installprice;
			brkConnect.getSession().getSessionStorage().getGlobal("valid_order_executeprice").getAsDouble(true) = executeprice;
		}
		
		
	};

	int SilverImpl::generateBrokerResponseSignal(
		BrkLib::BrokerResponseOrder const& brespOrder, 
		BrkLib::BrokerConnect& brkConnect
	)
	{

		int newevent = EVENT_BROKER_RESP_NO_SIGNAL;

		//if (brespOrder.brokerComment_m.find(ORDER_COMMENT) <0)
			// this is not our order
		//	return newevent;

		CppUtils::String posId;
		
		if (brespOrder.resultCode_m ==BrkLib::PT_ORDER_ESTABLISHED ) {

			if (brespOrder.resultDetailCode_m & BrkLib::ODR_ORDER_INSTALLED) {
				// order can be installed
				
				if (brespOrder.positionType_m == BrkLib::POS_LIMIT_BUY) {
					newevent = EVENT_BROKER_RESP_LONG_ORDER_INSTALLED;
				}
				else if (brespOrder.positionType_m == BrkLib::POS_LIMIT_SELL) {
					newevent = EVENT_BROKER_RESP_SHORT_ORDER_INSTALLED;
				}
				
				brkConnect.getSession().getSessionStorage().getGlobal("valid_order_installprice").getAsDouble(true)  = brespOrder.opOpenPrice_m;
			}
			else if (brespOrder.resultDetailCode_m & BrkLib::ODR_ORDER_EXECUTED) {
				// or already installed order can be executed

				if (brespOrder.positionType_m == BrkLib::POS_BUY) {
					newevent = EVENT_BROKER_RESP_LONG_ORDER_EXECUTED;
				}
				else if (brespOrder.positionType_m == BrkLib::POS_SELL) {
					newevent = EVENT_BROKER_RESP_SHORT_ORDER_EXECUTED;
				}
				
				brkConnect.getSession().getSessionStorage().getGlobal("valid_order_executeprice").getAsDouble(true) = brespOrder.opOpenPrice_m;
			}
			posId = brespOrder.brokerPositionID_m;
		}
		else if (brespOrder.resultCode_m ==BrkLib::PT_ORDER_MODIFIED ) {
			
			// order either installed or executed was modified

			if (brespOrder.positionType_m == BrkLib::POS_LIMIT_BUY) {
				newevent = EVENT_BROKER_RESP_LONG_ORDER_MODIFIED;
				
				if (brespOrder.resultDetailCode_m & BrkLib::ORD_EXECUTE_PRICE_CHANGED_OK) {
					brkConnect.getSession().getSessionStorage().getGlobal("valid_order_installprice").getAsDouble(true)  = brespOrder.opOpenPrice_m;
				}
			}
			else if (brespOrder.positionType_m == BrkLib::POS_LIMIT_SELL) {
				newevent = EVENT_BROKER_RESP_SHORT_ORDER_MODIFIED;
				
				if (brespOrder.resultDetailCode_m & BrkLib::ORD_EXECUTE_PRICE_CHANGED_OK) {
					brkConnect.getSession().getSessionStorage().getGlobal("valid_order_installprice").getAsDouble(true)  = brespOrder.opOpenPrice_m;
				}
			}
			else if (brespOrder.positionType_m == BrkLib::POS_BUY) {
				newevent = EVENT_BROKER_RESP_LONG_ORDER_MODIFIED;
				
				if (brespOrder.resultDetailCode_m & BrkLib::ORD_EXECUTE_PRICE_CHANGED_OK) {
					brkConnect.getSession().getSessionStorage().getGlobal("valid_order_executeprice").getAsDouble(true) = brespOrder.opOpenPrice_m;
				}
			}
			else if (brespOrder.positionType_m == BrkLib::POS_SELL) {
				newevent = EVENT_BROKER_RESP_SHORT_ORDER_MODIFIED;
				
				if (brespOrder.resultDetailCode_m & BrkLib::ORD_EXECUTE_PRICE_CHANGED_OK) {
					brkConnect.getSession().getSessionStorage().getGlobal("valid_order_executeprice").getAsDouble(true) = brespOrder.opOpenPrice_m;
				}
			}

			posId = brespOrder.brokerPositionID_m;
		}
		else if (brespOrder.resultCode_m ==BrkLib::PT_ORDER_CLOSED ) {
			if (brespOrder.positionType_m == BrkLib::POS_BUY) {
				newevent = EVENT_BROKER_RESP_LONG_ORDER_CLOSED;
			}
			else if (brespOrder.positionType_m == BrkLib::POS_SELL) {
				newevent = EVENT_BROKER_RESP_SHORT_ORDER_CLOSED;
			}
			posId = brespOrder.brokerPositionID_m;
		}
		else if (brespOrder.resultCode_m ==BrkLib::PT_ORDER_CANCELLED ) {
			if (brespOrder.positionType_m == BrkLib::POS_LIMIT_BUY) {
				newevent = EVENT_BROKER_RESP_LONG_ORDER_CANCELLED;
			}
			else if (brespOrder.positionType_m == BrkLib::POS_LIMIT_SELL) {
				newevent = EVENT_BROKER_RESP_SHORT_ORDER_CANCELLED;
			}
			posId = brespOrder.brokerPositionID_m;
		}
		// errors
		else if (brespOrder.resultCode_m ==BrkLib::PT_CANNOT_ESTABLISH_ORDER ) {
			newevent = EVENT_BROKER_REJECTED_ORDER_OPER;
			posId = brespOrder.brokerPositionID_m;
		}
		else if (brespOrder.resultCode_m ==BrkLib::PT_CANNOT_MODIFY_ORDER ) {
			newevent = EVENT_BROKER_REJECTED_ORDER_OPER;
			posId = brespOrder.brokerPositionID_m;
		}
		else if (brespOrder.resultCode_m ==BrkLib::PT_CANNOT_CLOSE_ORDER ) {
			newevent = EVENT_BROKER_REJECTED_ORDER_OPER;
			posId = brespOrder.brokerPositionID_m;
		}
		else if (brespOrder.resultCode_m ==BrkLib::PT_CANNOT_CANCEL_ORDER ) {
			newevent = EVENT_BROKER_REJECTED_ORDER_OPER;
			posId = brespOrder.brokerPositionID_m;
		}
				
		brkConnect.getSession().getSessionStorage().getGlobal("valid_order_id").getAsString(true) = posId;

		return newevent;
	}

	void SilverImpl::onLevel2DataArrived(
			HlpStruct::RtLevel2Data const& level2data,
			Inqueue::TradingParameters const& descriptor,
			CppUtils::String const& runName,
			CppUtils::String const& comment
	)
	{
	}
	

	void SilverImpl::sendHandshakeOrder(BrkLib::BrokerConnect& brkConnect)
	{
		// handshake order
		BrkLib::Order handShake;

		
		handShake.orderType_m = BrkLib::OP_HANDSHAKE;
		handShake.comment_m = "Handshake Order";

		BrkLib::BrokerOperation oper(brkConnect);
		oper.unblockingIssueOrder( handShake);
	}

	void SilverImpl::sendMetainfoOrder(BrkLib::BrokerConnect& brkConnect)
	{
		BrkLib::Order mInfo;
		mInfo.orderType_m = BrkLib::OP_GET_SYMBOL_METAINFO;
		mInfo.symbolNum_m = tradeSymbolNum_m;
		mInfo.symbolDenom_m = tradeSymbolDenom_m;

		BrkLib::BrokerOperation oper(brkConnect);
		oper.unblockingIssueOrder( mInfo);
	}
		
	bool SilverImpl::recalculateSilverParameters(
		int symbol_id,
		Inqueue::ContextAccessor& historyaccessor,
		HlpStruct::RtData const& rtdata
	)
	{
		// silver array
		int maxSilverInd = ssp_m + tsp_m;
		HlpStruct::PriceDataList dataListChannels(maxSilverInd);

		// ichimoka arr
		int maxIchimokaInd = senkou_m + kijun_m;
		HlpStruct::PriceDataList ichimokaArr(maxIchimokaInd);
		
		
		// main loop through the data
		int cnt = 0;

		
		double close_price_period;
		double open_price_period;
		double high_price_period;
		double low_price_period;

		double time_price_period;

		double close_price_period_prev; // close price for previous period

		int maxBreakInd = max(maxSilverInd, maxIchimokaInd);

		
		// calculate here Silver Channel parameters
		Inqueue::DataPtr ptr = historyaccessor.dataLast(symbol_id);
		
		HlpStruct::PriceData last_price_candle;
		while(historyaccessor.dataPrev(symbol_id,ptr)) {
			HlpStruct::PriceData const &pd = historyaccessor.getCurrentPriceData(symbol_id, ptr);

			if (cnt==0) {
				// bid prices, this returns open and close prices of a period
				// note  - this is previous period
				close_price_period = pd.close2_m;
				open_price_period = pd.open2_m;
				high_price_period = pd.high2_m;
				low_price_period = pd.low2_m;
				time_price_period = pd.time_m;

				// store last price data
				last_price_candle = pd;

				LOGEVENTA(HlpStruct::CommonLog::LL_DEBUG,  ALGSILVER2, "Previous candle" , pd.toString() );

			}

			if (cnt==1) {
				LOGEVENTA(HlpStruct::CommonLog::LL_DEBUG,  ALGSILVER2, "Pre-previous candle" , pd.toString() );
				close_price_period_prev = pd.close2_m;
			}

			// break if 
			if (cnt >= maxBreakInd )
				break;

			if (cnt < maxSilverInd)
				dataListChannels[ cnt ] = pd;

			if (cnt < maxIchimokaInd)
				ichimokaArr[cnt] = pd;
			
			//advance
			cnt++;
		}

		// not enough data
		if (cnt < (maxBreakInd-1)) {
			return false;
		}

		// there is only bid pricess available in btest

		// silver parameters

		// begin from the shift e.i. iteration from 0 to SSP - e.i this iteration through least recent SSP bars
		double sc_high = Math::calcMax(dataListChannels, Math::P_HighBid, 0, ssp_m);
		double sc_low = Math::calcMin(dataListChannels, Math::P_LowBid, 0, ssp_m);

		double sc_dist = sc_high - sc_low;
		
		double sc_silver_low=sc_low+sc_dist*SILVCH/100.0;
    double sc_silver_high=sc_high-sc_dist*SILVCH/100.0;
    
    double sc_sky_low = sc_low+sc_dist*SKYCH/100.0;
    double sc_sky_high = sc_high-sc_dist*SKYCH/100.0;

    double sc_zen_low = sc_low+sc_dist*ZENCH/100.0;
    double sc_zen_high = sc_high-sc_dist*ZENCH/100.0;

		// future parameters
		double sc_highfut = Math::calcMax(dataListChannels, Math::P_HighBid, tsp_m, maxSilverInd);
		double sc_lowfut = Math::calcMin(dataListChannels, Math::P_LowBid, tsp_m, maxSilverInd);
		double sc_future_dist = sc_highfut - sc_lowfut;
                
    double sc_future_low = sc_lowfut-sc_future_dist*FUTCH/100;
    double sc_future_high = sc_highfut+sc_future_dist*FUTCH/100;
		// currently we have everything, will store this        

		// save this to global
		historyaccessor.getThreadGlobal().getGlobal("sc_high").getAsDouble(true) = sc_high;
		historyaccessor.getThreadGlobal().getGlobal("sc_low").getAsDouble(true) = sc_low;
		
		historyaccessor.getThreadGlobal().getGlobal("sc_silver_low").getAsDouble(true) = sc_silver_low;
		historyaccessor.getThreadGlobal().getGlobal("sc_silver_high").getAsDouble(true) = sc_silver_high;

		historyaccessor.getThreadGlobal().getGlobal("sc_sky_low").getAsDouble(true) = sc_sky_low;
		historyaccessor.getThreadGlobal().getGlobal("sc_sky_high").getAsDouble(true) = sc_sky_high;

		historyaccessor.getThreadGlobal().getGlobal("sc_zen_low").getAsDouble(true) = sc_zen_low;
		historyaccessor.getThreadGlobal().getGlobal("sc_zen_high").getAsDouble(true) = sc_zen_high;

		historyaccessor.getThreadGlobal().getGlobal("sc_future_low").getAsDouble(true) = sc_future_low;
		historyaccessor.getThreadGlobal().getGlobal("sc_future_high").getAsDouble(true) = sc_future_high;

		historyaccessor.getThreadGlobal().getGlobal("open_price_period").getAsDouble(true) = open_price_period;
		historyaccessor.getThreadGlobal().getGlobal("close_price_period").getAsDouble(true) = close_price_period;
		historyaccessor.getThreadGlobal().getGlobal("high_price_period").getAsDouble(true) = high_price_period;
		historyaccessor.getThreadGlobal().getGlobal("low_price_period").getAsDouble(true) = low_price_period;

		historyaccessor.getThreadGlobal().getGlobal("time_price_period").getAsDouble(true) = time_price_period;
		

		//
		historyaccessor.getThreadGlobal().getGlobal("close_price_period_prev").getAsDouble(true) = close_price_period_prev;

		// calculate Ichimoka
		double sc_tenkan_max = Math::calcMax(ichimokaArr, Math::P_HighBid,0, tenkan_m);
		double sc_tenkan_min = Math::calcMin(ichimokaArr, Math::P_LowBid,0, tenkan_m);
		
		double sc_tenkan = (sc_tenkan_min+sc_tenkan_max)/2.0;

		double sc_kijun_max = Math::calcMax(ichimokaArr, Math::P_HighBid,0, kijun_m);
		double sc_kijun_min = Math::calcMin(ichimokaArr, Math::P_LowBid,0, kijun_m);

		double sc_kijun = (sc_kijun_min+sc_kijun_max)/2.0;
		
		
		double sc_sencouB_min = Math::calcMin(ichimokaArr, Math::P_LowBid,kijun_m,kijun_m+senkou_m);
		double sc_sencouB_max = Math::calcMax(ichimokaArr, Math::P_HighBid,kijun_m,kijun_m+senkou_m);
		double sc_sencouB = (sc_sencouB_min+sc_sencouB_max)/2.0;

		// claculate sc_sencouA we need sc_kijun & sc_tenkan kijun_m bars before
		double sc_kijun_max_prev = Math::calcMax(ichimokaArr, Math::P_HighBid,kijun_m, kijun_m + kijun_m);
		double sc_kijun_min_prev = Math::calcMin(ichimokaArr, Math::P_LowBid,kijun_m, kijun_m + kijun_m);
		double sc_kijun_prev = (sc_kijun_min_prev+sc_kijun_max_prev)/2.0;

		double sc_tenkan_max_prev = Math::calcMax(ichimokaArr, Math::P_HighBid,kijun_m, tenkan_m + kijun_m);
		double sc_tenkan_min_prev = Math::calcMin(ichimokaArr, Math::P_LowBid,kijun_m, tenkan_m + kijun_m);
		double sc_tenkan_prev = (sc_tenkan_min_prev+sc_tenkan_max_prev)/2.0;

		double sc_sencouA = (sc_kijun_prev + sc_tenkan_prev)/2.0;
		// store this
		historyaccessor.getThreadGlobal().getGlobal("sc_tenkan_max").getAsDouble(true) = sc_tenkan_max;
		historyaccessor.getThreadGlobal().getGlobal("sc_tenkan_min").getAsDouble(true) = sc_tenkan_min;
		historyaccessor.getThreadGlobal().getGlobal("sc_tenkan").getAsDouble(true) = sc_tenkan;

		historyaccessor.getThreadGlobal().getGlobal("sc_kijun_max").getAsDouble(true) = sc_kijun_max;
		historyaccessor.getThreadGlobal().getGlobal("sc_kijun_min").getAsDouble(true) = sc_kijun_min;
		historyaccessor.getThreadGlobal().getGlobal("sc_kijun").getAsDouble(true) = sc_kijun;

		historyaccessor.getThreadGlobal().getGlobal("sc_sencouB_min").getAsDouble(true) = sc_sencouB_min;
		historyaccessor.getThreadGlobal().getGlobal("sc_sencouB_max").getAsDouble(true) = sc_sencouB_max;
		historyaccessor.getThreadGlobal().getGlobal("sc_sencouB").getAsDouble(true) = sc_sencouB;

		historyaccessor.getThreadGlobal().getGlobal("sc_kijun_max_prev").getAsDouble(true) = sc_kijun_max_prev;
		historyaccessor.getThreadGlobal().getGlobal("sc_kijun_min_prev").getAsDouble(true) = sc_kijun_min_prev;
		historyaccessor.getThreadGlobal().getGlobal("sc_kijun_prev").getAsDouble(true) = sc_kijun_prev;

		historyaccessor.getThreadGlobal().getGlobal("sc_tenkan_max_prev").getAsDouble(true) = sc_tenkan_max_prev;
		historyaccessor.getThreadGlobal().getGlobal("sc_tenkan_min_prev").getAsDouble(true) = sc_tenkan_min_prev;
		historyaccessor.getThreadGlobal().getGlobal("sc_tenkan_prev").getAsDouble(true) = sc_tenkan_prev;

		historyaccessor.getThreadGlobal().getGlobal("sc_sencouA").getAsDouble(true) = sc_sencouA;

		/*
		// will make detailed logging of what we have here
		LOGEVENTA(HlpStruct::CommonLog::LL_DEBUGDETAIL,  ALGSILVER2, "Silver parameters on period finish" ,
			"New ticker time" << CppUtils::getAscTime(rtdata.getTime()) <<
			"open_price_period" << open_price_period << 
			"close_price_period" << close_price_period << 
			"sc_future_high" << sc_future_high << 
			"sc_future_low" << sc_future_low << 
			"sc_zen_high" << sc_zen_high << 
			"sc_zen_low" << sc_zen_low << 
			"sc_sky_high" << sc_sky_high << 
			"sc_sky_low" << sc_sky_low << 
			"sc_silver_high" << sc_silver_high << 
			"sc_silver_low" << sc_silver_low << 
			"sc_high" << sc_high << 
			"sc_low" << sc_low << 

			"sc_sencouB" << sc_sencouB << 
			"sc_sencouA" << sc_sencouA
			
			);
		

		// another log that will be comparing with MT 
		LOGEVENTA(HlpStruct::CommonLog::LL_DEBUGDETAIL,  ALGSILVER2, "MT comparison2" ,
			"Ticker unix time:"+CppUtils::double2String(last_price_candle.time_m) << 
			"GMT ticker time:" + CppUtils::getGmtTime2(last_price_candle.time_m) + " Local ticker time:" + CppUtils::getAscTime2(last_price_candle.time_m) <<
			"Bar open:" + CppUtils::float2String(last_price_candle.open2_m, -1, 4) << 
			"Bar high:" + CppUtils::float2String(last_price_candle.high2_m, -1, 4) << 
			"Bar low:" + CppUtils::float2String(last_price_candle.low2_m, -1, 4) <<
			"Bar close:" + CppUtils::float2String(last_price_candle.close2_m, -1, 4) <<

			"Silver-channel High:"+ CppUtils::float2String(sc_silver_high, -1, 4) <<
			"Silver-channel Low:"+ CppUtils::float2String(sc_silver_low, -1, 4) <<
			"Sky-channel High:"+ CppUtils::float2String(sc_sky_high, -1, 4) <<
			"Sky-channel Low:"+ CppUtils::float2String(sc_sky_low, -1, 4) <<
			"Zen-channel High:"+ CppUtils::float2String(sc_zen_high, -1, 4) <<
			"Zen-channel Low:"+ CppUtils::float2String(sc_zen_low, -1, 4) <<
			"Future-channel Low:"+ CppUtils::float2String(sc_future_low, -1, 4) <<
			"Future-channel High:"+ CppUtils::float2String(sc_future_high, -1, 4) <<
			"Status:closed"
		);

		LOGEVENTA(HlpStruct::CommonLog::LL_DEBUGDETAIL,  ALGSILVER2, "MT comparison3" ,
			"Ticker unix time:"+CppUtils::double2String(last_price_candle.time_m) << 
			"GMT ticker time:" + CppUtils::getGmtTime2(last_price_candle.time_m) + " Local ticker time:" + CppUtils::getAscTime2(last_price_candle.time_m) <<
			"Bar open:" + CppUtils::float2String(last_price_candle.open2_m, -1, 4) << 
			"Bar high:" + CppUtils::float2String(last_price_candle.high2_m, -1, 4) << 
			"Bar low:" + CppUtils::float2String(last_price_candle.low2_m, -1, 4) <<
			"Bar close:" + CppUtils::float2String(last_price_candle.close2_m, -1, 4) <<

			"Tenkan sen:" + CppUtils::float2String(sc_tenkan,-1,4) <<
			"Kijun sen:" + CppUtils::float2String(sc_kijun,-1,4) << 
			"Senkou Span A:" + CppUtils::float2String(sc_sencouA,-1,4) << 
			"Senkou Span B:" + CppUtils::float2String(sc_sencouB, -1, 4) <<
			"Status:closed"

		);
		
		*/

		
			
		return true;

	}

	bool SilverImpl::isSilverparametersInitialized(Inqueue::ContextAccessor& historyaccessor)
	{
		return historyaccessor.getThreadGlobal().isGlobalExist("sc_zen_low");
	}

	int SilverImpl::generatePeriodFinishedSignal(
		Inqueue::ContextAccessor& historyaccessor, 
		BrkLib::BrokerConnect& brkConnect	
	) {
	
		int event = EVENT_PERIOD_CHECK_NO_SIGNAL;
		// calculate signals and make dicision for flat mode
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    bool scy_channel_in_cloud = false;
    //one interpretation - sky low or sky high in the clowd
		double sc_sencouA = historyaccessor.getThreadGlobal().getGlobal("sc_sencouA").getAsDouble();
		double sc_sencouB = historyaccessor.getThreadGlobal().getGlobal("sc_sencouB").getAsDouble();
		double sc_sky_low = historyaccessor.getThreadGlobal().getGlobal("sc_sky_low").getAsDouble();
		double sc_sky_high = historyaccessor.getThreadGlobal().getGlobal("sc_sky_high").getAsDouble();

		double sc_zen_low = historyaccessor.getThreadGlobal().getGlobal("sc_zen_low").getAsDouble();
		double sc_zen_high = historyaccessor.getThreadGlobal().getGlobal("sc_zen_high").getAsDouble();

		if (sc_sencouA >= sc_sencouB) {
			if ((sc_sencouA >= sc_sky_low) && (sc_sencouB <= sc_sky_low)) {
         scy_channel_in_cloud= true;
			}
			if ((sc_sencouA >= sc_sky_high) && (sc_sencouB <= sc_sky_high)) { 
         scy_channel_in_cloud= true;
			}
		}
		else {
			if ((sc_sencouB >= sc_sky_low) && (sc_sencouA <= sc_sky_low)) {
            scy_channel_in_cloud= true;
			}
			if ((sc_sencouB >= sc_sky_high) && (sc_sencouA <= sc_sky_high)) {
            scy_channel_in_cloud= true;
			};
		};
	

		//%other interpretation - cloud inside range sky low and sky high
		if (
			(sc_sky_high >= sc_sencouB) && 
			(sc_sky_low <= sc_sencouB) && 
			(sc_sky_high >= sc_sencouA) && 
			(sc_sky_low <= sc_sencouA)) 
		{
			scy_channel_in_cloud = true;
		};  

		//LOGEVENTA(HlpStruct::CommonLog::LL_DEBUGDETAIL,  ALGSILVER2, "Some parameters" ,
		//	CppUtils::String("scy_channel_in_cloud: ") + (scy_channel_in_cloud ? "true":"false") );

		// open and close price of PREVIOUS period
		double open_price_period = historyaccessor.getThreadGlobal().getGlobal("open_price_period").getAsDouble();
		double close_price_period = historyaccessor.getThreadGlobal().getGlobal("close_price_period").getAsDouble();
		double close_price_period_prev = historyaccessor.getThreadGlobal().getGlobal("close_price_period_prev").getAsDouble();


		// store previos values if applicanle
		if (brkConnect.getSession().getSessionStorage().isGlobalExist("long_tprofit_period")) {
			brkConnect.getSession().getSessionStorage().getGlobal("prev_long_tprofit_period").getAsDouble(true) = 
				brkConnect.getSession().getSessionStorage().getGlobal("long_tprofit_period").getAsDouble();
			
			brkConnect.getSession().getSessionStorage().getGlobal("prev_long_sloss_period").getAsDouble(true) = 
				brkConnect.getSession().getSessionStorage().getGlobal("long_sloss_period").getAsDouble();

			brkConnect.getSession().getSessionStorage().getGlobal("prev_long_execprice_period").getAsDouble(true) = 
				brkConnect.getSession().getSessionStorage().getGlobal("long_execprice_period").getAsDouble();


			brkConnect.getSession().getSessionStorage().getGlobal("prev_short_tprofit_period").getAsDouble(true) = 
				brkConnect.getSession().getSessionStorage().getGlobal("short_tprofit_period").getAsDouble();

			brkConnect.getSession().getSessionStorage().getGlobal("prev_short_sloss_period").getAsDouble(true) = 
				brkConnect.getSession().getSessionStorage().getGlobal("short_sloss_period").getAsDouble();

			brkConnect.getSession().getSessionStorage().getGlobal("prev_short_execprice_period").getAsDouble(true) = 
				brkConnect.getSession().getSessionStorage().getGlobal("short_execprice_period").getAsDouble();

		}

		//
		double short_sloss  = sc_zen_high- stop_loss_pips_m;
		double short_tprofit = sc_sky_low;
		double short_execprice = sc_sky_high;

		// store this 
		brkConnect.getSession().getSessionStorage().getGlobal("short_tprofit_period").getAsDouble(true) = short_tprofit;
		brkConnect.getSession().getSessionStorage().getGlobal("short_sloss_period").getAsDouble(true) = short_sloss;
		brkConnect.getSession().getSessionStorage().getGlobal("short_execprice_period").getAsDouble(true) = short_execprice;

		// for long
		double long_sloss  = sc_zen_low+stop_loss_pips_m;
		double long_tprofit = sc_sky_high+spread_m;
		double long_execprice = sc_sky_low;

		// store this 
		brkConnect.getSession().getSessionStorage().getGlobal("long_tprofit_period").getAsDouble(true) = long_tprofit;
		brkConnect.getSession().getSessionStorage().getGlobal("long_sloss_period").getAsDouble(true) = long_sloss;
		brkConnect.getSession().getSessionStorage().getGlobal("long_execprice_period").getAsDouble(true) = long_execprice;
		
		if   ((close_price_period<=sc_sky_high) && (/*open_price_period - to ignore gaps*/close_price_period_prev>sc_sky_high) && scy_channel_in_cloud) {
        //%short
        
				if (abs((short_tprofit-short_execprice)/(short_execprice-short_sloss)) >= prof_loss_ratio_m) {
       
						event = EVENT_PERIOD_CHECK_SELL_LIMIT;

				};
		}
		else if ((close_price_period>=sc_sky_low) && (/*open_price_period - to ignore gaps*/close_price_period_prev<sc_sky_low)  && scy_channel_in_cloud) {
        //%long
        
				if (abs((long_tprofit-long_execprice)/(long_execprice-long_sloss)) >= prof_loss_ratio_m) {

					event = EVENT_PERIOD_CHECK_BUY_LIMIT;

				};
		}
		else {
			// EVENT_ADOPT_EXECPRICE_SL_TP_ORDER_OPER
			// check whether we may need to change exec price, sl, tp
			if (brkConnect.getSession().getSessionStorage().isGlobalExist("prev_long_tprofit_period")) {
				// previos are existing

			if (brkConnect.getSession().getSessionStorage().getGlobal("prev_long_tprofit_period").getAsDouble() != 
				brkConnect.getSession().getSessionStorage().getGlobal("long_tprofit_period").getAsDouble())
				event = EVENT_ADOPT_EXECPRICE_SL_TP_ORDER_OPER;
			
			else if (brkConnect.getSession().getSessionStorage().getGlobal("prev_long_sloss_period").getAsDouble() != 
				brkConnect.getSession().getSessionStorage().getGlobal("long_sloss_period").getAsDouble())
				event = EVENT_ADOPT_EXECPRICE_SL_TP_ORDER_OPER;

			else if (brkConnect.getSession().getSessionStorage().getGlobal("prev_long_execprice_period").getAsDouble() != 
				brkConnect.getSession().getSessionStorage().getGlobal("long_execprice_period").getAsDouble())
				event = EVENT_ADOPT_EXECPRICE_SL_TP_ORDER_OPER;

			else if (brkConnect.getSession().getSessionStorage().getGlobal("prev_short_tprofit_period").getAsDouble() != 
				brkConnect.getSession().getSessionStorage().getGlobal("short_tprofit_period").getAsDouble())
				event = EVENT_ADOPT_EXECPRICE_SL_TP_ORDER_OPER;

			else if (brkConnect.getSession().getSessionStorage().getGlobal("prev_short_sloss_period").getAsDouble() != 
				brkConnect.getSession().getSessionStorage().getGlobal("short_sloss_period").getAsDouble())
				event = EVENT_ADOPT_EXECPRICE_SL_TP_ORDER_OPER;

			else if (brkConnect.getSession().getSessionStorage().getGlobal("prev_short_execprice_period").getAsDouble() != 
				brkConnect.getSession().getSessionStorage().getGlobal("short_execprice_period").getAsDouble())
				event = EVENT_ADOPT_EXECPRICE_SL_TP_ORDER_OPER;
			
			}

		}


		
		
		return event;

	};

	int SilverImpl::generateEachTickSignal(Inqueue::ContextAccessor& historyaccessor, HlpStruct::RtData const& rtdata, BrkLib::BrokerConnect& brkConnect)
	{
		int event = EVENT_TICK_CHECK_NO_SIGNAL;
		double sc_kijun = historyaccessor.getThreadGlobal().getGlobal("sc_kijun").getAsDouble(true);
		
		// signals
		if (rtdata.getRtData().bid_m < sc_kijun) {
			event = EVENT_TICK_CHECK_PRICE_LESS_KIJUN;
		}
		else if (rtdata.getRtData().bid_m >= sc_kijun){
			event  = EVENT_TICK_CHECK_PRICE_MORE_KIJUN;
		}

		return event;

	}
		
	void SilverImpl::processEvent(int const event, BrkLib::BrokerConnect& brkConnect)
	{
		

		/**
		*/

			int newstate = brkConnect.getSession().getState();

			// logging
			LOGEVENTA(HlpStruct::CommonLog::LL_DEBUGDETAIL,  ALGSILVER2, "State machine event arrived" , event);
			
			// logging
			LOGEVENTA(HlpStruct::CommonLog::LL_DEBUGDETAIL,  ALGSILVER2, "State machine initial state" , newstate);

			// actions on the arc
			if (brkConnect.getSession().getState()==STATE_UNDEFINED) {
			}
			else if (brkConnect.getSession().getState()==STATE_NO_OPEN_POS) {
				// no open pos, ask for open position
				if (event==EVENT_PERIOD_CHECK_BUY_LIMIT) {
					doMachineAction(brkConnect, ACTION_ASK_FOR_LONG_LIMIT_ORDER_INSTALL);
					newstate = STATE_LONG_ASKED_INSTALL;
				}
				else if (event==EVENT_PERIOD_CHECK_SELL_LIMIT) {
					doMachineAction(brkConnect, ACTION_ASK_FOR_SHORT_LIMIT_ORDER_INSTALL);
					newstate = STATE_SHORT_ASKED_INSTALL;
				}
			}
			// asked install
			else if (brkConnect.getSession().getState()==STATE_SHORT_ASKED_INSTALL) {
				if (event==EVENT_BROKER_RESP_SHORT_ORDER_INSTALLED) {
					newstate = STATE_SHORT_INSTALLED;
				}
				else if (event==EVENT_BROKER_REJECTED_ORDER_OPER) {
					newstate=STATE_NO_OPEN_POS;
				}

				
			}
			else if (brkConnect.getSession().getState()==STATE_LONG_ASKED_INSTALL) {
				if (event==EVENT_BROKER_RESP_LONG_ORDER_INSTALLED) {
					newstate = STATE_LONG_INSTALLED;
				}
				else if (event==EVENT_BROKER_REJECTED_ORDER_OPER) {
					newstate=STATE_NO_OPEN_POS;
				}
			}

			// pending orders
			else if (brkConnect.getSession().getState()==STATE_SHORT_INSTALLED) {
				if (event == EVENT_TICK_CHECK_PRICE_LESS_KIJUN) {
					// need to cancel
					doMachineAction(brkConnect, ACTION_ASK_FOR_SHORT_LIMIT_ORDER_CANCEL);
					newstate = STATE_SHORT_ASKED_CANCEL;
				}
				// if order automatically deinstalled
				else if (event == EVENT_BROKER_RESP_SHORT_ORDER_CANCELLED) {
					newstate = STATE_NO_OPEN_POS;
				}
				else if (event==EVENT_BROKER_RESP_SHORT_ORDER_EXECUTED) {
					newstate = STATE_SHORT_EXECUTED;
				}
				else if (event==EVENT_ADOPT_EXECPRICE_SL_TP_ORDER_OPER) {
					doMachineAction(brkConnect, ACTION_ASK_FOR_SHORT_ADOPT_EXEC_TP_SL_ORDER);
					newstate = STATE_SHORT_ASKED_TO_ADOPT_EXEC_SL_TP_ON_INSTALLED_STATE;
				}
				
			}
			else if (brkConnect.getSession().getState()==STATE_LONG_INSTALLED) {
				if (event == EVENT_TICK_CHECK_PRICE_MORE_KIJUN) {
					// need to cancel
					doMachineAction(brkConnect, ACTION_ASK_FOR_LONG_LIMIT_ORDER_CANCEL);
					newstate = STATE_LONG_ASKED_CANCEL;
				}
				// if order automatically deinstalled
				else if (event == EVENT_BROKER_RESP_LONG_ORDER_CANCELLED) {
					newstate = STATE_NO_OPEN_POS;
				}
				else if (event==EVENT_BROKER_RESP_LONG_ORDER_EXECUTED) {
					newstate = STATE_LONG_EXECUTED;
				}
				else if (event==EVENT_ADOPT_EXECPRICE_SL_TP_ORDER_OPER) {
					doMachineAction(brkConnect, ACTION_ASK_FOR_LONG_ADOPT_EXEC_TP_SL_ORDER);
					newstate = STATE_LONG_ASKED_TO_ADOPT_EXEC_SL_TP_ON_INSTALLED_STATE;
				}
			}

			// executed orders
			else if (brkConnect.getSession().getState()==STATE_SHORT_EXECUTED) {
				if (event == EVENT_TICK_CHECK_PRICE_LESS_KIJUN) {
					// no loss level
					doMachineAction(brkConnect, ACTION_ASK_FOR_SHORT_ORDER_MODIFY_SETNOLOSS);
					newstate = STATE_SHORT_ASKED_MODIFY;
				}
				// if order automatically closed
				else if (event == EVENT_BROKER_RESP_SHORT_ORDER_CLOSED) {
					newstate = STATE_NO_OPEN_POS;
				}
				else if (event==EVENT_ADOPT_EXECPRICE_SL_TP_ORDER_OPER) {
					doMachineAction(brkConnect, ACTION_ASK_FOR_SHORT_ADOPT_TP_SL_ORDER);
					newstate = STATE_SHORT_ASKED_TO_ADOPT_SL_TP_ON_EXECUTED_STATE;
				}
			}
			else if (brkConnect.getSession().getState()==STATE_LONG_EXECUTED) {
				if (event == EVENT_TICK_CHECK_PRICE_MORE_KIJUN) {
					// no loss level
					doMachineAction(brkConnect, ACTION_ASK_FOR_LONG_ORDER_MODIFY_SETNOLOSS);
					newstate = STATE_LONG_ASKED_MODIFY;
				}
				// if order automatically closed
				else if (event == EVENT_BROKER_RESP_LONG_ORDER_CLOSED) {
					newstate = STATE_NO_OPEN_POS;
				}
				else if (event==EVENT_ADOPT_EXECPRICE_SL_TP_ORDER_OPER) {
					doMachineAction(brkConnect, ACTION_ASK_FOR_LONG_ADOPT_TP_SL_ORDER);
					newstate = STATE_LONG_ASKED_TO_ADOPT_SL_TP_ON_EXECUTED_STATE;
				}
			}

			// no loss passed
			else if (brkConnect.getSession().getState()==STATE_SHORT_EXECUTED_NOLOSS) {
			
				// if order automatically closed
				if (event == EVENT_BROKER_RESP_SHORT_ORDER_CLOSED) {
					newstate = STATE_NO_OPEN_POS;
				}
				else if (event==EVENT_ADOPT_EXECPRICE_SL_TP_ORDER_OPER) {
					doMachineAction(brkConnect, ACTION_ASK_FOR_SHORT_ADOPT_TP_ORDER);
					newstate = STATE_SHORT_ASKED_TO_ADOPT_TP_ON_EXECUTED_NOLOSS_STATE;
				}
			}
			else if (brkConnect.getSession().getState()==STATE_LONG_EXECUTED_NOLOSS) {
				// if order automatically closed
				if (event == EVENT_BROKER_RESP_LONG_ORDER_CLOSED) {
					newstate = STATE_NO_OPEN_POS;
				}
				else if (event==EVENT_ADOPT_EXECPRICE_SL_TP_ORDER_OPER) {
					doMachineAction(brkConnect, ACTION_ASK_FOR_LONG_ADOPT_TP_ORDER);
					newstate = STATE_LONG_ASKED_TO_ADOPT_TP_ON_EXECUTED_NOLOSS_STATE;
				}
			}

			// we have non-confirmed order transaction
			else if (brkConnect.getSession().getState()==STATE_SHORT_ASKED_MODIFY) {
				if (event==EVENT_BROKER_RESP_SHORT_ORDER_MODIFIED) {
					newstate = STATE_SHORT_EXECUTED_NOLOSS;
				}
				else if (event==EVENT_BROKER_REJECTED_ORDER_OPER) {
					newstate=STATE_SHORT_EXECUTED;
				}
			}
			else if (brkConnect.getSession().getState()==STATE_LONG_ASKED_MODIFY) {
				if (event==EVENT_BROKER_RESP_LONG_ORDER_MODIFIED) {
					newstate = STATE_LONG_EXECUTED_NOLOSS;
				}
				else if (event==EVENT_BROKER_REJECTED_ORDER_OPER) {
					newstate=STATE_LONG_EXECUTED;
				}
			}
			//
			else if (brkConnect.getSession().getState()==STATE_SHORT_ASKED_CLOSE) {
				if (event==EVENT_BROKER_RESP_SHORT_ORDER_CLOSED) {
					newstate = STATE_NO_OPEN_POS;
				}
			}
			else if (brkConnect.getSession().getState()==STATE_LONG_ASKED_CLOSE) {
				if (event==EVENT_BROKER_RESP_LONG_ORDER_CLOSED) {
					newstate = STATE_NO_OPEN_POS;
				}
			}
			//
			else if (brkConnect.getSession().getState()==STATE_SHORT_ASKED_CANCEL) {
				if (event==EVENT_BROKER_RESP_SHORT_ORDER_CANCELLED) {
					newstate = STATE_NO_OPEN_POS;
				}
				else if (event==EVENT_BROKER_REJECTED_ORDER_OPER) {
					newstate = STATE_SHORT_INSTALLED;
				}
			}
			else if (brkConnect.getSession().getState()==STATE_LONG_ASKED_CANCEL) {
				if (event==EVENT_BROKER_RESP_LONG_ORDER_CANCELLED) {
					newstate = STATE_NO_OPEN_POS;
				}
				else if (event==EVENT_BROKER_REJECTED_ORDER_OPER) {
					newstate = STATE_LONG_INSTALLED;
				}
			}
			
			// if we asked to adopt execprice or and sl or and tp according period finish
			else if (brkConnect.getSession().getState()==STATE_SHORT_ASKED_TO_ADOPT_EXEC_SL_TP_ON_INSTALLED_STATE) {
				if (event==EVENT_BROKER_RESP_SHORT_ORDER_MODIFIED) {
					newstate = STATE_SHORT_INSTALLED;
				}
				else if (event==EVENT_BROKER_REJECTED_ORDER_OPER) {
					newstate = STATE_SHORT_INSTALLED;
				}
				
				
			}
			else if (brkConnect.getSession().getState()==STATE_LONG_ASKED_TO_ADOPT_EXEC_SL_TP_ON_INSTALLED_STATE) {
				if (event==EVENT_BROKER_RESP_LONG_ORDER_MODIFIED) {
					newstate = STATE_LONG_INSTALLED;
				}
				else if (event==EVENT_BROKER_REJECTED_ORDER_OPER) {
					newstate = STATE_LONG_INSTALLED;
				}
				
			}

			//
			else if (brkConnect.getSession().getState()==STATE_SHORT_ASKED_TO_ADOPT_SL_TP_ON_EXECUTED_STATE) {
				if (event==EVENT_BROKER_RESP_SHORT_ORDER_MODIFIED) {
					newstate = STATE_SHORT_EXECUTED;
				}
				else if (event==EVENT_BROKER_REJECTED_ORDER_OPER) {
					newstate = STATE_SHORT_EXECUTED;
				}
				

			}
			else if (brkConnect.getSession().getState()==STATE_LONG_ASKED_TO_ADOPT_SL_TP_ON_EXECUTED_STATE) {
				if (event==EVENT_BROKER_RESP_LONG_ORDER_MODIFIED) {
					newstate = STATE_LONG_EXECUTED;
				}
				else if (event==EVENT_BROKER_REJECTED_ORDER_OPER) {
					newstate = STATE_LONG_EXECUTED;
				}
				

			}
			//
			else if (brkConnect.getSession().getState()==STATE_SHORT_ASKED_TO_ADOPT_TP_ON_EXECUTED_NOLOSS_STATE) {
				if (event==EVENT_BROKER_RESP_SHORT_ORDER_MODIFIED) {
					newstate = STATE_SHORT_EXECUTED_NOLOSS;
				}
				else if (event==EVENT_BROKER_REJECTED_ORDER_OPER) {
					newstate = STATE_SHORT_EXECUTED_NOLOSS;
				}
				
			}
			else if (brkConnect.getSession().getState()==STATE_LONG_ASKED_TO_ADOPT_TP_ON_EXECUTED_NOLOSS_STATE) {
				if (event==EVENT_BROKER_RESP_LONG_ORDER_MODIFIED) {
					newstate = STATE_LONG_EXECUTED_NOLOSS;
				}
				else if (event==EVENT_BROKER_REJECTED_ORDER_OPER) {
					newstate = STATE_LONG_EXECUTED_NOLOSS;
				}
			}

			// actions when we arrived at new state
			if (newstate != brkConnect.getSession().getState()) {

				if (newstate==STATE_UNDEFINED) {
				}
				else if (newstate==STATE_NO_OPEN_POS) {
				}
				else if (newstate==STATE_SHORT_ASKED_INSTALL) {
				}
				else if (newstate==STATE_LONG_ASKED_INSTALL) {
				}
				else if (newstate==STATE_SHORT_INSTALLED) {
				}
				else if (newstate==STATE_LONG_INSTALLED) {
				}
				else if (newstate==STATE_SHORT_EXECUTED) {
				}
				else if (newstate==STATE_LONG_EXECUTED) {
				}
				else if (newstate==STATE_SHORT_ASKED_MODIFY) {
				}
				else if (newstate==STATE_LONG_ASKED_MODIFY) {
				}
				else if (newstate==STATE_SHORT_ASKED_CLOSE) {
				}
				else if (newstate==STATE_LONG_ASKED_CLOSE) {
				}
				else if (newstate==STATE_SHORT_ASKED_CANCEL) {
				}
				else if (newstate==STATE_LONG_ASKED_CANCEL) {
				}

				// set new state
				brkConnect.getSession().setState(newstate);
			};

			// logging
			LOGEVENTA(HlpStruct::CommonLog::LL_DEBUGDETAIL,  ALGSILVER2, "State machine new state" , newstate);

			


	}; // end of function
	
	void SilverImpl::doMachineAction(BrkLib::BrokerConnect& brkConnect, int actionId)
	{
		BrkLib::BrokerOperation oper(brkConnect);
		if (actionId==ACTION_ASK_FOR_SHORT_LIMIT_ORDER_INSTALL) {
				BrkLib::Order order;

				double tprofit = brkConnect.getSession().getSessionStorage().getGlobal("short_tprofit_period").getAsDouble();
				double sloss = brkConnect.getSession().getSessionStorage().getGlobal("short_sloss_period").getAsDouble();
				double execprice = brkConnect.getSession().getSessionStorage().getGlobal("short_execprice_period").getAsDouble();

				order.comment_m = "ACTION_ASK_FOR_SHORT_LIMIT_ORDER_INSTALL";
				order.orderType_m = (BrkLib::OP_SELL_LIMIT |	BrkLib::OP_STOPLOSS_SET | BrkLib::OP_TAKEPROFIT_SET);

				order.provider_m = tradeProvider_m;
				order.symbolNum_m = tradeSymbolNum_m;
				order.symbolDenom_m = tradeSymbolDenom_m;
				order.orVolume_m = posVolume_m;
				order.orPrice_m = execprice; 
				order.orSLprice_m = sloss;
				order.orTPprice_m = tprofit;

				oper.unblockingIssueOrder(order);
		}
		else if (actionId==ACTION_ASK_FOR_LONG_LIMIT_ORDER_INSTALL) {

			BrkLib::Order order;

			double tprofit = brkConnect.getSession().getSessionStorage().getGlobal("long_tprofit_period").getAsDouble();
			double sloss = brkConnect.getSession().getSessionStorage().getGlobal("long_sloss_period").getAsDouble();
			double execprice = brkConnect.getSession().getSessionStorage().getGlobal("long_execprice_period").getAsDouble();

			order.comment_m = "ACTION_ASK_FOR_LONG_LIMIT_ORDER_INSTALL";
			order.orderType_m = (BrkLib::OP_BUY_LIMIT | BrkLib::OP_STOPLOSS_SET | BrkLib::OP_TAKEPROFIT_SET);
			order.provider_m = tradeProvider_m;
			order.symbolNum_m = tradeSymbolNum_m;
			order.symbolDenom_m = tradeSymbolDenom_m;
			order.orVolume_m = posVolume_m;
			order.orPrice_m = execprice; 
			order.orSLprice_m = sloss;
			order.orTPprice_m = tprofit;

			oper.unblockingIssueOrder(order);

		}

		else if (actionId==ACTION_ASK_FOR_SHORT_ORDER_MODIFY_SETNOLOSS) {
			CppUtils::String const& orderId = brkConnect.getSession().getSessionStorage().getGlobal("valid_order_id").getAsString();
			double const& executeprice = brkConnect.getSession().getSessionStorage().getGlobal("valid_order_executeprice").getAsDouble();

			BrkLib::Order order;
			order.comment_m = "ACTION_ASK_FOR_SHORT_ORDER_MODIFY_SETNOLOSS";
			order.orderType_m = BrkLib::OP_STOPLOSS_SET;
			order.brokerPositionID_m = orderId;
			order.provider_m = tradeProvider_m;
			order.symbolNum_m = tradeSymbolNum_m;
			order.symbolDenom_m = tradeSymbolDenom_m;
			order.orVolume_m = posVolume_m;
			order.orSLprice_m = executeprice;

			oper.unblockingIssueOrder(order);
		}
		else if (actionId==ACTION_ASK_FOR_LONG_ORDER_MODIFY_SETNOLOSS) {

			CppUtils::String const& orderId = brkConnect.getSession().getSessionStorage().getGlobal("valid_order_id").getAsString();
			double const& executeprice = brkConnect.getSession().getSessionStorage().getGlobal("valid_order_executeprice").getAsDouble();

			BrkLib::Order order;
			order.comment_m = "ACTION_ASK_FOR_LONG_ORDER_MODIFY_SETNOLOSS";
			order.orderType_m = BrkLib::OP_STOPLOSS_SET;
			order.brokerPositionID_m = orderId;
			order.provider_m = tradeProvider_m;
			order.symbolNum_m = tradeSymbolNum_m;
			order.symbolDenom_m = tradeSymbolDenom_m;
			order.orVolume_m = posVolume_m;
			order.orSLprice_m = executeprice + spread_m;

			oper.unblockingIssueOrder(order);

		}

		else if (actionId==ACTION_ASK_FOR_SHORT_ORDER_CLOSE) {
			CppUtils::String const& orderId = brkConnect.getSession().getSessionStorage().getGlobal("valid_order_id").getAsString();

			BrkLib::Order order;
			order.comment_m = "ACTION_ASK_FOR_SHORT_ORDER_CLOSE";
			order.orderType_m = BrkLib::OP_CLOSE;
			order.brokerPositionID_m = orderId;
			order.provider_m = tradeProvider_m;
			order.symbolNum_m = tradeSymbolNum_m;
			order.symbolDenom_m = tradeSymbolDenom_m;
			order.orVolume_m = posVolume_m;

			oper.unblockingIssueOrder(order);

		}
		else if (actionId==ACTION_ASK_FOR_LONG_ORDER_CLOSE) {
			CppUtils::String const& orderId = brkConnect.getSession().getSessionStorage().getGlobal("valid_order_id").getAsString();

			BrkLib::Order order;
			order.comment_m = "ACTION_ASK_FOR_LONG_ORDER_CLOSE";
			order.orderType_m = BrkLib::OP_CLOSE;
			order.brokerPositionID_m = orderId;
			order.provider_m = tradeProvider_m;
			order.symbolNum_m = tradeSymbolNum_m;
			order.symbolDenom_m = tradeSymbolDenom_m;
			order.orVolume_m = posVolume_m;

			oper.unblockingIssueOrder(order);
		}

		else if (actionId==ACTION_ASK_FOR_SHORT_LIMIT_ORDER_CANCEL) {
			CppUtils::String const& orderId = brkConnect.getSession().getSessionStorage().getGlobal("valid_order_id").getAsString();

			BrkLib::Order order;
			order.comment_m = "ACTION_ASK_FOR_SHORT_LIMIT_ORDER_CANCEL";
			order.orderType_m = BrkLib::OP_CANCEL;
			order.brokerPositionID_m = orderId;
			order.provider_m = tradeProvider_m;
			order.symbolNum_m = tradeSymbolNum_m;
			order.symbolDenom_m = tradeSymbolDenom_m;
			order.orVolume_m = posVolume_m;

			oper.unblockingIssueOrder(order);
		}
		else if (actionId==ACTION_ASK_FOR_LONG_LIMIT_ORDER_CANCEL) {
			CppUtils::String const& orderId = brkConnect.getSession().getSessionStorage().getGlobal("valid_order_id").getAsString();
			
			BrkLib::Order order;
			order.comment_m = "ACTION_ASK_FOR_LONG_LIMIT_ORDER_CANCEL";
			order.orderType_m = BrkLib::OP_CANCEL;
			order.brokerPositionID_m = orderId;
			order.provider_m = tradeProvider_m;
			order.symbolNum_m = tradeSymbolNum_m;
			order.symbolDenom_m = tradeSymbolDenom_m;
			order.orVolume_m = posVolume_m;

			oper.unblockingIssueOrder(order);

		}
		else if (actionId==ACTION_ASK_FOR_SHORT_ADOPT_EXEC_TP_SL_ORDER) {
			CppUtils::String const& orderId = brkConnect.getSession().getSessionStorage().getGlobal("valid_order_id").getAsString();

			double execprice = brkConnect.getSession().getSessionStorage().getGlobal("short_execprice_period").getAsDouble(true);
			double stoploss = brkConnect.getSession().getSessionStorage().getGlobal("short_sloss_period").getAsDouble(true);
			double takeprofit = brkConnect.getSession().getSessionStorage().getGlobal("short_tprofit_period").getAsDouble(true);

			BrkLib::Order order;

			order.comment_m = "ACTION_ASK_FOR_SHORT_ADOPT_EXEC_TP_SL_ORDER";
			order.orderType_m = (BrkLib::OP_EXEC_PRICE_SET | BrkLib::OP_TAKEPROFIT_SET | BrkLib::OP_STOPLOSS_SET);
			order.brokerPositionID_m = orderId;
			order.provider_m = tradeProvider_m;
			order.symbolNum_m = tradeSymbolNum_m;
			order.symbolDenom_m = tradeSymbolDenom_m;
			order.orVolume_m = posVolume_m;
			order.orTPprice_m = takeprofit;
			order.orSLprice_m = stoploss;
			order.orPrice_m = execprice;

			oper.unblockingIssueOrder(order);

		}
		else if (actionId==ACTION_ASK_FOR_SHORT_ADOPT_TP_SL_ORDER) {
			CppUtils::String const& orderId = brkConnect.getSession().getSessionStorage().getGlobal("valid_order_id").getAsString();

			double stoploss = brkConnect.getSession().getSessionStorage().getGlobal("short_sloss_period").getAsDouble(true);
			double takeprofit = brkConnect.getSession().getSessionStorage().getGlobal("short_tprofit_period").getAsDouble(true);

			BrkLib::Order order;

			order.comment_m = "ACTION_ASK_FOR_SHORT_ADOPT_TP_SL_ORDER";
			order.orderType_m = (BrkLib::OP_TAKEPROFIT_SET | BrkLib::OP_STOPLOSS_SET);
			order.brokerPositionID_m = orderId;
			order.provider_m = tradeProvider_m;
			order.symbolNum_m = tradeSymbolNum_m;
			order.symbolDenom_m = tradeSymbolDenom_m;
			order.orVolume_m = posVolume_m;
			order.orTPprice_m = takeprofit;
			order.orSLprice_m = stoploss;

			oper.unblockingIssueOrder(order);
		}
		else if (actionId==ACTION_ASK_FOR_SHORT_ADOPT_TP_ORDER) {
			CppUtils::String const& orderId = brkConnect.getSession().getSessionStorage().getGlobal("valid_order_id").getAsString();

			double takeprofit = brkConnect.getSession().getSessionStorage().getGlobal("short_tprofit_period").getAsDouble(true);

			BrkLib::Order order;

			order.comment_m = "ACTION_ASK_FOR_SHORT_ADOPT_TP_ORDER";
			order.orderType_m = BrkLib::OP_TAKEPROFIT_SET;
			order.brokerPositionID_m = orderId;
			order.provider_m = tradeProvider_m;
			order.symbolNum_m = tradeSymbolNum_m;
			order.symbolDenom_m = tradeSymbolDenom_m;
			order.orVolume_m = posVolume_m;
			order.orTPprice_m = takeprofit;

			oper.unblockingIssueOrder(order);
		}

		//
		else if (actionId==ACTION_ASK_FOR_LONG_ADOPT_EXEC_TP_SL_ORDER) {
			CppUtils::String const& orderId = brkConnect.getSession().getSessionStorage().getGlobal("valid_order_id").getAsString();

			double execprice = brkConnect.getSession().getSessionStorage().getGlobal("long_execprice_period").getAsDouble(true);
			double stoploss = brkConnect.getSession().getSessionStorage().getGlobal("long_sloss_period").getAsDouble(true);
			double takeprofit = brkConnect.getSession().getSessionStorage().getGlobal("long_tprofit_period").getAsDouble(true);

			BrkLib::Order order;
			order.comment_m = "ACTION_ASK_FOR_LONG_ADOPT_EXEC_TP_SL_ORDER";

			order.orderType_m = (BrkLib::OP_EXEC_PRICE_SET | BrkLib::OP_TAKEPROFIT_SET | BrkLib::OP_STOPLOSS_SET);
			order.brokerPositionID_m = orderId;
			order.provider_m = tradeProvider_m;
			order.symbolNum_m = tradeSymbolNum_m;
			order.symbolDenom_m = tradeSymbolDenom_m;
			order.orVolume_m = posVolume_m;
			order.orTPprice_m = takeprofit;
			order.orSLprice_m = stoploss;
			order.orPrice_m = execprice;

			oper.unblockingIssueOrder(order);

		}
		else if (actionId==ACTION_ASK_FOR_LONG_ADOPT_TP_SL_ORDER) {
			CppUtils::String const& orderId = brkConnect.getSession().getSessionStorage().getGlobal("valid_order_id").getAsString();

			double stoploss = brkConnect.getSession().getSessionStorage().getGlobal("long_sloss_period").getAsDouble(true);
			double takeprofit = brkConnect.getSession().getSessionStorage().getGlobal("long_tprofit_period").getAsDouble(true);

			BrkLib::Order order;

			order.comment_m = "ACTION_ASK_FOR_LONG_ADOPT_TP_SL_ORDER";
			order.orderType_m = (BrkLib::OP_TAKEPROFIT_SET | BrkLib::OP_STOPLOSS_SET);
			order.brokerPositionID_m = orderId;
			order.provider_m = tradeProvider_m;
			order.symbolNum_m = tradeSymbolNum_m;
			order.symbolDenom_m = tradeSymbolDenom_m;
			order.orVolume_m = posVolume_m;
			order.orTPprice_m = takeprofit;
			order.orSLprice_m = stoploss;

			oper.unblockingIssueOrder(order);
		}
		else if (actionId==ACTION_ASK_FOR_LONG_ADOPT_TP_ORDER) {
			CppUtils::String const& orderId = brkConnect.getSession().getSessionStorage().getGlobal("valid_order_id").getAsString();
			
			double takeprofit = brkConnect.getSession().getSessionStorage().getGlobal("long_tprofit_period").getAsDouble(true);

			BrkLib::Order order;

			order.comment_m = "ACTION_ASK_FOR_LONG_ADOPT_TP_ORDER";
			order.orderType_m = BrkLib::OP_TAKEPROFIT_SET;
			order.brokerPositionID_m = orderId;
			order.provider_m = tradeProvider_m;
			order.symbolNum_m = tradeSymbolNum_m;
			order.symbolDenom_m = tradeSymbolDenom_m;
			order.orVolume_m = posVolume_m;
			order.orTPprice_m = takeprofit;

			oper.unblockingIssueOrder(order);
		}

		
	

		// logging
		LOGEVENTA(HlpStruct::CommonLog::LL_DEBUGDETAIL,  ALGSILVER2, "State machine action done" , actionId );
		
	}

	/**
	Alert helpers
	*/

	void SilverImpl::doAlertOnEachTick( CppUtils::String const& runName, Inqueue::ContextAccessor& historyaccessor, HlpStruct::RtData const& rtdata)
	{
		// notify when price is close to sc_kijun
		double sc_kijun = historyaccessor.getThreadGlobal().getGlobal("sc_kijun").getAsDouble(true);
		double bid_price = rtdata.getRtData().bid_m;

		double distance = abs(bid_price - sc_kijun)/sc_kijun;
		


		if (priceCloseLevel_m >= distance) {

			double sc_sencouA = historyaccessor.getThreadGlobal().getGlobal("sc_sencouA").getAsDouble();
			double sc_sencouB = historyaccessor.getThreadGlobal().getGlobal("sc_sencouB").getAsDouble();
			
			double sc_sky_low = historyaccessor.getThreadGlobal().getGlobal("sc_sky_low").getAsDouble();
			double sc_sky_high = historyaccessor.getThreadGlobal().getGlobal("sc_sky_high").getAsDouble();

			double sc_silver_low = historyaccessor.getThreadGlobal().getGlobal("sc_silver_low").getAsDouble();
			double sc_silver_high = historyaccessor.getThreadGlobal().getGlobal("sc_silver_high").getAsDouble();

			double sc_future_low = historyaccessor.getThreadGlobal().getGlobal("sc_future_low").getAsDouble();
			double sc_future_high = historyaccessor.getThreadGlobal().getGlobal("sc_future_high").getAsDouble();

			double sc_zen_low = historyaccessor.getThreadGlobal().getGlobal("sc_zen_low").getAsDouble();
			double sc_zen_high = historyaccessor.getThreadGlobal().getGlobal("sc_zen_high").getAsDouble();

			double sc_kijun = historyaccessor.getThreadGlobal().getGlobal("sc_kijun").getAsDouble(true);
			double sc_tenkan = historyaccessor.getThreadGlobal().getGlobal("sc_tenkan").getAsDouble(true);

			double open_price_period = historyaccessor.getThreadGlobal().getGlobal("open_price_period").getAsDouble();
			double close_price_period = historyaccessor.getThreadGlobal().getGlobal("close_price_period").getAsDouble();
			double high_price_period = historyaccessor.getThreadGlobal().getGlobal("high_price_period").getAsDouble();
			double low_price_period = historyaccessor.getThreadGlobal().getGlobal("low_price_period").getAsDouble();
			double close_price_period_prev = historyaccessor.getThreadGlobal().getGlobal("close_price_period_prev").getAsDouble();


			double time_price_period = historyaccessor.getThreadGlobal().getGlobal("time_price_period").getAsDouble();


			// make alert
			ALERTEVENTNC(runName, BrkLib::AP_MEDIUM, "Bid price is close to sc_kijun: " << ", " << 
				"Gmt Arrived ticker time:" + CppUtils::getGmtTime2(rtdata.getTime()) << ", " << 
				"Current bid price:" + CppUtils::float2String(bid_price,-1,4) << ", " <<  
				"Kijun sen:" + CppUtils::float2String(sc_kijun,-1,4) << ", " << 
				"Proximity:" + CppUtils::float2String(priceCloseLevel_m,-1,4) << ", " << 
				
				// other parameters
				"Gmt just finished time:" + CppUtils::getGmtTime2(time_price_period) << ", " << 
				"Just finished open:" + CppUtils::float2String(open_price_period, -1, 4) << ", " << 
				"Just finished high:" + CppUtils::float2String(high_price_period, -1, 4) << ", " <<  
				"Just finished low:" + CppUtils::float2String(low_price_period, -1, 4) << ", " << 
				"Just finished close:" + CppUtils::float2String(close_price_period, -1, 4) << ", " << 

				"Tenkan sen:" + CppUtils::float2String(sc_tenkan,-1,4) << ", " << 
				"Kijun sen:" + CppUtils::float2String(sc_kijun,-1,4) << ", " <<  
				"Senkou Span A:" + CppUtils::float2String(sc_sencouA,-1,4) << ", " <<  
				"Senkou Span B:" + CppUtils::float2String(sc_sencouB, -1, 4) << ", " << 

				"Silver-channel High:"+ CppUtils::float2String(sc_silver_high, -1, 4) << ", " << 
				"Silver-channel Low:"+ CppUtils::float2String(sc_silver_low, -1, 4) << ", " << 
				"Sky-channel High:"+ CppUtils::float2String(sc_sky_high, -1, 4) << ", " << 
				"Sky-channel Low:"+ CppUtils::float2String(sc_sky_low, -1, 4) << ", " << 
				"Zen-channel High:"+ CppUtils::float2String(sc_zen_high, -1, 4) << ", " << 
				"Zen-channel Low:"+ CppUtils::float2String(sc_zen_low, -1, 4) << ", " << 
				"Future-channel Low:"+ CppUtils::float2String(sc_future_low, -1, 4) << ", " << 
				"Future-channel High:"+ CppUtils::float2String(sc_future_high, -1, 4) << ", " <<
				"If POS OPENED breakthrough KIJUN means no-loss level" << ", " <<
				"If POS PENDING breakthrough KIJUN means cancellation"
				);
			
		}

	}
	
	void SilverImpl::doAlertEachPeriod( CppUtils::String const& runName, Inqueue::ContextAccessor& historyaccessor, HlpStruct::RtData const& rtdata )
	{
		// this is expected to be called on each period finish
		// alert

		double sc_sencouA = historyaccessor.getThreadGlobal().getGlobal("sc_sencouA").getAsDouble();
		double sc_sencouB = historyaccessor.getThreadGlobal().getGlobal("sc_sencouB").getAsDouble();
		
		double sc_sky_low = historyaccessor.getThreadGlobal().getGlobal("sc_sky_low").getAsDouble();
		double sc_sky_high = historyaccessor.getThreadGlobal().getGlobal("sc_sky_high").getAsDouble();

		double sc_silver_low = historyaccessor.getThreadGlobal().getGlobal("sc_silver_low").getAsDouble();
		double sc_silver_high = historyaccessor.getThreadGlobal().getGlobal("sc_silver_high").getAsDouble();

		double sc_future_low = historyaccessor.getThreadGlobal().getGlobal("sc_future_low").getAsDouble();
		double sc_future_high = historyaccessor.getThreadGlobal().getGlobal("sc_future_high").getAsDouble();

		double sc_zen_low = historyaccessor.getThreadGlobal().getGlobal("sc_zen_low").getAsDouble();
		double sc_zen_high = historyaccessor.getThreadGlobal().getGlobal("sc_zen_high").getAsDouble();

		double sc_kijun = historyaccessor.getThreadGlobal().getGlobal("sc_kijun").getAsDouble(true);
		double sc_tenkan = historyaccessor.getThreadGlobal().getGlobal("sc_tenkan").getAsDouble(true);

		double open_price_period = historyaccessor.getThreadGlobal().getGlobal("open_price_period").getAsDouble();
		double close_price_period = historyaccessor.getThreadGlobal().getGlobal("close_price_period").getAsDouble();
		double high_price_period = historyaccessor.getThreadGlobal().getGlobal("high_price_period").getAsDouble();
		double low_price_period = historyaccessor.getThreadGlobal().getGlobal("low_price_period").getAsDouble();
		double close_price_period_prev = historyaccessor.getThreadGlobal().getGlobal("close_price_period_prev").getAsDouble();


		double time_price_period = historyaccessor.getThreadGlobal().getGlobal("time_price_period").getAsDouble();


		// claculate potentila profit /loss ratio 
		double tpsl1 = abs(sc_sky_low-sc_sky_high)/abs(sc_zen_low-sc_sky_low);
		double tpsl2 = abs(sc_sky_low-sc_sky_high)/abs(sc_zen_high-sc_sky_high);

		ALERTEVENTNC(runName, BrkLib::AP_MEDIUM, "Silver period finished: " << ", " << 
			"Gmt Arrived ticker time:" + CppUtils::getGmtTime2(rtdata.getTime()) << ", " << 
			"Gmt just finished time:" + CppUtils::getGmtTime2(time_price_period) << ", " << 
			"Just finished open:" + CppUtils::float2String(open_price_period, -1, 4) << ", " << 
			"Just finished high:" + CppUtils::float2String(high_price_period, -1, 4) << ", " << 
			"Just finished low:" + CppUtils::float2String(low_price_period, -1, 4) << ", " << 
			"Just finished close:" + CppUtils::float2String(close_price_period, -1, 4) << ", " << 
			"Pre finished close:" + CppUtils::float2String(close_price_period_prev, -1, 4) << ", " << 

			"Tenkan sen:" + CppUtils::float2String(sc_tenkan,-1,4) << ", " << 
			"Kijun sen:" + CppUtils::float2String(sc_kijun,-1,4) << ", " << 
			"Senkou Span A:" + CppUtils::float2String(sc_sencouA,-1,4) << ", " << 
			"Senkou Span B:" + CppUtils::float2String(sc_sencouB, -1, 4) << ", " << 

			"Silver-channel High:"+ CppUtils::float2String(sc_silver_high, -1, 4) << ", " << 
			"Silver-channel Low:"+ CppUtils::float2String(sc_silver_low, -1, 4) << ", " << 
			"Sky-channel High:"+ CppUtils::float2String(sc_sky_high, -1, 4) << ", " << 
			"Sky-channel Low:"+ CppUtils::float2String(sc_sky_low, -1, 4) << ", " << 
			"Zen-channel High:"+ CppUtils::float2String(sc_zen_high, -1, 4) << ", " << 
			"Zen-channel Low:"+ CppUtils::float2String(sc_zen_low, -1, 4) << ", " << 
			"Future-channel Low:"+ CppUtils::float2String(sc_future_low, -1, 4) << ", " << 
			"Future-channel High:"+ CppUtils::float2String(sc_future_high, -1, 4) << ", "
			"Expect SLOSS=zen, TPROFIT=sky breakthrough, EXECPRICE=sky opposite" << ", "
			"Potential TP/SL ratio: " + CppUtils::float2String(tpsl1, -1, 4) + " or "+ CppUtils::float2String(tpsl2, -1, 4)
		);


	}
	

} // end of namespace