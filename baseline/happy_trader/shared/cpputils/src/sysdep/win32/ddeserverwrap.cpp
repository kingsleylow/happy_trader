#include "ddeserverwrap.hpp"
#include "../../log.hpp"
#include "../../except.hpp"

namespace CppUtils {
// ------------------------------



//
// Format lists
//
// need to be adjusted later
static WORD CF_CUSTOM_XLTABLE = -1;
static char const* SZCF_CUSTOM_XLTABLE = "XlTable";



static WORD MyFormatList[] = {
    CF_TEXT,
		CF_CUSTOM_XLTABLE,
    NULL};

static WORD SysFormatList[] = {
    CF_TEXT,
    NULL};

//
// Standard format name lookup table
//


CFTAGNAME CFNames[] = {
    CF_TEXT,        SZCF_TEXT,       
    CF_BITMAP,      SZCF_BITMAP,     
    CF_METAFILEPICT,SZCF_METAFILEPICT,
    CF_SYLK,        SZCF_SYLK,       
    CF_DIF,         SZCF_DIF,        
    CF_TIFF,        SZCF_TIFF,       
    CF_OEMTEXT,     SZCF_OEMTEXT,    
    CF_DIB,         SZCF_DIB,        
    CF_PALETTE,     SZCF_PALETTE,    
    CF_PENDATA,     SZCF_PENDATA,    
    CF_RIFF,        SZCF_RIFF,       
    CF_WAVE,        SZCF_WAVE,       
    NULL,           NULL
    };


#define MAXFORMATS      128     // max no of CF formats we will list
#define MAXOPS  ((UINT)128)     // max no of opcodes we can execute
#define MAXRESULTSIZE   256     // largest result string returned

//
// Define the pointer type we use in the exec cmd op table
//


	
	// ---------------------------------
	// empty ctors & dtors
	DDEServerWrap::DDEServerWrap():
	pListener_m(0)
	{
		// empty serverInfo_m
		memset(&serverInfo_m, '\0', sizeof(DDESERVERINFO));
	}

	DDEServerWrap::~DDEServerWrap()
	{
		// if any
		uninitializeServer();
	}

	void DDEServerWrap::registerListener(DDEServerWrapListener& listener)
	{
		LOCK_FOR_SCOPE(*this);
		pListener_m = &listener;
	}

	void DDEServerWrap::clearListener()
	{
		LOCK_FOR_SCOPE(*this);
		pListener_m = NULL;
	}

	
	void DDEServerWrap::initServer( String const& apps)
	{
		if (apps.size() ==0)
			THROW(CppUtils::OperationFailed, "exc_InvalidServiceName", "ctx_InitDDEServer", "");

		BOOL result = InitializeDDE((LPSTR)apps.c_str(), NULL, NULL, 0);
		if (!result)
			THROW(CppUtils::OperationFailed, "exc_CannotInitDDE", "ctx_InitDDEServer", "");

	

	}

	
	void DDEServerWrap::uninitializeServer()
	{
		UninitializeDDE();
	
	}

	
	void DDEServerWrap::addDDEItem(DDEItem const& item)
	{
		

		PDDEITEMINFO pItem = AddDDEItem(
			(LPSTR)item.topic_m.c_str(), 
			(LPSTR)item.item_m.c_str(),
      MyFormatList,
     (PDDEREQUESTFN)call_valueRequest_m.bind<DDEServerWrap, HDDEDATA, UINT , HSZ , HSZ >(this, &DDEServerWrap::valueRequest ),
		 (PDDEPOKEFN)call_valuePoke_m.bind<DDEServerWrap, BOOL, UINT , HSZ , HSZ , HDDEDATA >(this, &DDEServerWrap::valuePoke )
		);

		if (pItem==NULL)
			THROW(CppUtils::OperationFailed, "exc_CannotADDDDEItem", "ctx_addItem", "topic: " + item.topic_m + ", item: " + item.item_m);
		

	}


	void DDEServerWrap::removeDDEItem(DDEItem const& item)
	{
		BOOL result = RemoveDDEItem((LPSTR)item.topic_m.c_str(), (LPSTR)item.item_m.c_str());
		if (!result)
			THROW(CppUtils::OperationFailed, "exc_CannotRemoveDDEItem", "ctx_removeItem", "topic: " + item.topic_m + ", item: " + item.item_m);
	}


	// only topics
	void DDEServerWrap::addDDETopic(DDEItem const& item)
	{
		PDDETOPICINFO pTopic = AddDDETopic( 
			(LPSTR)item.topic_m.c_str(),
			NULL,
     (PDDEREQUESTFN)call_valueRequest_m.bind<DDEServerWrap, HDDEDATA, UINT , HSZ , HSZ >(this, &DDEServerWrap::valueRequest ),
		 (PDDEPOKEFN)call_valuePoke_m.bind<DDEServerWrap, BOOL, UINT , HSZ , HSZ , HDDEDATA >(this, &DDEServerWrap::valuePoke )
		);

		if (pTopic==NULL)
			THROW(CppUtils::OperationFailed, "exc_CannotADDDDETopic", "ctx_addItem", "topic: " + item.topic_m);
		
	}

	void DDEServerWrap::removeDDETopic(DDEItem const& item)
	{
		BOOL result = RemoveDDETopic((LPSTR)item.topic_m.c_str());

		if (!result)
			THROW(CppUtils::OperationFailed, "exc_CannotRemoveDDETopic", "ctx_removeItem", "topic: " + item.topic_m );


	}

	// --------------
	BOOL DDEServerWrap::valuePoke(UINT wFmt, HSZ hszTopic, HSZ hszItem, HDDEDATA hData)
	{
		DDEServerWrapListener* listener = getListener();

		if (listener==NULL)
			return FALSE;

		PTSTR psz; 
		DWORD cb;
		String stopic = getStringFromHandle(hszTopic);
		String sitem = getStringFromHandle(hszItem);
		String	svalue;
		
		// notify
		DDEItem item(stopic, sitem);
		


		//
		if (hData) {
				if (wFmt == CF_TEXT) {
					#define MAXCCH2  1024

					cb = DdeGetData(hData, NULL, 0, 0);
					if (!hData || !cb)	return FALSE;

					if (cb > MAXCCH2) {                // possibly HUGE object
						psz = (PTSTR)malloc(MAXCCH2 * sizeof(TCHAR));
						DdeGetData(hData, (PBYTE)psz, (MAXCCH2 - 46) * sizeof(TCHAR), 0L);
						wsprintf(&psz[MAXCCH2 - 46], TEXT("<---Size=%ld"), cb);
					}
					else {
						psz = (PTSTR)malloc(cb * sizeof(TCHAR));
						DdeGetData(hData, (LPBYTE)psz, cb, 0L);
					}

					svalue = psz;
					listener->onDDEDataPoke(item, svalue);
					free(psz);
				}
				else if (wFmt==CF_CUSTOM_XLTABLE) 
				{
					DWORD pcbDataSize = 0;
					LPBYTE p = DdeAccessData(hData, &pcbDataSize);
					if (!p) 
						return FALSE;
					// pass for further processing
					
					listener->onDDEXLTablePoke(item, (CppUtils::Byte const*)p, (size_t)pcbDataSize );

					DdeUnaccessData(hData);

				}else
					// format not translated
					return FALSE;


							
				
				// uninit
				DdeFreeDataHandle(hData);
		}

		

		return TRUE;
	}

	HDDEDATA DDEServerWrap::valueRequest(UINT wFmt, HSZ hszTopic, HSZ hszItem)
	{
			
		DDEServerWrapListener* listener = getListener();

		if (listener==NULL)
			return NULL;

		
		String stopic = getStringFromHandle(hszTopic);
		String sitem = getStringFromHandle(hszItem);
		String	svalue;

		
		// return new value to svalue
		DDEItem item(stopic, sitem);
		listener->onDDEDataRequested(item, svalue);

		// 

		if (wFmt != CF_TEXT) 
			return NULL;

    return DdeCreateDataHandle(serverInfo_m.dwDDEInstance,
															(LPBYTE)svalue.c_str(),
															svalue.size()+1,
                              0,
                              hszItem,
                              CF_TEXT,
                              NULL);

    


	}

	
	

	// -------------------------------------------
  // HELPERS
	//
	// -------------------------------------------

	BOOL DDEServerWrap::InitializeDDE(
                   LPSTR lpszServiceName,
                   LPDWORD lpdwDDEInst,
                   PFNCALLBACK lpfnCustomCallback,
                   DWORD dwFilterFlags )
{
	  
		if (serverInfo_m.dwDDEInstance != 0)
			return FALSE;

    UINT uiResult;

    //
    // Make a proc instance for the standard callback
    //

    serverInfo_m.pfnStdCallback = 
			(PFNCALLBACK)call_StdDDECallback_m.bind<DDEServerWrap, HDDEDATA,	WORD, WORD, HCONV, HSZ, HSZ, HDDEDATA, DWORD, DWORD>(this, &DDEServerWrap::StdDDECallback );
			 

    //
    // Make sure the application hasn't requested any filter options 
    // which will prevent us from working correctly.
    //

    dwFilterFlags &= !( CBF_FAIL_CONNECTIONS 
                      | CBF_SKIP_CONNECT_CONFIRMS
                      | CBF_SKIP_DISCONNECTS);

    //
    // Initialize DDEML.  Note that DDEML doesn't make any callbacks
    // during initialization so we don't need to worry about the
    // custom callback yet.
    //

    uiResult = DdeInitialize(&serverInfo_m.dwDDEInstance,
                             serverInfo_m.pfnStdCallback,
                             dwFilterFlags,
                             0);

    if (uiResult != DMLERR_NO_ERROR) return FALSE;

    //
    // make a proc instance for the custom callback if there is one
    //

    if (lpfnCustomCallback) {

        serverInfo_m.pfnCustomCallback = lpfnCustomCallback;
    }

    //
    // Return the DDE instance id if it was requested
    //

    if (lpdwDDEInst) {

        *lpdwDDEInst = serverInfo_m.dwDDEInstance;
    }

    //
    // Copy the service name and create a DDE name handle for it
    //

		serverInfo_m.lpszServiceName = _strdup(lpszServiceName);
    serverInfo_m.hszServiceName = DdeCreateStringHandle(serverInfo_m.dwDDEInstance,
                                                      serverInfo_m.lpszServiceName,
                                                      CP_WINANSI);

    //
    // Add all the system topic items to the service tree
    //

    AddDDEItem(SZDDESYS_TOPIC,
               SZDDESYS_ITEM_TOPICS,
               SysFormatList,
							 (PDDEREQUESTFN)call_SysReqTopics_m.bind<DDEServerWrap, HDDEDATA, UINT , HSZ , HSZ >(this, &DDEServerWrap::SysReqTopics),
               NULL);

    AddDDEItem(SZDDESYS_TOPIC,
               SZDDESYS_ITEM_SYSITEMS,
               SysFormatList,
               (PDDEREQUESTFN)call_SysReqItems_m.bind<DDEServerWrap, HDDEDATA, UINT , HSZ , HSZ >(this, &DDEServerWrap::SysReqItems),
               NULL);

    AddDDEItem(SZDDESYS_TOPIC,
               SZDDE_ITEM_ITEMLIST,
               SysFormatList,
               (PDDEREQUESTFN)call_SysReqItems_m.bind<DDEServerWrap, HDDEDATA, UINT , HSZ , HSZ >(this, &DDEServerWrap::SysReqItems),
               NULL);

    AddDDEItem(SZDDESYS_TOPIC,
               SZDDESYS_ITEM_FORMATS,
               SysFormatList,
               (PDDEREQUESTFN)call_SysReqFormats_m.bind<DDEServerWrap, HDDEDATA, UINT , HSZ , HSZ >(this, &DDEServerWrap::SysReqFormats),
               NULL);

    AddDDEItem(SZDDESYS_TOPIC, 
               SZ_PROTOCOLS,
               SysFormatList,
               (PDDEREQUESTFN)call_SysReqProtocols_m.bind<DDEServerWrap, HDDEDATA, UINT , HSZ , HSZ >(this, &DDEServerWrap::SysReqProtocols),
               NULL);


    //
    // Register the name of our service
    //

    DdeNameService(serverInfo_m.dwDDEInstance, 
                   serverInfo_m.hszServiceName,
                   NULL,
                   DNS_REGISTER);


		// register format (hacky)
		CF_CUSTOM_XLTABLE = RegisterClipboardFormat(SZCF_CUSTOM_XLTABLE);
		if (CF_CUSTOM_XLTABLE==0)
			return FALSE;

		MyFormatList[1] = CF_CUSTOM_XLTABLE;


		
    return TRUE;
}

// ----------------------------------

void DDEServerWrap::UninitializeDDE()
{
		// return if not initialized
		if (serverInfo_m.dwDDEInstance==0)
			return;

    PDDETOPICINFO pTopic, pPrevTopic;
    PDDEITEMINFO pItem, pPrevItem;

    //
    // Unregister the service name
    //

    DdeNameService(serverInfo_m.dwDDEInstance, 
                   serverInfo_m.hszServiceName,
                   NULL,
                   DNS_UNREGISTER);

    //
    // free the name handle
    //

    DdeFreeStringHandle(serverInfo_m.dwDDEInstance, serverInfo_m.hszServiceName);

    //
    // Walk the server topic list, freeing all the other string handles
    //

    pTopic = serverInfo_m.pTopicList;
    while (pTopic) {

        DdeFreeStringHandle(serverInfo_m.dwDDEInstance, pTopic->hszTopicName);
        pTopic->hszTopicName = NULL;

        //
        // Free any item handles it owns
        //

        pItem = pTopic->pItemList;
        while (pItem) {

            DdeFreeStringHandle(serverInfo_m.dwDDEInstance, pItem->hszItemName);
            pItem->hszItemName = NULL;

						pPrevItem = pItem;
            pItem = pItem->pNext;

						// free other resources
						free(pPrevItem->pszItemName);
						free(pPrevItem);

        }

				pPrevTopic = pTopic;
        pTopic = pTopic->pNext;

				// free other resources
				free(pPrevTopic->pszTopicName);
				free(pPrevTopic);
    }

    //
    // Release DDEML
    //

		if (serverInfo_m.lpszServiceName)
			free(serverInfo_m.lpszServiceName);

    DdeUninitialize(serverInfo_m.dwDDEInstance);
    //serverInfo_m.dwDDEInstance = NULL;

    //
    // Free any proc instances we created
    //

		/*
    if (serverInfo_m.pfnCustomCallback) {
        FreeProcInstance((FARPROC)serverInfo_m.pfnCustomCallback);
        serverInfo_m.pfnCustomCallback = NULL;
    }

    FreeProcInstance((FARPROC)serverInfo_m.pfnStdCallback);
    serverInfo_m.pfnStdCallback = NULL;
		*/

		
		// zeroes the whole structure
		memset(&serverInfo_m, '\0', sizeof(DDESERVERINFO));

}

// -----------------------------------

PDDETOPICINFO DDEServerWrap::FindTopicFromName(LPSTR lpszName)
{
    PDDETOPICINFO pTopic;

    pTopic = serverInfo_m.pTopicList;
    while (pTopic) {

        if (lstrcmpi(pTopic->pszTopicName, lpszName) == 0) {
            break;
        }

        pTopic = pTopic->pNext;
    }

    return pTopic;
}

//-----------------------------------

PDDETOPICINFO DDEServerWrap::FindTopicFromHsz(HSZ hszName)
{
    PDDETOPICINFO pTopic;

    pTopic = serverInfo_m.pTopicList;
    while (pTopic) {

        if (DdeCmpStringHandles(pTopic->hszTopicName, hszName) == 0) {
            break;
        }

        pTopic = pTopic->pNext;
    }

    return pTopic;
}

//-----------------------------------

PDDEITEMINFO DDEServerWrap::FindItemFromName(PDDETOPICINFO pTopic, LPSTR lpszItem)
{
    PDDEITEMINFO pItem;

    pItem = pTopic->pItemList;
    while (pItem) {

        if (lstrcmpi(pItem->pszItemName, lpszItem) == 0) {
            break;
        }

        pItem = pItem->pNext;
    }

    return pItem;
}

//-----------------------------------

PDDEITEMINFO DDEServerWrap::FindItemFromHsz(PDDETOPICINFO pTopic, HSZ hszItem)
{
    PDDEITEMINFO pItem;

    pItem = pTopic->pItemList;
    while (pItem) {

        if (DdeCmpStringHandles(pItem->hszItemName, hszItem) == 0) {
            break;
        }

        pItem = pItem->pNext;
    }

    return pItem;
}

//-----------------------------------

PDDEEXECCMDFNINFO DDEServerWrap::FindExecCmdFromName(PDDETOPICINFO pTopic, LPSTR pszCmd)
{
    PDDEEXECCMDFNINFO pCmd;

    pCmd = pTopic->pCmdList;
    while (pCmd) {

        if (lstrcmpi(pCmd->pszCmdName, pszCmd) == 0) {
            break;
        }

        pCmd = pCmd->pNext;
    }

    return pCmd;
}


//-----------------------------------

PDDETOPICINFO DDEServerWrap::AddDDETopic(LPSTR lpszTopic,
                          PDDEEXECFN pfnExec,
                          PDDEREQUESTFN pfnRequest,
                          PDDEPOKEFN pfnPoke
													)   
{
    PDDETOPICINFO pTopic;

    //
    // See if we already have this topic
    //

    pTopic = FindTopicFromName(lpszTopic);

    if (pTopic) {

        //
        // We already have this one so just update its info
        //

        pTopic->pfnExec = pfnExec;
        pTopic->pfnRequest = pfnRequest;
        pTopic->pfnPoke = pfnPoke;

    } else {

        //
        // Create a new topic
        //

        pTopic = (PDDETOPICINFO)malloc(sizeof(DDETOPICINFO));
        if (!pTopic) return NULL;

        //
        // Fill out the info
        //

				pTopic->pszTopicName = _strdup(lpszTopic);
        pTopic->hszTopicName = DdeCreateStringHandle(serverInfo_m.dwDDEInstance,
                                                    pTopic->pszTopicName,
                                                    CP_WINANSI);
        pTopic->pItemList = NULL;
        pTopic->pfnExec = pfnExec;
        pTopic->pfnRequest = pfnRequest;
        pTopic->pfnPoke = pfnPoke;
        pTopic->pCmdList = NULL;

        //
        // Add it to the list
        //

        pTopic->pNext = serverInfo_m.pTopicList;
        serverInfo_m.pTopicList = pTopic;

        //
        // Add handlers for its item list and formats.
        //

        AddDDEItem(lpszTopic,
                   SZDDE_ITEM_ITEMLIST,
                   SysFormatList,
                   (PDDEREQUESTFN)call_SysReqItems_m.bind<DDEServerWrap, HDDEDATA, UINT , HSZ , HSZ >(this, &DDEServerWrap::SysReqItems),
                   NULL);

        AddDDEItem(lpszTopic,
                   SZDDESYS_ITEM_FORMATS,
                   SysFormatList,
                   (PDDEREQUESTFN)call_TopicReqFormats_m.bind<DDEServerWrap, HDDEDATA, UINT , HSZ , HSZ >(this, &DDEServerWrap::TopicReqFormats),
                   NULL);

    }

    return pTopic;
}


//-----------------------------------

BOOL DDEServerWrap::RemoveDDETopic(LPSTR lpszTopic)
{
    PDDETOPICINFO pTopic, pPrevTopic;
    PDDECONVINFO pCI;

    //
    // See if we have this topic by walking the list
    //

    pPrevTopic = NULL;
    pTopic = serverInfo_m.pTopicList;
    while (pTopic) {

        if (lstrcmpi(pTopic->pszTopicName, lpszTopic) == 0) {

            //
            // Found it. Disconnect any active conversations on this topic
            //

            while (pCI = FindConversation(pTopic->hszTopicName)) {

                //
                // Tell DDEML to disconnect it
                //

                DdeDisconnect(pCI->hConv);

                //
                // We don't get notified until later that it's gone
                // so remove it from the list now so we won't keep
                // finding it in this loop.
                // When DDEML sends the disconnect notification later
                // it won't be there to remove but that doesn't matter.
                //

                RemoveConversation(pCI->hConv, pCI->hszTopicName);

            }

            //
            // Remove all the execute command handlers in the topic
            //

            while(pTopic->pCmdList) {

                if (!RemoveDDEExecCmd(lpszTopic,
                                      (LPSTR)pTopic->pCmdList->pszCmdName)) {
                    return FALSE; // some error
                }
            }

            //
            // Free all the items in the topic
            //
        
            while(pTopic->pItemList) {
        
                if (!RemoveDDEItem(lpszTopic, 
                                   (LPSTR)pTopic->pItemList->pszItemName)) {
                    return FALSE; // some error
                }    
            }
        
            //
            // Unlink it from the list.
            //

            if (pPrevTopic) {

                pPrevTopic->pNext = pTopic->pNext;

            } else {

                serverInfo_m.pTopicList = pTopic->pNext;

            }

            //
            // Release its string handle
            //
        
            DdeFreeStringHandle(serverInfo_m.dwDDEInstance,
                                pTopic->hszTopicName);
        
            //
            // Free the memory associated with it
            //
        
						free(pTopic->pszTopicName);
            free(pTopic);
        
            return TRUE;

        }

        pTopic = pTopic->pNext;
    }

    //
    // We don't have this topic
    //

    return FALSE;
}

//-----------------------------------

PDDEITEMINFO DDEServerWrap::AddDDEItem(LPSTR lpszTopic, 
                        LPSTR lpszItem, 
                        LPWORD lpFormatList,
                        PDDEREQUESTFN lpReqFn, 
                        PDDEPOKEFN lpPokeFn)
{
    PDDEITEMINFO pItem = NULL;
    PDDETOPICINFO pTopic;

    //
    // See if we have this topic already
    //

    pTopic = FindTopicFromName(lpszTopic);

    if (!pTopic) {

        //
        // We need to add this as a new topic
        //

        pTopic = AddDDETopic(lpszTopic,
                             NULL,
                             NULL,
                             NULL);
    }

    if (!pTopic) return NULL;  // failed

    //
    // See if we already have this item
    //

    pItem = FindItemFromName(pTopic, lpszItem);

    if (pItem) {

        //
        // Just update the info in it
        //

        pItem->pfnRequest = lpReqFn;
        pItem->pfnPoke = lpPokeFn;
        pItem->pFormatList = lpFormatList;

    } else {

        //
        // Create a new item
        //
				

        pItem = (PDDEITEMINFO)malloc(sizeof(DDEITEMINFO));
        if (!pItem) return NULL;

        //
        // Fill out the info
        //

        pItem->pszItemName = _strdup( lpszItem );
        pItem->hszItemName = DdeCreateStringHandle(serverInfo_m.dwDDEInstance,
                                                   pItem->pszItemName,
                                                   CP_WINANSI);
        pItem->pTopic = pTopic;
        pItem->pfnRequest = lpReqFn;
        pItem->pfnPoke = lpPokeFn;
        pItem->pFormatList = lpFormatList;

        //
        // Add it to the existing item list for this topic
        //

        pItem->pNext = pTopic->pItemList;
        pTopic->pItemList = pItem;

    }

    return pItem;
}

//-----------------------------------

BOOL DDEServerWrap::RemoveDDEItem(LPSTR lpszTopic, LPSTR lpszItem)
{
    PDDETOPICINFO pTopic;
    PDDEITEMINFO pItem, pPrevItem;

    //
    // See if we have this topic
    //

    pTopic = FindTopicFromName(lpszTopic);

    if (!pTopic) {
        return FALSE;
    }

    //
    // Walk the topic item list looking for this item.
    //

    pPrevItem = NULL;
    pItem = pTopic->pItemList;
    while (pItem) {

        if (lstrcmpi(pItem->pszItemName, lpszItem) == 0) {

            //
            // Found it.  Unlink it from the list.
            //

            if (pPrevItem) {

                pPrevItem->pNext = pItem->pNext;

            } else {

                pTopic->pItemList = pItem->pNext;

            }

            //
            // Release its string handle
            //

            DdeFreeStringHandle(serverInfo_m.dwDDEInstance, pItem->hszItemName);

            //
            // Free the memory associated with it
            //

						free(pItem->pszItemName);
            free(pItem);

            return TRUE;
        }

        pPrevItem = pItem;
        pItem = pItem->pNext;
    }

    //
    // We don't have that one
    //

    return FALSE;
}

//-----------------------------------

PDDEEXECCMDFNINFO DDEServerWrap::AddDDEExecCmd(LPSTR pszTopic, 
                              LPSTR pszCmdName,
                              PDDEEXECCMDFN pExecCmdFn,
                              UINT uiMinArgs,
                              UINT uiMaxArgs)
{
    PDDEEXECCMDFNINFO pCmd = NULL;
    PDDEEXECCMDFNINFO pHead;
    PDDETOPICINFO pTopic;

    //
    // See if we have this topic already
    //

    pTopic = FindTopicFromName(pszTopic);

    if (!pTopic) {

        //
        // We need to add this as a new topic
        //

        pTopic = AddDDETopic(pszTopic,
                             NULL,
                             NULL,
                             NULL);
    }

    if (!pTopic) return NULL;  // failed

    //
    // See if we already have this command
    //

    pCmd = FindExecCmdFromName(pTopic, pszCmdName);

    if (pCmd) {

        //
        // Just update the info in it
        //

        pCmd->pFn = pExecCmdFn;
        pCmd->uiMinArgs = uiMinArgs;
        pCmd->uiMaxArgs = uiMaxArgs;

    } else {

        //
        // Create a new item
        //

        pCmd = (PDDEEXECCMDFNINFO) malloc(sizeof(DDEEXECCMDFNINFO));
        if (!pCmd) return NULL;

        //
        // Fill out the info
        //

				pCmd->pszCmdName = _strdup( pszCmdName );
        pCmd->pTopic = pTopic;
        pCmd->pFn = pExecCmdFn;
        pCmd->uiMinArgs = uiMinArgs;
        pCmd->uiMaxArgs = uiMaxArgs;

        //
        // Add it to the existing cmd list for this topic
        //

        pHead = pTopic->pCmdList;
        pCmd->pNext = pTopic->pCmdList;
        pTopic->pCmdList = pCmd;

        //
        // If this was the first command added to the list,
        // add the 'Result' command too.  This supports the
        // Execute Control 1 protocol.
        //

        AddDDEExecCmd(pszTopic,
                      SZ_RESULT,
											(PDDEEXECCMDFN)call_SysResultExecCmd_m.bind<DDEServerWrap, BOOL, PDDETOPICINFO, LPSTR, UINT, UINT, LPSTR FAR*>(this, &DDEServerWrap::SysResultExecCmd ),
                      1, 1);

    }

    return pCmd;
}

//-----------------------------------

BOOL DDEServerWrap::RemoveDDEExecCmd(LPSTR pszTopic, LPSTR pszCmdName)
{
    PDDETOPICINFO pTopic;
    PDDEEXECCMDFNINFO pCmd, pPrevCmd;

    //
    // See if we have this topic
    //

    pTopic = FindTopicFromName(pszTopic);

    if (!pTopic) {
        return FALSE;
    }

    //
    // Walk the topic item list looking for this cmd.
    //

    pPrevCmd = NULL;
    pCmd = pTopic->pCmdList;
    while (pCmd) {

        if (lstrcmpi(pCmd->pszCmdName, pszCmdName) == 0) {

            //
            // Found it.  Unlink it from the list.
            //

            if (pPrevCmd) {

                pPrevCmd->pNext = pCmd->pNext;

            } else {

                pTopic->pCmdList = pCmd->pNext;

            }

            //
            // Free the memory associated with it
            //

						free(pCmd->pszCmdName);
            free(pCmd);

            return TRUE;
        }

        pPrevCmd = pCmd;
        pCmd = pCmd->pNext;
    }

    //
    // We don't have that one
    //

    return FALSE;
}

//-----------------------------------

LPSTR DDEServerWrap::GetCFNameFromId(WORD wFmt, LPSTR lpBuf, int iSize)
{
    PCFTAGNAME pCTN;

    //
    // Try for a standard one first
    //

    pCTN = CFNames;
    while (pCTN->wFmt) {
        if (pCTN->wFmt == wFmt) {
						strncpy(lpBuf, pCTN->pszName, iSize);
            return lpBuf;
        }
        pCTN++;
    }

    //
    // See if it's a registered one
    //

    if (GetClipboardFormatName(wFmt, lpBuf, iSize) == 0) {

        //
        // Nope.  It's unknown
        //

        *lpBuf = '\0';
    }

    return lpBuf;
}

//-----------------------------------

WORD DDEServerWrap::GetCFIdFromName(LPSTR pszName)
{
    PCFTAGNAME pCTN;

    //
    // Try for a standard one first
    //

    pCTN = CFNames;
    while (pCTN->wFmt) {
        if (lstrcmpi(pCTN->pszName, pszName) == 0) {
            return pCTN->wFmt;
        }
        pCTN++;
    }

    //
    // Register it
    //

    return RegisterClipboardFormat(pszName);
}

//-----------------------------------

void DDEServerWrap::PostDDEAdvise(PDDEITEMINFO pItemInfo)
{
    if (pItemInfo && pItemInfo->pTopic) {
			DdePostAdvise(serverInfo_m.dwDDEInstance,
                      pItemInfo->pTopic->hszTopicName,
                      pItemInfo->hszItemName);
    }
}


//-----------------------------------
// TODO call back
HDDEDATA DDEServerWrap::StdDDECallback(
																WORD wType,
                                WORD wFmt,
                                HCONV hConv,
                                HSZ hsz1,
                                HSZ hsz2,
                                HDDEDATA hData,
                                DWORD dwData1,
                                DWORD dwData2
	)
{
		//
		//
		// 
		
    HDDEDATA hDdeData = NULL;

    switch (wType) {
    case XTYP_CONNECT_CONFIRM:

        //
        // Add a new conversation to the list
        //

        AddConversation(hConv, hsz1);
        break;

    case XTYP_DISCONNECT:

        //
        // Remove a conversation from the list
        //

        RemoveConversation(hConv, hsz1);
        break;

    case XTYP_WILDCONNECT:

        //
        // We only support wild connects to either a NULL service
        // name or to the name of our own service.
        //

        if ((hsz2 == NULL)
					|| !DdeCmpStringHandles(hsz2, serverInfo_m.hszServiceName)) {

            return DoWildConnect(hsz1);

        }
        break;

        //
        // For all other messages we see if we want them here
        // and if not, they get passed on to the user callback
        // if one is defined.
        //

    case XTYP_ADVSTART:
    case XTYP_CONNECT:
    case XTYP_EXECUTE:
    case XTYP_REQUEST:
    case XTYP_ADVREQ:
    case XTYP_ADVDATA:
    case XTYP_POKE:

        //
        // Try and process them here first.
        //

        if (DoCallback(wType,
                       wFmt,
                       hConv,
                       hsz1,
                       hsz2,
                       hData,
                       &hDdeData)) {

            return hDdeData;
        }

        //
        // Fall Through to allow the custom callback a chance
        //

    default:

			if (serverInfo_m.pfnCustomCallback != NULL) {

            return(serverInfo_m.pfnCustomCallback(wType,  
                                                wFmt, 
                                                hConv, 
                                                hsz1, 
                                                hsz2, 
                                                hData,
                                                dwData1, 
                                                dwData2));
        }
    }

    return (HDDEDATA) NULL;
}

//-----------------------------------

BOOL DDEServerWrap::DoCallback(WORD wType,
                WORD wFmt,
                HCONV hConv,
                HSZ hszTopic,
                HSZ hszItem,
                HDDEDATA hData,
                HDDEDATA *phReturnData)
{
    PDDETOPICINFO pTopic;
    PDDEITEMINFO pItem;
    LPWORD pFormat;
    PDDEPOKEFN pfnPoke;
    CONVINFO ci;
    PDDEREQUESTFN pfnRequest;


    //
    // See if we know the topic
    //

    pTopic = FindTopicFromHsz(hszTopic);
    if (!pTopic) {

        return FALSE;

    }

    //
    // See if this is an execute request for the topic
    //

    if (wType == XTYP_EXECUTE) {

        //
        // See if the user supplied a generic function to handle this
        // or, if not, see if there is a command table.  If so run
        // the parser over the command string.
        //

        if (pTopic->pfnExec) {

            //
            // Call the exec function to process it
            //

            if ((*pTopic->pfnExec)( hszTopic, hData)) {

                *phReturnData = (HDDEDATA) DDE_FACK;
                return TRUE;

            }

        } else if (pTopic->pCmdList) {

            //
            // Try to parse and execute the request
            //

            if (ProcessExecRequest(pTopic, hData)) {

                *phReturnData = (HDDEDATA) DDE_FACK;
                return TRUE;

            }
        }

        //
        // Either no function or it didn't get handled by the function
        //

        *phReturnData = (HDDEDATA) DDE_FNOTPROCESSED;
        return TRUE;
    }

    //
    // See if this is a connect request. Accept it if it is.
    //

    if (wType == XTYP_CONNECT) {

        *phReturnData = (HDDEDATA) TRUE;
        return TRUE;
    }

    //
    // For any other transaction we need to be sure this is an
    // item we support and in some cases, that the format requested
    // is supported for that item.
    //

				
	
    pItem = FindItemFromHsz(pTopic, hszItem);
    if (!pItem) {

        //
        // Not an item we support
        //

				// try to self-registre it
				pItem = onUnknownIemDDEItem(pTopic, hszItem);
				if (pItem == NULL)
					return FALSE;
    }

    //
    // See if this is a supported format
    //

		

    pFormat = pItem->pFormatList;
    while (*pFormat) {

        if (*pFormat == wFmt) break;
        pFormat++;
    }

    if (! *pFormat) return FALSE; // not one we support

    //
    // Now just do whatever is required for each specific transaction
    //

    switch (wType) {
    case XTYP_ADVSTART:

        //
        // Start an advise request.  Topic/item and format are ok.
        //

        *phReturnData = (HDDEDATA) TRUE;
        break;

    case XTYP_POKE:
    case XTYP_ADVDATA:

        //
        // Some data for us. See if we have a poke function for 
        // this item or for this topic in general.
        //

        *phReturnData = (HDDEDATA) DDE_FNOTPROCESSED;
        pfnPoke = pItem->pfnPoke;
        if (!pfnPoke) pfnPoke = pTopic->pfnPoke;
        if (pfnPoke) {

            if ((*pfnPoke)(wFmt, hszTopic, hszItem, hData)) {

                //
                // Data at the server has changed.  See if we
                // did this ourself (from a poke) or if it's from
                // someone else.  If it came from elsewhere then post
                // an advise notice of the change.
                //

                ci.cb = sizeof(CONVINFO);
                if (DdeQueryConvInfo(hConv, (DWORD)QID_SYNC, &ci)) {

                    if (! (ci.wStatus & ST_ISSELF)) {

                        //
                        // It didn't come from us
                        //

                        DdePostAdvise(serverInfo_m.dwDDEInstance,
                                      hszTopic,
                                      hszItem);
                    }
                }

                *phReturnData = (HDDEDATA) DDE_FACK; // say we took it

            }
        }
        break;

    case XTYP_ADVREQ:
    case XTYP_REQUEST:

        //
        // Attempt to start an advise or get the data on a topic/item
        // See if we have a request function for this item or
        // a generic one for the topic
        //

        pfnRequest = pItem->pfnRequest;
        if (!pfnRequest) pfnRequest = pTopic->pfnRequest;
        if (pfnRequest) {

            *phReturnData = (*pfnRequest)( wFmt, hszTopic, hszItem);

        } else {

            *phReturnData = (HDDEDATA) NULL;

        }
        break;

    default:
        break;
    }

    //
    // Say we processed the transaction in some way
    //

    return TRUE;

}

// ----------------------------------

HDDEDATA DDEServerWrap::SysReqTopics(UINT wFmt, HSZ hszTopic, HSZ hszItem)
{
    HDDEDATA hData;
    PDDETOPICINFO pTopic; 
    int cb, cbOffset;

    //
    // Create an empty data object to fill
    //

		hData = DdeCreateDataHandle(serverInfo_m.dwDDEInstance,
                                NULL,
                                0,
                                0,
                                hszItem,
                                wFmt,
                                0);
    pTopic = serverInfo_m.pTopicList;
    cbOffset = 0;
    while (pTopic) {

        //
        // put in a tab delimiter unless this is the first item
        //

        if (cbOffset != 0) {
            DdeAddData(hData, (LPBYTE)SZ_TAB, lstrlen(SZ_TAB), cbOffset);
            cbOffset += lstrlen(SZ_TAB);
        }

        //
        // Copy the string name of the topic
        //

        cb = lstrlen(pTopic->pszTopicName);
        DdeAddData(hData, (LPBYTE)pTopic->pszTopicName, cb, cbOffset);
        cbOffset += cb;

        pTopic = pTopic->pNext;

    }

    //
    // Put a NULL on the end
    //

    DdeAddData(hData, (LPBYTE)"", 1, cbOffset);

    return hData;
}

// ----------------------------------

HDDEDATA DDEServerWrap::SysReqItems(UINT wFmt, HSZ hszTopic, HSZ hszItem)
{
    HDDEDATA hData;
    PDDETOPICINFO pTopic;
    PDDEITEMINFO pItem;
    int cb, cbOffset;

    //
    // Find the system topic
    //

    pTopic = FindTopicFromHsz(hszTopic);
    if (!pTopic) return NULL;

    //
    // Create an empty data object to fill
    //

		hData = DdeCreateDataHandle(serverInfo_m.dwDDEInstance,
                                NULL,
                                0,
                                0,
                                hszItem,
                                wFmt,
                                0);

    //
    // Walk the item list
    //

    cbOffset = 0;
    pItem = pTopic->pItemList;
    while (pItem) {

        //
        // put in a tab delimiter unless this is the first item
        //

        if (cbOffset != 0) {
            DdeAddData(hData, (LPBYTE)SZ_TAB, lstrlen(SZ_TAB), cbOffset);
            cbOffset += lstrlen(SZ_TAB);
        }

        //
        // Copy the string name of the item
        //

        cb = lstrlen(pItem->pszItemName);
        DdeAddData(hData, (LPBYTE)pItem->pszItemName, cb, cbOffset);
        cbOffset += cb;

        pItem = pItem->pNext;

    }

    //
    // Put a NULL on the end
    //

    DdeAddData(hData, (LPBYTE)"", 1, cbOffset);

    return hData;
}

// ----------------------------------

HDDEDATA DDEServerWrap::SysReqFormats(UINT wFmt, HSZ hszTopic, HSZ hszItem)
{
    HDDEDATA hData;
    PDDETOPICINFO pTopic;
    PDDEITEMINFO pItem;
    WORD wFormats[MAXFORMATS];

    wFormats[0] = NULL; // start with an empty list

    //
    // Create an empty data object to fill
    //

		hData = DdeCreateDataHandle(serverInfo_m.dwDDEInstance,
                                NULL,
                                0,
                                0,
                                hszItem,
                                wFmt,
                                0);

    //
    // Walk the topic list
    //

    pTopic = serverInfo_m.pTopicList;
    while (pTopic) {

        //
        // Walk the item list for this topic
        //
    
        pItem = pTopic->pItemList;
        while (pItem) {
    
            //
            // Walk the formats list for this item
            // adding each one to the main list if unique
            //
    
            AddFormatsToList(wFormats, MAXFORMATS, pItem->pFormatList);
    
            pItem = pItem->pNext;
        }

        pTopic = pTopic->pNext;
    }

    //
    // Walk the table and build the text form
    //

    return MakeDataFromFormatList(wFormats, wFmt, hszItem);

}


// ----------------------------------

HDDEDATA DDEServerWrap::TopicReqFormats(UINT wFmt, HSZ hszTopic, HSZ hszItem)
{
    HDDEDATA hData;
    PDDETOPICINFO pTopic;
    PDDEITEMINFO pItem;
    WORD wFormats[MAXFORMATS];

    //
    // Find the topic info
    //

    pTopic = FindTopicFromHsz(hszTopic);
    if (!pTopic) return NULL;

    wFormats[0] = NULL; // start with an empty list

    //
    // Create an empty data object to fill
    //

		hData = DdeCreateDataHandle(serverInfo_m.dwDDEInstance,
                                NULL,
                                0,
                                0,
                                hszItem,
                                wFmt,
                                0);

    //
    // Walk the item list for this topic
    //

    pItem = pTopic->pItemList;
    while (pItem) {

        //
        // Walk the formats list for this item
        // adding each one to the main list if unique
        //

        AddFormatsToList(wFormats, MAXFORMATS, pItem->pFormatList);

        pItem = pItem->pNext;
    }

    //
    // Walk the table and build the text form
    //

    return MakeDataFromFormatList(wFormats, wFmt, hszItem);

}


//-----------------------------------
HDDEDATA DDEServerWrap::SysReqProtocols( UINT wFmt, HSZ hszTopic, HSZ hszItem)
{
    static char sz[] = SZ_EXECUTECONTROL1;

		return DdeCreateDataHandle(serverInfo_m.dwDDEInstance,
                               (LPBYTE)sz,
                               lstrlen(sz)+1,
                               0,
                               hszItem,
                               CF_TEXT,
                               0);
}

// ----------------------------------

HDDEDATA DDEServerWrap::DoWildConnect(HSZ hszTopic)
{
    PDDETOPICINFO pTopic;
    int iTopics = 0;
    HDDEDATA hData;
    PHSZPAIR pHszPair;

    //
    // See how many topics we will be returning
    //

    if (hszTopic == NULL) {

        //
        // Count all the topics we have
        //

				pTopic = serverInfo_m.pTopicList;
        while (pTopic) {
            iTopics++;
            pTopic = pTopic->pNext;
        }

    } else {

        //
        // See if we have this topic in our list
        //

        pTopic = serverInfo_m.pTopicList;
        while (pTopic) {
            if (DdeCmpStringHandles(pTopic->hszTopicName, hszTopic) == 0) {
                iTopics++;
                break;
            }
            pTopic = pTopic->pNext;
        }
    }

    //
    // If we have no match or no topics at all, just return
    // NULL now to refuse the connect
    //

    if (!iTopics) return (HDDEDATA) NULL;

    //
    // Allocate a chunk of DDE data big enough for all the HSZPAIRS
    // we'll be sending back plus space for a NULL entry on the end
    //

    hData = DdeCreateDataHandle(serverInfo_m.dwDDEInstance,
                                NULL,
                                (iTopics + 1) * sizeof(HSZPAIR),
                                0,
                                NULL,
                                0,
                                0);

    //
    // Check we actually got it.
    //

    if (!hData) return (HDDEDATA) NULL;

    pHszPair = (PHSZPAIR) DdeAccessData(hData, NULL);

    //
    // Copy the topic data
    //

    if (hszTopic == NULL) {

        //
        // Copy all the topics we have (includes the system topic)
        //

        pTopic = serverInfo_m.pTopicList;
        while (pTopic) {

            pHszPair->hszSvc = serverInfo_m.hszServiceName;
            pHszPair->hszTopic = pTopic->hszTopicName;

            pHszPair++;
            pTopic = pTopic->pNext;
        }

    } else {

        //
        // Just copy the one topic asked for
        //

        pHszPair->hszSvc = serverInfo_m.hszServiceName;
        pHszPair->hszTopic = hszTopic;

        pHszPair++;

    }

    //
    // Put the terminator on the end
    //

    pHszPair->hszSvc = NULL;
    pHszPair->hszTopic = NULL;

    //
    // Finished with the data block
    //

    DdeUnaccessData(hData);

    //
    // Return the block handle
    //

    return hData;
}

// ----------------------------------

HDDEDATA DDEServerWrap::MakeDataFromFormatList(LPWORD pFmt, WORD wFmt, HSZ hszItem)
{
    HDDEDATA hData;
    int cbOffset, cb;
    char buf[256];

    //
    // Create an empty data object to fill
    //

    hData = DdeCreateDataHandle(serverInfo_m.dwDDEInstance,
                                NULL,
                                0,
                                0,
                                hszItem,
                                wFmt,
                                0);

    //
    // Walk the format list
    //

    cbOffset = 0;
    while (*pFmt) {

        //
        // put in a tab delimiter unless this is the first item
        //

        if (cbOffset != 0) {
            DdeAddData(hData, (LPBYTE)SZ_TAB, lstrlen(SZ_TAB), cbOffset);
            cbOffset += lstrlen(SZ_TAB);
        }

        //
        // Copy the string name of the format
        //

        GetCFNameFromId(*pFmt, buf, sizeof(buf));
        cb = lstrlen(buf);
        DdeAddData(hData, (LPBYTE)buf, cb, cbOffset);
        cbOffset += cb;

        pFmt++;

    }

    //
    // Put a NULL on the end
    //

    DdeAddData(hData, (LPBYTE)"", 1, cbOffset);

    return hData;
}

// ----------------------------------

void DDEServerWrap::AddFormatsToList(LPWORD pMain, int iMax, LPWORD pList)
{
    LPWORD pFmt, pLast;
    int iCount;

    if (!pMain || !pList) return;

    //
    // Count what we have to start with
    //

    iCount = 0;
    pLast = pMain;
    while (*pLast) {
        pLast++;
        iCount++;
    }

    //
    // Walk the new list ensuring we don't add the item if there
    // isn't room or we have it already.
    //

    while ((iCount < iMax) && *pList) {

        //
        // See if we have this one
        //

        pFmt = pMain;
        while (*pFmt) {

            if (*pFmt == *pList) {

                //
                // Already got this one
                //

                goto next_fmt; // I know.  I hate this too.
            }

            pFmt++;
        }

        //
        // Put it on the end of the list
        //

        *pLast++ = *pList;
        iCount++;

next_fmt:

        pList++;
    }

    //
    // Stick a null on the end to terminate the list
    //

    *pLast = NULL;
}

// ----------------------------------

PDDECONVINFO DDEServerWrap::FindConversation(HSZ hszTopic)
{
    PDDECONVINFO pCI;

    //
    // Try to find the info in the list
    //

    pCI = serverInfo_m.pConvList;
    while (pCI) {

        if (DdeCmpStringHandles(pCI->hszTopicName, hszTopic) == 0) {

            return pCI;

        }
    
        pCI = pCI->pNext;
    }

    return NULL;
}

//-----------------------------------

BOOL DDEServerWrap::AddConversation(HCONV hConv, HSZ hszTopic)
{
    PDDECONVINFO pCI;

    //
    // Allocate some memory for the info and fill it in
    //

    pCI = (PDDECONVINFO)malloc( sizeof(DDECONVINFO));
    if (!pCI) {
        return FALSE;
    }

    pCI->hConv = hConv;
    pCI->hszTopicName = hszTopic;

    //
    // Add it into the list
    //

    pCI->pNext = serverInfo_m.pConvList;
    serverInfo_m.pConvList = pCI;

    return TRUE;
}

// ----------------------------------

BOOL DDEServerWrap::RemoveConversation(HCONV hConv, HSZ hszTopic)
{
    PDDECONVINFO pCI, pPrevCI;

    //
    // Try to find the info in the list
    //

    pCI = serverInfo_m.pConvList;
    pPrevCI = NULL;
    while (pCI) {

        if ((pCI->hConv == hConv)
        &&  (DdeCmpStringHandles(pCI->hszTopicName,hszTopic) == 0)) {

            //
            // Found it. Unlink it from the list
            //

            if (pPrevCI) {

                pPrevCI->pNext = pCI->pNext;

            } else {

                serverInfo_m.pConvList = pCI->pNext;

            }

            //
            // Free the memory
            //

            free(pCI);

            return TRUE;

        }

        pPrevCI = pCI;
        pCI = pCI->pNext;
    }

    //
    // Not in the list
    //

    return FALSE;
}

// ----------------------------------

BOOL DDEServerWrap::ProcessExecRequest(PDDETOPICINFO pTopic, HDDEDATA hData)
{
    LPSTR pData;
    BOOL bResult = FALSE;
    POP OpTable[MAXOPS];
    PPOP ppOp, ppArg;
    UINT uiNargs;
    LPSTR pArgBuf = NULL;
    char szResult[MAXRESULTSIZE];
    PDDECONVINFO pCI;

    if (!hData) return FALSE;
    pData = (LPSTR) DdeAccessData(hData, NULL);
    if (!pData) return FALSE;

    //
    // Allocate some memory for the string argument buffer
    // Allocate a lot more than we might need so we can avoid
    // doing any space tests.
    //

    pArgBuf = (LPSTR)malloc(2* strlen(pData));
    if (!pArgBuf) {
        goto PER_exit;
    }

    //
    // Get a pointer to the current conversation
    //

    pCI = FindConversation(pTopic->hszTopicName);

    //
    // Parse and execute each command in turn.
    // If an error occurs, set the error return string
    // and return FALSE.
    //

    while (pData && *pData) {

        //
        // Parse a single command
        //

        szResult[0] = '\0';
        bResult = ParseCmd(&pData,
                           pTopic,
                           szResult,
                           sizeof(szResult),
                           OpTable,
                           MAXOPS,
                           pArgBuf);

        if (!bResult) {

            //
            // See if the current conversation has a results
            // item to pass the error string back in
            //

            if (pCI && pCI->pResultItem) {

                pCI->pResultItem->hData = 
                    DdeCreateDataHandle(serverInfo_m.dwDDEInstance,
                                        (LPBYTE)szResult,
                                        lstrlen(szResult)+1,
                                        0,
                                        pCI->pResultItem->hszItemName,
                                        CF_TEXT,
                                        0);
    
            }

            goto PER_exit;
        }

        //
        // Execute the op list
        //

        ppOp = OpTable;

        while (*ppOp) {

            //
            // Count the number of args
            //

            uiNargs = 0;
            ppArg = ppOp+1;
            while (*ppArg) {
                uiNargs++;
                ppArg++;
            }

            //
            // Call the function, passing the address of the first arg
            //

            ppArg = ppOp+1;
            szResult[0] = '\0';
            bResult = (*((PDDEEXECCMDFN)*ppOp))(pTopic,
                                                szResult,
                                                sizeof(szResult),
                                                uiNargs,
                                                (LPSTR FAR *)ppArg);
            //
            // See if the current conversation has a results
            // item to pass the result string back in
            //

            if (pCI && pCI->pResultItem) {

                pCI->pResultItem->hData = 
                    DdeCreateDataHandle(serverInfo_m.dwDDEInstance,
                                        (LPBYTE)szResult,
                                        lstrlen(szResult)+1,
                                        0,
                                        pCI->pResultItem->hszItemName,
                                        CF_TEXT,
                                        0);
    
            }
    
            if (!bResult) {
                goto PER_exit;
            }
    
            //
            // move on to the next function
            //
    
            while (*ppOp) ppOp++;
            ppOp++;

        }

    }

    //
    // if we get this far we're done
    //

    bResult = TRUE;

PER_exit:

    DdeUnaccessData(hData);
    if (pArgBuf) {
        free(pArgBuf);
    }

    return bResult;
}


// ----------------------------------

BOOL DDEServerWrap::ParseCmd(LPSTR FAR *ppszCmdLine,
                     PDDETOPICINFO pTopic,
                     LPSTR pszError,
                     UINT uiErrorSize,
                     PPOP pOpTable,
                     UINT uiNops,
                     LPSTR pArgBuf)
{
    LPSTR pCmd, pArg;
    PPOP ppOp = pOpTable;
    PDDEEXECCMDFNINFO pExecFnInfo;
    UINT uiNargs;
    char cTerm;

    *ppOp = NULL;
    pCmd = SkipWhiteSpace(*ppszCmdLine);

    //
    // Scan for a command leadin
    //

    if (!ScanForChar('[', &pCmd)) {
        strncpy(pszError,
                  "Missing '['",
                  uiErrorSize-1);
        return FALSE;
    }

    //
    // Scan for a valid command
    //

    pExecFnInfo = ScanForCommand(pTopic->pCmdList, &pCmd);
    if (!pExecFnInfo) {
        strncpy(pszError,
                  "Invalid Command",
                  uiErrorSize-1);
        return FALSE;
    }

    //
    // Add the function pointer to the opcode list
    //

    *ppOp++ = pExecFnInfo->pFn;

    //
    // Scan for any arguments
    //

    uiNargs = 0;
    if (ScanForChar('(', &pCmd)) {

        //
        // Copy each argument to the op list
        //

        do {

            pArg = ScanForString(&pCmd, &cTerm, &pArgBuf);

            if (pArg) {

                *ppOp++ = pArg;
                uiNargs++;

            }

        } while (cTerm == ',');

        //
        // Confirm we have a terminating ) char
        //

        if ((cTerm != ')')
        && (!ScanForChar(')', &pCmd))) {
            strncpy(pszError,
                      "Missing ')'",
                      uiErrorSize-1);
            return FALSE;
        }

    }   
       
    //
    // Test that we have a terminating ] char
    //

    if (!ScanForChar(']', &pCmd)) {
        strncpy(pszError,
                  "Missing ']'",
                  uiErrorSize-1);
        return FALSE;
    }

    //
    // Test the number of args is correct
    //

    if (uiNargs < pExecFnInfo->uiMinArgs) {
        strncpy(pszError,
                  "Too few arguments",
                  uiErrorSize-1);
        return FALSE;
    }

    if (uiNargs > pExecFnInfo->uiMaxArgs) {
        strncpy(pszError,
                  "Too many arguments",
                  uiErrorSize-1);
        return FALSE;
    }

    //
    // Terminate this op list with a NULL
    //

    *ppOp++ = NULL;

    pCmd = SkipWhiteSpace(pCmd);

    //
    // Put a final NULL on the op list
    //

    *ppOp = NULL;
 
    //
    // Update the buffer pointer
    //

    *ppszCmdLine = pCmd;

    return TRUE;
}


// ----------------------------------

BOOL DDEServerWrap::SysResultExecCmd(	PDDETOPICINFO pTopic,
                                 LPSTR pszResult, 
                                 UINT uiResultSize, 
                                 UINT uiNargs, 
                                 LPSTR FAR *ppArgs)
{
    PDDECONVINFO pCI;

    //
    // Find the conversation info
    //

    pCI = FindConversation(pTopic->hszTopicName);
    if (!pCI) return FALSE; // internal error

    //
    // See if we already have a temporary result item and
    // if we do, get rid of it
    //

    if (pCI->pResultItem) {

        RemoveDDEItem((LPSTR)pTopic->pszTopicName,
                      (LPSTR)pCI->pResultItem->pszItemName);
    }

    //
    // Add a new temporary result item to the current conversation
    //

    pCI->pResultItem = AddDDEItem((LPSTR)pTopic->pszTopicName,
                                  ppArgs[0],
                                  SysFormatList,
																	(PDDEREQUESTFN)call_SysReqResultInfo_m.bind<DDEServerWrap, HDDEDATA, UINT , HSZ , HSZ >(this, &DDEServerWrap::SysReqResultInfo ),
                                  NULL);

    return TRUE;
}

// ----------------------------------

HDDEDATA DDEServerWrap::SysReqResultInfo(UINT wFmt, HSZ hszTopic, HSZ hszItem)
{
    HDDEDATA hData;
    PDDETOPICINFO pTopic;
    PDDEITEMINFO pItem;

    //
    // Find the item
    //

    pTopic = FindTopicFromHsz(hszTopic);
    pItem = FindItemFromHsz(pTopic, hszItem);

    //
    // See if it has any data to return
    //

    hData = pItem->hData;

    if (!hData) {

        //
        // Send back an empty string
        //

        hData = DdeCreateDataHandle(serverInfo_m.dwDDEInstance,
                                   (LPBYTE)"",
                                   1,
                                   0,
                                   hszItem,
                                   CF_TEXT,
                                   wFmt);
    }

    //
    // Delete the item
    //

    RemoveDDEItem((LPSTR)pTopic->pszTopicName,
                  (LPSTR)pItem->pszItemName);

    return hData;

}

// ----------------------------------

LPSTR DDEServerWrap::SkipWhiteSpace(LPSTR pszString)
{
    while (pszString && *pszString && isspace(*pszString)) {
        pszString++;
    }
    return pszString;
}

// ----------------------------------

LPSTR DDEServerWrap::ScanForChar(char c, LPSTR *ppStr)
{
    LPSTR p;

    p = SkipWhiteSpace(*ppStr);
    if (*p == c) {
        *ppStr = p+1;
        return p;
    }
    return NULL; // not found
}

// ----------------------------------

PDDEEXECCMDFNINFO DDEServerWrap::ScanForCommand(PDDEEXECCMDFNINFO pCmdInfo, 
                                        LPSTR *ppStr)
{
    LPSTR p, pStart;
    char cSave;


    p = pStart = SkipWhiteSpace(*ppStr);

    //
    // Check the first char is alpha
    //

    if (!isalpha(*p)) {
        return NULL;
    }

    //
    // Collect alpha-num chars until we get to a non-alpha.
    //

    while (isalnum(*p)) p++;

    //
    // Terminate the source temporarily with a null
    //

    cSave = *p;
    *p = '\0';

    //
    // Search for a command that matches the name we have
    //

    while (pCmdInfo) {

        if (_stricmp(pStart, pCmdInfo->pszCmdName) == 0) {

            //
            // Found it, so restore the delimter and
            // return the info pointer
            //

            *p = cSave;
            *ppStr = p;
            return pCmdInfo;
        }
        pCmdInfo = pCmdInfo->pNext;
    }

    //
    // Didn't find it, so restore delimiter and return
    //

    *p = cSave;
    return NULL; // not found
}

// ----------------------------------

LPSTR DDEServerWrap::ScanForString(LPSTR *ppStr, LPSTR pszTerm, LPSTR *ppArgBuf)
{
    LPSTR pIn, pStart, pOut;
    BOOL bInQuotes = FALSE;

    pIn = SkipWhiteSpace(*ppStr);
    pOut = pStart = *ppArgBuf;

    //
    // See if this string is enclosed in quotes
    //

    if (*pIn == '"') {
        bInQuotes = TRUE;
        pIn++;
    }

    do {

        //
        // Test for the end of the string
        //

        if (bInQuotes) {

            if ((*pIn == '"') 
            && (*(pIn+1) != '"')) {
                pIn++;  // skip over the quote
                break;
            }

        } else {

            if (!IsValidStringChar(*pIn)) {
                break;
            }

        }

        //
        // Test for an escape sequence
        //

        if ((*pIn == '"')
        && (*pIn == '"')) {
            pIn++; // skip the escaping first quote
        }

        if (*pIn == '\\') {
            pIn++; // skip the escaping backslash
        }

        //
        // Copy the char to the arg buffer
        //

        *pOut++ = *pIn++;

    } while(*pIn);

    *pOut++ = '\0';

    //
    // Set up the terminating char and update the scan pointer
    //

    *pszTerm = *pIn;
    if (*pIn) {
        *ppStr = pIn+1;
    } else {
        *ppStr = pIn;
    }

    //
    // Update the arg buffer to the next free bit
    //

    *ppArgBuf = pOut;

    return pStart;
}

// ----------------------------------

BOOL DDEServerWrap::IsValidStringChar(char c)
{
    //
    // if it's 0-9 or a-z or A-Z it's ok
    //

    if (isalnum(c)) return TRUE;

    //
    // Test for other valid chars
    //

    switch (c) {
    case '_':
    case '$':
    case '.':
        return TRUE;
        break;

    default:
        break;
    }

    return FALSE;
}


// ----------------------------------

CppUtils::String DDEServerWrap::getStringFromHandle(HSZ hszValue)
{
	
	// GET Topic Name
	DWORD cb = (DWORD)DdeQueryString(serverInfo_m.dwDDEInstance, hszValue, NULL, 0, 0) + 1;
	PTSTR psz = (PTSTR)malloc(cb * sizeof(TCHAR));
	DdeQueryString(serverInfo_m.dwDDEInstance, hszValue, psz, cb, 0);	
	String svalue = psz;
	free(psz);

	return svalue;

}

// ----------------------------------



PDDEITEMINFO DDEServerWrap::onUnknownIemDDEItem(PDDETOPICINFO pTopic, HSZ hszItem)
{
	CppUtils::String sitem = getStringFromHandle(hszItem);
	CppUtils::String stopic = (pTopic->pszTopicName ? pTopic->pszTopicName : "");

	CppUtils::DDEItem ddeitem(stopic, sitem);

	{
		LOCK_FOR_SCOPE(*this);
		if (pListener_m) {
			if (!pListener_m->onUnknownDDEItem(ddeitem)) {
				return NULL;
			}
		}
		else
			// do not register if no listener
			return NULL;
	}
	
	
	
	PDDEITEMINFO pItem = AddDDEItem(
					(LPSTR)stopic.c_str(),
					(LPSTR)sitem.c_str(),
					MyFormatList,
					(PDDEREQUESTFN)call_valueRequest_m.bind<DDEServerWrap, HDDEDATA, UINT , HSZ , HSZ >(this, &DDEServerWrap::valueRequest ),
					(PDDEPOKEFN)call_valuePoke_m.bind<DDEServerWrap, BOOL, UINT , HSZ , HSZ , HDDEDATA >(this, &DDEServerWrap::valuePoke )
	);

	return pItem;
}


}; // end of namespace