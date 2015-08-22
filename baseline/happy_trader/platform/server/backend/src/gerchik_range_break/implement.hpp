#ifndef _GERCHIK_RANGE_BREAK_IMP_INCLUDED
#define _GERCHIK_RANGE_BREAK_IMP_INCLUDED


#include "gerchik_range_breakdefs.hpp"
#include "action.hpp"
#include "state.hpp"
#include "event.hpp"
#include "brokercontext.hpp"
#include "signaller.hpp"


#define ALG_M_RANGE_BREAK "ALG_M_RANGE_BREAK"
#define RESPONSE_TAG_REMAIN_SHARES tag_m

#define ALL_SYMBOL_ID "-ALL-"
// -------------------------
#define LOG_EQUITY(RUN_NAME, PROVIDER, L,E) {	\
	/*DRAWOBJ_TEXT(RUN_NAME, CppUtils::getTime(),ALG_M_RANGE_BREAK, ALL_SYMBOL_ID, PROVIDER, BrkLib::AP_MEDIUM, "EQUITY", \
		" Equity: " << ((BrkLib::BrokerResponseEquity const&)E).curPositionReportProfit_m <<	\
		" Comment: " << ((BrkLib::BrokerResponseEquity const&)E).brokerComment_m \
	);*/ \
}



#define LOG_SYMBOL(RUN_NAME,PROVIDER, L,S,X) {	\
	DRAWOBJ_TEXT(RUN_NAME,CppUtils::getTime(),ALG_M_RANGE_BREAK, S, PROVIDER, BrkLib::AP_MEDIUM, "LOG", X );	\
	/*LOGEVENT( HlpStruct::CommonLog::LL_INFO, ALG_M_RANGE_BREAK, "LOG_SYMBOL - Run name: " << RUN_NAME << " provider: " << PROVIDER << \
		" short text: " << S << " content: " << X );*/	\
}

// with context
#define LOG_SYMBOL_CTX(RUN_NAME,PROVIDER,L,S,X,C) {	\
	DRAWOBJ_TEXT(RUN_NAME,CppUtils::getTime(),ALG_M_RANGE_BREAK, S, PROVIDER, BrkLib::AP_MEDIUM, "LOG_CTX", X << " CONTEXT: " << C );	\
	/*LOGEVENT( HlpStruct::CommonLog::LL_INFO, ALG_M_RANGE_BREAK, "LOG_SYMBOL_CTX - Run name: " << RUN_NAME << " provider: " << PROVIDER << \
		 " short text: " << S << " content: " << X << " context: " << C);*/	\
}

// with context from broker
#define LOG_SYMBOL_CTX_BRK(RUN_NAME,PROVIDER,L,S,X,C) {	\
	DRAWOBJ_TEXT(RUN_NAME,CppUtils::getTime(),ALG_M_RANGE_BREAK, S, PROVIDER, BrkLib::AP_MEDIUM, "LOG_CTX_BRK", X << " CONTEXT: " << C );	\
	/*LOGEVENT( HlpStruct::CommonLog::LL_INFO, ALG_M_RANGE_BREAK, "LOG_SYMBOL_CTX_BRK - Run name: " << RUN_NAME << " provider: " << PROVIDER << \
		 " short text: " << S << " content: " << X << " context: " << C);*/	\
}

#define LOG_COMMON(RUN_NAME,PROVIDER, L,X)	{		\
	DRAWOBJ_TEXT(RUN_NAME,CppUtils::getTime(),ALG_M_RANGE_BREAK, ALL_SYMBOL_ID, PROVIDER, BrkLib::AP_MEDIUM, "LOG", X );	\
	/*LOGEVENT( HlpStruct::CommonLog::LL_INFO, ALG_M_RANGE_BREAK, "LOG_COMMON - Run name: " << RUN_NAME << " provider: " << PROVIDER << " content: " << X );*/\
}

#define LOG_PRINT(RUN_NAME, PROVIDER, SYMBOL, BID, ASK, VOLUME, COLOR)	\
	/*DRAWOBJ_PRINT(RUN_NAME, CppUtils::getTime(), ALG_M_RANGE_BREAK, SYMBOL, PROVIDER, BID, ASK, VOLUME, COLOR );*/

#define LOG_LEVEL1(RUN_NAME, PROVIDER, SYMBOL, BEST_BID_PRICE, BEST_BID_VOLUME, BEST_ASK_PRICE, BEST_ASK_VOLUME )	\
	/*DRAWOBJ_LEVEL1(RUN_NAME, CppUtils::getTime(), ALG_M_RANGE_BREAK, SYMBOL, PROVIDER, BEST_BID_PRICE,BEST_BID_VOLUME, BEST_ASK_PRICE, BEST_ASK_VOLUME)*/




namespace AlgLib {

	
	// ------------------

	
	class GERCHIK_RANGE_BREAK_EXP MRangeBreak: public AlgorithmBase {

	public:

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

		// ctor & dtor
		MRangeBreak( Inqueue::AlgorithmHolder& algHolder);

		virtual ~MRangeBreak();
		
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

		virtual void onEventArrived(
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
		//void generateNewEvent(BrkLib::BrokerConnect& brconnect, TradingEventList& elist, HlpStruct::RtLevel1Data const& level1data);

		/*
		void generateNewEvent(
			BrkLib::BrokerConnect& brconnect, 
			Inqueue::ContextAccessor& historyaccessor, 
			Inqueue::TradingParameters const& descriptor,
			TradingEventList& elist, 
			HlpStruct::RtData const& rtdata
		);
		*/

		// generate new states if any
		void machineEventsReaction(BrkLib::BrokerConnect& brconnect, TradingEventList const& elist);

		// called from the upper if satte is changed
		void onStateChanged(BrkLib::BrokerConnect& brconnect, TradingState const& oldState, TradingState& newState, TradingEvent const& cause_event);

	

		// this is global function to do actions
		void doAction(BrkLib::BrokerConnect& brconnect, Action const& action);


		BrokerContext& getBrokerContext(CppUtils::String const& symbol);

		void updateBrokerContext(CppUtils::String const& symbol, BrokerContext const& context);


		void pushProcessingEvent(BrkLib::BrokerConnect const& brkConnect, TradingEventList const& eventList);
		
		// this returns the number of securities in progress - e.i. when we want to limit our operations per one stock per time
		int getSecuritiesNumberInProgress() const;

		// if arrived time is within working time
		bool isWithinWorkingTime(double const &ttime) const;
		
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


		
		
		// list of symbols for RT - from laser
		CppUtils::StringSet symbolLevel1_rt_m;

					
		// this is provider containing RT stocks - laser provider
		CppUtils::String providerForRTSymbols_m;

		
		
		
		// order method and place
		CppUtils::String order_method_m;

		CppUtils::String order_place_m;

		/*	
		map<CppUtils::String, HlpStruct::RtLevel1Data > level1Data_m;

		// current level 1 data
		map<CppUtils::String, HlpStruct::RtLevel1Data > currentLevel1Data_m;

		CppUtils::Mutex currentLevel1DataMtx_m;
		*/

		// rt data
		map<CppUtils::String, HlpStruct::RtData > rtData_m;


		// the above data must to be mutexed
		CppUtils::Mutex priceRTDataMtx_m;

		CppUtils::Mutex priceLevel1DataMtx_m;


		// this class contains structures containing parameters fro orders
		// e-i calculated signal context
		map<CppUtils::String, Signaller> signalStructure_m;

		// mutex for signal structure
		CppUtils::Mutex signalStructureMtx_m;

		// the machine state

		map<CppUtils::String,TradingState> state_m;

		map<CppUtils::String,BrokerContext> brokerContext_m;

		CppUtils::Mutex brokerContextMtx_m;

		// mutex securing work on processing event. state change, etc...
		CppUtils::Mutex jobMutex_m;

		CppUtils::String internalRun_name_m;

		
		// parameters
		// volume
		//double SHARES_m;

		// if singleton
		double CONCURRENT_TRADE_SECURITIES_m;

		// bollinger signals
		double BOLLINGER_PERIOD_m;

		double BOLLINGER_STDDEV_MULT_m;

		// this is a suspect to enter
		double BOLLINGER_ENTER_CRITERIA_m;

		// path to export
		//CppUtils::String pathToExport_m;

		double unixTimeBeginTrade_m;

		double unixTimeEndTrade_m;

		int hoursBegin_m;
		
		int minutesBegin_m;

		int hourEnd_m;
		
		int minutesEnd_m;
				
	};


}; // end of namespace

#endif