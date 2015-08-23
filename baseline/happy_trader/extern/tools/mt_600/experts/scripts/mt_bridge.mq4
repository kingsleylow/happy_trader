//+------------------------------------------------------------------+
//|                                                    mt_bridge.mq4 |
//|                        Copyright 2013, MetaQuotes Software Corp. |
//|                                        http://www.metaquotes.net |
//+------------------------------------------------------------------+
#property copyright "Copyright 2013, MetaQuotes Software Corp."
#property link      "http://www.metaquotes.net"

#include <socket.mqh>

#property show_inputs
extern int port=2007;
extern string ip_address="";
//extern int slippage = 3;

#define MODULE "MT_BRIDGE: "
#define COMMAND_END_SYMBOL "="
#define COMMAND_DELIMITER_SYMBOL "-"
#define COMMAND_FINISH_SYMBOL ";"
// -------------------------------

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
  




// ----------------------------------


int parse_subcommands(string & data, string &response)
{
    response = "";
    
   if (StringLen(data) < 0) {
      Print(MODULE, "Empty command");
      response = "ERROR:Empty command";
      return(0);
   }
     
   string commands[];     
   bool r = SplitString(data, COMMAND_DELIMITER_SYMBOL, commands);
   if (!r) {
      Print(MODULE, "Command cannot be splitted into subcommands, trying to use as it is");
      return (processor(data, response));
   } 
   
   //extract token
   string command_token = "";
   if (!extractUniqueToken(commands, 0, command_token, response )) {
        return(0);
   }
   
   // 
   
   int cmd_count = ArraySize(commands);
   if (cmd_count <=1) {
      response = "ERROR:Command list is not of correct length";
      Print(MODULE, response);
      return(0);
   }
   
   // command begins from one
   for(int i = 1; i < cmd_count; i++) {
      // trimming
      string command_i = StringTrimRight(StringTrimLeft(commands[i]));
      if (StringLen(command_i) > 0) {
         string last_cmd_char = StringSubstr(command_i, StringLen(command_i)-1);
      
         if (last_cmd_char != COMMAND_FINISH_SYMBOL)
            command_i = command_i + COMMAND_FINISH_SYMBOL;
         
         Print("Trying to process the command: [" + command_i + "], num#", i);
         
         string command_response = "";
         int command_result = processor(command_i, command_response);
         
         if (command_result == -1) {
            // stop command
            return (-1);
         }
         
         response = response + command_response + COMMAND_DELIMITER_SYMBOL;
         
      }
   }
   
   response = response + command_token;
   
   return (0);
      
}

// ----------------------------------
string generateString(int blocks)
{
   string r ="";
   
   for(int  i =0; i < blocks; i++) {
      r = r + "0123456789";
   }
   return(r);
}

// ----------------------------------
// extract parameters
// retursn false if validation failed or can't extract parameter
bool extractSymbol(string& parameter_array[], int position, string& symbol, string& response )
{
   symbol = "";
   
   int parameters_size = ArraySize(parameter_array);
   if (position >= parameters_size) {
      response = "ERROR:Invalid position for extracting symbol [" + position + "]";
      Print(MODULE,response);
      return(false);
   }
   
   symbol = parameter_array[position];
   if (StringLen(symbol) <= 0) {
      response = "ERROR:Invalid symbol";
      Print(MODULE,response);
      return(false);
   }
   
   Print(MODULE,"Extracted symbol: ", symbol, " at position: ",position );
   return (true);
   
}

// ----------------------------------

bool extractLots(string& parameter_array[], int position, double& lots, string& response )
{
   lots = -1;
   
   int parameters_size = ArraySize(parameter_array);
   if (position >= parameters_size) {
      response = "ERROR:Invalid position for extracting volume [" + position + "]";
      Print(MODULE,response);
      return(false);
   }
   
   string lots_str  = parameter_array[position];
  
   if (StringLen(lots_str) <= 0) {
     response = "ERROR:Invalid volume";
     Print(MODULE,response);
     return(false);
   }
      
   lots = MathAbs(StrToDouble (lots_str));
   if (lots ==0) {
      response = "ERROR:Zero volume";
      Print(MODULE,response);
      return(false);
   }
   
   Print(MODULE,"Extracted volume: ", lots, " at position: ",position );
   return (true);
   
}
// ----------------------------------

bool extractSippage(string& parameter_array[], int position, int& slippage, string& response )
{
   slippage = -1;
   
   int parameters_size = ArraySize(parameter_array);
   if (position >= parameters_size) {
      response = "ERROR:Invalid position for extracting slippage [" + position + "]";
      Print(MODULE,response);
      return(false);
   }
   
   string slippage_str  = parameter_array[position];
  
   if (StringLen(slippage_str) <= 0) {
     response = "ERROR:Invalid slippage";
     Print(MODULE,response);
     return(false);
   }
      
   slippage = MathAbs(StrToInteger (slippage_str));
   Print(MODULE,"Extracted slippage: ", slippage, " at position: ",position );
   
   return (true);
   
}

// ----------------------------------

bool extractTicket(string& parameter_array[], int position, int& ticket, string& response )
{
   ticket = -1;
   
   int parameters_size = ArraySize(parameter_array);
   if (position >= parameters_size) {
      response = "ERROR:Invalid position for extracting order ticket [" + position + "]";
      Print(MODULE,response);
      return(false);
   }
   
   string ticket_str  = parameter_array[position];
  
   if (StringLen(ticket_str) <= 0) {
     response = "ERROR:Invalid slippage";
     Print(MODULE,response);
     return(false);
   }
      
   ticket = MathAbs(StrToInteger (ticket_str));
   if (ticket==0) {
      response = "ERROR:Invalid ticket";
      Print(MODULE,response);
      return(false);
   }
   
   Print(MODULE,"Extracted ticket: ", ticket, " at position: ",position );
   return (true);
   
}

// ----------------------------------------

bool extractUniqueToken(string& parameter_array[], int position, string& token, string& response )
{
   token = "";
   
   int parameters_size = ArraySize(parameter_array);
   if (position >= parameters_size) {
      response = "ERROR:Invalid position for extracting token [" + position + "]";
      Print(MODULE,response);
      return(false);
   }
   
   token = parameter_array[position];
   if (StringLen(token) <= 0) {
      response = "ERROR:Invalid token";
      Print(MODULE,response);
      return(false);
   }
   
   Print(MODULE,"Extracted token: ", token, " at position: ",position );
   return (true);
   
}

// ----------------------------------


int processor(string& data, string &response)
{
  
   response = "";
    
   if (StringLen(data) <= 0) {
       Print(MODULE, "Empty command data");
       response = "ERROR:Empty command data";
       return(0);
   }
   
  
   int ticket = -1;
   
   
   
   // parse value
   string result[];
   bool r = SplitString(data, COMMAND_FINISH_SYMBOL, result);
   if (!r) {
      Print(MODULE, "Unknown command: ", data);
      response = "ERROR:Unknown command";
      return(0);
   }
   
   if (ArraySize(result) < 1) {
      Print(MODULE,"Unknown command: ", data);
      response = "ERROR:Cannot parse data";
      return(0);
   }
   
   string command = StringTrimLeft(StringTrimRight(result[0]));
   if (StringLen(command) <= 0) {
       response = "ERROR:Empty command token";
       Print(MODULE, response);
       return(0);
   }
   
     
  
   if (command=="SELL") {
      Print(MODULE,"Opening SHORT");
      
      
      string symbol = "";
      if (!extractSymbol(result, 1, symbol, response )) {
         return(0);
      }
          
       
      double lots = -1;
      if (!extractLots(result,2, lots, response )) {
         return(0);
      }
      
      int slippage=-1;
      if (!extractSippage(result, 3, slippage, response )) {
         return(0);
      }
      
      
      RefreshRates();
      int digits = MarketInfo(symbol, MODE_DIGITS);
      double bid   =NormalizeDouble(MarketInfo(symbol,MODE_BID), digits);
      double ask   =NormalizeDouble(MarketInfo(symbol,MODE_ASK),digits);
      
      ticket = OrderSend(symbol, OP_SELL, lots, bid, slippage, 0, 0); 
      if (ticket < 0) {
         Print(MODULE,"OrderSend(...) failed");
         response = "ERROR:OrderSend failed ["+GetLastError()+"]";
         return(0);
      }
      
      response  = "OK;"+ticket;
   }
   else if (command=="BUY") {
      Print(MODULE,"Opening LONG");
      symbol = "";
      if (!extractSymbol(result, 1, symbol, response )) {
         return(0);
      }
      
      lots = -1;
      if (!extractLots(result, 2, lots, response )) {
         return(0);
      }
      
      slippage=-1;
      if (!extractSippage(result, 3, slippage, response )) {
         return(0);
      }
      
      RefreshRates();
      digits = MarketInfo(symbol, MODE_DIGITS);
      bid   =NormalizeDouble(MarketInfo(symbol,MODE_BID), digits);
      ask   =NormalizeDouble(MarketInfo(symbol,MODE_ASK),digits);
      
      ticket = OrderSend(symbol, OP_BUY, lots, ask, slippage, 0, 0); 
      if (ticket < 0) {
         Print(MODULE,"OrderSend(...) failed");
         response = "ERROR:OrderSend failed ["+GetLastError()+"]";
         return(0);
      }
      response  = "OK;"+ticket;
   }
   else if (command=="CLOSE") {
      Print(MODULE,"Closing order");
      //Sleep(20*1000);
      
     
      
      if (!extractTicket(result, 1, ticket, response )) {
         return(0);
      }
      
      slippage=-1;
      if (!extractSippage(result, 2, slippage, response )) {
         return(0);
      }
      
      if (!OrderSelect(ticket, SELECT_BY_TICKET , MODE_TRADES)) {
         Print(MODULE,"Invalid order by provided ticket");
         response = "ERROR:Invalid order by provided ticket";
         return(0);
      }
      
      
      symbol = OrderSymbol();
      RefreshRates();
      digits = MarketInfo(symbol, MODE_DIGITS);
      bid   =NormalizeDouble(MarketInfo(symbol,MODE_BID), digits);
      ask   =NormalizeDouble(MarketInfo(symbol,MODE_ASK),digits);
      
      lots = OrderLots();
      bool result_close = false;
      if (OrderType() == OP_BUY) {
         result_close = OrderClose(ticket, lots, bid, slippage );
      }
      else if (OrderType() == OP_SELL) {
         result_close = OrderClose(ticket, lots, ask, slippage );
      }
      
      if (!result_close) {
         Print(MODULE,"OrderClose(...) failed");
         response = "ERROR:OrderClose failed ["+GetLastError()+"]";
         return(0);
      }
      
      
      response  = "OK";
   }
   else if (command=="STOP") {
      Print(MODULE,"Stopping MT server");
      response  = "OK";
      return(-1);
   }
   else if (command=="GET_OPEN_ORDERS") {
      Print(MODULE,"Get open trades");
      response ="OK:";
      
      int total=OrdersTotal();  
      for(int pos=0; pos < total; pos++ ){
         if(OrderSelect(pos,SELECT_BY_POS, MODE_TRADES)==false) 
            continue;
            
         
         response = response + OrderSymbol() + ";" + OrderTicket() + ";";
      }
   }
   else if (command=="GET_CLOSE_ORDERS") {
      Print(MODULE,"Get close trades");
      response ="OK:";
      
      total=OrdersHistoryTotal();  
      for(pos=0; pos < total; pos++ ){
         if(OrderSelect(pos,SELECT_BY_POS, MODE_HISTORY)==false) 
            continue;
            
         
         response = response + OrderSymbol() + ";" + OrderTicket() + ";";
      }
      
      
   }
   else if (command=="TEST") {
      // returns arbitrary data
      response = "OK:"+generateString(200) + "!!!END!!!";
   }
   
  
  
   
   return(0);
   
}

// --------------------------------

void start_server_loop(string id,int socket, int& msgsock, bool& finish_app) {
    msgsock = -1;   
    msgsock = sock_accept(socket);
    int err_n = errno();
    finish_app = false;
    
    if (err_n!=0) {
       Print(MODULE,"socket error on accepting: " + err_n + ", exiting from the script");
       finish_app = true;
       return;
    }
    
    while(true) {
      // main loop
      string full_item = "";
      // read up to delimiter
      while((StringLen(full_item) <= 0) || (StringSubstr(full_item, StringLen(full_item)-1) != COMMAND_END_SYMBOL)) {
         err_n = errno();
         if (err_n!=0 || IsStopped()) {
            Print(MODULE,"socket error on reading data: " + err_n + ", exiting from the loop");
            return;
         }
         
         string item = sock_receive(msgsock);
         full_item  = full_item + item;
         
      }
      
      // remove last =
      full_item = StringSubstr(full_item, 0, StringLen(full_item)-1);  
      Print(MODULE,"Data of length received: " + StringLen(full_item));     
      Print(MODULE,"Data to be processed: '" + full_item + "'");     
      string response = "";
      
      //int process_res = processor(full_item, response);
      full_item = StringTrimLeft(StringTrimRight(full_item));
      int process_res = parse_subcommands(full_item, response);
      if (process_res < 0) {
          Print(MODULE,"Command to stop received, exiting from the script"); 
          finish_app = true;
          return; 
      }
          
      send_response_packet(msgsock, response);
     
    }  
}
      
   
void send_response_packet(int msgsock, string& data)
{
       Print(MODULE,"Sending response packet....");
      int response_length = StringLen(data);
      
      if (response_length >= 9999) {
         Print(MODULE,"Response is too big, cutting...");
         data  = StringSubstr(data, 0, 9999);
      }
      
      string response_length_str = StringLen(data);
      
      // a little bit hacky but need to transfer the length first
      if (StringLen(response_length_str) == 0) {
         response_length_str = "    ";
      }
      else if (StringLen(response_length_str) == 1) {
         response_length_str = response_length_str + "   ";
      }
      else if (StringLen(response_length_str) == 2) {
         response_length_str = response_length_str + "  ";
      }
      else if (StringLen(response_length_str) == 3) {
         response_length_str = response_length_str + " ";
      }
      else if (StringLen(response_length_str) == 4) {
         // do nothing
         //response_str = response_str + "";
      }
      
          
      Print(MODULE,"Sending response length back: " + response_length_str);
      sock_send(msgsock,response_length_str); 
      Print(MODULE,"Sending response back: " + data);
      sock_send(msgsock,data);
}
   
int  start() {

   Print(MODULE,"Matlab bridge started");
   
   int listen_socket = -1;
   while (!IsStopped()) {
      
      Print(MODULE,"Going to open server socket");
      listen_socket = open_socket(port,ip_address);
      if (listen_socket <=0) {
         Print(MODULE,"Server socket could not be opened");
         listen_socket = -1;
         break;
      }
      
      Print(MODULE,"Going to accept new connection");
      
      int msgsock = -1;
      bool finish_app = false;
      
      start_server_loop("main",listen_socket, msgsock, finish_app);
      if (finish_app) {
         Print(MODULE,"Trying to stop application...");
         if (msgsock > 0) {
            sock_close(msgsock);
            msgsock = -1;
         }
         break;
      }
      
            
      if (msgsock > 0) {
         sock_close(msgsock);
         msgsock = -1;
      }
      
      if (listen_socket > 0) {
         sock_close(listen_socket);
         listen_socket = -1;
      }
     
      
      Print(MODULE,"Going to accept loop again...");
     
   }
   
   Print(MODULE,"Deinitializing sockets before exit");
   
 
   if (listen_socket > 0) {
      sock_close(listen_socket);
   }
   
   // may call twise, not an issue
   sock_cleanup();
   
   Print(MODULE,"Finished");
   
   return(0);
}


