
#include "algorithmsilverchannel.hpp"
#include "../brklib/brklib.hpp"

// math stuff
#include "../math/math.hpp"

#define ALGSILVER "ALGSILVER"


namespace AlgLib {
	
	static id_m = 0;
	
	AlgorithmBase *creator (Inqueue::AlgorithmHolder* algholder)
	{
		return new AlgorithmSilverChannel(*algholder);
	}

	void terminator (AlgorithmBase *pthis)
	{
		assert(pthis);
		delete pthis;
	}

	AlgorithmBase::CreatorAlgorithmFun createAlgorithm = creator;

	AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm = terminator;


	// intreface to be implemented

	AlgorithmSilverChannel::AlgorithmSilverChannel( Inqueue::AlgorithmHolder& algHolder):
		AlgorithmBase(algHolder)
	{
		
	}

	AlgorithmSilverChannel::~AlgorithmSilverChannel()
	{
	}

	void AlgorithmSilverChannel::onLibraryInitialized(
		Inqueue::AlgBrokerDescriptor const& descriptor, 
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
		)
	{
	
		//doLog(HlpStruct::CommonLog::LL_INFO, "Algorithm loaded");
		forceSendConnectEvent_m = true;
		if (brokerlib==NULL)
			return;

		CppUtils::StringList keys;
		descriptor.initialAlgBrkParams_m.algParams_m.getValueKeys(keys);

		for(int i = 0; i < keys.size(); i++) {
			CppUtils::Value const& param = descriptor.initialAlgBrkParams_m.algParams_m.getParameter( keys[i] );
			LOGEVENT(HlpStruct::CommonLog::LL_INFO,  ALGSILVER, "Param: " << keys[i] << " Value: " << param.getAsString() );
			
		}

		//
		aggrPeriod_m = (Inqueue::AggregationPeriods)atoi(descriptor.initialAlgBrkParams_m.algParams_m.getParameter("AGGRPER").getAsString().c_str());
		multFactor_m = atoi(descriptor.initialAlgBrkParams_m.algParams_m.getParameter("MULTFACT").getAsString().c_str());
		//
		LOGEVENT(HlpStruct::CommonLog::LL_INFO,  ALGSILVER, "Initialized: aggrPeriod_m=" << aggrPeriod_m << " multFactor_m=" << multFactor_m);

		
		BrkLib::BrokerConnect brkConn(brokerlib, "dummy", false, true, runName.c_str(), comment.c_str());

		brkConn.connect();
		
		if (brkConn.getSession().isConnected()) {
				checkState(brkConn);

				// handshake order
				BrkLib::Order handShake;

				handShake.ID_m.generate();
				handShake.orderType_m = BrkLib::OP_HANDSHAKE;
				handShake.comment_m = "dummy handshake order";

				BrkLib::BrokerOperation brkOper(brkConn);
				brkOper.unblockingIssueOrder(handShake);
			
									
		}
		
	

	}

	void AlgorithmSilverChannel::onLibraryFreed()
	{
		LOGEVENT(HlpStruct::CommonLog::LL_INFO,  ALGSILVER, "Algorithm released" );
	}

	void AlgorithmSilverChannel::onBrokerResponseArrived(
		BrkLib::BrokerConnect &brkConnect,
			BrkLib::BrokerResponseBase const& bresponse
		)
	{
		
	
		if (bresponse.type_m==BrkLib::RT_BrokerResponseOrder) {
				int event = EVENT_NO_ACTION;		
			// session is always valid here

			BrkLib::BrokerResponseOrder const& bRespOrder = (BrkLib::BrokerResponseOrder const&)bresponse;
			
			/*
			if (bRespOrder.resultCode_m == BrkLib::PT_ORDER_ESTABLISHED) {
				event = EVENT_ORDER_OPENED;
			}
			else if (bRespOrder.resultCode_m == BrkLib::PT_ORDER_CLOSED) {
				event = EVENT_ORDER_CLOSED;
			}
			
			doStep(event);
			*/
		}

		
	}

	// will implement state machine
	void AlgorithmSilverChannel::onDataArrived(
		HlpStruct::RtData const& rtdata, 
		Inqueue::ContextAccessor& historyaccessor, 
		Inqueue::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
	)
	{
		// initialize event
		int event = EVENT_NO_ACTION;

		
		if (brokerlib != NULL) {
			
			// broker lib is valid
			BrkLib::BrokerConnect brkConn(brokerlib, "dummy", false, true, runName.c_str(), comment.c_str());
			
			
			

			// attempt to connect
			brkConn.connect();
		
			
			if (brkConn.getSession().isConnected()) {

				checkState(brkConn);


				

				// query for necessary signal
				// parameters
				int maShort = atol(descriptor.algParams_m.getParameter("MA_SHORT").getAsString().c_str());
				int maLong = atol(descriptor.algParams_m.getParameter("MA_LONG").getAsString().c_str());
				//int lots = descriptor.algParams_m.getParameter("LOTS").getAsString();
				int lots = 100000;

				/*
				ALG_LOGEVENT(HlpStruct::CommonLog::LL_INFO, getSessionName(), ALGSILVER, 
					"MA_SHORT=" << CppUtils::long2String(maShort) << 
					" MA_LONG=" << CppUtils::long2String(maLong) <<
					" LOTS=" << CppUtils::long2String(lots)
				);
				*/
				
							
				
				int symbol_c = historyaccessor.cacheSymbol(rtdata.getProvider(), rtdata.getSymbol(), aggrPeriod_m, multFactor_m );
				HlpStruct::PriceData const& unclosed = historyaccessor.getUnclosedCandle(symbol_c);
				if (symbol_c > 0) {
				
					if (!unclosed.isNewPeriod()) {
						// force to work only when period is switched
						// make check only after cache operation happened
						
				


						LOGEVENT(HlpStruct::CommonLog::LL_INFO,  ALGSILVER, "Still old period: " << unclosed.toString() );
						return;
					}
					else {
						LOGEVENT(HlpStruct::CommonLog::LL_INFO,  ALGSILVER, "New period switch: " << unclosed.toString() );
					}
					
												  
					Inqueue::DataPtr ptr = historyaccessor.dataLast(symbol_c);
						HlpStruct::PriceDataList dataListShort(maShort);
						HlpStruct::PriceDataList dataListLong(maLong);
						int maxCnt = max(maShort, maLong);
  
            int cnt = 0;
						while(historyaccessor.dataPrev(symbol_c,ptr)) {
							if (++cnt > maxCnt)
								break;

							HlpStruct::PriceData const &pd = historyaccessor.getCurrentPriceData(symbol_c, ptr);

							/*
							ALG_LOGEVENTA(HlpStruct::CommonLog::LL_INFO,  ALGSILVER, 
								"Unclosed time: " <<  CppUtils::getAscTime(unclosed.time_m) << " # " << cnt-1 << " iter price data time: " << CppUtils::getAscTime(pd.time_m),
								"Open=" + CppUtils::double2String(pd.open2_m) <<
								"High=" + CppUtils::double2String(pd.high2_m) <<
								"Low=" + CppUtils::double2String(pd.low2_m) <<
								"Close=" + CppUtils::double2String(pd.close2_m) );
							*/

							if (cnt <= maShort)
								dataListShort[cnt - 1] = pd;
							if (cnt <= maLong)
								dataListLong[cnt - 1] = pd;
						}

						// not enough data
						if (cnt < maxCnt) {
						
							LOGEVENT(HlpStruct::CommonLog::LL_DEBUG,  ALGSILVER, "Not enough data: " << cnt );

							return;
						}

						// calculate avarages
						// close bid prices
						double avrShort = Math::calcSimpleAvr(dataListShort, Math::P_CloseBid);
						double avrLong = Math::calcSimpleAvr(dataListLong, Math::P_CloseBid);

						LOGEVENTA(HlpStruct::CommonLog::LL_DEBUGDETAIL,  ALGSILVER, "Trade parameters",
							CppUtils::getAscTime(rtdata.getTime()) << 
							rtdata.getSymbol() <<
							"Bid=" + CppUtils::double2String(rtdata.getRtData().bid_m) <<
							"Ask=" + CppUtils::double2String(rtdata.getRtData().ask_m) <<
							"Avr Short=" + CppUtils::double2String(avrShort) << 
							"Avr Long=" + CppUtils::double2String(avrLong)	);
					
						// get the previous step unique for each step

						double avrShortPrev = -1;
						double avrLongPrev	=	-1;

						if (historyaccessor.getThreadGlobal().isGlobalExist(GLB_MA_SHORT)) {
							avrShortPrev = historyaccessor.getThreadGlobal().getGlobal(GLB_MA_SHORT).getAsDouble();
						}
 
						if (historyaccessor.getThreadGlobal().isGlobalExist(GLB_MA_LONG)) {
							avrLongPrev = historyaccessor.getThreadGlobal().getGlobal(GLB_MA_LONG).getAsDouble();
						}

						if (avrShortPrev > 0 && avrLongPrev > 0) { 
							// here we have valid signals
							if (avrLongPrev < avrShortPrev && avrLong > avrShort) {
								// sell signal
								event = EVENT_SHORT_SIGNAL;
								LOGEVENT(HlpStruct::CommonLog::LL_INFO,  ALGSILVER, "Signal to open SHORT position" );
							}
							else if (avrLongPrev > avrShortPrev && avrLong < avrShort) {
								// buy signal 
								event = EVENT_LONG_SIGNAL;
								LOGEVENT(HlpStruct::CommonLog::LL_INFO,  ALGSILVER, "Signal to open LONG position" );
							}
							else {
								LOGEVENT(HlpStruct::CommonLog::LL_INFO,  ALGSILVER, "So far no valid signals" );
							}

							// begin state machine
							if (brkConn.getSession().getState()==STATE_NO_OPEN_POS) {
			
								if (event==EVENT_LONG_SIGNAL) {
									// open long

									BrkLib::Order openLongOrder;

									openLongOrder.ID_m.generate();
									openLongOrder.orderType_m = BrkLib::OP_BUY;
									openLongOrder.symbolNum_m = "EUR";
									openLongOrder.symbolDenom_m = "USD";
									openLongOrder.orVolume_m = 100000.0;
									openLongOrder.orPrice_m = -1;

									BrkLib::BrokerOperation oper(brkConn);
									oper.unblockingIssueOrder(openLongOrder);
									
								}
								else if (event==EVENT_SHORT_SIGNAL) {
									// open short
									BrkLib::Order openShortOrder;

									openShortOrder.ID_m.generate();
									openShortOrder.orderType_m = BrkLib::OP_SELL;
									openShortOrder.symbolNum_m = "EUR";
									openShortOrder.symbolDenom_m = "USD";
									openShortOrder.orVolume_m = 100000.0;
									openShortOrder.orPrice_m = -1;

									BrkLib::BrokerOperation oper(brkConn);
									oper.unblockingIssueOrder(openShortOrder);
									

								}
							}
							else if (brkConn.getSession().getState()==STATE_SHORT_OPENED) {
								if (event==EVENT_LONG_SIGNAL) {
									// close pos
									BrkLib::Order closeOrder;
									closeOrder.ID_m.generate();
									closeOrder.brokerPositionID_m = posId_m;
									closeOrder.orVolume_m = 100000.0;
									closeOrder.orderType_m = BrkLib::OP_CLOSE;
									
									BrkLib::BrokerOperation oper(brkConn);
									oper.unblockingIssueOrder(closeOrder);
									
								

									// need to open opposite
									// open long
									BrkLib::Order openShortOrder;

									openShortOrder.ID_m.generate();
									openShortOrder.orderType_m = BrkLib::OP_BUY;
									openShortOrder.symbolNum_m = "EUR";
									openShortOrder.symbolDenom_m = "USD";
									openShortOrder.orVolume_m = 100000.0;
									openShortOrder.orPrice_m = -1;

									oper.unblockingIssueOrder(openShortOrder);
									
								}
							
							}
							else if (brkConn.getSession().getState()==STATE_LONG_OPENED) {
								if (event==EVENT_SHORT_SIGNAL) {
									// close pos
									BrkLib::Order closeOrder;
									closeOrder.ID_m.generate();
									closeOrder.brokerPositionID_m = posId_m;
									closeOrder.orVolume_m = 100000.0;
									closeOrder.orderType_m = BrkLib::OP_CLOSE;
									
									BrkLib::BrokerOperation oper(brkConn);
									oper.unblockingIssueOrder(closeOrder);

									

									// need to open opposite
									// open short
									BrkLib::Order openLongOrder;

									openLongOrder.ID_m.generate();
									openLongOrder.orderType_m = BrkLib::OP_SELL;
									openLongOrder.symbolNum_m = "EUR";
									openLongOrder.symbolDenom_m = "USD";
									openLongOrder.orVolume_m = 100000.0;
									openLongOrder.orPrice_m = -1;

									oper.unblockingIssueOrder(openLongOrder);
									
								}
							}
							// end state machine


						}
						

						// store globals
						CppUtils::Value avrShortVal, avrLongVal;
						avrShortVal.getAsDouble(true) = avrShort;
						avrLongVal.getAsDouble(true) = avrLong;

						historyaccessor.getThreadGlobal().putGlobal(GLB_MA_SHORT,avrShortVal );
						historyaccessor.getThreadGlobal().putGlobal(GLB_MA_LONG,avrLongVal );

												
					
				}
				else {
					LOGEVENT(HlpStruct::CommonLog::LL_WARN,  ALGSILVER, "Cannot cache symbol: " << CppUtils::String(rtdata.getProvider()) << " - " << rtdata.getSymbol() );
				}
				

			}
			else {
				LOGEVENT(HlpStruct::CommonLog::LL_WARN,  ALGSILVER,"Cannot connect to broker layer: " + brkConn.getSession().getConnection() );
		
			}


		


		}
		
	}

	void AlgorithmSilverChannel::onLevel2DataArrived(
			HlpStruct::RtLevel2Data const& level2data,
			Inqueue::TradingParameters const& descriptor,
			CppUtils::String const& runName,
			CppUtils::String const& comment
	)
	{
	}
	
	
	bool AlgorithmSilverChannel::onEventArrived(HlpStruct::EventData const &eventdata, HlpStruct::EventData& response)
	{
		return false;
	}

	CppUtils::String const& AlgorithmSilverChannel::getName() const
	{
		static const CppUtils::String name("Silver Channel");
		return name;
	}

	
	// checks for automat state
	void AlgorithmSilverChannel::checkState(BrkLib::BrokerConnect &brkConn)
	{
		brkConn.getSession().setState( STATE_NO_OPEN_POS );
		posId_m.clear();

		// here we shall check what about positions to determine the state of automat
		BrkLib::BrokerOperation brkOper(brkConn);
		BrkLib::PositionList const& poslist = brkOper.getBrokerPositionList( BrkLib::QUERY_OPEN );
		for(int i = 0; i < poslist.size(); i++) {
			BrkLib::Position const& pos = poslist[ i ];

			// basing on close time determine whether we have open orders 
			if (pos.ID_m.isValid()  ) {
				if (pos.lastPosType_m==BrkLib::POS_BUY) {
					brkConn.getSession().setState(STATE_LONG_OPENED);
					LOGEVENT(HlpStruct::CommonLog::LL_INFO,  ALGSILVER, "Current state is: " << "STATE_LONG_OPENED" );

					posId_m = pos.brokerPositionID_m;
					return;
				}
				else if (pos.lastPosType_m==BrkLib::POS_SELL) {
					brkConn.getSession().setState(STATE_SHORT_OPENED);
					LOGEVENT(HlpStruct::CommonLog::LL_INFO,  ALGSILVER, "Current state is: " << "STATE_SHORT_OPENED" );

					posId_m = pos.brokerPositionID_m;
					return;
				}
			}
		}
		
		LOGEVENT(HlpStruct::CommonLog::LL_INFO,  ALGSILVER, "Current state is: " << "STATE_NO_OPEN_POS" );

	};

	

} // end of namespace