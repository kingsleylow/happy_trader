#ifndef _ALGLIBMT2_MTWORKERTHREAD_INCLUDED
#define _ALGLIBMT2_MTWORKERTHREAD_INCLUDED


#include "alglibmetatrader2defs.hpp"
#include "settingsmanager.hpp"
#include "mtrunner.hpp"

#define MTWORKTHREAD "MTWORKTHREAD"

namespace AlgLib {
	
	/**
	* class incapsulating worker therad
	*/

	class AlgorithmMt;
	class MTWorkerThreadManager;

	template<class T>
	class MtWorkerThreadProxy;

	class MTWorkerThread: private CppUtils::Mutex
	{
		template<class T>
		friend class MtWorkerThreadProxy;
	public:

		class CheckForRemovals_front_if {
		public:
			CheckForRemovals_front_if()
			{
			};

			bool operator()(AlgLib::GeneralCommand const& user) const
			{
				if (user.lifetime() == 0) {
					LOG(MSG_DEBUG, MTWORKTHREAD, "Removing element: [" << user.toString()<< "] because life time is 0");
					
					return true;
				}

				return false;
			}
		};

		enum States
		{
			S_DUMMY = 0,
			S_CREATED  =1,
			S_RUNNING = 2,
			S_FINISHING =3,
			S_FINISHED = 4,
			S_FAILED  =5
		};

		struct ThreadContext
		{
			ThreadContext():
				Tid(-1),
				State(MTWorkerThread::S_DUMMY),
				LastHeartBeat(-1),
				RunningOk(false),
				mtPid(-1)
			{
				ID.generate();
			};

			int Tid;
			MTWorkerThread::States State;
			double LastHeartBeat;
			bool RunningOk;
			CppUtils::Uid ID;
			int mtPid;
		};


		MTWorkerThread(CppUtils::String const& mtInstance, int const delayTime, SettingsManager const& smanager, AlgorithmMt& base);

		~MTWorkerThread();

		// start thread, throws exception
		void startWork();
		
		// void stop work, throws exception
		void stopWork();

		// run in a separate thread
		void run();

		inline CppUtils::String const& getInstance() const
		{
			return mtInstance_m;
		}


		inline AlgLib::DataQueueSerializable & getDataQueue()
		{
			return dataQueue_m;
		}

		inline AlgLib::DataQueueSerializable const& getDataQueue() const
		{
			return (AlgLib::DataQueueSerializable const&)dataQueue_m;
		}


		

		bool isRunningNormally() const;

		void getContext(ThreadContext& ctx) const;

		

	private:
			
			void internalLock();

			void internalUnLock();

			bool mtFunction();

			
			
			CppUtils::String mtInstance_m;
			
			int delayTime_m;

			CppUtils::Thread* thread_m;

			// run manager
			MtRunManager mtRunManager_m;

			AlgorithmMt& base_m;

		 	SettingsManager const& settingsManager_m;

			MTSettings const& thisSettings_m;

			CppUtils::Event shutDownEvent_m;


			// main queue keeping our data
			AlgLib::DataQueueSerializable dataQueue_m;

			
			mutable ThreadContext ctx_m;

			CppUtils::Event threadStarted_m;



	};
};

#endif