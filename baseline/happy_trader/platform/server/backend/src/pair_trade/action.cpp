#include "action.hpp"
#include "action.hpp"
#include "implement.hpp"

namespace AlgLib {

	void BrokerResponseRecipient::onUnknownCallbackreceived(
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
	)
	{
		// here we go with all unknown stuff
	}

	// ------------------------------------

	bool AE_Init_Context::TE_Confirm_Open_Short_ExitHandler(TE_Confirm_Open_Short const& e)
	{
		clear();

		return true;
	}
	
	bool AE_Init_Context::TE_Confirm_Open_Nothing_ExitHandler(TE_Confirm_Open_Nothing const& e)
	{
		clear();

		return true;
	}

	bool AE_Init_Context::TE_Confirm_Open_Long_ExitHandler(TE_Confirm_Open_Long const& e)
	{
		clear();

		return true;
	}

	void AE_Init_Context::clear()
	{
		getContext().getRobot().getGlobalTradeContext().get().nextPairTradeParam_m.get().clear();
		getContext().getRobot().getGlobalTradeContext().get().ourTradeDirection_m = BrkLib::TD_NONE;
	}


	// ------------------------------------
	
	bool A_Completed::TE_Executed_All_EnterHandler(TE_Executed_All const& e)
	{
		// we finished with our deal
		// need to go to the enxt step


		// reset timeout after completion
		getContext().getRobot().getGlobalTradeContext().get().state_Life_Time_TS_Ready_Open = -1;

		if (getContext().getRobot().getGlobalTradeContext().get().ourTradeDirection_m == BrkLib::TD_SHORT) {
			TE_Completed_Short	e;
			getContext().getRobot().getTradingMachine().processEvent<TE_Completed_Short>(e);
		}
		
		if (getContext().getRobot().getGlobalTradeContext().get().ourTradeDirection_m == BrkLib::TD_LONG) {
			TE_Completed_Long	e;
			getContext().getRobot().getTradingMachine().processEvent<TE_Completed_Long>(e);
		}																						   
		return true;
	}

	// ------------------------------------------

	bool AT_Check_All_Executed::execute(CppUtils::BaseEvent const& eventToProcess)
	{

		bool passed = true;
		int paramSize = machine().context().getBase().getRobot().getGlobalTradeContext().get().nextPairTradeParam_m.get().size();
		for(int i = 0; i < paramSize; i++) {
			MultuSymbolRequestEntry const& entry_i = machine().context().getBase().getRobot().getGlobalTradeContext().get().nextPairTradeParam_m.get()[ i ];

			if (entry_i.volumeToFill_m !=0) {
				passed = false;
				break;
			}
		};
	
		if (	passed	)		{

			//LOG_SYMBOL(machine().context().getBase().getRunName(), machine().context().getBase().getBarDataProvider_Smartcom(), "", 
			//	machine().context().getContext() << machine().context().getBase().getRobot().getGlobalTradeContext().get().toDescriptionString()
			//);

			TE_Executed_All e;
			machine().context().getBase().getRobot().getTradingMachine().processEvent<TE_Executed_All>(e);

		}



		return true;
	}
	// ------------------------------------------

	// ------------------------------------

	void A_Issue_Pair_Open::openPos()
	{
		MyBrokerContext context(*this);

		int paramSize = getContext().getRobot().getGlobalTradeContext().get().nextPairTradeParam_m.get().size();

		
		// reset time of position life
		
		getContext().getRobot().getGlobalTradeContext().get().state_Life_Time_TS_Ready_Open = getContext().getRobot().getTradeParam().brokerTimeoutSec_m;
		
		

		// issue all orders
		for(int i = 0; i < paramSize; i++) {
			MultuSymbolRequestEntry& entry_i = getContext().getRobot().getGlobalTradeContext().get().nextPairTradeParam_m.get()[ i ];

			// set up volume to fill for that
			entry_i.volumeToFill_m = entry_i.volume_m;

			LOG_SYMBOL(getContext().getBase().getRunName(), getContext().getBase().getBarDataProvider_Smartcom(), "", 
				getContext().getContext()  << "Issue open order for symbol: " << entry_i.symbol_m << " volume to fill is: " << entry_i.volumeToFill_m
			);
			
			LOG(MSG_INFO, "foo", "Sending order for symbol: " << entry_i.symbol_m << " operation: " << "type: "<< BrkLib::tradeDirection2String(entry_i.direction_m) << " volume: " << entry_i.volume_m);

			if (entry_i.direction_m == BrkLib::TD_LONG) {
				getContext().getRobot().resolveBrokerDialog().issueBuyOrder( getContext().getRobot().getTradeParam().tradePrortfolio_m,context, entry_i.symbol_m, entry_i.volume_m );
			}
			else if (entry_i.direction_m == BrkLib::TD_SHORT) {
				getContext().getRobot().resolveBrokerDialog().issueSellOrder( getContext().getRobot().getTradeParam().tradePrortfolio_m,context, entry_i.symbol_m, entry_i.volume_m );
			}
		}	
	
	}


	bool A_Issue_Pair_Open::TE_Signal_Short_EnterHandler(TE_Signal_Short const& e)
	{
		
		openPos();
		return true;
	}
	
	bool A_Issue_Pair_Open::TE_Signal_Long_EnterHandler(TE_Signal_Long const& e)
	{
		openPos();
		return true;
	}

	// broker response handler
	void A_Issue_Pair_Open::onCallbackReceived(
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
	)
	{
		if (bresponse.type_m == BrkLib::RT_BrokerResponseOrder) {
			BrkLib::BrokerResponseOrder const& bRespOrder = (BrkLib::BrokerResponseOrder const&)bresponse;
		
			if (bRespOrder.resultDetailCode_m & BrkLib::ODR_ORDER_EXECUTED && bRespOrder.resultCode_m == BrkLib::PT_ORDER_ESTABLISHED) {
				// increase the number of execute positiosn
				//getContext().getRobot().getGlobalTradeContext().get().currentlyOpenPositions_m++;

				int paramSize = getContext().getRobot().getGlobalTradeContext().get().nextPairTradeParam_m.get().size();
				for(int i = 0; i < paramSize; i++) {
					MultuSymbolRequestEntry& entry_i = getContext().getRobot().getGlobalTradeContext().get().nextPairTradeParam_m.get()[ i ];

					// entry_i.volumeToFill_m > 0 !!! as we must expect the call back
					if (entry_i.symbol_m == bRespOrder.symbol_m && entry_i.volumeToFill_m > 0) {
						entry_i.volumeToFill_m -= abs(bRespOrder.opVolume_m); 
						LOG(MSG_INFO, "foo", "Returning response symbol: " << entry_i.symbol_m << " operation: " << "type: "<< BrkLib::orderTypeBitwise2String(bRespOrder.opOrderType_m) << " volume: " << bRespOrder.opVolume_m << " remaing volume: " << entry_i.volumeToFill_m );

						LOG_SYMBOL(getContext().getBase().getRunName(), getContext().getBase().getBarDataProvider_Smartcom(), "", 
							getContext().getContext()  << "Confirmed execution for symbol: " << entry_i.symbol_m << " volume to fill: " << entry_i.volumeToFill_m
						);

					

						// event
						TE_Execute_Signal	e;
						getContext().getRobot().getTradingMachine().processEvent<TE_Execute_Signal>(e);
																    
						break;
					}

				}

				
				

			}
			else if (bRespOrder.resultCode_m==BrkLib::PT_GENERAL_ERROR_HAPPENED) {

			
															  
				TE_Broker_Error e;
				e.errorReason_m = bRespOrder.brokerComment_m;
				getContext().getRobot().getTradingMachine().processEvent<TE_Broker_Error>(e);
			}
		}
	}

	// ------------------------------------
	
	// -----------------------------------

	// -----------------------------------
	
	bool A_Notify_User_Error::TE_Broker_Error_EnterHandler(TE_Broker_Error const& e)
	{
		return true;
	}

	bool A_Notify_User_Error::TE_Stop_Trade_EnterHandler(TE_Stop_Trade const& e)
	{
		return true;
	}


	// ------------------------------------

	bool AE_TS_Open_Short_Exit::TE_Signal_Long_ExitHandler(TE_Signal_Long const& e)
	{
		
		// double volume
		
		//LOG(MSG_INFO, "foo", "init trading param");

		getContext().getRobot().getGlobalTradeContext().get().nextPairTradeParam_m = 
			getContext().getRobot().getLevelCalculator().getEntryListLongDoubleVolume();

		getContext().getRobot().getGlobalTradeContext().get().ourTradeDirection_m = BrkLib::TD_LONG;

		// reset timeout
		getContext().getRobot().getGlobalTradeContext().get().state_Life_Time_TS_Ready_Open = -1;

		return true;
	}

	bool AE_TS_Open_Short_Exit::TE_Stop_Trade_ExitHandler(TE_Stop_Trade const& e)
	{
		return true;
	}

	// ------------------------------------


	bool AE_TS_Open_Long_Exit::TE_Signal_Short_ExitHandler(TE_Signal_Short const& e)
	{
		//LOG(MSG_INFO, "foo", "init trading param");

		// double volume
		getContext().getRobot().getGlobalTradeContext().get().nextPairTradeParam_m = 
			getContext().getRobot().getLevelCalculator().getEntryListShortDoubleVolume();

		getContext().getRobot().getGlobalTradeContext().get().ourTradeDirection_m = BrkLib::TD_SHORT;

		// rset timout
		getContext().getRobot().getGlobalTradeContext().get().state_Life_Time_TS_Ready_Open = -1;

		return true;
	}
	
	bool AE_TS_Open_Long_Exit::TE_Stop_Trade_ExitHandler(TE_Stop_Trade const& e)
	{
		return true;
	}

	// ------------------------------------

	
	bool AE_TS_Open_Nothing_Exit::TE_Signal_Short_ExitHandler(TE_Signal_Short const& e)
	{
		//LOG(MSG_INFO, "foo", "init trading param");

		// signle volume
		getContext().getRobot().getGlobalTradeContext().get().nextPairTradeParam_m = 
			getContext().getRobot().getLevelCalculator().getEntryListShortSingleVolume();

		getContext().getRobot().getGlobalTradeContext().get().ourTradeDirection_m = BrkLib::TD_SHORT;

		// reset timeout 
		getContext().getRobot().getGlobalTradeContext().get().state_Life_Time_TS_Ready_Open = -1;

		return true;
	}

	bool AE_TS_Open_Nothing_Exit::TE_Signal_Long_ExitHandler(TE_Signal_Long const& e)
	{
		// signle volume
		
		//LOG(MSG_INFO, "foo", "init trading param");

		getContext().getRobot().getGlobalTradeContext().get().nextPairTradeParam_m = 
			getContext().getRobot().getLevelCalculator().getEntryListLongSingleVolume();

		getContext().getRobot().getGlobalTradeContext().get().ourTradeDirection_m = BrkLib::TD_LONG;

		// reset timeout
		getContext().getRobot().getGlobalTradeContext().get().state_Life_Time_TS_Ready_Open = -1;

		return true;
	}
	bool AE_TS_Open_Nothing_Exit::TE_Stop_Trade_ExitHandler(TE_Stop_Trade const& e)
	{
		return true;
	}

	// ------------------------------------

	bool AT_Store_Pending_Signal::execute(CppUtils::BaseEvent const& eventToProcess)
	{
		// store pending signals

		if (eventToProcess.getClassId() == TE_Signal_Short::CLASS_ID)	{
			// store
			//LOG(MSG_INFO, "foo", "storing short oper");
			// store
			machine().context().getBase().getRobot().getLevelCalculator().pushPendingSingal(BrkLib::TD_SHORT);
		}
		else if (eventToProcess.getClassId() == TE_Signal_Long::CLASS_ID)	{
			//LOG(MSG_INFO, "foo", "storing long oper");
			// store
			machine().context().getBase().getRobot().getLevelCalculator().pushPendingSingal(BrkLib::TD_LONG);

		}
		return true;
	}

	// ------------------------------------

	bool A_Check_Pending_Orders_TS_Open_Short::TE_Completed_Short_EnterHandler(TE_Completed_Short const& e)
	{
		
		//LOG(MSG_INFO, "foo", "sending pending!- 1");
		int sent = getContext().getBase().getRobot().getLevelCalculator().sendPendingSignals();
		//LOG(MSG_INFO, "foo", "1 - sending pending of size: " << sent);
		return true;
	}
	
	bool A_Check_Pending_Orders_TS_Open_Short::TE_Confirm_Open_Short_EnterHandler(TE_Confirm_Open_Short const& e)
	{
		return true;
	}

	// ------------------------------------

	bool A_Check_Pending_Orders_TS_Open_Long::TE_Completed_Long_EnterHandler(TE_Completed_Long const& e)
	{
		//LOG(MSG_INFO, "foo", "sending pending! - 2");
		int sent = getContext().getBase().getRobot().getLevelCalculator().sendPendingSignals();
		//LOG(MSG_INFO, "foo", "2 - sending pending of size: " << sent);
		return true;
	}
	
	bool A_Check_Pending_Orders_TS_Open_Long::TE_Confirm_Open_Long_EnterHandler(TE_Confirm_Open_Long const& e)
	{
		return true;
	}

	// ------------------------------------

	bool A_Check_Pending_Orders_TS_Open_Nothing::TE_Confirm_Open_Nothing_EnterHandler(TE_Confirm_Open_Nothing const& e)
	{
		return true;
	}

	// ------------------------------------


}; // end of ns