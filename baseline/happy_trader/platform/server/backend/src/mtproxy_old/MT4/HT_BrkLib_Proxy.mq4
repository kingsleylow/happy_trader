//+------------------------------------------------------------------+
//|                                              HT_BrkLib_Proxy.mq4 |
//|                      Copyright © 2006, MetaQuotes Software Corp. |
//|                                        http://www.metaquotes.net |
//+------------------------------------------------------------------+
#include <stdlib.mqh>

#include <mtproxy_incl.mqh>


#property copyright "Copyright © 2008, Victor Zoubok"
#property link      "victor.z@tut.by"  

/////////////////////////////
string VERSION="1.1.58";


// constants
extern string mt_pipe_g = "\\\\.\\pipe\\mt4_native_pipe";
extern string remote_pipe_g = "\\\\.\\pipe\\mt4_client_pipe";
extern int failure_attempts_g = 20;


///////////////////////////////
// массив открытых позиций состоянием на предыдущий тик
int pre_OrdersArray[][2]; // [количество позиций][№ тикета, тип позиции]

///////////////////////////////
// symbols as there is no function that can get available symbol list
string Currencies_g[] = {"AED", "AUD", "BHD", "BRL", "CAD", "CHF", "CNY", 
                       "CYP", "CZK", "DKK", "DZD", "EEK", "EGP", "EUR",
                       "GBP", "HKD", "HRK", "HUF", "IDR", "ILS", "INR",
                       "IQD", "IRR", "ISK", "JOD", "JPY", "KRW", "KWD",
                       "LBP", "LTL", "LVL", "LYD", "MAD", "MXN", "MYR",
                       "NOK", "NZD", "OMR", "PHP", "PLN", "QAR", "RON",
                       "RUB", "SAR", "SEK", "SGD", "SKK", "SYP", "THB",
                       "TND", "TRY", "TWD", "USD", "VEB", "XAG", "XAU",
                       "YER", "ZAR"}; 
string Symbols_g[];
int    SymbolCount_g = 0; 

  
///////////////////////////////////////
// Log level

int ERROR   =  0;
int WARN    =  1;
int OK      =  2;
int DEBUG   =  3;   

// log string presentation
string logLevelsStr_g[3] = {"ERROR", "WARN ", " OK  ", "DEBUG"};

// Log level
extern int GLOBAL_LOG_LEVEL=3;

///////////////////////////////////////
// log global variable
int log_m = -1;


/////////////////////////////////
// Command type
int CMD_ORDER_NOTRECOGN = 0;
int CMD_ORDER_SEND   = 1;
int CMD_ORDER_MODIFY = 2;		
int CMD_ORDER_CANCEL = 3;
int CMD_ORDER_CLOSE  = 4; 
int CMD_ORDER_MODIFY_EXECUTE  = 5; 
int CMD_ORDER_HANDSHAKE = 100; // just handshake


/////////////////////////////
int MAX_ARRAY_SIZE=45;

/////////////////////////////

int DELIMIT_CHAR = ',';


//
int init()
{
   doLog(OK, "******Attempting to initialize****** " + VERSION);
   
   int result = 0;
   
   //string global_file="";
   //doLog(OK, "the global entries file name : " + global_file);
   
      
   // pipe
   result = initialize(mt_pipe_g,"");
   if (result <=0) {
      doLog(ERROR, "Pipe listener failed: " + mt_pipe_g);
      Comment("Pipe listener failed: " + mt_pipe_g);
      return(-1);
   }
   doLog(OK, "Pipe listener started on: " + mt_pipe_g);
   
   
   // creates symbol list
   createSymbolList();
   doLog(OK, "The number of symbols provided: " + SymbolCount_g);
   
   result = 0;
   
   // just call to store symbol metainfo
   for ( int i = 0; i < SymbolCount_g; i++) {
      double point =MarketInfo(Symbols_g[i],MODE_POINT);
      int    digits=MarketInfo(Symbols_g[i],MODE_DIGITS);
      int    spread=MarketInfo(Symbols_g[i],MODE_SPREAD);
      
      if (point > 0 && digits> 0 && spread > 0) {
         result = pushSymbolMetaInfo(Symbols_g[i], point, digits, spread);
         if (result > 0) {
            doLog(OK, "Successfully saved metainfo for symbol: " + Symbols_g[i]);
            continue;
         }
      }
      
      doLog(ERROR, "Cannot save metainfo for symbol: " + Symbols_g[i]);
      

   }
   
   doLog(OK, "Will update position list");
   result = updatePositionList();
   if (result <=0) {
      doLog(ERROR, "Cannot update position list");
      Comment("Cannot update position list");
      return(-1);
   }
   
   checkEvents();
   doLog(OK, "Updated position list succesfully and checked for new events");
   
   Comment("Initialized: " + VERSION);
   return(0);
}


int deinit()
{
   // 
   int result = 0;
   result = deinitialize();
   if (result <=0) {
      doLog(ERROR, "Deinitialization failed");
      Comment("Deinitialization failed");
      closeLog();
   }
   
   doLog(OK, "******Session ended******");
   closeLog();
   return(0);
   
}

int start()
 {

   // this will read input queue file, process all the commands
   // then write output data to out queue
   doLog(OK, "          >>>>>> Ticker arrived >>>>>>");
   
   // update position list
   int result = updatePositionList();
   if (result <=0) {
      doLog(ERROR, "Cannot update position list, will not go further...");
      Comment("Cannot update position list, will not go further...");
      return(-1);
   }
   
   checkEvents();
   doLog(OK, "Updated position list succesfully and checked for new events");
      
   
   string curInpLine[ ];
   string curOutLine[ ];
   
   // set up max array size
   ArrayResize(curInpLine, MAX_ARRAY_SIZE);
       
  
   int cnt = 0;
   while (true) { 
      // initialize array 
      for(int i = 0; i < MAX_ARRAY_SIZE; i++) 
         curInpLine[ i ] = "";
      
    
      // need to have enough memory
      
      string line = "                                                                                                                                                                                                    ";
      if (getPendingCommand(line) <=0 )
         break;
      
      if (StringLen(line) <= 0)
         break;
    
      doLog(DEBUG, "Read the command: \"" + line + "\"");
      parseCommaDelimied(line, curInpLine);
         
      processCommand(curInpLine);
      
      cnt++;
      
   }
   
   doLog(OK, "Processed "+cnt+" commands");
   
      
   return(0);
 }
  



// helper to parse this
void parseCommaDelimied(string line, string &curInpLine[]) {
   // read whole line up to 
    for(int i = 0; i < MAX_ARRAY_SIZE; i++) {
      curInpLine[ i ] = "";
    }
    
    
    string line_process = line;
    int line_length = StringLen(line);
    if (line_length <= 0)
      return;
      
    int c = StringGetChar(line, line_length-1);
    if (c != DELIMIT_CHAR)
      line_process = line + ",";
    else
      line_process = line;
      
    line_length = StringLen(line_process);
      
    int token = 0;  
    int prev = 0;
    for(i = 0; i < line_length; i++) {
      c = StringGetChar(line_process, i);
      if (c==DELIMIT_CHAR)  {
      
         string token_str = "";
         if (i>prev)
            token_str = StringSubstr(line_process, prev, i-prev);
            
         curInpLine[ token ] = token_str;
         
         //doLog(DEBUG, "Token number: " + token + " : " + token_str);
         //
         prev = i+1;
         token++;
         
         if (token > MAX_ARRAY_SIZE )
            return;
      }
    }
    
}

// process command reading array
// parseFileLine
void processCommand(string &curInpLine[] ) {
      
   bool cmd_success = false;
   int commandId = StrToInteger(curInpLine[ 0 ]);
   string broker_session="";
   
   // log
   int cmd = -1;
   string cmdResponse = "";  
     
   if (commandId==CMD_ORDER_SEND) {
      
	      // parameters: symbol, cmd, volume, price, slippage, stoploss, takeprofit, comment, magic, expiration
	      //int OrderSend( string symbol, int cmd, double volume, double price, int slippage, double stoploss, double takeprofit, string comment=NULL, int magic=0, datetime expiration=0, color arrow_color=CLR_NONE) 
	      string symbol = curInpLine[ 1 ];
	      cmd = StrToInteger(curInpLine[ 2 ]);
	      double volume = StrToDouble(curInpLine[ 3 ]);
	      double price = StrToDouble(curInpLine[ 4 ]);
	      double slippage = StrToDouble(curInpLine[ 5 ]);
	      double stoploss = StrToDouble(curInpLine[ 6 ]);
	      double takeprofit = StrToDouble(curInpLine[ 7 ]);
	      string comment = curInpLine[ 8 ];
	      int magic = StrToInteger(curInpLine[ 9 ]);
	      double expiration = StrToDouble(curInpLine[ 10 ]);
	      string orderId = curInpLine[ 11 ];
	      broker_session = curInpLine[ 12 ];
	      
	      
	      // in case of delayed order store order install time & order install price
	      // otherwise comment is ignored
	      //if (
	      //  cmd==OP_BUYLIMIT ||
	      //  cmd==OP_SELLLIMIT ||
	      //  cmd==OP_BUYSTOP ||
	      //  cmd==OP_SELLSTOP
	      //) {
	        // this should be install time and install price
	      //  comment = TimeCurrent() + "," + price+",";
	      //}
	      
	      
	      // make some default parameters processing
	      if (price <= 0) {
	        if (cmd==OP_BUY) {
	           price = Ask;
	        }
	        else if (cmd == OP_SELL) {
	           price = Bid;
	        }
	       
	      }
	      
	      if (slippage <= 0) 
	        slippage = 3;
	      
	      if (stoploss <=0)
	        stoploss = 0;
	      
	      if (takeprofit <=0)
	          takeprofit = 0;
	      
	      if (expiration <=0)
	        expiration = 0;
	      
         // parameters: symbol, cmd, volume, price, slippage, stoploss, takeprofit, comment, magic, expiration
         
         doLog(DEBUG, "MQL call:" + "OrderSend(" + symbol +","+ cmd+","+ volume+","+ price+","+ slippage+","+ stoploss+","+ takeprofit+","+ comment+","+ magic+","+ expiration+","+ "CLR_NONE" + ")");
	      int order_ticket = OrderSend( symbol, cmd, volume, price, slippage, stoploss, takeprofit, comment, magic, expiration, CLR_NONE); 
	      
	      
	      
	      
	      //  create response here 
	     	cmdResponse = createOrderSendResponse(order_ticket, orderId, cmd_success);    
	     	
	     	// save order ID
	     	if (order_ticket > 0) 
            saveOuterId(orderId, order_ticket);
      
	     	doLog(DEBUG, "OrderSend(...) executed with result:" + order_ticket);  
	      
	      
	      // further we are passing other stuff which is necessary
	      
	   }
	   else if (commandId==CMD_ORDER_MODIFY) {
	       // parameters: ticket, price, stoploss, takeprofit, expiration
	       int ticket = StrToInteger(curInpLine[ 1 ]);
	       price = StrToDouble(curInpLine[ 2 ]);
	       stoploss = StrToDouble(curInpLine[ 3 ]);
	       takeprofit = StrToDouble(curInpLine[ 4 ]);
	       expiration = StrToDouble(curInpLine[ 5 ]);
	       orderId = curInpLine[ 6 ];
	       broker_session = curInpLine[ 7 ];
	       
	       // make default adjustements
	       if (stoploss <=0)
	        stoploss = 0;
	      
	      if (takeprofit <=0)
	          takeprofit = 0;
	      
	      if (expiration <=0)
	        expiration = 0;
	        
	        
	       doLog(DEBUG, "MQL call:" + "OrderModify(" + ticket +","+ price+","+ stoploss+","+ takeprofit+","+ expiration + ","+ "CLR_NONE" + ")");
	       bool result_modify = OrderModify( ticket, price, stoploss, takeprofit, expiration, CLR_NONE); 
	       
	       
	       // in case of delayed order store order install time & order install price
	       // otherwise comment is ignored
	       //if (
	       // cmd==OP_BUYLIMIT ||
	       // cmd==OP_SELLLIMIT ||
	       // cmd==OP_BUYSTOP ||
	       // cmd==OP_SELLSTOP
	       //) {
	          // this should be install time and install price
	       //   comment = TimeCurrent() + "," + price+",";
	       //}
	       
	       
	       
	       //  create response here
	       cmdResponse = createOrderModifyResponse(ticket, result_modify, orderId, cmd_success);
	       doLog(DEBUG, "OrderModify(...) executed with result:" + result_modify);
	  }     
	  else if (commandId==CMD_ORDER_CANCEL ) {
	       ticket = StrToInteger(curInpLine[ 1 ]);     
	       orderId = curInpLine[ 2 ];
	       broker_session = curInpLine[ 3 ];
	       
	       doLog(DEBUG, "MQL call:" + "OrderDelete(" + ticket + ")");

	       bool result_delete = OrderDelete( int ticket ); 
	       
	       //  create response here
	       cmdResponse = createOrderDeleteResponse(ticket, result_delete, orderId, cmd_success);
	       
	       doLog(DEBUG, "OrderDelete(...) executed with result:" + result_delete);
	       
	       
	  }
	  else if (commandId==CMD_ORDER_CLOSE ) {
	  
	     // depending whether order is executed
	     ticket = StrToInteger(curInpLine[ 1 ]);   
	     double lots = StrToDouble(curInpLine[ 2 ]);
	     price = StrToDouble(curInpLine[ 3 ]);
	     slippage = StrToDouble(curInpLine[ 4 ]);
	     orderId = curInpLine[ 5 ];
	     broker_session = curInpLine[ 6 ];
	     
	     // default adjustements
	     if (slippage <= 0) 
	        slippage = 3;
	        
	     // default price   
	     if (price <= 0) {
	        if (OrderSelect( ticket, SELECT_BY_TICKET , MODE_TRADES )) {
	           int ord_type = OrderType( );
	           
	           if (ord_type==OP_BUY)
	              price = Bid;
	           else if (ord_type==OP_SELL)
	              price = Ask;
	        }
	     }
	     
	     
	     // return default lots number
	     if (lots <=0) {
	        if (OrderSelect( ticket, SELECT_BY_TICKET , MODE_TRADES )) {
	           lots = OrderLots();
	        }
	     }
	        
	      
	     
	     doLog(DEBUG, "MQL call:" + "OrderClose(" + ticket +","+ lots+","+ price+","+ slippage + ")");

	     bool result_close = OrderClose( ticket, lots, price, slippage ); 
	     
	     //  create response here
	     cmdResponse = createOrderCloseResponse(ticket, result_close, orderId, cmd_success);
	     
	     // string description
	     doLog(DEBUG, "OrderClose(...) executed with result:" + result_close);
	 }  
	 else if (commandId == CMD_ORDER_HANDSHAKE) {
	   string handshake_data = curInpLine[ 1 ];
	   orderId = curInpLine[ 2 ];
	   broker_session = curInpLine[ 3 ];
	   
	   
	   // response in the form orderId, handshake_data, broker_session
	   cmdResponse = createHandshakeResponse(handshake_data, orderId, cmd_success);
	   doLog(DEBUG, "Handshake response was sent with data:" + handshake_data);
	   
	 }
    else {
        // not recognized
        int code = -1000;
        cmdResponse = CMD_ORDER_NOTRECOGN + "," + code + "," + commandId +",";
        doLog(ERROR, "Command not recognized: " + commandId);
        
        // assume successfull
        cmd_success = true;
    }
   
   
   //
   sendResponseToClient(cmdResponse, broker_session, remote_pipe_g);
   //
   
   
   // remove if success
   if (!cmd_success) {
      int cntc = getPendingCounterNumber();   
      if (cntc <= failure_attempts_g ) {
         doLog(ERROR, "Cannot delete the command from the queue as it was not executed correctly and the number of attempts did not exceeded: " + cntc);
      }
      else {
         int resdel = deletePendingCommand();
         doLog(WARN, "Removed the command from the queue as the number of attempts exceeded: " + cntc);
      }
   }
   else {
      deletePendingCommand();
      doLog(OK, "Removed the command from the queue as operation finished successfully");
   }
   
   
}

void sendResponseToClient(string cmdResponse, string sessionId, string client )
{
   int result = 0;
   
   // add broker session here
   string to_send = cmdResponse + sessionId + ",";
   
   
   result = pushCommandResult(client, to_send);
   if (result <=0) {
      doLog(ERROR, "Cannot write command \"" + to_send + "\" to remote pipe: \"" + client + "\"");
   }
   else {
      doLog(OK, "Successfully wrote command \"" + to_send + "\" to remote pipe: \"" + client + "\"");
   }
   
   
      
   
}

// create response for OrderSend function
string createOrderSendResponse(int order_ticket, string orderId, bool& cmd_success )
{
 
   int error_code =  0;
   string error_descr = "";
   
   string response = CMD_ORDER_SEND + "," + orderId + ",";
   
   cmd_success = true;
   if (order_ticket < 0) {
       error_code =  GetLastError();
       error_descr = ErrorDescription(error_code);
       cmd_success = false;
   }
   
   
   response = response + order_ticket + "," + error_code + "," + error_descr + ",";
   
   
   if (OrderSelect( order_ticket, SELECT_BY_TICKET , MODE_TRADES  )) {
      response =  response + OrderType( ) + ",";
      response =  response + OrderSymbol( ) + ",";
      response =  response + OrderOpenPrice( ) + ",";
      response =  response + OrderOpenTime( ) + ",";
	   response =  response + OrderExpiration( ) + ",";
	   response =  response + OrderTakeProfit( ) + ",";
	   response =  response + OrderStopLoss( ) + ",";
      response =  response + OrderCommission( ) + ",";
      response =  response + OrderLots( ) + ",";
      
   }
   else {
      response =  response + ",";
      response =  response + ",";
      response =  response + ",";
      response =  response + ",";
	   response =  response + ",";
	   response =  response + ",";
	   response =  response + ",";
      response =  response + ",";
      response =  response + ",";
   }
   
   return (response);
}

// create response for OrderSend function
string createOrderModifyResponse(int order_ticket, bool result_modify, string orderId, bool& cmd_success )
{
   
   int error_code =  0;
   string error_descr = "";
   
   string response = CMD_ORDER_MODIFY + "," + orderId + ",";
   
   cmd_success = true;
   if (!result_modify) {
       error_code =  GetLastError();
       error_descr = ErrorDescription(error_code);
       cmd_success = false;
   }
   
   
   response = response + order_ticket + "," + error_code + "," + error_descr + ",";
   
   if (OrderSelect( order_ticket, SELECT_BY_TICKET , MODE_TRADES  )) {
      response =  response + OrderType( ) + ",";
      response =  response + OrderSymbol( ) + ",";
      response =  response + OrderOpenPrice( ) + ",";
	   response =  response + OrderExpiration( ) + ",";
	   response =  response + OrderTakeProfit( ) + ",";
	   response =  response + OrderStopLoss( ) + ",";
      response =  response + OrderCommission( ) + ",";
      response =  response + OrderLots( ) + ",";
   }
   else {
      response =  response + ",";
      response =  response + ",";
      response =  response + ",";
	   response =  response + ",";
	   response =  response + ",";
	   response =  response + ",";
      response =  response + ",";
      response =  response + ",";
   }
   
   return (response);
}


// create response when order was executed
string createOrderExecuteResponse(int order_ticket, bool result_modify, string orderId, bool& cmd_success )
{
   
   int error_code =  0;
   string error_descr = "";
   
   string response = CMD_ORDER_MODIFY_EXECUTE + "," + orderId + ",";
   
   cmd_success = true;
   if (!result_modify) {
       error_code =  GetLastError();
       error_descr = ErrorDescription(error_code);
       cmd_success = false;
   }
   
   
   response = response + order_ticket + "," + error_code + "," + error_descr + ",";
   
   if (OrderSelect( order_ticket, SELECT_BY_TICKET , MODE_TRADES  )) {
      response =  response + OrderType( ) + ",";
      response =  response + OrderSymbol( ) + ",";
      response =  response + OrderOpenPrice( ) + ",";
	   response =  response + OrderExpiration( ) + ",";
	   response =  response + OrderTakeProfit( ) + ",";
	   response =  response + OrderStopLoss( ) + ",";
      response =  response + OrderCommission( ) + ",";
      response =  response + OrderLots( ) + ",";
   }
   else {
      response =  response + ",";
      response =  response + ",";
      response =  response + ",";
	   response =  response + ",";
	   response =  response + ",";
	   response =  response + ",";
      response =  response + ",";
      response =  response + ",";
   }
   
   return (response);
}


// create response for OrderDelete function
// note that this is cancel non-executed order 
string createOrderDeleteResponse(int order_ticket, bool result_delete,  string orderId, bool& cmd_success )
{
   int error_code =  0;
   string error_descr = "";
   
   string response = CMD_ORDER_CANCEL + "," + orderId + ",";
   
   cmd_success = true;
   if (!result_delete) {
       error_code =  GetLastError();
       error_descr = ErrorDescription(error_code);
       cmd_success = false;
   }
   
   
   response = response + order_ticket + "," + error_code + "," + error_descr + ",";
   if (OrderSelect( order_ticket, SELECT_BY_TICKET , MODE_HISTORY  )) {
      response =  response + OrderType( ) + ",";
      response =  response + OrderSymbol( ) + ",";
      
      
   } else {
      response =  response + ",";
      response =  response + ",";
      
   }
   
   return (response);
}

// create response for OrderDelete function
// note that this is cancel non-executed order 
string createHandshakeResponse(string handshakedata, string orderId, bool& cmd_success)
{
   string response = CMD_ORDER_HANDSHAKE + "," + orderId + "," + handshakedata + ",";
   cmd_success = true;
   return (response);
}

string createOrderCloseResponse(int order_ticket, bool result_close , string orderId, bool& cmd_success )
{
   int error_code =  0;
   string error_descr = "";
   
   string response = CMD_ORDER_CLOSE + "," + orderId + ",";
   
   cmd_success = true;
   if (!result_close) {
       error_code =  GetLastError();
       error_descr = ErrorDescription(error_code);
       cmd_success = false;
   }
   
   
   response = response + order_ticket + "," + error_code + "," + error_descr + ",";
   
   // expect this order is selected as historical normally
   if (OrderSelect( order_ticket, SELECT_BY_TICKET , MODE_HISTORY  )) {
      response =  response + OrderType( ) + ","; 
      response =  response + OrderSymbol( ) + ",";
      response =  response + OrderOpenPrice( ) + ",";
      response =  response + OrderOpenTime( ) + ",";
      response =  response + OrderClosePrice( ) + ",";
      response =  response + OrderCloseTime( )  + ",";
	   response =  response + OrderExpiration( ) + ",";
	   response =  response + OrderTakeProfit( ) + ",";
	   response =  response + OrderStopLoss( ) + ",";
      response =  response + OrderCommission( ) + ",";
      response =  response + OrderLots( ) + ",";
   }
   else {
      response =  response + ",";
      response =  response + ",";
      response =  response + ",";
      response =  response + ",";
      response =  response + ",";
	   response =  response + ",";
	   response =  response + ",";
	   response =  response + ",";
	   response =  response + ",";
      response =  response + ",";
      response =  response + ",";
   }
   
   return (response);
}




////////////////////////////////////////
// this updates the position list
// called from init & start
// need to make events here
int updatePositionList()
{  

   int result; 
   
   string order_symbol;
   int order_ticket = -1;
   int order_type = -1;
   double order_install_time = -1;
   double order_execute_time = -1;
   double order_close_time = -1;
      
   double order_install_price = -1;
   double order_execute_price = -1;
   double order_close_price = -1;
      
   double order_pure_profit = -1;
   double order_comision = -1;
   double order_swap = -1;
   
   double order_tp_price = -1;
   double order_sl_price = -1;
      
   double order_lots = -1;
   double order_expiration = -1;
   string order_comment;
   
        
      
   int total_living_orders=OrdersTotal();
   for(int pos=0; pos < total_living_orders; pos++) {
    
      if( !OrderSelect(pos,SELECT_BY_POS,MODE_TRADES) ) {
        doLog(ERROR, "Cannot select living order with #: " + pos);
        continue;
      }
      
    
      
      
      // 
      order_symbol = OrderSymbol( );
      order_ticket = OrderTicket( );
      order_type = OrderType();
      
      order_install_time = -1;
      order_execute_time = -1;
      
      order_install_price = -1;
      order_execute_price = -1;
      
      order_close_price = OrderClosePrice( );
      order_close_time = OrderCloseTime( );
      
      order_pure_profit =  OrderProfit( );
      order_comision = OrderCommission( );
      order_swap = OrderSwap( );
      
      order_lots = OrderLots( );
      order_comment = OrderComment( );
      
      order_tp_price = OrderTakeProfit();
      order_sl_price = OrderStopLoss();
      order_expiration = OrderExpiration();
      
      if (order_type==OP_BUY || order_type==OP_SELL) {
         // executed
         order_execute_time = OrderOpenTime( );
         order_execute_price = OrderOpenPrice( );
         
      }
      else {
         // 
         order_install_time = OrderOpenTime( );
         order_install_price = OrderOpenPrice( );
      }
         
      
      if (order_type >=6)
         continue;
         
          
      // push history
      result = pushPositionMetaInfo(
	     order_symbol,
	     order_ticket,
	     order_type,
	     order_install_time,
	     order_execute_time,
	     order_close_time,
	     order_install_price,
	     order_execute_price,
	     order_close_price,
	     order_comision,
	     order_lots,
	     order_pure_profit,
	     order_swap,
	     0, // alive orders
	     order_comment,
	     order_tp_price,
	     order_sl_price,
	     order_expiration
      );
      
      if (result <=0)
         return (-1);
      
           
      if (StringLen(order_symbol) <=0) order_symbol = "      ";
      
      /*
      doLog(DEBUG, "Living position info:  " + "symbol:" + order_symbol + ", ticker:" + order_ticket + ", type:"+ order_type + ", install time:" + TimeToStr(order_install_time) +
       ", execute time:" + TimeToStr(order_execute_time) + ", close time:" + TimeToStr(order_close_time) + ", install price:" + order_install_price + 
       ", execute price:" + order_execute_price + ", close price:" +order_close_price + 
       ", comission:" +  order_comision + ", lots:" + order_lots + ", pure profit:" + order_pure_profit + ", swap:" + order_swap);
      */
      
    };
    
    int total_archived_orders=HistoryTotal();
    for(pos=0; pos < total_archived_orders; pos++) {
      // archived position info
      if( !OrderSelect(pos,SELECT_BY_POS,MODE_HISTORY ) ) {
        doLog(ERROR, "Cannot select archive order with #: " + pos);
        continue;
      }
      
      order_symbol = OrderSymbol( );
      order_ticket = OrderTicket( );
      order_type = OrderType();
     
      order_install_time = -1; 
      order_install_price = -1;
     
      order_execute_time = OrderOpenTime( );
      order_execute_price = OrderOpenPrice( );
      
      order_close_time = OrderCloseTime( );
      order_close_price = OrderClosePrice( );
      
      order_pure_profit =  OrderProfit( );
      order_comision = OrderCommission( );
      order_swap = OrderSwap( );
      
      order_lots = OrderLots( );
      order_comment = OrderComment( );
      
      order_tp_price = OrderTakeProfit();
      order_sl_price = OrderStopLoss();
      
      order_expiration = OrderExpiration();
      
      if (order_type >=6)
         continue;
      
      // push history
      result = pushPositionMetaInfo(
	     order_symbol,
	     order_ticket,
	     order_type,
	     order_install_time,
	     order_execute_time,
	     order_close_time,
	     order_install_price,
	     order_execute_price,
	     order_close_price,
	     order_comision,
	     order_lots,
	     order_pure_profit,
	     order_swap,
	     1, // history orders
	     order_comment,
	     order_tp_price,
	     order_sl_price,
	     order_expiration
      );
      
      if (result <=0)
         return (-1);
         
            
         
      if (StringLen(order_symbol) <=0) order_symbol = "      ";
       
      /*   
      doLog(DEBUG, "Archive position info: " + "symbol:" + order_symbol + ", ticker:" + order_ticket + ", type:"+ order_type + ", install time:" + TimeToStr(order_install_time) +
       ", execute time:" + TimeToStr(order_execute_time) + ", close time:" + TimeToStr(order_close_time) + ", install price:" + order_install_price + 
       ", execute price:" + order_execute_price + ", close price:" +order_close_price + 
       ", comission:" +  order_comision + ", lots:" + order_lots + ", pure profit:" + order_pure_profit + ", swap:" + order_swap);
      */
      

   } // end of the loop


   return (1);
   
}

// Helper functions


///////////////////////////////////////
// this creates symbol list
int createSymbolList()
  {
   int CurrencyCount = ArrayRange(Currencies_g, 0);
   int Loop, SubLoop;
   string TempSymbol;
   for(Loop = 0; Loop < CurrencyCount; Loop++)
       for(SubLoop = 0; SubLoop < CurrencyCount; SubLoop++)
         {
           TempSymbol = Currencies_g[Loop] + Currencies_g[SubLoop];
           if(MarketInfo(TempSymbol, MODE_BID) > 0)
             {
               ArrayResize(Symbols_g, SymbolCount_g + 1);
               Symbols_g[SymbolCount_g] = TempSymbol;
               SymbolCount_g++;
             }
         }
   return(0);
 }
 
 // write log
void doLog(int level, string data)
{
   if (level <= GLOBAL_LOG_LEVEL) {   
      if (log_m <0) {
         string logname = "ht_brklib_proxy_"+TimeLocal()+".log";
         log_m = FileOpen(logname,FILE_CSV|FILE_WRITE,' ' );
         if (log_m < 1) {
            Comment("Cannot open log file " + logname);
            return;
         }
      }
   
      
      FileWrite(log_m, "[ " + logLevelsStr_g[level] + " ] - [ "+ TimeToStr(CurTime()) + " ] " + data);
      FileFlush(log_m);
   }
}  

// close log
void closeLog()
{
   FileClose(log_m);
   log_m = -1;
}


// Helpers to create position events 




// called upon new ticker arrival
// checks for open/close events
void checkEvents()
{
   doLog(DEBUG, "Will check for events" );
   
	// флаг первого запуска
	static bool first = true;
	// код последней ошибки
	int _GetLastError = 0;
	// общее количество позиций
	int _OrdersTotal = OrdersTotal();
	doLog(DEBUG, "Currently the number of total orders: "+ _OrdersTotal);
	// кол-во позиций, соответствующих критериям (текущий инструмент и заданный MagicNumber),
	// состоянием на текущий тик
	int now_OrdersTotal = 0;
	// кол-во позиций, соответствующих критериям, состоянием на предыдущий тик
	static int pre_OrdersTotal = 0;
	// массив открытых позиций состоянием на текущий тик
	int now_OrdersArray[][2]; // [№ в списке][№ тикета, тип позиции]
	// текущий номер позиции в массиве now_OrdersArray (для перебора)
	int now_CurOrder = 0;
	// текущий номер позиции в массиве pre_OrdersArray (для перебора)
	int pre_CurOrder = 0;

	// массив для хранения количества закрытых позиций каждого типа
	// int now_ClosedOrdersArray[6][3]; // [тип ордера][тип закрытия]
	// массив для хранения количества сработавших отложенных ордеров
	// int now_OpenedPendingOrders[4]; // [тип ордера]

	// временные флаги
	bool OrderClosed = true, PendingOrderOpened = false;
	// временные переменные
	int ticket = 0, type = -1, close_type = -1;

	
	// изменяем размер массива открытых позиций под текущее кол-во
	ArrayResize( now_OrdersArray, MathMax( _OrdersTotal, 1 ) );
	// обнуляем массив
	ArrayInitialize( now_OrdersArray, 0.0 );

	// обнуляем массивы закрытых позиций и сработавших ордеров
	//ArrayInitialize( now_ClosedOrdersArray, 0.0 );
	//ArrayInitialize( now_OpenedPendingOrders, 0.0 );

	//+------------------------------------------------------------------+
	//| Перебираем все позиции и записываем в массив только те, которые
	//| соответствуют критериям
	//+------------------------------------------------------------------+
	for ( int z = _OrdersTotal - 1; z >= 0; z -- )
	{
		if ( !OrderSelect( z, SELECT_BY_POS ) )
		{
			_GetLastError = GetLastError();
			doLog(ERROR, "OrderSelect( "+ z+ ", SELECT_BY_POS ) - Error #" +  _GetLastError );
			continue;
		}
		
		int cur_ticket = OrderTicket();
		int cur_type = OrderType();
		
		doLog(DEBUG, "Current list: ticker="+ticket+" type="+type );
		
		// iterate through all orders
		now_OrdersArray[now_OrdersTotal][0] = cur_ticket;
		now_OrdersArray[now_OrdersTotal][1] = cur_type;
		
		now_OrdersTotal ++;
		
	}
	// изменяем размер массива открытых позиций под кол-во позиций, соответствующих критериям
	ArrayResize( now_OrdersArray, MathMax( now_OrdersTotal, 1 ) );

	//+------------------------------------------------------------------+
	//| Перебираем список позиций предыдущего тика, и считаем сколько закрылось позиций и
	//| сработало отложенных ордеров
	//+------------------------------------------------------------------+
	for ( pre_CurOrder = 0; pre_CurOrder < pre_OrdersTotal; pre_CurOrder ++ )
	{
		// запоминаем тикет и тип ордера
		ticket = pre_OrdersArray[pre_CurOrder][0];
		type   = pre_OrdersArray[pre_CurOrder][1];
		
		doLog(DEBUG, "Previous list: ticker="+ticket+" type="+type );
		
		// предпологаем, что если это позиция, то она закрылась
		OrderClosed = true;
		// предполагаем, что если это был отложенный ордер, то он не сработал
		PendingOrderOpened = false;

		// перебираем все позиции из текущего списка открытых позиций
		for ( now_CurOrder = 0; now_CurOrder < now_OrdersTotal; now_CurOrder ++ )
		{
			// если позиция с таким тикетом есть в списке,
			if ( ticket == now_OrdersArray[now_CurOrder][0] )
			{
				// значит позиция не была закрыта (ордер не был удалён)
				OrderClosed = false;

				// если её тип поменялся,
				if ( type != now_OrdersArray[now_CurOrder][1] )
				{
					// значит это был отложенный ордер, и он сработал
					PendingOrderOpened = true;
				}
				break;
			}
		}
		// если была закрыта позиция (удалён ордер),
		if ( OrderClosed )
		{
		   
			// выбираем её
			if ( !OrderSelect( ticket, SELECT_BY_TICKET ) )
			{
				_GetLastError = GetLastError();
				doLog(ERROR, "OrderSelect( " + z + ", SELECT_BY_TICKET ) - Error #" +  _GetLastError );
				continue;  
			}
			
						
			// и определяем, КАК закрылась позиция (удалился ордер):
			if ( type < 2 )
			{
				// Бай и Селл: 0 - вручную, 1 - СЛ, 2 - ТП
				close_type = 0;
				if ( StringFind( OrderComment(), "[sl]" ) >= 0 ) close_type = 1;
				if ( StringFind( OrderComment(), "[tp]" ) >= 0 ) close_type = 2;
			}
			else
			{
				// Отложенные ордера: 0 - вручную, 1 - время истечения
				close_type = 0;
				if ( StringFind( OrderComment(), "expiration" ) >= 0 ) close_type = 1;
			}
			
			if (!first) {
			   // send notification - order is closed (not deleted as deletion occures only manually)
			   // outer ID and session ID are dummy
			   
			   int order_type = OrderType( );
			   if (order_type==OP_BUY || order_type==OP_SELL) {
			      doLog(DEBUG, "Order closed with ticket: " + ticket);
			   
			      string orderId = resolveOuterId(ticket);
			      bool cmd_success = false;
			      string responseCmd = createOrderCloseResponse(ticket, true, orderId, cmd_success);
			
			      sendResponseToClient(responseCmd, "", remote_pipe_g);
			   }
			   else if (order_type==OP_BUYLIMIT  || order_type==OP_BUYSTOP || order_type==OP_SELLLIMIT  || order_type==OP_SELLSTOP ) {
			      doLog(DEBUG, "Order cancelled with ticket: " + ticket);
			   
			      orderId = resolveOuterId(ticket);
			      cmd_success = false;
			      responseCmd = createOrderDeleteResponse(ticket, true, orderId, cmd_success );
			
			      sendResponseToClient(responseCmd, "", remote_pipe_g);
			   }
			   
			   
			   
			   // here we will remove order ticket from the list
			   removeOuterId(ticket);
			}
			
			
			continue;
		}
		// если сработал отложенный ордер,
		if ( PendingOrderOpened )
		{
			if (!first) {
			   // send notification - order is closed (not deleted as deletion occures only manually)
			   // outer ID and session ID are dummy
			   doLog(DEBUG, "Delayed order executed with ticket: " + ticket);
			   orderId = resolveOuterId(ticket);
            cmd_success = false;
			   responseCmd = createOrderExecuteResponse(ticket, true, orderId, cmd_success);
			
			   sendResponseToClient(responseCmd, "", remote_pipe_g);
			}
			
			
			continue;
		}
	}

	

	//---- сохраняем массив текущих позиций в массив предыдущих позиций
	ArrayResize( pre_OrdersArray, MathMax( now_OrdersTotal, 1 ) );
	for ( now_CurOrder = 0; now_CurOrder < now_OrdersTotal; now_CurOrder ++ )
	{
		pre_OrdersArray[now_CurOrder][0] = now_OrdersArray[now_CurOrder][0];
		pre_OrdersArray[now_CurOrder][1] = now_OrdersArray[now_CurOrder][1];
	}
	pre_OrdersTotal = now_OrdersTotal;
	
	if (first)
	  first = false;
		
	doLog(DEBUG, "Finished event check" );
}


// save and resolve external order ID based on order ticket
void saveOuterId(string orderId, int ticket) {
   int result = setGlobal(DoubleToStr(ticket,0), orderId); 
   doLog(DEBUG, "setGlobal(...) returned: " + result + " for ticket: " + ticket);  
   
   if (result!=1) {
      doLog(ERROR, "setGlobal(...) returned with invalid code!. Data loss may happen");
   }
}

string resolveOuterId(int ticket) {
   string orderId = "                                                                          ";
   int result = getGlobal(DoubleToStr(ticket,0), orderId);
   doLog(DEBUG, "getGlobal(...) returned with code : \"" + result + "\" for ticket: \"" + ticket+ "\" and result: \"" + orderId + "\"");
   
   if (result!=1) {
      doLog(ERROR, "getGlobal(...) returned with invalid code!. Data loss may happen");
   }
   
   return (orderId);
}

// remove extrenal id
void removeOuterId(int ticket) {
   int result = removeGlobal(DoubleToStr(ticket,0));
   doLog(DEBUG, "removeGlobal(...) returned: " + result + " for ticket: " + ticket);
   
    if (result!=1) {
      doLog(ERROR, "removeGlobal(...) returned with invalid code!. Data loss may happen");
   }
}


