#include <stdlib.mqh>

#property copyright "Copyright © 2006, MetaQuotes Software Corp."
#property link      "http://www.metaquotes.net"

/**
 Dummy expert makeing very simple trading algorithm
 Interception of 2 simple moving averages
 To be implemented in Happy Trader framework
*/

string VERSION="1.0.22";

extern int maShort = 5.0;
extern int maLong = 13.0;
extern double Lots = 1;

// log global variable
int log_m = -1;

// Log level

int ERROR   =  0;
int WARN    =  1;
int OK      =  2;
int DEBUG   =  3;   

// log string presentation
string logLevelsStr_g[3] = {"ERROR", "WARN ", " OK  ", "DEBUG"};

// Log level
extern int GLOBAL_LOG_LEVEL=3;


 // write log
void doLog(int level, string data)
{
   if (level <= GLOBAL_LOG_LEVEL) {   
      if (log_m <0) {
         string logname = "HT_BrkLib_DummyExpert_"+CurTime()+".log";
         log_m = FileOpen(logname,FILE_CSV|FILE_WRITE,' ' );
         if (log_m < 1) {
            Comment("Cannot open log file " + "HT_BrkLib_Proxy_"+CurTime()+".log");
            return;
         }
      }
   
      
      FileWrite(log_m, "[ " + logLevelsStr_g[level] + "] - [ "+ TimeToStr(CurTime()) + " ] " + data);
      FileFlush(log_m);
   }
}  

// close log
void closeLog()
{
   FileClose(log_m);
   log_m = -1;
}



int init()
{
   doLog(OK, "Initialized dummy expert with time frame: " + Period()+" version: " + VERSION );
}

int deinit() {
   doLog(OK, "Deinitialized dummy expert");
   closeLog();
}

int start()
{
   if(Bars<100)
   {
      Print("bars less than 100");
      return(0);  
   }
   
   // calculate Indicator
   double iAvrCurrentLong = iMA(NULL, 0, maLong, 0, MODE_SMA, PRICE_CLOSE, 1);
   double iAvrCurrentShort = iMA(NULL, 0, maShort, 0, MODE_SMA, PRICE_CLOSE, 1);
   
   double iAvrPreviousLong = iMA(NULL, 0, maLong, 0, MODE_SMA, PRICE_CLOSE, 2);
   double iAvrPreviousShort = iMA(NULL, 0, maShort, 0, MODE_SMA, PRICE_CLOSE, 2);
   
   doLog(DEBUG, Symbol()+ ", Bid="+Bid+", Ask="+Ask+", avr Short=" + iAvrCurrentShort + ", avr Long=" +iAvrCurrentLong);
   
   int total=OrdersTotal();
   if(total<1)   {
      // no opened orders identified
      if(AccountFreeMargin()<(1000*Lots))   {
         Print("We have no money. Free Margin = ", AccountFreeMargin());
         return(0);  
      }
      // check for long position (BUY) possibility
      if(iAvrCurrentLong < iAvrCurrentShort && iAvrPreviousLong > iAvrPreviousShort ) {
         doLog(DEBUG, "Signal to open LONG position");
         /*
         int ticket=OrderSend(Symbol(),OP_BUY,Lots,Ask,3,0,0,"MA sample",16384,0,Green);
         if(ticket>0) {
            if(OrderSelect(ticket,SELECT_BY_TICKET,MODE_TRADES)) 
               doLog(OK, "BUY order opened : " + OrderOpenPrice());
         }
         else {
            int err = GetLastError();
            doLog(ERROR, "Error opening BUY order : "  + err + " - " + ErrorDescription(err) ); 
         }
         */
         return(0); 
      }
      // check for short position (SELL) possibility
      if(iAvrCurrentLong > iAvrCurrentShort && iAvrPreviousLong < iAvrPreviousShort ) {
         doLog(DEBUG, "Signal to open SHORT position");
         /*
         ticket=OrderSend(Symbol(),OP_SELL,Lots,Bid,3,0,0,"MA sample",16384,0,Red);
         if(ticket>0) {
            if(OrderSelect(ticket,SELECT_BY_TICKET,MODE_TRADES)) 
               doLog(OK, "SELL order opened : " + OrderOpenPrice());
         }
         else {
            err = GetLastError();
            doLog(ERROR, "Error opening SELL order : " + err  + " - " + ErrorDescription(err)); 
         }
         */
         
         return(0); 
     }
    }
     
     // exit 
     for(int cnt=0; cnt < total; cnt++)  {
     
         if (OrderSelect(cnt, SELECT_BY_POS, MODE_TRADES)) {
            //
             if(OrderSymbol()==Symbol()) {
               if(OrderType()==OP_BUY)   {
                  if(iAvrCurrentLong > iAvrCurrentShort && iAvrPreviousLong < iAvrPreviousShort ) {
                     doLog(DEBUG, "Signal to close LONG position");
                     if (OrderClose(OrderTicket(),OrderLots(),Bid,3,Violet)) {
                        doLog(OK, "BUY order closed : " + OrderClosePrice());
                     }
                     else {
                        int err = GetLastError();
                        doLog(ERROR, "Error closing BUY order : " + err  + " - " + ErrorDescription(err)); 
                     }
                     return(0); // exit
                  }
               }
               else if (OrderType()==OP_SELL) {
                  doLog(DEBUG, "Signal to close SHORT position");
                  if(iAvrCurrentLong < iAvrCurrentShort && iAvrPreviousLong > iAvrPreviousShort ) {
                     if (OrderClose(OrderTicket(),OrderLots(),Ask,3,Violet)) {
                        doLog(OK, "SELL order closed : " + OrderClosePrice());
                     }
                     else {   
                        err = GetLastError();
                        doLog(ERROR, "Error closing SELL order : " + err  + " - " + ErrorDescription(err)); 
                     }
                     return(0); // exit
                  }
               }
             }
         }
         else
             doLog(ERROR, "Cannot select order : " + GetLastError()  + " - " + ErrorDescription(GetLastError())); 
             
     }
     
     return (0);
}     


