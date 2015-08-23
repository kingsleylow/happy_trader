#include "mtqueuecore.hpp"
#include "helperstruct.hpp"


#define MTPROXYCORE "MTPROXYCORE"

#define TRY \
	try {

#define CATCH_ALL(FUNNAME)	\
	return DLL_FUN_CALL_OK;\
	}\
	catch(CppUtils::Exception& e)	\
	{\
		LOG(MSG_ERROR, MTPROXYCORE, "Exception in: [" << FUNNAME << "] " << e.message() );\
	}\
	catch(...)\
	{\
		LOG(MSG_ERROR, MTPROXYCORE, "Unknown exception in: [" << FUNNAME << "] " );\
	}\
	return DLL_FUN_CALL_FAILURE;

#define CATCH_ALL_1()	\
	return DLL_FUN_CALL_OK;\
	}\
	catch(CppUtils::Exception& e)	\
	{\
		LOG(MSG_ERROR, MTPROXYCORE, "Exception: " << e.message() );\
	}\
	catch(...)\
	{\
		LOG(MSG_ERROR, MTPROXYCORE, "Unknown exception" );\
	}\
	return DLL_FUN_CALL_FAILURE;



extern "C" {

	// ------------------
int PROCCALL sayHallo()
{
		LOG(MSG_INFO, MTPROXYCORE, "Version: " << MTQUEUE_VERSION << ",  hallo said");
		return 0;
}


int PROCCALL initialize(int const logLevel, int const workThreadsCount)
{
	TRY 
		
		int refcnt = -1;
		MtQueue::MtQueueManager::initialize(MtQueue::MtQueueManager::IF_CREATE, NULL, &refcnt);
		if (refcnt != 1) {
			// decrement ref count
			MtQueue::MtQueueManager::initialize(MtQueue::MtQueueManager::IF_FREE);

			// raise exception
			THROW(CppUtils::OperationFailed, "exc_OnlySingleSCriptAllowed", "ctx_init", "");
		}

		MtQueue::MtQueueManager* ppProxymanager = NULL;
		MtQueue::MtQueueManager::initialize(MtQueue::MtQueueManager::IF_RESOLVE, &ppProxymanager);
		if (ppProxymanager)	{
			ppProxymanager->startup(workThreadsCount, QUEUE_MAX_CAPACITY, logLevel);
			//ppProxymanager->signRequest("nonce=1389213856&method=getInfo", "f6e7296cfc05d47707dd4f7984c022be3e5d2125e064da9a0bdc135c73d19f0f");
		}
		else 
			THROW(CppUtils::OperationFailed, "exc_InvalidProxyManagerPointer", "ctx_RESOLVE_INST_PTR", "");
		
	
		  
	CATCH_ALL("initialize");

}

int PROCCALL	deinitialize()
{
	TRY 
		MtQueue::MtQueueManager::initialize(MtQueue::MtQueueManager::IF_FREE);
	

	CATCH_ALL("deinitialize");
}

int PROCCALL pushRemoveImmediately(char const* url, char const* data, char const* filename, int repeatCount)
{
	TRY 
		MtQueue::DataToSend data_packet;
		MtQueue::FileDataEntry fe(filename, data, url);

		data_packet.initialize(fe, MtQueue::DataToSend::LTF_RemoveImmediately, repeatCount);
		
		MtQueue::MtQueueManager* ppProxymanager = NULL;
		MtQueue::MtQueueManager::initialize(MtQueue::MtQueueManager::IF_RESOLVE, &ppProxymanager);
		if (ppProxymanager)	
			ppProxymanager->push( data_packet );
		else 
			THROW(CppUtils::OperationFailed, "exc_InvalidProxyManagerPointer", "ctx_RESOLVE_INST_PTR", "");

	
	CATCH_ALL("pushRemoveImmedaitely");
}

int PROCCALL pushNeverRemove(char const* url, char const* data, char const* filename, int repeatCount)
{
	TRY 
		MtQueue::DataToSend data_packet;
		MtQueue::FileDataEntry fe(filename, data, url);


		data_packet.initialize(fe, MtQueue::DataToSend::LTF_DoNotRemove, repeatCount);
		
		MtQueue::MtQueueManager* ppProxymanager = NULL;
		MtQueue::MtQueueManager::initialize(MtQueue::MtQueueManager::IF_RESOLVE, &ppProxymanager);
		if (ppProxymanager)	
			ppProxymanager->push( data_packet );
		else 
			THROW(CppUtils::OperationFailed, "exc_InvalidProxyManagerPointer", "ctx_RESOLVE_INST_PTR", "");

	CATCH_ALL("pushNeverRemove");
}

int PROCCALL pushRemoveAfterFailsNumber(char const* url, char const* data, char const* filename, int failNumber, int repeatCount)
{
	TRY 
		MtQueue::DataToSend data_packet;
		MtQueue::FileDataEntry fe(filename, data, url);


		data_packet.initializeWithMaxFailCounter(fe, failNumber, repeatCount );
		
		MtQueue::MtQueueManager* ppProxymanager = NULL;
		MtQueue::MtQueueManager::initialize(MtQueue::MtQueueManager::IF_RESOLVE, &ppProxymanager);
		if (ppProxymanager)	
			ppProxymanager->push( data_packet );
		else 
			THROW(CppUtils::OperationFailed, "exc_InvalidProxyManagerPointer", "ctx_RESOLVE_INST_PTR", "");

	CATCH_ALL("pushRemoveAfterFailsNumber");
}


int PROCCALL	rbt_getInfo(MqlStr* outData, char const* key, char const* secret)
{
	TRY 
		
		
	CppUtils::String result;

		MtQueue::MtQueueManager* ppProxymanager = NULL;
		MtQueue::MtQueueManager::initialize(MtQueue::MtQueueManager::IF_RESOLVE, &ppProxymanager);
		if (ppProxymanager)
			result = ppProxymanager->rbt_getInfo(key, secret);
		else 
			THROW(CppUtils::OperationFailed, "exc_InvalidProxyManagerPointer", "ctx_RESOLVE_INST_PTR", "");


	if (!MtQueue::MqlStrHelper::copy(outData, result.c_str()))
		  THROW(CppUtils::OperationFailed, "exc_NonSufficientStringBuffer_Server", "ctx_rbt_getInfo", "");

		
	CATCH_ALL("rbt_getInfo");
}



	
} // 
