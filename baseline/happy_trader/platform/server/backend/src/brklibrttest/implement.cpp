#include "implement.hpp"
#include "rtbroker.hpp"

#define BROKERENGINE "BROKERENGINE"
namespace BrkLib {
	

	BrokerEngine::BrokerEngine( RtBroker& rtbroker):
		operationProcessor_m(*this, *this),
		positionHolder_m(*this, *this, *this),
		broker_m(&rtbroker)
	{
	}
		
	BrokerEngine::~BrokerEngine()
	{
	}

	void BrokerEngine::initialize(SimulationProfileData const simProfileData, bool const useInlyOnePositionPerSymbol, bool const notUseCloseCommands)
	{
		simulProfileData_m =simProfileData;
		positionHolder_m.initialize(useInlyOnePositionPerSymbol);

		notUseCloseCommands_m = notUseCloseCommands;

		BRK_LOG(getBroker().getName().c_str(), BRK_MSG_INFO, BROKERENGINE, "Broker Engine initialized, useOnlyOnePositionPerSymbol: " << 
			CppUtils::bool2String(useInlyOnePositionPerSymbol) << ", notUseCloseCommands: " << CppUtils::bool2String(notUseCloseCommands));

		
	}

	void BrokerEngine::onNewPricedataArrived(CppUtils::String const& provider, CppUtils::String const& symbol, double const& bidPrice, double const& askPrice, double const& curTime)
	{
		
		Price pdata(bidPrice,askPrice,curTime );
		
		if (bidPrice <= 0 || askPrice <= 0 || curTime<=0 )
				THROW_BRK(getBroker().getName().c_str(),  "exc_InvalidBidAskPrices", "ctx_onNewPricedataArrived", "");

		{
			LOCK_FOR_SCOPE(priceDataMtx_m);
			
			// for fast access no more additional checks		
			priceData_m[provider][symbol] = pdata;

		}

		// need to proceed pending orders as there is price update
		operationProcessor_m.processPendingOperations(pdata);
	}

	void BrokerEngine::onNewOrderArrived(Order const& order, RtSession& session)
	{
		CppUtils::Uid cancelOrderId;
		CppUtils::Uid moveOrderId;
		CppUtils::Uid positionId;

		
		if (order.orderType_m & OP_ORDER_CANCEL) {
			cancelOrderId.fromStringNoExcept( order.brokerOrderID_m );
		};

		if (order.orderType_m & OP_ORDER_MOVE) {
			moveOrderId = CppUtils::Uid(order.brokerOrderID_m);
			moveOrderId.fromStringNoExcept(order.brokerOrderID_m);
		}
	
		positionId.fromStringNoExcept(order.brokerPositionID_m);
		
		OrderOperation op(
			orderType2InternalType(order.orderType_m),
			order.provider_m,
			order.symbol_m,
			&session,
			order.orVolume_m,
			order.orPrice_m,
			order.orStopPrice_m,
			resolveFastTradingInfo(order.provider_m, order.symbol_m),
			cancelOrderId, // for cancellation 
			moveOrderId,
			order.ID_m,
			positionId, // for cancellation 
			order
		);

		operationProcessor_m.processOperation(op);
		
	}

	Price BrokerEngine::resolveFastPriceData(CppUtils::String const& provider, CppUtils::String const& symbol) const
	{
		LOCK_FOR_SCOPE(priceDataMtx_m);
		map< CppUtils::String, map<CppUtils::String, Price > >::const_iterator it = 
			priceData_m.find(provider);

		if (it==priceData_m.end())
			THROW_BRK(getBroker().getName().c_str(),  "exc_ProviderNotRegistred", "ctx_ResolvePriceData", provider);

		map<CppUtils::String, Price >::const_iterator it2 = it->second.find(symbol);

		if (it2==it->second.end())
			THROW_BRK(getBroker().getName().c_str(),  "exc_SymbolNotRegistred", "ctx_ResolvePriceData", symbol);

		Price const& pd = it2->second;
		if (!pd.isValid())
			THROW_BRK(getBroker().getName().c_str(),  "exc_PriceDataMustBeValid", "ctx_ResolvePriceData", provider + " - " + symbol );

		return pd;
	}


	bool BrokerEngine::existFastTradingInfo(CppUtils::String const& provider, CppUtils::String const& symbol) const
	{
		
		map< CppUtils::String, map<CppUtils::String, SymbolTradingInfo > >::const_iterator it = 
			simulProfileData_m.symbolsTradingInfo_m.find(provider);

		if (it==simulProfileData_m.symbolsTradingInfo_m.end()) {
		 return false;
		}

		map<CppUtils::String, SymbolTradingInfo >::const_iterator it2 = it->second.find(symbol);

		if (it2==it->second.end()) {
			return false;
		}

		return true;
	}

	SymbolTradingInfo const& BrokerEngine::resolveFastTradingInfo(CppUtils::String const& provider, CppUtils::String const& symbol) const
	{
		static SymbolTradingInfo defaultInfo;
		map< CppUtils::String, map<CppUtils::String, SymbolTradingInfo > >::const_iterator it = 
			simulProfileData_m.symbolsTradingInfo_m.find(provider);

		if (it==simulProfileData_m.symbolsTradingInfo_m.end()) {
			//THROW(CppUtils::OperationFailed, "exc_ProviderNotregistred", "ctx_ResolveTradingInfo", provider);
			BRK_LOG(getBroker().getName().c_str(), BRK_MSG_WARN, BROKERENGINE, "Provider is not registered in symbol metainfo: " << provider <<  ", use default");


			return defaultInfo;
		}

		map<CppUtils::String, SymbolTradingInfo >::const_iterator it2 = it->second.find(symbol);

		if (it2==it->second.end()) {
			//THROW(CppUtils::OperationFailed, "exc_SymbolNotRegistred", "ctx_ResolveTradingInfo", symbol);
			BRK_LOG(getBroker().getName().c_str(), BRK_MSG_WARN, BROKERENGINE, "Symbol is not registered in symbol metainfo: " << symbol << ", use default");
			return defaultInfo;
		}

		return it2->second;
	}

	RtBroker& BrokerEngine::getBroker()
	{
		if (!broker_m)
			THROW_BRK(getBroker().getName().c_str(),  "exc_BrokerEngineNotInitialized", "ctx_getBrokerRef", "");
		return *broker_m;
	}

	RtBroker const& BrokerEngine::getBroker()	const
	{
		if (!broker_m)
			THROW_BRK(getBroker().getName().c_str(),  "exc_BrokerEngineNotInitialized", "ctx_getBrokerRef", "");
		return *broker_m;
	}

	PositionHolder& BrokerEngine::getPositionHolder()
	{
		return positionHolder_m;
	}
		

	void BrokerEngine::onPositionUpdate(CppUtils::String const& provider, CppUtils::String const& symbol, PositionForSymbol const& position)
	{
		BRK_LOG(getBroker().getName().c_str(), BRK_MSG_INFO, BROKERENGINE, "onPositionUpdate(...), provider: " << provider 
			<< " symbol: " << symbol << " position: " << position.toString());

		if (position.history_m) {
			// just closed
			SymbolTradingInfo const& tinfo = resolveFastTradingInfo(provider, symbol);

			getBroker().sendResponseClosePos(
				(RtSession&)*position.brkSession_m,
				position.getOpenTime(),
				symbol,
				provider,
				position.direction_m,
				position.getAvrOpenPrice(),
				position.getAvrClosePrice(),
				position.getFullCloseVolume(),
				position.positionUid_m.toString(),
				position.getOpenTime(),
				position.getCloseTime(),
				tinfo
			);
		}
	}

	void BrokerEngine::onRequireBuyOperation(
		CppUtils::String const& provider, 
		CppUtils::String const& symbol, 
		double const& price, 
		double const& volume, 
		double const& ttime, 
		BrokerSession& brkSess,
		OrderOperation const& oper
	)
	{
		BRK_LOG(getBroker().getName().c_str(), BRK_MSG_INFO, BROKERENGINE, "onRequireBuyOperation(...)");

		Order order;
		order.provider_m = provider;
		order.symbol_m = symbol;
		order.orderType_m = BrkLib::OP_BUY;
	
		order.orVolume_m = volume;
		order.orPrice_m = price;
		order.orderIssueTime_m = ttime;

		order.orderValidity_m = oper.order_m.orderValidity_m;
		order.context_m  = oper.order_m.context_m;
		

		onNewOrderArrived(order,(RtSession&)brkSess);
		
	}

	void BrokerEngine::onRequireSellOperation(
		CppUtils::String const& provider, 
		CppUtils::String const& symbol, 
		double const& price, 
		double const& volume, 
		double const& ttime, 
		BrokerSession& brkSess,
		OrderOperation const& oper
	)
	{
		BRK_LOG(getBroker().getName().c_str(), BRK_MSG_INFO, BROKERENGINE, "onRequireSellOperation(...)");

		Order order;
		order.provider_m = provider;
		order.symbol_m = symbol;
		order.orderType_m = BrkLib::OP_SELL;
	
		order.orVolume_m = volume;
		order.orPrice_m = price;
		order.orderIssueTime_m = ttime;

		order.orderValidity_m = oper.order_m.orderValidity_m;
		order.context_m  = oper.order_m.context_m;
	
		onNewOrderArrived(order,(RtSession&)brkSess);
															   
	}

	void BrokerEngine::onInstallationProcedure(OperationStructureInstall const& os, OrderOperation const& oper)
	{
	
		BRK_LOG(getBroker().getName().c_str(), BRK_MSG_INFO, BROKERENGINE, "onInstallationProcedure(...), operation: " << oper.toString() 
			<< " returning data: " << os.toString());

		// must be adopted
		OrderType ot = oper.order_m.orderType_m;
	
		if (oper.order_m.orderType_m & BrkLib::OP_BUY_STOP_LIMIT) {
			if (oper.type_m == OOT_Limit_Buy) {
				ot = OP_BUY_LIMIT;
			}
		}

		if (oper.order_m.orderType_m & BrkLib::OP_SELL_STOP_LIMIT) {
			if (oper.type_m == OOT_Limit_Sell) {
				ot = OP_SELL_LIMIT;
			}
		}

		if (oper.order_m.orderType_m & BrkLib::OP_CLOSE_SHORT_STOP_LIMIT)	{
			if (oper.type_m == OOT_Limit_Buy) {
				ot = OP_CLOSE_SHORT_LIMIT;
			}
		}

		if (oper.order_m.orderType_m & BrkLib::OP_CLOSE_LONG_STOP_LIMIT) {
			if (oper.type_m == OOT_Limit_Sell) {
				ot = OP_CLOSE_LONG_LIMIT;
			}
		}

		CppUtils::String comment;
		if (ot != oper.order_m.orderType_m) {
			comment = "STOP LIMIT -> LIMIT";
		}


		double price = -1,	stopprice = -1;
		if (oper.isStopOperration())
			stopprice = os.installPrice_m;
		else 
			price = os.installPrice_m; 

		// order pending before
		getBroker().sendResponseOnPendingOperation(
				(BrkLib::RtSession &)*os.brkSession_m,
				oper.affectedPositionId_m.toString(), 
				price,
				stopprice, 
				ot, // order type
				os.installTime_m, 
				os.volume_m, 
				PT_ORDER_ESTABLISHED, 
				ODR_ORDER_PENDING,
				os.symbol_m,
				os.provider_m,
				oper.order_m,
				NULL,
				comment.c_str()
			);


		// then order installed

		getBroker().sendResponseOnPendingOperation(
				(BrkLib::RtSession &)*os.brkSession_m,
				oper.affectedPositionId_m.toString(), 
				price,
				stopprice, 
				ot, // order type
				os.installTime_m, 
				os.volume_m, 
				PT_ORDER_ESTABLISHED, 
				ODR_ORDER_INSTALLED,
				os.symbol_m,
				os.provider_m,
				oper.order_m,
				NULL,
				comment.c_str()
			);

	
	}

	void BrokerEngine::onExecutionProcedure(OperationStructureExecute const& os, OrderOperation const& oper)
	{
		BRK_LOG(getBroker().getName().c_str(), BRK_MSG_INFO, BROKERENGINE, "onExecutionProcedure(...), operation: " << oper.toString() 
			<< " returning data: " << os.toString());

		double price = -1, stopprice = -1;
		if (oper.isStopOperration())
			stopprice = os.executePrice_m;
		else 
			price = os.executePrice_m; 

		
		OrderType ot = OP_DUMMY;
		if (os.ta_m == PTA_Create || os.ta_m == PTA_Increase) {
			
			if (os.tradeDirection_m == TD_LONG) {
				ot = OP_BUY; 
			}
			else if (os.tradeDirection_m == TD_SHORT) {
				ot = OP_SELL; 
			}
			else
				THROW_BRK(getBroker().getName().c_str(), "exc_InvalidTradeDirection", "ctx_onExecutionProcedure", "");
		}
		else if (os.ta_m == PTA_Decrease) {
			if (os.tradeDirection_m == TD_LONG) {
				ot = ( notUseCloseCommands_m ? OP_SELL : OP_CLOSE_LONG );
			}
			else if (os.tradeDirection_m == TD_SHORT) {
				ot = ( notUseCloseCommands_m ? OP_BUY : OP_CLOSE_SHORT );
			}
			else
				THROW_BRK(getBroker().getName().c_str(),  "exc_InvalidTradeDirection", "ctx_onExecutionProcedure", "");
		}
		else
			THROW_BRK(getBroker().getName().c_str(),  "exc_InvalidPositionTradeAction", "ctx_onExecutionProcedure", "");


		getBroker().sendResponseOnPendingOperation(
				(BrkLib::RtSession &)*os.brkSession_m,
				oper.affectedPositionId_m.toString(), 
				price,
				stopprice, 
				ot, // order type
				os.executeTime_m, 
				os.volume_m, 
				PT_ORDER_ESTABLISHED, 
				ODR_ORDER_EXECUTED,
				os.symbol_m,
				os.provider_m,
				oper.order_m, // original order
				NULL
		);
	
	}

	void BrokerEngine::onCancellationProcedure(OperationStructureCancel const& os, OrderOperation const& oper, OrderOperation const& removed_oper )
	{
		BRK_LOG(getBroker().getName().c_str(), BRK_MSG_INFO, BROKERENGINE, "onCancellationProcedure(...), operation: " << oper.toString() <<
			" removed operation: " << removed_oper.toString() << 
			" returning data: " << os.toString());


		getBroker().sendResponseOnPendingOperation(
				(BrkLib::RtSession &)*os.brkSession_m,
				"", //no position 
				-1,
				-1, 
				OP_ORDER_CANCEL,
				-1, 
				-1, 
				PT_ORDER_CANCELLED, 
				ODR_OK,
				os.symbol_m,
				os.provider_m,
				removed_oper.order_m, // original order
				&oper.order_m
			);
	}

	void BrokerEngine::onMoveProcedure(OperationStructureMove const& os, OrderOperation const& oper, OrderOperation const& moved_oper )
	{
		BRK_LOG(getBroker().getName().c_str(), BRK_MSG_INFO, BROKERENGINE, "Order moved");

		getBroker().sendResponseOnPendingOperation(
				(BrkLib::RtSession &)*os.brkSession_m,
				"", //no position 
				moved_oper.price_m,
				moved_oper.stopPrice_m, 
				OP_ORDER_MOVE,
				os.moveTime_m, 
				-1, 
				PT_ORDER_MODIFIED, 
				ODR_OK,
				os.symbol_m,
				os.provider_m,
				moved_oper.order_m, // original order
				&oper.order_m
			);
	}


	OrderOperationType BrokerEngine::orderType2InternalType(int const orderType) const
	{
		OrderOperationType r = OOT_Dummy;

		if (orderType & OP_ORDER_MOVE) {
			r = OOT_Move;
		}
		else if (orderType & OP_ORDER_CANCEL) {
			r = OOT_Cancel;
		}
		else if (orderType & OP_BUY) {
			r=OOT_Market_Buy; 
		}
		else if (orderType & OP_SELL) {
			r=OOT_Market_Sell;
		}
		else if (orderType & OP_BUY_LIMIT) {
			r=OOT_Limit_Buy;
		}
		else if (orderType & OP_SELL_LIMIT) {
			r=OOT_Limit_Sell;
		}
		else if (orderType & OP_BUY_STOP) {
			r=OOT_Stop_Buy;
		}
		else if (orderType & OP_SELL_STOP) {
			r=OOT_Stop_Sell;
		}
		else if (orderType & OP_BUY_STOP_LIMIT) {
			r=OOT_Stop_Limit_Buy;
		}
		else if (orderType & OP_SELL_STOP_LIMIT) {
			r=OOT_Stop_Limit_Sell;
		}
		else
			if (notUseCloseCommands_m)
				THROW_BRK(getBroker().getName().c_str(),  "exc_CannotUseCloseCommands", "ctx_orderType2InternalType", "");


		if (orderType & OP_CLOSE_SHORT) {
			r=OOT_Market_Buy;
		}
		else if (orderType & OP_CLOSE_LONG) {
			r=OOT_Market_Sell;
		}
		else if (orderType & OP_CLOSE_SHORT_LIMIT) {
			r=OOT_Limit_Buy;
		}
		else if (orderType & OP_CLOSE_LONG_LIMIT) {
			r=OOT_Limit_Sell;
		}
		else if (orderType & OP_CLOSE_SHORT_STOP) {
			r=OOT_Stop_Buy;
		}
		else if (orderType & OP_CLOSE_LONG_STOP) {
			r=OOT_Stop_Sell;
		}																									 
		else if (orderType & OP_CLOSE_SHORT_STOP_LIMIT) {
			r=OOT_Stop_Limit_Buy;
		}
		else if (orderType & OP_CLOSE_LONG_STOP_LIMIT) {
			r=OOT_Stop_Limit_Sell;
		}

		return r;
	}

	

}; // end if ns