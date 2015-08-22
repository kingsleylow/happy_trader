#ifndef _MTPROXY_MTPROXYCORE_INCLUDED
#define _MTPROXY_MTPROXYCORE_INCLUDED

#include "mtproxydefs.hpp"



// MT specific definitions
#pragma pack(push,1)
struct RateInfo
  {
   unsigned int      ctm;
   double            open;
   double            low;
   double            high;
   double            close;
   double            vol;
  };
#pragma pack(pop)

//
struct MqlStr
{
   int               len;
   char             *string;
};

extern "C" {

// C style functions 
// called on initialization
// starts server pipe 
// shall be single instance over all MT threads
// returns non-zero on success
MTPROXY_EXP int PROCCALL  initialize(char const* serverpipe, char const* globaFileName);

// called on initialization
// stops pipe listener and pipe client
MTPROXY_EXP int PROCCALL  deinitialize();

// called on ticker arrival from MT 
// get commands from input command queue
// returns non-zero on success
// note that memory allocation cannot be perfromed within so we may copy only to the existing buffer
MTPROXY_EXP int PROCCALL  getPendingCommand(char* cmd);

// get the number of times we accessed pending command - just 
// to calculate how much time we are goint to repeat operation
MTPROXY_EXP int PROCCALL  getPendingCounterNumber();

// delete pending command
MTPROXY_EXP int PROCCALL  deletePendingCommand();

// called on ticker arrival from MT 
// write to the pipe the result of execution
// returns non-zero on success
MTPROXY_EXP int PROCCALL  pushCommandResult(char const* remotetpipe, char const* cmdresult);

// called from MT 
// upon start - just to store info
MTPROXY_EXP int PROCCALL pushSymbolMetaInfo(
	char const* symbol, 
	double const pointvalue, 
	int const digit, 
	int const spread
);

// pushes position information
// on already opened (installed or executed) positions
// called at the same place as pushSymbolMetaInfo
MTPROXY_EXP int PROCCALL pushPositionMetaInfo(
	char const* symbol,
	int ticket,
	int ordertype,
	double const installtime,
	double const executetime,
	double const closetime,
	double const installprice,
	double const executeprice,
	double const closeprice,
	double const comission,
	double const lots,
	double const orderpureprofit,
	double const orderpureswap,
	int const isHistory,
	char const* comment,
	double const tpPrice,
	double const slPrice,
	double const orderExpiration
);

// helper functions to hold global information
MTPROXY_EXP int PROCCALL setGlobal(char const* id, char const* value);

MTPROXY_EXP int PROCCALL removeGlobal(char const* id);

MTPROXY_EXP int PROCCALL removeAllGlobals();

MTPROXY_EXP int PROCCALL getGlobal(char const* id, char* value);



}
#endif