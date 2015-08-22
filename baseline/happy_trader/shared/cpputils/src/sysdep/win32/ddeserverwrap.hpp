#ifndef _CPPUTILS_DDESERVERWRAP_INCLUDED
#define _CPPUTILS_DDESERVERWRAP_INCLUDED


#include "../../cpputilsdefs.hpp"
#include "../../mutex.hpp"
#include "ddewrap.hpp"
#include <ddeml.h>


namespace CppUtils {

//------------------
// custom formats
//#define CF_CUSTOM_XLTABLE 49840


// ------------------

typedef void FAR *POP;
typedef POP FAR *PPOP;

	// ----------------------
	// helper structures
#define SZCF_TEXT           "TEXT"        
#define SZCF_BITMAP         "BITMAP"      
#define SZCF_METAFILEPICT   "METAFILEPICT"
#define SZCF_SYLK           "SYLK"        
#define SZCF_DIF            "DIF"         
#define SZCF_TIFF           "TIFF"        
#define SZCF_OEMTEXT        "OEMTEXT"     
#define SZCF_DIB            "DIB"         
#define SZCF_PALETTE        "PALETTE"     
#define SZCF_PENDATA        "PENDATA"     
#define SZCF_RIFF           "RIFF"     
#define SZCF_WAVE           "WAVE"     
#define SZCF_UNICODETEXT    "UNICODETEXT" 
#define SZCF_ENHMETAFILE    "ENHMETAFILE" 

//
// String names for some standard DDE strings not
// defined in DDEML.H
//

#define SZ_READY            "Ready"
#define SZ_BUSY             "Busy"
#define SZ_TAB              "\t"
#define SZ_RESULT           "Result"
#define SZ_PROTOCOLS        "Protocols"
#define SZ_EXECUTECONTROL1  "Execute Control 1"

//
// Definition for a DDE Request processing function
//

// -----------------

class DDEServerWrap;

// ----------------
typedef HDDEDATA (CALLBACK DDEREQUESTFN)(UINT wFmt, HSZ hszTopic, HSZ hszItem);
typedef DDEREQUESTFN *PDDEREQUESTFN;

//
// Definition for a DDE Poke processing function
//

typedef BOOL (CALLBACK DDEPOKEFN)(UINT wFmt, HSZ hszTopic, HSZ hszItem, HDDEDATA hData);
typedef DDEPOKEFN *PDDEPOKEFN;

//
// Definition for a DDE Execute processing function
//

typedef BOOL (CALLBACK DDEEXECFN)(HSZ hszTopic, HDDEDATA hData);
typedef DDEEXECFN *PDDEEXECFN;

//
// Definition for a DDE execute command procession function
//

typedef BOOL (CALLBACK DDEEXECCMDFN)(struct _DDETOPICINFO FAR *pTopic,
                            LPSTR pszResultString,
                            UINT uiResultSize,
                            UINT uiNargs,
                            LPSTR FAR *ppArgs);
typedef DDEEXECCMDFN FAR *PDDEEXECCMDFN;

//
// Structure used to hold a clipboard id and its text name
//

typedef struct _CFTAGNAME {
    WORD wFmt;
    LPSTR pszName;
		
} CFTAGNAME, FAR *PCFTAGNAME;

//
// Structure used to store information on a DDE execute
// command processor function
//

typedef struct _DDEEXECCMDFNINFO {
    struct _DDEEXECCMDFNINFO FAR *pNext; // pointer to the next item
    struct _DDETOPICINFO FAR *pTopic; // pointer to the topic it belongs to
    LPSTR pszCmdName;           // The name of the command
    PDDEEXECCMDFN pFn;          // A pointer to the function
    UINT uiMinArgs;             // min number of args
    UINT uiMaxArgs;             // max number of args
} DDEEXECCMDFNINFO, FAR *PDDEEXECCMDFNINFO;

//
// Structure used to store information on a DDE item
//

typedef struct _DDEITEMINFO {
    struct _DDEITEMINFO FAR *pNext; // pointer to the next item
    LPSTR   pszItemName;        // pointer to its string name
    HSZ     hszItemName;        // DDE string handle for the name
    struct _DDETOPICINFO FAR *pTopic; // pointer to the topic it belongs to
    LPWORD  pFormatList;        // ptr to null term list of CF format words.
    PDDEREQUESTFN pfnRequest;   // pointer to the item specific request processor
    PDDEPOKEFN    pfnPoke;      // pointer to the item specific poke processor
    HDDEDATA    hData;          // data for this item (not used by stddde.c)
} DDEITEMINFO, FAR *PDDEITEMINFO;

//
// Structure used to store information on a DDE topic
//

typedef struct _DDETOPICINFO {
    struct _DDETOPICINFO FAR *pNext;// pointer to the next topic
    LPSTR   pszTopicName;       // pointer to its string name
    HSZ     hszTopicName;       // DDE string handle for the name
    PDDEITEMINFO pItemList;     // pointer to its item list
    PDDEEXECFN  pfnExec;        // pointer to the generic execute processor
    PDDEREQUESTFN pfnRequest;   // pointer to the generic request processor
    PDDEPOKEFN    pfnPoke;      // pointer to the generic poke processor
    PDDEEXECCMDFNINFO pCmdList; // pointer to the execute command list
} DDETOPICINFO, FAR *PDDETOPICINFO;

//
// Structure used to store information about a DDE conversation
//

typedef struct _DDECONVINFO {
    struct _DDECONVINFO FAR *pNext; // pointer to the next one
    HCONV hConv;                    // handle to the conversation
    HSZ hszTopicName;               // HSZ for the topic of the conversation
    PDDEITEMINFO pResultItem;       // pointer to a temp result item
} DDECONVINFO, FAR *PDDECONVINFO;

//
// Structure used to store information on a DDE server
// which has only one service
//

typedef struct _DDESERVERINFO {
    LPSTR   lpszServiceName;        // pointer to the service string name
    HSZ     hszServiceName;         // DDE string handle for the name
    PDDETOPICINFO pTopicList;       // pointer to the topic list
    DWORD dwDDEInstance;            // DDE Instance value
    PFNCALLBACK pfnStdCallback;     // pointer to standard DDE callback fn
    PFNCALLBACK pfnCustomCallback;  // pointer to custom DDE callback fn
    PDDECONVINFO pConvList;         // pointer to the active conversation list
} DDESERVERINFO, FAR *PDDESERVERINFO;


// --------------------------
// listener base class
class CPPUTILS_EXP DDEServerWrapListener
{
public:
	// client gives to the server
	virtual void onDDEDataPoke(DDEItem const& item, CppUtils::String const& value) = 0;

	// XLTable for further processing
	virtual void onDDEXLTablePoke(DDEItem const& item, CppUtils::Byte const* p, size_t pDataLength) = 0;

	// server gives the data upon client request
	virtual void onDDEDataRequested(DDEItem const& item, CppUtils::String& value) = 0;

	// return true if we want that our item will be self-registered
	virtual bool onUnknownDDEItem(DDEItem const& item) = 0;
};

// --------------------------


class CPPUTILS_EXP DDEServerWrap: public Mutex {
		
public:
		

		DDEServerWrap();

		virtual ~DDEServerWrap();

  
		void initServer( String const& apps);

		void uninitializeServer();

		// add & remove DDE items to process
		void addDDEItem(DDEItem const& item);

		void removeDDEItem(DDEItem const& item);

		// add & remove topics
		void addDDETopic(DDEItem const& item);

		void removeDDETopic(DDEItem const& item);

		// register and clear listener
		void registerListener(DDEServerWrapListener& listener);

		void clearListener();

		
		inline DDEServerWrapListener * getListener()
		{
			return pListener_m;
		}
		
	private:

		// notification happening when we have unknown 
		// if we want to add this item - just register
		PDDEITEMINFO onUnknownIemDDEItem(PDDETOPICINFO pTopic, HSZ hszItem);


		BOOL CALLBACK valuePoke( UINT wFmt, HSZ hszTopic, HSZ hszItem, HDDEDATA hData);
		stdcall_proxy call_valuePoke_m;

		HDDEDATA CALLBACK valueRequest(UINT wFmt, HSZ hszTopic, HSZ hszItem);
		stdcall_proxy call_valueRequest_m;

		BOOL InitializeDDE(   LPSTR lpszServiceName,
                          LPDWORD lpdwDDEInst,
                          PFNCALLBACK lpfnCustomCallback,
                          DWORD dwFilterFlags);

		void UninitializeDDE(void);

		PDDETOPICINFO AddDDETopic(LPSTR lpszTopic,
                                 PDDEEXECFN pfnExec,     
                                 PDDEREQUESTFN pfnRequest,
                                 PDDEPOKEFN pfnPoke);
		BOOL RemoveDDETopic(LPSTR lpszTopic);

		PDDEITEMINFO AddDDEItem(LPSTR lpszTopic, 
                               LPSTR lpszItem, 
                               LPWORD pFormatList,
                               PDDEREQUESTFN lpReqFn, 
                               PDDEPOKEFN lpPokeFn);
		BOOL RemoveDDEItem(LPSTR lpszTopic, LPSTR lpszItem);

		PDDEEXECCMDFNINFO AddDDEExecCmd(LPSTR pszTopic, 
                                       LPSTR pszCmdName,
                                       PDDEEXECCMDFN pExecCmdFn,
                                       UINT uiMinArgs,
                                       UINT uiMaxArgs);

		BOOL RemoveDDEExecCmd(LPSTR pszTopic, LPSTR pszCmdName);

		void PostDDEAdvise(PDDEITEMINFO pItemInfo);
		PDDETOPICINFO FindTopicFromName(LPSTR lpszName);
		PDDEITEMINFO FindItemFromName(PDDETOPICINFO pTopic, LPSTR lpszItem);
		PDDETOPICINFO FindTopicFromHsz(HSZ hszName);
		PDDEITEMINFO FindItemFromHsz(PDDETOPICINFO pTopic, HSZ hszItem);
		PDDEEXECCMDFNINFO FindExecCmdFromName(PDDETOPICINFO pTopic, LPSTR pszCmd);
		LPSTR GetCFNameFromId(WORD wFmt, LPSTR lpBuf, int iSize);
		WORD GetCFIdFromName(LPSTR pszName);

	private:

		// local functions

		HDDEDATA CALLBACK SysReqTopics(UINT wFmt, HSZ hszTopic, HSZ hszItem);
		stdcall_proxy call_SysReqTopics_m;

		HDDEDATA CALLBACK SysReqItems(UINT wFmt, HSZ hszTopic, HSZ hszItem);
		stdcall_proxy call_SysReqItems_m;

		HDDEDATA CALLBACK SysReqFormats(UINT wFmt, HSZ hszTopic, HSZ hszItem);
		stdcall_proxy call_SysReqFormats_m;

		HDDEDATA CALLBACK SysReqProtocols(UINT wFmt, HSZ hszTopic, HSZ hszItem);
		stdcall_proxy call_SysReqProtocols_m;

		HDDEDATA CALLBACK SysReqResultInfo(UINT wFmt, HSZ hszTopic, HSZ hszItem);
		stdcall_proxy call_SysReqResultInfo_m;

		HDDEDATA CALLBACK TopicReqFormats( UINT wFmt, HSZ hszTopic, HSZ hszItem);
		stdcall_proxy call_TopicReqFormats_m;

		BOOL CALLBACK SysResultExecCmd(PDDETOPICINFO pTopic,
																		LPSTR pszResult, 
																		UINT uiResultSize, 
																		UINT uiNargs, 
																		LPSTR FAR *ppArgs);
		stdcall_proxy call_SysResultExecCmd_m;

		HDDEDATA CALLBACK StdDDECallback(WORD wType,
																		WORD wFmt,
																		HCONV hConv,
																		HSZ hsz1,
																		HSZ hsz2,
																		HDDEDATA hData,
																		DWORD dwData1,
																		DWORD dwData2);
		 

		HDDEDATA DoWildConnect(HSZ hszTopic);
		BOOL DoCallback(WORD wType,
										WORD wFmt,
										HCONV hConv,
										HSZ hsz1,
										HSZ hsz2,
										HDDEDATA hData,
										HDDEDATA *phReturnData);


		HDDEDATA MakeDataFromFormatList(LPWORD pFmt, WORD wFmt, HSZ hszItem);
		void AddFormatsToList(LPWORD pMain, int iMax, LPWORD pList);
		BOOL AddConversation(HCONV hConv, HSZ hszTopic);
		BOOL RemoveConversation(HCONV hConv, HSZ hszTopic);
		PDDECONVINFO FindConversation(HSZ hszTopic);
		LPSTR SkipWhiteSpace(LPSTR pszString);
		BOOL ProcessExecRequest(PDDETOPICINFO pTopic, HDDEDATA hData);
		BOOL ParseCmd(LPSTR FAR *ppszCmdLine,
												PDDETOPICINFO pTopic,
												LPSTR pszError,
												UINT uiErrorSize,
												PPOP pOpTable,
												UINT uiNops,
												LPSTR pArgBuf);
		LPSTR ScanForChar(char c, LPSTR *ppStr);
		PDDEEXECCMDFNINFO ScanForCommand(PDDEEXECCMDFNINFO pInfoTable, LPSTR *ppStr);
		LPSTR ScanForString(LPSTR *ppStr, LPSTR pszTerm, LPSTR *ppArgBuf);
		BOOL IsValidStringChar(char c);

		// return string value from handle
		CppUtils::String getStringFromHandle(HSZ hszTopic);
		
		

		private:

			// server info
			DDESERVERINFO serverInfo_m;

			// listener
			DDEServerWrapListener* pListener_m;

			// 
			stdcall_proxy call_StdDDECallback_m;

			

	}; // end of class

	
};

#endif