//+------------------------------------------------------------------+
//|                                                      relogin.mq4 |
//|                        Copyright 2013, MetaQuotes Software Corp. |
//|                                        http://www.metaquotes.net |
//+------------------------------------------------------------------+
#property copyright "Copyright 2013, MetaQuotes Software Corp."
#property link      "http://www.metaquotes.net"

//+------------------------------------------------------------------+
//| script program start function                                    |
//+------------------------------------------------------------------+

//+------------------------------------------------------------------+
//|                                                 export_state.mq4 |
//|                        Copyright 2013, MetaQuotes Software Corp. |
//|                                        http://www.metaquotes.net |
//+------------------------------------------------------------------+
#property copyright "Copyright 2013, MetaQuotes Software Corp."
#property link      "http://www.metaquotes.net"
#property show_inputs
#include <WinUser32.mqh>
 
// constants

extern int max_connect_attempt_g=10;
extern int dump_period_g = 20;
extern string target_dir_g = "c:\\tmp\\export_mt4"; // dir must exist
extern string accounts_list_g = "c:\\tmp\\export_mt4_settings\\accounts.txt"; // account list file

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
#import

 
#define VK_HOME 0x24
#define VK_DOWN 0x28
#define VK_ENTER 0x0D
#define RESCANSERVERS_COMMAND_CODE   37400
#define OPENLOGINWINDOW_COMMAND_CODE 35429

#define BM_CLICK                       0x00F5
#define MA_ACTIVATE                    1
#define HTCLIENT                       1


// константы для функции _lopen
#define OF_READ               0
#define OF_WRITE              1
#define OF_READWRITE          2
#define OF_SHARE_COMPAT       3
#define OF_SHARE_DENY_NONE    4
#define OF_SHARE_DENY_READ    5
#define OF_SHARE_DENY_WRITE   6
#define OF_SHARE_EXCLUSIVE    7



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
   if (typ == OP_BUY)  r = "buy";
   if (typ == OP_SELL) r = "sell";
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
//|                                |
//+------------------------------------------------------------------+
string SaveOrder(string title)
{
     string result = "";
     int typ;
     typ = OrderType();
     if (typ == OP_BUY || typ == OP_SELL)
     {

         result = StringConcatenate(
                  title,",",
                  typ2str(OrderType()),",",
                  TimeToStr(OrderOpenTime()),",",
                  OrderSymbol(),",",
                  OrderMagicNumber(),",",
                  p2str(OrderLots(),3),",",
                  p2str(OrderOpenPrice(),5),",",
                  p2str(OrderClosePrice(),5),",",
                  p2str(OrderProfit(),3),",",
                  OrderComment(), "\r\n"
               );
     }
     
     
     return(result);
 
}   

//+------------------------------------------------------------------+
//|                                |
//+------------------------------------------------------------------+

void makeExport(string accPrefix, string postfix)
{

   // create directory
   string dir_path = StringConcatenate(target_dir_g, "\\", postfix);
   
   int empty[];
   CreateDirectoryA(dir_path,empty);
   
   
   
   string file_path = StringConcatenate(dir_path, "\\" , accPrefix,"_history.csv");
   
   int handle,cnt,i;
   
   
   string file_contet = "";
   string dummy = "";
   if (IsConnected( ) && !IsStopped()) {
      
      file_contet = StringConcatenate("Opened/Closed",",", "Type",",","Time and Date",",","Symbol",",","Magic Number",",","Lots",",","Open",",","Close",",","Profit",",","Comment","\r\n");
      cnt = OrdersHistoryTotal();
      for (i=0;i<cnt;i++)
      {
          
          if (OrderSelect(i,SELECT_BY_POS, MODE_HISTORY) == true)
          {
             file_contet = StringConcatenate( file_contet, SaveOrder("closed"));
          }
      }
      cnt = OrdersTotal();
      for (i=0;i<cnt;i++)
      {
          
          if (OrderSelect(i,SELECT_BY_POS, MODE_TRADES) == true)
          {
            
            file_contet = StringConcatenate( file_contet, SaveOrder("open"));
            
          }
      }
   } else {
      file_contet = "not connected";
   }
   
  
   if (WriteFileSimple(file_path, file_contet) != 0) {
      Print("Error writing file: ", file_path );
   }
   
   
   
  
}

//+------------------------------------------------------------------+
//|                                |
//+------------------------------------------------------------------+

void fullExport(string postfix)
{
   string accNumber = AccountNumber();
    
   makeExport(accNumber, postfix);
   Print("Exported:", accNumber);
       
   
  
}

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

int process(string login, string password, string postfix)
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
            fullExport( postfix );
         }
      
         
         
      } else {
         Print("Error attempting to work with login: ", login);
         return (-1);
      }
      
      return (0);
}


// ----------------------------


int start()
{
   
  // read accounts;
  string raw_accounts =  ReadFile(accounts_list_g);
  if (StringLen(raw_accounts) <=0) {
      Print("No valid accounts provided in the file: ", accounts_list_g);
      return(0);
  }
   
   
  string account_list[]; 
  
  SplitString(raw_accounts, "\r\n", account_list);
  int array_account_num = ArraySize(account_list);
  
  Print("Read number of accounts: ", array_account_num);
  if (array_account_num <=0) {
       Print("Empty list in the file: ", accounts_list_g);
       return(0);
  }
  
  
  
  
  // infinite loop 
  while(!IsStopped()) {
    
   string postfix = TimeCurrent(); 
   
   for(int i = 0; i < array_account_num; i++) {
         string account_line = StringTrimRight(StringTrimLeft(account_list[i]));
         
         if (StringLen(account_line) <=0 ) {
            Print("Account data at: ", i, " are invalid");
            continue;
         }
         
         
         
         string user = StringSplitSimple(account_line, ",", 0);
         string password = StringSplitSimple(account_line, ",", 1);
         
         if ((StringLen(user) <=0) || (StringLen(password) <=0)) {
             Print("User/pasword at: ", i, " invalid");
             continue;
            
         } 
      
         Print("******************Processing user/password: ", user, "/" , password);
         process(user, password, postfix);
         Print("******************Finished user: ", user);
      
         // sleep 1 second
         Sleep(1*1000);
        
      
      }
     
      Print("******************Finished loop waiting for: ", dump_period_g, " seconds..." );
      Sleep(dump_period_g *1000 );
      // upper loop 
    }
    
   
//----
   return(0);
  }



