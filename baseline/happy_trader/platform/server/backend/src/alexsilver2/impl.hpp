#ifndef _ALEXSILVER2_IMP_INCLUDED
#define _ALEXSILVER2_IMP_INCLUDED

#include "../brklib/brklib.hpp"
#include "alexsilver2defs.hpp"


// Silver algorithm machine states
// algorithm is expected to manage only one position
#define STATE_UNDEFINED				0 // state undefined - machine cannot be used
#define STATE_NO_OPEN_POS			1 // no open pos

#define STATE_SHORT_ASKED_INSTALL			2 // we issued short limit order, but did not get confirmation it is installed
#define STATE_LONG_ASKED_INSTALL			3 // we issued long limit order, but did not get confirmation it is installed

#define STATE_SHORT_INSTALLED		4 // we got confirmation short limit is installed
#define STATE_LONG_INSTALLED		5 // we got confirmation long limit is installed

#define STATE_SHORT_EXECUTED	6 // we got confirmation short limit is executed - e.i position opened
#define STATE_LONG_EXECUTED		7 // we got confirmation long limit is executed - e.i position opened

#define STATE_SHORT_ASKED_MODIFY 8 // we asked short order to be modified
#define STATE_LONG_ASKED_MODIFY 9 //we asked long order to be modified

#define STATE_SHORT_ASKED_CLOSE 10 // we asked short order to be closed - order was alredy executed
#define STATE_LONG_ASKED_CLOSE 11 //we asked long order to be closed - order was already executed

#define STATE_SHORT_ASKED_CANCEL 12 // we asked short limit order to be closed
#define STATE_LONG_ASKED_CANCEL 13 //we asked long limit order to be closed

#define STATE_SHORT_EXECUTED_NOLOSS 14 // when we moved order to noloss level - no need to do this again
#define STATE_LONG_EXECUTED_NOLOSS 15 // when we moved order to noloss level - no need to do this again

#define STATE_SHORT_ASKED_TO_ADOPT_EXEC_SL_TP_ON_INSTALLED_STATE 16 // when we asked to adopt exec price or TP or SL on installed state
#define STATE_LONG_ASKED_TO_ADOPT_EXEC_SL_TP_ON_INSTALLED_STATE 17 // when we asked to adopt exec price or TP or SL on installed state

#define STATE_SHORT_ASKED_TO_ADOPT_SL_TP_ON_EXECUTED_STATE 18 // when we asked to adopt exec price or TP or SL on executed state
#define STATE_LONG_ASKED_TO_ADOPT_SL_TP_ON_EXECUTED_STATE 19 // when we asked to adopt exec price or TP or SL on executed state

#define STATE_SHORT_ASKED_TO_ADOPT_TP_ON_EXECUTED_NOLOSS_STATE 21 // when we asked to adopt exec price or TP or SL on executed noloss state
#define STATE_LONG_ASKED_TO_ADOPT_TP_ON_EXECUTED_NOLOSS_STATE 22 // when we asked to adopt exec price or TP or SL on executed noloss state


// Silver algorithm machine events
#define EVENT_TICK_CHECK_NO_SIGNAL								0
#define EVENT_TICK_CHECK_PRICE_MORE_KIJUN					1  // whether the current price is more  kijun
#define EVENT_TICK_CHECK_PRICE_LESS_KIJUN					2  // whether the current price is less kijun
#define EVENT_PERIOD_CHECK_NO_SIGNAL							3  // whether at the end of period no signal
#define EVENT_PERIOD_CHECK_BUY_LIMIT							4  // ... BUY LIMIT SIGNAL
#define EVENT_PERIOD_CHECK_SELL_LIMIT							5	 // ... SELL_LIMIT_SIGNAL

#define EVENT_BROKER_RESP_SHORT_ORDER_INSTALLED		6	 // if broker response confirmed order is installed
#define EVENT_BROKER_RESP_LONG_ORDER_INSTALLED		7	 // if broker response confirmed order is installed

#define EVENT_BROKER_RESP_SHORT_ORDER_EXECUTED		8	 // if broker resposne confirmed order is executed
#define EVENT_BROKER_RESP_LONG_ORDER_EXECUTED			9	 // if broker resposne confirmed order is executed

#define EVENT_BROKER_RESP_SHORT_ORDER_MODIFIED		10	 // if broker resposne confirmed order is modified
#define EVENT_BROKER_RESP_LONG_ORDER_MODIFIED			11	 // if broker resposne confirmed order is modified

#define EVENT_BROKER_RESP_SHORT_ORDER_CLOSED			12	 // if broker resposne confirmed order was closed
#define EVENT_BROKER_RESP_LONG_ORDER_CLOSED				13	 // if broker resposne confirmed order was closed

#define EVENT_BROKER_RESP_SHORT_ORDER_CANCELLED		14	 // if broker resposne confirmed order was cancelled
#define EVENT_BROKER_RESP_LONG_ORDER_CANCELLED		15	 // if broker resposne confirmed order was cancelled

#define EVENT_BROKER_RESP_NO_SIGNAL								16		// whether at broker response no signal
#define EVENT_BROKER_REJECTED_ORDER_OPER					17    // any operations with broker failed 

#define EVENT_ADOPT_EXECPRICE_SL_TP_ORDER_OPER		18    // upon each period finish we changing (if applicable) execprice, tp, sl - if this applicable to the state


// Silver Channel state machine actions
#define ACTION_ASK_FOR_SHORT_LIMIT_ORDER_INSTALL						1 // ask for order installation
#define ACTION_ASK_FOR_LONG_LIMIT_ORDER_INSTALL							2

#define ACTION_ASK_FOR_SHORT_ORDER_MODIFY_SETNOLOSS		3 // ask for modification of noloss of executed orders
#define ACTION_ASK_FOR_LONG_ORDER_MODIFY_SETNOLOSS		4

#define ACTION_ASK_FOR_SHORT_ORDER_CLOSE							5 // ask for order closure, they are executed
#define ACTION_ASK_FOR_LONG_ORDER_CLOSE								6

#define ACTION_ASK_FOR_SHORT_LIMIT_ORDER_CANCEL							7 // ask for order cancellation
#define ACTION_ASK_FOR_LONG_LIMIT_ORDER_CANCEL							8

#define ACTION_ASK_FOR_SHORT_ADOPT_EXEC_TP_SL_ORDER					9 // if applicable force order to change execprice, SL, TP
#define ACTION_ASK_FOR_SHORT_ADOPT_TP_SL_ORDER							10 // when  order is executed adopt sl and tp
#define ACTION_ASK_FOR_SHORT_ADOPT_TP_ORDER									11 // when order is executed in noloss adopt tp

#define ACTION_ASK_FOR_LONG_ADOPT_EXEC_TP_SL_ORDER					12 // if applicable force order to change execprice, SL, TP
#define ACTION_ASK_FOR_LONG_ADOPT_TP_SL_ORDER							13 // when  order is executed adopt sl and tp
#define ACTION_ASK_FOR_LONG_ADOPT_TP_ORDER									14 // when order is executed in noloss adopt tp

// Silver Channel constants
#define SILVCH 38.2
#define SKYCH 23.6
#define ZENCH 0.0
#define FUTCH 61.8



// order comment that detects orders set up by this dll
#define ORDER_COMMENT "E174C312-086F-472E-8FF0-92D2D53BABF4"


namespace AlgLib {
	
	class ALEXSILVER2_EXP SilverImpl: public AlgorithmBase {
		
	public:

		
		// ctor & dtor
		SilverImpl( Inqueue::AlgorithmHolder& algHolder);

		virtual ~SilverImpl();
		
		// intreface to be implemented
		virtual void onLibraryInitialized(
			Inqueue::AlgBrokerDescriptor const& descriptor, 
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		);

		virtual void onLibraryFreed();

		// this will be called only 
		virtual void onBrokerResponseArrived(
			BrkLib::BrokerConnect &brkConnect,
			BrkLib::BrokerResponseBase const& bresponse
		);


		// note that this is called from any thread which is subscribed as algrorith thread
		// 

		virtual void onDataArrived(
			HlpStruct::RtData const& rtdata, 
			Inqueue::ContextAccessor& historyaccessor, 
			Inqueue::TradingParameters const& descriptor,
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		);

		virtual void onLevel2DataArrived(
			HlpStruct::RtLevel2Data const& level2data,
			Inqueue::TradingParameters const& descriptor,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		);
	
	
	
		virtual bool onEventArrived(HlpStruct::EventData const &eventdata, HlpStruct::EventData& response);


		virtual CppUtils::String const& getName() const;

	private:
		

		// sends handshake order
		void sendHandshakeOrder(BrkLib::BrokerConnect& brkConnect);

		void sendMetainfoOrder(BrkLib::BrokerConnect& brkConnect);

		// recalculate silver parameters
		bool recalculateSilverParameters(int symbol_id, Inqueue::ContextAccessor& historyaccessor, HlpStruct::RtData const& rtdata );

		// whether parameters are initialized to recalculate it
		bool isSilverparametersInitialized(Inqueue::ContextAccessor& historyaccessor);

		// generate signal according to re-calculated data upon current period finish
		int generatePeriodFinishedSignal(Inqueue::ContextAccessor& historyaccessor, BrkLib::BrokerConnect& brkConnect);

		// generate signal upon each tick 
		int generateEachTickSignal(Inqueue::ContextAccessor& historyaccessor, HlpStruct::RtData const& rtdata, BrkLib::BrokerConnect& brkConnect);

		// process event
		void processEvent(int const event, BrkLib::BrokerConnect& brkConnect);

		// checks for automat state
		void initialCheckState(Inqueue::ContextAccessor& historyaccessor, BrkLib::BrokerConnect& brkConnect);

		// generates signal upon broker response arrived
		int generateBrokerResponseSignal(BrkLib::BrokerResponseOrder const& brespOrder, BrkLib::BrokerConnect& brkConnect);
		
		// do machine actions
		// all relevant data must be contained in session variable
		void doMachineAction(BrkLib::BrokerConnect& brkConnect, int actionId);

		// alerts 
		void doAlertOnEachTick( CppUtils::String const& runName, Inqueue::ContextAccessor& historyaccessor, HlpStruct::RtData const& rtdata);

		void doAlertEachPeriod( CppUtils::String const& runName, Inqueue::ContextAccessor& historyaccessor, HlpStruct::RtData const& rtdata );

	private:
		
		CppUtils::Mutex eventProcessingMtx_m;
		
		// passed as parameter
		Inqueue::AggregationPeriods aggrPeriod_m;

		// mult factor
		int multFactor_m;

		// current position ID
		CppUtils::String posId_m;

		// trading symbol
		CppUtils::String tradeSymbol_m;


		// trading provider
		CppUtils::String tradeProvider_m;


		// silver channel parameters
		double tenkan_m;
		double kijun_m;
		double senkou_m;

		// to calculate channels
		int ssp_m;
		int tsp_m;

		// profit loss ratio
		double prof_loss_ratio_m;

		// parameter of Silver Channel wneh settins SL
		// the value is measured in real price units 
		double stop_loss_pips_m;

		// expected spread
		// the value is measured in real price units
		// TODO
		double spread_m;

		// trading symbol
		CppUtils::String tradeSymbolNum_m;

		CppUtils::String tradeSymbolDenom_m;

		// position volume
		double posVolume_m;

		// just started
		bool justStarted_m;

		// parameter showing when price is observed to be close to target price
		double priceCloseLevel_m;

		// the amount of bars to be pre-loaded
		long preloadHistoryData_m;

	
		
	};
};

#endif
