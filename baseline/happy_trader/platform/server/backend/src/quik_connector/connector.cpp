

#include "connector_api.h"
#include "connector.hpp"

#define CONNECTOR "CONNECTOR"

// ------------------------
// definitions
#define CHECK_ENV(ENV_HNDL)	\
	if (ENV_HNDL==NULL) return QC_INVALID_ENV_HANDLE;

#define CHECK_ERR(ERR_HNDL)	\
	if (ERR_HNDL==NULL) return QC_INVALID_ERROR_HANDLE;

#define CONN(ENV_HNDL)	\
	reinterpret_cast<QuikConnector::ConnectorInstance*>(ENV_HNDL)

#define ERR(ERR_HNDL)	\
	reinterpret_cast<QuikConnector::ConnectorErrorInstance*>(ERR_HNDL)


#define QTRY(ENVHN, ERRHN) \
	int _result = QC_FUN_OK;	\
	CHECK_ENV(ENVHN);	\
	CHECK_ERR(ERRHN);	\
	QuikConnector::ConnectorErrorInstance* errInst = reinterpret_cast<QuikConnector::ConnectorErrorInstance*>(ERRHN);	\
	errInst->setNoError();	\
	try {

#define QCATCH	\
	}	\
	catch(QuikConnector::QuikApiException& qe)	\
	{	\
		_result = QC_FUN_FAILURE;	\
		errInst->init(qe);	\
	}	\
	catch(CppUtils::Exception& e)	\
	{	\
		_result = QC_FUN_FAILURE;	\
		errInst->init(e);	\
	}	\
	catch(...) \
	{	\
		_result = QC_FUN_FAILURE;	\
		errInst->init("Unknown exception");	\
	};\
	return _result;

//--------
// no error handle exist

#define QTRY_NHN \
	int _result = QC_FUN_OK;	\
	try {

#define QCATCH_NHN	\
	}	\
	catch(CppUtils::Exception& e)	\
	{	\
		LOG( MSG_WARN, CONNECTOR, "Exception in ( " << __FUNCTION__ << " ) : " << e.message() << " - " << e.context() << " - " << e.arg() << " - " << e.addInfo());	\
		_result = QC_FUN_FAILURE;	\
	}	\
	catch(...) \
	{	\
		LOG( MSG_WARN, CONNECTOR, "Unknown exception in ( " << __FUNCTION__ << " )" );	\
		_result = QC_FUN_FAILURE;	\
	};\
	return _result;

// pure C code
// implementation of connector_api.h
#ifdef __cplusplus
extern "C" {
#endif

// --------------------------

BOOL APIENTRY DllMain(HINSTANCE hinstDll, DWORD dwReasion, LPVOID lpReserved) {
		if(dwReasion == DLL_PROCESS_ATTACH) {
			QuikConnector::DDEQuikServerWrap::registerWindowClass(hinstDll);
		}

		return TRUE;
}

// ------------------------
// -------------------------------
	// functions to be delegated from C
	int qc_create_enviroment(void** envPP )
	{
		QTRY_NHN;
			*envPP = new QuikConnector::ConnectorInstance();
		QCATCH_NHN;

	}

	// releases also all error handles
	int qc_release_enviroment(void* envP)
	{
		QTRY_NHN;
			QuikConnector::ConnectorInstance *ptr = CONN(envP);
			delete ptr;
		QCATCH_NHN;
	}

	int qc_get_create_error_handle(void* envP, void** errorPP)
	{
		CHECK_ENV(envP);
		QTRY_NHN;
			*errorPP = CONN(envP)->newErrorInstance();
		QCATCH_NHN;
	}

	// good programming practice to call that
	int qc_get_release_error_handle(void* envP, void* errorP)
	{
		CHECK_ENV(envP);
		QTRY_NHN;
			CONN(envP)->releaseErrorInstance(errorP);
		QCATCH_NHN;
	}


	// ----------------------------
	// this returns data residing in error handle
	// may return NULL - means no error
	int qc_get_error_msg(void* errorP, char** errorMsgPP)
	{
		CHECK_ERR(errorP);
		
		QTRY_NHN;
			*errorMsgPP = NULL;
			*errorMsgPP = (char*)ERR(errorP)->message_m.c_str();
		QCATCH_NHN;
	}

	int qc_get_error_type(void* errorP, char** errorTypePP)
	{
		CHECK_ERR(errorP);
		
		QTRY_NHN;
			*errorTypePP = NULL;
			*errorTypePP = (char*)ERR(errorP)->type_m.c_str();
		QCATCH_NHN;
	}

	int qc_get_error_arg(void* errorP, char** errorArgPP)
	{
		CHECK_ERR(errorP);
		
		QTRY_NHN;
			*errorArgPP = NULL;
			*errorArgPP = (char*)ERR(errorP)->arg_m.c_str();
		QCATCH_NHN;
	}

	int qc_get_error_ctx(void* errorP, char** errorCtxPP)
	{
		CHECK_ERR(errorP);
		
		QTRY_NHN;
			*errorCtxPP = NULL;
			*errorCtxPP = (char*)ERR(errorP)->context_m.c_str();
		QCATCH_NHN;
	}

	int qc_get_error_quik_code(void* errorP, int* errorQuikCodeP)
	{
		CHECK_ERR(errorP);
		
		QTRY_NHN;
			*errorQuikCodeP = 0;
			*errorQuikCodeP = ERR(errorP)->quikCode_m;
		QCATCH_NHN;
	}

	int qc_get_error_quik_ext_code(void* errorP, int* errorQuikExtCodeP)
	{
		CHECK_ERR(errorP);
		
		QTRY_NHN;
			*errorQuikExtCodeP = 0;
			*errorQuikExtCodeP = ERR(errorP)->quikExtCode_m;
		QCATCH_NHN;
	}

	int qc_get_error_quik_message(void* errorP, char** errorQuikMsgPP)
	{
		CHECK_ERR(errorP);
		
		QTRY_NHN;
			*errorQuikMsgPP = 0;
			*errorQuikMsgPP = (char*)ERR(errorP)->quikError_m.c_str();
		QCATCH_NHN;
	}

	// -------------------------

	int qc_connect(void* envP, void* errorP, char* pathToQuik)
	{
		QTRY(envP, errorP);
			if (pathToQuik==NULL)
				return QC_INVALID_PATH_TO_TERMINAL;

			CONN(envP)->qConnect(CppUtils::String(pathToQuik));
		QCATCH;

	}

	int qc_disconnect(void* envP, void* errorP)
	{
		QTRY(envP, errorP);
			CONN(envP)->qDisconnect();
		QCATCH;
	}

	int qc_is_dll_connected(void* envP, void* errorP, int* result)
	{

		QTRY(envP, errorP);
			*result = 0;
			*result = CONN(envP)->isDllConnected();
		QCATCH;
	}

	int qc_is_terminal_connected(void* envP, void* errorP, int* result)
	{
		QTRY(envP, errorP);
			*result = 0;
			*result = CONN(envP)->isQuikConnected();
		QCATCH;
	}

	int qc_init(
		void* envP, 
		void* errorP, 
		char* ddeServiceName, 
		PDDEITEM ddeInitialItemListP, 
		PQCFUN callbackFun,
		PQCCONNECTSTATUSFUN callbackConnectFun,
		PQCTRANSSTATUSFUN callbackTransactFun
	)
	{
		QTRY(envP, errorP);
			if (ddeServiceName==NULL)
				return QC_INVALID_SERVICE_NAME;

		
			CONN(envP)->initialize(
				ddeServiceName,
				CONN(envP)->convertToItemList(ddeInitialItemListP),
				callbackFun,
				callbackConnectFun,
				callbackTransactFun
			);
			
		QCATCH;
	}

	int qc_deinit(void* envP, void* errorP)
	{
		QTRY(envP, errorP);
			CONN(envP)->deinitialize();
		QCATCH;
	}

	int qc_async_trans(void* envP, void* errorP, char* transStr)
	{
		QTRY(envP, errorP);
			if (transStr==NULL)
				return QC_INVALID_TRANSACT_STR;
		
			CONN(envP)->qAsynchTransact( transStr );
		QCATCH;
	}


// ------------------------

#ifdef __cplusplus
}
#endif


// any helper functions here
namespace QuikConnector {

	ConnectorErrorInstance::ConnectorErrorInstance():
	quikCode_m(0),
	quikExtCode_m(0)
	{
	}

	

	void ConnectorErrorInstance::setNoError()
	{
		type_m.clear();
		message_m.clear();
		context_m.clear();
		arg_m.clear();
		addInfo_m.clear();
		quikCode_m = 0;
		quikExtCode_m = 0;
		quikError_m.clear();
	}

	void ConnectorErrorInstance::init(QuikApiException& e)
	{
		
		init((CppUtils::Exception&)e);
		quikCode_m = e.getQuikCode();
		quikExtCode_m = e.getQuikExtendedCode();
		quikError_m = e.getQuikError();
	}

	void ConnectorErrorInstance::init(CppUtils::Exception& e)
	{
		setNoError();

		type_m = e.type();
		message_m = e.message();
		context_m = e.context();
		arg_m = e.arg();
		addInfo_m = e.addInfo();
	}

	void ConnectorErrorInstance::init(CppUtils::String const& message)
	{
		setNoError();
		message_m = message;
	}

// --------------------------------------------
	
	ConnectorInstance::ConnectorInstance():
	ddeWrap_m(NULL),
	callback_m(NULL)
	{
	}

	ConnectorInstance::~ConnectorInstance()
	{
		// ignore all errors
		try {
			deinitialize();
		}
		catch(CppUtils::Exception& e)
		{
			LOG( MSG_WARN, CONNECTOR, "Exception in deinitalization of ConnectorInstance: " << e.message());
		}
		catch(...)
		{
			LOG( MSG_WARN, CONNECTOR, "Unknown exception in deinitalization of ConnectorInstance");
		}

		// 
		try {
			clearAll();
		}
		catch(CppUtils::Exception& e)
		{
			LOG( MSG_WARN, CONNECTOR, "Exception in clearing error structures of ConnectorInstance: " << e.message());
		}
		catch(...)
		{
			LOG( MSG_WARN, CONNECTOR, "Unknown exception in clearing error structures");
		}
	}

	void* ConnectorInstance::newErrorInstance()
	{
		LOCK_FOR_SCOPE(*this);

		ConnectorErrorInstance* errP = new ConnectorErrorInstance();
		errorInstances_m.insert(errP);

		return (void*)errP;
	}

	void ConnectorInstance::releaseErrorInstance(void* errP)
	{
		LOCK_FOR_SCOPE(*this);

		ConnectorErrorInstance* errPConnector = reinterpret_cast<ConnectorErrorInstance*>(errP);
		set<ConnectorErrorInstance*>::iterator it = errorInstances_m.find(errPConnector);
		if (it==errorInstances_m.end()) 
			return;
		
		errorInstances_m.erase(it);
		delete errPConnector;

	}

	void ConnectorInstance::onDDEDataPoke(CppUtils::DDEItem const& item, CppUtils::String const& value)
	{
	}

	void ConnectorInstance::onDDEXLTablePoke(CppUtils::DDEItem const& item, CppUtils::Byte const* p, size_t pDataLength)
	{
		if (!callback_m)
			return;

		
		// here the main function - data from QUIK are arrived
		try {
			vector< vector <CppUtils::String> > out;
			CppUtils::crackXtableData(out, p, pDataLength);

			int xTableRows = out.size();

			if (xTableRows > 0) {
				int xTableCols = out[0].size();

				if (xTableCols > 0) {
				  
					DDEITEM cddeitem;
					cddeitem.itemName = (char*)item.item_m.c_str();
					cddeitem.topicName = (char*)item.topic_m.c_str();

					// create array and fill it row by row
					// so access will be xTableDataPPP[irow][icol]
					// read C arrays carefully
					char*** xTableDataPPP = (char***)malloc(xTableRows*sizeof(char**));
					char** buf = (char**)malloc(xTableRows*xTableCols*sizeof(char*));

					// begin iteration
					char **ptr = buf;
					for(int i = 0; i < xTableRows; i++) {
						
						// row begin
						*(xTableDataPPP + i) = ptr;

						for(int j = 0; j < xTableCols; j++) {
							
							*ptr = (char*)out[i][j].c_str();
							
							// advance
							ptr++;
						}
					}
					
					// notify
					try {
						callback_m(
							this,  
							&cddeitem, 
							xTableDataPPP,
							xTableRows,	
							xTableCols
						);
							
					}
					catch(CppUtils::Exception& e)
					{
						LOG( MSG_WARN, CONNECTOR, "Exception in custom callback: " << e.message() << " - " << e.context() << " - " << e.arg() << " - " << e.addInfo() );

					}
					catch(...)
					{
						LOG( MSG_WARN, CONNECTOR, "Unknown exception in custom callback");
					}
					

					// free memory
					free(buf);
					free(xTableDataPPP);
				}

			}
		}
		catch(CppUtils::Exception& e)
		{
			LOG( MSG_WARN, CONNECTOR, "Exception in XLTable data processing handler: " << e.message() << " - " << e.context() << " - " << e.arg() << " - " << e.addInfo() );
		}
		catch(...)
		{
			LOG( MSG_WARN, CONNECTOR, "Unknown exception in XLTable data processing handler");
		}

	}

	void ConnectorInstance::onDDEDataRequested(CppUtils::DDEItem const& item, CppUtils::String& value)
	{
	}

	bool ConnectorInstance::onUnknownDDEItem(CppUtils::DDEItem const& item)
	{
		// always returns true - allow self-registering
		return true;
	}

	void ConnectorInstance::deinitialize()
	{
		LOCK_FOR_SCOPE(*this);

		if (ddeWrap_m) {
			delete ddeWrap_m;
			ddeWrap_m = NULL;
			callback_m = NULL;
			initCallbacks(0, 0);
		}
	}

	void ConnectorInstance::initialize(
		CppUtils::String const& ddeserviceName, 
		CppUtils::DDEItemList const& itemList, 
		PQCFUN callback,
		PQCCONNECTSTATUSFUN callbackConnectFun,
		PQCTRANSSTATUSFUN callbackTransactFun
	)
	{
		LOCK_FOR_SCOPE(*this);

		if (!ddeWrap_m) {
			initCallbacks(callbackConnectFun, callbackTransactFun);
			callback_m = callback;
			ddeWrap_m = new DDEQuikServerWrap(*this, ddeserviceName, itemList );
		}
	}

	void ConnectorInstance::clearAll()
	{
		LOCK_FOR_SCOPE(*this);

		for(set<ConnectorErrorInstance*>::iterator it = errorInstances_m.begin(); it != errorInstances_m.end(); it++) {
			delete reinterpret_cast<ConnectorErrorInstance*>(*it);
		}

	}

	// ------------------------------
	// helpers
	CppUtils::DDEItemList ConnectorInstance::convertToItemList(PDDEITEM ddeInitialItemListP)
	{
		CppUtils::DDEItemList result;
		
		if (ddeInitialItemListP != NULL) {
			PDDEITEM item = ddeInitialItemListP;

			while(item->itemName) {
				CppUtils::String sitem = (item->itemName ? item->itemName : "");
				CppUtils::String stopic = (item->topicName? item->topicName : "");
				result.push_back( CppUtils::DDEItem(stopic, sitem ) );

				item++;
			}

		}

		return result;
	}

}; // end of namespace
