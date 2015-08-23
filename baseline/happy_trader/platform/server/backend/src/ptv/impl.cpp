#include "impl.hpp"

#include "../inqueue/commontypes.hpp"
#include "../math/math.hpp"

#define PROVIDER "MT Provider"
#define ALGORITHM_PTV "ALGORITHM_PTV"


#define SYMBOL_ID						1
#define SYMBOL_NAME					2



// breakthrough position state
#define POS_BREAKTHROUGH_ESTABLISHED 1
#define POS_BREAKTHROUGH_EXECUTED_1_DAY 2
#define POS_BREAKTHROUGH_EXECUTED_MORE_1_DAY 3

// reflection order states
#define POS_REFLECT_ESTABLISHED 4
#define POS_REFLECT_EXECUTED		5

// the tag of breakthrough order 
#define RESPONSE_TAG_BREAKTHROUGH 10
#define RESPONSE_TAG_REFLECTION 20

// ----------------------------------



namespace AlgLib {

AlgorithmBase *creator (Inqueue::AlgorithmHolder* algholder)
{
		return new AlgorithmPtv(*algholder);
}

void terminator (AlgorithmBase *pthis)
{
		assert(pthis);
		delete pthis;
}

AlgorithmBase::CreatorAlgorithmFun createAlgorithm = creator;

AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm = terminator;

// -----------------------------------------

		
AlgorithmPtv::AlgorithmPtv( Inqueue::AlgorithmHolder& algHolder):
	AlgorithmBase(algHolder)
{
}

AlgorithmPtv::~AlgorithmPtv()
{  
}
		
		// intreface to be implemented
void AlgorithmPtv::onLibraryInitialized(
		Inqueue::AlgBrokerDescriptor const& descriptor, 
		BrkLib::BrokerBase* brokerlib, 
		CppUtils::String const& runName,
		CppUtils::String const& comment
	)
{

		// Read trading parameters
		// ------------------
	
		CppUtils::String const& symbol = descriptor.initialAlgBrkParams_m.algParams_m.getParameter("trade_symbol").getAsString();

		tradeParams_m[symbol].point_value_m = -1; 
		tradeParams_m[symbol].atr_period_m = atoi(descriptor.initialAlgBrkParams_m.algParams_m.getParameter("atr_period").getAsString().c_str());
		
		tradeParams_m[symbol].bars_no_trend_m = atoi(descriptor.initialAlgBrkParams_m.algParams_m.getParameter("bars_no_trend").getAsString().c_str());
		tradeParams_m[symbol].trend_channel_m= atoi(descriptor.initialAlgBrkParams_m.algParams_m.getParameter("trend_channel").getAsString().c_str());

		tradeParams_m[symbol].hour_signal_begin_m = atoi(descriptor.initialAlgBrkParams_m.algParams_m.getParameter("hour_signal_begin").getAsString().c_str());
		tradeParams_m[symbol].hour_signal_end_m= atoi(descriptor.initialAlgBrkParams_m.algParams_m.getParameter("hour_signal_end").getAsString().c_str());

		tradeParams_m[symbol].days_remove_orders_m=atoi(descriptor.initialAlgBrkParams_m.algParams_m.getParameter("days_remove_orders").getAsString().c_str());

		LOGEVENT(HlpStruct::CommonLog::LL_INFO,  ALGORITHM_PTV, "For symbol: " << symbol << " - ATR period is: " << tradeParams_m[symbol].atr_period_m );

		// ------------------
		CppUtils::String comments = comment + " - " + "USDCHF - breakthrough, EURUSD- reflection visa versa, alert version";
		//CppUtils::String comments = comment + " - " + "simple run";
		BrkLib::BrokerConnect brkConnect(brokerlib, "dummy", false, false, runName.c_str(), comments.c_str());
		brkConnect.connect();
		
		
		if (!brkConnect.getSession().isConnected()) {
			LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  ALGORITHM_PTV, "On initialization session cannot be established: " << brkConnect.getSession().getConnection());
			return;
		}

		// init global
		brokerConnect_m = brkConnect;
		
		// no trend
		
		brkConnect.getSession().getSessionStorage().getGlobal(SYMBOL_NAME).getAsString(true) = symbol;
		

		// this session symbol ID
		brkConnect.getSession().getSessionStorage().getGlobal(SYMBOL_ID).getAsInt(true) = -1;

	
			
		// set initial state 
		brkConnect.getSession().setState(0);
	
	
		// handshake 
		sendHandshakeOrder(brkConnect);		


		for(map<CppUtils::String, TradeParams >::const_iterator it = tradeParams_m.begin();
			it != tradeParams_m.end(); it++) 
		{ 
			CppUtils::String const& fullSymbol = it->first;

			// metainfo order for all of our symbols
			sendMetainfoOrder(brkConnect, fullSymbol );
		}


		

	LOGEVENT(HlpStruct::CommonLog::LL_INFO,  ALGORITHM_PTV, "Initialized");
}

void AlgorithmPtv::onLibraryFreed()
{
	LOGEVENT(HlpStruct::CommonLog::LL_INFO,  ALGORITHM_PTV, "Freed");
}

bool AlgorithmPtv::onThreadStarted(Inqueue::ContextAccessor& contextaccessor) {
		return false;
};

void AlgorithmPtv::onDataArrived(
			HlpStruct::RtData const& rtdata, 
			Inqueue::ContextAccessor& historyaccessor, 
			Inqueue::TradingParameters const& descriptor,
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		)
{
	
	// new period arose
	// broker
	if (brokerlib==NULL) {
			LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  ALGORITHM_PTV, "Broker library is not loaded" );
			return;
	}

	CppUtils::String const& symbol = brokerConnect_m.getSession().getSessionStorage().getGlobal(SYMBOL_NAME).getAsString();

	if (symbol != rtdata.getSymbol()) {
		LOGEVENT(HlpStruct::CommonLog::LL_DEBUG,  ALGORITHM_PTV, "For symbol: " << rtdata.getSymbol() << " no subscription" );
		return;
	}
		
	int symbol_id = brokerConnect_m.getSession().getSessionStorage().getGlobal(SYMBOL_ID).getAsInt();

	
	if (symbol_id < 0) {
		symbol_id = historyaccessor.cacheSymbol(rtdata.getProvider(), rtdata.getSymbol(), Inqueue::AP_Day, 1);
		LOGEVENT(HlpStruct::CommonLog::LL_INFO,  ALGORITHM_PTV, "Cached symbol " << rtdata.getSymbol() );
		
		if (symbol_id<0) {
			LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  ALGORITHM_PTV, "Cannot cache symbol" << rtdata.getSymbol() );
			return;
		}

		brokerConnect_m.getSession().getSessionStorage().getGlobal(SYMBOL_ID).getAsInt(true) = symbol_id;
	}

	
	// trade parameters
	TradeParams const& tradePars = tradeParams_m[rtdata.getSymbol()];

	if (!tradePars.isValid()) {
		LOGEVENT(HlpStruct::CommonLog::LL_DEBUG,  ALGORITHM_PTV, "For symbol: " << rtdata.getSymbol() << " no metainfo data" );
		return;
	}

	

	// unclosed candle to detect new period
	HlpStruct::PriceData const& unclosed = historyaccessor.getUnclosedCandle(symbol_id);



	if (!unclosed.isNewPeriod()) {
		// not the new period
		eachTickBreakThroughOrderManage(rtdata, historyaccessor, brokerConnect_m,tradePars, symbol_id);

		// reflection order
		eachTickReflectionOrderManage(rtdata, historyaccessor, brokerConnect_m,tradePars, symbol_id);

	}
	else {
		
					
		// make end of day calculations
		recalcLastDay(historyaccessor, brokerConnect_m,tradePars, symbol_id);

		if (previousDay_m.isValid()) {

			// here we have valid data for now finished day and day ago

			// make order management
			if (strcmp(rtdata.getSymbol(),"USDCHF")==0)
				lastDayBreakThroughOrderManage(rtdata, historyaccessor, brokerConnect_m,tradePars, symbol_id);

			// reflection order management
			if (strcmp(rtdata.getSymbol(),"EURUSD")==0)
				lastDayReflectionOrderManage(rtdata, historyaccessor, brokerConnect_m,tradePars, symbol_id);
		

			// adjust upon each tick even if period is finished - because it is new tick!
			eachTickBreakThroughOrderManage(rtdata, historyaccessor, brokerConnect_m,tradePars, symbol_id);

			// reflection order
			eachTickReflectionOrderManage(rtdata, historyaccessor, brokerConnect_m,tradePars, symbol_id);
		}


	
		/*
		if (currentDay_m.trend_signal_m==1) {
					LOGEVENT(HlpStruct::CommonLog::LL_INFO, ALGORITHM_PTV, "Upper trend: " << CppUtils::getGmtTime2(rtdata.getTime())
						<< " Channel: " << CppUtils::float2String(currentDay_m.lowerChannel_m, -1, 4) << " - " << CppUtils::float2String(currentDay_m.upperChannel_m, -1, 4)
					);
		}
		else if (currentDay_m.trend_signal_m== -1) {
					LOGEVENT(HlpStruct::CommonLog::LL_INFO, ALGORITHM_PTV, "Lower trend: " << CppUtils::getGmtTime2(rtdata.getTime())
						<< " Channel: " << CppUtils::float2String(currentDay_m.lowerChannel_m, -1, 4) << " - " << CppUtils::float2String(currentDay_m.upperChannel_m, -1, 4)	
					);
		}
		else if (currentDay_m.trend_signal_m== 0) {
				LOGEVENT(HlpStruct::CommonLog::LL_INFO, ALGORITHM_PTV, "No trend: " << CppUtils::getGmtTime2(rtdata.getTime())
					<< " Channel: " << CppUtils::float2String(currentDay_m.lowerChannel_m, -1, 4) << " - " << CppUtils::float2String(currentDay_m.upperChannel_m, -1, 4)	
				);
		}
		
		
		if (firedSignals_m.lowChannelTouch_m == 1) {
			LOGEVENT(HlpStruct::CommonLog::LL_INFO, ALGORITHM_PTV, "Lower channel touch: " << CppUtils::getGmtTime2(rtdata.getTime()) );
		}

		if (firedSignals_m.upperChannelTouch_m == 1) {
			LOGEVENT(HlpStruct::CommonLog::LL_INFO, ALGORITHM_PTV, "Upper channel touch: " << CppUtils::getGmtTime2(rtdata.getTime()) );
		}

		*/
		

		// set up previus day
		previousDay_m = currentDay_m;

		// reset fired signals
		firedSignals_m.clear();


	} // end of new period

	// claculate 

}

void AlgorithmPtv::onLevel2DataArrived(
			HlpStruct::RtLevel2Data const& level2data,
			Inqueue::ContextAccessor& historyaccessor, 
			Inqueue::TradingParameters const& descriptor,
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		)
{
}

void AlgorithmPtv::onLevel1DataArrived(
			HlpStruct::RtLevel1Data const& level1data,
			Inqueue::ContextAccessor& historyaccessor, 
			Inqueue::TradingParameters const& descriptor,
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		)
{
}

void AlgorithmPtv::onBrokerResponseArrived(
			BrkLib::BrokerConnect &brkConnect,
			BrkLib::BrokerResponseBase const& bresponse
		)
{
	if (bresponse.type_m==BrkLib::RT_BrokerResponseMetaInfo) {
			BrkLib::BrokerResponseMetaInfo const& bRespMinfo = (BrkLib::BrokerResponseMetaInfo const&)bresponse;

			if ((bRespMinfo.resultCode_m == BrkLib::PT_GENERAL_INFO) && ((bRespMinfo.resultDetailCode_m & BrkLib::ODR_OK) > 0)) {
			
				// set up spread
				

				LOGEVENTA(HlpStruct::CommonLog::LL_INFO,  ALGORITHM_PTV, "Metainfo arrived" ,	
					"symbol NUM" << bRespMinfo.symbolNum_m
					<< "symbol DENOM" << bRespMinfo.symbolDenom_m 
					<< "point VALUE" << bRespMinfo.pointValue_m
				);

				// from monday to friday
				tradeParams_m[bRespMinfo.symbolNum_m + bRespMinfo.symbolDenom_m].point_value_m = bRespMinfo.pointValue_m;
				
			} 
	}
	else if (bresponse.type_m == BrkLib::RT_BrokerResponseOrder) {
				// here we have a confirmation that the order is opened
				BrkLib::BrokerResponseOrder const& bRespOrder = (BrkLib::BrokerResponseOrder const&)bresponse;

				if (bresponse.resultCode_m == BrkLib::PT_HANDSHAKEARRIVED ) 
					return;

				if ( bRespOrder.tag_m == RESPONSE_TAG_BREAKTHROUGH) {
					// breakthrough order
					if (bRespOrder.resultCode_m==BrkLib::PT_ORDER_ESTABLISHED ) {
						if (bRespOrder.resultDetailCode_m & BrkLib::ODR_ORDER_INSTALLED) {
							breakThroughPositionsState_m[ bRespOrder.brokerPositionID_m ] = POS_BREAKTHROUGH_ESTABLISHED;
						}
						else if (bRespOrder.resultDetailCode_m & BrkLib::ODR_ORDER_EXECUTED) {
							// initial execution
							breakThroughPositionsState_m[ bRespOrder.brokerPositionID_m ] = POS_BREAKTHROUGH_EXECUTED_1_DAY;
						}
					}
					else if (bRespOrder.resultCode_m == BrkLib::PT_ORDER_CLOSED ||bRespOrder.resultCode_m==BrkLib::PT_ORDER_CANCELLED ) {
						breakThroughPositionsState_m.erase( bRespOrder.brokerPositionID_m );
					}
				}
				else if ( bRespOrder.tag_m == RESPONSE_TAG_REFLECTION) {
					// reflection order

					if (bRespOrder.resultCode_m==BrkLib::PT_ORDER_ESTABLISHED ) {
						if (bRespOrder.resultDetailCode_m & BrkLib::ODR_ORDER_INSTALLED) {
							reflectionPositionsState_m[ bRespOrder.brokerPositionID_m ] = POS_REFLECT_ESTABLISHED;
						}
						else if (bRespOrder.resultDetailCode_m & BrkLib::ODR_ORDER_EXECUTED) {
							// initial execution
							reflectionPositionsState_m[ bRespOrder.brokerPositionID_m ] = POS_REFLECT_EXECUTED;
						}
					}
					else if (bRespOrder.resultCode_m == BrkLib::PT_ORDER_CLOSED ||bRespOrder.resultCode_m==BrkLib::PT_ORDER_CANCELLED ) {
						reflectionPositionsState_m.erase( bRespOrder.brokerPositionID_m );
					}

				}
				else {
					LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  ALGORITHM_PTV, "Invalid order tag: " << bRespOrder.brokerPositionID_m );
				}
					

				
				

	}
		


}

bool AlgorithmPtv::onEventArrived(HlpStruct::EventData const &eventdata, Inqueue::ContextAccessor& contextaccessor, HlpStruct::EventData& response)
{
	return true;
}
	
CppUtils::String const& AlgorithmPtv::getName() const
{
	static const CppUtils::String name("ptv_samle");
	return name;
}

// ------------------------------------
// helpers

void AlgorithmPtv::sendHandshakeOrder(BrkLib::BrokerConnect& brkConnect)
	{
		// handshake order
		BrkLib::Order handShake;

		
		handShake.orderType_m = BrkLib::OP_HANDSHAKE;
		handShake.comment_m = "Handshake Order for PTV";

		BrkLib::BrokerOperation oper(brkConnect);
		//oper.unblockingIssueOrder( handShake);
		orderSendHelper(oper, handShake);
	}

	// ------------------------------

	void AlgorithmPtv::sendMetainfoOrder(
		BrkLib::BrokerConnect& brkConnect, 
		CppUtils::String const& symbol	)
	{
		BrkLib::Order mInfo;
		mInfo.orderType_m = BrkLib::OP_GET_SYMBOL_METAINFO;
		initFromFullName(symbol, mInfo);
		
		mInfo.provider_m = PROVIDER;
		

		BrkLib::BrokerOperation oper(brkConnect);
		//oper.unblockingIssueOrder( mInfo);
		orderSendHelper(oper, mInfo);
	}

	// ----------------------------

	void AlgorithmPtv::recalcLastDay(
		Inqueue::ContextAccessor& historyaccessor, 
		BrkLib::BrokerConnect& brkConnect, 
		TradeParams const& tradePars, 
		int const symbol_id 
	)
	{
			
	
		// store previous values
		//previousDay_m = currentDay_m;

		currentDay_m.clear();

		// new period
		Inqueue::DataPtr ptr = historyaccessor.dataLast(symbol_id);
		HlpStruct::PriceDataList priceDataList;

		if (historyaccessor.getBackWardPriceList(symbol_id, priceDataList, ptr, tradePars.atr_period_m)) {
			currentDay_m.dayATR_m = Math::calcATR(priceDataList, Math::P_Bid );
			

	  
			//
			// trend recognition period
			ptr = historyaccessor.dataLast(symbol_id);
			if (historyaccessor.getBackWardPriceList(symbol_id, priceDataList, ptr, tradePars.trend_channel_m)) {
			
				//
				double upc = previousDay_m.upperChannel_m;
				double lowc = previousDay_m.lowerChannel_m;

				

				// 
				int trend_switch = 0;
				int is_trend = previousDay_m.trend_signal_m;
				int days_no_trend = previousDay_m.days_no_trend_m;

				// current price (we asusume we begin analysys aftre day closure)
				HlpStruct::CandleData candle;
				priceDataList[0].getBidCandle(candle);
			

				if (upc > 0 && candle.high_m > upc ) {
					trend_switch = 1; // upper trend
					days_no_trend = 0;
				}

				
				if (lowc > 0 && candle.low_m < lowc ) {
					trend_switch = -1; // lower trend
					days_no_trend = 0;
				}
					
				      
							
				// был тренд но мы его не детектируем - инкрементируем счетчик
				if (is_trend != 0 && trend_switch==0) {
					days_no_trend++;
	         
					if (days_no_trend >= tradePars.bars_no_trend_m) {
							is_trend = 0;
					}
				}


				// this is done later as results at the end of the day
				if (is_trend==0) {
					if (trend_switch==1) {
						is_trend = 1;
					}
				
					if (trend_switch==-1) {
						is_trend = -1;
					}
				}

				
			
							
				
				// prev

				
				// store
				currentDay_m.trend_signal_m = is_trend;
				currentDay_m.days_no_trend_m = days_no_trend;

			

				// new channel
				Math::calcChannel(upc, lowc, priceDataList, Math::P_Bid );

				
				currentDay_m.upperChannel_m = upc;
				currentDay_m.lowerChannel_m = lowc;

			
				// last day
				currentDay_m.dayHigh_m = candle.high_m;
				currentDay_m.dayLow_m = candle.low_m;
				currentDay_m.dayClose_m = candle.close_m;
				currentDay_m.dayOpen_m = candle.open_m;
			

			}

			
		}

			
	}

// ----------------------------

void AlgorithmPtv::eachTickBreakThroughOrderManage(
			HlpStruct::RtData const& rtdata,
			Inqueue::ContextAccessor& historyaccessor, 
			BrkLib::BrokerConnect& brkConnect, 
			TradeParams const& tradePars, 
			int const symbol_id 
)
{

		
		// channel
		double upc = currentDay_m.upperChannel_m;
		double lowc = currentDay_m.lowerChannel_m;


		//int hour_signal_begin = tradePars.hour_signal_begin_m;
		//int hour_signal_end = tradePars.hour_signal_end_m;

		// signal break through happened
		if (rtdata.getRtData().bid_m >= upc ) {
			firedSignals_m.upperChannelBreakThrough_m = 1;
		}

		if (rtdata.getRtData().bid_m <= lowc) {
			firedSignals_m.lowChannelBreakThrough_m = 1;
		}



}

// -------------------------------

void AlgorithmPtv::eachTickReflectionOrderManage(
			HlpStruct::RtData const& rtdata,
			Inqueue::ContextAccessor& historyaccessor, 
			BrkLib::BrokerConnect& brkConnect, 
			TradeParams const& tradePars, 
			int const symbol_id 
		)
{
	

}

// -------------------------------

void AlgorithmPtv::lastDayBreakThroughOrderManage(
			HlpStruct::RtData const& rtdata,
			Inqueue::ContextAccessor& historyaccessor, 
			BrkLib::BrokerConnect& brkConnect, 
			TradeParams const& tradePars, 
			int const symbol_id 
)
{
				

		// whether is trend
		int is_trend = currentDay_m.trend_signal_m; 

		// set up new orders if we have signals
		double upc = currentDay_m.upperChannel_m;
		double lowc = currentDay_m.lowerChannel_m;
		
		int signal_sell = firedSignals_m.lowChannelBreakThrough_m;
		int signal_buy = firedSignals_m.upperChannelBreakThrough_m;
		
		double atrVAL = currentDay_m.dayATR_m;

		// get the previous date minimum
		double last_high = currentDay_m.dayHigh_m;
		double last_low = currentDay_m.dayLow_m;
		double last_close = currentDay_m.dayClose_m;

		double prev_last_high = previousDay_m.dayHigh_m;
		double prev_last_low = previousDay_m.dayLow_m;
		double prev_last_close = previousDay_m.dayClose_m;

	
		// iterate through position list and adjust SL
		BrkLib::BrokerOperation oper(brkConnect);
		BrkLib::PositionList const& poslist = oper.getBrokerPositionList(BrkLib::QUERY_OPEN | BrkLib::QUERY_PENDING);
		for(int i = 0; i < poslist.size(); i++) {
			BrkLib::Position const& pos_i = poslist[i];
			
			// if this order exists
			map<CppUtils::String, int>::iterator it = breakThroughPositionsState_m.find(pos_i.brokerPositionID_m);

			if (it != breakThroughPositionsState_m.end() ) {
				int state_o = it->second;


				if (pos_i.posState_m == BrkLib::STATE_OPEN) {
					// need to create new order to change SL if pos is opened
					
					if (state_o==POS_BREAKTHROUGH_EXECUTED_1_DAY) {


						// must set up last extremum
						if (pos_i.lastPosType_m == BrkLib::POS_BUY ) {
							if (pos_i.stopLossPrice_m < last_low) {

								/*
								BrkLib::Order order_change;
								order_change.changeStopLoss(
									rtdata.getTime(), 
									pos_i.brokerPositionID_m, 
									rtdata.getProvider(),
									rtdata.getSymbol(),
									last_low,
									"changing stoploss - after order is active"
								);


								

						
								//oper.unblockingIssueOrder(order_change);
								orderSendHelper(oper, order_change);
								*/
							}
						}
						else if (pos_i.lastPosType_m == BrkLib::POS_SELL ) {
							if (pos_i.stopLossPrice_m > last_high) {
								

								/*
								BrkLib::Order order_change;

								order_change.changeStopLoss(
									rtdata.getTime(), 
									pos_i.brokerPositionID_m, 
									rtdata.getProvider(),
									rtdata.getSymbol(),
									last_high,
									"changing stoploss - after order is active"
								);

										
						
								//oper.unblockingIssueOrder(order_change);
								orderSendHelper(oper, order_change);
								*/
							}
						}



						// set up new state
						it->second = POS_BREAKTHROUGH_EXECUTED_MORE_1_DAY;
					}
					else if (state_o==POS_BREAKTHROUGH_EXECUTED_MORE_1_DAY) {
						
						// another state
						// must set up last extremum if closure price more than 
						if (pos_i.lastPosType_m == BrkLib::POS_BUY ) {
							if (last_high > 0 && prev_last_high >0 && last_high > prev_last_high) {
								if (pos_i.stopLossPrice_m < last_low) {
										BrkLib::Order order_change;

										BrkLib::changeStopLoss( 
											order_change,
											rtdata.getTime(), 
											pos_i.brokerPositionID_m, 
											rtdata.getProvider(),
											rtdata.getSymbol2().substr(0,3),
											rtdata.getSymbol2().substr(3,3),
											last_low,
											"changing stoploss - next day tracking"
										);
											
										order_change.tag_m = RESPONSE_TAG_BREAKTHROUGH;
										//oper.unblockingIssueOrder(order_change);
										orderSendHelper(oper, order_change);

								}
							}
						}
						else if (pos_i.lastPosType_m == BrkLib::POS_SELL ) {
							if (last_low > 0 && prev_last_low >0 && last_low < prev_last_low) {
								if (pos_i.stopLossPrice_m > last_high) {
										BrkLib::Order order_change;
										BrkLib::changeStopLoss(
											order_change,
											rtdata.getTime(), 
											pos_i.brokerPositionID_m, 
											rtdata.getProvider(),
											rtdata.getSymbol2().substr(0,3),
											rtdata.getSymbol2().substr(3,3),
											last_high,
											"changing stoploss - next day tracking"
										);
		
						
										order_change.tag_m = RESPONSE_TAG_BREAKTHROUGH;
										//oper.unblockingIssueOrder(order_change);
										orderSendHelper(oper, order_change);
								}
							}
						}

					} //end of state check
				 
				
				} // end if open state
				else if (pos_i.posState_m == BrkLib::STATE_PENDING) {
					
					// expiration
					double time_span = tradePars.bars_no_trend_m*24*60*60;
					if ((rtdata.getTime() - pos_i.installTime_m) > time_span) {
						BrkLib::Order order_cancel;

						BrkLib::initCancelOrder(
							order_cancel, 
							rtdata.getTime(), 
							pos_i.brokerPositionID_m, 
							rtdata.getProvider(),
							rtdata.getSymbol2().substr(0,3),
							rtdata.getSymbol2().substr(3,3),
							"cancel - bars no trend"
						);

					
						order_cancel.tag_m = RESPONSE_TAG_BREAKTHROUGH;	
						//oper.unblockingIssueOrder(order_cancel);
						orderSendHelper(oper, order_cancel);
					}
					
					// the touch of previous channel
					
					if ( 
						(isLong(pos_i.lastPosType_m) && signal_sell) ||
						(!isLong(pos_i.lastPosType_m) && signal_buy )
					)
					{

						BrkLib::Order order_cancel;

						BrkLib::initCancelOrder(
							order_cancel, 
							rtdata.getTime(), 
							pos_i.brokerPositionID_m, 
							rtdata.getProvider(),
							rtdata.getSymbol2().substr(0,3),
							rtdata.getSymbol2().substr(3,3),
							"cancel - opposite channel touch"
						);


						order_cancel.tag_m = RESPONSE_TAG_BREAKTHROUGH;
						//oper.unblockingIssueOrder(order_cancel);
						orderSendHelper(oper, order_cancel);
					}

				}
			
			} // end if pending

		} // end if in the list


		// check if we need to open new order
		
		if (signal_buy ) {
			BrkLib::Order order;

			double slPrice = upc - atrVAL;
			double tpPrice = upc + atrVAL;
			double execPrice = upc;

			BrkLib::initStopOrder(
				order, 
				rtdata.getProvider(), 
				rtdata.getSymbol2().substr(0,3),
				rtdata.getSymbol2().substr(3,3), 
				true, 
				slPrice, 
				tpPrice, 
				execPrice, 
				100000, 
				-1, 
				rtdata.getTime()
			);
			order.comment_m = "buy breakthrough";

			BrkLib::BrokerOperation oper(brkConnect);
			order.tag_m = RESPONSE_TAG_BREAKTHROUGH;
			//oper.unblockingIssueOrder(order);
			orderSendHelper(oper, order);
			
		}
		
		if (signal_sell ) {
			BrkLib::Order order;

			double slPrice = lowc + atrVAL;
			double tpPrice = lowc - atrVAL;
			double execPrice = lowc;

			BrkLib::initStopOrder(
				order, 
				rtdata.getProvider(), 
				rtdata.getSymbol2().substr(0,3),
				rtdata.getSymbol2().substr(3,3),
				false, 
				slPrice, 
				tpPrice, 
				execPrice, 
				100000, 
				-1,
				rtdata.getTime());
			order.comment_m = "sell breakthrough";

			BrkLib::BrokerOperation oper(brkConnect);
			order.tag_m = RESPONSE_TAG_BREAKTHROUGH;
			//oper.unblockingIssueOrder(order);
			orderSendHelper(oper, order);
		}

}

void AlgorithmPtv::lastDayReflectionOrderManage(
			HlpStruct::RtData const& rtdata,
			Inqueue::ContextAccessor& historyaccessor, 
			BrkLib::BrokerConnect& brkConnect, 
			TradeParams const& tradePars, 
			int const symbol_id 
		)
{

	BrkLib::BrokerOperation oper(brkConnect);
	BrkLib::PositionList const& poslist = oper.getBrokerPositionList(BrkLib::QUERY_OPEN | BrkLib::QUERY_PENDING);
	for(int i = 0; i < poslist.size(); i++) {
		BrkLib::Position const& pos_i = poslist[i];

		// manage already opened orderts
		map<CppUtils::String, int>::iterator it = reflectionPositionsState_m.find(pos_i.brokerPositionID_m);
		if (it != breakThroughPositionsState_m.end() ) {
			int state_o = it->second;

			if (pos_i.posState_m == BrkLib::STATE_OPEN) {
			
			}
			else if (pos_i.posState_m == BrkLib::STATE_PENDING) {

				// need to cancel orders - after one day we cancel pending orders
				// as all these day were issued a day ago we are cancelling all of them
				BrkLib::Order order_cancel;

			
				order_cancel.tag_m = RESPONSE_TAG_REFLECTION;
				BrkLib::initCancelOrder(
							order_cancel, 
							rtdata.getTime(), 
							pos_i.brokerPositionID_m, 
							rtdata.getProvider(),
							rtdata.getSymbol2().substr(0,3),
							rtdata.getSymbol2().substr(3,3),
							"cancel - 1 day elapsed"
				);
				//oper.unblockingIssueOrder(order_cancel);
				orderSendHelper(oper, order_cancel);


			}
		} // end if in the list
	}


	// ------------------------------
	if (firedSignals_m.lowChannelBreakThrough_m==1 && currentDay_m.dayClose_m > previousDay_m.lowerChannel_m) {
		firedSignals_m.lowChannelTouch_m = 1;
		
	}

	if (firedSignals_m.upperChannelBreakThrough_m==1 && currentDay_m.dayClose_m < previousDay_m.upperChannel_m ) {
		firedSignals_m.upperChannelTouch_m = 1;
	}


	// log that

	// if we have signals set limit orders from the touched level
	// but this is old channel

	double upc = previousDay_m.upperChannel_m;
	double lowc = previousDay_m.lowerChannel_m;
	double atrVAL = currentDay_m.dayATR_m;
		

	/*
	if (firedSignals_m.upperChannelTouch_m==1) {
		// sell
		BrkLib::Order order;

		double slPrice = upc + atrVAL;
		double tpPrice = upc - atrVAL;
		double execPrice = upc;

		order.initLimitOrder(rtdata.getProvider(), rtdata.getSymbol(), false, slPrice, tpPrice, execPrice, 100000, -1, rtdata.getTime());
		order.comment_m = "sell reflection";

		BrkLib::BrokerOperation oper(brkConnect);
		order.tag_m = RESPONSE_TAG_REFLECTION;
		oper.unblockingIssueOrder(order);
		
	}
	else if (firedSignals_m.lowChannelTouch_m==1) {

		// buy
		BrkLib::Order order;

		double slPrice = lowc - atrVAL;
		double tpPrice = lowc + atrVAL;
		double execPrice = lowc;

		order.initLimitOrder(rtdata.getProvider(), rtdata.getSymbol(), true, slPrice, tpPrice, execPrice, 100000, -1, rtdata.getTime());
		order.comment_m = "buy reflection";

		BrkLib::BrokerOperation oper(brkConnect);
		order.tag_m = RESPONSE_TAG_REFLECTION;
		//oper.unblockingIssueOrder(order);
		orderSendHelper(oper, order);
		
	}
	*/


	
	if (firedSignals_m.upperChannelTouch_m==1) {
		// sell
		BrkLib::Order order;

		double slPrice = upc - atrVAL;
		double tpPrice = upc + atrVAL;
		double execPrice = upc;

		BrkLib::initStopOrder(
			order, 
			rtdata.getProvider(),
			rtdata.getSymbol2().substr(0,3),
			rtdata.getSymbol2().substr(3,3),
			true, slPrice, tpPrice, execPrice, 100000, -1, rtdata.getTime());
		order.comment_m = "buy stop pseudo-reflection";

		BrkLib::BrokerOperation oper(brkConnect);
		order.tag_m = RESPONSE_TAG_REFLECTION;
		//oper.unblockingIssueOrder(order);
		orderSendHelper(oper, order);
		
	}
	else if (firedSignals_m.lowChannelTouch_m==1) {

		// buy
		BrkLib::Order order;

		double slPrice = lowc + atrVAL;
		double tpPrice = lowc - atrVAL;
		double execPrice = lowc;

		BrkLib::initStopOrder(
			order, 
			rtdata.getProvider(), 
			rtdata.getSymbol2().substr(0,3),
			rtdata.getSymbol2().substr(3,3),
			false, 
			slPrice, 
			tpPrice, 
			execPrice, 
			100000, 
			-1, 
			rtdata.getTime());
		order.comment_m = "sell stop pseudo-reflection";

		BrkLib::BrokerOperation oper(brkConnect);
		order.tag_m = RESPONSE_TAG_REFLECTION;
		//oper.unblockingIssueOrder(order);
		orderSendHelper(oper, order);
		
	}
	


}

// ----------------

void AlgorithmPtv::orderSendHelper(BrkLib::BrokerOperation& oper, BrkLib::Order const& order)
{
	// send alert
	oper.unblockingIssueOrder( order );

	ALERTEVENTNC("DUMMY",BrkLib::AP_MEDIUM, "OPEN POS", 
		"Symbol: " << order.symbolNum_m << "-" << order.symbolDenom_m << "\n " <<
		"Issue time: " <<(order.orderIssueTime_m > 0 ? CppUtils::getGmtTime2(order.orderIssueTime_m): "NA" ) << "\n " <<
		"Order type: " <<	BrkLib::getOrderTypeName(order.orderType_m) << "\n " <<
		"SL Price: " <<	(order.orSLprice_m > 0 ? CppUtils::float2String(order.orSLprice_m, -1, 4) : "NA" ) << "\n " <<
		"TP Price: " <<	(order.orTPprice_m > 0 ? CppUtils::float2String(order.orTPprice_m, -1, 4) : "NA" ) << "\n " <<
		"Execution price: " <<	(order.orPrice_m > 0 ? CppUtils::float2String(order.orPrice_m, -1, 4) : "NA" ) << "\n " <<
		"Expire on: " <<	(order.orExpirationTime_m > 0 ? CppUtils::getGmtTime2(order.orExpirationTime_m): "NA" ) 
	);
}


}; // end of namespace