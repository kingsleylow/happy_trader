
// ImportNetEventsProcDLL.mqh

#import "NetEventsProcDLL.dll"
// Only for Clients:
int ConnectTo(string  ps8_ServerIP, // in - string ps8_ServerIP = "0123456789123456"
              int     s32_Port,     // in 
              int&    ph_Client[]); // out - int ph_Client[1]
                 
int ConnectClose(int h_Client);    // in
//
// Only for Server:
int ServerOpen(int  s32_Port);      // in

int GetAllConnections(int& ph_Client[],           // out - int ph_Client[62]
                      int& ps32_ClientIP[],       // out - int ps32_ClientIP[62]
                      int& ps32_ClientCount[]);   // out - int ps32_ClientCount[1]
                       
int DisconnectClient(int h_Client); // in

int ServerClose();
//
// For both: Clients and Server
int SendToInt   (int  h_Client,        // in
                 int& ps32_SendBuf[],  // in
                 int  s32_SendBufLen); // in - SendBuf[] array size in int element 
                 
int SendToDouble(int     h_Client,        // in
                 double& pd_SendBuf[],    // in
                 int     s32_SendBufLen); // in - SendBuf[] array size in double element 
                 
int SendToString(int    h_Client,        // in
                 string ps8_SendBuf,     // in
                 int    s32_SendBufLen); // in - SendBuf string size in char element
                 

int ReadFromInt   (int h_Client,            // in
                   int& ps32_ReadBuf[],     // in 
                   int  s32_ReadBufLen,     // in  - ReadBuf[] array size in int element
                   int& ps32_ReadLen[]);    // out - int ps32_ReadLen[1] - count of actually read data in int element
                  
int ReadFromDouble(int     h_Client,        // in
                   double& pd_ReadBuf[],    // in
                   int     s32_ReadBufLen,  // in  - ReadBuf[] array size in double element
                   int&    ps32_ReadLen[]); // out - int ps32_ReadLen[1] - count of actually read data in double element
                   
int ReadFromString(int     h_Client,        // in
                   string  ps8_ReadBuf,     // in
                   int     s32_ReadBufLen,  // in  - ReadBuf   string size in char element
                   int&    ps32_ReadLen[]); // out - int ps32_ReadLen[1] - count of actually read data in char element
//                   
#import
//***************************************************************************************

#define WSAEINTR                 1004
#define WSAEBADF                 1009
#define WSAEACCES                10013
#define WSAEFAULT                10014
#define WSAEINVAL                10022
#define WSAEMFILE                10024
#define WSAEWOULDBLOCK           10035
#define WSAEINPROGRESS           10036
#define WSAEALREADY              10037
#define WSAENOTSOCK              10038
#define WSAEDESTADDRREQ          10039
#define WSAEMSGSIZE              10040
#define WSAEPROTOTYPE            10041
#define WSAENOPROTOOPT           10042
#define WSAEPROTONOSUPPORT       10043
#define WSAESOCKTNOSUPPORT       10044
#define WSAEOPNOTSUPP            10045
#define WSAEPFNOSUPPORT          10046
#define WSAEAFNOSUPPORT          10047
#define WSAEADDRINUSE            10048
#define WSAEADDRNOTAVAIL         10049 
#define WSAENETDOWN              10050 
#define WSAENETUNREACH           10051 
#define WSAENETRESET             10052 
#define WSAECONNABORTED          10053 
#define WSAECONNRESET            10054 
#define WSAENOBUFS               10055
#define WSAEISCONN               10056
#define WSAENOTCONN              10057
#define WSAESHUTDOWN             10058
#define WSAETOOMANYREFS          10059
#define WSAETIMEDOUT             10060
#define WSAECONNREFUSED          10061
#define WSAELOOP                 10062 
#define WSAENAMETOOLONG          10063
#define WSAEHOSTDOWN             10064 
#define WSAEHOSTUNREACH          10065
#define WSAENOTEMPTY             10066
#define WSAEPROCLIM              10067
#define WSAEUSERS                10068 
#define WSAEDQUOT                10069 
#define WSAESTALE                10070 
#define WSAEREMOTE               10071
#define WSASYSNOTREADY           10091
#define WSAVERNOTSUPPORTED       10092
#define WSANOTINITIALISED        10093
#define WSAEDISCON               10101
#define WSAENOMORE               10102
#define WSAECANCELLED            10103
#define WSAEINVALIDPROCTABLE     10104
#define WSAEINVALIDPROVIDER      10105
#define WSAEPROVIDERFAILEDINIT   10106
#define WSASYSCALLFAILURE        10107 
#define WSASERVICE_NOT_FOUND     10108
#define WSATYPE_NOT_FOUND        10109
#define WSA_E_NO_MORE            10110
#define WSA_E_CANCELLED          10111
#define WSAEREFUSED              10112
#define WSAHOST_NOT_FOUND        11001
#define WSATRY_AGAIN             11002 
#define WSANO_RECOVERY           11003
#define WSANO_DATA               11004
#define WSA_IO_PENDING           997 
#define WSA_IO_INCOMPLETE        996
#define WSA_INVALID_HANDLE       6 
#define WSA_INVALID_PARAMETER    87
#define WSA_NOT_ENOUGH_MEMORY    8
#define WSA_OPERATION_ABORTED    995

#define ERROR_INVALID_PARAMETER  87
#define INADDR_NONE              0xffffffff
#define ERROR_TIMEOUT            1460
#define OK                       0

// Get a human readable error message for an API error code
string GetErrMsg(int s32_Error)
{
	// Some translations of error codes are really stupid --> show the original error code.
	string s_NoExplanationYet = " - We have no yet explanation for this error";
	
	switch (s32_Error)
	{
		case WSAEINTR:                return("WSAEINTR");  
		case WSAEBADF:                return("WSAEBADF"); 
		case WSAEACCES:               return("WSAEACCES");  
		case WSAEFAULT:               return("WSAEFAULT");  
		case WSAEINVAL:               return("WSAEINVAL");  
		case WSAEMFILE:               return("WSAEMFILE");  
		case WSAEWOULDBLOCK:          return("WSAEWOULDBLOCK");  
		case WSAEINPROGRESS:          return("WSAEINPROGRESS");  
		case WSAEALREADY:             return("WSAEALREADY");  
		case WSAENOTSOCK:             return("WSAENOTSOCK");  
		case WSAEDESTADDRREQ:         return("WSAEDESTADDRREQ");  
		case WSAEMSGSIZE:             return("WSAEMSGSIZE");  
		case WSAEPROTOTYPE:           return("WSAEPROTOTYPE");  
		case WSAENOPROTOOPT:          return("WSAENOPROTOOPT");  
		case WSAEPROTONOSUPPORT:      return("WSAEPROTONOSUPPORT");  
		case WSAESOCKTNOSUPPORT:      return("WSAESOCKTNOSUPPORT");  
		case WSAEOPNOTSUPP:           return("WSAEOPNOTSUPP");  
		case WSAEPFNOSUPPORT:         return("WSAEPFNOSUPPORT");  
		case WSAEAFNOSUPPORT:         return("WSAEAFNOSUPPORT");  
		case WSAEADDRINUSE:           return("WSAEADDRINUSE");  
		case WSAEADDRNOTAVAIL:        return("WSAEADDRNOTAVAIL");  
		case WSAENETDOWN:             return("WSAENETDOWN");  
		case WSAENETUNREACH:          return("WSAENETUNREACH");  
		case WSAENETRESET:            return("WSAENETRESET");  
		case WSAECONNABORTED:         return("WSAECONNABORTED");  
		case WSAECONNRESET:           return("WSAECONNRESET");  
		case WSAENOBUFS:              return("WSAENOBUFS");  
		case WSAEISCONN:              return("WSAEISCONN");  
		case WSAENOTCONN:             return("WSAENOTCONN");  
		case WSAESHUTDOWN:            return("WSAESHUTDOWN");  
		case WSAETOOMANYREFS:         return("WSAETOOMANYREFS");  
		case WSAETIMEDOUT:            return("WSAETIMEDOUT");  
		case WSAECONNREFUSED:         return("WSAECONNREFUSED");  
		case WSAELOOP:                return("WSAELOOP");  
		case WSAENAMETOOLONG:         return("WSAENAMETOOLONG");  
		case WSAEHOSTDOWN:            return("WSAEHOSTDOWN");  
		case WSAEHOSTUNREACH:         return("WSAEHOSTUNREACH");  
		case WSAENOTEMPTY:            return("WSAENOTEMPTY");  
		case WSAEPROCLIM:             return("WSAEPROCLIM");  
		case WSAEUSERS:               return("WSAEUSERS");  
		case WSAEDQUOT:               return("WSAEDQUOT");  
		case WSAESTALE:               return("WSAESTALE");  
		case WSAEREMOTE:              return("WSAEREMOTE");  
		case WSASYSNOTREADY:          return("WSASYSNOTREADY");  
		case WSAVERNOTSUPPORTED:      return("WSAVERNOTSUPPORTED");  
		case WSANOTINITIALISED:       return("WSANOTINITIALISED");  
		case WSAEDISCON:              return("WSAEDISCON");  
		case WSAENOMORE:              return("WSAENOMORE");  
		case WSAECANCELLED:           return("WSAECANCELLED");  
		case WSAEINVALIDPROCTABLE:    return("WSAEINVALIDPROCTABLE");  
		case WSAEINVALIDPROVIDER:     return("WSAEINVALIDPROVIDER");  
		case WSAEPROVIDERFAILEDINIT:  return("WSAEPROVIDERFAILEDINIT");  
		case WSASYSCALLFAILURE:       return("WSASYSCALLFAILURE");  
		case WSASERVICE_NOT_FOUND:    return("WSASERVICE_NOT_FOUND");  
		case WSATYPE_NOT_FOUND:       return("WSATYPE_NOT_FOUND");  
		case WSA_E_NO_MORE:           return("WSA_E_NO_MORE");  
		case WSA_E_CANCELLED:         return("WSA_E_CANCELLED");  
		case WSAEREFUSED:             return("WSAEREFUSED");  
		case WSAHOST_NOT_FOUND:       return("WSAHOST_NOT_FOUND");  
		case WSATRY_AGAIN:            return("WSATRY_AGAIN");  
		case WSANO_RECOVERY:          return("WSANO_RECOVERY");  
		case WSANO_DATA:              return("WSANO_DATA");  
		case WSA_IO_PENDING:          return("WSA_IO_PENDING");  
		case WSA_IO_INCOMPLETE:       return("WSA_IO_INCOMPLETE");  
		case WSA_INVALID_HANDLE:      return("WSA_INVALID_HANDLE");  
		case WSA_INVALID_PARAMETER:   return("WSA_INVALID_PARAMETER");  
		case WSA_NOT_ENOUGH_MEMORY:   return("WSA_NOT_ENOUGH_MEMORY");  
		case WSA_OPERATION_ABORTED:   return("WSA_OPERATION_ABORTED");  
		
		case ERROR_INVALID_PARAMETER: return("ERROR_INVALID_PARAMETER");
		case INADDR_NONE:             return("INADDR_NONE"); 
		case ERROR_TIMEOUT:           return("ERROR_TIMEOUT");
		case OK:                      return("OK");
		default:
		   return (DoubleToStr(s32_Error,0)+s_NoExplanationYet);
	}
}

// Convert DWORD IP to string IP
string FormatIP(int IP)
{
   int s32_Byte0 = 0x000000FF & IP;
   int s32_Byte1 = 0x000000FF & (IP>>8);
   int s32_Byte2 = 0x000000FF & (IP>>16);
   int s32_Byte3 = 0x000000FF & (IP>>24);
   
   return(DoubleToStr(s32_Byte0,0)+"."+
          DoubleToStr(s32_Byte1,0)+"."+
          DoubleToStr(s32_Byte2,0)+"."+
          DoubleToStr(s32_Byte3,0));
}	