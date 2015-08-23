//+------------------------------------------------------------------+
//|                                              create_accounts.mq4 |
//|                        Copyright 2013, MetaQuotes Software Corp. |
//|                                        http://www.metaquotes.net |
//+------------------------------------------------------------------+
#property copyright "Copyright 2013, MetaQuotes Software Corp."
#property link      "http://www.metaquotes.net"


#include <WinUser32.mqh>

#import "user32.dll"
  int GetParent( int hWnd );
  int GetDlgItem( int hDlg, int nIDDlgItem );
  int GetLastActivePopup( int hWnd );
  int GetAncestor (int hWnd, int gaFlags);


#import "kernel32.dll"
   void GetLocalTime(int& TimeArray[]);
   
   
#define VK_HOME 0x24
#define VK_DOWN 0x28
#define VK_ENTER 0x0D
#define OPENLOGINWINDOW_COMMAND_CODE  35429

 
void Login(  )
{
   int hwnd = WindowHandle(Symbol(), Period());
   int hwnd_parent = 0;
 
   while (!IsStopped())
   {
      hwnd = GetParent(hwnd);
      
      if (hwnd == 0)
        break;
        
      hwnd_parent = hwnd;
   }
   
   
   if (hwnd_parent != 0)  // нашли главное окно
   {
     PostMessageA(hwnd_parent,WM_COMMAND,100001,0);
     Sleep(1000);
     
      
     int dialog_login_hwnd=GetLastActivePopup(hwnd_parent);  
     
     if (dialog_login_hwnd != 0) {
     
            int hlogin_edit=GetDlgItem(GetDlgItem(dialog_login_hwnd,0x49D),0x3E9);
				int hlogin_password=GetDlgItem(dialog_login_hwnd,0x4C4);
				int hlogin_server=GetDlgItem(GetDlgItem(dialog_login_hwnd,0x50D),0x3E9);
				
				
     }
     
     
     
   }
 
  return;
}

int start()
{
   Print("Started");
   
  
   Login();
   
   Print("Finished");
   
   return(0);
}
//+------------------------------------------------------------------+