#ifndef _QUIK_CONNECTOR_CONNECTOR_API_INCLUDED
#define _QUIK_CONNECTOR_CONNECTOR_API_INCLUDED

#			if defined (QUIK_CONNECTOR_BCK_EXPORTS)
#				define QUIK_CONNECTOR_EXP __declspec(dllexport)
#			else
#				define QUIK_CONNECTOR_EXP __declspec(dllimport)
#			endif


// return codes for functions
#define QC_FUN_OK								0
#define QC_FUN_FAILURE					1
#define QC_INVALID_ENV_HANDLE		2
#define QC_INVALID_ERROR_HANDLE 3
#define QC_INVALID_PATH_TO_TERMINAL 4
#define QC_INVALID_SERVICE_NAME 5
#define QC_INVALID_TRANSACT_STR 6


#define QC_NO_ERROR_DETECTED		100


// -----------------------------

#define CALLBACK __stdcall


// pure C header
#ifdef __cplusplus
extern "C" {
#endif

// ----------------------------------
// that structure contains DDEITEM 
typedef struct _DDEITEM
{
	char* topicName;
	
	char* itemName;

} DDEITEM, *PDDEITEM;

// this is an example of the data to be passed to qc_init
//DDEITEM DEFAULTDDEITEMARRAY1[] = {{"[book1]sheet1", "R1C1:R1C1"}, {"[book1]sheet1", "R1C1:R2C2"}, {0, 0}};


// ----------------------------------
// callback function
// xTableDataPPP - this is C array of array of char*
// so xTableDataPPP[iRow][iCol] - this is the way to access char* data
// xTableRows - the number of rows
// xTableCols - the number of columns
typedef int (CALLBACK QCFUN)
(
	void* envP,  
	PDDEITEM ddeItem, 
	char*** xTableDataPPP,
	int xTableRows,
	int xTableCols
);

typedef QCFUN *PQCFUN;


// ----------------------------------
// callback connect function
// this is called at the time of connect
typedef int (CALLBACK QCCONNECTSTATUSFUN)
(
	void* envP,  
	long nConnectionEvent,
	long nExtendedErrorCode, 
	char* lpcstrInfoMessage
);

typedef QCCONNECTSTATUSFUN *PQCCONNECTSTATUSFUN;

// ----------------------------------
// callback tarnsaction function
// whatever transaction is issued this is called during that time
typedef int (CALLBACK QCTRANSSTATUSFUN)
(
	void* envP,  
	long nTransactionResult, 
	long nTransactionExtendedErrorCode, 
	long nTransactionReplyCode, 
	unsigned long dwTransId, 
	double dOrderNum, 
	char* lpcstrTransactionReplyMessage
);

typedef QCTRANSSTATUSFUN *PQCTRANSSTATUSFUN;

// ----------------------------------
// creates enviroment handle
QUIK_CONNECTOR_EXP int qc_create_enviroment(void** envPP );

// releases also all error handles
// releases enviroment handle + any error handles allocated
QUIK_CONNECTOR_EXP int qc_release_enviroment(void* envP);

// ----------------------------------
// creates error handle
QUIK_CONNECTOR_EXP int qc_get_create_error_handle(void* envP, void** errorPP);

// good programming practice to call that
// releases error handle
QUIK_CONNECTOR_EXP int qc_get_release_error_handle(void* envP, void* errorP);


// ----------------------------
// this returns data residing in error handle
// the char* ptr is allocated inside error handle
// so no need to take care about this
// just get the pointer
QUIK_CONNECTOR_EXP int qc_get_error_msg(void* errorP, char** errorMsgPP);

QUIK_CONNECTOR_EXP int qc_get_error_type(void* errorP, char** errorTypePP);

QUIK_CONNECTOR_EXP int qc_get_error_arg(void* errorP, char** errorArgPP);

QUIK_CONNECTOR_EXP int qc_get_error_ctx(void* errorP, char** errorCtxPP);

QUIK_CONNECTOR_EXP int qc_get_error_quik_code(void* errorP, int* errorQuikCodeP);

QUIK_CONNECTOR_EXP int qc_get_error_quik_ext_code(void* errorP, int* errorQuikExtCodeP);

QUIK_CONNECTOR_EXP int qc_get_error_quik_message(void* errorP, char** errorQuikMsgPP);

// -------------------------

QUIK_CONNECTOR_EXP int qc_connect(void* envP, void* errorP, char* pathToQuik);

QUIK_CONNECTOR_EXP int qc_disconnect(void* envP, void* errorP);

QUIK_CONNECTOR_EXP int qc_is_dll_connected(void* envP, void* errorP, int* result);

QUIK_CONNECTOR_EXP int qc_is_terminal_connected(void* envP, void* errorP, int* result);

// ddeInitialItemList - this is an array of DDEITEM structures
// we pass the address of the first element in that array
// array must be NULL terminated
QUIK_CONNECTOR_EXP int qc_init(
		void* envP, 
		void* errorP, 
		char* ddeServiceName, 
		PDDEITEM ddeInitialItemListP, 
		PQCFUN callbackFun,
		PQCCONNECTSTATUSFUN callbackConnectFun,
		PQCTRANSSTATUSFUN callbackTransactFun
);

// deinitialize whole enviroment
QUIK_CONNECTOR_EXP int qc_deinit(void* envP, void* errorP);

// issues asynch transaction
QUIK_CONNECTOR_EXP int qc_async_trans(void* envP, void* errorP, char* transStr);


// ------------------------------

#ifdef __cplusplus
}
#endif


#endif