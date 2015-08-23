// ddeclientDlg.h : 標頭檔 
//

#pragma once
#include "afxwin.h"


// CddeclientDlg 對話方塊
class CddeclientDlg : public CDialog,
	public CppUtils::DDEWrap
{
// 建構
public:
	CddeclientDlg(CWnd* pParent = NULL);	


	enum { IDD = IDD_DDECLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	

protected:
	HICON m_hIcon;

	
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()


private:
	// custom functions
	void connectToDdeServer();

	void disconnectFromDdeServer();

	virtual void onDataArrived(CppUtils::DDEItem const& item, CppUtils::String const& value);

	void writeOkOutput(CppUtils::String const& topic, CppUtils::String const& item, CppUtils::String const& value);

	void writeErrorOutput(CppUtils::String const& errorMsg);

	void appendTextToControl(CppUtils::String const& value);

	
	
public:
	afx_msg void OnClose( );

	afx_msg void OnBnClickedOk();
	afx_msg LRESULT OnQueryEndSession(WPARAM wParam, LPARAM lParam);
	CEdit outEdit_m;
	CEdit appEdit_m;

	CppUtils::PipeClient client_m;
};
