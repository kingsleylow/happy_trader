#include "mtworkerthread.hpp"

#include "exchange_packets_base.hpp"
#include "exchange_packets_mt.hpp"
#include "exchange_packet_alglib_mt_response.hpp"
#include "exchange_packet_alglib_mt_request.hpp"
#include "exchange_packet_alglib_out.hpp"
#include "algorithmmt.hpp"


namespace AlgLib {


	// Booting function
	int bootServiceThread (void *ptr) {

#ifdef HANDLE_NATIVE_EXCEPTIONS
			Testframe::init();
#endif
		// Wait a little
		CppUtils::sleepThread( 500 );

		MTWorkerThread* pThis = (MTWorkerThread*)ptr; 

		pThis->run();

		return 42;

	}

	MTWorkerThread::MTWorkerThread(CppUtils::String const& mtInstance, int const delayTime, SettingsManager const& smanager, AlgorithmMt& base):
		mtInstance_m(mtInstance),
		delayTime_m(delayTime <=0 ? MT_WORK_THREAD_SLEEP_SEC : delayTime ),
		thread_m(NULL),
		mtRunManager_m(base, smanager),
		base_m(base),
		settingsManager_m(smanager),
		thisSettings_m(settingsManager_m.resolveSettingsViaInstanceName(mtInstance_m))
	{
	
		//DEBUG_LOG(base_m, "MT thread starting...: " << mtInstance );
		LOG(MSG_INFO, MTWORKTHREAD, "MT thread starting...: " << mtInstance );

		dataQueue_m.initialize(mtInstance);


		// recover queue from cache if applicable
		// recover from queue cache
		if (thisSettings_m.getStartUpSettings().autorecover) {

			dataQueue_m.recoverFromCache();
			LOG(MSG_INFO, MTWORKTHREAD,"Recovering queue data from cache for terminal: " << mtInstance_m << " new queue size is: " << dataQueue_m.size());
		}
		else {
			LOG(MSG_INFO, MTWORKTHREAD,"No autorecover for terminal: " << mtInstance_m);
		}

		// read initially from file
		
		vector<AlgLib::AutoLoadFileStruct::AutoLoadFileRow> const& rows = thisSettings_m.getStartUpSettings().fstruct_m.rows_m;

		if (rows.size() > 0 ) {
			AlgLib::GeneralCommandList gclist;

			for(int i = 0; i < rows.size(); i++){
				AlgLib::AutoLoadFileStruct::AutoLoadFileRow const row_i = rows[i];

				LOG(MSG_DEBUG, MTWORKTHREAD, "Adding from startup file: " << row_i.user_m << "/***/" << row_i.srv_m << "/" << row_i.tout_m);
				AlgLib::GeneralCommand gc(AlgLib::UserInfoStruct(row_i.user_m, row_i.pass_m, row_i.srv_m, row_i.tout_m) );
				gclist.push_back( gc );

			};

			LOG(MSG_DEBUG, MTWORKTHREAD, "Adding totally: " << rows.size() << " to the queue" );
			dataQueue_m.put(gclist);
		}


		// init mt runner
		mtRunManager_m.prepare(mtInstance);

	}

	MTWorkerThread::~MTWorkerThread()
	{
		stopWork();
	}

	void MTWorkerThread::startWork()
	{
		LOCK_FOR_SCOPE(*this);

		if (thread_m) {
			THROW(CppUtils::OperationFailed, "exc_ThreadAlreadyStarted", "ctx_MTWorkerThread_startWork", "");
		}

		shutDownEvent_m.resetEvent();

		
		//{
		//	LOCK_FOR_SCOPE(*this);
			ctx_m.State = S_CREATED;
		//}

		thread_m = new CppUtils::Thread(bootServiceThread, this);
		
		if (!threadStarted_m.lock(MT_WORK_THREAD_START_WAIT_SEC*1000))
			THROW(CppUtils::OperationFailed, "exc_ThreadCannotBeStarted", "ctx_MTWorkerThread_startWork", "");
	}
		
	void MTWorkerThread::stopWork()
	{
		

		if (!thread_m)
			return;
			//THROW(CppUtils::OperationFailed, "exc_ThreadAlreadyStopped", "ctx_MTWorkerThread_stopWork", "");
		
		shutDownEvent_m.signalEvent();


		if (!thread_m->waitForThread(MT_WORK_THREAD_FINISH_WAIT_SEC*1000)) {
			THROW(CppUtils::OperationFailed, "exc_CannotFinishTherad", "ctx_MTWorkerThread_stopWork", "");
		}
		
		{
			LOCK_FOR_SCOPE(*this);

			//{
			//	LOCK_FOR_SCOPE(*this);
				ctx_m.State = S_FINISHED;
			//}
		
			delete thread_m;
			thread_m = NULL;
		}
	}

	void MTWorkerThread::run()
	{
		
		DEBUG_LOG(base_m, "MT work thread successfully started: " << mtInstance_m );
		LOG(MSG_INFO, MTWORKTHREAD, "MT work thread successfully started: " << mtInstance_m);
		
		threadStarted_m.signalEvent();	

		int sleep_ms = delayTime_m * 1000;
		
		{
			LOCK_FOR_SCOPE(*this);
			ctx_m.State = S_RUNNING;
			ctx_m.Tid = CppUtils::getTID();
			ctx_m.LastHeartBeat = CppUtils::getTime();
			ctx_m.mtPid = -1;
		}
	


		while(true) {
			try {
			
					if (shutDownEvent_m.lock(sleep_ms))
						break;

					if (!mtFunction())
						break;
		
					{
						LOCK_FOR_SCOPE(*this);
						ctx_m.LastHeartBeat = CppUtils::getTime();
					}
			}
			catch(CppUtils::Exception& e) {
				LOG(MSG_ERROR, MTWORKTHREAD, "Error in MT work thread: " << e.message());
			}
#ifdef HANDLE_NATIVE_EXCEPTIONS
			catch(Testframe::Exception& e) {
				LOG(MSG_FATAL, MTWORKTHREAD, "MSVC Error in MT work thread: " << e.message());
			}
#endif
			catch(...) {
				LOG(MSG_ERROR, MTWORKTHREAD, "Unknown error in MT work thread");
			}
		}

		{
			LOCK_FOR_SCOPE(*this);
			ctx_m.State = S_FINISHING;
		}
	
		LOG(MSG_INFO, MTWORKTHREAD, "MT work thread finishing");
	}

	bool MTWorkerThread::mtFunction()
	{
			try {

				// need to perform all operations here

				// need to extract consequent value from the queue
				// then grab data an dpush further

					AlgLib::GeneralCommand gc;
					bool data_found, data_available, is_last;

					// do not wait cause we immediately get out
					CheckForRemovals_front_if pr;
					dataQueue_m.front(0, gc, data_found, data_available,is_last, pr);
					if ( data_found || data_available ) {

						if (gc.getType() == AlgLib::GeneralCommand::CT_ConnectUserInfo) {

							UserInfoStruct const& ui = gc.getUserInfo();
							LOG(MSG_DEBUG, MTWORKTHREAD, "Got data from queue: " << gc.toString() );
							
							//
							mtRunManager_m.run( ui.user_m, ui.password_m, ui.server_m );
							
						}
						else if (gc.getType() == AlgLib::GeneralCommand::CT_Order) {
							// no-op
						}
						else
							THROW(CppUtils::OperationFailed, "exc_InvalidDataTypeInQueue", "ctx_mtFunction", "");
						


					}



			}
			catch(CppUtils::Exception& e) {
				LOG(MSG_ERROR, MTWORKTHREAD, "Error in MT function: " << e.message());
			}
			catch(...) {
				LOG(MSG_ERROR, MTWORKTHREAD, "Unknown error in MT function");
			}

			return true;
	}

	


	
	bool MTWorkerThread::isRunningNormally() const
	{
		LOCK_FOR_SCOPE(*this);
		return (ctx_m.State == S_RUNNING);
	}
	

	void MTWorkerThread::getContext(ThreadContext& ctx) const
	{
		LOCK_FOR_SCOPE(*this);
		
		// adjust current pid
		ctx_m.mtPid = mtRunManager_m.getMtPid();
		ctx = ctx_m;
		
	}


	void MTWorkerThread::internalLock()
	{
		//LOG(MSG_INFO, MTWORKTHREAD, "Locked mt work thread");
		lock();
	}

	void MTWorkerThread::internalUnLock()
	{
		//LOG(MSG_INFO, MTWORKTHREAD, "UnLocked mt work thread");
		unlock();
	}


};

