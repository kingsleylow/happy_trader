// ddeclient.cpp : 定義應用程式的類別行為。
//

#include "stdafx.h"
#include "mtmanager.h"
#include "mtmanagerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

   
// CddeclientApp  
   
BEGIN_MESSAGE_MAP(CddeclientApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CddeclientApp 建構

CddeclientApp::CddeclientApp()
{
	// TODO: 在此加入建構程式碼，
	// 將所有重要的初始設定加入 InitInstance 中
}


// 僅有的一個 CddeclientApp 物件

CddeclientApp theApp;

///////////////////////////////


CAppLineInfo::CAppLineInfo(CddeclientApp& clientApp):
clientApp_m(clientApp),
cnt_m(0),
itemCnt_m(-1)
{
	
	clientApp_m.paramExitOnError_m  = true;
	clientApp_m.paramEnableOutput_m  =true;
}

void CAppLineInfo::ParseParam(
			const char* pszParam, 
			BOOL bFlag, 
			BOOL bLast 
)
{
	cnt_m++;

	if (!bFlag) {
		if (cnt_m==1 || cnt_m ==2) {
			if (stricmp(pszParam, "closeonerror")==0) {
				clientApp_m.paramExitOnError_m  =true;
			}
			else if (stricmp(pszParam, "donotcloseonerror")==0) {
				clientApp_m.paramExitOnError_m = false;
			}
			else if (stricmp(pszParam, "outenable")==0) {
				clientApp_m.paramEnableOutput_m = true;
			}
			else if (stricmp(pszParam, "outdisable")==0) {
				clientApp_m.paramEnableOutput_m = false;
			}
			else
				THROW(CppUtils::OperationFailed, "exc_Invalid2FirstParamaters", "ctx_ParseParamaters", pszParam);
		}

		if (cnt_m==3) {
			// application
			clientApp_m.paramApp_m = pszParam;
			itemCnt_m = 1;
		}
		
		if (cnt_m > 3) {
			if (itemCnt_m > 0) {

				
				// if item 
				if (((itemCnt_m >> 1) << 1)==itemCnt_m) {
					// even - ITEM
					CppUtils::DDEItem& lastItem = clientApp_m.paramItemList_m[clientApp_m.paramItemList_m.size() - 1];
					lastItem.item_m = pszParam;

				}
				else {
					// odd  - TOPIC
					CppUtils::DDEItem newItem;
					newItem.topic_m = pszParam;
					clientApp_m.paramItemList_m.push_back(newItem);

				}

				itemCnt_m++;
			}
			else
				THROW(CppUtils::OperationFailed, "exc_InvalidTopicItemParamaters", "ctx_ParseParamaters", pszParam);
		}

		
	}


};

//////////////////////////////

BOOL CddeclientApp::InitInstance()
{
	
	InitCommonControls();
	CWinApp::InitInstance();
	AfxEnableControlContainer();
	
	//SetRegistryKey(_T("本機 AppWizard 所產生的應用程式"));

	

	CddeclientDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	
	}
	else if (nResponse == IDCANCEL)
	{
	
	}

	
	return FALSE;
}
