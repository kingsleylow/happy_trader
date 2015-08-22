#ifndef _BRKLIBRTTEST_BROKER_INCLUDED
#define _BRKLIBRTTEST_BROKER_INCLUDED

#include "brklibrttestdefs.hpp"
#include "implement.hpp"

/**
This is the implementation of backets library that can work of any algorithm to perform history simulation

*/
namespace BrkLib {

	/**
	 Forwards
	*/

	class RtSession;
	


	/**
	 Helper structures
	*/


	

	

	
	// -----------------------------------------
	
	class BRKLIBRTTEST_EXP RtBroker: 
		public BrkLib::BrokerBase
	{
		friend class RtSession;
		
		friend class BrokerEngine;
	public:
		
		
		RtBroker( Inqueue::AlgorithmHolder& algHolder);

		virtual ~RtBroker();
		
		virtual void onLibraryInitialized(HlpStruct::AlgBrokerDescriptor const& descriptor);

		virtual void onLibraryFreed();
		
		virtual CppUtils::String const& getName() const;

public:

		virtual double getBrokerSystemTime();

		// create and destroy session objects
		virtual BrokerSession& createSessionObject(CppUtils::String const& connectstring, CppUtils::String const& runName, CppUtils::String const& comment);

		virtual BrokerSession& resolveSessionObject(CppUtils::Uid const& uid);

		virtual void getAvailableSessions(CppUtils::UidList& activeSesionsUids) const;

		virtual void deleteSessionObject(BrokerSession& obj, CppUtils::String const& runName);
		

		// connect 
		virtual void connectToBroker(BrokerSession& session, CppUtils::String const& runName);

		// disconnect
		virtual void disconnectFromBroker(BrokerSession& session, CppUtils::String const& runName);


		virtual bool unblockingIssueOrder(BrkLib::BrokerSession &session, BrkLib::Order const& order);

		virtual void getBrokerResponseList(BrkLib::BrokerSession &session) const;
		
		virtual void getBrokerResponseList(BrkLib::BrokerSession &session, BrkLib::OperationResult const or) const;
		
		void getBrokerPositionList(int const posStateQuery, BrkLib::BrokerSession &session, PositionList &posList);

		
		virtual void onDataArrived(	HlpStruct::RtData const& rtdata);
		
		virtual void initializeBacktest(
			BrkLib::SimulationProfileData const& simulProfileData, 
			BrokerBase::ProviderSymbolCookieMap const& providerSymbolCookieMap 
		);

		virtual void initializeBacktest(BrkLib::SimulationProfileData const& simulProfileData );

		virtual bool isBacktestLibrary() const;

		virtual bool getTickInfo(
			BrkLib::BrokerSession &session, 
			CppUtils::String const& symbolNum, 
			CppUtils::String const& symbolDenom
		);

		
		// subscribe for nes retreival
		virtual bool subscribeForNews(
			BrkLib::BrokerSession &session
		) ;

		// returns last error information
		virtual bool getLastErrorInfo(
			BrkLib::BrokerSession &session
		) ;

		// subscribe for quotes - they will arrive via onBrokerResponseArrived
		virtual bool subscribeForQuotes(
			BrkLib::BrokerSession &session,
			CppUtils::String const& symbolNum, 
			CppUtils::String const& symbolDenom
		) ;

		// returns the history from the server
		virtual bool getHistoryInfo(
			BrkLib::BrokerSession &session,
			CppUtils::String const& symbolNum, 
			CppUtils::String const& symbolDenom,
			HlpStruct::AggregationPeriods aggrPeriod,
			int const multfactor
		);

		
		virtual void onCustomBrokerEventArrived(
			HlpStruct::CallingContext& callContext,
			CppUtils::String &customOutputData
		)
		{
		}

		virtual void onThreadStarted( bool const firstLib, bool const lastLib
		)
		{
		};

		virtual void onThreadFinished( HlpStruct::TradingParameters const& tradeParams, bool const firstLib, bool const lastLib		)
		{
		};


	private:		

	
		inline CppUtils::String getSymbolContext(Order const& order) const
		{
			return order.provider_m + " [ " + order.symbol_m + " ] ";
		}

		

		
		void processHandshake(Order const& order, RtSession& session, BrokerResponseBase& result);

		void getSymbolMetainfo(Order const& order, RtSession& session, BrokerResponseMetaInfo& brespMinfo);

		void issueOrder(BrkLib::BrokerSession &session, Order const& order);
	

		void sendErrorResponse(Order const& order, RtSession& session, BrokerResponseOrder& result, int const errcode,CppUtils::String const& reason);

		void sendResponseClosePos(
			BrkLib::RtSession & rtsession,
			double const& ttime,
			CppUtils::String const& symbol,
			CppUtils::String const& provider,
			TradeDirection const td,
			double const& openPrice,
			double const& closePrice,
			int const volume,
			CppUtils::String const& brokerPositionId,
			double const& posOpenTime,
			double const& posCloseTime,
			SymbolTradingInfo const& tinfo
		);

		void sendResponseOnPendingOperation(
			BrkLib::RtSession & rtsession,
			CppUtils::String const&  brokerPositionId,
			double const& price,
			double const& stopprice,
			BrkLib::OrderType const orderType,
			double const& ttime,
			int const volume,
			OperationResult const resultCode_m,
			int const resultDetailCode,
			CppUtils::String const& symbol,
			CppUtils::String const& provider,
			Order const& order,
			Order const* causeOrder =  NULL,// order thart caused operation on other order
			char const * brokerComment = NULL
		);
		
				

		// functions that prepare response
		void prepareResponseHeader(Order const& order, BrokerResponseBase& result);
		
	private:

		BrokerEngine engine_m;
			
		// connection map 

		CppUtils::Mutex sessionsMtx_m;

		map<CppUtils::String, RtSession*> sessions_m;

		map<CppUtils::Uid, RtSession*> sessions2_m;
	


		
	};



}

#endif