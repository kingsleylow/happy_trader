#include "operationprocessor.hpp"
#include "implement.hpp"
#include "rtbroker.hpp"

#define OPERATIONPROCESSOR "OPERATIONPROCESSOR"

namespace BrkLib {
	OperationProcessor::OperationProcessor( BrokerEngine& brokerEngine, OperationProcessorCallBack& callback):
		brokerEngine_m(brokerEngine),
		callback_m(callback)
	{
	}
	
	OperationProcessor::~OperationProcessor()
	{
	}

	void OperationProcessor::processOperation(OrderOperation& oper)
	{
		Price p = getBrokerEngine().resolveFastPriceData(oper.provider_m, oper.symbol_m );
		
		checkCommonOperationFields(oper);

		

		// delegate to helper
		processOperation(p, oper );

		
	}

	void OperationProcessor::processOperation(Price const& p, OrderOperation& oper)
	{
		bool register_oper = false;
		BRK_LOG(getBrokerEngine().getBroker().getName().c_str(), BRK_MSG_INFO, OPERATIONPROCESSOR, "processOperation(...) oper: " << oper.toString());

		switch(oper.type_m) {
			case OOT_Market_Buy:
				{
					double price = p.ask_m + oper.tinfo_m.slippage_m;
				
					OperationStructureInstall oi(oper.provider_m, oper.symbol_m, p.time_m, price, oper.volume_m, oper.brkSession_m, OOT_Market_Buy);
					callback_m.onInstallationProcedure(oi, oper);

				
					getBrokerEngine().getPositionHolder().goLong(oper.provider_m, oper.symbol_m, p.time_m,	price, oper.volume_m, oper.affectedPositionId_m, oper.brkSession_m, oper );
					
					
				}
				break;
			case OOT_Market_Sell:
				{
					double price = p.bid_m - oper.tinfo_m.slippage_m;
				
					OperationStructureInstall oi(oper.provider_m, oper.symbol_m, p.time_m, price, oper.volume_m,oper.brkSession_m, OOT_Market_Sell);
					callback_m.onInstallationProcedure(oi, oper);

				
					getBrokerEngine().getPositionHolder().goShort(oper.provider_m, oper.symbol_m, p.time_m,	price, oper.volume_m, oper.affectedPositionId_m,  oper.brkSession_m, oper );

				}
				break;
			case OOT_Cancel:
				cancelOrder(oper, p);
				break;
			case OOT_Move:
				moveOrder(oper, p);
				break;
			case OOT_Limit_Buy:
			case OOT_Limit_Sell:
				{
					checkLimitOperation(oper, p);
					register_oper = true;

					OperationStructureInstall oi(oper.provider_m, oper.symbol_m, p.time_m, oper.price_m, oper.volume_m, oper.brkSession_m, oper.type_m);
					callback_m.onInstallationProcedure(oi, oper);
				}
				break;
			case OOT_Stop_Buy:
			case OOT_Stop_Sell:
				{
					checkStopOperation(oper, p);
					register_oper = true;

					OperationStructureInstall oi(oper.provider_m, oper.symbol_m, p.time_m, oper.price_m, oper.volume_m, oper.brkSession_m, oper.type_m);
					callback_m.onInstallationProcedure(oi, oper);
				}
				break;
			case OOT_Stop_Limit_Buy:
			case OOT_Stop_Limit_Sell:
				{
					checkStopLimitOperation(oper, p);
					register_oper = true;

					OperationStructureInstall oi(oper.provider_m, oper.symbol_m, p.time_m, oper.price_m, oper.volume_m, oper.brkSession_m, oper.type_m);
					callback_m.onInstallationProcedure(oi, oper);
				}
				break;
			default:
				THROW_BRK(getBrokerEngine().getBroker().getName().c_str(), "exc_InvalidOperationType", "ctx_processOperation", "" );
		};

		if (register_oper) {
			LOCK_FOR_SCOPE(*this);
			pendingOps_m[oper.orderUid_m] = oper;

			BRK_LOG(getBrokerEngine().getBroker().getName().c_str(), BRK_MSG_INFO, OPERATIONPROCESSOR, "Registering new operation with ID: "
				<< oper.orderUid_m << " pending operation size is: " <<  pendingOps_m.size());
		}
	}

	void OperationProcessor::processPendingOperations(Price const& p)
	{
	
		LOCK_FOR_SCOPE(*this);

		
			
		for( map<CppUtils::Uid, OrderOperation>::iterator i = pendingOps_m.begin(); i != pendingOps_m.end(); ) {
			OrderOperation &oper = i->second;

			switch(oper.type_m) {
				case OOT_Limit_Buy:
					{
						if (p.ask_m <= oper.price_m)	{
							double price = p.ask_m + oper.tinfo_m.slippage_m;
						
							
							getBrokerEngine().getPositionHolder().goLong(oper.provider_m, oper.symbol_m, p.time_m,	price, oper.volume_m, oper.affectedPositionId_m, oper.brkSession_m, oper );
							
						
							
							// remove pos
							pendingOps_m.erase(i++);
							
							BRK_LOG(getBrokerEngine().getBroker().getName().c_str(), BRK_MSG_INFO, OPERATIONPROCESSOR, 
								"Limit Buy executed, new pending orders list size: " << pendingOps_m.size());
							
							continue;
						}
					}
					break;
				case OOT_Limit_Sell:
					{
						if (p.bid_m >= oper.price_m)	{
							double price = p.bid_m - oper.tinfo_m.slippage_m;
						
						
							getBrokerEngine().getPositionHolder().goShort(oper.provider_m, oper.symbol_m, p.time_m,	price, oper.volume_m, oper.affectedPositionId_m, oper.brkSession_m, oper );
							
						

							// remove pos
							pendingOps_m.erase(i++);

							BRK_LOG(getBrokerEngine().getBroker().getName().c_str(), BRK_MSG_INFO, OPERATIONPROCESSOR, 
								"Limit Sell executed, new pending orders list size: " << pendingOps_m.size());

						
							continue;
						}
					}
					break;
				case OOT_Stop_Buy:
					{
						if (p.ask_m >= oper.stopPrice_m) {
							double price = p.ask_m + oper.tinfo_m.slippage_m;
						
							
							getBrokerEngine().getPositionHolder().goLong(oper.provider_m, oper.symbol_m, p.time_m,	price, oper.volume_m, oper.affectedPositionId_m, oper.brkSession_m, oper );
							
						

							
							pendingOps_m.erase(i++);

							// remove pos
							BRK_LOG(getBrokerEngine().getBroker().getName().c_str(), BRK_MSG_INFO, OPERATIONPROCESSOR, 
								"Stop Buy executed, new pending orders list size: " << pendingOps_m.size());

							continue;
						}
					}
					break;
				case OOT_Stop_Sell:
					{
						if (p.bid_m <= oper.stopPrice_m) {
							double price = p.bid_m - oper.tinfo_m.slippage_m;;
						
							getBrokerEngine().getPositionHolder().goShort(oper.provider_m, oper.symbol_m, p.time_m,	price, oper.volume_m, oper.affectedPositionId_m, oper.brkSession_m, oper );
							
					

							// remove pos
							pendingOps_m.erase(i++);

							BRK_LOG(getBrokerEngine().getBroker().getName().c_str(), BRK_MSG_INFO, OPERATIONPROCESSOR, 
								"Stop Sell executed, new pending orders list size: " << pendingOps_m.size());

							
							continue;
						}
					}
					break;
				case OOT_Stop_Limit_Buy:
					{
						if (p.ask_m >= oper.stopPrice_m) {
							// now it is limit order
							CppUtils::Uid new_uid; new_uid.generate();
							OrderOperation oper_limit_buy(OOT_Limit_Buy, oper.provider_m, oper.symbol_m, oper.brkSession_m, oper.volume_m, oper.stopPrice_m, -1, oper.tinfo_m, oper.cancelOrderId_m, oper.moveOrderId_m, new_uid, oper.affectedPositionId_m, oper.order_m);

							pendingOps_m.erase(i++);

							BRK_LOG(getBrokerEngine().getBroker().getName().c_str(), BRK_MSG_INFO, OPERATIONPROCESSOR, 
								"Stop Limit Buy -> Limit Buy, new pending orders list size: " << pendingOps_m.size());

							// insert another op
							processOperation(p, oper_limit_buy);


							continue;
						}
					}
					break;
				case OOT_Stop_Limit_Sell:
					{
						if (p.bid_m <= oper.stopPrice_m) {
							// now it is limit order
							CppUtils::Uid new_uid; new_uid.generate();
							OrderOperation oper_limit_sell(OOT_Limit_Sell, oper.provider_m, oper.symbol_m, oper.brkSession_m, oper.volume_m, oper.stopPrice_m, -1, oper.tinfo_m,  oper.cancelOrderId_m, oper.moveOrderId_m, new_uid, oper.affectedPositionId_m, oper.order_m);

							// remove thsi op
							pendingOps_m.erase(i++);

							BRK_LOG(getBrokerEngine().getBroker().getName().c_str(), BRK_MSG_INFO, OPERATIONPROCESSOR, 
								"Stop Limit Sell -> Limit Sell, new pending orders list size: " << pendingOps_m.size());

							// insert another op
							processOperation(p, oper_limit_sell);

							continue;
						}
					}
					break;
				default:
					THROW_BRK(getBrokerEngine().getBroker().getName().c_str(), "exc_InvalidOperationType", "ctx_processPendingOperations", "");
			}

			//
			++i;
		}
	}

	void OperationProcessor::cancelOrder(OrderOperation const& oper,Price const& p)
	{
		LOCK_FOR_SCOPE(*this);
		if (oper.cancelOrderId_m.isValid()) {
			// cancel this order
			map<CppUtils::Uid, OrderOperation>::iterator i = pendingOps_m.find(oper.cancelOrderId_m);
			if (i == pendingOps_m.end())
				THROW_BRK(getBrokerEngine().getBroker().getName().c_str(), "exc_InvalidCancellationOrderId", "ctx_cancelOrder", getCommonOperationDescription(oper, p));

			OperationStructureCancel co(oper.provider_m, oper.symbol_m, oper.brkSession_m, oper.cancelOrderId_m, p.time_m );
			callback_m.onCancellationProcedure(co, oper, i->second);
			pendingOps_m.erase( i );

			BRK_LOG(getBrokerEngine().getBroker().getName().c_str(), BRK_MSG_INFO, OPERATIONPROCESSOR, 
								"Cancel executed, new pending orders list size: " << pendingOps_m.size());
		}
		else {
			// cancel all orders related to provider - symbol
			for( map<CppUtils::Uid, OrderOperation>::iterator i = pendingOps_m.begin(); i != pendingOps_m.end(); ) {
				if (i->second.symbol_m == oper.symbol_m && i->second.provider_m == oper.provider_m) {

					OperationStructureCancel co(oper.provider_m, oper.symbol_m, oper.brkSession_m, i->first, p.time_m );
					callback_m.onCancellationProcedure(co, oper, i->second);

					pendingOps_m.erase(i++);
					BRK_LOG(getBrokerEngine().getBroker().getName().c_str(), BRK_MSG_INFO, OPERATIONPROCESSOR, 
								"Cancel executed, new pending orders list size: " << pendingOps_m.size());
				}
				else {
					++i;
				}

			}
		}
	}

	void OperationProcessor::checkCommonOperationFields(OrderOperation const& oper)
	{
		if (oper.provider_m.size() == 0)
			THROW_BRK(getBrokerEngine().getBroker().getName().c_str(), "exc_InvalidProvider", "ctx_checkCommonOperationFields", oper.toString());

		if (oper.symbol_m.size() == 0)
			THROW_BRK(getBrokerEngine().getBroker().getName().c_str(), "exc_InvalidSymbol", "ctx_checkCommonOperationFields", oper.toString());

		if (!oper.orderUid_m.isValid())
			THROW_BRK(getBrokerEngine().getBroker().getName().c_str(), "exc_InvalidOrderUID", "ctx_checkCommonOperationFields", oper.toString());

	}

	void OperationProcessor::checkLimitOperation(OrderOperation const& oper,Price const& p)
	{
		if (oper.volume_m <=0)
			THROW_BRK(getBrokerEngine().getBroker().getName().c_str(), "exc_VolumeLessZero", "ctx_checkLimitOperation", getCommonOperationDescription(oper, p));

		if (oper.price_m <=0)
			THROW_BRK(getBrokerEngine().getBroker().getName().c_str(), "exc_PriceLessZero", "ctx_checkLimitOperation", getCommonOperationDescription(oper, p));

		if(oper.type_m == OOT_Limit_Buy) {
			if ((p.ask_m - oper.price_m) < (oper.tinfo_m.minLimitOrderDistance_m+oper.tinfo_m.slippage_m)) 
				THROW_BRK(getBrokerEngine().getBroker().getName().c_str(), "exc_InvalidPrice", "ctx_checkLimitOperation",getCommonOperationDescription(oper, p));
		
		}
		else if(oper.type_m == OOT_Limit_Sell) {
			if ((oper.price_m - p.bid_m) < (oper.tinfo_m.minLimitOrderDistance_m+oper.tinfo_m.slippage_m)) 
				THROW_BRK(getBrokerEngine().getBroker().getName().c_str(), "exc_InvalidPrice", "ctx_checkLimitOperation", getCommonOperationDescription(oper, p));
			
		}
	}

	void OperationProcessor::checkStopOperation(OrderOperation const& oper,Price const& p)
	{
		if (oper.volume_m <=0)
			THROW_BRK(getBrokerEngine().getBroker().getName().c_str(),  "exc_VolumeLessZero", "ctx_checkStopOperation", getCommonOperationDescription(oper, p));

		if (oper.stopPrice_m <=0)
			THROW_BRK(getBrokerEngine().getBroker().getName().c_str(),  "exc_StopPriceLessZero", "ctx_checkStopOperation", getCommonOperationDescription(oper, p));

		if(oper.type_m == OOT_Stop_Sell) {
			// sell
			if ((p.bid_m - oper.stopPrice_m ) < (oper.tinfo_m.minStopOrderDistance_m+oper.tinfo_m.slippage_m)) 
				THROW_BRK(getBrokerEngine().getBroker().getName().c_str(),  "exc_InvalidStopPrice", "ctx_checkStopOperation", getCommonOperationDescription(oper, p));
			
		}
		else if (oper.type_m == OOT_Stop_Buy) {
			if ((oper.stopPrice_m - p.ask_m ) < (oper.tinfo_m.minStopOrderDistance_m+oper.tinfo_m.slippage_m)) 
				THROW_BRK(getBrokerEngine().getBroker().getName().c_str(),  "exc_InvalidStopPrice", "ctx_checkStopOperations", getCommonOperationDescription(oper, p));
			 
		}

	}


	void OperationProcessor::checkStopLimitOperation(OrderOperation const& oper,Price const& p)
	{
		if (oper.volume_m <=0)
			THROW_BRK(getBrokerEngine().getBroker().getName().c_str(),  "exc_VolumeLessZero", "ctx_checkStopLimitOperation", getCommonOperationDescription(oper, p));

		if (oper.price_m <=0)
			THROW_BRK(getBrokerEngine().getBroker().getName().c_str(),  "exc_StopPriceLessZero", "ctx_checkStopLimitOperation", getCommonOperationDescription(oper, p));

		if (oper.stopPrice_m <=0)
			THROW_BRK(getBrokerEngine().getBroker().getName().c_str(),  "exc_StopPriceLessZero", "ctx_checkStopLimitOperation", getCommonOperationDescription(oper, p));

		if(oper.type_m == OOT_Stop_Limit_Buy) {
			if ((oper.stopPrice_m - p.ask_m ) < (oper.tinfo_m.minStopOrderDistance_m+oper.tinfo_m.slippage_m)) 
				THROW_BRK(getBrokerEngine().getBroker().getName().c_str(),  "exc_InvalidStopPrice", "ctx_checkStopLimitOperation", getCommonOperationDescription(oper, p));
		}
		else if(oper.type_m == OOT_Stop_Limit_Sell) {
			if ((p.bid_m - oper.stopPrice_m ) < (oper.tinfo_m.minStopOrderDistance_m+oper.tinfo_m.slippage_m)) 
				THROW_BRK(getBrokerEngine().getBroker().getName().c_str(),  "exc_InvalidStopPrice", "ctx_checkStopLimitOperation", getCommonOperationDescription(oper, p));
			
		}
	}

	void OperationProcessor::moveOrder(OrderOperation const& oper, Price const& p)
	{
		// use price and stopprice as  anew level of price
		// and order ID

		LOCK_FOR_SCOPE(*this);

		map<CppUtils::Uid, OrderOperation>::iterator i = pendingOps_m.find(oper.moveOrderId_m);
		if (i == pendingOps_m.end())
			THROW_BRK(getBrokerEngine().getBroker().getName().c_str(), "exc_InvalidMoveOrderId", "ctx_moveOrder", oper.moveOrderId_m.toString());

		
		OrderOperation& operation_to_move = i->second;

		operation_to_move.stopPrice_m = oper.stopPrice_m;
		operation_to_move.price_m = oper.price_m;

		// check prices
		switch(operation_to_move.type_m)
		{
			case OOT_Limit_Buy:
			case OOT_Limit_Sell:
				checkLimitOperation(operation_to_move, p);
				break;
			case OOT_Stop_Buy:
			case OOT_Stop_Sell:
				checkStopOperation(operation_to_move, p);
				break;
			case OOT_Stop_Limit_Buy:
			case OOT_Stop_Limit_Sell:
				checkStopLimitOperation(operation_to_move, p);
				break;
			default:
				THROW_BRK(getBrokerEngine().getBroker().getName().c_str(),  "exc_InvalidOrderToMove", "ctx_MoveOrder", getCommonOperationDescription(oper, p));
		};

		OperationStructureMove co(oper.provider_m, oper.symbol_m, oper.brkSession_m, i->first, p.time_m );
		callback_m.onMoveProcedure(co, oper, operation_to_move);

	}

	

}; // end if ns
