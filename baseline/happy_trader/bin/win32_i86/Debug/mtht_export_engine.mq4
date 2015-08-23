#property copyright "(C) 2013,2014, Victor Zoubok"
//WWWW#property link      "http://www.metaquotes.net"
#property show_inputs
#include <WinUser32.mqh>

extern string run_script_as_slave_tmpdir_g = "c:\\temp\\foosave.txt";
extern int log_level_g = -1; // if it is -1 it is set according configurations
extern int wait_kill_g = -1; // when this will be killed
// ----------------------------



int MTST_GENERAL_ERROR= -1;
int MTST_NO_ERROR = 0;
int MST_BOOL_TRUE=100;
int MST_BOOL_FALSE=200;


// ---------------------------
// log level consts

int MSG_FATAL  = 0;
int MSG_ERROR  = 1;
int MSG_WARN  = 2;
int MSG_INFO  = 3;
int MSG_DEBUG  = 4;
int MSG_DEBUG_DETAIL  = 5;

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
	int initialize(int dieSec);
	int getTerminalUid( string& terminal_uid );
	int saveFileContent(string &file, string &content);
	void internallog(int const level, string const &content);
   void setloglevel(int const level);
  

#import

// --------------------------------

string getTerminalUidWrapper()
{
	string terminal_uid = "00000000000000000000000000000000";
   getTerminalUid( terminal_uid );

	return (terminal_uid);
}


// --------------------------------
void mt4Log(int const level, string par1)
{
  
   internallog(level, par1);

}

void mt4Log(int const level, string par1, string par2)
{
   string res = par1 + " " + par2;
   internallog(level, res);
}

void mt4Log(int const level, string par1, string par2, string par3)
{
   string res = par1 + " " + par2  + " " + par3;
   internallog(level, res);
}

void mt4Log(int const level, string par1, string par2, string par3, string par4)
{
   string res = par1 + " " + par2  + " " + par3 + " " + par4;
   internallog(level, res);
}

void mt4Log(int const level, string par1, string par2, string par3, string par4, string par5)
{
   string res = par1 + " " + par2  + " " + par3 + " " + par4 + " " + par5;
   internallog(level, res);
}
void mt4Log(int const level, string par1, string par2, string par3, string par4, string par5, string par6)
{
   string res = par1 + " " + par2  + " " + par3 + " " + par4 + " " + par5 + " " + par6;
   internallog(level, res);
}



// -------------------------------

int Parent()
{
    int hwnd = WindowHandle(Symbol(), Period());
    int hwnd_parent = 0;
    while (!IsStopped())  {
       hwnd = GetParent(hwnd);   
       if (hwnd == 0) 
         break;
       
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

string api_getOrderTypeCodeAsString(int const code) {

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
         
         //mt4Log(MSG_INFO, "Exporting data, account info: ", AccountBalance());
         //mt4Log(MSG_INFO, "Exporting data, connected: ", IsConnected());
         //mt4Log(MSG_INFO, "Exporting data, server: ", AccountServer());
         
  
         result = StringConcatenate(result, "{");   
         result = StringConcatenate(result, api_json_string_key_value("timestamp", TimeCurrent() ) ); 
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

      
         string result =  "{" + api_json_string_key_value("TIMESTAMP", TimeCurrent() ) + ", ";   
         result = result + "\"ORDER_LIST\" : [";
         int collection_size = 0;
         
         int total=OrdersTotal();  
        
         for(int pos=0; pos < total; pos++ ){
            if(OrderSelect(pos,SELECT_BY_POS, MODE_TRADES)==false) 
               continue;
            
            if (collection_size == 0) {
               result = result +  api_GetAllHistory_heper(requestDepthDays, false);
            }
            else {
               
               result = result + ", " + api_GetAllHistory_heper(requestDepthDays, false);
            }
            
            collection_size++;
            
         }
         
         result =result + "] }";
      
         return (result);   

}
 

 // ---------------------------

string api_GetAllHistoryOrders(int requestDepthDays) {

         string result =  "{" + api_json_string_key_value("TIMESTAMP", TimeCurrent() ) + ", ";   
         result = result + "\"ORDER_LIST\" : [";
         int collection_size = 0;
         
         int total=OrdersHistoryTotal();  
        
         
         for(int pos=0; pos < total; pos++ ){
            if(OrderSelect(pos,SELECT_BY_POS, MODE_HISTORY)==false) 
               continue;
           
            
            if (collection_size == 0) {
               result = result +  api_GetAllHistory_heper(requestDepthDays, false);
            }
            else {
               
               result = result + ", " + api_GetAllHistory_heper(requestDepthDays, false);
            }
            
            collection_size++;
            
            
         }
         
         result =result + "] }";
   

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
    int pending_size = -1;
    
    int attempts = 0;
    while (!(IsConnected() && StringLen(AccountCompany()) > 0) ) {
      mt4Log(MSG_INFO, "Company name invalid, thus invalid packet, repeating...");	 
      Sleep(1000);
      attempts++;
      
      if (attempts >=10)
         return("");
    }
    
   
   
    string result  = "{" + api_json_string_key_value("COMMAND", "DATA_PACKET")+ ", ";
    result = result + api_json_string_key_value("MT_UID", mt_uid )+ ", ";
    result = result +  api_json_integer_key_value("PENDING_SIZE", pending_size )+ ", ";
    
    result = result + " \"DATA\": { ";
    
    
    result = result +  "\"ACCOUNT_INFO\":";   
    result = result +  api_GetAccountInformation();
    result = result +  ", ";  
    
    
    result = result +  "\"OPEN_ORDERS\":";   
    result = result +  api_GetAllOpenedOrders(requestDepthDays);
    result = result +  ", ";  
    
    
    result = result +  "\"HISTORY_ORDERS\":";   
    result = result +  api_GetAllHistoryOrders(requestDepthDays);
    result = result +  " }";  
    result = result +  " }";
   
    return (result);
}




// ----------------------------------------

string collectDataToBeExported(string const& mt_uid)
{
   int request_depth = -1;
   string content_tobe_sent = prepareDataToExport( request_depth, mt_uid );
   
   return (content_tobe_sent);
}



// ----------------------------------------




void OnInit()
{
  
}

// ----------------------------------------

void OnDeinit(const int reason)
{
   
}

// ----------------------------------------

void closeTerminalGracefully()
{
      
      int hwnd_parent=Parent();
      if (hwnd_parent != 0) {
				 
			mt4Log(MSG_INFO, "Slave MT suiciding...");	 
         PostMessageA(hwnd_parent,WM_CLOSE,0,0);
      }
      else {
         mt4Log(MSG_WARN, "Cannot close terminal because parent HWND is not resolved");
      }
      
}

// ----------------------------------------

// check if we run as slave
void checkRunAsSlave()
{
  
      
      // set log level
      //setlogprefix( "mt_child_");
      // setloglevel(log_level_g);
      
      mt4Log(MSG_INFO, "**** Slave MT started processing ****");
      
      //Print("This is slave MT so we export data after login and exit, parent terminal UID: ", run_script_as_slave_parent_mtuid_g);
      //string screen_hint = "Slave MT, parent UID is: [" + run_script_as_slave_parent_mtuid_g + "]" ;
      //putTextOnChart("");
      
      Sleep(2000);
       
      int attempts  = 10;
      while(!IsConnected()) {
         Sleep(1000);
         
         if (IsStopped())
            break;
            
         if (attempts-- <=0)
            break;
      }  
      
      if (IsConnected()) {
         
         string newfileUid = getTerminalUidWrapper();
         string fullPath  = run_script_as_slave_tmpdir_g + "\\" + newfileUid + "_"+AccountNumber() + "_tmp_dat.txt";
         
         //Print("Connection ok, proceeding...");
         string export_data = collectDataToBeExported(newfileUid);
         int export_data_length = StringLen(export_data);
         if (export_data_length > 0) {
            //Print("Got data of length: ", export_data_length, " will save to: ", fullPath);
            
            if (saveFileContent(fullPath, export_data) != MTST_NO_ERROR) {
               mt4Log(MSG_WARN, "Exception on saving result file: " + fullPath);
            }
            
            mt4Log(MSG_INFO, "Slave MT returned length: ", export_data_length, " and saved to: ", fullPath);
         }
         else {
            mt4Log(MSG_WARN, "Slave MT returned zero length, not saving");
         }
         
   
      }
   
   
}

// ----------------------------
// MAIN ENTRY FUNCTION
// ----------------------------
int start()
{
  
	Print("DLL successfully loaded: [", getVersion(), "]");
	
	// die in 40 secs
	initialize(wait_kill_g);
	
	// no need in initialization- just export and that's it
	checkRunAsSlave();
	closeTerminalGracefully();
	
   return(0);
 }



