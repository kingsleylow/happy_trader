// ddeclient.h : PROJECT_NAME 應用程式的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error 在對 PCH 包含此檔案前先包含 'stdafx.h'
#endif

#include "resource.h"		// 主要符號

class CddeclientApp;
class CAppLineInfo: public CCommandLineInfo 
{
	public:	
		
		CAppLineInfo(CddeclientApp& clientApp);

		virtual void ParseParam(
			const char* pszParam, 
			BOOL bFlag, 
			BOOL bLast 
		);

private:
	CddeclientApp& clientApp_m;

	int cnt_m;

	int itemCnt_m;

};

//////////////////////////////

class CddeclientApp : public CWinApp
{
public:
	bool paramExitOnError_m;
	bool paramEnableOutput_m;
	CppUtils::String paramApp_m;
	CppUtils::DDEItemList paramItemList_m;
public:
	CddeclientApp();

// 覆寫
	public:
	virtual BOOL InitInstance();

// 程式碼實作

	DECLARE_MESSAGE_MAP()

	//

	

};

extern CddeclientApp theApp;
