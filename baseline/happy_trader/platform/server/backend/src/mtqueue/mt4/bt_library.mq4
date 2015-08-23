//+------------------------------------------------------------------+
//|                                                 ht_data_send.mq4 |
//|                        Copyright 2013,          Victor Zoubok    |
//+------------------------------------------------------------------+
#property copyright "Copyright 2013, MetaQuotes Software Corp."
#property link      "http://www.metaquotes.net"




/**
* flags returned by tryLock (expected to be boolean but must comply with DLL_FUN_CALL_OK or DLL_FUN_CALL_FAILURE)
*/

int DLL_FUN_CALL_TRUE   = 1;
int DLL_FUN_CALL_FALSE  = 0;

/** 
* These are log level values. They control how much information is written into log files
* Log files are written into <MT path>\logs. 
*/


int MSG_FATAL  = 0;
int MSG_ERROR  = 1;
int MSG_WARN  = 2;
int MSG_INFO  = 3;
int MSG_DEBUG  = 4;
int MSG_DEBUG_DETAIL  = 5;

/**
*  Imvalid handler constant
*/
int HANDLER_INVALID  = -1;


// buffer size to read strings
#define BUFFERSIZE  256



/** 
* Imported functions
* 
*/
#import "htmtqueue_bck.dll"
   
   
   int setLoglevel(int loglevel);
	 int initEnv();
	 int setEnvParams(int envHndl, string key, string secret, string apiUrl);
   int btc_getInfo(int envHndl);
	 int btc_generalCall(int envHndl, string nonce, string method, string parameters);
	 void freeEnv(int envHndl);
	 void freeResponse(int envHndl, int respHndl);
	 int respGetError(int envHndl, int respHndl, int& statusCode[], string& statusMessage[] );
	 int respStartMessageRead(int envHndl, int respHndl, int& messagelength[]);
	 int respReadMessage(int envHndl, int respHndl, int& buffer[], int bufferLength, int& actualread[]);
	 int makeScreenShot();
#import

//+------------------------------------------------------------------+
//| script program start function                                    |
//+------------------------------------------------------------------+


// helper function t read message
string respReadMessageWrapper(int envHndl, int respHndl)
{
    // read reply
	 int messagelength[1];
	 messagelength[0] = -1;
	 if (respStartMessageRead(envHndl, respHndl, messagelength ) == DLL_FUN_CALL_FALSE) {
			Print("respStartMessageRead(...) failed");
			return ("");
	 }
	 
	 int actualRead[1];
	 actualRead[0] = -1;

		 
	 int cBuffer[BUFFERSIZE];
	 string total_text = "";
	 
	 int bytes_buffer_size = BUFFERSIZE*4;
	 
	 while(true) {
	 
			int res = respReadMessage(envHndl, respHndl, cBuffer, bytes_buffer_size, actualRead);
			if (res==DLL_FUN_CALL_FALSE) {
				Print("respReadMessage(...) failed");
				return ("");
			}
			
		
			if(actualRead[0] <=0)
				break;

			// read chunk
			string text = "";
			for(int i = 0; i < BUFFERSIZE; i++)
         {
				text = text + CharToStr(cBuffer[i] & 0x000000FF);
				if(StringLen(text) >= actualRead[0])
        		break;
				text = text + CharToStr(cBuffer[i] >> 8 & 0x000000FF);
				if(StringLen(text) >= actualRead[0])
        		break;
				text = text + CharToStr(cBuffer[i] >> 16 & 0x000000FF);
				if(StringLen(text) >= actualRead[0])
					break;
				text = text + CharToStr(cBuffer[i] >> 24 & 0x000000FF);
				if(StringLen(text) >= actualRead[0])
					break;
			}

			total_text = total_text + text;

	 }
	 
	 return (total_text);
}


int start()
{
		
	 setLoglevel(MSG_DEBUG_DETAIL);

	 int envHndl = initEnv();
	 if (envHndl == HANDLER_INVALID) {
			Print("Cannot init enviroment");
			return(-1);
	 }

	 string key="UEWN3HEZ-PPO4LUOV-A8WG85ZI-0NE27G58-909S03XR";
   string secret = "f6e7296cfc05d47707dd4f7984c022be3e5d2125e064da9a0bdc135c73d19f0f";
	 string api_url = ""; // empty string means default value
	 
	 if (setEnvParams(envHndl, key, secret, api_url) == DLL_FUN_CALL_FALSE ) {
			Print("setEnvParams(...) failed");
			return(-1);
	 }
	 
	  while(!IsStopped()) {

	     int hndl_btc_getInfo = btc_getInfo(envHndl);
	     if (hndl_btc_getInfo == HANDLER_INVALID) {
			    Print("btc_getInfo(...) failed");
			    return(-1);
	     }

	     // status
	     string statusMessage[1];
	     // alocated enough space here
	     statusMessage[0]="                                                                                     "; 

	     int statusCode[1];
	     statusCode[0] = -1;

	     if (respGetError(envHndl, hndl_btc_getInfo, statusCode, statusMessage) == DLL_FUN_CALL_FALSE ) {
			    Print("respGetError(...) failed");
			    return(-1);
		    }
   
	     Print("respGetError(...)-> status code: ",statusCode[0]," status mesage: ", statusMessage[0] );

	     // read reply
	     string reply_str = respReadMessageWrapper(envHndl, hndl_btc_getInfo);
	     Print("respReadMessage(...)->", reply_str );


	     freeResponse(envHndl, hndl_btc_getInfo);
	     
	     Sleep(3000);
	     
	     
	 
	     // another function
	     string nonce = TimeLocal(); 
	     Print("TransHistory call, nonce is: ", nonce);
	     int hndl_ActiveOrders = btc_generalCall(envHndl, nonce, "TransHistory", "");
	     if (hndl_ActiveOrders == HANDLER_INVALID) {
			    Print("btc_generalCall(...) failed");
			    return(-1);
	     }
	     
	 
	 
	     if (respGetError(envHndl, hndl_ActiveOrders, statusCode, statusMessage) == DLL_FUN_CALL_FALSE ) {
			    Print("respGetError(...) failed");
			    return(-1);
		    }
   
	     Print("respGetError(...)-> status code: ",statusCode[0]," status mesage: ", statusMessage[0] );

	     // read reply
	     reply_str = respReadMessageWrapper(envHndl, hndl_ActiveOrders);
	     Print("respReadMessage(...)->", reply_str );


	     freeResponse(envHndl, hndl_ActiveOrders);
	 }

	 freeEnv(envHndl);
   return(0);
}
//+------------------------------------------------------------------+