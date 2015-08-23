//+------------------------------------------------------------------+
//|                                                 cutom_reload.mq4 |
//|                        Copyright 2013, MetaQuotes Software Corp. |
//|                                        http://www.metaquotes.net |
//+------------------------------------------------------------------+
#property copyright "Copyright 2013, MetaQuotes Software Corp."
#property link      "http://www.metaquotes.net"

#property indicator_chart_window


bool WantKomma=true;       // if Excel wants "," instead of "." for cent

//+------------------------------------------------------------------+
//| script program start function                                    |

//+------------------------------------------------------------------+

#property show_inputs
#include <WinUser32.mqh>
 

extern int Pause = 10; // Пауза между переключениями торговых счетов
int credentials_number_g = 3;
string credentials_g[3][2] = {"477352","fwxt2qz",   "477354","tdi5fvp",   "477356","gz0ncpb" };
int max_connect_attempt_g=10;
extern int dump_period_g = 15;
 
#import "user32.dll"
  int GetParent( int hWnd );
  int GetDlgItem( int hDlg, int nIDDlgItem );
  int GetLastActivePopup( int hWnd );
  int GetAncestor (int hWnd, int gaFlags);

 

 
#define VK_HOME 0x24
#define VK_DOWN 0x28
#define VK_ENTER 0x0D
#define RESCANSERVERS_COMMAND_CODE   37400
#define OPENLOGINWINDOW_COMMAND_CODE 35429
#define PAUSE 1000
#define BM_CLICK                       0x00F5
#define MA_ACTIVATE                    1
#define HTCLIENT                       1

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
 
string typ2str(int typ)
{
   string r = "";
   if (typ == OP_BUY)  r = "buy";
   if (typ == OP_SELL) r = "sell";
   return (r);
 }

string p2str(double p, int digits)
{
   string s,r;
   s = DoubleToStr(p,digits);
   r = s;
   if (WantKomma)
   {
      p = StringFind(s,".",0);
   
      if (p != -1)
      {
         r = StringSubstr(s,0,p)+","+StringSubstr(s,p+1);
      }
   }
   return (r);
}
   
   
void SaveOrder(string title, int handle)
{
     int typ;
     typ = OrderType();
     if (typ == OP_BUY || typ == OP_SELL)
     {

         FileWrite(handle,
                  title,
                  typ2str(OrderType()),
                  TimeToStr(OrderOpenTime()),
                  OrderSymbol(),
                  OrderMagicNumber(),
                  p2str(OrderLots(),3),
                  p2str(OrderOpenPrice(),5),
                  p2str(OrderClosePrice(),5),
                  p2str(OrderProfit(),3),
                  OrderComment());
     }
 
}   

void makeExport(string accPrefix, string postfix)
{
int handle,cnt,i;
   cnt = OrdersHistoryTotal();
   
   handle = FileOpen(StringConcatenate(accPrefix,"_history_", postfix, ".csv"),FILE_WRITE|FILE_CSV);
   FileWrite(handle,"Opened/Closed","Type","Time and Date","Symbol","Magic Number","Lots","Open","Close","Profit","Comment");
   for (i=0;i<cnt;i++)
   {
       if (OrderSelect(i,SELECT_BY_POS, MODE_HISTORY) == true)
       {
         SaveOrder("closed",handle);
       }
   }
   cnt = OrdersTotal();
   for (i=0;i<cnt;i++)
   {
       if (OrderSelect(i,SELECT_BY_POS, MODE_TRADES) == true)
       {
         SaveOrder("open",handle);
       }
   }
   FileClose(handle);
   
}

void fullExport()
{
   string accNumber = AccountNumber();
   string postfix = TimeCurrent( );
   if (IsConnected( ) && !IsStopped()) {
      makeExport(accNumber, postfix);
      Print("Exported ok:", accNumber);
   }
   else {
      Print("No connection for account:", accNumber);
   }     
   
  
}

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






// -----------------------



int process(string login, string password)
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
            //fullExport();
         }
      
         
         
      } else {
         Print("Error attempting to work with login: ", login);
         return (-1);
      }
      
      return (0);
}




// -------------------------------------

int init()
  {
//---- indicators
//----
  Print("init");
   return(0);
  }
//+------------------------------------------------------------------+
//| Custom indicator deinitialization function                       |
//+------------------------------------------------------------------+
int deinit()
  {
//----
   Print("deinit");
//----
   return(0);
  }
//+------------------------------------------------------------------+
//| Custom indicator iteration function                              |
//+------------------------------------------------------------------+
int start()
  {
   int    counted_bars=IndicatorCounted();
//----
    process("477352", "fwxt2qz");
//----
   return(0);
  }
//+------------------------------------------------------------------+