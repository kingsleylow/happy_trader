#include "mtqueuecore.hpp"
#include "helperstruct.hpp"


// global isnatnce
MtQueue::MtQueueManager manager_g;

#define MTQUEUECORE "MTQUEUECORE"

#define TRY \
	try {

#define CATCH_ALL(FUNNAME, RETVAL_FAIL)	\
	}\
	catch(CppUtils::Exception& e)	\
	{\
		LOG(MSG_ERROR, MTQUEUECORE, "Exception in: [" << FUNNAME << "] " << e.message() );\
	}\
	catch(...)\
	{\
		LOG(MSG_ERROR, MTQUEUECORE, "Unknown exception in: [" << FUNNAME << "] " );\
	}\
	return RETVAL_FAIL;

#define CATCH_ALL_RN(FUNNAME)	\
	}\
	catch(CppUtils::Exception& e)	\
	{\
		LOG(MSG_ERROR, MTQUEUECORE, "Exception in: [" << FUNNAME << "] " << e.message() );\
	}\
	catch(...)\
	{\
		LOG(MSG_ERROR, MTQUEUECORE, "Unknown exception in: [" << FUNNAME << "] " );\
	}\


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                                         )
{
        switch (ul_reason_for_call)
        {
        case DLL_PROCESS_ATTACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
                break;
        }
        return TRUE;
}

extern "C" {

	// ------------------
void PROCCALL	setLoglevel(int const logLevel)
{
	CppUtils::Log::setLogLevel(logLevel);
	LOG_FORCE(MTQUEUECORE, "Adjusted log level to: " << CppUtils::Log::getLogLevel());
}


int PROCCALL	makeScreenShot()
{
	TRY 
		manager_g.makeScreenShot();
		return DLL_FUN_CALL_TRUE;
	CATCH_ALL("makeScreenShot", HANDLER_INVALID)
}

int PROCCALL	initEnv()
{
	TRY 
		return manager_g.initEnv();

	CATCH_ALL("initEnv", HANDLER_INVALID)
}

int PROCCALL	setEnvParams(int const envHndl, char* key, char* secret, char* apiUrl)
{
	TRY 
		manager_g.setEnvParams(envHndl, key, secret, apiUrl);
		return DLL_FUN_CALL_TRUE;

	CATCH_ALL("setEnvParams", DLL_FUN_CALL_FALSE)
}

// call getInfo
// returns response handler
int PROCCALL	btc_getInfo(int const envHndl)
{
	TRY 
		int newID  = HANDLER_INVALID;
		manager_g.btc_getInfo(envHndl, newID);
		return newID;

	CATCH_ALL("btc_getInfo", HANDLER_INVALID)
}

int PROCCALL	btc_generalCall(int const envHndl,  char const* nonce, char const* method, char const* parameters)
{
	TRY 
		int newID  = HANDLER_INVALID;
		manager_g.btc_generalCall(envHndl, nonce, method, parameters, newID);

		return newID;

	CATCH_ALL("btc_generalCall", HANDLER_INVALID)
}


int PROCCALL respGetError(int const envHndl, int const respHndlr,  int* statusCode, MqlStr* statusString)
{
	TRY 
		
		int statusCode_p;
		CppUtils::String statusMessage_p;

		manager_g.respGetError(envHndl, respHndlr, statusCode_p, statusMessage_p );
		if (statusCode!=NULL)
			*statusCode = statusCode_p;

		// just copy the maximum what we can
		if (!MtQueue::MqlStrHelper::copy(statusString, statusMessage_p.c_str()))
			LOG(MSG_WARN, MTQUEUECORE, "respGetError(...) -> status message was cut due to small buffer");

		LOG(MSG_DEBUG, MTQUEUECORE, "respGetError(...) -> status message is: \"" << statusMessage_p << "\"");

		return DLL_FUN_CALL_TRUE;

	CATCH_ALL("respGetError", DLL_FUN_CALL_FALSE)
}


int PROCCALL	respStartMessageRead(int const envHndl, int const respHndlr, int* messageLength)
{
	TRY 
		int messageLengthP = -1;
		manager_g.respStartMessageRead(envHndl, respHndlr, messageLengthP);

		if (messageLength)
			*messageLength = messageLengthP;
		return DLL_FUN_CALL_TRUE;
	CATCH_ALL("respStartMessageRead", DLL_FUN_CALL_FALSE)
}

int PROCCALL	respReadMessage(int const envHndl, int const respHndlr, char* buffer, int const bufferLength, int* actuallyRead)
{
		TRY 
			int readBytes = -1;
			manager_g.respReadMessage(envHndl, respHndlr, buffer, bufferLength,readBytes);

			if (actuallyRead)
				*actuallyRead = readBytes;

			return DLL_FUN_CALL_TRUE;
		CATCH_ALL("respStartMessageRead", DLL_FUN_CALL_FALSE)
}

char* PROCCALL	respGetMessage(int const envHndl, int const respHndlr)
{
	TRY 
		//CppUtils::String manager_g.respGetMessage(envHndl, respHndlr);
		return NULL;
	CATCH_ALL("respGetMessage", NULL)
}

// deinitilize any hadler
void PROCCALL	freeEnv(int const envHndl)
{
	TRY 
		manager_g.freeEnv(envHndl);

	CATCH_ALL_RN("freeEnv")
}

void PROCCALL	freeResponse(int const envHndlr, int const respHndlr)
{
	TRY 
		manager_g.freeResponse(envHndlr, respHndlr);

	CATCH_ALL_RN("freeResponse")
}



	
} // 
