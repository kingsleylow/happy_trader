#ifndef _SMARTCOM_ALG_MISHA2_BROKERDIALOG_INCLUDED
#define _SMARTCOM_ALG_MISHA2_BROKERDIALOG_INCLUDED

#include "smartcom_alg_misha2defs.hpp"
#include "statemachinecontext.hpp"


namespace AlgLib {
	/**
	* This helper class sends orders
	*/
	class BrokerDialog {
	public:

		BrokerDialog();

		~BrokerDialog();

		inline bool isInitialized() const
		{
			return brokerConnect_m != NULL;
		}

		inline bool isConnected() const
		{
			return (brokerConnect_m != NULL && brokerConnect_m->getSession().isConnected());
		}

		inline CppUtils::String const& getConnectString() const
		{
			return connectString_m;
		}

		void initAndConnect(BrkLib::BrokerBase* brokerLibPtr, CppUtils::String const& connectString, CppUtils::String const& runName, CppUtils::String const& comment, CppUtils::String const& provider);

		void deinit();

		// market
		void issueBuyOrder(CppUtils::String const& portfolio, BrkLib::BrokerContext const& ctx, CppUtils::String const& symbol,double const& volume);

		void issueSellOrder(CppUtils::String const& portfolio, BrkLib::BrokerContext const& ctx, CppUtils::String const& symbol, double const& volume);

		// stop open
		void issueBuyOrder_Stop(CppUtils::String const& portfolio, BrkLib::BrokerContext const& ctx, double const& stopPrice, CppUtils::String const& symbol,double const& volume);

		void issueSellOrder_Stop(CppUtils::String const& portfolio, BrkLib::BrokerContext const& ctx, double const& stopPrice, CppUtils::String const& symbol, double const& volume);


		// limit open
		void issueBuyOrder_Limit(CppUtils::String const& portfolio, BrkLib::BrokerContext const& ctx, double const& limitPrice, CppUtils::String const& symbol,double const& volume);

		void issueSellOrder_Limit(CppUtils::String const& portfolio, BrkLib::BrokerContext const& ctx, double const& limitPrice, CppUtils::String const& symbol, double const& volume);

		
		void issueCancelOrder(CppUtils::String const& portfolio, BrkLib::BrokerContext const& ctx, CppUtils::String const& symbol, CppUtils::String const& brokerOrderId);

	private:

		void addPortfolio(BrkLib::Order &order, CppUtils::String const& portfolio);

		BrkLib::BrokerConnect* brokerConnect_m;

		CppUtils::String connectString_m;

		CppUtils::String provider_m;

	
	};
};

#endif