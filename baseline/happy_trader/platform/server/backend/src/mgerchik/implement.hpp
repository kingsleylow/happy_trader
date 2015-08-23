#ifndef _MGERCHIK_IMP_INCLUDED
#define _MGERCHIK_IMP_INCLUDED

#include "../brklib/brklib.hpp"
#include "mgerchikdefs.hpp"
#include "slogger.hpp"
#include "event.hpp"
#include "state.hpp"
#include "action.hpp"
#include "brokercontext.hpp"

#define RESPONSE_TAG_REMAIN_SHARES tag_m

/*
#define LOG_EQUITY(PROVIDER, L,E) {	\
	if (L <= logger_m.getLogLevel()) {	\
		logger_m.addAccountEntry(L, E );	\
	}	\
}

#define LOG_SYMBOL(PROVIDER, L,S,X) {	\
	if (L <= logger_m.getLogLevel()) {	\
		ostrstream sstr; sstr << X << ends; \
		logger_m.addLogEntryNoProvider(L, S, sstr.str() );	\
		sstr.freeze(false); \
	}	\
}

// with context
#define LOG_SYMBOL_CTX(PROVIDER, L,S,X,C) {	\
	if (L <= logger_m.getLogLevel()) {	\
		ostrstream sstr;  sstr << X << ends; \
		ostrstream sstr1; sstr1 << C << ends; \
		logger_m.addLogEntryNoProvider(L,S, sstr.str(), sstr1.str() );	\
		sstr.freeze(false); \
		sstr1.freeze(false); \
	}	\
}

#define LOG_COMMON(PROVIDER, L,X)	{		\
	if (L <= logger_m.getLogLevel()) {	\
		ostrstream sstr; sstr << X << ends; \
		logger_m.addLogEntry(L,sstr.str() );	\
		sstr.freeze(false);  \
	}	\
}

#define LOG_PRINT(PROVIDER, SYMBOL, BID, ASK, VOLUME, COLOR)	\

#define LOG_LEVEL1(PROVIDER, SYMBOL, BEST_BID, BEST_BID_VOLUME, BEST_ASK, BEST_ASK_VOLUME )	\

*/
// -------------------------
#define LOG_EQUITY(RUN_NAME, PROVIDER, L,E) {	\
	DRAWOBJ_TEXT(RUN_NAME, CppUtils::getTime(),"GERCHIK_DRO", "-ALL-", PROVIDER, BrkLib::AP_MEDIUM, "EQUITY", \
		" Equity: " << ((BrkLib::BrokerResponseEquity const&)E).curPositionReportProfit_m <<	\
		" Comment: " << ((BrkLib::BrokerResponseEquity const&)E).brokerComment_m \
	); \
}


#define LOG_SYMBOL(RUN_NAME,PROVIDER, L,S,X) {	\
	DRAWOBJ_TEXT(RUN_NAME,CppUtils::getTime(),"GERCHIK_DRO", S, PROVIDER, BrkLib::AP_MEDIUM, "LOG", X );	\
}

// with context
#define LOG_SYMBOL_CTX(RUN_NAME,PROVIDER,L,S,X,C) {	\
	DRAWOBJ_TEXT(RUN_NAME,CppUtils::getTime(),"GERCHIK_DRO", S, PROVIDER, BrkLib::AP_MEDIUM, "LOG_CTX", X << " CONTEXT: " << C );	\
}

// with context from broker
#define LOG_SYMBOL_CTX_BRK(RUN_NAME,PROVIDER,L,S,X,C) {	\
	DRAWOBJ_TEXT(RUN_NAME,CppUtils::getTime(),"GERCHIK_DRO", S, PROVIDER, BrkLib::AP_MEDIUM, "LOG_CTX_BRK", X << " CONTEXT: " << C );	\
}

#define LOG_COMMON(RUN_NAME,PROVIDER, L,X)	{		\
	DRAWOBJ_TEXT(RUN_NAME,CppUtils::getTime(),"GERCHIK_DRO", "-ALL-", PROVIDER, BrkLib::AP_MEDIUM, "LOG", X );	\
}

#define LOG_PRINT(RUN_NAME, PROVIDER, SYMBOL, BID, ASK, VOLUME, COLOR)	\
	DRAWOBJ_PRINT(RUN_NAME, CppUtils::getTime(), "GERCHIK_DRO", SYMBOL, PROVIDER, BID, ASK, VOLUME, COLOR );

#define LOG_LEVEL1(RUN_NAME, PROVIDER, SYMBOL, BEST_BID_PRICE, BEST_BID_VOLUME, BEST_ASK_PRICE, BEST_ASK_VOLUME )	\
	DRAWOBJ_LEVEL1(RUN_NAME, CppUtils::getTime(), "GERCHIK_DRO", SYMBOL, PROVIDER, BEST_BID_PRICE,BEST_BID_VOLUME, BEST_ASK_PRICE, BEST_ASK_VOLUME)


// -------------------------
#define OLD_STATE_BEGIN(STATE)	\
	switch(STATE) {

#define OLD_STATE_ENTRY_BEGIN(STATE_CONST)	\
	case STATE_CONST: {

#define OLD_STATE_ENTRY_END()	\
	}	\
	break;

#define OLD_STATE_END()	\
	default: ;	\
 };	

#define OLD_STATE_ENTRY_ALL() \
	{

#define OLD_STATE_ENTRY_ALL_END()	\
	}


// --------------------

#define NEW_STATE_BEGIN(STATE)	{ \
	switch(STATE) {

#define NEW_STATE_ENTRY_BEGIN(STATE_CONST)	\
	case STATE_CONST: {

#define NEW_STATE_ENTRY_END()	\
	};	\
	break;

#define NEW_STATE_END()	\
	default: ;	\
	};	}; \

// -------------------

#define NEW_EVENT_BEGIN(EVENT)	{ \
	switch(EVENT) {

#define NEW_EVENT_ENTRY_BEGIN(EVENT_CONST)	\
	case EVENT_CONST: {

#define NEW_EVENT_ENTRY_END()	\
	};	\
	break;

#define NEW_EVENT_END()	\
	default: ;	\
};	}; \

// -----------------------------------
#define ACTION_BEGIN(ACTION)	{ \
	switch(ACTION) {

#define ACTION_ENTRY_BEGIN(ACTION_CONST)	\
	case ACTION_CONST: {

#define ACTION_ENTRY_END()	\
	};	\
	break;

#define ACTION_END()	\
	default: ;	\
};	}; \

// ----------------------------------

namespace AlgLib {

	struct EventProcessingStruct 
	{
		EventProcessingStruct(BrkLib::BrokerConnect const& brkConnect, TradingEventList const& eventList):
			brokerConnect_m(brkConnect),
			eventList_m(eventList)
		{
		}

		EventProcessingStruct()
		{
		}

		BrkLib::BrokerConnect brokerConnect_m;

		TradingEventList eventList_m;
	};

	// ------------------

	struct SymbolHelperHolder
	{
		SymbolHelperHolder():
		lastDayClose_m(-1),
		thisDayOpen_m(-1),
		dateLastDayClose_m(-1)
		{
		}

		double lastDayClose_m;

		double thisDayOpen_m;

		double dateLastDayClose_m;

		CppUtils::String symbol_m;

		CppUtils::String toString() const {

			return (CppUtils::String("[ ") + symbol_m + " ] Last Day ( "+CppUtils::getGmtTime2(dateLastDayClose_m) +" ) Close=" + CppUtils::float2String( lastDayClose_m, -1, 4) +
				" Today Open=" + CppUtils::float2String(thisDayOpen_m, -1,4));
		}
		
	};

	// ----------------------



	
	class MGERCHIK_EXP MGerchik: public AlgorithmBase {

	public:

		// ctor & dtor
		MGerchik( Inqueue::AlgorithmHolder& algHolder);

		virtual ~MGerchik();
		
		// intreface to be implemented
		virtual void onLibraryInitialized(
			Inqueue::AlgBrokerDescriptor const& descriptor, 
			BrkLib::BrokerBase* brokerlib, 
			CppUtils::String const& runName,
			CppUtils::String const& comment
		);

		virtual void onLibraryFreed();

		virtual void onDataArrived(
			HlpStruct::RtData const& rtdata, 
			Inqueue::ContextAccessor& historyaccessor, 
			Inqueue::TradingParameters const& descriptor,
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		);

		virtual void onRtProviderSynchEvent (
			Inqueue::ContextAccessor& contextaccessor,
			HlpStruct::RtData::SynchType const synchFlg,
			double const& synchTime,
			CppUtils::String const& providerName,
			CppUtils::String const& message
		);

		virtual void onLevel2DataArrived(
			HlpStruct::RtLevel2Data const& level2data,
			Inqueue::ContextAccessor& historyaccessor, 
			Inqueue::TradingParameters const& descriptor,
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		);

		virtual void onLevel1DataArrived(
			HlpStruct::RtLevel1Data const& level1data,
			Inqueue::ContextAccessor& historyaccessor, 
			Inqueue::TradingParameters const& descriptor,
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		);

		virtual void onBrokerResponseArrived(
			BrkLib::BrokerConnect &brkConnect,
			BrkLib::BrokerResponseBase const& bresponse
		);

		virtual void MGerchik::onEventArrived(
				Inqueue::ContextAccessor& contextaccessor, 
				HlpStruct::CallingContext& callContext,
				bool& result,
				CppUtils::String &customOutputData
		);
	
		virtual void onThreadStarted(
			Inqueue::ContextAccessor& contextaccessor, bool const firstLib, bool const lastLib
		);

		virtual void onThreadFinished(
			Inqueue::ContextAccessor& contextaccessor, bool const firstLib, bool const lastLib
		)
		{
		}


		virtual CppUtils::String const& getName() const;
		
		// -----------------------

	public:
		// this function implement event processing in a separate thread to avoid recursion
		void threadProcessingThread();

	private:

				
		// set of function which will generate new event
		void generateNewEvent(BrkLib::BrokerConnect& brconnect, TradingEventList& elist, HlpStruct::RtLevel1Data const& level1data);

		void generateNewEvent(BrkLib::BrokerConnect& brconnect, TradingEventList& elist, HlpStruct::RtData const& rtdata);

		// generate new states if any
		void machineEventsReaction(BrkLib::BrokerConnect& brconnect, TradingEventList const& elist);

		// called from the upper if satte is changed
		void onStateChanged(BrkLib::BrokerConnect& brconnect, TradingState const& oldState, TradingState& newState, TradingEvent const& cause_event);


		void parseFileSymbolList(CppUtils::StringSet& symbols, CppUtils::String const& fileName);

		void calculateCorrelation(int const depth, Inqueue::ContextAccessor& historyaccessor);

		// this is global function to do actions
		void doAction(BrkLib::BrokerConnect& brconnect, Action const& action);


		BrokerContext& getBrokerContext(CppUtils::String const& symbol);

		void updateBrokerContext(CppUtils::String const& symbol, BrokerContext const& context);


		void pushProcessingEvent(BrkLib::BrokerConnect const& brkConnect, TradingEventList const& eventList);
		
		// this returns the number of securities in progress - e.i. when we want to limit our operations per one stock per time
		int getSecuritiesNumberInProgress() const;

	private:

		CppUtils::Thread* thread_m;

		// event whether thread started
		CppUtils::Event threadStarted_m;

		// signal to shutdown
		CppUtils::Event shutDownThread_m;

		// the queue to extract processing data
		CppUtils::FifoQueue<EventProcessingStruct, 200> receivedProcessingData_m;

		// event signalling this arrived
		CppUtils::Event receivedProcessingDataArrived_m;

		// mtx securing that
		CppUtils::Mutex receivedProcessingDataMtx_m;


		// get info from MT broko - index name but need to find match in our precalc history list
		// which is common
		CppUtils::String convertRtIndexNameToPrecalcHistoryName(CppUtils::String const& name);

		// list of symbols - from historical data
		CppUtils::StringSet symbolsLevel1_precalc_m;

		// list of indexes for pre-claculation - from historical data
		CppUtils::StringSet indexList_precalc_m;

		// list of symbols for RT - from laser
		CppUtils::StringSet symbolLevel1_rt_m;

		// list of indexes for RT - from broko
		CppUtils::StringSet indexList_rt_m;

		// depth in correlation
		int depthCorrelation_m;

		// this is provider containing nly historical daily data- only historical
		CppUtils::String providerForPrecalcHistory_m;

		// this is provider containing RT stocks - laser provider
		CppUtils::String providerForRTSymbols_m;

		// this is for endexes - MT
		CppUtils::String providerForRTIndexes_m;

		// if we start therad we perform only precalculation (generate FILE_DATA_PRECALC)
		bool precalcOnly_m;

		// precalculation results
		CppUtils::String precalcDataFile_m;

		double SBA_m;

		double PR_m;

		// default pos volume
		double SHARES_m;

		bool SINGLETON_m;

		bool LONG_ONLY_m;

		// time out after order is open (ses)
		int TIMEOUT_AFTER_OPEN_m;

		// if open order is IOC
		bool OPEN_IOC_m;

		// if open IOC-  it will pass timeout
		int OPEN_IOC_TIMEOUT_SEC_m;

		double SLIDE_m; // 0.05

		double RISK_MIN_m; // if less then risk do not open pos

		// order method and place
		CppUtils::String order_method_m;

		CppUtils::String order_place_m;

			
		// provider here is providerForRTSymbols_m
		map<CppUtils::String, HlpStruct::RtLevel1Data > level1Data_m;

		// current level 1 data
		map<CppUtils::String, HlpStruct::RtLevel1Data > currentLevel1Data_m;

		CppUtils::Mutex currentLevel1DataMtx_m;

		// rt data
		map<CppUtils::String, HlpStruct::RtData > rtData_m;


		// the above data must to be mutexed
		CppUtils::Mutex priceRTDataMtx_m;

		CppUtils::Mutex priceLevel1DataMtx_m;



		map<CppUtils::String, SymbolHelperHolder> symbolHelperData_m;

		// the machine state

		map<CppUtils::String,TradingState> state_m;

		map<CppUtils::String,BrokerContext> brokerContext_m;

		CppUtils::Mutex brokerContextMtx_m;

		// this is the base path for logging
		CppUtils::String logBasePath_m;

		SLogger logger_m;

		// mutex securing work on processing event. state change, etc...
		CppUtils::Mutex jobMutex_m;

		CppUtils::String internalRun_name_m;

				
	};


}; // end of namespace

#endif