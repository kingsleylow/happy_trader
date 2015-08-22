//+------------------------------------------------------------------+
//|                                                      relogin.mq4 |
//|                        Copyright 2013, MetaQuotes Software Corp. |
//|                                        http://www.metaquotes.net |
//+------------------------------------------------------------------+

#property copyright "Copyright 2013, MetaQuotes Software Corp."
#property link      "http://www.metaquotes.net"
#property show_inputs
#include <WinUser32.mqh>


 
// constants

extern int max_connect_attempt_g=10;
extern int dump_period_g = 2;
extern string target_dir_g = ""; // dir must exist
//extern string accounts_list_g = "c:\\tmp\\export_mt4_settings\\accounts.txt"; // account list file
extern string remote_collector_pipe_g = "\\\\.\\pipe\\CENTRAL_COLLECTOR_PIPE";
extern string server_mt_pipe_g = "\\\\.\\pipe\\MT4_CLIENT_PIPE1";


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
   int _lopen  (string path, int of);
   int _lcreat (string path, int attrib);
   int _llseek (int handle, int offset, int origin);
   int _lread  (int handle, string buffer, int bytes);
   int _lwrite (int handle, string buffer, int bytes);
   int _lclose (int handle);
   int CreateDirectoryA(string path, int atrr[]);
   void GetLocalTime(int& TimeArray[]);
   
#import "htmtproxy_bck.dll"
   int getVersion();
   int init(string pipename, int loglevel);
   int deinit();
   int connectToPipeServer(string remotepipe, string terminal_path);
	int disconnectFromPipeServer();
	int pushDataToPipeServer(string data, string& response[]);
   int getPollingQueueNextItem( int tout_msec, string& user[], string& password[]);
	int getTerminalUid( string& terminal_uid[] );
	int getQueueSize(  );
	int isConnectedToPipeServer();

#import

 
#define VK_HOME 0x24
#define VK_DOWN 0x28
#define VK_ENTER 0x0D
#define RESCANSERVERS_COMMAND_CODE   37400
#define OPENLOGINWINDOW_COMMAND_CODE 35429

#define BM_CLICK                       0x00F5
//#define MA_ACTIVATE                    1



// константы для функции _lopen
#define OF_READ               0
#define OF_WRITE              1
#define OF_READWRITE          2
#define OF_SHARE_COMPAT       3
#define OF_SHARE_DENY_NONE    4
#define OF_SHARE_DENY_READ    5
#define OF_SHARE_DENY_WRITE   6
#define OF_SHARE_EXCLUSIVE    7


string getFullTimeStamp()
{
   int TimeArray[4];
   int nYear,nMonth,nDay,nHour,nMin,nSec,nMilliSec;
   
   GetLocalTime(TimeArray);
   Print(GetLastError());
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



// _lopen  : Откpывает указанный файл. Возвpащает: описатель файла.
// _lcreat : Создает указанный файл.   Возвpащает: описатель файла.
// _llseek : Устанавливает указатель в откpытом файле. Возвpащает: 
// новое смещение указателя.
// _lread  : Считывает из откpытого файла указанное число байт. 
// Возвpащает: число считанных байт; 0 - если конец файла.
// _lwrite : Записывает данные из буфеpа в указанный файл. Возвpащает: 
// число записанных байт.
// _lclose : Закpывает указанный файл. Возвpащает: 0.
// В случае неуспешного завеpшения все функции возвращают значение 
// HFILE_ERROR=-1.
 
// path   : Стpока, опpеделяющая путь и имя файла.
// of     : Способ открытия.
// attrib : 0 - чтение или запись; 1 - только чтение; 2 - невидимый или 
// 3 - системный.
// handle : Файловый описатель.
// offset : Число байт, на котоpое пеpемещается указатель.
// origin : Указывает начальную точку и напpавление пеpемещения: 0 - 
// впеpед от начала; 1 - с текущей позиции; 2 - назад от конца файла.
// buffer : Пpинимающий/записываемый буфеp.
// bytes  : Число считываемых байт.
 
// Способы открытия (параметр of):
// int OF_READ            =0; // Открыть файл только для чтения
// int OF_WRITE           =1; // Открыть файл только для записи
// int OF_READWRITE       =2; // Открыть файл в режиме запись/чтение
// int OF_SHARE_COMPAT    =3; // Открывает файл в режиме общего 
// совместного доступа. В этом режиме любой процесс может открыть данный 
// файл любое количество раз. При попытке открыть этот файл в любом другом
// режиме, функция возвращает HFILE_ERROR.
// int OF_SHARE_DENY_NONE =4; // Открывает файл в режиме общего доступа 
// без запрета на чтение/запись другим процессам. При попытке открытия 
// данного файла в режиме OF_SHARE_COMPAT, функция возвращает HFILE_ERROR.
// int OF_SHARE_DENY_READ =5; // Открывает файл в режиме общего доступа с 
// запретом на чтение другим процессам. При попытке открытия данного файла 
// с флагами OF_SHARE_COMPAT и/или OF_READ или OF_READWRITE, функция 
// возвращает HFILE_ERROR.
// int OF_SHARE_DENY_WRITE=6; // Тоже самое, только с запретом на запись.
// int OF_SHARE_EXCLUSIVE =7; // Запрет текущему и другим процессам на 
// доступ к этому файлу в режимах чтения/записи. Файл в этом режиме можно 
// открыть только один раз (текущим процессом). Все остальные попытки 
// открытия файла будут провалены.
//+------------------------------------------------------------------+
//|   прочитать файл и вернуть строку с содержимым                   |
//+------------------------------------------------------------------+
string ReadFile (string path) 
  {
    int handle=_lopen (path,OF_READ);
    int read_size = 50;           
    string char50="x                                                 ";

    if(handle<0) 
      {
        Print("Error _lopen ",path); 
        return ("");
      }
    int result=_llseek (handle,0,0);      
    if(result<0) 
      {
        Print("Error _llseek" ); 
        return ("");
      }
    string buffer="";
    int count=0;
    int last;
    
    result=_lread (handle,char50,read_size);
    while(result>0 && result == read_size) 
      {
        buffer=buffer + char50;
        count++;
        result=_lread (handle,char50,read_size);
        last = result;
     }
    
    //Print("Последний прочитанный блок имеет размер в байтах:", last);
    char50 = StringSubstr(char50,0,last);
    buffer = buffer + char50;    
    result=_lclose (handle);              
    if(result<0)  
      Print("Error _lclose ",path);
    return (buffer);
  }
 
//+------------------------------------------------------------------+
//|  записать содержимое буфера по указанному пути                   |
//+------------------------------------------------------------------+
void WriteFile (string path, string buffer) 
  {
    int count=StringLen (buffer); 
    int result;
    int handle=_lopen (path,OF_WRITE);
    if(handle<0) 
      {
        handle=_lcreat (path,0);
        if(handle<0) 
          {
            Print ("Ошибка создания файла ",path);
            if (!CreateFullPath(path))
               {
               Print("Не удалось создать папку:",path);
               return;
               }
            else handle=_lcreat (path,0);   
          }
        result=_lclose (handle);
        handle = -1;
     }
    if (handle < 0) handle=_lopen (path,OF_WRITE);               
    if(handle<0) 
      {
        Print("Ошибка открытия файла ",path); 
        return;
      }
    result=_llseek (handle,0,0);          
    if(result<0) 
      {
        Print("Ошибка установки указателя"); 
        return;
      }
    result=_lwrite (handle,buffer,count); 
    if(result<0)  
        Print("Ошибка записи в файл ",path," ",count," байт");
    result=_lclose (handle);              
    if(result<0)  
        Print("Ошибка закрытия файла ",path);
    return;        
  }
  
int WriteFileSimple (string path, string buffer) 
  {
    int count=StringLen (buffer); 
    int result;
    int handle=_lopen (path,OF_WRITE);
    if(handle<0) 
    {
        handle=_lcreat (path,0);
        if(handle<0) 
          {
            Print ("Error creating file ",path);
            return(-1);
          }
        result=_lclose (handle);
    }
    
    handle=_lopen (path,OF_WRITE);               
    if(handle<0) 
    {
        Print("Error opening file ",path); 
        return(-1);
    }
    result=_llseek (handle,0,0);          
    if(result<0) 
    {
        Print("Error placing pointer"); 
    }
    result=_lwrite (handle,buffer,count); 
    if(result<0) { 
        Print("Error writing to file ",path," ",count," bytes");
    }
    
    result=_lclose (handle);              
    if(result<0) {  
        Print("Error closing file ",path); 
        return(-1);
    }
    
    return(0);
  }
  
//+------------------------------------------------------------------+
//|  создает все необходимые папки и подпапки                        |
//+------------------------------------------------------------------+
bool CreateFullPath(string path)
{
   bool res = false;
   if (StringLen(path)==0) return(false);
   Print("Создаем путь=>",path);
//----
   string folders[];
   if (!ParsePath(folders, path)) return(false);
   Print("Всего вложенных папок:", ArraySize(folders));
   
   int empty[];
   int i = 0;
   while (CreateDirectoryA(folders[i],empty)==0) i++;
   Print("Создали папку:",folders[i]);
   i--;
   while (i>=0) 
      {
      CreateDirectoryA(folders[i],empty);
      Print("Создали папку:",folders[i]);
      i--;
      }
   if (i<0) res = true;   
//----
   return(res);
}
   
//+------------------------------------------------------------------+
//| разбить путь на массив подпапок                                  |
//+------------------------------------------------------------------+
bool ParsePath(string & folder[], string path)
   {
   bool res = false;
   int k = StringLen(path);
   if (k==0) return(res);
   k--;

   Print("Распарсим путь=>", path);
   int folderNumber = 0;
//----
   int i = 0;
   while ( k >= 0 )
      {
      int char = StringGetChar(path, k);
      //Print("char = ",char, "=>",CharToStr(char));
      if ( char == 92) //  обратный слеш "\"
         {
         if (StringGetChar(path, k-1)!= 92)
            {
            folderNumber++;
            ArrayResize(folder,folderNumber);
            folder[folderNumber-1] = StringSubstr(path,0,k);
            Print(folderNumber,":",folder[folderNumber-1]);
            }
         else break;         
         }
      k--;   
      }
   if (folderNumber>0) res = true;   
//----
   return(res);   
   }   

//+------------------------------------------------------------------+
//|                                |
//+------------------------------------------------------------------+

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
 
 
//+------------------------------------------------------------------+
//|                                |
//+------------------------------------------------------------------+
string typ2str(int typ)
{
   string r = "";
   if (typ == OP_BUY)  r = "BUY";
   if (typ == OP_SELL) r = "SELL";
   return (r);
 }

//+------------------------------------------------------------------+
//|                                |
//+------------------------------------------------------------------+

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


//+------------------------------------------------------------------+  
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
      Print("ERROR - size of parsed string not expected.", true);
      return (false);
   }
}


//+------------------------------------------------------------------+
//| find n-th substring based on separator 
//+------------------------------------------------------------------+
string StringSplitSimple(string input,string sep, int index)
{
   int count=0;
   int oldpos=0;
   int pos=StringFind(input,sep,0);
   while(pos>=0&&count<=index)
   {
      if(count==index)
      {
         if(pos==oldpos)
         {
            return("");
         }
         else
         {
            return(StringSubstr(input,oldpos,pos-oldpos));
         }
      }
   
      oldpos=pos+StringLen(sep);
      pos=StringFind(input,sep,oldpos);
      count++;
   }
   
   if(count==index)
   {
      return(StringSubstr(input,oldpos));
   }
   
   return("");
}
   
   

//+------------------------------------------------------------------+
//|  PREPARE DATA FUNCTIONS                              |
//+------------------------------------------------------------------+

string api_json_string_key_value( string key, string value ) {

   return (StringConcatenate( "\"", key, "\"" , " : ", "\"", value , "\"" ));

}

string api_json_double_key_value( string key, double value )
{
   
   return (StringConcatenate( "\"", key, "\"" , " : ", DoubleToStr( value, 5) ));

}

string api_json_integer_key_value( string key, int value ) {
   
   return (StringConcatenate( "\"", key, "\"" , " : ", value) );

} 

string api_json_bool_key_value( string key, bool value ) {
   
   string value_str;
   if (value) value_str = "true"; 
      else 
   value_str = "false";
   return (StringConcatenate( "\"", key, "\"" , " : ", value_str) );

} 

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

string prepareDataToExport(int requestDepthDays, string mt_uid)
{   
   
    
    int pending_size = getQueueSize(  );
    
  
    
    Print("Pending users: ", pending_size);
  
    
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






//+------------------------------------------------------------------+
//|                                |
//+------------------------------------------------------------------+

void makeExport(  string mt_uid)
{
   string accPrefix = AccountNumber();
   int request_depth = -1;
   /*
   if (getGlobalParameters(request_depth) == MTST_GENERAL_ERROR) {
   
      Print("System function (getGlobalParameters) failed");
      return;
   }
   */

   // create directory
   //string dir_path = StringConcatenate(target_dir_g, "\\", postfix);
   
   //int empty[];
   //CreateDirectoryA(dir_path,empty);
   
   
   string content_tobe_sent = prepareDataToExport( request_depth, mt_uid );
    
   if (StringLen(target_dir_g) > 0) {
      // save to file if path is expected to be valid
      string file_path = StringConcatenate(target_dir_g, "\\" , getFullTimeStamp(), "_", accPrefix,"_hst.csv");
   
      int handle,cnt,i;
   
   
     
   
      if (WriteFileSimple(file_path, content_tobe_sent) != 0) {
         Print("Error writing file: ", file_path );
      }
   }
   
   // attempt to push to pipe
   string result[1];
   if (pushDataToPipeServer(content_tobe_sent, result) != MTST_NO_ERROR) {
      
      Print("Error pushing data to pipe server");
   }
   
   if (result[0] != RETURN_MT_DATA_ACCEPTED) {
   
      Print("Pipe server returned error: ", result[0]);
   }
   
   Print("Exported:", accPrefix, " OK");
  
}

//+------------------------------------------------------------------+
//|                                |
//+------------------------------------------------------------------+



//+------------------------------------------------------------------+
//|                                |
//+------------------------------------------------------------------+

int connectToUserPass(string login, string password)
{
   // get root
   int hwnd_parent=Parent();
   if (hwnd_parent != 0) {
      //Print("Parent found: ", hwnd_parent);
    } else {
      Print("Parent not found");
      return(-1);
   }
   
   PostMessageA(hwnd_parent,WM_COMMAND,OPENLOGINWINDOW_COMMAND_CODE,0);
   Sleep(1*1000);
       
   int dialog_login_hwnd=GetLastActivePopup(hwnd_parent);  
   if (dialog_login_hwnd != 0) {
      //Print("Login Dialog found: ", dialog_login_hwnd);
    } else {
      Print("Login Dialog not found");
      return(-1);
   } 
   
   int hlogin_edit=GetDlgItem(GetDlgItem(dialog_login_hwnd,0x49D),0x3E9);
   if (hlogin_edit != 0) {
      //Print("Login edit found: ", hlogin_edit);
    } else {
      Print("Login edit not found");
      return(-1);
   } 
   
   SendMessageA(hlogin_edit,WM_CLEAR,0,0);
   SetWindowTextA(hlogin_edit,login);
   Sleep(1*1000);
   
   
   int hlogin_password=GetDlgItem(dialog_login_hwnd,0x4C4);
   if (hlogin_password != 0) {
      //Print("Password edit found: ", hlogin_password);
    } else {
      Print("Password edit not found");
      return(-1);
   } 
   
   SendMessageA(hlogin_password,WM_CLEAR,0,0);
   SetWindowTextA(hlogin_password,password);
   Sleep(1*1000);
   
   // press login
   int hbutton_login=GetDlgItem(dialog_login_hwnd,0x1);
   if (hbutton_login != 0) {
      //Print("Login button found: ", hbutton_login);
    } else {
      Print("Login button not found");
      return(-1);
   } 
   
   SendMessageA(hbutton_login,BM_CLICK,0,0);
   Sleep(1*1000);
   
   Print("Attempt to login happened: ", login);
      
   return (0);
}





//+------------------------------------------------------------------+
//|                                |
//+------------------------------------------------------------------+

int process(string login, string password,  string mt_uid)
{
      // initial connection
      if (connectToUserPass(login, password) == 0) {
      
         
         // reconnect
         int mattempts = max_connect_attempt_g;
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
            makeExport(  mt_uid );
         }
      
         
         
      } else {
         Print("Error attempting to work with login: ", login);
         return (-1);
      }
      
      return (0);
}


// ----------------------------
// MAIN ENTRY FUNCTION
// ----------------------------
int start()
{
  
  Print("Starting initialization:",getFullTimeStamp(), " version: ", getVersion());
  
  if(init(server_mt_pipe_g, MSG_DEBUG_DETAIL) != MTST_NO_ERROR) {
      Print("Cannot initialize");
      return(0);
  }
  
  string terminal_uid[1];
  getTerminalUid( terminal_uid );
  Print("Terminal UID: ", terminal_uid[0]);
  
  
  if (connectToPipeServer(remote_collector_pipe_g, TerminalPath()) !=MTST_NO_ERROR) {
      Print("Cannot connect to remote pipe: ", remote_collector_pipe_g);
      deinit();
      return(0);
  }
  
  
  
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
      
      //int MTST_POLLING_NODATA -2
      //int MTST_POLLING_OK -3
      //int MTST_POLLING_OK_LAST -4
      // MTST_GENERAL_ERROR

      string user[1], password[1];
      bool result = getPollingQueueNextItem( 2*1000, user, password);
      
      // MTST_GENERAL_ERROR or MST_BOOL_TRUE or MST_BOOL_FALSE
      if (result == MTST_GENERAL_ERROR) {
         Print("System function (getPollingQueueNextItem) returned error, exiting...");
         deinit();
         return(0);
      }
      // wait for data availabe for 10 seconds
      else if (result == MTST_POLLING_NODATA) {
       
         Print("No valid data for processing, waiting...");
         continue;
      }
      else if (result == MTST_POLLING_OK || result == MTST_POLLING_OK_LAST) {
         Print("(getPollingQueueNextItem) result is OK");
      }
      else {
         Print("(getPollingQueueNextItem) unexpected result, exiting...");
         deinit();
         return(0);
      }
   
     
     
      string user_i = user[0];
      string password_i = password[0];   
         
      
      Print("******************Processing user/password: ", user_i, "/" , "****");
      process(user_i, password_i, terminal_uid[0] );
      Print("******************Finished user: ", user);
      
      // sleep 1 second
      //Sleep(1*1000);
        
     
      // finish read
      
     
      if (result == MTST_POLLING_OK_LAST) {
      
         
         Print("******************Finished loop waiting for: ", dump_period_g, " seconds..." );
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



