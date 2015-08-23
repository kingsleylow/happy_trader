#include "mtqueuemanager.hpp"


#define _CRT_SECURE_NO_DEPRECATE
#define CRYPTOPP_DEFAULT_NO_DLL
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1

#include "shared/crypto/config.h"
#include "shared/crypto/misc.h"
#include "shared/crypto/hex.h"
#include "shared/crypto/cryptlib.h"
#include "shared/crypto/default.h"
#include "shared/crypto/md5.h"


/*
#include "crypto/filters.h"
#include "crypto/sha.h"
#include "crypto/hmac.h"
*/

#define MTQUEUEMANAGER "MTQUEUEMANAGER"

namespace MtQueue {

	


	// Booting function
	int bootServiceThread (void *ptr) {
		// Wait a little
		//CppUtils::sleepThread( 500 );

		MtQueueManager* pThis = (MtQueueManager*)ptr; 
		
		pThis->run();
		
		return 42;
	}

	int bootWorkingThread (void *ptr) {
		// Wait a little
		//CppUtils::sleepThread( 500 );

		WorkingThread* pwt = (WorkingThread*)ptr; 
		MtQueueManager* base = (MtQueueManager*)pwt->base_m;


		pwt->threadStarted_m.signalEvent();
		pwt->tid_m = CppUtils::getTID();

		LOG(MSG_INFO, MTQUEUEMANAGER,"Work thread started");
		
		base->runWorkThread();
		
		return 42;
	}



	void MtQueueManager::initialize(int const opFlag, MtQueue::MtQueueManager** ppProxymanager, int* refCountPar) 
	{
		static CppUtils::Mutex mutex;
		static int refCounted = 0;
		static MtQueue::MtQueueManager* pmPtr = NULL;

		LOCK_FOR_SCOPE(mutex);

		if (opFlag == IF_CREATE) {
			
			if (refCounted == 0) {
				if(!pmPtr) {
					pmPtr = new MtQueue::MtQueueManager();
					LOG(MSG_INFO, MTQUEUEMANAGER, "Root object created");
				}
			}

			refCounted++;
		}
		else if (opFlag == IF_FREE){
			// destroy
			if (--refCounted <=0) {
				if(pmPtr) {
					delete pmPtr;
					pmPtr = NULL;

					LOG(MSG_INFO, MTQUEUEMANAGER, "Root object fully destroyed");
				}

				refCounted = 0;
			}
		}
		else if (opFlag == IF_RESOLVE){
			//LOG(MSG_DEBUG, MTQUEUEMANAGER, "Root object resolved");
		}

		if (refCountPar)
			*refCountPar = refCounted;

	
		if (ppProxymanager)
		*ppProxymanager = pmPtr;
	}




	MtQueueManager::MtQueueManager():
	controlThread_m(NULL),
	maxCapacity_m(-1),
	transport_m(*this)
	{  
		LOG(MSG_INFO, MTQUEUEMANAGER, "Ctor");
	}




	MtQueueManager::~MtQueueManager()
	{
		// though we have to manage this explicitely. but...
		shutdown();

		LOG(MSG_INFO, MTQUEUEMANAGER, "Dtor");
		
	}

	void MtQueueManager::shutdown()
	{
		LOCK_FOR_SCOPE(*this);

		if (!controlThread_m)
			return;
		
		shutDownThread_m.signalEvent();
	
		controlThread_m->waitForThread();

		delete controlThread_m;
		controlThread_m = NULL;
		LOG(MSG_INFO, MTQUEUEMANAGER, "Control thread stopped");
		

		for(int i =0; i < consumers_m.size(); i++) {
			WorkingThread* wt = consumers_m[i];
			if (wt) {
				delete wt;
				LOG(MSG_INFO, MTQUEUEMANAGER, "Working thread stopped: " << wt->tid_m);
			}
			else {
				LOG(MSG_WARN, MTQUEUEMANAGER, "Working thread entry invalid");
			}

		}

		consumers_m.clear();

		LOG(MSG_INFO, MTQUEUEMANAGER, "Deinitialized fully");
		
	}

	void MtQueueManager::startup(int const workThreadsCount, int const queueMaxCapacity, int logLevel)
	{
		LOCK_FOR_SCOPE(*this);

		if (controlThread_m)
			return;
		
		// set log leve;
		if (logLevel >= MSG_FATAL && logLevel <= MSG_DEBUG_DETAIL) {
			CppUtils::Log::setLogLevel(logLevel);
			LOG(MSG_INFO, MTQUEUEMANAGER, "Log level changed to: " << CppUtils::Log::getLogLevel() );
		}

		if (queueMaxCapacity <=0 || queueMaxCapacity > QUEUE_MAX_CAPACITY) {
			maxCapacity_m = QUEUE_MAX_CAPACITY;
			LOG(MSG_INFO, MTQUEUEMANAGER, "Assigning default capacity: " << QUEUE_MAX_CAPACITY);
		}


		// service thread
		controlThread_m = new CppUtils::Thread( bootServiceThread, this );

		// if cannot start thread for 10 seconds
		if (!threadStarted_m.lock(10000))
			THROW(CppUtils::OperationFailed, "exc_CannorStartControlThread", "ctx_initialize", "");

		if (workThreadsCount <=0 && workThreadsCount > MAX_WORK_THREADS)
			THROW(CppUtils::OperationFailed, "exc_WrongNumberOfWorkingThreads", "ctx_initialize", workThreadsCount);
		
		// working threads
		for(int i = 0; i < workThreadsCount; i++) {
			WorkingThread* wt = new WorkingThread();
			wt->base_m = this;
			wt->thread_m = new CppUtils::Thread(bootWorkingThread, wt);

			if (!wt->threadStarted_m.lock(10000))
				THROW(CppUtils::OperationFailed, "exc_CannorStartWorkThread", "ctx_initialize", wt->tid_m);

			consumers_m.push_back(wt);
		}


	}

	void MtQueueManager::run()
	{
		threadStarted_m.signalEvent();
		LOG(MSG_INFO, MTQUEUEMANAGER,"Control thread started");

		int sleep_msec = CONTROL_THREAD_SLEEP_PERIOD_SEC * 1000;

		CppUtils::NativeEvent events[] = 
		{
			shutDownThread_m.getNativeEvent()				// 0	
		};

		while(true) {

			

			try {
				// wait some of them 
				int res = CppUtils::Event::lockForSomeEvents(events, 1, sleep_msec);


				if (res==0) {
					// shutdown
					break;
				}

				    

			}
			catch(CppUtils::Exception& e) {
				LOG(MSG_ERROR, MTQUEUEMANAGER, "Error in helper thread: " << e.message());
			}
			catch(...) {
				LOG(MSG_ERROR, MTQUEUEMANAGER, "Unknown error in event processing thread");
			}

		}

			LOG(MSG_INFO, MTQUEUEMANAGER,"Control thread finishing");
	}

	void MtQueueManager::runWorkThread()
	{
		

		int sleep_msec = CONTROL_THREAD_SLEEP_PERIOD_SEC * 1000;

		CppUtils::NativeEvent events[] = 
		{
			shutDownThread_m.getNativeEvent(),				// 0	
			dataArrived_m.getNativeEvent()
		};

		while(true) {

			
			try {
				// wait some of them 
				int res = CppUtils::Event::lockForSomeEvents(events, 2, sleep_msec);


				if (res==0) {
					// shutdown
					break;
				}

			
				if (res==1) {
					
					DataToSend data;
					bool found = false;
					// we have our event
					{
						LOCK_FOR_SCOPE(queueMtx_m);
						
						if (!queue_m.empty()) {
							data = queue_m.front();
							found = true;

							// remove
							queue_m.pop_front();

							if (queue_m.empty()) {
								// reset
								dataArrived_m.clearEvent();
							}
						}
					}

					if (found) {
						// process when lock released

						int repeatAttempts = data.getRepeatAttempts();
						
						TransportLayer::Result res;
						for(int k = 1; k <= repeatAttempts; k++) {
							res = transport_m.sendData(data);
								
							if (res.isOk()) {
								LOG(MSG_DEBUG, MTQUEUEMANAGER, "For event: [" << data.getEventId() << "] result was OK");
								break;
							}

							if (isShutDownState()) {
								LOG(MSG_DEBUG, MTQUEUEMANAGER, "Shutdown, breaking..." );
								break;
							}
							
							// error happend
							LOG(MSG_WARN, MTQUEUEMANAGER, "For event: [" << data.getEventId() << "] repeating: " << k <<", max count: " << repeatAttempts << ", reason: [" << res.getMessage() << "]");

						}



						// need  decide if we need to push this again to the queue
						if (data.getLifeTimeFlag() == DataToSend::LTF_DoNotRemove) {
							// push again to the queue
							LOG(MSG_DEBUG, MTQUEUEMANAGER, "Pushing data to the queue again, cause this message is never removed");
							pushInternally( data );
						}
						else if (data.getLifeTimeFlag() == DataToSend::LTF_RemoveAfterFailAttempts) {
							if (!res.isOk()) {
								if (!data.checkFailCounterTriggers()) {
									// fail not triggers, so adding to the queue again
									LOG(MSG_DEBUG, MTQUEUEMANAGER, "Pushing data to the queue again, cause fail number is still: " << data.getFailNumber());
									pushInternally( data );
								}
								else {
									LOG(MSG_DEBUG, MTQUEUEMANAGER, "Though request failed, the message is removed because the number of attempts is maximum:" << data.getFailNumber());
								}
							}

						}
						

					}

				}

					    

			}
			catch(CppUtils::Exception& e) {
				LOG(MSG_ERROR, MTQUEUEMANAGER, "Error in work thread: " << e.message());
			}
			catch(...) {
				LOG(MSG_ERROR, MTQUEUEMANAGER, "Unknown error in work thread");
			}

		}

		LOG(MSG_INFO, MTQUEUEMANAGER,"Work thread finishing");
	}

	void MtQueueManager::push(DataToSend const& data)
	{
		LOG(MSG_DEBUG, MTQUEUEMANAGER, "Pushing event to the queue with id: [" << data.getEventId() << "]");


		if (!data.isValid())
			THROW(CppUtils::OperationFailed, "exc_DataNotValid", "ctx_push", "");

		pushInternally( data );

	}

	
	void MtQueueManager::pushInternally(DataToSend const& data)
	{
		LOCK_FOR_SCOPE(queueMtx_m);

		if (isShutDownState())
			THROW(CppUtils::OperationFailed, "exc_WeAreInShutdownState", "ctx_pushInternally", "");

		if (maxCapacity_m > 0 && queue_m.size() >= maxCapacity_m) 
			THROW(CppUtils::OperationFailed, "exc_QueueReachedMaxCapacity", "ctx_pushInternally", maxCapacity_m);
		
		
		queue_m.push_back(data);
		if (queue_m.size() == 1) {
			dataArrived_m.signalEvent();
		}
	}

	

	

	CppUtils::String MtQueueManager::rbt_getInfo(  char const* key, char const* secret)
	{
		if (secret==NULL || key==NULL)
			THROW(CppUtils::OperationFailed, "exc_InvalidSecretOrkey", "ctx_rbt_getInfo", "");

		CppUtils::String secret_s = secret;
		CppUtils::String key_s = key;


		LOG(MSG_INFO, MTQUEUEMANAGER, "Calling <getInfo> from https://btc-e.com/tapi, secret: " << secret_s << ", key: " << key_s );
		SimpleDataEntry se;
		
		se.url_m="https://btc-e.com/tapi";

		CppUtils::String queryString="nonce=";
		queryString.append(CppUtils::long2String(CppUtils::getTime()));
		queryString.append("&method=").append("getInfo");
		LOG(MSG_INFO, MTQUEUEMANAGER, "Query string generated: " << queryString);

		CppUtils::String signature = signRequest(queryString, secret_s);

		// need to sign queryString with secret_s

		se.props_m["Key"] =  key_s;
		se.props_m["Sign"]= CppUtils::toLowerCase(signature);

	
		// this is post data
		se.data_m = queryString;
		
		TransportLayer::Result res = transport_m.sendSimpleData(se);

		if (!res.isOk()) {
			THROW(CppUtils::OperationFailed, "exc_POSTOpFailed", "ctx_rbt_getInfo", res.getCode() << " - " << res.getMessage());
		}

		return res.data().toString();
		
	}

	


	CppUtils::String MtQueueManager::signRequest(CppUtils::String const& requestString, CppUtils::String const& secret)
	{
		CppUtils::String mac_print;
		try
		{
			//CryptoPP::SecByteBlock key(32);

			//CppUtils::String  secret_s = CppUtils::toUpperCase( secret );
			//CppUtils::String decoded_secret;

			//CryptoPP::StringSource ss(secret_s, new CryptoPP::HexDecoder(	new CryptoPP::StringSink(decoded_secret)	) ); // StringSource
			/*
			CryptoPP::HexDecoder decoder;

			decoder.Put( (byte*)secret_s.data(), secret_s.size() );
			decoder.MessageEnd();
			CryptoPP::word64 size = decoder.MaxRetrievable();
			if(size && size <= SIZE_MAX)
			{
				decoded_secret.resize(size);		
				decoder.Get((byte*)decoded_secret.data(), decoded_secret.size());
			}
			*/
			CryptoPP::HMAC< CryptoPP::SHA512 > hma;
			hma.SetKey((byte*)secret.c_str(), secret.size());

			CppUtils::String mac;
			CryptoPP::StringSource(requestString, true,  new CryptoPP::HashFilter(hma, new CryptoPP::StringSink(mac) ) ); // StringSource

			CryptoPP::StringSource(mac, true, new CryptoPP::HexEncoder( new CryptoPP::StringSink(mac_print)  ) ); 

		}
		catch(const CryptoPP::Exception& e)
		{
			THROW(CppUtils::OperationFailed, "exc_SignRequestError", "ctx_signRequest", e.what());
		}
		
		return mac_print;
	}


}; // end of namespace