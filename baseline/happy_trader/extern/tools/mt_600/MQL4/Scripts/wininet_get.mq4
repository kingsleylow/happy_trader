bool bWinInetDebug = false;

string errorMsg;
 
int hSession_IEType_g = 0;
int hSession_Direct_g = 0;
int Internet_Open_Type_Preconfig = 0;
int Internet_Open_Type_Direct = 1;
int Internet_Open_Type_Proxy = 3;
int Buffer_LEN = 250;


#import "wininet.dll"
 
#define INTERNET_FLAG_PRAGMA_NOCACHE    0x00000100 // Forces the request to be resolved by the origin server, even if a cached copy exists on the proxy.
#define INTERNET_FLAG_NO_CACHE_WRITE    0x04000000 // Does not add the returned entity to the cache. 
#define INTERNET_FLAG_RELOAD            0x80000000 // Forces a download of the requested file, object, or directory listing from the origin server, not from the cache.
#define INTERNET_FLAG_KEEP_CONNECTION   0x00400000  // use keep-alive semantics
#define INTERNET_OPEN_TYPE_PRECONFIG                    0   // use registry configuration
#define INTERNET_SERVICE_HTTP   3
#define HTTP_ADDREQ_FLAG_ADD            0x20000000
#define HTTP_ADDREQ_FLAG_REPLACE        0x80000000
int InternetOpenW(
    string     &sAgent,
    int        lAccessType,
    string     &sProxyName,
    string     &sProxyBypass,
    int     lFlags=0
);
 
int InternetOpenUrlW(
    int     hInternetSession,
    string    &sUrl, 
    string    &sHeaders,
    int     lHeadersLength=0,
    int     lFlags=0,
    int     lContext=0 
);
 
int InternetReadFile(int hFile,uchar &sBuffer[],int lNumBytesToRead,int &lNumberOfBytesRead);
 
int InternetCloseHandle(
    int     hInet
);

int HttpOpenRequestW(
    int hConnect,
    string &lpszVerb,
    string &lpszObjectName,
    string &lpszVersion,
    string &lpszReferrer,
    string &lplpszAcceptTypes,
    int  dwFlags,
    int  dwContext);
    
int  InternetOpenW(
    string &lpszAgent,
    int dwAccessType,
    string &lpszProxy,
    string &lpszProxyBypass,
    int dwFlags
    );    
    
int InternetConnectW(
    int hInternet,
    string &lpszServerName,
    int nServerPort,
    string &lpszUserName,
    string &lpszPassword,
    int dwService,
    int dwFlags,
    int dwContext
    );

bool HttpSendRequestW(
    int hRequest,
    string &lpszHeaders,
    int dwHeadersLength,
    string &lpOptional,
    int dwOptionalLength
    );  
    
bool HttpAddRequestHeadersW(
    int hRequest,
    string &lpszHeaders,
    int dwHeadersLength,
    int  dwModifiers
    );          
#import



int logId;
void Log(string st)
{
   if(logId>=0)
   {
      
      FileWrite(logId, TimeToStr(TimeCurrent(),TIME_DATE|TIME_SECONDS) + ": " + st);
   }
}



void  OnStart() 
{ 
   string answer="";
   GrabWeb("http://www.ya.ru", answer);
   Print("Result length:", StringLen(answer));
   
      int r=FileOpen("foo.txt",FILE_BIN|FILE_WRITE);
      FileWriteString(r,answer);
      FileClose(r);
}



int hSession(bool Direct)
{
    string InternetAgent;
    string nill="";
    if (hSession_IEType_g == 0)
    {
        InternetAgent = "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; Q312461)";
       
        hSession_IEType_g = InternetOpenW(InternetAgent, Internet_Open_Type_Preconfig, nill, nill, 0);
        hSession_Direct_g = InternetOpenW(InternetAgent, Internet_Open_Type_Direct, nill, nill, 0);
        
        
    }
    if (Direct) 
    { 
        return(hSession_Direct_g); 
    }
    else 
    {
        return(hSession_IEType_g); 
    }
}

void readPage(int hRequest,string &Out)
{
   // читаем страницу 
   uchar ch[100];
   string toStr="";
   int dwBytes,h;
   while(InternetReadFile(hRequest,ch,100,dwBytes))
   {
      if(dwBytes<=0) break;
      toStr=toStr+CharArrayToString(ch,0,dwBytes);
   }
   Out=toStr;
}

bool GrabWeb(string strUrl, string& strWebPage)
{
   Print("Starting...");
   
    int     hInternet;
    int        iResult;
   
    
    int hSessionVar = hSession(FALSE);
    
    string nill="";
    
    hInternet = InternetOpenUrlW(hSessionVar, strUrl, nill, 0, 
                                INTERNET_FLAG_NO_CACHE_WRITE | 
                                INTERNET_FLAG_PRAGMA_NOCACHE | 
                                INTERNET_FLAG_RELOAD, 0);
                                
 
    
    if (hInternet == 0) 
        return(false);
        
   Print("Reading data...");
    readPage(hInternet, strWebPage);
   
      
    iResult = InternetCloseHandle(hInternet);
    if (iResult == 0) 
        return(false);
   
    return(true);
}

void OnDeinit(const int reason) {
   FileClose(logId);
  
}