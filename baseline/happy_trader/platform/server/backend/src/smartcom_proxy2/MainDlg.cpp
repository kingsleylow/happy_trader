// MainDlg.cpp : Implementation of CMainDlg

#include "smartcom_proxy2defs.h"
#include "MainDlg.h"
#include "MainSmartComEngine.h"

#include "platform\server\backend\src\inqueue\inqueue.hpp"
#include "platform\server\backend\src\inqueue\commontypes.hpp"
#include "platform\server\backend\src\inqueue\outproc.hpp"


//#define SEND_TEST_OHCL(SYMBOL, DATETIME, OPEN, HIGH, LOW, CLOSE, VOLUME)	\
//	Inqueue::OutProcessor::send_rt_data_ohcl( CppUtils::parseFormattedDateTime("M/d/Y h:m:s",DATETIME,0), "dummy_rt", SYMBOL, OPEN, HIGH, LOW, CLOSE, VOLUME)

#define TIMER_1SEC 1
static char const* ICON_TEXT = "Smartcom API";
static char const* SIMULATE_TICKS_OFF_TEXT = "Simulate Ticks Off";
static char const* SIMULATE_TICKS_ON_TEXT = "Simulate Ticks On";

namespace SmartComHelperLib {


	// CMainDlg

CMainDlg::CMainDlg(CMainSmartComEngine& mainEngine):
mainEngine_m(mainEngine),
simulatingTicks_m(false),
simulatingTicksCounter_m(0)
{
}

CMainDlg::~CMainDlg()
{
}

																				  

void CMainDlg::addLogEntry(CppUtils::String const& logMsg)
{
	CppUtils::String fullString;
	fullString.append("[ ");
	fullString.append(CppUtils::getGmtTime(CppUtils::getTime()));
	fullString.append(" ] - ");
	fullString.append(logMsg);

	::SendDlgItemMessage(HWND(*this), IDC_LIST_LOG, LB_ADDSTRING, 0, (LPARAM)fullString.c_str());

}

void CMainDlg::initialize(char const* loginId)
{
	::SetDlgItemText(HWND(*this), IDC_EDIT_LOGIN, loginId);

	CppUtils::String initLog = "Server ver: ";
	initLog.append(this->serverVersionString_m).append(" Client ver: ").append(this->clientVersionString_m);
	mainEngine_m.notification(N_OK, initLog.c_str(), NULL);
}

void CMainDlg::setUpInitialData(CppUtils::String const&clientVerString, CppUtils::String const&srvVerString)
{
	clientVersionString_m = clientVerString;

	serverVersionString_m =	 srvVerString;
}

void CMainDlg::clearSymbolList()
{
	//CppUtils::Uid uid;
	//uid.generate();

	symbolFileEntry_m.init(createTempHtml());


	symbolFileEntry_m.getStream() << "<table>";
}

void CMainDlg::addSymbolEntry(
															long const& row, 
															long const& nrows, 
															char const* symbol, 
															char const* short_name, 
															char const*  long_name, 
															char const*  type, 
															long const& decimals, 
															long const& lot_size, 
															double const& punkt,
															double const&  step, 
															char const*  sec_ext_id, 
															char const*  sec_exch_name, 
															double const& expiry_date, 
															double const& days_before_expiry
															)
{

	if (symbolFileEntry_m.isOpen()) {

		symbolFileEntry_m.getStream() << "<tr bgcolor=gray>";
		symbolFileEntry_m.getStream() << "<td nowrap=true>" << CppUtils::long2String(row) << "</td>";
		symbolFileEntry_m.getStream() << "<td nowrap=true>" << CppUtils::long2String(nrows) << "</td>";
		symbolFileEntry_m.getStream() << "<td nowrap=true>" << symbol << "</td>";
		symbolFileEntry_m.getStream() << "<td nowrap=true>" << short_name << "</td>";
		symbolFileEntry_m.getStream() << "<td nowrap=true>" << long_name << "</td>";
		symbolFileEntry_m.getStream() << "<td nowrap=true>" << type << "</td>";
		symbolFileEntry_m.getStream() << "<td nowrap=true>" << CppUtils::long2String(decimals) << "</td>";
		symbolFileEntry_m.getStream() << "<td nowrap=true>" << CppUtils::long2String(lot_size) << "</td>";
		symbolFileEntry_m.getStream() << "<td nowrap=true>" << CppUtils::double2String(punkt) << "</td>";
		symbolFileEntry_m.getStream() << "<td nowrap=true>" << CppUtils::double2String(step) << "</td>";
		symbolFileEntry_m.getStream() << "<td nowrap=true>" << (sec_ext_id ? sec_ext_id: "") << "</td>";
		symbolFileEntry_m.getStream() << "<td nowrap=true>" << (sec_exch_name ? sec_exch_name: "")  << "</td>";
		symbolFileEntry_m.getStream() << "<td nowrap=true>" << CppUtils::getGmtTime2(expiry_date) << "</td>";
		symbolFileEntry_m.getStream() << "<td nowrap=true>" << CppUtils::long2String((int)days_before_expiry) << "</td>";
		symbolFileEntry_m.getStream() << "</tr>";



		if (row >= (nrows-1)) {

			symbolFileEntry_m.getStream() << "</table>";
			CppUtils::String fileN = symbolFileEntry_m.getFileName();

			symbolFileEntry_m.deinit();
			openTempHtml(fileN);
		

		}
	}


}

void CMainDlg::addSymbolHeader(
															 char const* row_c, 
															 char const* nrows_c, 
															 char const* symbol_c, 
															 char const* short_name_c, 
															 char const*  long_name_c, 
															 char const*  type_c, 
															 char const* decimals_c, 
															 char const* lot_size_c, 
															 char const* punkt_c, 
															 char const*  step_c, 
															 char const*  sec_ext_id_c, 
															 char const*  sec_exch_name_c, 
															 char const* expiry_date_c, 
															 char const* days_before_expiry_c
															 )
{
	if (symbolFileEntry_m.isOpen()) {

		symbolFileEntry_m.getStream() << "<tr bgcolor=red style='border:1px solid black'>";

		symbolFileEntry_m.getStream() << "<td nowrap=true>" << row_c << "</td>";
		symbolFileEntry_m.getStream() << "<td nowrap=true>" << nrows_c << "</td>";
		symbolFileEntry_m.getStream() << "<td nowrap=true>" << symbol_c << "</td>";
		symbolFileEntry_m.getStream() << "<td nowrap=true>" << short_name_c << "</td>";
		symbolFileEntry_m.getStream() << "<td nowrap=true>" << long_name_c << "</td>";
		symbolFileEntry_m.getStream() << "<td nowrap=true>" << type_c << "</td>";
		symbolFileEntry_m.getStream() << "<td nowrap=true>" << decimals_c << "</td>";
		symbolFileEntry_m.getStream() << "<td nowrap=true>" << lot_size_c << "</td>";
		symbolFileEntry_m.getStream() << "<td nowrap=true>" << punkt_c << "</td>";
		symbolFileEntry_m.getStream() << "<td nowrap=true>" << step_c << "</td>";
		symbolFileEntry_m.getStream() << "<td nowrap=true>" << sec_ext_id_c << "</td>";
		symbolFileEntry_m.getStream() << "<td nowrap=true>" << sec_exch_name_c << "</td>";
		symbolFileEntry_m.getStream() << "<td nowrap=true>" << expiry_date_c << "</td>";
		symbolFileEntry_m.getStream() << "<td nowrap=true>" << days_before_expiry_c << "</td>";

		symbolFileEntry_m.getStream() << "</tr>";

	}





}

void CMainDlg::setConnectionStatus(bool const is_connected)
{
	if (is_connected)
		::SetDlgItemText(HWND(*this), IDC_EDIT_CONNECTED, "CONNECTED");
	else
		::SetDlgItemText(HWND(*this), IDC_EDIT_CONNECTED, "DISCONNECTED");

}



void CMainDlg::clearPortfolioList()
{
	//CppUtils::Uid uid;
	//uid.generate();

	portfolioEntry_m.init(createTempHtml());
	if (portfolioEntry_m.isOpen()) {
		portfolioEntry_m.getStream() << "<table>";

		portfolioEntry_m.getStream() << "<tr bgcolor=red style='border:1px solid black'>";
		portfolioEntry_m.getStream() << "<td nowrap=true>Portfolio Name</td>";
		portfolioEntry_m.getStream() << "<td nowrap=true>Portfolio Exchange</td>";
		portfolioEntry_m.getStream() << "</tr>";
	}
}

void CMainDlg::addPortfolioRow(long row, long nrows, char const* portfolioName, char const* portfolioExch)
{
	if (portfolioEntry_m.isOpen()) {

		portfolioEntry_m.getStream() << "<tr bgcolor=gray>";
		portfolioEntry_m.getStream() << "<td nowrap=true>" << (portfolioName ? portfolioName: "" )<< "</td>";
		portfolioEntry_m.getStream() << "<td nowrap=true>" << (portfolioExch ? portfolioExch : "") << "</td>";

		portfolioEntry_m.getStream() << "</tr>";



		if (row >= (nrows-1)) {

			portfolioEntry_m.getStream() << "</table>";
			CppUtils::String fileN = portfolioEntry_m.getFileName();

			portfolioEntry_m.deinit();
			openTempHtml(fileN);
			

		}
	}
}

CppUtils::String CMainDlg::createTempHtml()
{
	CppUtils::Uid uid;
	uid.generate();

	CppUtils::String dir=	 CppUtils::getTempPath() + PATH_SEPARATOR_STR;
	dir += SMARTCOM_TEMP_DIR;

	CppUtils::makeDir(dir);
	CppUtils::String file = dir + PATH_SEPARATOR_STR +uid.toString()+ "_temp.html"; 

	return file;
}


void CMainDlg::openTempHtml(CppUtils::String const& file)
{
	::ShellExecute(NULL, "open", CppUtils::getDefaultBrowser().c_str(), file.c_str(), NULL, SW_SHOWNORMAL);

}

void CMainDlg::mesageBox(char const* message)
{
	::MessageBox( NULL, message, "Warning", MB_ICONERROR | MB_SERVICE_NOTIFICATION | MB_SYSTEMMODAL );
}

void CMainDlg::mesageBox(CppUtils::String const& message)
{
	mesageBox(message.c_str());
}

void CMainDlg::showIcon()
{
	NOTIFYICONDATA nid;  
	memset(&nid,0,sizeof(NOTIFYICONDATA));

	
	nid.cbSize = sizeof(NOTIFYICONDATA);  
	nid.hWnd = HWND(*this);  
	nid.uID = 100;				  
	nid.uVersion = NOTIFYICON_VERSION;  
	nid.uCallbackMessage = WM_TRAY_ICON_MESSAGE;
	
	HINSTANCE hMyInstance = GetModuleHandle(NULL);
	nid.hIcon = ::LoadIcon(NULL, IDI_APPLICATION);

	//nid.hIcon =(HICON)LoadImage(hMyInstance, MAKEINTRESOURCE(IDI_MAIN_ICON), IMAGE_ICON, 16, 16, 0);
	if (nid.hIcon != NULL) {
		strncpy (nid.szTip,ICON_TEXT,strlen(ICON_TEXT)); // set tool tip text
		nid.szTip[strlen(ICON_TEXT)-1]='\0';
 
		nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP ;  
		Shell_NotifyIcon(NIM_ADD, &nid); 
	}
	else {
		LOG(MSG_WARN, "MAIN_DLG", CppUtils::getOSError());
	}

}

void CMainDlg::hideIcon()
{
	NOTIFYICONDATA nid;  
	memset(&nid,0,sizeof(NOTIFYICONDATA));

	nid.cbSize = sizeof(NOTIFYICONDATA);  
	nid.hWnd = HWND(*this);  
	nid.uID = 100;  
	Shell_NotifyIcon(NIM_DELETE, &nid); 

}

// HANDLERS
// -------------------------

LRESULT CMainDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CAxDialogImpl<CMainDlg>::OnInitDialog(uMsg, wParam, lParam, bHandled);

	RECT rect; 
	::GetWindowRect(HWND(*this), &rect);
	int width = rect.right - rect.left;

	// horizontal
	::SendDlgItemMessage(HWND(*this), IDC_LIST_LOG, LB_SETHORIZONTALEXTENT, width*4, -1);
	bHandled = TRUE;

	::SetDlgItemText(HWND(*this), IDC_SIMULATE, SIMULATE_TICKS_OFF_TEXT);

	setConnectionStatus(false);

	// timer
	UINT_PTR rt = ::SetTimer(HWND(*this),                // handle to main window 
			TIMER_1SEC,               // timer identifier 
			1000,                     // 1-second interval 
			NULL); 

	if (!rt)
		THROW(CppUtils::OperationFailed, "exc_CannotCreateTimer", "ctx_OnInitDialog", CppUtils::getOSError());


	return 1;  // Let the system set the focus
}

LRESULT CMainDlg::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	ShowWindow(SW_HIDE);
	showIcon();

	return 1;
}

LRESULT CMainDlg::OnBnClickedButtonQuerySymb(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here

	HRESULT hr = mainEngine_m.checkConnectStatus();

	if (FAILED(hr)) {
		mesageBox("Not connected");
		return 0;
	}

	hr = mainEngine_m.querySymbols();
	
	if (FAILED(hr)) {
		mesageBox("Cannot query symbols");
	}

	return 0;    
}

LRESULT CMainDlg::OnBnClickedButtonGetPortfolio(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here

	HRESULT hr = mainEngine_m.checkConnectStatus();

	if (FAILED(hr)) {
		mesageBox("Not connected");
		return 0;
	}

	hr = mainEngine_m.getPortfolioList();

	if (FAILED(hr)) {
		mesageBox("Cannot get portfolio list");
	}

	return 0;
}

LRESULT CMainDlg::OnBnClickedButtonStatus(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	HRESULT hr = mainEngine_m.checkConnectStatus();

	if (FAILED(hr)) {
		mesageBox("Not connected");
		return 0;
	}
	
	try {
		CppUtils::StringSet leve11Symbols;
		CppUtils::StringSet ticksSymbols;
		CppUtils::StringSet level2Symbols;

		mainEngine_m.getSubscribedSymbols(leve11Symbols, ticksSymbols, level2Symbols);

		// now create HTML report
		FileEntry fe;
		fe.init(createTempHtml());

		CppUtils::String fileName = fe.getFileName();

		// ----
		fe.getStream() << "<table><tr><td bgcolor=red style='border:1px solid black'>" << "Subscribed LEVEL 1 symbols" << "</td></tr>";
		for( CppUtils::StringSet::const_iterator it = leve11Symbols.begin(); it != leve11Symbols.end(); it++) {
				fe.getStream() << "<tr bgcolor=gray><td>" << *it << "</td></tr>";
		}
		fe.getStream() << "</table>";

		// ----
		fe.getStream() << "<table><tr><td bgcolor=red style='border:1px solid black'>" << "Subscribed tick symbols" << "</td></tr>";
		for( CppUtils::StringSet::const_iterator it = ticksSymbols.begin(); it != ticksSymbols.end(); it++) {
				fe.getStream() << "<tr bgcolor=gray><td>" << *it << "</td></tr>";
		}
		fe.getStream() << "</table>";

		// ----
		fe.getStream() << "<table><tr><td bgcolor=red style='border:1px solid black'>" << "Subscribed LEVEL 2 symbols" << "</td></tr>";
		for( CppUtils::StringSet::const_iterator it = level2Symbols.begin(); it != level2Symbols.end(); it++) {
				fe.getStream() << "<tr bgcolor=gray><td>" << *it << "</td></tr>";
		}
		fe.getStream() << "</table>";


		fe.deinit();
		openTempHtml(fileName);

	}
	catch(CppUtils::Exception& e)
	{
		mesageBox(e.message());
	}
	catch(...)
	{
		mesageBox("Unknown exception");
	}


	return 0;
}

LRESULT CMainDlg::OnBnClickedButtonCancelHistory(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	mainEngine_m.freeHistoryBarRetrievalTasksForcibly();
	return 0;
}

LRESULT CMainDlg::OnBnClickedDisConnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	mainEngine_m.silentlyDisconnectBeforeStartEnd();
	return 0;
}


LRESULT CMainDlg::OnBnClickedSimulate(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (simulatingTicks_m) {
		simulatingTicks_m = false;
		mainEngine_m.generate_Simulation_Bars_OnFinish();
		::SetDlgItemText(HWND(*this), IDC_SIMULATE, SIMULATE_TICKS_OFF_TEXT);
	}
	else {
		simulatingTicks_m = true;
		simulatingTicksCounter_m = 0;
		mainEngine_m.generate_Simulation_Bars_OnStart();

		::SetDlgItemText(HWND(*this), IDC_SIMULATE, SIMULATE_TICKS_ON_TEXT);
	}
	return 0;
}

LRESULT CMainDlg::OnIconMesssage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;

	switch (lParam) {
		case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
    case WM_LBUTTONDOWN:
			hideIcon();
			ShowWindow(SW_SHOW);
			break;
		default:
			;
	}

	return 0;
}

LRESULT CMainDlg::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	try {
		mainEngine_m.onTimer_1_sec();

		if (simulatingTicks_m)
			generateSimulatingTicks_Bars();

	}
	catch(CppUtils::Exception& e)
	{
		CppUtils::String es("Exception in OnTimer(...) call: ");
		es.append(EXC2STR(e));
		addLogEntry(es.c_str());

		if (simulatingTicks_m)
			simulatingTicks_m = false;
	}
	catch(...)
	{
		addLogEntry("Unknown exception in OnTimer(...) call");

		if (simulatingTicks_m)
			simulatingTicks_m = false;
	}

	return 0;
}

LRESULT CMainDlg::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	::KillTimer(HWND(*this), TIMER_1SEC); 
	return 0;
}

/**
* Helpers
*/

void CMainDlg::generateSimulatingTicks_Bars()
{
	if (simulatingTicksCounter_m++ >= 59) {
		simulatingTicksCounter_m = 0;
		mainEngine_m.generate_Simulation_Bars_1Min();
	}

}


}; // end of namepsace