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

	bool A_Issue_Limit::TE_Level_Detected_EnterHandler(TE_Level_Detected const& e)
	{
		MyBrokerContext context(*this);

		double limitPrice = -1;
		getContext().getRobot().getGlobalTradeContext().get( getContext().getMachineSymbol() ).tradeDirection_m = e.direction_m;
		if (e.direction_m == BrkLib::TD_LONG) {
			limitPrice = e.detectedPriceLevel_m - 1;
			getContext().getRobot().getGlobalTradeContext().get(getContext().getMachineSymbol()).levelDetectedPrice_m = e.detectedPriceLevel_m;

			getContext().getRobot().resolveBrokerDialog().issueBuyOrder_Limit(	
				"",
				context, 
				limitPrice, 
				getContext().getMachineSymbol(),
				getContext().getRobot().getTradeParam().tradeVolume_m
			);
		}
		else if (e.direction_m == BrkLib::TD_SHORT) {
			limitPrice = e.detectedPriceLevel_m + 1;
			getContext().getRobot().getGlobalTradeContext().get(getContext().getMachineSymbol()).levelDetectedPrice_m = e.detectedPriceLevel_m;

			getContext().getRobot().resolveBrokerDialog().issueSellOrder_Limit(	
				"",
				context, 
				limitPrice, 
				getContext().getMachineSymbol(),
				getContext().getRobot().getTradeParam().tradeVolume_m
			);
		}
				

		return true;
 	};

	void A_Issue_Limit::onCallbackReceived(
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
	)	
	{
		if (bresponse.type_m == BrkLib::RT_BrokerResponseOrder) {
		
			BrkLib::BrokerResponseOrder const& bRespOrder = (BrkLib::BrokerResponseOrder const&)bresponse;

			if (bRespOrder.resultCode_m ==  BrkLib::PT_ORDER_ESTABLISHED)	{
				if (bRespOrder.resultDetailCode_m & BrkLib::ODR_ORDER_INSTALLED) {
					
					// store globals
					getContext().getRobot().getGlobalTradeContext().get(bRespOrder.symbol_m).limitOrderPrice_m = bRespOrder.opPrice_m;
					getContext().getRobot().getGlobalTradeContext().get(bRespOrder.symbol_m).brokerLimitOrderId_m = bRespOrder.brokerOrderID_m;
					getContext().getRobot().getGlobalTradeContext().get(bRespOrder.symbol_m).brokerPositionId_m = bRespOrder.brokerPositionID_m;

					TE_Limit_Pending e;
					getContext().getRobot().getTradingMachines().processEvent<TE_Limit_Pending>(bRespOrder.symbol_m, e);
				}
				else if (bRespOrder.resultDetailCode_m & BrkLib::ODR_ORDER_EXECUTED) {
					TE_Limit_Executed e;
					getContext().getRobot().getTradingMachines().processEvent<TE_Limit_Executed>(bRespOrder.symbol_m, e);
				}

				//

			}
			else if (bRespOrder.resultCode_m==BrkLib::PT_GENERAL_ERROR_HAPPENED) {
				getContext().getRobot().getGlobalTradeContext().get(getContext().getMachineSymbol()).errorReason_m = bRespOrder.brokerComment_m;

				TE_Broker_Error e;
				getContext().getRobot().getTradingMachines().processEvent<TE_Broker_Error>(bRespOrder.symbol_m, e);

			}
		}
	}

	// ----------------------------------------------------

	bool A_Mismatched_Execution_Enter::TE_Stop_Executed_EnterHandler(TE_Stop_Executed const& e)
	{
		TE_Unconditional ev;
		getContext().getRobot().getTradingMachines().processEvent<TE_Unconditional>(getContext().getMachineSymbol(), ev);

		return true;
	}

	bool A_Mismatched_Execution_Enter::TE_Limit_TP_Executed_EnterHandler(TE_Limit_TP_Executed const& e)
	{
		TE_Unconditional ev;
		getContext().getRobot().getTradingMachines().processEvent<TE_Unconditional>(getContext().getMachineSymbol(), ev);

		return true;
	}

	// -----------------------------------------------------

	bool AE_Mismatched_Execution::TE_Unconditional_ExitHandler(TE_Unconditional const& e)
	{
		return true;
	}

	// -----------------------------------------------------

	bool A_Issue_Stop::TE_Limit_Pending_EnterHandler(TE_Limit_Pending const& e)
	{
		MyBrokerContext context(*this);

		double stopPrice = -1;
		
		if (getContext().getRobot().getGlobalTradeContext().get(getContext().getMachineSymbol()).tradeDirection_m == BrkLib::TD_LONG) {
			stopPrice = getContext().getRobot().getGlobalTradeContext().get(getContext().getMachineSymbol()).levelDetectedPrice_m + 1;
			getContext().getRobot().resolveBrokerDialog().issueSellOrder_Stop(	
				"",
				context, 
				stopPrice, 
				getContext().getMachineSymbol(),
				getContext().getRobot().getTradeParam().tradeVolume_m
			);
		}
		else if (getContext().getRobot().getGlobalTradeContext().get(getContext().getMachineSymbol()).tradeDirection_m == BrkLib::TD_SHORT) {
			stopPrice = getContext().getRobot().getGlobalTradeContext().get(getContext().getMachineSymbol()).levelDetectedPrice_m - 1;
			getContext().getRobot().resolveBrokerDialog().issueBuyOrder_Stop(	
				"",
				context, 
				stopPrice, 
				getContext().getMachineSymbol(),
				getContext().getRobot().getTradeParam().tradeVolume_m
			);
		}
	
		

		return true;
 	};

	void A_Issue_Stop::onCallbackReceived(
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
	)	
	{
		if (bresponse.type_m == BrkLib::RT_BrokerResponseOrder) {
		
			BrkLib::BrokerResponseOrder const& bRespOrder = (BrkLib::BrokerResponseOrder const&)bresponse;

			if (bRespOrder.resultCode_m ==  BrkLib::PT_ORDER_ESTABLISHED)	{
				if (bRespOrder.resultDetailCode_m & BrkLib::ODR_ORDER_PENDING) {
					
					
					getContext().getRobot().getGlobalTradeContext().get(bRespOrder.symbol_m).stopOrderPrice_m = bRespOrder.opStopPrice_m;
					getContext().getRobot().getGlobalTradeContext().get(bRespOrder.symbol_m).brokerStopOrderId_m = bRespOrder.brokerOrderID_m; 

					TE_Stop_Pending e;
					getContext().getRobot().getTradingMachines().processEvent<TE_Stop_Pending>(bRespOrder.symbol_m, e);
				}
				else if (bRespOrder.resultDetailCode_m & BrkLib::ODR_ORDER_EXECUTED) {
					TE_Stop_Executed e;
					getContext().getRobot().getTradingMachines().processEvent<TE_Stop_Executed>(bRespOrder.symbol_m, e);
				}
			}
			else if (bRespOrder.resultCode_m==BrkLib::PT_GENERAL_ERROR_HAPPENED) {
				getContext().getRobot().getGlobalTradeContext().get(getContext().getMachineSymbol()).errorReason_m = bRespOrder.brokerComment_m;
				getContext().getRobot().getGlobalTradeContext().get(bRespOrder.symbol_m).stopOrderPrice_m = bRespOrder.opStopPrice_m;

				TE_Broker_Error e;
				getContext().getRobot().getTradingMachines().processEvent<TE_Broker_Error>(bRespOrder.symbol_m, e);

			}
		}
	}

	// -----------------------------------------------------

	bool A_Issue_Limit_TP::TE_Limit_Executed_EnterHandler(TE_Limit_Executed const& e)
	{
		MyBrokerContext context(*this);

		double limitPriceTP = -1;
		
		
		if (getContext().getRobot().getGlobalTradeContext().get(getContext().getMachineSymbol()).tradeDirection_m == BrkLib::TD_LONG) {
			limitPriceTP = getContext().getRobot().getGlobalTradeContext().get(getContext().getMachineSymbol()).levelDetectedPrice_m + getContext().getRobot().getTradeParam().tp_relativeLevel_m;
			getContext().getRobot().resolveBrokerDialog().issueBuyOrder_Limit(	
				"",
				context, 
				limitPriceTP, 
				getContext().getMachineSymbol(),
				getContext().getRobot().getTradeParam().tradeVolume_m
			);
		}
		else if (getContext().getRobot().getGlobalTradeContext().get(getContext().getMachineSymbol()).tradeDirection_m == BrkLib::TD_SHORT) {
			limitPriceTP = getContext().getRobot().getGlobalTradeContext().get(getContext().getMachineSymbol()).levelDetectedPrice_m - getContext().getRobot().getTradeParam().tp_relativeLevel_m;
			getContext().getRobot().resolveBrokerDialog().issueSellOrder_Limit(	
				"",
				context, 
				limitPriceTP, 
				getContext().getMachineSymbol(),
				getContext().getRobot().getTradeParam().tradeVolume_m
			);
		}
				

		return true;
 	};

	void A_Issue_Limit_TP::onCallbackReceived(
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
	)	
	{
		if (bresponse.type_m == BrkLib::RT_BrokerResponseOrder) {
		
			BrkLib::BrokerResponseOrder const& bRespOrder = (BrkLib::BrokerResponseOrder const&)bresponse;

			if (bRespOrder.resultCode_m ==  BrkLib::PT_ORDER_ESTABLISHED)	{
				if (bRespOrder.resultDetailCode_m & BrkLib::ODR_ORDER_INSTALLED) {
					
					
					getContext().getRobot().getGlobalTradeContext().get(bRespOrder.symbol_m).limitTPOrderPrice_m = bRespOrder.opPrice_m;
					getContext().getRobot().getGlobalTradeContext().get(bRespOrder.symbol_m).brokerLimitTPOrderId_m = bRespOrder.brokerOrderID_m;

					TE_Limit_TP_Pending e;
					getContext().getRobot().getTradingMachines().processEvent<TE_Limit_TP_Pending>(bRespOrder.symbol_m, e);
				}
				else if (bRespOrder.resultDetailCode_m & BrkLib::ODR_ORDER_EXECUTED) {
					TE_Stop_Executed e;
					getContext().getRobot().getTradingMachines().processEvent<TE_Stop_Executed>(bRespOrder.symbol_m, e);
				}
			}
			else if (bRespOrder.resultCode_m==BrkLib::PT_GENERAL_ERROR_HAPPENED) {
				getContext().getRobot().getGlobalTradeContext().get(getContext().getMachineSymbol()).errorReason_m = bRespOrder.brokerComment_m;

				TE_Broker_Error e;
				getContext().getRobot().getTradingMachines().processEvent<TE_Broker_Error>(bRespOrder.symbol_m, e);

			}
		}
	}

	// ----------------------------------------------------

	bool A_Cancel_Limit::TE_Stop_Cancelled_EnterHandler(TE_Stop_Cancelled const& e)
	{
		MyBrokerContext context(*this);
		getContext().getRobot().resolveBrokerDialog().issueCancelOrder(
			"",
			context,
			getContext().getMachineSymbol(),
			getContext().getRobot().getGlobalTradeContext().get(getContext().getMachineSymbol()).brokerLimitOrderId_m
		);


		return true;
 	};

	bool A_Cancel_Limit::TE_Level_Lost_EnterHandler(TE_Level_Lost const& e)
	{
		MyBrokerContext context(*this);
		getContext().getRobot().resolveBrokerDialog().issueCancelOrder(
			"",
			context,
			getContext().getMachineSymbol(),
			getContext().getRobot().getGlobalTradeContext().get(getContext().getMachineSymbol()).brokerLimitOrderId_m
		);

		return true;
 	};

	void A_Cancel_Limit::onCallbackReceived(
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
	)
	{
			
		if (bresponse.type_m == BrkLib::RT_BrokerResponseOrder) {
		
			BrkLib::BrokerResponseOrder const& bRespOrder = (BrkLib::BrokerResponseOrder const&)bresponse;

			if (bRespOrder.resultCode_m == BrkLib::PT_ORDER_CANCELLED) {
				if (bRespOrder.resultDetailCode_m & BrkLib::ODR_OK) {

					// reset
					getContext().getRobot().getGlobalTradeContext().get(getContext().getMachineSymbol()).brokerLimitOrderId_m = "";

					TE_Limit_Cancelled e;
					getContext().getRobot().getTradingMachines().processEvent<TE_Limit_Cancelled>(bRespOrder.symbol_m, e);
				}
			}
			else if (bRespOrder.resultCode_m==BrkLib::PT_GENERAL_ERROR_HAPPENED) {
				getContext().getRobot().getGlobalTradeContext().get(getContext().getMachineSymbol()).errorReason_m = bRespOrder.brokerComment_m;

				TE_Broker_Error e;
				getContext().getRobot().getTradingMachines().processEvent<TE_Broker_Error>(bRespOrder.symbol_m, e);

			}
		}
	}

	// -------------------------------------------------------------

	bool A_Cancel_Stop::TE_Level_Lost_EnterHandler(TE_Level_Lost const& e)
	{
		MyBrokerContext context(*this);
		getContext().getRobot().resolveBrokerDialog().issueCancelOrder(
			"",
			context,
			getContext().getMachineSymbol(),
			getContext().getRobot().getGlobalTradeContext().get(getContext().getMachineSymbol()).brokerStopOrderId_m
		);

		return true;
	}

	void A_Cancel_Stop::onCallbackReceived(
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
	)
	{
		if (bresponse.type_m == BrkLib::RT_BrokerResponseOrder) {
		
			BrkLib::BrokerResponseOrder const& bRespOrder = (BrkLib::BrokerResponseOrder const&)bresponse;

			if (bRespOrder.resultCode_m == BrkLib::PT_ORDER_CANCELLED) {
				if (bRespOrder.resultDetailCode_m & BrkLib::ODR_OK) {

					// reset
					getContext().getRobot().getGlobalTradeContext().get(getContext().getMachineSymbol()).brokerStopOrderId_m = "";

					TE_Stop_Cancelled e;
					getContext().getRobot().getTradingMachines().processEvent<TE_Stop_Cancelled>(bRespOrder.symbol_m, e);
				}
			}
			else if (bRespOrder.resultCode_m==BrkLib::PT_GENERAL_ERROR_HAPPENED) {
				getContext().getRobot().getGlobalTradeContext().get(getContext().getMachineSymbol()).errorReason_m = bRespOrder.brokerComment_m;

				TE_Broker_Error e;
				getContext().getRobot().getTradingMachines().processEvent<TE_Broker_Error>(bRespOrder.symbol_m, e);

			}
		}
	}


	// -----------------------------------------------------
	bool A_Initial_Enter::TE_Stop_Cancelled_TP_Exit_EnterHandler(TE_Stop_Cancelled_TP_Exit const& e)
	{
		TE_Unconditional ev;
		getContext().getRobot().getTradingMachines().processEvent<TE_Unconditional>(getContext().getMachineSymbol(), ev);

		return true;
	}

	bool A_Initial_Enter::TE_Limit_Cancelled_EnterHandler(TE_Limit_Cancelled const& e)
	{
		TE_Unconditional ev;
		getContext().getRobot().getTradingMachines().processEvent<TE_Unconditional>(getContext().getMachineSymbol(), ev);

		return true;
	}

	bool A_Initial_Enter::TE_Limit_TP_Cancelled_EnterHandler(TE_Limit_TP_Cancelled const& e)
	{
		TE_Unconditional ev;
		getContext().getRobot().getTradingMachines().processEvent<TE_Unconditional>(getContext().getMachineSymbol(), ev);

		return true;
	}

	bool A_Initial_Enter::executeDefaultHandler(CppUtils::BaseEvent const& eventToProcess)
	{
		TE_Unconditional ev;
		getContext().getRobot().getTradingMachines().processEvent<TE_Unconditional>(getContext().getMachineSymbol(), ev);
		return true;
	}

	// -----------------------------------------------------
	bool A_Cancel_Limit_TP::TE_Stop_Executed_EnterHandler(TE_Stop_Executed const& e)
	{
		MyBrokerContext context(*this);
		getContext().getRobot().resolveBrokerDialog().issueCancelOrder(
			"",
			context,
			getContext().getMachineSymbol(),
			getContext().getRobot().getGlobalTradeContext().get(getContext().getMachineSymbol()).brokerLimitTPOrderId_m
		);

		return true;
	}
	
	bool A_Cancel_Limit_TP::TE_Stop_Cancelled_Market_Exit_EnterHandler(TE_Stop_Cancelled_Market_Exit const& e)
	{
		MyBrokerContext context(*this);
		getContext().getRobot().resolveBrokerDialog().issueCancelOrder(
			"",
			context,
			getContext().getMachineSymbol(),
			getContext().getRobot().getGlobalTradeContext().get(getContext().getMachineSymbol()).brokerLimitTPOrderId_m
		);

		return true;
	}
	
	void A_Cancel_Limit_TP::onCallbackReceived(
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
	)
	{
		if (bresponse.type_m == BrkLib::RT_BrokerResponseOrder) {
		
			BrkLib::BrokerResponseOrder const& bRespOrder = (BrkLib::BrokerResponseOrder const&)bresponse;

			if (bRespOrder.resultCode_m == BrkLib::PT_ORDER_CANCELLED) {
				if (bRespOrder.resultDetailCode_m & BrkLib::ODR_OK) {

					// reset
					getContext().getRobot().getGlobalTradeContext().get(getContext().getMachineSymbol()).brokerLimitTPOrderId_m = "";

					TE_Limit_TP_Cancelled e;
					getContext().getRobot().getTradingMachines().processEvent<TE_Limit_TP_Cancelled>(bRespOrder.symbol_m, e);
				}
			}
			else if (bRespOrder.resultCode_m==BrkLib::PT_GENERAL_ERROR_HAPPENED) {
				getContext().getRobot().getGlobalTradeContext().get(getContext().getMachineSymbol()).errorReason_m = bRespOrder.brokerComment_m;

				TE_Broker_Error e;
				getContext().getRobot().getTradingMachines().processEvent<TE_Broker_Error>(bRespOrder.symbol_m, e);

			}
		}
	}

	// -----------------------------------------------------

	bool A_Issue_Market_Close::TE_Level_Lost_EnterHandler(TE_Level_Lost const& e)
	{
		MyBrokerContext context(*this);
		
		if (getContext().getRobot().getGlobalTradeContext().get(getContext().getMachineSymbol()).tradeDirection_m == BrkLib::TD_LONG) {
			
			getContext().getRobot().resolveBrokerDialog().issueSellOrder(	
				"",
				context, 
				getContext().getMachineSymbol(),
				getContext().getRobot().getTradeParam().tradeVolume_m
			);
		}
		else if (getContext().getRobot().getGlobalTradeContext().get(getContext().getMachineSymbol()).tradeDirection_m == BrkLib::TD_SHORT) {
			
			getContext().getRobot().resolveBrokerDialog().issueBuyOrder(
				"",
				context, 
				getContext().getMachineSymbol(),
				getContext().getRobot().getTradeParam().tradeVolume_m
			);
		}

		return true;
	}
	
	void A_Issue_Market_Close::onCallbackReceived(
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
	)
	{
		if (bresponse.type_m == BrkLib::RT_BrokerResponseOrder) {
			BrkLib::BrokerResponseOrder const& bRespOrder = (BrkLib::BrokerResponseOrder const&)bresponse;
			if (bRespOrder.resultCode_m ==  BrkLib::PT_ORDER_ESTABLISHED)	{
					
					if (bRespOrder.resultDetailCode_m & BrkLib::ODR_ORDER_INSTALLED) {
						// installed
						TE_Market_Close_Pending e;
						getContext().getRobot().getTradingMachines().processEvent<TE_Market_Close_Pending>(bRespOrder.symbol_m, e);
					}
					else if (bRespOrder.resultDetailCode_m & BrkLib::ODR_ORDER_EXECUTED) {
						// executed
						TE_Market_Close_Executed e;
						getContext().getRobot().getTradingMachines().processEvent<TE_Market_Close_Executed>(bRespOrder.symbol_m, e);

					}
			}
			else if (bRespOrder.resultCode_m==BrkLib::PT_GENERAL_ERROR_HAPPENED) {
					getContext().getRobot().getGlobalTradeContext().get(getContext().getMachineSymbol()).errorReason_m = bRespOrder.brokerComment_m;

					TE_Broker_Error e;
					getContext().getRobot().getTradingMachines().processEvent<TE_Broker_Error>(bRespOrder.symbol_m, e);

			}
		}
	}

	// -----------------------------------------------------
	bool A_Cancel_Stop_Market_Exit::TE_Market_Close_Executed_EnterHandler(TE_Market_Close_Executed const& e)
	{
		// cancel stop
		MyBrokerContext context(*this);
		getContext().getRobot().resolveBrokerDialog().issueCancelOrder(
			"",
			context,
			getContext().getMachineSymbol(),
			getContext().getRobot().getGlobalTradeContext().get(getContext().getMachineSymbol()).brokerStopOrderId_m
		);

		return true;
	}
	
	void A_Cancel_Stop_Market_Exit::onCallbackReceived(
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
	)
	{
		if (bresponse.type_m == BrkLib::RT_BrokerResponseOrder) {
		
			BrkLib::BrokerResponseOrder const& bRespOrder = (BrkLib::BrokerResponseOrder const&)bresponse;

			if (bRespOrder.resultCode_m == BrkLib::PT_ORDER_CANCELLED) {
				if (bRespOrder.resultDetailCode_m & BrkLib::ODR_OK) {

					// reset
					getContext().getRobot().getGlobalTradeContext().get(getContext().getMachineSymbol()).brokerStopOrderId_m = "";

					TE_Stop_Cancelled_Market_Exit e;
					getContext().getRobot().getTradingMachines().processEvent<TE_Stop_Cancelled_Market_Exit>(bRespOrder.symbol_m, e);
				}
			}
			else if (bRespOrder.resultCode_m==BrkLib::PT_GENERAL_ERROR_HAPPENED) {
				getContext().getRobot().getGlobalTradeContext().get(getContext().getMachineSymbol()).errorReason_m = bRespOrder.brokerComment_m;

					TE_Broker_Error e;
					getContext().getRobot().getTradingMachines().processEvent<TE_Broker_Error>(bRespOrder.symbol_m, e);

			}
		}
	}

	// -----------------------------------------------------

	bool A_Cancel_Stop_TP_Exit::TE_Limit_TP_Executed_EnterHandler(TE_Limit_TP_Executed const& e)
	{
		MyBrokerContext context(*this);
		getContext().getRobot().resolveBrokerDialog().issueCancelOrder(
			"",
			context,
			getContext().getMachineSymbol(),
			getContext().getRobot().getGlobalTradeContext().get(getContext().getMachineSymbol()).brokerStopOrderId_m
		);

		return true;
	}
	
	void A_Cancel_Stop_TP_Exit::onCallbackReceived(
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
	)
	{
		if (bresponse.type_m == BrkLib::RT_BrokerResponseOrder) {
		
			BrkLib::BrokerResponseOrder const& bRespOrder = (BrkLib::BrokerResponseOrder const&)bresponse;

			if (bRespOrder.resultCode_m == BrkLib::PT_ORDER_CANCELLED) {
				if (bRespOrder.resultDetailCode_m & BrkLib::ODR_OK) {

					// reset
					getContext().getRobot().getGlobalTradeContext().get(getContext().getMachineSymbol()).brokerStopOrderId_m = "";

					TE_Stop_Cancelled e;
					getContext().getRobot().getTradingMachines().processEvent<TE_Stop_Cancelled>(bRespOrder.symbol_m, e);
				}
			}
			else if (bRespOrder.resultCode_m==BrkLib::PT_GENERAL_ERROR_HAPPENED) {
				getContext().getRobot().getGlobalTradeContext().get(getContext().getMachineSymbol()).errorReason_m = bRespOrder.brokerComment_m;

					TE_Broker_Error e;
					getContext().getRobot().getTradingMachines().processEvent<TE_Broker_Error>(bRespOrder.symbol_m, e);

			}
		}
	}

	// -----------------------------------------------------

	

}; // end of ns