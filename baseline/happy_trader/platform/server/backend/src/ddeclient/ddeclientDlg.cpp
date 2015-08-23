// ddeclientDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "ddeclient.h"
#include "ddeclientDlg.h"
#include ".\ddeclientdlg.h"
#include "platform\server\backend\src\hlpstruct\hlpstruct.hpp"

//#include "platform/server/backend/src/quik_connector/connector_api.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif 
 

#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#include <AtlConv.h>
#include <atlsafe.h>
#include <ATLComTime.h>



// 對 App About 使用 CAboutDlg 對話方塊

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 對話方塊資料
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

// 程式碼實作
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	
END_MESSAGE_MAP()



void CddeclientDlg::connectToDdeServer()
{
	
	CAppLineInfo cmdinfo(theApp);
	theApp.ParseCommandLine(cmdinfo);
   
		
	initializeDDE(theApp.paramApp_m, theApp.paramItemList_m);
	adviseItems();
     
	  
	appEdit_m.SetWindowText(theApp.paramApp_m.c_str());

}

void CddeclientDlg::disconnectFromDdeServer()
{
	unadviseItems();
	uninitializeDDE();

	
}




// -------------
void CddeclientDlg::onDataArrived(CppUtils::DDEItem const& item, CppUtils::String const& value)
{
	writeOkOutput(item.topic_m, item.item_m, value);
}

void CddeclientDlg::writeOkOutput(CppUtils::String const& topic, CppUtils::String const& item, CppUtils::String const& value)
{
	CppUtils::String okString = "|<0><" + topic + "><"+ item + "><" +value + ">\r\n";

	// write to stream
	
	cout << okString << endl;
	cout.flush();
	
	// 
	if (!theApp.paramEnableOutput_m)
		return;

	appendTextToControl(okString);
}

void CddeclientDlg::writeErrorOutput(CppUtils::String const& errorMsg)
{
	CppUtils::String errorString = "|<1><" + errorMsg + ">\r\n";
	cout << errorString << endl; 
	cout.flush();

	if (!theApp.paramEnableOutput_m)
		return;

	
	appendTextToControl(errorString);

}

void CddeclientDlg::appendTextToControl(CppUtils::String const& value)
{
	int winln = outEdit_m.GetWindowTextLength();

	// set thsi limit
	if ((winln + value.size()) >= 30000) {
		outEdit_m.SetSel(0, winln);
	}
	else {
		outEdit_m.SetSel(winln, winln);
	}
	outEdit_m.ReplaceSel(value.c_str());
}

////////////////////////////////////////////////////////

CddeclientDlg::CddeclientDlg(CWnd* pParent /*=NULL*/)	:
		CDialog(CddeclientDlg::IDD, pParent)
		
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CddeclientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDDATA, outEdit_m);
	DDX_Control(pDX, IDC_EDITAPPL, appEdit_m);
}

BEGIN_MESSAGE_MAP(CddeclientDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CLOSE( )
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_MESSAGE( WM_QUERYENDSESSION, OnQueryEndSession )
END_MESSAGE_MAP()

// -------------------------------------------

afx_msg LRESULT CddeclientDlg::OnQueryEndSession(WPARAM wParam, LPARAM lParam)
{
	LOG( MSG_INFO, "DDECLIENT", "Logging off or shutting down, trying to reject...");
	return 0;
}

BOOL CddeclientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	
	

	// IDM_ABOUTBOX 必須在系統命令範圍之中。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	
	SetIcon(m_hIcon, TRUE);	
	SetIcon(m_hIcon, FALSE);

	
	// connect to DDE server
	try {
		connectToDdeServer();
	}
	catch(CppUtils::Exception &e) {
		writeErrorOutput(e.message() + " - " + e.context() + " - " + e.arg());
	}
	catch(...) {
		writeErrorOutput("Unknown errror on connectiong to DDE server");
	}

	
	return TRUE;  // 傳回 TRUE，除非您對控制項設定焦點


}

void CddeclientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}


void CddeclientDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 繪製的裝置內容

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 將圖示置中於用戶端矩形
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 描繪圖示
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}


HCURSOR CddeclientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CddeclientDlg::OnClose( )
{
	try {
		disconnectFromDdeServer();
	}
	catch(CppUtils::Exception &e) {
		writeErrorOutput(e.message() + " - " + e.context() + " - " + e.arg());
	}
	catch(...) {
		writeErrorOutput("Unknown errror on disconnectiong from DDE server");
	}

	DestroyWindow();
}

void CddeclientDlg::OnBnClickedOk()
{
	try {
		disconnectFromDdeServer();
	}
	catch(CppUtils::Exception &e) {
		writeErrorOutput(e.message() + " - " + e.context() + " - " + e.arg());
	}
	catch(...) {
		writeErrorOutput("Unknown errror on disconnectiong from DDE server");
	}
	
	OnOK();
}
