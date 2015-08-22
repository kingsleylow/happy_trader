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

extern int dump_period_g = 1;
extern string remote_collector_pipe_g = "\\\\.\\pipe\\CENTRAL_COLLECTOR_PIPE";
extern string server_mt_pipe_g = "\\\\.\\pipe\\MT4_CLIENT_PIPE";




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
   
#import "htmtproxy_bck.dll"
   int sayHallo(string hallostring);
   int init(string pipename);
   int deinit();
   int connectToPipeServer(string remotepipe);
	int disconnectFromPipeServer();
	int pushDataToPipeServer(string data);

#import

 


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




//+------------------------------------------------------------------+
//|                                |
//+------------------------------------------------------------------+


// ----------------------------


int start()
{
  sayHallo("Hallo from MT4"); 
  init(server_mt_pipe_g);
  if (connectToPipeServer(remote_collector_pipe_g) != 0) {
      Print("Cannot connect to remote pipe: ", remote_collector_pipe_g);
      return(0);
  }
  
  
  Print("Starting");
   
 
  // infinite loop 
  while(!IsStopped()) {
        
      string data = "";
      data = StringConcatenate (TerminalPath( ), ";", AccountNumber( ), ";", AccountServer( ),";", TerminalName( ), ";", ";",TerminalCompany() );
      
      Print("Sendings: ", data);  
      pushDataToPipeServer(data);
      
     
      Print("******************Finished loop waiting for: ", dump_period_g, " seconds..." );
      Sleep(dump_period_g *1000 );
      // upper loop 
   }
    
   
//----
   
   Print("Finishing");
   disconnectFromPipeServer();
   deinit();
   
   return(0);
}



