//+---------------------------------------------------------------------------+
//|                                            Client.mq4                     |
//|                      Copyright © 2012, http://www.mql4.com/ru/users/more  |
//|                                       tradertobe@gmail.com                |
//+---------------------------------------------------------------------------+
#property copyright "Copyright © 2012, http://www.mql4.com/ru/users/more"
#property link      "http://www.mql4.com/ru/users/more"
#include <ImportNetEventsProcDLL.mqh>
/*int ConnectTo(string  ps8_ServerIP, // in - string ps8_ServerIP = "0123456789123456"
                int     s32_Port,     // in 
                int&    ph_Client[]); // out - int ph_Client[1]
*/
/*int SendToDouble(int     h_Client,        // in
                   double& pd_SendBuf[],    // in
                   int     s32_SendBufLen); // in - SendBuf[] array size in double element 
*/
/*int ReadFromDouble(int     h_Client,        // in
                     double& pd_ReadBuf[],    // in
                     int     s32_ReadBufLen,  // in  - ReadBuf[] array size in double element
                     int&    ps32_ReadLen[]); // out - int ps32_ReadLen[1] - count of actually read data in double element
*/                      
/*int ConnectClose(int h_Client);    // in
*/ 
       
// Globals variables
int s32_Error;
int i;
// for int ConnectTo(ps8_ServerIP, s32_Port, ph_Client);  // out - int h_Client[1]
string ps8_ServerIP = "127.0.0.1";  // mine local IP
int    s32_Port = 2000;
int    ph_Client[1];

bool b_ConnectTo = false;

// for int SendToDouble(ph_Client[0], pd_Buf, s32_BufLen);  
// for int ReadFromDouble(ph_Client[0], pd_Buf, s32_BufLen, ps32_ReadLen);
double pd_Buf[1];
int    s32_BufLen = 1;
int    ps32_ReadLen[1];
//+------------------------------------------------------------------+
//| expert initialization function                                   |
//+------------------------------------------------------------------+
int init()
{
//----
   if (!b_ConnectTo)
   {
      s32_Error = ConnectTo(ps8_ServerIP, s32_Port, ph_Client);
      Print("ConnectTo(...) return is: ",GetErrMsg(s32_Error));
      Print("ConnectTo(...) handle is: ",ph_Client[0]);
      
      if (s32_Error == OK)
      {
         b_ConnectTo = true;
         Print("Client now is ConnectTo the Server: ",ps8_ServerIP);
      }
   }
//----
   return(0);
}
//+------------------------------------------------------------------+
//| expert deinitialization function                                 |
//+------------------------------------------------------------------+
int deinit()
{
//----
   if (b_ConnectTo)
   {
      s32_Error = ConnectClose(ph_Client[0]);
      Print("ConnectClose(...) return is: ",GetErrMsg(s32_Error));
      
      if (s32_Error == OK)
         b_ConnectTo = false;
   }
//----
   return(0);
}  

int start()
{
//----
   if (!b_ConnectTo)
      return(0);
   
   RefreshRates();
   
   double pd_Value[1];
   
   pd_Value[0] = NormalizeDouble(Bid,Digits);
      
   s32_Error = SendToDouble(ph_Client[0], pd_Value, s32_BufLen);
   
   
   if (s32_Error != 0)
   {
      Print("SendToDouble(",ph_Client[0],"...) return is: ",GetErrMsg(s32_Error));
      return(1);
   }
   else
      Print("SendToDouble(",ph_Client[0],"...) return is: OK");
      
   s32_Error = ReadFromDouble(ph_Client[0], pd_Buf, s32_BufLen, ps32_ReadLen);      
   
   if (s32_Error != 0)
   {
      Print("ReadFromDouble(",ph_Client[0],"...) return is: ", GetErrMsg(s32_Error));
      return(1);
   }
   else
      Print("ReadFromDouble(",ph_Client[0],"...) return is: OK"); 
   
   pd_Buf[0] = NormalizeDouble(pd_Buf[0],Digits);
   
   if (ps32_ReadLen[0] > 0)
      Print("Read doble value is: ", pd_Buf[0]);
   
//----
   return(0);
  }
//+------------------------------------------------------------------+