#ifndef _BRKLIB_BROKER_INCLUDED
#define _BRKLIB_BROKER_INCLUDED

#include "brklibdefs.hpp"
#include "brokerstruct.hpp"
#include "brokersession.hpp"
#include "proxy.hpp"



namespace AlgLib {
	class AlgorithmBase;
};


namespace Inqueue {
	class AlgorithmHolder;
	enum AlgorithmStatus;
	class OutProcessor;
	
}

namespace BrkLib {
	
	/**
	 This is the class for storing delayed ordres
	*/

	struct DelayedOrder
	{
		DelayedOrder():
			executionPrice_m(-1),
			execType_m( DO_DUMMY )
		{
		}

		CppUtils::String toString() const
		{
			CppUtils::String result;

			result += "Delayed order ID: " + order_m.ID_m.toString()+"\n";
			if (execType_m == DO_DUMMY) {
				result += " execution type: DO_DUMMY\n";
			}
			else if (execType_m == DO_EXEC_PRICE_MORE) {
				result += " execution type: DO_EXEC_PRICE_MORE\n";
			}
			else if (execType_m == DO_EXEC_PRICE_LESS) {
				result += " execution type: DO_EXEC_PRICE_LESS\n";
			}

			result += " execution price: "+CppUtils::float2String(executionPrice_m,-1,4);
			result += " comment: ["+comment_m + "]";
			return result;
		}

		Order order_m;

		DelayedOrderType execType_m;

		double executionPrice_m;

		CppUtils::String comment_m;			
	
	};

	/**
		This is the base class for all the implemented brokers
	*/


	

	class BRKLIB_EXP BrokerBase	{
		friend class BrokerConnect;
		friend class BrokerOperation;
	public:
		typedef BrokerBase* (*CreatorBrokerFun) (Inqueue::AlgorithmHolder* );

		typedef void (*DestroyerBrokerFun) (BrokerBase*);

		// special map containing cookies for each provider symbol pair
		typedef map<CppUtils::String, map<CppUtils::String, int> > ProviderSymbolCookieMap;

	public:
		// ctor & dtor
		BrokerBase( Inqueue::AlgorithmHolder& algHolder);

		virtual ~BrokerBase();

		//	this is called one time before initialization
		virtual void onLibraryInitialized(HlpStruct::AlgBrokerDescriptor const& descriptor) = 0;

		//	this is called one time after initialization
		virtual void onLibraryFreed() = 0;

		// return the name of the broker
		virtual CppUtils::String const& getName() const = 0;

	
		// main functions that handles all the remote events
		// this is a virtual function to make late linkage to inqueue_bck library
		// for now here we have broker commands to be processed (like open order, close position, etc...)
		virtual bool processBrokerRelatedEvent(HlpStruct::EventData const &request, HlpStruct::EventData &response );

		
private:



	
protected:

		// this returnes broker system time
		virtual double getBrokerSystemTime(
		) = 0;

		// this creates session object and return the reference
		// this session object will be alive during <onDataArrived> call
		// this will not automatically connect to the broker
		// but may need to create structures to support multy-session enviroment
		virtual BrokerSession& createSessionObject(
			CppUtils::String const& connectstring, 
			CppUtils::String const& runName,
			CppUtils::String const& comment
		) = 0;


		// this resolves session object via UID wich uniquesly identifies the session
		// otherwise throws Exception
		virtual BrokerSession& resolveSessionObject(CppUtils::Uid const& uid) = 0;

		// returns available sessions for that broker 
		virtual void getAvailableSessions(CppUtils::UidList& activeSesionsUids) const = 0;

	
		// remove session object
		// as this must be done here inside the library
		// after this call the pointer to BrokerSession becomes invalid
		virtual void deleteSessionObject(
			BrokerSession& obj, 
			CppUtils::String const& runName
		) = 0;

		//initializes sesison object 
		// this is to be called inside <onDataArrive> handler just to connect/resolve to defined broker
		// broker must itself support a number of sessions if multy-sesion trading is expected
		// it gets the parameters defined in session object and make a connection
		virtual void connectToBroker(
			BrokerSession& session, 
			CppUtils::String const& runName
		) = 0;

		// this is to be called inside <onDataArrive> handler just to disconnect defined broker
		// according the parameters defined in session object
		virtual void disconnectFromBroker(
			BrokerSession& session, 
			CppUtils::String const& runName
		) = 0;

		// issues order against broker. It returns immediately returning false if cannot pass order because of any 
		// kind of severe problem, otherwise it returns true
		// generally you must check the status of response only via onBrokerResponseArrived event
		// as event can be asynchronious, position can be splitted, whatever...
		virtual bool unblockingIssueOrder(BrokerSession &session, Order const& order) = 0;

		// returns current tick information
		// passing the pair of symbols
		virtual bool getTickInfo(
			BrokerSession &session, 
			CppUtils::String const& symbolNum, 
			CppUtils::String const& symbolDenom
		) = 0;

		
		// subscribe for nes retreival
		virtual bool subscribeForNews(
			BrokerSession &session
		) = 0;

		// returns last error information
		virtual bool getLastErrorInfo(
			BrokerSession &session
		) = 0;

		// subscribe for quotes - they will arrive via onBrokerResponseArrived
		virtual bool subscribeForQuotes(
			BrokerSession &session,
			CppUtils::String const& symbolNum, 
			CppUtils::String const& symbolDenom
		) = 0;

		// returns the history from the server
		virtual bool getHistoryInfo(
			BrokerSession &session,
			CppUtils::String const& symbolNum, 
			CppUtils::String const& symbolDenom,
			HlpStruct::AggregationPeriods aggrPeriod,
			int const multfactor
		) = 0;

		

		// The functions returning the broker statuses returns the references to the structures
    // this should be implemented that different threads may access this concurrently
		// even with the same BrokerSession object e.i we could trade from different 
		// threads as our server may operate in that way
		// you need to generate exception if want to preserve trading off one sesion from one thread

		// please note that this is server history in contradiction in what you may collect 
		// via onBrokerResponseArrived calls
		// this function must start retreival of responsed via onBrokerResponseArrived call
		// thus clent must wait for asynch response
		virtual void getBrokerResponseList(BrokerSession &session) const = 0;

		// this is the version of previos function
		// filtering contained responses from the server
		// note that this function returns what is on the server
		// may not match with the list retreived via onBrokerResponseArrived
		// this function must start retreival of responsed via onBrokerResponseArrived call
		// thus clent must wait for asynch response
		virtual void getBrokerResponseList(BrokerSession &session, OperationResult const or) const = 0;
		
		// returns current position list for seleceted session
		// only PositionList will provide actual information about your current balance
		// history of trading and other information
		// note that position list is expected to be contained in database tables 
		virtual void getBrokerPositionList(int const posStateQuery, BrokerSession &session, PositionList &posList) = 0;


		// there is a special broker event  - HlpStruct::EventData::ET_BrokerEvent & remote_customEvent
		// which will go to derived
		virtual void onCustomBrokerEventArrived(
			HlpStruct::CallingContext& callContext,
			CppUtils::String &customOutputData
		) = 0;

		

public:
		// if thread started and finished
		virtual void onThreadStarted(
			bool const firstLib, bool const lastLib
		) = 0;

		virtual void onThreadFinished(
			HlpStruct::TradingParameters const& tradeParams, bool const firstLib, bool const lastLib
		) = 0;

		// for backtest engines this call will pass RT data 
		// this is done BEFORE the same data goes to algorithm library
		// this is done in the same thread as algorithm library works
		// real brokers must raise exception here
		virtual void onDataArrived(	HlpStruct::RtData const& rtdata ) = 0;

		// whether the library supports - e.i whether we need to call onDataArrived 
		virtual bool isBacktestLibrary() const = 0;

		// this function is called at the beginning of each backtest
		// and may get some usefull parameters
		virtual void initializeBacktest(BrkLib::SimulationProfileData const& simulProfileData, ProviderSymbolCookieMap const& providerSymbolCookieMap ) = 0;

		// the second version
		virtual void initializeBacktest(BrkLib::SimulationProfileData const& simulProfileData ) = 0;

	public:

		//register delayed order - thread safe
		void registerDelayedOrder(
			BrokerSession &session, 
			DelayedOrderType const type, 
			double const& execPrice, 
			Order const& order,
			CppUtils::String const& comment
		);

		// remove delayed order 
		// note that when executed the order is removed from the list
		// retursn true if order exists - thread safe
		bool removeDelayedOrder(BrokerSession &session, CppUtils::Uid const& uid);

		// thread safe
		void removeAllDelayedOrders(BrokerSession &session);

		// this must be called externally from the layer having prices
		// to check if it is ready for execution
		void onDelayedOrderExecutionCheck(
			BrokerSession& session, 
			double const& currentPrice, 
			CppUtils::String const& provider,
			CppUtils::String const& symbol
		);

		void setUpMode(int mode);

		inline int getMode() const
		{
			return brokerMode_m;
		}

		inline bool useOnlyOpenPosCommands() const
		{
			return !(brokerMode_m & BM_USE_CLOSE_POS_COMMANDS);
		}
	protected:
		

		// this is event generated by broker (if any) that broker event arrived
		// event function called by broker layer 
		// passing current HlpStruct::AlgBrokerDescriptor - that may change asynchroniously
		// note that this function can be called either from current therad when using backtest simulator
		// as ...IssueOrder() will, call internalyy this event
		// but actual brokers may call this from separate thread
		
		// ! note that this call must be used internally in implemented code
		// it is delegated to algorithm library
		
		// ! note one must check the actual type of BrokerResponse
		// then cast to real type 
		void onBrokerResponseArrived(
			BrokerSession &session,
			BrokerResponseBase const& bresponse
		);

		
		inline Inqueue::AlgorithmHolder const& getAlgHolder() const
		{
			return algHolder_m;
		}
		
		inline Inqueue::AlgorithmHolder& getAlgHolder()
		{
			return algHolder_m;
		}
		

		// some event generation functions
		void sendSessionCreateEvent(BrokerSession const& session, CppUtils::String const& runName, CppUtils::String const& comment);

		void sendSessionConnectEvent(BrokerSession const& session, CppUtils::String const& runName);

		void sendSessionDisconnectEvent(BrokerSession const& session, CppUtils::String const& runName);

		void sendSessionDestroyEvent(BrokerSession const& session, CppUtils::String const& runName);


	private:

		
		// REFRENCE TO ALGORITHM HOLDER object
		Inqueue::AlgorithmHolder& algHolder_m;

	
		// reference to event sender
		Inqueue::OutProcessor &outproc_m;

	
		// map contaning session pointers for remote management
		// it contains sessions that are establisehd via _RP functions
		map<CppUtils::Uid, BrokerSession*> remoteSessions_m;

		map<CppUtils::Uid, DelayedOrder> delayedOrders_m;

		CppUtils::Mutex delayedOrdersMtx_m;

		int brokerMode_m;



	};
};




#endif