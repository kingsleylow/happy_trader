#include "brokerdialog.hpp"

#define BROKER_DIALOG "BROKER_DIALOG"
namespace AlgLib {


	BrokerDialog::BrokerDialog():
		brokerConnect_m(NULL)
	{
	}

	BrokerDialog::~BrokerDialog()
	{
		deinit();
	}

	void BrokerDialog::initAndConnect( BrkLib::BrokerBase* brokerLibPtr, CppUtils::String const& connectString, CppUtils::String const& runName, CppUtils::String const& comment, CppUtils::String const& provider)
	{
			
		
		provider_m = provider;
		if (!isInitialized()) {
			  connectString_m = connectString;
				if (!brokerLibPtr)
					return;

				brokerConnect_m = new BrkLib::BrokerConnect(brokerLibPtr, connectString.c_str(), false, false, runName.c_str(), comment.c_str());
				brokerConnect_m->connect();

				//if (!brokerConnect_m->getSession().isConnected()) {
				//	THROW(CppUtils::OperationFailed, "exc_CannotConnectToBroker", "ctx_BrokerDialogInit", connectString);
				//}

				
				brokerConnect_m->getSession().getBroker().setUpMode(BrkLib::BM_ONE_POSITION_PER_SYMBOL);
				
		 }
	}

	void BrokerDialog::deinit()
	{
		if (isInitialized()) {
			delete brokerConnect_m;
			brokerConnect_m = NULL;
		}
	}

	void BrokerDialog::issueBuyOrder(CppUtils::String const& portfolio, BrkLib::BrokerContext const& ctx, CppUtils::String const& symbol,double const& volume)
	{
		
		if (!isConnected())
			return;

		BrkLib::Order order;

		//order.ID_m.generate();
		addPortfolio(order, portfolio);

		order.provider_m = provider_m;
		order.symbol_m = symbol;
		order.orderType_m = BrkLib::OP_BUY;
		order.orderValidity_m = BrkLib::OV_DAY;
		order.orVolume_m = volume;
		order.context_m.setContext(ctx);

		BrkLib::BrokerOperation oper(*brokerConnect_m);
		oper.unblockingIssueOrder(order);

	}

	void BrokerDialog::issueBuyOrder_Limit(CppUtils::String const& portfolio, BrkLib::BrokerContext const& ctx, double const& limitPrice, CppUtils::String const& symbol,double const& volume)
	{
		
		if (!isConnected())
			return;

		BrkLib::Order order;

		//order.ID_m.generate();
		addPortfolio(order, portfolio);

		order.provider_m = provider_m;
		order.symbol_m = symbol;
		order.orderType_m = BrkLib::OP_BUY_LIMIT;
		order.orderValidity_m = BrkLib::OV_DAY;
		order.orVolume_m = volume;
		order.orPrice_m =limitPrice; 
		order.context_m.setContext(ctx);
				
	
		BrkLib::BrokerOperation oper(*brokerConnect_m);
		oper.unblockingIssueOrder(order);
	}

	void BrokerDialog::issueBuyOrder_Stop(CppUtils::String const& portfolio, BrkLib::BrokerContext const& ctx, double const& stopPrice, CppUtils::String const& symbol,double const& volume)
	{
		if (!isConnected())
			return;

		BrkLib::Order order;

		//order.ID_m.generate();
		addPortfolio(order, portfolio);

		order.provider_m = provider_m;
		order.symbol_m = symbol;
		order.orderType_m = BrkLib::OP_BUY_STOP;
		order.orderValidity_m = BrkLib::OV_DAY;
		order.orVolume_m = volume;
		order.orStopPrice_m = stopPrice;
	
		order.context_m.setContext(ctx);
				
	
		BrkLib::BrokerOperation oper(*brokerConnect_m);
		oper.unblockingIssueOrder(order);
	}

	void BrokerDialog::issueSellOrder_Stop(CppUtils::String const& portfolio, BrkLib::BrokerContext const& ctx, double const& stopPrice, CppUtils::String const& symbol, double const& volume)
	{
		if (!isConnected())
			return;

		BrkLib::Order order;

		//order.ID_m.generate();
		addPortfolio(order, portfolio);

		order.provider_m = provider_m;
		order.symbol_m = symbol;
		order.orderType_m = BrkLib::OP_SELL_STOP;
		order.orderValidity_m = BrkLib::OV_DAY;
		order.orVolume_m = volume;
		order.orStopPrice_m = stopPrice;
		
		order.context_m.setContext(ctx);
				
	
		BrkLib::BrokerOperation oper(*brokerConnect_m);
		oper.unblockingIssueOrder(order);
	}

	void BrokerDialog::issueSellOrder_Limit(CppUtils::String const& portfolio, BrkLib::BrokerContext const& ctx, double const& limitPrice, CppUtils::String const& symbol, double const& volume)
	{
		if (!isConnected())
			return;

		BrkLib::Order order;

		//order.ID_m.generate();
		addPortfolio(order, portfolio);

		order.provider_m = provider_m;
		order.symbol_m = symbol;
		order.orderType_m = BrkLib::OP_SELL_LIMIT;
		order.orderValidity_m = BrkLib::OV_DAY;
		order.orVolume_m = volume;
		order.orPrice_m =limitPrice; 
		order.context_m.setContext(ctx);
				
	
		BrkLib::BrokerOperation oper(*brokerConnect_m);
		oper.unblockingIssueOrder(order);
	}

	void BrokerDialog::issueSellOrder(CppUtils::String const& portfolio, BrkLib::BrokerContext const& ctx, CppUtils::String const& symbol,double const& volume)
	{
		if (!isConnected())
			return;

		BrkLib::Order order;

		//order.ID_m.generate();
		addPortfolio(order, portfolio);

		order.provider_m = provider_m;
		order.symbol_m = symbol;
		order.orderType_m = BrkLib::OP_SELL;
		order.orderValidity_m = BrkLib::OV_DAY;
		order.orVolume_m = volume;
		order.context_m.setContext(ctx);

		
		BrkLib::BrokerOperation oper(*brokerConnect_m);
		oper.unblockingIssueOrder(order);

	}

	
	void BrokerDialog::issueCancelOrder(CppUtils::String const& portfolio, BrkLib::BrokerContext const& ctx, CppUtils::String const& symbol, CppUtils::String const& brokerOrderId)
	{
		if (!isConnected())
			return;

		BrkLib::Order order;

		//order.ID_m.generate();
		addPortfolio(order, portfolio);

		order.provider_m = provider_m;
		order.brokerOrderID_m = brokerOrderId;
		order.symbol_m = symbol;
		order.orderType_m = BrkLib::OP_ORDER_CANCEL;
		order.orderValidity_m = BrkLib::OV_DAY;
		order.context_m.setContext(ctx);

		BrkLib::BrokerOperation oper(*brokerConnect_m);
		oper.unblockingIssueOrder(order);

	}

	void BrokerDialog::addPortfolio(BrkLib::Order &order, CppUtils::String const& portfolio)
	{
		order.additionalInfo_m.append("PORTFOLIO=").append(portfolio);
	}
};