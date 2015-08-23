#ifndef _BRKLIB_BRKOPER_INCLUDED
#define _BRKLIB_BRKOPER_INCLUDED

#include "brklibdefs.hpp"
#include "brokerstruct.hpp"
#include "brokersession.hpp"
#include "brkconnect.hpp"

namespace Inqueue {
	class AlgorithmHolder;
	enum AlgorithmStatus;
	class OutProcessor;
}


namespace BrkLib {
	/*
	 This is the helper class that provides the ONLY interface to perform operations against broker.
	*/
	class BRKLIB_EXP BrokerOperation {
	
	public:
		BrokerOperation(BrokerConnect& connect);

		~BrokerOperation();

		double getBrokerSystemTime();

		// operations against broker
		bool unblockingIssueOrder(Order const& order);

		bool getTickInfo(
			CppUtils::String const& symbolNum, 
			CppUtils::String const& symbolDenom
		);
		
		// subscribe for nes retreival
		bool subscribeForNews(
		);

		// returns last error information
		bool getLastErrorInfo(
		);

		// subscribe for quotes - they will arrive via onBrokerResponseArrived
		bool subscribeForQuotes(
			CppUtils::String const& symbolNum, 
			CppUtils::String const& symbolDenom
		);

		// returns the history from the server
		bool getHistoryInfo(
			CppUtils::String const& symbolNum, 
			CppUtils::String const& symbolDenom,
			HlpStruct::AggregationPeriods aggrPeriod,
			int const multfactor
		);

		void setUpMode(int mode);

		
		void getBrokerResponseList() const;
		
		void getBrokerResponseList(OperationResult const or) const;
		
		
		void getBrokerPositionList(int const posStateQuery, PositionList &posList) const;

		
		// delayed orders
		void registerDelayedOrder(DelayedOrderType const type, double const& execPrice, Order const& order, CppUtils::String const& comment);

		bool removeDelayedOrder(CppUtils::Uid const& uid);

		void removeAllDelayedOrders();


	private:

		inline bool isOperationAllowed() const
		{
			return ( (brkConnect_m.brkSessionPtr_m != NULL ) && brkConnect_m.getSession().isConnected() );
		}

		inline BrokerSession& getSession() const
		{
			return *brkConnect_m.brkSessionPtr_m;
		}

		
		inline BrokerBase& getBroker() const
		{
			return *brkConnect_m.brkBasePtr_m;
		}


		// broker connect reference
		BrokerConnect &brkConnect_m;


	};

};

#endif