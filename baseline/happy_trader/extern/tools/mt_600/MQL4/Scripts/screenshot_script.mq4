//+------------------------------------------------------------------+
//|                        Copyright 2013,          Victor Zoubok    |
//+------------------------------------------------------------------+
#property copyright "Copyright 2013, MetaQuotes Software Corp."
#property link      "http://www.metaquotes.net"



#property script_show_inputs
//#property show_inputs
input  int sc_x_dim_p = 1000;
input  int sc_y_dim_p = 764;
input  string url_p = "http://blog.goit.co/addpost.aspx";
input  string username_p = "username=harris";
input  string password_p = "password=pass";
input  string title_p = "Title=title...";
input  string comment_p = "Comments=comment...";
input  string file_p = "fileUpload1";
input  string success_http_response_p = "Done";


// -----------------------
// globals for onBeforeUpload_1(...)
int Slippage_g   = 3;
extern int Stop_Loss_g  = 25;
extern int Take_Profit_g = 25;

string Stop_g = "";
string Target_g = "";
//input string Description = "";

// -----------------------





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


// buffer size to read strings
#define BUFFERSIZE  256



/** 
* Imported functions
* 
*/
#import "htmtqueue_bck.dll"
   

   int setLoglevel(int loglevel);
	void getUid( string& terminal_uid );
	int pushFileToBlog(
	  string content, 
	  int contentLength,
	  string url, 
	  
	  string user_param_name,
	  string user, 
	  
	  string password_param_name,
	  string password,
	  
	  string title_param_name,
	  string title,
	  
	  string comment_param_name,
	  string comment,
	  
	  string file_param_name,
	  string filename,
	  
	  string& statusMsg,
	  string& httpMsg
   );

		
#import

//+------------------------------------------------------------------+
//|  одирование данных в base64                                      |
//+------------------------------------------------------------------+

static int ExtBase64Encode[64]={ 'A','B','C','D','E','F','G','H','I','J','K','L','M',
                                 'N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
                                 'a','b','c','d','e','f','g','h','i','j','k','l','m',
                                 'n','o','p','q','r','s','t','u','v','w','x','y','z',
                                 '0','1','2','3','4','5','6','7','8','9','+','/'      };

                               
//+------------------------------------------------------------------+
//|  одирование данных в base64                                      |
//+------------------------------------------------------------------+

void Base64EncodeFromArray(int& in[], int len, string &out)
  {
   int i=0,pad=0;
//---- пройдемс€ и закодируем
   while(i<len)
     {
      //---- извлекаем байты
      int b3,b2,b1=in[i];
      i++;
      if(i>=len) { b2=0; b3=0; pad=2; }
      else
        {
         b2=in[i];
         i++;
         if(i>=len) { b3=0; pad=1; }
         else       { b3=in[i]; i++; }
        }
      //----
      int c1=(b1 >> 2);
      int c2=(((b1 & 0x3) << 4) | (b2 >> 4));
      int c3=(((b2 & 0xf) << 2) | (b3 >> 6));
      int c4=(b3 & 0x3f);  

      out=out+CharToString(ExtBase64Encode[c1]);
      out=out+CharToString(ExtBase64Encode[c2]);
      switch(pad)
        {  
         case 0:
           out=out+CharToString(ExtBase64Encode[c3]);
           out=out+CharToString(ExtBase64Encode[c4]);
           break;
         case 1:
           out=out+CharToString(ExtBase64Encode[c3]);
           out=out+"=";
           break;
         case 2:
           out=out+"==";
           break;
        }
     }
//----
  }
  


string getUidWrapper()
{
	string uid;
	uid = "00000000000000000000000000000000";
	//uid = "12345678910111213141516171819202";
   getUid( uid );

	return (uid);
}

void splitParameter(string paramStr, string& first, string& second)
{
   first = "";
   second = "";
   
   if (StringLen(paramStr) <=0)
      return;
      
   int idx = StringFind(paramStr, "=");
   if (idx == -1)
      return;
      
   first = StringSubstr(paramStr, 0, idx );
   second = StringSubstr(paramStr, idx+1 );
   
   Print("Splited: ", "[", first, "]", "[",  second, "]");
   
}


// I do not keep responsibility on this :)
void onBeforeUpload_1()
{
   
   ObjectDelete(0,"SELL_lIMIT");
   ObjectDelete(0,"TP_lIMIT");
   ObjectDelete(0,"SL_lIMIT");
  
   double Price = ChartPriceOnDropped();
   //bool   result;
   int    cmd,total,error,slippage;
   
//----
   int NrOfDigits = Digits();                           // Nr. of decimals used by Symbol
   int PipAdjust;                                       // Pips multiplier for value adjustment
      if(NrOfDigits == 5 || NrOfDigits == 3)            // If decimals = 5 or 3
         PipAdjust = 10;                                // Multiply pips by 10
         else
      if(NrOfDigits == 4 || NrOfDigits == 2)            // If digits = 4 or 3 (normal)
         PipAdjust = 1;            
//----   
   
   slippage = Slippage_g * PipAdjust; 
   //  slippage = Slippage * PipAdjust; 
   
   double stop_loss = Price + Stop_Loss_g * Point() * PipAdjust;
   double take_profit = Price - Take_Profit_g * Point() * PipAdjust; 
   
   
   MqlTick curtick;
   if (!SymbolInfoTick(Symbol(), curtick))  {
      return;
   }
   
   int jpy = StringFind(Symbol(),"JPY",2);
   
   if(jpy > -1){
      jpy = 3;
   }
   else {
      jpy = 5;
   }
   
   //Alert(jpy);
   
   if(curtick.bid > Price)  {
 
   
    ObjectCreate(0, "SELL_lIMIT",OBJ_HLINE,0,0,Price,Red);
    ObjectSetInteger(0, "SELL_lIMIT", OBJPROP_STYLE, STYLE_DASHDOT);
    ObjectSetInteger(0, "SELL_lIMIT", OBJPROP_COLOR, LimeGreen);
    
    ObjectCreate(0, "TP_lIMIT",OBJ_HLINE,0,0,take_profit,Red);
    ObjectSetInteger(0, "TP_lIMIT", OBJPROP_STYLE, STYLE_DASHDOT);
    ObjectSetInteger(0, "TP_lIMIT", OBJPROP_COLOR, Crimson);
    
    ObjectCreate(0, "SL_lIMIT",OBJ_HLINE,0,0,stop_loss,Red);
    ObjectSetInteger(0, "SL_lIMIT", OBJPROP_STYLE, STYLE_DASHDOT);
    ObjectSetInteger(0, "SL_lIMIT", OBJPROP_COLOR, Crimson);
    

    Stop_g = DoubleToString(NormalizeDouble(stop_loss,jpy),jpy);
    Target_g = DoubleToString(NormalizeDouble(take_profit,jpy),jpy);
    
    
    
   }
   else if(curtick.bid < Price)  {
   
   
    ObjectCreate(0, "SELL_lIMIT",OBJ_HLINE,0,0,Price,Red);
    ObjectSetInteger(0, "SELL_lIMIT", OBJPROP_STYLE, STYLE_DASHDOT);
    ObjectSetInteger(0, "SELL_lIMIT", OBJPROP_COLOR, LimeGreen);
    
    ObjectCreate(0, "TP_lIMIT",OBJ_HLINE,0,0,take_profit,Red);
    ObjectSetInteger(0, "TP_lIMIT", OBJPROP_STYLE, STYLE_DASHDOT);
    ObjectSetInteger(0, "TP_lIMIT", OBJPROP_COLOR, Crimson);
    
    ObjectCreate(0, "SL_lIMIT",OBJ_HLINE,0,0,stop_loss,Red);
    ObjectSetInteger(0, "SL_lIMIT", OBJPROP_STYLE, STYLE_DASHDOT);
    ObjectSetInteger(0, "SL_lIMIT", OBJPROP_COLOR, Crimson);
    

    Stop_g = DoubleToString(NormalizeDouble(stop_loss,jpy),jpy);
    Target_g = DoubleToString(NormalizeDouble(take_profit,jpy),jpy);
  
   }
   
   Print("Finished onBeforeUpload_1(...) -> Stop=",Stop_g, ", Target=", Target_g);
  
}

void OnStart()
{
		
	 setLoglevel(MSG_DEBUG_DETAIL);
	 
	 if (TerminalInfoInteger(TERMINAL_X64)) {
	   Print("Terminal is 64 bits");
	 }
	 else {
	   Print("Terminal is 32 bits");
	 }
	 
	 Print("Making pre-upload work");
	 onBeforeUpload_1();
	 Print("Pre-upload work finished");
	
	 Print("Making screenshot...");
	 string filename = getUidWrapper() + ".gif";
	 Print("filename: ", filename);

	 if (!ChartScreenShot(0, filename, sc_x_dim_p, sc_y_dim_p, ALIGN_RIGHT)) {
		Print("ChartScreenShot(...) failed: ", GetLastError());
		return;
	 }
	 
	 int fhndl = FileOpen(filename, FILE_BIN | FILE_READ );
	 
	 if(fhndl <1 )
    {
      Print("GIF cannot be opened: ", GetLastError());
     	return;
    }
    
    Print("Opened GIF file: ", filename);
    
    int fs = FileSize(fhndl);
    if (fs <=0) {
      Print("GIF is empty!");
      FileClose(fhndl); 
     	return;
    }
    
    Print("Reading file content of size: ", fs);
    
    int output_buff[];
    ArrayResize(output_buff, fs);
      
    int cnt = 0;
    while(!FileIsEnding(fhndl)) {
      int char_s = FileReadInteger( fhndl, CHAR_VALUE);
      output_buff[cnt] = char_s;
      
      cnt = cnt+1;
      if (cnt >=fs)
         break;
      
    }
    
    FileClose(fhndl); 
    Print("Closed GIF file");
     
    
    string encoded_content;
    Base64EncodeFromArray(output_buff,fs, encoded_content );
    
        
    /*
    if (fs != StringLen(content)) {
      Print("Invalid GIF read, string length is: ",StringLen(content) );
      FileClose(fhndl); 
     	return(-1);
    }
    */
    
    
    int encoded_content_len = StringLen(encoded_content);
    Print("Encoded length: ", encoded_content_len);
     
    //string statusMsg[1];
    //statusMsg[0] = "r                                                                                                                                          ";
    
     string user_param_name;
	  string user; 
	  
	  splitParameter(username_p, user_param_name, user);
	  
	  string password_param_name;
	  string password;
	  
	  splitParameter(password_p, password_param_name, password);
	  
	  string title_param_name;
	  string title;
	  
	  splitParameter(title_p, title_param_name, title);
	  
	  string comment_param_name;
	  string comment;
	  
	  splitParameter(comment_p, comment_param_name, comment);
	  
	 
    
    string statusMsg =  "r                                                                                                                                          ";
    string httpMsg =    "d                                                                                                                                          ";
    int rpost= pushFileToBlog(
	    encoded_content, 
	    encoded_content_len,
	    url_p, 
	    
	    user_param_name, 
	    user,
	    
	    password_param_name,
	    password,
	    
	    title_param_name,
	    title,
	    
	    comment_param_name,
	    comment,
	    
	    file_p, 
	    filename,
	    
	    statusMsg,
	    httpMsg
     );
     
     
      
     // different from MQL5 
     statusMsg = StringTrimLeft(StringTrimRight(statusMsg));
     httpMsg = StringTrimLeft(StringTrimRight(httpMsg));
    
     
       
     
     string dlgmsg = "";
     if (rpost != DLL_FUN_CALL_TRUE) {
         dlgmsg = "Exception on posting data: ["+ statusMsg+ "]";
         Print(dlgmsg);
         MessageBox(dlgmsg, "ERROR", 0);
     }
     else {
     
         // parse message here
         if (StringCompare(httpMsg, success_http_response_p, false)==0) {
            dlgmsg = "Successfully posted data: ["+ statusMsg+ "], HTTP layer returned: ["+ httpMsg+ "]";
         } else {
            dlgmsg = "HTTP server returned invalid response: ["+ statusMsg+ "], HTTP layer returned: ["+ httpMsg+ "]";
         }
         
         Print(dlgmsg);
        
         MessageBox(dlgmsg, "OK", 0);
     }
     
   
   
     
    
    // pass this content further
    
   


}
//+---------------------------------------------------------------