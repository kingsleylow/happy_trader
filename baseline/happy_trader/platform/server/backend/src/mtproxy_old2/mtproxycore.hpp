#ifndef _MTPROXY_MTPROXYCORE_INCLUDED
#define _MTPROXY_MTPROXYCORE_INCLUDED

#include "mtproxydefs.hpp"
#include "mtproxymanager.hpp"


// for polling queue
#define MTST_POLLING_NODATA -2
#define MTST_POLLING_OK -3
#define MTST_POLLING_OK_LAST -4
//

#define MTST_GENERAL_ERROR -1
#define MTST_NO_ERROR 0
#define MST_BOOL_TRUE 100
#define MST_BOOL_FALSE 200

#ifdef _DEBUG

#define MTPROXY_VERSION 2014011500

#else
#define MTPROXY_VERSION 20140115

#endif




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
   int len;
   char* string;
};


namespace MtProxy {
	class MqlStrHelper
	{
	public:
		
		
		// retursns the number of copied
		static bool copy(MqlStr* dstMqlStr, char const* srcStr)
		{
			
			if (dstMqlStr && dstMqlStr->string){
				size_t dstBufSize =  strlen(dstMqlStr->string);
				if (dstBufSize > 0) {

					if (srcStr) {
					
						size_t scrStrLen = strlen(srcStr);
						if (scrStrLen ==0) {
							initEmptyString(dstMqlStr);
							// because empty string
							return true;
						}
						else {
							// copy at max dstBufSize
							strncpy(dstMqlStr->string, srcStr, dstBufSize );
							return (scrStrLen <= dstBufSize);
						}

						
					} else {
						initEmptyString(dstMqlStr);
						// because empty string
						return true;
					}
				
				}

			}

			return false;
		}

		

	private:
		static void initEmptyString(MqlStr* dstStr)
		{
			dstStr->len = 0;
			dstStr->string[0] = '\0';
		}

	};
	


}; // end of ns


extern "C" {


// write entries and apply exclusive lock to the file

MTPROXY_EXP int PROCCALL	getVersion();

MTPROXY_EXP int PROCCALL	init(char const* configXml, int hWnd);

MTPROXY_EXP int PROCCALL	deinit();

// sends data to main server
MTPROXY_EXP int PROCCALL	connectToPipeServer( );

MTPROXY_EXP int PROCCALL	disconnectFromPipeServer( );

MTPROXY_EXP int PROCCALL	pushDataToPipeServer( char const* data, MqlStr* response);

MTPROXY_EXP int PROCCALL	getQueueSize( );



// MTST_POLLING_OK_LAST - ok but queue is empty - returned the last available
// MTST_POLLING_OK  - ok
// MTST_GENERAL_ERROR : general error
// MTST_POLLING_NODATA - there is no data after tout_msec

MTPROXY_EXP int PROCCALL	getPollingQueueNextItem(  int const tout_msec, MqlStr* user, MqlStr* password, MqlStr* server);

// returns all kind of data
MTPROXY_EXP int PROCCALL	getPollingQueueNextItem2(  int const tout_msec, MqlStr* data );




// get request depth in days and other parametrs
//MTPROXY_EXP int PROCCALL	getGlobalParameter( int& requestDepthDays);


MTPROXY_EXP int PROCCALL	getTerminalUid(MqlStr* terminal_uid);

// returns if connected to pipe server
// if not neet to tell user that we are not connected
MTPROXY_EXP int PROCCALL isConnectedToPipeServer();


MTPROXY_EXP int PROCCALL getConnectAttemptNumber();

MTPROXY_EXP int PROCCALL callConnectForm(char const* user, char const* password, char const* server );

}

#endif