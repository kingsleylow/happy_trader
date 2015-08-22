#ifndef _BRKLIBBTEST_BROKER_INCLUDED
#define _BRKLIBBTEST_BROKER_INCLUDED

#include "brklibbtestdefs.hpp"

/**
This is the implementation of backets library that can work of any algorithm to perform history simulation

*/
namespace BrkLib {

	/**
	 Forwards
	*/

	class BtSession;


	/**
	 Helper structures
	*/

	struct PendingCommand {
		PendingCommand(Order const& order, BtSession& session):
			btSession_m(session),
			order_m(order)
		{
		}
		Order order_m;
		BtSession& btSession_m;
	};

	//

	struct Price {
		
		void clear()
		{
			bid_m = -1;

			ask_m = -1;

			time_m = -1;
		}

		bool isValid() const
		{
			return bid_m > 0 && ask_m > 0 && time_m > 0;
		}

		double bid_m;

		double ask_m;

		double time_m;
	};

	//
	struct PriceData
	{
		Price curPrice_m;
		
	};

	
	// -----------------------------------------
	
	class BRKLIBBTEST_EXP BtBroker: 
		public BrkLib::BrokerBase
	{
		friend class BtSession;
	public:
		
		
		BtBroker( Inqueue::AlgorithmHolder& algHolder);

		virtual ~BtBroker();
		
		virtual void onLibraryInitialized(Inqueue::AlgBrokerDescriptor const& descriptor);

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
		
		virtual BrkLib::PositionList const& getBrokerPositionList(int const posState, BrkLib::BrokerSession &session);

		virtual void updatePositionTagFields(BrkLib::BrokerSession &session, CppUtils::Uid const posUid, int const tag1, int const tag2, int const posState);

		virtual void onDataArrived(	HlpStruct::RtData const& rtdata);
		
		virtual void initializeBacktest(
			Inqueue::SimulationProfileData const& simulProfileData, 
			BrokerBase::ProviderSymbolCookieMap const& providerSymbolCookieMap 
		);
		
		virtual void initializeBacktest(Inqueue::SimulationProfileData const& simulProfileData );



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
			Inqueue::AggregationPeriods aggrPeriod,
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

		virtual void onThreadFinished( bool const firstLib, bool const lastLib
		)
		{
		};
	private:			

		inline bool isLong(PosType const posType) const
		{
			if (posType==POS_BUY || posType==POS_LIMIT_BUY || posType==POS_STOP_BUY)
				return true;
			else if (posType==POS_SELL || posType==POS_LIMIT_SELL || posType==POS_STOP_SELL)
				return false;
			
			THROW(CppUtils::OperationFailed, "exc_PositionIsNotInConsistentState", "ctx_IsPositionLong", "");
		}

		// processes pending orders

		void doProcessPendingOrderHelper(BrkLib::BrokerSession &session, Order const& order);

		//void doProcessPendingOrders();

		void executeCurrentOrders();

		void processHandshake(Order const& order, BtSession& session, BrokerResponseBase& result);

		void getSymbolMetainfo(Order const& order, BtSession& session, BrokerResponseMetaInfo& brespMinfo);

		
		
		void openBuyPos(Order const& order, BtSession& session, BrokerResponseOrder& result);

		void openSellPos(Order const& order, BtSession& session, BrokerResponseOrder& result);

		
		
		void installBuyLimit(Order const& order, BtSession& session, BrokerResponseOrder& result);

		void installSellLimit(Order const& order, BtSession& session, BrokerResponseOrder& result);


		void installBuyStop(Order const& order, BtSession& session, BrokerResponseOrder& result);

		void installSellStop(Order const& order, BtSession& session, BrokerResponseOrder& result);

		void changeSLTPExecExpir(Order const& order, BtSession& session, BrokerResponseOrder& result);

		

		void cancelOrder(Order const& order, BtSession& session, BrokerResponseOrder& result);

		void closeOrder(Order const& order, BtSession& session, BrokerResponseOrder& result);


		inline PriceData const& resolveFastPriceData(int const cookie) const;

		int resolveProviderSymbolCookie(CppUtils::String const& provider, CppUtils::String const& symbol) const;

		int resolveProviderSymbolCookieNoExcept(CppUtils::String const& provider, CppUtils::String const& symbol) const;


		//SymbolTradingInfo const& resolveTradingInfo(CppUtils::String const& provider, CppUtils::String const& symbol) const;

		SymbolTradingInfo const& resolveFastTradingInfo(int const cookie) const;

		bool isMetaInfoExists(CppUtils::String const& provider, CppUtils::String const& symbol) const;
		

		// functions that prepare response
		void prepareResponseHeader(Order const& order, BrokerResponseBase& result);
		
		void prepareBrokerResponseOrderHeader(
			SymbolTradingInfo const& tinfo,
			Order const& order, 
			BrokerResponseOrder& oresult, 
			double const& brokerIssueTime,
			double const& openTime,
			double const& openPrice,
			PosType const posType,
			double const& volume,
			double const& expirTime
		);

		// initialize common values for new position
		void initPosCommonValues(
			Position& pos, 
			BrokerResponseOrder& oresult, 
			CppUtils::Uid const& newUid, 
			BtSession& session);

		// internal execution and closure of position
		void execPosition(Position& pos, double const &execTime, double const &execPrice, PosState const newState, PosType const newType);

		void closePosition(Position& pos, double const &closeTime, double const &closePrice, PosState const newState);

		

		// locates opened or pending position or throws an exception
		Position& locateActivePosition(CppUtils::Uid const& uid);
		
		// checks the ability to install TP/SL and initialize result
		bool checkTPSLtoChange(	
			Order const& order,
			BrokerResponseOrder& result,
			SymbolTradingInfo const& tinfo, 
			PriceData const& pdata,
			bool const islong,
			double const& newTpPrice,
			double const& newSlPrice
		);

		bool checkExpirDatetoChange(
			Order const& order,
			BrokerResponseOrder& result,
			SymbolTradingInfo const& tinfo, 
			PriceData const& pdata
		);
		
		// connection map 

		CppUtils::Mutex sessionsMtx_m;

		map<CppUtils::String, BtSession*> sessions_m;

		map<CppUtils::Uid, BtSession*> sessions2_m;

		// pending orders
		CppUtils::Mutex pendingOrdesMtx_m;

		//queue<PendingCommand> pendingOrders_m;

		

		CppUtils::Mutex positionsMtx_m;

		// map of active (installed ) positions
		map<CppUtils::Uid, Position> pendingPositions_m;

		// map of already opened positions
		map<CppUtils::Uid, Position> openedPositions_m;

		// map of closed positions 
		map<CppUtils::Uid, Position> historyPositions_m; 


		// this is global simulation fata passed from server layer
		Inqueue::SimulationProfileData simulProfileData_m;

		// fast accessor to symbol trading metainfo - through index
		vector<BrkLib::SymbolTradingInfo> symbTradingInfoFast_m;

		// this is the map to contain cookies from each provider symbol
		BrokerBase::ProviderSymbolCookieMap providerSymbolCookieMap_m;


		// price data may be accessed from many threads
		CppUtils::Mutex priceDataMtx_m; 

		// fast accessor for price data
		vector<PriceData> priceDataFast_m;	
		
		
	};



}

#endif