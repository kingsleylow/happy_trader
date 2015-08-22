#ifndef _ALGLIBMT2_MTWORKERMANAGER_INCLUDED
#define _ALGLIBMT2_MTWORKERMANAGER_INCLUDED

#include "alglibmetatrader2defs.hpp"
#include "settingsmanager.hpp"
#include "mtworkerthread.hpp"

#define MTWORKTHREADENGINE "MTWORKTHREADENGINE"

namespace AlgLib {



	template<class T>
	class MtWorkerThreadProxy
	{
		public: 
			MtWorkerThreadProxy(MTWorkerThread& base, T& accessObj):
				base_m(&base),
				accessObj_m(&accessObj)
			{
				acquire();
			} 

			~MtWorkerThreadProxy()
			{
			
				release();
			}

			// copy ctr
			MtWorkerThreadProxy(MtWorkerThreadProxy const& rhs):
				base_m(rhs.base_m),
				accessObj_m(rhs.accessObj_m)
			{
				acquire();
			}

			// make move cemantic
			MtWorkerThreadProxy(MtWorkerThreadProxy && rhs):
				base_m(rhs.base_m),
				accessObj_m(rhs.accessObj_m)
			{
				rhs.clear();
			}

	

			T& operator ()() 
			{
				return *accessObj_m;
			}

			T const& operator ()() const
			{
				return (T const&)*accessObj_m;
			}

		private:

			MtWorkerThreadProxy();

			MtWorkerThreadProxy& operator = (MtWorkerThreadProxy const& rhs);

			void clear()
			{
				base_m = NULL;
				accessObj_m = NULL;
			}

			void acquire()
			{
				if (base_m)
					base_m->internalLock();
			}

			void release()
			{
				if (base_m)
					base_m->internalUnLock();
			
			}

			MTWorkerThread* base_m;

			T* accessObj_m;

		};


	/**
	* Class incapsulating all wrk with all working threads

	*/

	class MTWorkerThreadManager: private CppUtils::Mutex
	{
	public:

		class CheckForRemovals_remove_if
		{
		public:
			

			CheckForRemovals_remove_if():
				cur_time_m((int)CppUtils::getTime())
			{
			};

			bool operator()(AlgLib::BaseQueueElement& elem)
			{
				if (elem.lifetime() <= 0)
					return false;

				if (elem.lastAccessTime() <=0)
					return false;

				int cur_livetime = cur_time_m - elem.lastAccessTime();
				if (cur_livetime > elem.lifetime()) {

					LOG(MSG_DEBUG, MTWORKTHREADENGINE, "Removing element: [" << elem.toString()<< "] because life time detected is: " << cur_livetime);
					return true;
				}

				return false;
			}

		private:
			int cur_time_m;
		};



		MTWorkerThreadManager(SettingsManager const& smanager, AlgorithmMt& base);

		~MTWorkerThreadManager();

		// launches all threads
		void startAllWork();

		// finish all threads
		void finishAllWork();

		// helper to get access to thread object performing locking 
		MtWorkerThreadProxy<AlgLib::DataQueueSerializable> getDataQueueObjectProxy(CppUtils::String const& name);
		
	

		// performs thread restart and wait for existing thread completion
		void restartThread(CppUtils::String const& name);
		

		// checks for hearbeats to be recent
		void checkMtConection();

		// check if queues have old data so we would remove them
		void checkQueuesforOldData();

		
		void getContext(CppUtils::String const& name, MTWorkerThread::ThreadContext& ctx) const;

		void getAllContexts( vector<MTWorkerThread::ThreadContext>& ctxList) const;

		void getAllRunningPids( set<int>& pids) const;

		bool isRunningNormally(CppUtils::String const& name) const;


	private:

		AlgorithmMt& base_m;

		SettingsManager const& settingsManager_m;

		map<CppUtils::String, MTWorkerThread*> workingThreads_m;

	};

};


#endif