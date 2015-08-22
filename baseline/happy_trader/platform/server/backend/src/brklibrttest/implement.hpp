#ifndef _BRKLIBRTTEST_IMPLEMENT_INCLUDED
#define _BRKLIBRTTEST_IMPLEMENT_INCLUDED

#include "brklibrttestdefs.hpp"
#include "positionholder.hpp"
#include "commonstruct.hpp"
#include "rtsession.hpp"
#include "operationprocessor.hpp"

namespace BrkLib {


	/**
	* 1) For simplifying use only BUY and SELL commands, do not use CLOSE_LONG or CLOSE_SHORT
	* 2) Thus positions are the same if symbols are same
	*/
	class RtBroker;
	class BrokerEngine: public PositionHolder::PositionHolderCallback, public OperationProcessorCallBack {
	public:
		struct OrderHolderStruct
		{
			
			OrderHolderStruct(Order const& order, RtSession& session ):
			order_m(order),
			session_m( session )
			{
			}

			Order order_m;
			RtSession& session_m; 
		};

		BrokerEngine( RtBroker& rtbroker);
		
		~BrokerEngine();

		void initialize(SimulationProfileData const simProfileData, bool const useInlyOnePositionPerSymbol, bool const notUseCloseCommands);

		// here we check delayed orders - id they are executed
		void onNewPricedataArrived(CppUtils::String const& provider, CppUtils::String const& symbol,double const& bidPrice, double const& askPrice, double const& curTime);

		// here we check market orders
		void onNewOrderArrived(Order const& order, RtSession& session);

		Price resolveFastPriceData(CppUtils::String const& provider, CppUtils::String const& symbol) const;

		bool existFastTradingInfo(CppUtils::String const& provider, CppUtils::String const& symbol) const;

		SymbolTradingInfo const& resolveFastTradingInfo(CppUtils::String const& provider, CppUtils::String const& symbol) const;

		RtBroker& getBroker();

		RtBroker const& BrokerEngine::getBroker()	const;

		PositionHolder& getPositionHolder();
		
		virtual void onPositionUpdate(CppUtils::String const& provider, CppUtils::String const& symbol, PositionForSymbol const& position);

		virtual void onRequireBuyOperation(
			CppUtils::String const& provider, 
			CppUtils::String const& symbol, 
			double const& price, 
			double const& volume, 
			double const& ttime, 
			BrokerSession& brkSess,
			OrderOperation const& oper
		);
	
		virtual void onRequireSellOperation(
			CppUtils::String const& provider, 
			CppUtils::String const& symbol, 
			double const& price, 
			double const& volume, 
			double const& ttime, 
			BrokerSession& brkSess,
			OrderOperation const& oper
		);
	

		virtual void onInstallationProcedure(OperationStructureInstall const& os, OrderOperation const& oper);

		virtual void onExecutionProcedure(OperationStructureExecute const& os, OrderOperation const& oper);

		virtual void onCancellationProcedure(OperationStructureCancel const& os, OrderOperation const& oper, OrderOperation const& removed_oper );

		virtual void onMoveProcedure(OperationStructureMove const& os, OrderOperation const& oper, OrderOperation const& moved_oper );

		inline OperationProcessor& getOperationProcessor()
		{
			return operationProcessor_m;
		}

	private:
		 
		OrderOperationType orderType2InternalType(int const orderType) const;

		
	private:


		OperationProcessor operationProcessor_m;

		PositionHolder positionHolder_m;

		map< CppUtils::String, map<CppUtils::String, Price > > priceData_m;

		CppUtils::Mutex priceDataMtx_m;

		
		// simulation profile
		SimulationProfileData simulProfileData_m;

		RtBroker* broker_m;

		bool notUseCloseCommands_m;

		

	};
};

#endif