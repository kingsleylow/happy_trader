#include "mtproxycore.hpp"
#include "mtproxymanager.hpp"


#define MTPROXYCORE "MTPROXYCORE"

#define TRY \
	try {

#define CATCH_ALL(FUNNAME)	\
	return MTST_NO_ERROR;\
	}\
	catch(CppUtils::Exception& e)	\
	{\
		LOG(MSG_ERROR, MTPROXYCORE, "Exception in: [" << #FUNNAME << "] " << e.message() );\
	}\
	catch(...)\
	{\
		LOG(MSG_ERROR, MTPROXYCORE, "Unknown exception in: [" << #FUNNAME << "] " );\
	}\
	return MTST_GENERAL_ERROR;

#define CATCH_ALL_NR(FUNNAME)	\
	}\
	catch(CppUtils::Exception& e)	\
	{\
		LOG(MSG_ERROR, MTPROXYCORE, "Exception in: [" << #FUNNAME << "] " << e.message() );\
	}\
	catch(...)\
	{\
		LOG(MSG_ERROR, MTPROXYCORE, "Unknown exception in: [" << #FUNNAME << "] " );\
	}\
	


#define CATCH_ALL_1()	\
	return MTST_NO_ERROR;\
	}\
	catch(CppUtils::Exception& e)	\
	{\
		LOG(MSG_ERROR, MTPROXYCORE, "Exception: " << e.message() );\
	}\
	catch(...)\
	{\
		LOG(MSG_ERROR, MTPROXYCORE, "Unknown exception" );\
	}\
	return MTST_GENERAL_ERROR;


#define RESOLVE_INST_PTR(FUN)	{ \
		MtProxy::MtProxyManager* ppProxymanager = NULL;	\
		MtProxy::MtProxyManager::initialize(MtProxy::MtProxyManager::IF_RESOLVE, &ppProxymanager);	\
		if (ppProxymanager)	\
			ppProxymanager->FUN;	\
		else \
			THROW(CppUtils::OperationFailed, "exc_InvalidProxyManagerPointer", "ctx_RESOLVE_INST_PTR", ""); \
		} \


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                                         )
{
        switch (ul_reason_for_call)
        {
        case DLL_PROCESS_ATTACH:
					MtProxy::MtProxyManager::moduleHandle_m = hModule;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
                break;
        }
        return TRUE;
}


extern "C" {

	// ------------------
int PROCCALL getVersion()
{
	return MTPROXY_VERSION;
}




int PROCCALL	init(char const* configXml, int hWnd)
{ 
	TRY
		int refcnt = -1;
		MtProxy::MtProxyManager::initialize(MtProxy::MtProxyManager::IF_CREATE, NULL, &refcnt);
		if (refcnt != 1) {
			// decrement ref count
			MtProxy::MtProxyManager::initialize(MtProxy::MtProxyManager::IF_FREE);

			// raise exception
			THROW(CppUtils::OperationFailed, "exc_OnlySingleSCriptAllowed", "ctx_init", "");
		}
	
		RESOLVE_INST_PTR(startup(configXml, hWnd));
	

	CATCH_ALL("init")
}

int PROCCALL deinit()
{
	TRY
		MtProxy::MtProxyManager::initialize(MtProxy::MtProxyManager::IF_FREE);
	CATCH_ALL("deinit")
}

// sends data to main server
int PROCCALL	connectToPipeServer()
{
	TRY
		RESOLVE_INST_PTR(connectToPipeServer());
		
	CATCH_ALL("connectToPipeServer")
}
// sends data to main server
int PROCCALL	disconnectFromPipeServer()
{
	TRY
		RESOLVE_INST_PTR(disconnectFromPipeServer());
		
	CATCH_ALL("disconnectFromPipeServer")
}
// sends data to main server
int PROCCALL	pushDataToPipeServer( char const* data, MqlStr* response)
{
	TRY
		
		CppUtils::String response_s;
		RESOLVE_INST_PTR(pushDataToPipeServer(data,response_s));
	
		
		if (!MtProxy::MqlStrHelper::copy(response, response_s.c_str()))
			THROW(CppUtils::OperationFailed, "exc_NonSufficientStringBuffer", "ctx_pushDataToPipeServer", "");
		
		
	CATCH_ALL("pushDataToPipeServer")
}



// return MTST_NO_ERROR - all is ok
// returns MTST_GENERAL_ERROR : general error
// MTST_POLLING_NODATA - there is no data after tout_msec

int PROCCALL	getPollingQueueNextItem( const int tout_msec,  MqlStr* user, MqlStr* password, MqlStr* server)
{
	TRY
		MtProxy::GeneralCommand gc;
		bool data_found, data_available, is_last;

		RESOLVE_INST_PTR(waitForUserPeriodicalQueueData( tout_msec, gc, data_found, data_available, is_last));
		


		// no data available - that means that no signal arrived on this queue that data came
		if (!data_available) 
			return MTST_POLLING_NODATA;

		// that generally means that after this queue get into signalled state but before data are picked up
		// another thread removed this data. Unlikely but my happen
		// because if this thread processes all dat and remove this from the queue
		// the enxt iteration we get into event wait cycle
		// all we need to do if retrun back to wait
		if (!data_found)
			return MTST_POLLING_NODATA;

		// now seems all is ok

		if (gc.getType() != MtProxy::GeneralCommand::CT_ConnectUserInfo)
			return MTST_GENERAL_ERROR;
		
		LOG(MSG_DEBUG, MTPROXYCORE, "getPollingQueueNextItem(...) extracted : " << gc.toString() );

		if (!MtProxy::MqlStrHelper::copy(user, gc.getUserInfo().user_m.c_str()))
			THROW(CppUtils::OperationFailed, "exc_NonSufficientStringBuffer_User", "ctx_getPollingQueueNextItem", "");
		
		
		if (!MtProxy::MqlStrHelper::copy(password, gc.getUserInfo().password_m.c_str()))
		  THROW(CppUtils::OperationFailed, "exc_NonSufficientStringBuffer_Password", "ctx_getPollingQueueNextItem", "");

		if (!MtProxy::MqlStrHelper::copy(server, gc.getUserInfo().server_m.c_str()))
		  THROW(CppUtils::OperationFailed, "exc_NonSufficientStringBuffer_Server", "ctx_getPollingQueueNextItem", "");

	

		if (is_last)
			return MTST_POLLING_OK_LAST;

		return MTST_POLLING_OK;
		
	CATCH_ALL_NR(getPollingQueueNextItem);
	return MTST_GENERAL_ERROR;
	

}

int PROCCALL	getPollingQueueNextItem2(  int const tout_msec, MqlStr* data )
{
		TRY
		MtProxy::GeneralCommand gc;
		bool data_found, data_available, is_last;

		RESOLVE_INST_PTR(waitForUserPeriodicalQueueData( tout_msec, gc, data_found, data_available, is_last));
		

		if (!(data_available && data_found)) 
			return MTST_POLLING_NODATA;


		// now seems all is ok

		if (
			gc.getType() != MtProxy::GeneralCommand::CT_ConnectUserInfo &&
			gc.getType() != MtProxy::GeneralCommand::CT_Order
		) {
				THROW(CppUtils::OperationFailed, "exc_InvalidDataTypeInQueue", "ctx_getPollingQueueNextItem2", "");
		}
		
		LOG(MSG_DEBUG, MTPROXYCORE, "getPollingQueueNextItem2(...) extracted : " << gc.getStringRepresentation() );

		if (!MtProxy::MqlStrHelper::copy(data, gc.getStringRepresentation().c_str()))
			THROW(CppUtils::OperationFailed, "exc_NonSufficientStringBuffer", "ctx_getPollingQueueNextItem2", "");
		
		
		if (is_last)
			return MTST_POLLING_OK_LAST;

		return MTST_POLLING_OK;
		
	CATCH_ALL_NR(getPollingQueueNextItem2);
	return MTST_GENERAL_ERROR;
}




int PROCCALL	getTerminalUid(MqlStr* terminal_uid)
{
	TRY
		CppUtils::Uid terminalUid;
		RESOLVE_INST_PTR(getTerminalUid(terminalUid))
		
		if (!terminalUid.isValid())
			return MTST_GENERAL_ERROR;

		if (!MtProxy::MqlStrHelper::copy(terminal_uid, terminalUid.toString().c_str()))
			THROW(CppUtils::OperationFailed, "exc_NonSufficientStringBuffer", "ctx_getTerminalUid", "");
		

		
		
	CATCH_ALL("getTerminalUid")
}

int PROCCALL getQueueSize( )
{
	int queuesize;
	RESOLVE_INST_PTR(readStatus(queuesize))
	
	
	return queuesize;

}

int PROCCALL isConnectedToPipeServer()
{
	bool connected;
	RESOLVE_INST_PTR(isConnectedToPipeServer(connected));

	return (connected ? MST_BOOL_TRUE : MST_BOOL_FALSE);
}

int PROCCALL getConnectAttemptNumber()
{
	int connectAttemptNumber;
	RESOLVE_INST_PTR(getConnectAttemptNumber(connectAttemptNumber));

	return connectAttemptNumber;

}

int PROCCALL callConnectForm(char const* user, char const* password, char const* server )
{
	TRY
		RESOLVE_INST_PTR(callConnectForm(user, password, server));
	

		return MTST_NO_ERROR;
	CATCH_ALL("callConnectForm")
}




	
} // end of namespace
