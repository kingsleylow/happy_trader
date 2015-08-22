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

 


// ��������� ��� ������� _lopen
#define OF_READ               0
#define OF_WRITE              1
#define OF_READWRITE          2
#define OF_SHARE_COMPAT       3
#define OF_SHARE_DENY_NONE    4
#define OF_SHARE_DENY_READ    5
#define OF_SHARE_DENY_WRITE   6
#define OF_SHARE_EXCLUSIVE    7



// _lopen  : ���p����� ��������� ����. ����p�����: ��������� �����.
// _lcreat : ������� ��������� ����.   ����p�����: ��������� �����.
// _llseek : ������������� ��������� � ���p���� �����. ����p�����: 
// ����� �������� ���������.
// _lread  : ��������� �� ���p����� ����� ��������� ����� ����. 
// ����p�����: ����� ��������� ����; 0 - ���� ����� �����.
// _lwrite : ���������� ������ �� ����p� � ��������� ����. ����p�����: 
// ����� ���������� ����.
// _lclose : ���p����� ��������� ����. ����p�����: 0.
// � ������ ����������� ����p����� ��� ������� ���������� �������� 
// HFILE_ERROR=-1.
 
// path   : ��p���, ��p��������� ���� � ��� �����.
// of     : ������ ��������.
// attrib : 0 - ������ ��� ������; 1 - ������ ������; 2 - ��������� ��� 
// 3 - ���������.
// handle : �������� ���������.
// offset : ����� ����, �� ����p�� ��p��������� ���������.
// origin : ��������� ��������� ����� � ���p������� ��p��������: 0 - 
// ���p�� �� ������; 1 - � ������� �������; 2 - ����� �� ����� �����.
// buffer : �p���������/������������ ����p.
// bytes  : ����� ����������� ����.
 
// ������� �������� (�������� of):
// int OF_READ            =0; // ������� ���� ������ ��� ������
// int OF_WRITE           =1; // ������� ���� ������ ��� ������
// int OF_READWRITE       =2; // ������� ���� � ������ ������/������
// int OF_SHARE_COMPAT    =3; // ��������� ���� � ������ ������ 
// ����������� �������. � ���� ������ ����� ������� ����� ������� ������ 
// ���� ����� ���������� ���. ��� ������� ������� ���� ���� � ����� ������
// ������, ������� ���������� HFILE_ERROR.
// int OF_SHARE_DENY_NONE =4; // ��������� ���� � ������ ������ ������� 
// ��� ������� �� ������/������ ������ ���������. ��� ������� �������� 
// ������� ����� � ������ OF_SHARE_COMPAT, ������� ���������� HFILE_ERROR.
// int OF_SHARE_DENY_READ =5; // ��������� ���� � ������ ������ ������� � 
// �������� �� ������ ������ ���������. ��� ������� �������� ������� ����� 
// � ������� OF_SHARE_COMPAT �/��� OF_READ ��� OF_READWRITE, ������� 
// ���������� HFILE_ERROR.
// int OF_SHARE_DENY_WRITE=6; // ���� �����, ������ � �������� �� ������.
// int OF_SHARE_EXCLUSIVE =7; // ������ �������� � ������ ��������� �� 
// ������ � ����� ����� � ������� ������/������. ���� � ���� ������ ����� 
// ������� ������ ���� ��� (������� ���������). ��� ��������� ������� 
// �������� ����� ����� ���������.
//+------------------------------------------------------------------+
//|   ��������� ���� � ������� ������ � ����������                   |
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
    
    //Print("��������� ����������� ���� ����� ������ � ������:", last);
    char50 = StringSubstr(char50,0,last);
    buffer = buffer + char50;    
    result=_lclose (handle);              
    if(result<0)  
      Print("Error _lclose ",path);
    return (buffer);
  }
 
//+------------------------------------------------------------------+
//|  �������� ���������� ������ �� ���������� ����                   |
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
            Print ("������ �������� ����� ",path);
            if (!CreateFullPath(path))
               {
               Print("�� ������� ������� �����:",path);
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
        Print("������ �������� ����� ",path); 
        return;
      }
    result=_llseek (handle,0,0);          
    if(result<0) 
      {
        Print("������ ��������� ���������"); 
        return;
      }
    result=_lwrite (handle,buffer,count); 
    if(result<0)  
        Print("������ ������ � ���� ",path," ",count," ����");
    result=_lclose (handle);              
    if(result<0)  
        Print("������ �������� ����� ",path);
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
//|  ������� ��� ����������� ����� � ��������                        |
//+------------------------------------------------------------------+
bool CreateFullPath(string path)
{
   bool res = false;
   if (StringLen(path)==0) return(false);
   Print("������� ����=>",path);
//----
   string folders[];
   if (!ParsePath(folders, path)) return(false);
   Print("����� ��������� �����:", ArraySize(folders));
   
   int empty[];
   int i = 0;
   while (CreateDirectoryA(folders[i],empty)==0) i++;
   Print("������� �����:",folders[i]);
   i--;
   while (i>=0) 
      {
      CreateDirectoryA(folders[i],empty);
      Print("������� �����:",folders[i]);
      i--;
      }
   if (i<0) res = true;   
//----
   return(res);
}
   
//+------------------------------------------------------------------+
//| ������� ���� �� ������ ��������                                  |
//+------------------------------------------------------------------+
bool ParsePath(string & folder[], string path)
   {
   bool res = false;
   int k = StringLen(path);
   if (k==0) return(res);
   k--;

   Print("��������� ����=>", path);
   int folderNumber = 0;
//----
   int i = 0;
   while ( k >= 0 )
      {
      int char = StringGetChar(path, k);
      //Print("char = ",char, "=>",CharToStr(char));
      if ( char == 92) //  �������� ���� "\"
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



