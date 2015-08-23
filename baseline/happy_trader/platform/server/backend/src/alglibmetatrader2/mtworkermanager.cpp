#include "mtworkermanager.hpp"
#include "algorithmmt.hpp"



namespace AlgLib {
		MTWorkerThreadManager::MTWorkerThreadManager(SettingsManager const& smanager, AlgorithmMt& base):
			base_m(base),
			settingsManager_m(smanager)
		{

		}

		MTWorkerThreadManager::~MTWorkerThreadManager()
		{
			finishAllWork();
		}

		// launches all threads
		void MTWorkerThreadManager::startAllWork()
		{
			LOCK_FOR_SCOPE(*this);

			finishAllWork();

			for(auto it = settingsManager_m.getSettings().mtSettingsMap_m.begin(); it != settingsManager_m.getSettings().mtSettingsMap_m.end(); it++) {
				MTSettings const & settings_i = it->second;

				MTWorkerThread *wt = new MTWorkerThread(settings_i.getStartUpSettings().mtName, -1, settingsManager_m, base_m);
				wt->startWork();

				workingThreads_m[settings_i.getStartUpSettings().mtName] = wt;

			}

			DEBUG_LOG(base_m, "Started");
		}

		// finish all threads
		void MTWorkerThreadManager::finishAllWork()
		{
			LOCK_FOR_SCOPE(*this);

			// finish
			for(auto it = workingThreads_m.begin(); it != workingThreads_m.end(); it++) {
				try {
					it->second->stopWork();
				}
				catch(CppUtils::Exception& e)
				{
					LOG(MSG_ERROR, MTWORKTHREADENGINE, "Exception on finishing work thread: " << e.message() << " - "<< it->second->getInstance());
				}
				catch(...)
				{
					LOG(MSG_ERROR, MTWORKTHREADENGINE, "Unknown exception on finishing work thread: " << it->second->getInstance());
				}

				delete it->second;

				DEBUG_LOG(base_m, "Finished");
			}
			 
			workingThreads_m.clear();

			LOG(MSG_INFO, MTWORKTHREADENGINE, "All MT threading work finished"); 
		}

		MtWorkerThreadProxy<AlgLib::DataQueueSerializable> MTWorkerThreadManager::getDataQueueObjectProxy(CppUtils::String const& name)
		{
			

			LOCK_FOR_SCOPE(*this);

			auto it = workingThreads_m.find(name);
			if (it == workingThreads_m.end())
				THROW(CppUtils::OperationFailed,"exc_InvalidMTName", "ctx_WorrManager_getThreadObjectProxy", name );

			MtWorkerThreadProxy<AlgLib::DataQueueSerializable> proxy(*it->second, it->second->getDataQueue());

			return proxy;
		}

		


		// called from different thread
		// so neeed to secure if modified externally
		
		void MTWorkerThreadManager::checkMtConection()
		{
			LOG(MSG_DEBUG, MTWORKTHREADENGINE, "Checking connections...");

			LOCK_FOR_SCOPE(*this);
			for(auto it = workingThreads_m.begin(); it != workingThreads_m.end(); it++) {
				if (!it->second->isRunningNormally())
					base_m.sendMTConnectStatus(false, it->first);
			}

		}
		

		void MTWorkerThreadManager::checkQueuesforOldData()
		{
			//LOG(MSG_DEBUG, MTWORKTHREADENGINE, "Checking MT threads for stale data...");

			LOCK_FOR_SCOPE(*this);
			for(auto it = workingThreads_m.begin(); it != workingThreads_m.end(); it++) {

				MtWorkerThreadProxy<AlgLib::DataQueueSerializable> proxy(*it->second, it->second->getDataQueue());

				CheckForRemovals_remove_if pr;
				int removed_cnt = proxy().checkForRemovals<CheckForRemovals_remove_if>( pr );

				LOG(MSG_DEBUG, MTWORKTHREADENGINE, "Checked if queue ["<< it->second->getInstance() << "] has stale data, removed: " << removed_cnt << ", size remained: " << proxy().size() );

			};

		}

		void MTWorkerThreadManager::restartThread(CppUtils::String const& name)
		{
			LOCK_FOR_SCOPE(*this);

			auto it = workingThreads_m.find(name);
			if (it == workingThreads_m.end())
				THROW(CppUtils::OperationFailed,"exc_InvalidMTName", "ctx_WorkManager_restartThread", name );

			it->second->stopWork();
			delete it->second;


			// start new thread
			MTWorkerThread *wt = new MTWorkerThread(name, -1, settingsManager_m, base_m);
			wt->startWork();

			workingThreads_m[name] = wt;


		}

	
		void MTWorkerThreadManager::getContext(CppUtils::String const& name, MTWorkerThread::ThreadContext& ctx) const
		{
			LOCK_FOR_SCOPE(*this);
			auto it = workingThreads_m.find(name);
			if (it == workingThreads_m.end())
				THROW(CppUtils::OperationFailed,"exc_InvalidMTName", "ctx_WorkManager_getContext", name );

			it->second->getContext(ctx);

		}

		void MTWorkerThreadManager::getAllContexts( vector<MTWorkerThread::ThreadContext>& ctxList) const
		{
			
			ctxList.clear();
			LOCK_FOR_SCOPE(*this);

			for(auto &it: workingThreads_m) {
				MTWorkerThread::ThreadContext ctx;
				it.second->getContext(ctx);

				ctxList.push_back( ctx );
			}
		}

		void MTWorkerThreadManager::getAllRunningPids( set<int>& pids) const
		{
			pids.clear();

				LOCK_FOR_SCOPE(*this);

			for(auto &it: workingThreads_m) {
				MTWorkerThread::ThreadContext ctx;
				it.second->getContext(ctx);

				if (ctx.mtPid > 0)
					pids.insert(ctx.mtPid);

			}
		}

		
		bool MTWorkerThreadManager::isRunningNormally(CppUtils::String const& name) const
		{
			LOCK_FOR_SCOPE(*this);
			auto it = workingThreads_m.find(name);
			if (it == workingThreads_m.end())
				THROW(CppUtils::OperationFailed,"exc_InvalidMTName", "ctx_WorkManager_isRunningNormally", name );

			return it->second->isRunningNormally();
		}
		

};
