//+------------------------------------------------------------------+
//|                        Copyright 2013,          Victor Zoubok    |
//+------------------------------------------------------------------+
#property copyright "Copyright 2013, MetaQuotes Software Corp."
#property link      "http://www.metaquotes.net"

#property show_inputs
extern int sc_x_dim_p = 1000;
extern int sc_y_dim_p = 764;
extern string url_p = "http://blog.goit.co/test.html";
extern string username_p = "harris";
extern string password_p = "pass";
extern string title_p = "title";
extern string comment_p = "title";





/**
* flags returned by tryLock (expected to be boolean but must comply with DLL_FUN_CALL_OK or DLL_FUN_CALL_FAILURE)
*/

int DLL_FUN_CALL_TRUE   = 1;
int DLL_FUN_CALL_FALSE  = 0;

/** 
* These are log level values. They control how much information is written into log files
* Log files are written into <MT path>\logs. 
*/


int MSG_FATAL  = 0;
int MSG_ERROR  = 1;
int MSG_WARN  = 2;
int MSG_INFO  = 3;
int MSG_DEBUG  = 4;
int MSG_DEBUG_DETAIL  = 5;


// buffer size to read strings
#define BUFFERSIZE  256



/** 
* Imported functions
* 
*/
#import "htmtqueue_bck.dll"
   
   
   int setLoglevel(int loglevel);
	void getUid( string& terminal_uid[] );
	int pushFileToBlog(
	  string content, 
	  int contentLength,
	  string url, 
	  string user, 
	  string password,
	  string title,
	  string comment,
	  string filename,
	  string& statusMsg[]
   );

		
#import

//+------------------------------------------------------------------+
//| Кодирование данных в base64                                      |
//+------------------------------------------------------------------+

static int ExtBase64Encode[64]={ 'A','B','C','D','E','F','G','H','I','J','K','L','M',
                                 'N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
                                 'a','b','c','d','e','f','g','h','i','j','k','l','m',
                                 'n','o','p','q','r','s','t','u','v','w','x','y','z',
                                 '0','1','2','3','4','5','6','7','8','9','+','/'      };
static int ExtBase64Decode[256]={
                    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
                    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
                    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  62,  -1,  -1,  -1,  63,
                    52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  -1,  -1,  -1,  -2,  -1,  -1,
                    -1,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
                    15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  -1,  -1,  -1,  -1,  -1,
                    -1,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,
                    41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  -1,  -1,  -1,  -1,  -1,
                    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
                    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
                    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
                    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
                    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
                    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
                    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
                    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1 };
                               
//+------------------------------------------------------------------+
//| Кодирование данных в base64                                      |
//+------------------------------------------------------------------+

void Base64EncodeFromArray(int& in[], int len, string &out)
  {
   int i=0,pad=0;
//---- пройдемся и закодируем
   while(i<len)
     {
      //---- извлекаем байты
      int b3,b2,b1=in[i];
      i++;
      if(i>=len) { b2=0; b3=0; pad=2; }
      else
        {
         b2=in[i];
         i++;
         if(i>=len) { b3=0; pad=1; }
         else       { b3=in[i]; i++; }
        }
      //----
      int c1=(b1 >> 2);
      int c2=(((b1 & 0x3) << 4) | (b2 >> 4));
      int c3=(((b2 & 0xf) << 2) | (b3 >> 6));
      int c4=(b3 & 0x3f);  

      out=out+CharToStr(ExtBase64Encode[c1]);
      out=out+CharToStr(ExtBase64Encode[c2]);
      switch(pad)
        {  
         case 0:
           out=out+CharToStr(ExtBase64Encode[c3]);
           out=out+CharToStr(ExtBase64Encode[c4]);
           break;
         case 1:
           out=out+CharToStr(ExtBase64Encode[c3]);
           out=out+"=";
           break;
         case 2:
           out=out+"==";
           break;
        }
     }
//----
  }
  
void Base64Encode(string in,string &out)
  {
   int i=0,pad=0,len=StringLen(in);
//---- пройдемся и закодируем
   while(i<len)
     {
      //---- извлекаем байты
      int b3,b2,b1=StringGetChar(in,i);
      i++;
      if(i>=len) { b2=0; b3=0; pad=2; }
      else
        {
         b2=StringGetChar(in,i);
         i++;
         if(i>=len) { b3=0; pad=1; }
         else       { b3=StringGetChar(in,i); i++; }
        }
      //----
      int c1=(b1 >> 2);
      int c2=(((b1 & 0x3) << 4) | (b2 >> 4));
      int c3=(((b2 & 0xf) << 2) | (b3 >> 6));
      int c4=(b3 & 0x3f);

      out=out+CharToStr(ExtBase64Encode[c1]);
      out=out+CharToStr(ExtBase64Encode[c2]);
      switch(pad)
        {
         case 0:
           out=out+CharToStr(ExtBase64Encode[c3]);
           out=out+CharToStr(ExtBase64Encode[c4]);
           break;
         case 1:
           out=out+CharToStr(ExtBase64Encode[c3]);
           out=out+"=";
           break;
         case 2:
           out=out+"==";
           break;
        }
     }
//----
  }
//+------------------------------------------------------------------+
//| Декодирование данных из Base64                                   |
//+------------------------------------------------------------------+
void Base64Decode(string in,string &out)
  {
   int i=0,len=StringLen(in);
   int shift=0,accum=0;
//---- идем до конца
   while(i<len)
     {
      //---- извлечем код
      int value=ExtBase64Decode[StringGetChar(in,i)];
      if(value<0 || value>63) break; // конец или неверная кодировка
      //---- развернем код
      accum<<=6;
      shift+=6;
      accum|=value;
      if(shift>=8)
        {
         shift-=8;
         value=accum >> shift;
         out=out+CharToStr(value & 0xFF);
        } 
      i++;
     }
//----
  }
//+------------------------------------------------------------------+


string getUidWrapper()
{
	string uid[1];
	//uid[0] = "00000000000000000000000000000000";
	uid[0] = "12345678910111213141516171819202";
   getUid( uid );

	return (uid[0]);
}


int start()
{
		
	 setLoglevel(MSG_DEBUG_DETAIL);
	 
	
	 Print("Making screenshot...");
	 string filename = getUidWrapper() + ".gif";
	 Print("filename: ", filename);

	 if (!WindowScreenShot(filename, sc_x_dim_p, sc_y_dim_p)) {
		Print("WindowScreenShot(...) failed: ", GetLastError());
		return(-1);
	 }
	 
	 int fhndl = FileOpen(filename, FILE_BIN | FILE_READ );
	 
	 if(fhndl <1 )
    {
      Print("GIF cannot be opened: ", GetLastError());
     	return(-1);
    }
    
    Print("Opened GIF file: ", filename);
    
    int fs = FileSize(fhndl);
    if (fs <=0) {
      Print("GIF is empty!");
      FileClose(fhndl); 
     	return(-1);
    }
    
    Print("Reading file content of size: ", fs);
    
    int output_buff[];
    ArrayResize(output_buff, fs);
      
    int cnt = 0;
    while(!FileIsEnding(fhndl)) {
      int char_s = FileReadInteger( fhndl, CHAR_VALUE);
      output_buff[cnt] = char_s;
      
      cnt = cnt+1;
      if (cnt >=fs)
         break;
      
    }
    
    string encoded_content;
    Base64EncodeFromArray(output_buff,fs, encoded_content );
    
        
    /*
    if (fs != StringLen(content)) {
      Print("Invalid GIF read, string length is: ",StringLen(content) );
      FileClose(fhndl); 
     	return(-1);
    }
    */
    
    
    int encoded_content_len = StringLen(encoded_content);
    Print("Encoded length: ", encoded_content_len);
     
    string statusMsg[1];
    statusMsg[0] = "r                                                                                                                                          ";
    
      int rpost= pushFileToBlog(
	    encoded_content, 
	    encoded_content_len,
	    url_p, 
	    username_p, 
	    password_p,
	    title_p,
	    comment_p,
	    filename,
	    statusMsg
     );
   
     if (rpost != DLL_FUN_CALL_TRUE) {
         Print("Exception on POST data: ", statusMsg[0]);
     }
     else {
         Print("Successfully posted data: ", statusMsg[0]);
     }
     
    
    // pass this content further
    
    FileClose(fhndl); 
    Print("Close GIF file");



	
   return(0);
}
//+---------------------------------------------------------------