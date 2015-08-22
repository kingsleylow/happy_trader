#property copyright "(C) 2013, Victor Zoubok"
//WWWW#property link      "http://www.metaquotes.net"
#property show_inputs
#include <WinUser32.mqh>



 
// constants      

//extern int max_connect_attempt_g=10;
extern int dump_period_g = 1;

//extern string target_dir_g = ""; // dir must exist
//extern string accounts_list_g = "c:\\tmp\\export_mt4_settings\\accounts.txt"; // account list file
//extern string remote_collector_pipe_g = "\\\\.\\pipe\\CENTRAL_COLLECTOR_PIPE";
//extern string server_mt_pipe_g = "\\\\.\\pipe\\MT4_CLIENT_PIPE1";

extern string config_file_g="c:\\work\\mt_runner\\data\\ht_settings.xml";


// polling queue constants
int MTST_POLLING_NODATA = -2;
int MTST_POLLING_OK  = -3;
int MTST_POLLING_OK_LAST = -4;


int MTST_GENERAL_ERROR= -1;
int MTST_NO_ERROR = 0;
int MST_BOOL_TRUE=100;
int MST_BOOL_FALSE=200;

string RETURN_MT_DATA_ACCEPTED = "OK";

// log level consts
/*
int MSG_FATAL  = 0;
int MSG_ERROR  = 1;
int MSG_WARN  = 2;
int MSG_INFO  = 3;
int MSG_DEBUG  = 4;
int MSG_DEBUG_DETAIL  = 5;
*/

int COMMAND_TYPE_ORDER = 1;
int COMMAND_TYPE_CONNECT = 2;


// ---------------------
 
 
#import "user32.dll"
  int GetParent( int hWnd );
  int GetDlgItem( int hDlg, int nIDDlgItem );
  int GetLastActivePopup( int hWnd );
  int GetAncestor (int hWnd, int gaFlags);


#import "kernel32.dll"
   void GetLocalTime(int& TimeArray[]);
   
#import "htmtproxy_bck.dll"
  int getVersion();
  int init(string configFile, int hWnd);
  int deinit();
  int connectToPipeServer();
	int disconnectFromPipeServer();
	int pushDataToPipeServer(string data, string& response);
	
	int getPollingQueueNextItem2( int tout_msec, string& data);
	int getTerminalUid( string& terminal_uid );
	int getQueueSize(  );
	int isConnectedToPipeServer();
	int getConnectAttemptNumber();
	int callConnectForm(string user, string name, string password);

#import

 
#define VK_HOME 0x24
#define VK_DOWN 0x28
#define VK_ENTER 0x0D
#define RESCANSERVERS_COMMAND_CODE   37400
#define OPENLOGINWINDOW_COMMAND_CODE 35429

#define BM_CLICK                       0x00F5
//#define MA_ACTIVATE                    1


string getFullTimeStamp()
{
   int TimeArray[4];
   int nYear,nMonth,nDay,nHour,nMin,nSec,nMilliSec;
   
   GetLocalTime(TimeArray);
   //Print(GetLastError());
   nYear=TimeArray[0]&0x0000FFFF;
   nMonth=TimeArray[0]>>16;
   nDay=TimeArray[1]>>16;
   nHour=TimeArray[2]&0x0000FFFF;
   nMin=TimeArray[2]>>16;
   nSec=TimeArray[3]&0x0000FFFF;
   nMilliSec=TimeArray[3]>>16;
   string res = StringConcatenate( nYear,"_", nMonth,"_", nDay,"_", nHour,"_", nMin,"_", nSec,"_", nMilliSec);
   return(res);
}


// --------------------------------


//+------------------------------------------------------------------+
// Helper function for parsing command strings, results resides in g_parsedString.
bool SplitString(string stringValue, string separatorSymbol, string& results[], int expectedResultCount = 0)
{
//	 Alert("--SplitString--");
//	 Alert(stringValue);

   if (StringFind(stringValue, separatorSymbol) < 0)
   {// No separators found, the entire string is the result.
      ArrayResize(results, 1);
      results[0] = stringValue;
   }
   else
   {   
      int separatorPos = 0;
      int newSeparatorPos = 0;
      int size = 0;

      while(newSeparatorPos > -1)
      {
         size = size + 1;
         newSeparatorPos = StringFind(stringValue, separatorSymbol, separatorPos);
         
         ArrayResize(results, size);
         if (newSeparatorPos > -1)
         {
            if (newSeparatorPos - separatorPos > 0)
            {  // Evade filling empty positions, since 0 size is considered by the StringSubstr as entire string to the end.
               results[size-1] = StringSubstr(stringValue, separatorPos, newSeparatorPos - separatorPos);
            }
         }
         else
         {  // Reached final element.
            results[size-1] = StringSubstr(stringValue, separatorPos, 0);
         }
         
         
         //Alert(results[size-1]);
         separatorPos = newSeparatorPos + 1;
      }
   }   
   
   if (expectedResultCount == 0 || expectedResultCount == ArraySize(results))
   {  // Results OK.
      return (true);
   }
   else
   {  // Results are WRONG.
      //Trace("ERROR - size of parsed string not expected.", true);
      return (false);
   }
}
  


// -------------------------------

int Parent()
 {
  int hwnd = WindowHandle(Symbol(), Period());
  int hwnd_parent = 0;
  while (!IsStopped())
   {
    hwnd = GetParent(hwnd);   
    if (hwnd == 0) break;
    hwnd_parent = hwnd;
   }
  return (hwnd_parent);
 }
 
 
// ------------------------------

string typ2str(int typ)
{
   string r = "";
   if (typ == OP_BUY)  r = "BUY";
   if (typ == OP_SELL) r = "SELL";
   return (r);
 }

// ------------------------------


string p2str(double p, int digits)
{
   string s,r;
   s = DoubleToStr(p,digits);
   r = s;
   if (false)
   {
      p = StringFind(s,".",0);
   
      if (p != -1)
      {
         r = StringSubstr(s,0,p)+","+StringSubstr(s,p+1);
      }
   }
   return (r);
}
   

// ---------------------------------

string api_json_string_key_value( string key, string value ) {

   return (StringConcatenate( "\"", key, "\"" , " : ", "\"", value , "\"" ));

}

// ---------------------------

string api_json_double_key_value( string key, double value )
{
   
   return (StringConcatenate( "\"", key, "\"" , " : ", DoubleToStr( value, 5) ));

}


// ------------------------------

string api_json_integer_key_value( string key, int value ) {
   
   return (StringConcatenate( "\"", key, "\"" , " : ", value) );

} 

// --------------------------

string api_json_bool_key_value( string key, bool value ) {
   
   string value_str;
   if (value) value_str = "true"; 
      else 
   value_str = "false";
   return (StringConcatenate( "\"", key, "\"" , " : ", value_str) );

} 

// ------------------------------

string api_getOrderTypeCodeAsString(int code) {

   switch (code) {
      case OP_BUY:
         return ("BUY");
      case OP_SELL:
         return ("SELL");
      case OP_BUYLIMIT:
         return ("BUYLIMIT");         
      case OP_BUYSTOP:
         return ("BUYSTOP");
      case OP_SELLLIMIT:
         return ("SELLLIMIT");
      case OP_SELLSTOP:
         return ("SELLSTOP");
      case 6:
         return ("BALANCE");  //jokers :)
      default:
         return ("N/A");
   }

}

// ----------------------------

string api_GetAccountInformation() {

         string result = "";

  
         result = StringConcatenate(result, "{");   
         result = StringConcatenate(result, api_json_integer_key_value("timestamp", TimeCurrent() ) ); 
         result = StringConcatenate(result, ", ", api_json_bool_key_value("connected", IsConnected() ) );    
         result = StringConcatenate(result, ", ", api_json_string_key_value("balance", AccountBalance() ) );   
         result = StringConcatenate(result, ", ", api_json_string_key_value("credit", AccountCredit() ) );
         result = StringConcatenate(result, ", ", api_json_string_key_value("company", AccountCompany() ) );
         result = StringConcatenate(result, ", ", api_json_string_key_value("currency", AccountCurrency() ) );
         result = StringConcatenate(result, ", ", api_json_string_key_value("equity", AccountEquity() ) );
         result = StringConcatenate(result, ", ", api_json_string_key_value("free_margin", AccountFreeMargin() ) );
         result = StringConcatenate(result, ", ", api_json_string_key_value("free_margin_mode", AccountFreeMarginMode() ) );
         result = StringConcatenate(result, ", ", api_json_integer_key_value("leverage", AccountLeverage() ) );
         result = StringConcatenate(result, ", ", api_json_string_key_value("margin", AccountMargin() ) );
         result = StringConcatenate(result, ", ", api_json_string_key_value("name", AccountName() ) );
         result = StringConcatenate(result, ", ", api_json_string_key_value("number", AccountNumber() ) );
         result = StringConcatenate(result, ", ", api_json_string_key_value("profit", AccountProfit() ) );
         result = StringConcatenate(result, ", ", api_json_string_key_value("server", AccountServer() ) );
         result = StringConcatenate(result, ", ", api_json_string_key_value("stopout_level", AccountStopoutLevel() ) );
         result = StringConcatenate(result, ", ", api_json_string_key_value("stopout_mode", AccountStopoutMode() ) );
         result = StringConcatenate(result, "}");   
   

   return (result);   
}

// -------------------------

string api_GetAllOpenedOrders(int requestDepthDays) {

         string result = "";

    
         result = StringConcatenate(result, "{");   
         result = StringConcatenate(result, api_json_integer_key_value("TIMESTAMP", TimeCurrent() ) );   

         string order_collection = "\"ORDER_LIST\" : [";
         
         int collection_size = 0;
         
         int total=OrdersTotal();  
         for(int pos=0; pos < total; pos++ ){
            if(OrderSelect(pos,SELECT_BY_POS, MODE_TRADES)==false) continue;
            
            string json_str = "";
         
            if ( collection_size > 0 ) {
               json_str = StringConcatenate(json_str, ",");                     
            }

            json_str = StringConcatenate(json_str, api_GetAllHistory_heper(requestDepthDays, false));   
            order_collection = StringConcatenate(order_collection, json_str);
            collection_size ++;
            
         }
         
         order_collection = StringConcatenate(order_collection, "]");
         
         result = StringConcatenate(result, ", ", order_collection);   
         
         result = StringConcatenate(result, "}");   
   

   return (result);   

}
 

 // ---------------------------

string api_GetAllHistoryOrders(int requestDepthDays) {

   string result = "";

    
         result = StringConcatenate(result, "{");   
         result = StringConcatenate(result, api_json_integer_key_value("TIMESTAMP", TimeCurrent() ) );   

         string order_collection = "\"ORDER_LIST\" : [";
         
         int collection_size = 0;
         
         int total=OrdersHistoryTotal();  
         for(int pos=0; pos < total; pos++ ){
            if(OrderSelect(pos,SELECT_BY_POS, MODE_HISTORY)==false) continue;
            
            string json_str = "";
         
            if ( collection_size > 0 ) {
               json_str = StringConcatenate(json_str, ",");                     
            }

            json_str = StringConcatenate(json_str, api_GetAllHistory_heper(requestDepthDays, true));
            order_collection = StringConcatenate(order_collection, json_str);
            
            collection_size ++;
            
         }
         
         order_collection = StringConcatenate(order_collection, "]");
         
         result = StringConcatenate(result, ", ", order_collection);   
         
         result = StringConcatenate(result, "}");   
   

   return (result);   

}

// ------------------------------


string api_GetAllHistory_heper(int requestDepthDays, bool is_history)
{
         // filter by open time for history orders only  - TODO
         bool show_line = true;
            
         string json_str = "";
         
         if (show_line) {
            
            json_str = StringConcatenate(json_str, "{");               
            json_str = StringConcatenate(json_str,       api_json_string_key_value("comment", OrderComment() ) );
            json_str = StringConcatenate(json_str, ", ", api_json_string_key_value("commision", OrderCommission() ) );
            json_str = StringConcatenate(json_str, ", ", api_json_string_key_value("expiration", OrderExpiration() ) );
            json_str = StringConcatenate(json_str, ", ", api_json_string_key_value("magic_number", OrderMagicNumber() ) );
            json_str = StringConcatenate(json_str, ", ", api_json_string_key_value("lots", OrderLots() ) );
            json_str = StringConcatenate(json_str, ", ", api_json_string_key_value("open_price", OrderOpenPrice() ) );
            json_str = StringConcatenate(json_str, ", ", api_json_string_key_value("open_time", OrderOpenTime() ) );
            json_str = StringConcatenate(json_str, ", ", api_json_string_key_value("close_price", OrderClosePrice() ) );
            json_str = StringConcatenate(json_str, ", ", api_json_string_key_value("close_time", OrderCloseTime() ) );
            json_str = StringConcatenate(json_str, ", ", api_json_string_key_value("profit", OrderProfit() ) );
            json_str = StringConcatenate(json_str, ", ", api_json_string_key_value("stop_loss", OrderStopLoss() ) );
            json_str = StringConcatenate(json_str, ", ", api_json_string_key_value("symbol", OrderSymbol() ) );            
            json_str = StringConcatenate(json_str, ", ", api_json_string_key_value("swap", OrderSwap() ) );
            json_str = StringConcatenate(json_str, ", ", api_json_string_key_value("take_profit", OrderTakeProfit() ) );
            json_str = StringConcatenate(json_str, ", ", api_json_string_key_value("ticket", OrderTicket() ) );
            json_str = StringConcatenate(json_str, ", ", api_json_integer_key_value("type", OrderType() ) );
            json_str = StringConcatenate(json_str, ", ", api_json_string_key_value("type_code", api_getOrderTypeCodeAsString(OrderType()) ) );
            json_str = StringConcatenate(json_str, "}");   
         }
         
         return (json_str);
}

// --------------------------------

string prepareDataToExport(int requestDepthDays, string mt_uid)
{   
    int pending_size = getQueueSize(  );
    Print("Number of data in the queue: ", pending_size);
  
    
    string result  = StringConcatenate("{", api_json_string_key_value("COMMAND", "DATA_PACKET"), ", ");
    result = StringConcatenate(result, api_json_string_key_value("MT_UID", mt_uid ), ", ");
    result = StringConcatenate(result, api_json_integer_key_value("PENDING_SIZE", pending_size ), ", ");
    
    result = StringConcatenate(result," \"DATA\": { " );
    
    
    result = StringConcatenate(result, "\"ACCOUNT_INFO\":");   
    result = StringConcatenate(result, api_GetAccountInformation());
    result = StringConcatenate(result, ", ");  
    
    
    result = StringConcatenate(result, "\"OPEN_ORDERS\":");   
    result = StringConcatenate(result, api_GetAllOpenedOrders(requestDepthDays));
    result = StringConcatenate(result, ", ");  
    
    
    result = StringConcatenate(result, "\"HISTORY_ORDERS\":");   
    result = StringConcatenate(result, api_GetAllHistoryOrders(requestDepthDays));
    result = StringConcatenate(result, " }");  
    result = StringConcatenate(result, " }");
    
    
  
   
   return (result);
}

// ----------------------------------------

bool makeExport(  string mt_uid)
{
   string accPrefix = AccountNumber();
   int request_depth = -1;
    
   
   
   string content_tobe_sent = prepareDataToExport( request_depth, mt_uid );
   
   
   // attempt to push to pipe
	 // response
	 string result = "m                                                                     ";


   if (pushDataToPipeServer(content_tobe_sent, result) != MTST_NO_ERROR) {
      
      Print("Error pushing data to pipe server, exiting...");
			return (false );
   }
   
   if (result != RETURN_MT_DATA_ACCEPTED) {
   
      Print("Pipe server returned error: [", result, "], exiting...");
			return (false );
   }
   
   Print("Exported:", accPrefix, " OK");
	 return (true );
  
}

// ----------------------------------------

int connectToUserPass2(string login, string password, string server)
{
	// another DLL version
	if (MTST_NO_ERROR == callConnectForm(login, password, server)) {
		return (0);
	}
	Print("callConnectForm(...) failure");
	return (-1);
}

string getTerminalUid_W()
{
	string terminal_uid = "00000000000000000000000000000000";
  getTerminalUid( terminal_uid );

	return (terminal_uid);
}

// ----------------------------------------

int process_export_only(string login, string password,  string server, string mt_uid) {

	Print("Trying to export data without reconnect for: ", login, "/" , "***", "/", server);

	if (!IsConnected()) {
		Print("Though we can reuse old connect data, connection lost, will try to reconnect");
		return (-1);
	}		

	if (!makeExport(  mt_uid )) {
				Print("Export failed");
				return (-1);
	 }
	 Print("Finished export without reconnect");
	 return (0);

}

int process(string login, string password,  string server, string mt_uid)
{
			Print("Trying to export data with reconnect for: ", login, "/" , "***", "/", server);

      // initial connection
      if (connectToUserPass2(login, password, server) == 0) {
      
         
         // reconnect
         int mattempts = getConnectAttemptNumber();
         while(!IsConnected()&& !IsStopped() && mattempts > 0) {
         
            Sleep(1*1000);  
         
            // another connect attempt
            Print("Waiting..., attempts left: ", mattempts);
            mattempts--;
         }
      
         if (mattempts <=0 && !IsConnected()&& !IsStopped()) {
            Print("User failed: ", login, ", continue...");
            return (-1);
         } else {
            // export
            Print("Performing export for a user: ", login);
            if (!makeExport(  mt_uid )) {
							 Print("Export failed");
							 return (-1);
						}
         }
      
         
         
      } else {
         Print("Error attempting to work with login: ", login);
         return (-1);
      }
      
			Print("Finished export");
      return (0);
}


// ----------------------------------------

int processReturnedComand(string raw_data, string& user, string& password, string& server, string& lifetime, string& order_info )
{
	string results[1];
	bool r = SplitString(raw_data, ";", results);
	if (!r) {
		return (-1);
	}

	//Print("array size:", ArraySize(results));

	if (results[0] == "CONNECT_USER") {
		// user;ppassword;server;lifetime
		if (ArraySize(results) != 5)
			return (-1);

		user = results[1];
		password = results[2];
		server = results[3];
		lifetime = results[4];

		return(COMMAND_TYPE_CONNECT);

		
	}
	else if (results[0] == "ORDER") {
		if (ArraySize(results) != 2)
				return (-1);

		order_info =  results[1];
		return(COMMAND_TYPE_ORDER);
	}
	else
		return (-1);
}

// ----------------------------------------

string initString(int blocks) {
	string r;

	for(int i = 0; i < blocks; i++) {
		r = r + "0123456789";
	}

	return(r);
}

// ----------------------------------------


// ----------------------------------------

// ----------------------------
// MAIN ENTRY FUNCTION
// ----------------------------
int start()
{
  
	Print("***** DLL loaded: ", getVersion(), " *****");
	
  // server_mt_pipe_g
  if(init(config_file_g, Parent()) != MTST_NO_ERROR) {
      Print("Cannot initialize");
      return(0);
  }
	/*
	// another DLL version
	if (MTST_NO_ERROR != callConnectForm("492473", "p5qprqp", "AlpariUK-Demo - Micro+Classic")) {
	   Print("callConnectForm(...) failed");
     deinit();
	   return (-1);
	}
	
	 Print("Finishing...");
   deinit();
   return (0);
  */


 
	string terminal_uid = getTerminalUid_W();
  Print("Terminal UID: ", terminal_uid);

  
  // remote_collector_pipe_g
  if (connectToPipeServer() !=MTST_NO_ERROR) {
      Print("Cannot connect to remote pipe");
      deinit();
      return(0);
  }
  
  
  string user_prev = "";
  string password_prev = "";   
	string server_prev = "";

  Print("Started OK");
   
  
  // infinite loop 
  while(!IsStopped()) {
       
   
      int connect_status = isConnectedToPipeServer();
      if (connect_status == MST_BOOL_FALSE) {
          Print("Connection to pipe server lost, exiting...");
          deinit();
          return(0);
      }
      // read accounts data
    
		

			string raw_data = initString(25);
			bool result_get_item = getPollingQueueNextItem2( 2*1000, raw_data);
			
      
      // MTST_GENERAL_ERROR or MST_BOOL_TRUE or MST_BOOL_FALSE
      if (result_get_item == MTST_GENERAL_ERROR) {
         Print("getPollingQueueNextItem2(...) returned error, exiting...");
         deinit();
         return(0);
      }
      // wait for data availabe for 10 seconds
      else if (result_get_item == MTST_POLLING_NODATA) {
       
         Print("No valid data for processing, waiting...");
         continue;
      }
      else if (result_get_item == MTST_POLLING_OK || result_get_item == MTST_POLLING_OK_LAST) {
         Print("getPollingQueueNextItem2(...) result is OK");
      }
      else {
         Print("getPollingQueueNextItem2(...) unexpected result, exiting...");
         deinit();
         return(0);
      }
 
			
			Print("Raw command obtained: ", raw_data);
			  
			string user_i, password_i, server_i, lifetime_i, order_info_i;
			int parse_result = processReturnedComand(raw_data,user_i, password_i, server_i, lifetime_i, order_info_i );
		  
			if (parse_result == COMMAND_TYPE_CONNECT) {
					// check if data are the same
					if (StringLen(user_prev) > 0 && StringLen(password_prev) > 0 && StringLen(server_prev) > 0) {
						if (user_prev == user_i && password_prev == password_i && server_prev == server_i ) {
							if (process_export_only(user_i, password_i, server_i, terminal_uid) ==0) {
								// ok with non reconnect export
								Sleep(1*1000);
							} else {
								// make full reconnect
								process(user_i, password_i, server_i, terminal_uid );
								Sleep(1*1000);
							}
						}
					} else {
							// make full reconnect
							process(user_i, password_i, server_i, terminal_uid );
							Sleep(1*1000);
					}

					// store
					user_prev = user_i;
					password_prev = password_i;   
					server_prev = server_i;
			}
			else if (parse_result == COMMAND_TYPE_ORDER) {
				Print("Order placeholder: ", order_info_i);
				continue;
			}  
			else {
				 Print("Cannot parse command, exit!");
         deinit();
         return(0);
			}
     
			
     
      if (result_get_item == MTST_POLLING_OK_LAST) {
         
         Print("Finished loop waiting...");
         Sleep(dump_period_g *1000 );
         
      }
   }
   
   
   
//----
   
   
   Print("Finishing");
   
   // disconnect from pipe server and deinitializing
   disconnectFromPipeServer();
   deinit();
   
   
   return(0);
 }



