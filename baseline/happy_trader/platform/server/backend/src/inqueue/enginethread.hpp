#ifndef _BACKEND_INQUEUE_ENGINETHREAD_INCLUDED
#define _BACKEND_INQUEUE_ENGINETHREAD_INCLUDED

#include "inqueuedefs.hpp"
#include "commontypes.hpp"
#include "algholder.hpp"
#include "engine.hpp"
#include "aholder.hpp"
#include "tickgenerator.hpp"
#include "aggregatedcache.hpp"


namespace Inqueue {
	
	class SimulHistoryContext;
	class InQueueController;
	
	struct EventDataHelper
	{
		EventDataHelper():
			broker_seq_num_m(-1)
		{
		}

		EventDataHelper(HlpStruct::EventData const& event_i, int const broker_seq_num):
			event_m(event_i),
			broker_seq_num_m(broker_seq_num)
		{
		}

		HlpStruct::EventData event_m;

		int broker_seq_num_m;
	};

	/**
	 Helper class that incapsulates engine thread.
	*/

	class EngineThread: public CppUtils::Mutex {
		friend class Engine;
		friend class ContextAccessor;
		friend class SimulationCollector;

	public:

		// starts processing therad subscribing a number of algorithm/broker pairs 
		EngineThread(Engine& engine, map<int, AlgorithmHolder*> algorithmholderlist, map<CppUtils::String, HlpStruct::TradingParameters> const& tradingParameters);

		// stop processing thread freeing all the resources
		~EngineThread();

		// the function implementing therad execution
		// status must be mutexed to be checked inside thread
		void threadRun();

		// retur current therad id
		inline int getThreadId() const
		{
			return id_m;
		}

		inline AggregatedCache& getAggregatedCache()
		{
			return aggregatedCache_m;
		}
 
		// caled from Quote processor main thread
		void pushRtData(HlpStruct::RtData & rtdata, bool const& tickerInsideExistingTime);

		// propagate level 2 data
		void pushLevel2Data(HlpStruct::RtLevel2Data& level2data);

		// propagate level 1 data
		void pushLevel1Data(HlpStruct::RtLevel1Data& level1data);

		// event data
		// if we send event to broker - use broker_seq_num = 1 or 2 otherwise no matter
		void pushEventData(int const broker_seq_num, HlpStruct::EventData const& eventdata);


		// returns the size of the queue
		bool isInputQueueEmpty() const;

		// return sthe size of input queue
		int inputQueuePendingSize() const;


		void pushRtDataHelper(HlpStruct::RtData & rtdata);

		

		// function that forces ant consequent calls of cache symbol to re-read 
		// memory cache 
		void forceToReReadChacheFromFile();

		// resolve what is in memory cache
		void checkSymbolCached(
			CppUtils::String const& provider, 
			CppUtils::String const& symbol, 
			HlpStruct::AggregationPeriods const period,
			int const multfactor,
			int &cookie,
			double & beginData,
			double & endData
		);

		// update trading parameters
		void updateTradingParameters(CppUtils::String const& algBrkName, HlpStruct::TradingParameters const& parameters);

		// returns trading parameters
		HlpStruct::TradingParameters getTradingParameters(CppUtils::String const& algBrkName) const;
	
	private:

		// helpers to switch on and off RT modes for the thread
		void forceThreadToRtMode(
			CppUtils::String const& runName, 
			CppUtils::String const& runComment,
			BrkLib::SimulationProfileData const& simProfile
		);

		
		void forceThreadToLazyMode();

		// force thread to simulation mode
    void forceThreadToSimulation(
			CppUtils::String const& runName,
			CppUtils::String const& runComment,
			bool singleSimRun,
			CppUtils::Uid& simulationHistoryUid
		);

		// stops simulation forcibly
		void stopSimulationForcibly();

		// simulation
		void simulationHelper();

		// simulation - this is the new version
		void simulationHelper2();

		
		// this passes event data to matching broker library
		// generally that broker could handle manuall requests
		// with orders
		void processAlgorithmBrokerInternalEvent(
			int const broker_seq_num,
			AlgorithmHolder* algHolder, 
			ContextAccessor& contextaccessor, 
			HlpStruct::EventData const &eventdata
		);

		// process level 2 data
		void processLevel2Data(
			AlgorithmHolder* algHolder, 
			ContextAccessor& historyaccessor, 
			HlpStruct::RtLevel2Data const& level2data,
			HlpStruct::TradingParameters const& tradeParams,
			CppUtils::String const& runName,
			CppUtils::String const& runComment
		);

		// process level 2 data
		void processLevel1Data(
			AlgorithmHolder* algHolder, 
			ContextAccessor& historyaccessor, 
			HlpStruct::RtLevel1Data const& level1data,
			HlpStruct::TradingParameters const& tradeParams,
			CppUtils::String const& runName,
			CppUtils::String const& runComment
		);
		

		// delegates simulation call
	inline void processSimulationTick(
		HlpStruct::RtData const& rtdata, 
		ContextAccessor& historyaccessor,
		HlpStruct::TradingParameters const& tradeParams,
		AlgorithmHolder* algHolderPtr
	);


	inline void processSimulationTickUpperLevel (
		HlpStruct::RtData const& rtdata, 
		AggregatedCache& simCache,
		bool const& isFirst,
		bool const& isLast,
		HlpStruct::TradingParametersList& traderParamsCache,
		vector<AlgorithmHolder*>& algHolderCache
	);

	// delegates RT call
	inline void processRTTick(
		AlgorithmHolder* aholder,
		HlpStruct::RtData const& rtdata, 
		ContextAccessor& historyaccessor, 
		HlpStruct::TradingParameters const& tradeParams,
		CppUtils::String const& runName,
		CppUtils::String const& runComment
	);

	

	void initTradeParamCacheAndBtest(
			BrkLib::SimulationProfileData const& simulationProfileStorage,
			BrkLib::BrokerBase::ProviderSymbolCookieMap const& providerSymbolCookieMap,
			HlpStruct::TradingParametersList& traderParamsCache,
			vector<AlgorithmHolder*>& algHolderCache
	);

	

	void process_Event_OnThreadStarted(
			AlgorithmHolder* aholder,
			ContextAccessor& historyaccessor,
			bool const firstLib,
			bool const lastLib
	);

	void process_Event_OnThreadFinished(
			AlgorithmHolder* aholder,
			ContextAccessor& historyaccessor,
			HlpStruct::TradingParameters const& tradeParams,
			bool const firstLib,
			bool const lastLib
	);


	
	// 
	CppUtils::Uid  getLastSimulationUid() const;
	


	HlpStruct::GlobalStorage& getThreadGlobalStorage();

				

private:
	
		
				
		CppUtils::String  getStringId() const;
		
		CppUtils::Thread* thread_m;

		Engine& engine_m;
		
		// alogorithms used for this thread
		map<int , AlgorithmHolder*> algorithmHolderList_m;

		ThreadStatus status_m;

		long id_m;

		//trading parameters

		CppUtils::Mutex tradingParametersMtx_m;
		// by default it gets load trading parameters but can be changed later
		map<CppUtils::String, HlpStruct::TradingParameters> tradingParameters_m;
		

		// received RT data
		//HlpStruct::RtDataQueue receivedData_m; 
		CppUtils::FifoQueue<HlpStruct::RtData, 200> receivedData_m;

		// level 2 data
		CppUtils::FifoQueue<HlpStruct::RtLevel2Data, 200> receivedLevel2Data_m;

		// level 2 data
		CppUtils::FifoQueue<HlpStruct::RtLevel1Data, 200> receivedLevel1Data_m;

		

		// event data queue - expecting small load so very tine steps
		CppUtils::FifoQueue<EventDataHelper, 10> eventData_m;

		// whether RT  data arrived
		CppUtils::Event dataArrived_m;

		// level 2 data arrived
		CppUtils::Event level2DataArrived_m;

		// level 1 data arrived
		CppUtils::Event level1DataArrived_m;

		// whether event arrived
		CppUtils::Event eventArrived_m;

		// whether need to shutdown
		CppUtils::Event shutDownThread_m;

		// whether need to push the state to simulation
		CppUtils::Event startSimulation_m;

	
		// history context
		HistoryContext& hstContext_m;

		// this is global aggregated cache responsible for aggregation
		AggregatedCache aggregatedCache_m;

	

		// this is when we need only history access and do our activity during single simulation run
		bool doSingleSimulationCall_m;

		// history UID if used with simulation
		CppUtils::Uid simulationHistoryUid_m;


				

		// data storage for thread <onDataArrived> functions
		HlpStruct::GlobalStorage globalThreadStorage_m;

		
		// event whether therad started
		CppUtils::Event threadStarted_m;

		// event whether thread initialized - while fires after thread goes from TH_Initializing to TH_Lazy
		CppUtils::Event threadInitialized_m;

		// event just to stop simulation
		CppUtils::Event stopSimulation_m;

		// parameter stating the name (RT or Simulation) run that is reflected in session list
		CppUtils::String runName_m;

		// comment accompanying run name
		CppUtils::String runComment_m;

		InQueueController& inqueueController_m;

				 
	};




	/**
	Collector class just to real ALL data from history context accessor
	*/

	
	class SimulationCollector
	{
	public:
		SimulationCollector(
			AggregatedCache& simCache, 
			HlpStruct::TradingParametersList& traderParamsCache, 
			vector<AlgorithmHolder*>& algHolderCache,
			Inqueue::HistoryContext& simhistcontext,
			EngineThread& engineThread
		):
				simCache_m(simCache),
				traderParamsCache_m(traderParamsCache),
				simhistcontext_m(simhistcontext),
				algHolderCache_m(algHolderCache),
				engineThread_m(engineThread)
		{
		}

		bool passRtData(HlpStruct::RtData& rtdata, bool const isFirst, bool const isLast);
		


	private:

		AggregatedCache& simCache_m;

		HlpStruct::TradingParametersList& traderParamsCache_m;

		vector<AlgorithmHolder*>& algHolderCache_m;

		Inqueue::HistoryContext& simhistcontext_m;

		EngineThread & engineThread_m;

	
	};


}; // end of namepsace

#endif

