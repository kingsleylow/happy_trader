#include "ddewrap.hpp"
#include "../../log.hpp"
#include "../../except.hpp"

namespace CppUtils {

	
	


// ------------------------
	
	HDDEDATA DDEWrap::UdprocDdeCallback(
			UINT wType, 
			UINT wFmt, 
			HCONV hConv, 
			HSZ hsz1,
			HSZ hsz2, 
			HDDEDATA hData, 
			DWORD lData1, 
			DWORD lData2
	 )
	{
				
		PTSTR psz;
		DWORD cb;
		String stopic, sitem, svalue;
		
		switch (wType) {
			case XTYP_REGISTER:
			case XTYP_UNREGISTER:
			case XTYP_DISCONNECT:
			case XTYP_XACT_COMPLETE:
					break;

			case XTYP_ADVDATA:
			// GET Topic Name
			cb = (DWORD)DdeQueryString(idInst_m, hsz1, NULL, 0, 0) + 1;
			psz = (PTSTR)malloc(cb * sizeof(TCHAR));
			DdeQueryString(idInst_m, hsz1, psz, cb, 0);	// ex) psz = "PLC1"
			stopic = psz;
			free(psz);
			
			// GET Item Name
			cb = (DWORD)DdeQueryString(idInst_m, hsz2, NULL, 0, 0) + 1;
			psz = (PTSTR)malloc(cb * sizeof(TCHAR));
			DdeQueryString(idInst_m, hsz2, psz, cb, 0);	// ex) psz = "D0"
			sitem = psz;
			free(psz);

					if (!hData)
							hData = DdeClientTransaction(NULL, 0L, hConv, hsz2, wFmt, XTYP_REQUEST, 1000, NULL);
	        
			if (hData) {
				#define MAXCCH  1024

				cb = DdeGetData(hData, NULL, 0, 0);
				if (!hData || !cb)	break;

				if (cb > MAXCCH) {                // possibly HUGE object
					psz = (PTSTR)malloc(MAXCCH * sizeof(TCHAR));
					DdeGetData(hData, (PBYTE)psz, (MAXCCH - 46) * sizeof(TCHAR), 0L);
					wsprintf(&psz[MAXCCH - 46], TEXT("<---Size=%ld"), cb);
				}
				else {
					psz = (PTSTR)malloc(cb * sizeof(TCHAR));
					DdeGetData(hData, (LPBYTE)psz, cb, 0L);
				}

				svalue = psz;

				// notify
				DDEItem item(stopic, sitem);
				onDataArrived(item, svalue);
				//
				
				
				free(psz);
				DdeFreeDataHandle(hData);
			}
			return((HDDEDATA)DDE_FACK);
			break;

		}; // switch
			
		return 0;

	}



	void DDEWrap::initializeDDE( String const& apps, DDEItemList const& itemlist, int toutmsec )
	{
		if (idInst_m != NULL)
			return;

		
	
		CONVCONTEXT cf = { sizeof(CONVCONTEXT), 0, 0, 0, 0L, 0L,
			{
				sizeof(SECURITY_QUALITY_OF_SERVICE),
				SecurityImpersonation,
				SECURITY_STATIC_TRACKING,
				TRUE
			}
		};

		memcpy(&ccFilter_m, &cf, sizeof(CONVCONTEXT));

		// initializa 

		long ret;
		HSZ			hszApp, hszTopic;
    HCONV		hConv;

		// use special thunk to call static call back function
		// if not compatible use CallBackAdapter with some restrictions
		/*
		if ((ret=DdeInitialize(&idInst_m, 
			callProxy_m.bind<DDEWrap, HDDEDATA,	UINT, UINT, HCONV, HSZ, HSZ, HDDEDATA, DWORD, DWORD>(this, &DDEWrap::UdprocDdeCallback ),
			APPCMD_CLIENTONLY, 0L)) != DMLERR_NO_ERROR
		){
			THROW(CppUtils::OperationFailed, "exc_CannotInitializeDDE", "ctx_DDEInit", ret);
		}
		*/

		if ((ret=DdeInitialize(&idInst_m, 
			CallBackAdapter<INSTANCE_THIS, DDEWrap, HDDEDATA,	UINT, UINT, HCONV, HSZ, HSZ, HDDEDATA, DWORD, DWORD>::callBackFun,
			APPCMD_CLIENTONLY, 0L)) != DMLERR_NO_ERROR
		){
			THROW(CppUtils::OperationFailed, "exc_CannotInitializeDDE", "ctx_DDEInit", ret);
		}

		// pass to callback
		//callBack_m.PidInst_m = &idInst_m;

		hszApp = DdeCreateStringHandle(idInst_m, apps.c_str(), 0);
		for(int i = 0; i < itemlist.size(); i++) {
			DDEItem item_i = itemlist[ i ];
			hszTopic = DdeCreateStringHandle(idInst_m, item_i.topic_m.c_str(), 0);

			if(!(hConv = DdeConnect(idInst_m, hszApp, hszTopic, &ccFilter_m))){
				
				DdeFreeStringHandle(idInst_m, hszApp);
				DdeFreeStringHandle(idInst_m, hszTopic);
				
				// TODO
				// release previous connections
				uninitializeDDE();
				THROW(CppUtils::OperationFailed, "exc_CannotConnectToDDEServer", "ctx_DDEInit", apps + " - " + item_i.topic_m);
				
			}

			//
			//thisMap_m[hConv] = this;

			DdeFreeStringHandle(idInst_m, hszApp);
			DdeFreeStringHandle(idInst_m, hszTopic);

			XACT* pxact = NULL;
			pxact = (XACT *)malloc(sizeof(XACT));
			memset(pxact, 0, sizeof(pxact));
			pxact->hConv = hConv;
			pxact->fsOptions = 0;
			pxact->ulTimeout = toutmsec; // timeout
			pxact->item_m = _strdup(item_i.item_m.c_str());
			
			pxact_m.push_back(pxact);

		}
	}

	DDEWrap::DDEWrap():
	idInst_m(0),
	pxact_m(0),
	callProxy_m(0)
	{
		callProxy_m = new CppUtils::CallBackAdapter<INSTANCE_THIS, DDEWrap, HDDEDATA,	UINT, UINT, HCONV, HSZ, HSZ, HDDEDATA, DWORD, DWORD>
			(this, &DDEWrap::UdprocDdeCallback);

	}

	DDEWrap::~DDEWrap()
	{
		uninitializeDDE();
		delete callProxy_m;
	}

	void DDEWrap::onDataArrived(DDEItem const& item, CppUtils::String const& value)
	{
		
	}

	void DDEWrap::uninitializeDDE()
	{
		for(int i = 0; i < pxact_m.size(); i++) {
			if (pxact_m[i]==NULL)
				continue;

			DdeDisconnect(pxact_m[i]->hConv);
			//thisMap_m.erase( pxact_m[i]->hConv );

			if(pxact_m[i]->ret){
				DdeFreeDataHandle((HDDEDATA)pxact_m[i]->ret);
				pxact_m[i]->ret = 0L;
			}
			
			if (pxact_m[i]->item_m)
				free( pxact_m[i]->item_m );
			free(pxact_m[i]);
			pxact_m[i] = NULL;

		}

		pxact_m.clear();
	
	};

	void DDEWrap::adviseItems()
	{
		for(int i = 0; i < pxact_m.size(); i++) {
			if (pxact_m[i]==NULL)
				continue;

			//
			XACT* pxact = pxact_m[i];
			//
			pxact->hszItem = DdeCreateStringHandle(idInst_m, pxact->item_m, 0);		// DDE Item Name
			pxact->hDdeData = 0;
			pxact->wFmt = CF_TEXT;
			pxact->wType = XTYP_ADVSTART;	// DDE Advice (dde automation link)
			if(!(pxact->ret = (DWORD) DdeClientTransaction((LPBYTE)pxact->hDdeData, 0xFFFFFFFF, pxact->hConv, 
					pxact->hszItem, pxact->wFmt, pxact->wType, pxact->ulTimeout, (LPDWORD)&pxact->Result))){
					
					THROW(CppUtils::OperationFailed, "exc_CannotCreateAutomaticLink", "ctx_adviseDDE", pxact->item_m);
			}
		  
		} // end loop

	}

	void DDEWrap::unadviseItems()
	{
		for(int i = 0; i < pxact_m.size(); i++) {
			if (pxact_m[i]==NULL)
				continue;

			pxact_m[i]->hszItem = DdeCreateStringHandle(idInst_m, pxact_m[i]->item_m, 0);		// DDE Item Name
			pxact_m[i]->hDdeData = 0;
			pxact_m[i]->wFmt = CF_TEXT;
			pxact_m[i]->wType = XTYP_ADVSTOP;	// DDE Advice (dde automation link)
			if(!(pxact_m[i]->ret = (DWORD) DdeClientTransaction((LPBYTE)pxact_m[i]->hDdeData, 0xFFFFFFFF, pxact_m[i]->hConv, 
					pxact_m[i]->hszItem, pxact_m[i]->wFmt, pxact_m[i]->wType, pxact_m[i]->ulTimeout, (LPDWORD)&pxact_m[i]->Result))){
				
						THROW(CppUtils::OperationFailed, "exc_CannotDestroyAutomaticLink", "ctx_unadviseDDE", (long)pxact_m[i]->ret);
			}
		};
	}

	// ---------------------------------

	
	
	

}; // end of namespace