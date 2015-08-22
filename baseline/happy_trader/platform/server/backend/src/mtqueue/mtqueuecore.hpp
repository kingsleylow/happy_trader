#ifndef _MTQUEUE_MTPROXYCORE_INCLUDED
#define _MTQUEUE_MTPROXYCORE_INCLUDED

#include "mtqueuedefs.hpp"
#include "mtqueuemanager.hpp"
#include "helperstruct.hpp"

// ------------------------
// constants

#define DLL_FUN_CALL_TRUE  1
#define DLL_FUN_CALL_FALSE  0

#define HANDLER_INVALID -1




namespace MtQueue {

	



}; // end of ns


extern "C" {



// set log level
MTQUEUE_EXP void PROCCALL	setLoglevel(int const logLevel);

MTQUEUE_EXP int PROCCALL	makeScreenShot();


// returns enviroment handler
// or HANDLER_INVALID
MTQUEUE_EXP int PROCCALL initEnv();

// set enviroment params
// returns DLL_FUN_CALL_TRUE or DLL_FUN_CALL_FALSE
MTQUEUE_EXP int PROCCALL	setEnvParams(int const envHndl, char* key, char* secret, char* apiUrl);

// call getInfo
// returns response handler
// or HANDLER_INVALID
MTQUEUE_EXP int PROCCALL	btc_getInfo(int const envHndl);


// low level call
// returns response handler
// or HANDLER_INVALI
MTQUEUE_EXP int PROCCALL	btc_generalCall(int const envHndl, char const* nonce, char const* method, char const* parameters);


// get the code of operation
// returs DLL_FUN_CALL_TRUE or DLL_FUN_CALL_FALSE
MTQUEUE_EXP int PROCCALL	respGetError(int const envHndl, int const respHndlr,  int* statusCode, MqlStr* statusString);

// initiates message read and provides message length
// returns DLL_FUN_CALL_TRUE or DLL_FUN_CALL_FALSE
MTQUEUE_EXP int PROCCALL	respStartMessageRead(int const envHndl, int const respHndlr, int* messageLength);

// returns actual number of bytes read to the buffer
// maximum number is buffer lengths
// this is expected to be sequentila read until the end of buffer reached
// call respStartMessageRead - to start reading
// returns DLL_FUN_CALL_TRUE or DLL_FUN_CALL_FALSE
MTQUEUE_EXP int PROCCALL	respReadMessage(int const envHndl, int const respHndlr, char* buffer, int const bufferLength, int* actuallyRead);


// deinitilize env hadler
MTQUEUE_EXP void PROCCALL	freeEnv(int const envHndl);

// deinit resp handler
MTQUEUE_EXP void PROCCALL	freeResponse(int const envHndlr, int const respHndlr);



}

#endif