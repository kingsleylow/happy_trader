#include "state.hpp"
#include "implement.hpp"

namespace AlgLib {

	bool TS_Initial::entry_action(CppUtils::BaseEvent const& event)
	{
			CAlgMisha& base = context().getBase();
			

			if (event.getClassId() == TE_On_Level2_Data::CLASS_ID) {
				 TE_On_Level2_Data const &event_i = (TE_On_Level2_Data const &)event;
				
				 // volume level is not initialized but cur tick price exists
				if (event_i.curTickPrice_m > 0) {

					int min_idx = -1;
					double min_price_diff = -1;

					// detect big volume, and must be closest to cutrrent tick price
					for(int i = 0; i < event_i.level2_m.getSize(); i++) {
						
							if (base.getTradeParam().cutoffVolume_m <= event_i.level2_m.getLevel2Entry(i).getVolume() ) {

								double price_diff = abs( event_i.level2_m.getLevel2Entry(i).getPrice() - event_i.curTickPrice_m);
								
								if (min_idx <0) {
									min_idx = i;
									min_price_diff = price_diff;
									continue;
								}

								if (min_price_diff > price_diff) {
									min_price_diff = price_diff;
									min_idx = i;
								}

							}
						

					}

				
					
					if (min_idx >=0) {
						// found
						//


						// issue event
						TE_Detected_Big_Volume new_event_i;
						new_event_i.trackingPrice_m = event_i.level2_m.getLevel2Entry( min_idx ).getPrice();
						// current time
						new_event_i.volumeDetectTime_m = CppUtils::getTime();
						new_event_i.trackingVolume_m = event_i.level2_m.getLevel2Entry( min_idx ).getVolume();

					
						base.getTradingMachines().processEvent<TE_Detected_Big_Volume>(context().getMachineSymbol(), new_event_i);
					}

				}
			}

			return true;
	}

	bool TS_LevelTrack::entry_action(CppUtils::BaseEvent const& event)
	{
			CAlgMisha& base = context().getBase();

			// foa all events we must reset context
			
			if (event.getClassId() == TE_On_Timer::CLASS_ID) {
					
				 TE_On_Timer const& event0_i = (TE_On_Timer const&)event;

				 double sec_elapsed = CppUtils::getTime() - volumeDetectTime_m;
				 if (sec_elapsed > base.getTradeParam().maxMinutesLevelTrack_m * 60) {
					 // must go back
					 TE_Detect_Time_Excess event_i;
					 event_i.secElapsed_m = sec_elapsed; 
					 base.getTradingMachines().processEvent<TE_Detect_Time_Excess>(context().getMachineSymbol(), event_i);
				 }
			}
			else if (event.getClassId() == TE_Detected_Big_Volume::CLASS_ID) {
				TE_Detected_Big_Volume const& event_i = (TE_Detected_Big_Volume const&)event;

				volumeDetectTime_m = event_i.volumeDetectTime_m;
				trackingPrice_m = event_i.trackingPrice_m;
				trackingVolume_m = event_i.trackingVolume_m;
			}
			else if (event.getClassId() == TE_On_Tick_Data::CLASS_ID) {
				// check if we can issue signals
				TE_On_Tick_Data	const& event2_i = (TE_On_Tick_Data	const&)event;

				if (trackingPrice_m > 0) {
					if (trackingPrice_m > event2_i.curPrice_m && trackingPrice_m <= event2_i.prevPrice_m)	{
						// down
						LOG_SYMBOL_SIGNAL(base.getRunName(), base.getRtdataProvider(), context().getMachineSymbol(), "Down signal, current price is: " << event2_i.curPrice_m);

						TE_Price_Crossed_Down event_i;
						base.getTradingMachines().processEvent<TE_Price_Crossed_Down>(context().getMachineSymbol(), event_i);

					}
					if (trackingPrice_m < event2_i.curPrice_m && trackingPrice_m >= event2_i.prevPrice_m) {
						// up
						LOG_SYMBOL_SIGNAL(base.getRunName(), base.getRtdataProvider(), context().getMachineSymbol(), "Up signal, current price is: " << event2_i.curPrice_m);

						TE_Price_Crossed_Up event_i;
						base.getTradingMachines().processEvent<TE_Price_Crossed_Up>(context().getMachineSymbol(), event_i);
					}

					if (base.getTradeParam().maxPriceDifference_m >=0) {
						if (abs(trackingPrice_m - event2_i.curPrice_m) > base.getTradeParam().maxPriceDifference_m) {

							TE_Price_Go_Far event_i;
							event_i.curPrice_m = event2_i.curPrice_m;
							event_i.trackingPrice_m = trackingPrice_m; 
							base.getTradingMachines().processEvent<TE_Price_Go_Far>(context().getMachineSymbol(), event_i);

						}
					}
				}
			}

			return true;
	}

	
	bool TS_IssueBuy::entry_action( CppUtils::BaseEvent const& event)
	{
		 CAlgMisha& base = context().getBase();

		 TE2_Start_Signal	event_i;
		 event_i.direction_m = BrkLib::TD_LONG;

		 base.getOrderMachines().processEvent<TE2_Start_Signal>(context().getMachineSymbol(),event_i );

		 return true;

	}


	bool TS_IssueSell::entry_action( CppUtils::BaseEvent const& event)
	{
		 CAlgMisha& base = context().getBase();

		 TE2_Start_Signal	event_i;
		 event_i.direction_m = BrkLib::TD_SHORT;

		 base.getOrderMachines().processEvent<TE2_Start_Signal>(context().getMachineSymbol(),event_i );

		 return true;
	}

	// ---------------------
	// Order machine
	// ---------------------
	// ---------------------//
	//											// !!!!!! /////////////
	// ---------------------//

	bool TS2_Ready_Start::entry_action ( CppUtils::BaseEvent const& event)
	{
		if (event.getClassId() == TE2_Start_Signal::CLASS_ID) {

			TE2_Start_Signal const& event_i = (TE2_Start_Signal const&)(event);

			// issue order
			CAlgMisha& base = context().getBase();

			// save
			direction_m = event_i.direction_m;
			MyBrokerContext c(*this);
			if (event_i.direction_m == BrkLib::TD_LONG) {
				LOG_SYMBOL_SIGNAL(base.getRunName(), base.getRtdataProvider(), context().getMachineSymbol(), "Issue buy (market)");
				base.issueBuyOrder(c,context().getMachineSymbol(), base.getTradeParam().tradeVolume_m);
			}
			else if (event_i.direction_m == BrkLib::TD_SHORT) {
				LOG_SYMBOL_SIGNAL(base.getRunName(), base.getRtdataProvider(), context().getMachineSymbol(), "Issue sell (market)");
				base.issueSellOrder(c,context().getMachineSymbol(), base.getTradeParam().tradeVolume_m);
			}

		}

		return true;
	}


	
	bool TS2_SL_Pending::entry_action( CppUtils::BaseEvent const& event)
	{
		CAlgMisha& base = context().getBase();
		if (event.getClassId() == TE2_Confirm_Order_Executed_Open::CLASS_ID) {

			// issue SL, need to know entry price and brokerOrderId from TE2_Confirm_Order_Executed

			TE2_Confirm_Order_Executed_Open const& event_i = (TE2_Confirm_Order_Executed_Open const&)(event);
		
			
			// store our open position ID
			this->brokerOpenPositionId_m = event_i.brokerPositionId_m;
			this->openPrice_m = event_i.executionPrice_m;
			

			// update current price here
			double cur_price = base.getCurrentPrice(context().getMachineSymbol());
			if (cur_price <=0) {
					LOG_SYMBOL_SIGNAL(base.getRunName(), base.getRtdataProvider(), context().getMachineSymbol(), 
						"On the stage to issue SL current price is negative! This can't be! Breaking!"
						);

					// go on with error
					TE2_Confirm_Order_Error_SL event_i;
					event_i.brokerOrderId_m =	 "N/A";
					event_i.reason = "Current price is invalid";
					base.getOrderMachines().processEvent< TE2_Confirm_Order_Error_SL >(context().getMachineSymbol(),event_i);
			}

			if (event_i.brokerPositionId_m.empty()) {
				LOG_SYMBOL_SIGNAL(base.getRunName(), base.getRtdataProvider(), context().getMachineSymbol(), 
						"On the stage to issue SL broker position ID is empty! This can't be! Breaking!"
						);

					// go on with error
					TE2_Confirm_Order_Error_SL event_i;
					event_i.brokerOrderId_m =	 "N/A";
					event_i.reason = "Position ID invalid";
					base.getOrderMachines().processEvent< TE2_Confirm_Order_Error_SL >(context().getMachineSymbol(),event_i);
			}

			MyBrokerContext c(*this);

			TS2_Ready_Start const& ready_start_state =  machine().state<TS2_Ready_Start>();
			if (ready_start_state.direction_m == BrkLib::TD_LONG) {

					
					double sprice = event_i.executionPrice_m - base.getTradeParam().sl_relativeLevel_m;
					
					
					
					LOG_SYMBOL_SIGNAL(base.getRunName(), base.getRtdataProvider(), context().getMachineSymbol(), 
						"Issue close long (SL) stop limit"  <<
						", cur price: " << cur_price <<
						", stop price: " << sprice << 
						", delayed: " << CppUtils::bool2String(base.getTradeParam().onlyMarketOrders_m)	<<
						", position ID: " << event_i.brokerPositionId_m
						);
					
					if (!base.getTradeParam().onlyMarketOrders_m)	 {
						base.issueCloseLongOrder_StopLimit(c,	sprice, sprice, context().getMachineSymbol(), base.getTradeParam().tradeVolume_m, event_i.brokerPositionId_m);
					}
					else {
						base.issueCloseLongOrder_Delayed_SL(c, sprice, context().getMachineSymbol(), base.getTradeParam().tradeVolume_m, event_i.brokerPositionId_m );
					}
			}
			else if (ready_start_state.direction_m == BrkLib::TD_SHORT) {
				double sprice = event_i.executionPrice_m + base.getTradeParam().sl_relativeLevel_m;

				// in any stop long (actually here we buy) current price must be LESS then stop price
				// though price can be old we trying to check it

			

				LOG_SYMBOL_SIGNAL(base.getRunName(), base.getRtdataProvider(), context().getMachineSymbol(), 
						"Issue close short (SL) stop limit" << 
						", cur price: " << cur_price <<
						", stop price: " << sprice << 
						", delayed: " << CppUtils::bool2String(base.getTradeParam().onlyMarketOrders_m) <<
						", position ID: " << event_i.brokerPositionId_m
					);


				if (!base.getTradeParam().onlyMarketOrders_m) {
						base.issueCloseShortOrder_StopLimit(c,	sprice, sprice,context().getMachineSymbol(), base.getTradeParam().tradeVolume_m, event_i.brokerPositionId_m );
				
				}
				else {
					base.issueCloseShortOrder_Delayed_SL(c, sprice, context().getMachineSymbol(), base.getTradeParam().tradeVolume_m, event_i.brokerPositionId_m );
				}
			}

		}

		return true;
	}

	bool TS2_In_Position::entry_action( CppUtils::BaseEvent const& event)
	{
		CAlgMisha& base = context().getBase();

		bool our = false;
		TS2_Ready_Start const& ready_start_state =  machine().state<TS2_Ready_Start>();
		if (event.getClassId() == TE2_Confirm_Order_Issued_SL::CLASS_ID) {
			TE2_Confirm_Order_Issued_SL const& event_i = (TE2_Confirm_Order_Issued_SL const&)(event);
			sl_brokerOrderId_m = event_i.brokerOrderId_m;
			our = true;
			
		}
		else if (event.getClassId() == TE2_Confirm_Order_Pending_SL::CLASS_ID) {
			TE2_Confirm_Order_Pending_SL const& event_i = (TE2_Confirm_Order_Pending_SL const&)(event);
			sl_brokerOrderId_m = event_i.brokerOrderId_m;
			our = true;
			
		}
		else
			return true;

		if (our) {
			TS2_SL_Pending const& main_state = machine().state<TS2_SL_Pending>();

			
			MyBrokerContext c(*this);

			// note that direction is MAIN direction
			if (ready_start_state.direction_m == BrkLib::TD_LONG) {
				// need to issue close order
				double limitp = main_state.openPrice_m + base.getTradeParam().tp_relativeLevel_m;
				LOG_SYMBOL_SIGNAL( base.getRunName(), base.getRtdataProvider(), context().getMachineSymbol(), 
					"Issue close long limit (TP) order with price: " << limitp <<
					", delayed: " << CppUtils::bool2String(base.getTradeParam().onlyMarketOrders_m)	<<
					", position ID: " << main_state.brokerOpenPositionId_m
					);

				// TP depending of what we have - delayed order or real order
				if (!base.getTradeParam().onlyMarketOrders_m)
					base.issueCloseLongOrder_Limit(c,limitp, context().getMachineSymbol(), base.getTradeParam().tradeVolume_m, main_state.brokerOpenPositionId_m);
				else
					base.issueCloseLongOrder_Delayed_TP(c,limitp, context().getMachineSymbol(), base.getTradeParam().tradeVolume_m, main_state.brokerOpenPositionId_m);
			}
			else if (ready_start_state.direction_m == BrkLib::TD_SHORT) {
				double limitp = main_state.openPrice_m - base.getTradeParam().tp_relativeLevel_m;
				LOG_SYMBOL_SIGNAL( base.getRunName(), base.getRtdataProvider(), context().getMachineSymbol(), 
					"Issue close short limit (TP) order with price: " << limitp <<
					", delayed: " << CppUtils::bool2String(base.getTradeParam().onlyMarketOrders_m) <<
					", position ID: " << main_state.brokerOpenPositionId_m
				);

				if (!base.getTradeParam().onlyMarketOrders_m)
					base.issueCloseShortOrder_Limit(c,limitp, context().getMachineSymbol(), base.getTradeParam().tradeVolume_m, main_state.brokerOpenPositionId_m);
				else
					base.issueCloseShortOrder_Delayed_TP(c,limitp, context().getMachineSymbol(), base.getTradeParam().tradeVolume_m, main_state.brokerOpenPositionId_m);
			}
																  
		}

		return true;

	}


	


	bool TS2_Ready::entry_action( CppUtils::BaseEvent const& event)
	{
		CAlgMisha& base = context().getBase();

		// delegate to main machine
		TS2_Ready_Start const& ready_start_state =  machine().state<TS2_Ready_Start>();

		if (event.getClassId() == TE2_Confirm_Order_Issued_TP::CLASS_ID) {
			 TE2_Confirm_Order_Issued_TP const& event_i = (TE2_Confirm_Order_Issued_TP const&)(event);

			 tp_brokerOrderId_m = event_i.brokerOrderId_m; 

			 if (ready_start_state.direction_m == BrkLib::TD_LONG) {
					TE_Confirm_Orders_Setup_Long event_t;
					base.getTradingMachines().processEvent<TE_Confirm_Orders_Setup_Long>(context().getMachineSymbol(),event_t );

			 }
			 else if (ready_start_state.direction_m == BrkLib::TD_SHORT) {
				 	TE_Confirm_Orders_Setup_Short event_t;
					base.getTradingMachines().processEvent<TE_Confirm_Orders_Setup_Short>(context().getMachineSymbol(),event_t );
			 }
		}
		
		
		return true;
	}



	bool TS2_Need_Clear_SL::entry_action( CppUtils::BaseEvent const& event)
	{

		CAlgMisha& base = context().getBase();

		if (event.getClassId() == TE2_Confirm_Order_Executed_TP::CLASS_ID) {
			// here need to cancel all other orders
								 
			TS2_In_Position const& main_state = machine().state<TS2_In_Position>();
			TS2_Ready const& main_state_TS2_Ready = machine().state<TS2_Ready>();
			TS2_SL_Pending const& main_state_TS2_SL_Pending = machine().state<TS2_SL_Pending>();

			LOG_SYMBOL_SIGNAL(base.getRunName(), base.getRtdataProvider(), context().getMachineSymbol(), 
					"Issue cancel SL" <<
					", delayed: " << CppUtils::bool2String(base.getTradeParam().onlyMarketOrders_m) << 
					", position ID: " << main_state_TS2_SL_Pending.brokerOpenPositionId_m <<
					", SL order ID: " << main_state.sl_brokerOrderId_m
			);

			MyBrokerContext c(*this);									  
			// one must give a error another - OK
			if (!base.getTradeParam().onlyMarketOrders_m) {
				// real
				base.issueCancelOrder(c,context().getMachineSymbol(), main_state_TS2_SL_Pending.brokerOpenPositionId_m, main_state.sl_brokerOrderId_m);
			} else {
				// delayed
				base.cancelAllDelayedOrders();
			}
		}

		return true;
	}

	bool TS2_Need_Clear_TP::entry_action( CppUtils::BaseEvent const& event)
	{

		CAlgMisha& base = context().getBase();

		if (event.getClassId() == TE2_Confirm_Order_Executed_SL::CLASS_ID) {
			// here need to cancel all other orders
								 
			TS2_In_Position const& main_state = machine().state<TS2_In_Position>();
			TS2_Ready const& main_state_TS2_Ready = machine().state<TS2_Ready>();
			TS2_SL_Pending const& main_state_TS2_SL_Pending = machine().state<TS2_SL_Pending>();

			LOG_SYMBOL_SIGNAL(base.getRunName(), base.getRtdataProvider(), context().getMachineSymbol(), 
					"Issue cancell TP" <<
					", delayed: " << CppUtils::bool2String(base.getTradeParam().onlyMarketOrders_m) <<
					", position ID: " << main_state_TS2_SL_Pending.brokerOpenPositionId_m <<
					", TP order ID: " << main_state_TS2_Ready.tp_brokerOrderId_m
					);

			MyBrokerContext c(*this);
			// one must give a error another - OK
			if (!base.getTradeParam().onlyMarketOrders_m) {
				// real
				base.issueCancelOrder(c,context().getMachineSymbol(), main_state_TS2_SL_Pending.brokerOpenPositionId_m, main_state_TS2_Ready.tp_brokerOrderId_m);
			} else {
				// delayed
				base.cancelAllDelayedOrders();
			}
		}

		return true;
	}

	bool TS2_Trade_Finished::entry_action( CppUtils::BaseEvent const& event)
	{
			CAlgMisha& base = context().getBase();
			
			if (event.getClassId() == TE2_Confirm_Order_Cancelled_TP::CLASS_ID ||
				event.getClassId() == TE2_Confirm_Order_Cancelled_SL::CLASS_ID	 ||
				event.getClassId() == TE2_Confirm_Order_Executed_Market_Close::CLASS_ID
			) {																								 
				// need to delegate to upper machine
				TS2_Ready_Start const& main_state = machine().state<TS2_Ready_Start>();

				if (main_state.direction_m == BrkLib::TD_LONG) {
					TE_Clear_Long event_t;
					base.getTradingMachines().processEvent<TE_Clear_Long>(context().getMachineSymbol(),event_t );

				}
				else if (main_state.direction_m == BrkLib::TD_SHORT) {
		 			TE_Clear_Short event_t;
					base.getTradingMachines().processEvent<TE_Clear_Short>(context().getMachineSymbol(),event_t );
				}			
				
				
			}

			return true;
	}

	bool TS2_Inconsistent::entry_action( CppUtils::BaseEvent const& event)
	{
		CAlgMisha& base = context().getBase();
		
		// delegate to main machine
		// delegate to main machine
		int direction = -1;
		
		TS2_Ready_Start const& main_state = machine().state<TS2_Ready_Start>();

		if (main_state.direction_m == BrkLib::TD_LONG) {
			TE_Confirm_Orders_Setup_Error_Long event_t;
			base.getTradingMachines().processEvent<TE_Confirm_Orders_Setup_Error_Long>(context().getMachineSymbol(),event_t );
		}
		else if (main_state.direction_m == BrkLib::TD_SHORT) {
		 	TE_Confirm_Orders_Setup_Error_Short event_t;
			base.getTradingMachines().processEvent<TE_Confirm_Orders_Setup_Error_Short>(context().getMachineSymbol(),event_t );
		}			
		return true;
	};


	bool TS2_Check_SL_Error::entry_action( CppUtils::BaseEvent const& event)
	{

		static char const* pattern = "При выставлении приказов СТОП и СТОП-ЛИМИТ на покупку текущие котировки должны быть строго ниже цены СТОП";
		CAlgMisha& base = context().getBase();
		
		

		if (event.getClassId() == TE2_Confirm_Order_Error_SL::CLASS_ID)	{
			TE2_Confirm_Order_Error_SL const& event_i = (TE2_Confirm_Order_Error_SL const&)(event);

			if (event_i.reason.find(pattern) != CppUtils::String::npos) {
				// found

				// normal error
				TE2_Error_Invalid_Stop_Price_SL invalidsp_error_event;
				invalidsp_error_event.reason =event_i.reason;
				invalidsp_error_event.brokerOrderId_m = event_i.brokerOrderId_m;
			

				base.getOrderMachines().processEvent< TE2_Error_Invalid_Stop_Price_SL >(context().getMachineSymbol(),invalidsp_error_event);

			} else {
				// normal error
				TE2_Common_Error_SL common_error_event;
				common_error_event.reason =event_i.reason;
				common_error_event.brokerOrderId_m = event_i.brokerOrderId_m;

				base.getOrderMachines().processEvent< TE2_Common_Error_SL >(context().getMachineSymbol(),common_error_event);

			}
		}

		return true;
	}

	bool TS2_Close_By_Market::entry_action( CppUtils::BaseEvent const& event)
	{
		CAlgMisha& base = context().getBase();
		MyBrokerContext c(*this);

		TS2_Ready_Start const& ready_start_state =  machine().state<TS2_Ready_Start>();

		if (event.getClassId() == TE2_Error_Invalid_Stop_Price_SL::CLASS_ID)	{
			TE2_Error_Invalid_Stop_Price_SL const& event_i = ( TE2_Error_Invalid_Stop_Price_SL const&)(event);

			// if we work with market orders only this must not happen
			if (!base.getTradeParam().onlyMarketOrders_m)	{

				TS2_SL_Pending const& main_state = machine().state<TS2_SL_Pending>();

				LOG_SYMBOL_SIGNAL(base.getRunName(), base.getRtdataProvider(), context().getMachineSymbol(), 
					"Issue market close because stop close failed" <<
					", position ID: " << main_state.brokerOpenPositionId_m
				);

								
				// need to issue market close order
				if(ready_start_state.direction_m ==	 BrkLib::TD_LONG) {
					base.issueCloseLongOrder(c,	context().getMachineSymbol(),  base.getTradeParam().tradeVolume_m, main_state.brokerOpenPositionId_m ); 
				}
				else if (ready_start_state.direction_m ==	 BrkLib::TD_SHORT) {
					base.issueCloseShortOrder(c,	context().getMachineSymbol(),  base.getTradeParam().tradeVolume_m, main_state.brokerOpenPositionId_m ); 
				}
			}
			else {
				// inconsistent
				LOG_SYMBOL_SIGNAL(base.getRunName(), base.getRtdataProvider(), context().getMachineSymbol(), 
					"Can't happen here as we are in delayed mode"
				);

				TE2_Confirm_Order_Error_Market_Close mclose_error_event;
				mclose_error_event.reason_m = "Cannot happen as we use delayed orders";

				base.getOrderMachines().processEvent< TE2_Confirm_Order_Error_Market_Close >(context().getMachineSymbol(),mclose_error_event);
			}


		}

		return true;
	}


}; // end of namespace

