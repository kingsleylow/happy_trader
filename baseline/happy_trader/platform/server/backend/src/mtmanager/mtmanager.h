// ddeclient.h : PROJECT_NAME ���ε{�����D�n���Y��
//

#pragma once

#ifndef __AFXWIN_H__
	#error �b�� PCH �]�t���ɮ׫e���]�t 'stdafx.h'
#endif

#include "resource.h"		// �D�n�Ÿ�

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

// �мg
	public:
	virtual BOOL InitInstance();

// �{���X��@

	DECLARE_MESSAGE_MAP()

	//

	

};

extern CddeclientApp theApp;
