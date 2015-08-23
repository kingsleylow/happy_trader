// ddeclientDlg.cpp : ¹ê§@ÀÉ
//

#include "stdafx.h"
#include "mtmanager.h"
#include "mtmanagerDlg.h"
#include ".\mtmanagerdlg.h"
#include "platform\server\backend\src\hlpstruct\hlpstruct.hpp"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif 
 

#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#include <AtlConv.h>
#include <atlsafe.h>
#include <ATLComTime.h>



// ¹ï App About ¨Ï¥Î CAboutDlg ¹ï¸Ü¤è¶ô

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ¹ï¸Ü¤è¶ô¸ê®Æ
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ¤ä´©

// µ{¦¡½X¹ê§@
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
     
	  
	//appEdit_m.SetWindowText(theApp.paramApp_m.c_str());

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
	/*
	int winln = outEdit_m.GetWindowTextLength();

	// set thsi limit
	if ((winln + value.size()) >= 30000) {
		outEdit_m.SetSel(0, winln);
	}
	else {
		outEdit_m.SetSel(winln, winln);
	}
	outEdit_m.ReplaceSel(value.c_str());
	*/
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
	
}

BEGIN_MESSAGE_MAP(CddeclientDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CLOSE( )
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_MESSAGE( WM_QUERYENDSESSION, OnQueryEndSession )
	ON_BN_CLICKED(IDC_START_MT, &CddeclientDlg::OnBnClickedStartMt)
	ON_BN_CLICKED(IDC_BUTTON1, &CddeclientDlg::OnBnClickedButton1)
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

	
	

	// IDM_ABOUTBOX ¥²¶·¦b¨t²Î©R¥O½d³ò¤§¤¤¡C
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

	
	return TRUE;  // ¶Ç¦^ TRUE¡A°£«D±z¹ï±±¨î¶µ³]©wµJÂI


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
		CPaintDC dc(this); // Ã¸»sªº¸Ë¸m¤º®e

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ±N¹Ï¥Ü¸m¤¤©ó¥Î¤áºÝ¯x§Î
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ´yÃ¸¹Ï¥Ü
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


BOOL CALLBACK EnumWindowsProc
(
	HWND hwnd,	// äåñêðèïòîð ðîäèòåëüñêîãî îêíà
	LPARAM lParam 	// îïðåäåëÿåìîå ïðîãðàììîé çíà÷åíèå
)
{
	char buf[2048];
	CddeclientDlg* pThis = (CddeclientDlg*)lParam;
	buf[0] = '\0';
	::GetWindowText(hwnd, buf, 2048);
	CppUtils::String caption = (buf ? buf : "");
	CppUtils::toupper(caption);
	if (caption.size() > 0 && caption.find("METATRADER") !=std::string::npos ) {
		pThis->setMtHwnd(hwnd);
		return FALSE;
	}


	return TRUE;


}

BOOL CALLBACK EnumWindowsProc2
(
	HWND hwnd,	// äåñêðèïòîð ðîäèòåëüñêîãî îêíà
	LPARAM lParam 	// îïðåäåëÿåìîå ïðîãðàììîé çíà÷åíèå
)
{
	char buf[2048];
	CddeclientDlg* pThis = (CddeclientDlg*)lParam;
	buf[0] = '\0';
	::GetWindowText(hwnd, buf, 2048);
	CppUtils::String caption = (buf ? buf : "");
	


	return TRUE;


}

void CddeclientDlg::OnBnClickedStartMt()
{
	// TODO: Add your control notification handler code here

	CppUtils::String user = "477354";
	CppUtils::String password = "tdi5fvp";
	int i =1;
	// chnage user
	
	setMtHwnd(NULL);
	BOOL r = ::EnumWindows(EnumWindowsProc,  (LPARAM)this );

	if (mtHwnd_m != NULL) {
		// find parent

		HWND parent_hwnd = ::GetAncestor(mtHwnd_m, 2);
		if (!parent_hwnd)
			return;

		::PostMessage(parent_hwnd,WM_COMMAND,35429,0);
		HWND dialog_login_hwnd=::GetLastActivePopup(parent_hwnd);  
		if (!dialog_login_hwnd) {
			::MessageBox( NULL, "Invalid Dialog", "Fatal", MB_ICONERROR | MB_SERVICE_NOTIFICATION | MB_SYSTEMMODAL );
			return;
			
		}

		/*

		HWND htlogin_edit_pre = ::GetDlgItem(dialog_login_hwnd,0x49D);
		if (!htlogin_edit_pre) {
			::MessageBox( NULL, "Invalid Pre Login control", "Fatal", MB_ICONERROR | MB_SERVICE_NOTIFICATION | MB_SYSTEMMODAL );
			return;
		}

		HWND hlogin_edit=::GetDlgItem(htlogin_edit_pre,0x3E9);
		if (!hlogin_edit) {
			::MessageBox( NULL, "Invalid Login control", "Fatal", MB_ICONERROR | MB_SERVICE_NOTIFICATION | MB_SYSTEMMODAL );
			return;
		}
		
		::SendMessage(hlogin_edit, WM_SETTEXT, 0, (LPARAM)((LPSTR) user.c_str()));
		//::SendMessage(hlogin_edit, EM_SETSEL, 0, -1);
		//::SendMessage(hlogin_edit, EM_REPLACESEL, 0, (LPARAM)((LPSTR) user.c_str()));

		
		HWND hlogin_password=::GetDlgItem(dialog_login_hwnd,0x4C4);
		if (!hlogin_password) {
			::MessageBox( NULL, "Invalid Password control", "Fatal", MB_ICONERROR | MB_SERVICE_NOTIFICATION | MB_SYSTEMMODAL );

			return;
		}

		::SendMessage(hlogin_password, WM_SETTEXT, 0, (LPARAM)((LPSTR) user.c_str()));
		//::SendMessage(hlogin_password, EM_SETSEL, 0, -1);
		//::SendMessage(hlogin_password, EM_REPLACESEL, 0, (LPARAM)((LPSTR) password.c_str()));

		*/

		/*
		HWND hbutton_login=::GetDlgItem(dialog_login_hwnd,0x1);

		
		//
		if (!hbutton_login) {
			::MessageBox( NULL, "Invalid Button Login", "Fatal", MB_ICONERROR | MB_SERVICE_NOTIFICATION | MB_SYSTEMMODAL );

			return;
		}

		::SendMessage(hbutton_login,BM_CLICK,0,0);
		*/
		

		
		CppUtils::sleep(2.0);

		::SendMessage(dialog_login_hwnd, BM_CLICK, 0, 0);
	}


}


void CddeclientDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	if (mtHwnd_m != NULL) {
	}

}
