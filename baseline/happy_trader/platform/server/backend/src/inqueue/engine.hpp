#ifndef _BACKEND_INQUEUE_ENGINE_INCLUDED
#define _BACKEND_INQUEUE_ENGINE_INCLUDED

#include "inqueuedefs.hpp"
#include "commontypes.hpp"
#include "tickgenerator.hpp"




namespace Inqueue {

	/**
	 The class is responsible for managing and providing life-cycle operations
	 with algorithms and brokers

	*/
	class EngineThread;
	class AlgorithmHolder;

	// working thread status
		// if thread does not work - it is TH_Lazy
		enum ThreadStatus {
			TH_Dummy					= 0,
			TH_Lazy						=	1,
			TH_RT							= 2,
			TH_Simulation			= 3,
			TH_Failure				= 4,
			TH_Initializing		= 5
		};

		class INQUEUE_BCK_EXP ThreadStatusToString {
		public:

			CppUtils::String operator ()(ThreadStatus const status)
			{
				if (status==TH_Dummy) return "Dummy";
				if (status==TH_Lazy) return "Lazy";
				if (status==TH_RT) return "RT";
				if (status==TH_Simulation) return "Simulation";
				if (status==TH_Failure) return "Failure";
				if (status==TH_Initializing) return "Initializing";
		
				return "";
			};
		};

		enum AlgorithmStatus {
			AS_Free = 1, // just created algorithm
			AS_RTParticipator = 2,
			AS_SimParticipator = 3
		};

	struct ThreadDescriptor
	{
		ThreadDescriptor():
			tid_m(-1),
			inputQueuePendingSize_m(0),
			tstatus_m(TH_Dummy)
		{
		}


		CppUtils::StringList loadedAlgBrkPairs_m;

		int tid_m;

		int inputQueuePendingSize_m;

		ThreadStatus tstatus_m;

	};


	class INQUEUE_BCK_EXP Engine: public CppUtils::RefCounted, public CppUtils::ProcessManager::Bootable, private CppUtils::Mutex {
		
	public:

		static CppUtils::String const &getRootObjectName ();

		Engine();

		~Engine();

		virtual void boot();

		// notifies all the running algorithms that RT data are available
		void pushRtData(HlpStruct::RtData& rtdata,bool const& tickerInsideExistingTime);

		// notifies all the running threads about arrival of level 2 data
		void pushLevel2Data(HlpStruct::RtLevel2Data& level2data);

		// notifies all the running threads about arrival of level 1 data
		void pushLevel1Data(HlpStruct::RtLevel1Data& level1data);


		// forces all the threads to update its cache from the file
		// normally after pushing RT data
		void forceThreadsToUpdateCache();


		// loads and initializes algroritm/ broker pairs
		void loadAlgorithm(
			HlpStruct::AlgBrokerDescriptor const& descriptor, 
			CppUtils::String const& session, 
			CppUtils::String const& runName,
			CppUtils::String const& runComment
		);

		// opposite to the previous
		void unloadAlgorithm(CppUtils::String const& algId );


		// launches algorithm thread, where all the subscribed algorithms will launch the specified function
		// on new data arrival in a separate therad
		// returns thread ID
		// algorithm identified by its string ID
		int startAlgorithmThread(CppUtils::StringList const& algorithms);

		// start & stop RT processing on loaded threads
		// that means trading functions are called by new RT data retreival
		void startRT(
			int const threadId, 
			CppUtils::String const& runName, 
			CppUtils::String const& runComment,
			BrkLib::SimulationProfileData const& simProfile
		);

		void stopRT(int const threadId);

		void stopHistorySimulation(int const threadId);

		// start simulation that occures in the same thread
		// it cannot be stopped, it goes until the ent of simulation
		void startHistorySimulation(
			CppUtils::String const& runName,
			CppUtils::String const& runComment,
			int const threadId, 
			bool singleSimRun,
			CppUtils::Uid & simulationHistoryUid
		);

		// checks symbol cached in memory
		void checkSymbolCached(int const threadId, 
			CppUtils::String const& provider, 
			CppUtils::String const& symbol, 
			HlpStruct::AggregationPeriods const period,
			int const multfactor,
			int &cookie,
			double & beginData,
			double & endData);
		
		// stop algorithm thread
		void stopAlgorithmThread(int const threadId);

		// return active currently working threads
		void getLoadedThreads(CppUtils::IntList& loadedThreads) const;

		// return loaded algorithms
		void getLoadedAlgorithms(CppUtils::StringList& loadedAlgorithms) const;

		// returns algorithm for the thread
		void getAlgorithmsForTheThread(int const threadId, CppUtils::StringList& threadAlgorithms) const;

		// return all parameters for all active threads
		void getLoadedThreads( vector<ThreadDescriptor>& threads ) const;

		// algorithm descriptor
		HlpStruct::AlgBrokerDescriptor  getAlgorithmDescriptor(CppUtils::String const& algId) const;

		
		// realoads trading paramaters for the defined therad
		void reloadTradingParametersForTheThread(
			int const threadId, 
			CppUtils::String const& algBrkName,
			HlpStruct::TradingParameters const& tradeParams
		);

		// returns current trading parameters for the thread
		void getTradingParametersForTheThread(
			int const threadId, 
			CppUtils::String const& algBrkName,
			HlpStruct::TradingParameters& tradeParams
		);

		// these set of functions that make a call agains defined broker in the defined thread
		void passThreadEvent(int const broker_seq_num, int const threadId, HlpStruct::EventData const& event );

		// get the queue size of teh queue
		bool isInputQueueEmpty(int const threadId) const;

		bool isAlgorithmLoaded(CppUtils::String const& algBrkName) const;

		// engine cleanup
		void performCompleteCleanup();


		
		// status of working thread
		ThreadStatus getThreadStatus(int const threadId);


		

	private:

		map<CppUtils::String, AlgorithmHolder*> loadedAlgorithms_m;

		map<int, EngineThread*> activeThreads_m;

	};

}; // end of namespace

#endif


