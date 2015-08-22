#include "windowproc.hpp"
#include "laserexcept.hpp"
#include <tchar.h>
#include <time.h>

#define LASERWINPROC "LASERWINPROC"
#define WIN_CLASS "GTSessionWindow"
#define CONNECTED "CONNECTED"
#define DISCONNECTED "DISCONNECTED"
#define DEFAULT_QUOTE_SERVER_ENTRY "Quote1"
#define NOT_AVAILABLE "N/A"


// ---------------------------------

#define LTRY  try  

#define LCATCH(FUN) \
			catch(CppUtils::Exception& e)	\
			{	\
				LOG(MSG_ERROR, LASERWINPROC, "Exception " << #FUN  << " " << e.message());	\
				LOGEVENT(HlpStruct::CommonLog::LL_ERROR, LASERWINPROC,  "Exception " << #FUN  << " - " << e.message() << " - " << e.arg());	\
			}	\
			catch(...) {	\
				LOG(MSG_ERROR, LASERWINPROC,  "Unknown exception " << #FUN );	\
				LOGEVENT(HlpStruct::CommonLog::LL_ERROR, LASERWINPROC,  "Unknown exception " << #FUN );	\
			};

	#define DELEGATOR(X) \
	{	\
		LOCK_FOR_SCOPE(WindowThreadProc::registerCallbackMtx_m);	\
		for(set<LaserBroker*>::iterator it = WindowThreadProc::callbackDelegates_m.begin(); it != WindowThreadProc::callbackDelegates_m.end(); it++) {	\
			(*it)->X;	\
		}	\
	}

namespace BrkLib {

	
	// ----------------------------------

	HINSTANCE WindowThreadProc::hinstDll_m = NULL;

	WindowThreadProc* WindowThreadProc::instance_m = NULL;

	int WindowThreadProc::refCnt_m = 0;

	CppUtils::Mutex WindowThreadProc::singletonMtx_m;


	CppUtils::Mutex WindowThreadProc::registerCallbackMtx_m;

	set<LaserBroker*> WindowThreadProc::callbackDelegates_m;

	HWND WindowThreadProc::win_m = 0;

	
	
	// --------------------
	// message to stop
	#define UWM_STOP_DLG_MSG _T("UWM_STOP_DLG_MSG-{7FDB2C34-5510-4d30-99A9-CD9752E0D680}")
	UINT UWM_STOP_DLG;

	#define UWM_STARTUP_LASER_MSG _T("UWM_STARTUP_LASER_MSG-{3DFFB929-3396-42cf-88EC-01AD5B0FA959}")
	UINT UWM_STARTUP_LASER;


	// ------------------------------------

	int bootThread (void *ptr)
	{
		// handle run-time exceptions
#ifdef HANDLE_NATIVE_EXCEPTIONS
			Testframe::init();
#endif

		WindowThreadProc* pThis = (WindowThreadProc*)ptr; 

		try {
			pThis->threadRun();
		}
		catch(CppUtils::Exception& e) {
			LOG(MSG_ERROR, LASERWINPROC, "Fatal exception: " << e.message());
			//CppUtils::displayError(CppUtils::String( "LASER API fatal exception, please reload: ") + e.message());
		}
#ifdef HANDLE_NATIVE_EXCEPTIONS
		catch(Testframe::Exception& e) {
			LOG(MSG_ERROR, LASERWINPROC, "Fatal MSVC exception: " << e.message());
			//CppUtils::displayError(CppUtils::String( "LASER API fatal MSVC exception, please reload: ") + e.message());
		}
#endif
		catch(...) {
			LOG(MSG_ERROR, LASERWINPROC, "Unknown fatal exception");
			//CppUtils::displayError("LASER API unknown exception, please reload: ");
		}

		return 42;

	}

	// ----------------------------------

	WindowThreadProc::WindowThreadProc( ):
	hSession_m(0),
	initializedAsProvider_m(false)
	{
	
		
		// initialize thread
		thread_m = new CppUtils::Thread(bootThread, this);
		LOG( MSG_DEBUG, LASERWINPROC, "Helper window thread created" );

		if (!wndCreated_m.lock(5000))
			THROW(LaserFailed, "exc_CannotFinishInitialization", "ctx_WindowThreadProcCtr", "");

	}
	
	WindowThreadProc::~WindowThreadProc()
	{
		if (!wndCreated_m.lock(5000))
			THROW(LaserFailed, "exc_CannotFinishInitialization", "ctx_WindowThreadProcDtr", "");

		shutDownLaserAPI();

		if (!wndFinished_m.lock(5000)) {
			LOG( MSG_ERROR, LASERWINPROC, "Cannot stop thread in timely manner" );
		}

	
		// Delete thread
		delete thread_m;
		thread_m = NULL;

	
		LOG( MSG_DEBUG, LASERWINPROC, "Window helper thread terminated" );



	}



	void WindowThreadProc::setApiVersionNumber(HWND dlg)
	{
	
		char buf[64];

		LONG version = GTAPI_VERSION;
		WORD a_vm = HIWORD(version);
		WORD a_ur = LOWORD(version);

		BYTE v = HIBYTE(a_vm);
		BYTE m = LOBYTE(a_vm);
    BYTE u = HIBYTE(a_ur);
		BYTE r = LOBYTE(a_ur);

		sprintf(buf, "%01u,%01u,%01u,%01u ", (int)v, (int)m, (int)r, (int)u );
		
		::SetDlgItemText(dlg, IDC_EDIT_API_VERSION, buf);

		
		
	}

	
	void WindowThreadProc::setLevel1SubscriptionStatus(bool is_level1_subscribed)
	{
		if (win_m==NULL)
			return;

		// IDC_CHECK_LEVEL1_SUBSCRIBED
		if (is_level1_subscribed)
			::CheckDlgButton(win_m, IDC_CHECK_LEVEL1_SUBSCRIBED, BST_CHECKED);
		else
			::CheckDlgButton(win_m, IDC_CHECK_LEVEL1_SUBSCRIBED, BST_UNCHECKED);

	}

	void WindowThreadProc::setEnableLooping(bool const isEnable)
	{
		if (isEnable)
			::CheckDlgButton(win_m, IDC_LOOP_ENTRIES_CHECK, BST_CHECKED);
		else
			::CheckDlgButton(win_m, IDC_LOOP_ENTRIES_CHECK, BST_UNCHECKED);
	}


	void WindowThreadProc::setSubscribedSymbolsNumber(int symbNumber)
	{
		
		if (win_m==NULL)
			return;
	
		setItemInteger(win_m, IDC_EDIT_SUBSCRIBED_SYMBOLS, symbNumber);


	}

	// -------------------

	void WindowThreadProc::setExecutorConnectStatus(bool is_connected, CppUtils::String const strAddr, CppUtils::String const& srvId)
	{
		
		if (win_m==NULL)
			return;
	
		

		if (is_connected)
			::SetDlgItemText(win_m, IDC_EDIT_API_EXEC_CONNECTED, CONNECTED);
		else
			::SetDlgItemText(win_m, IDC_EDIT_API_EXEC_CONNECTED, DISCONNECTED);

		::SetDlgItemText(win_m, IDC_EDIT_EXECUTOR_ADDR, strAddr.c_str());
		::SetDlgItemText(win_m, IDC_EDIT_EXECUTOR_ID, srvId.c_str());

	
	}

	void WindowThreadProc::setQuoteConnectStatus(bool is_connected, CppUtils::String const strAddr, CppUtils::String const& srvId)
	{
		if (win_m==NULL)
			return;

		if (is_connected)
			::SetDlgItemText(win_m, IDC_EDIT_API_QUOTE_CONNECTED, CONNECTED);
		else
			::SetDlgItemText(win_m, IDC_EDIT_API_QUOTE_CONNECTED, DISCONNECTED);

		::SetDlgItemText(win_m, IDC_EDIT_QUOTE_ADDR, strAddr.c_str());
		::SetDlgItemText(win_m, IDC_EDIT_QUOTE_ID, srvId.c_str());
		
	}

	void WindowThreadProc::setChartConnectStatus(bool is_connected, CppUtils::String const strAddr, CppUtils::String const& srvId)
	{
	
		if (win_m==NULL)
			return;

		if (is_connected)
			::SetDlgItemText(win_m, IDC_EDIT_API_CHART_CONNECTED, CONNECTED);
		else
			::SetDlgItemText(win_m, IDC_EDIT_API_CHART_CONNECTED, DISCONNECTED);

		::SetDlgItemText(win_m, IDC_EDIT_CHART_ADDR, strAddr.c_str());
		::SetDlgItemText(win_m, IDC_EDIT_CHART_ID, srvId.c_str());
	}

	void WindowThreadProc::setLevel2ConnectStatus(bool is_connected, CppUtils::String const strAddr, CppUtils::String const& srvId)
	{
	
		if (win_m==NULL)
			return;

		if (is_connected) {
			::SetDlgItemText(win_m, IDC_EDIT_API_LEVEL2_CONNECTED, CONNECTED);
		}
		else {
			::SetDlgItemText(win_m, IDC_EDIT_API_LEVEL2_CONNECTED, DISCONNECTED);
		}

		::SetDlgItemText(win_m, IDC_EDIT_LEVEL2_ADDR, strAddr.c_str());
		::SetDlgItemText(win_m, IDC_EDIT_LEVEL2_ID, srvId.c_str());
	
	}
	
	void WindowThreadProc::setItemInteger(HWND win, int resource_id, int const value)
	{
		if (win_m==NULL)
			return;

		char buf[32];
		_itoa(value, buf, 10);

		::SetDlgItemText(win_m, resource_id, buf);
	}
	// --------------------
	//  ZVV

	void WindowThreadProc::setNumberOfTickers(unsigned long const num)
	{
		if (win_m==NULL)
			return;

	
		setItemInteger(win_m, IDC_EDIT_TICK_NUM, num);
		
	}

		

	void WindowThreadProc::setReadyToOperate()
	{
		wndCreated_m.signalEvent();
	}
	
	void WindowThreadProc::setReadyFinished()
	{
		wndFinished_m.signalEvent();
	}

	
	void WindowThreadProc::threadRun()
	{
		LOG( MSG_DEBUG, LASERWINPROC, "Initializing GTAPIB" );

		
		if(gtInitialize (GTAPI_VERSION	 , hinstDll_m) != 0) {
			THROW(LaserFailed, "exc_CannotInitGTAPIB", "ctx_ThreadFun", "");
		}
		LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERWINPROC, "gtInitialize() - OK");
		
		
		// pass this ptr
		::DialogBoxParam(hinstDll_m, MAKEINTRESOURCE(IDD_DIALOGMAIN), NULL, (DLGPROC)&WindowThreadProc::testDialogProc, (LPARAM)this);
		gtUninitialize ();
		LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERWINPROC, "gtUninitialize() - OK");

		CppUtils::sleep(1.0);
		LOG( MSG_DEBUG, LASERWINPROC, "Finished message loop and deinit GTAPIB" );

	}

	void WindowThreadProc::shutDownLaserAPI()
	{
		::PostMessage(win_m, UWM_STOP_DLG, 0, (LPARAM)this);
	}

	void WindowThreadProc::startupLaserAPI()
	{
		::PostMessage(win_m, UWM_STARTUP_LASER, 0, (LPARAM)this);
	}

	

// ------------------------
// callbacks

int WindowThreadProc::OnExecMsgLogin(LPGTSESSION hSession, BOOL bLogin)
{
	LTRY {
	
		LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERWINPROC, "OnExecMsgLogin(): " << (bLogin == TRUE ? "SUCCESS":"FAILURE")  );
	}
	LCATCH(OnExecMsgLogin)
	return 0;
}

int WindowThreadProc::OnExecConnected(LPGTSESSION hSession) 
{ 
	LTRY {

		setExecutorConnectStatus(true, instance_m->serversSettings_m.getCurrentExecutor().toString(), instance_m->serversSettings_m.getCurrentExecutor().getId());
		
		LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERWINPROC, "Executor connected to address: " << instance_m->serversSettings_m.getCurrentExecutor().toString());
		
	}
	LCATCH(OnExecConnected)
	return 0;
};

int   WindowThreadProc::OnExecDisconnected(LPGTSESSION hSession) { 
	LTRY {
		setExecutorConnectStatus(false, instance_m->serversSettings_m.getCurrentExecutor().toString(), instance_m->serversSettings_m.getCurrentExecutor().getId());
		LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERWINPROC, "Executor disconnected"  );
		
	}
	LCATCH(OnExecDisconnected)
	return 0; 
};  


int  WindowThreadProc::OnExecMsgLoggedin(LPGTSESSION hSession) { 
	
	LTRY {
		
		instance_m->subscribe_all_symbols();
		LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERWINPROC, "Logged in and subscribed for symbols (!)"  );

		// success only here
		instance_m->set_startup_ok(true);
		instance_m->signal_logged_subscribed();
	}
	LCATCH(OnExecMsgLoggedin)

	return 0; 
	
};   


int   WindowThreadProc::OnExecMsgLogout(LPGTSESSION hSession) {
	LTRY {
		instance_m->unsubscribe_all_symbols();
		LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERWINPROC, "Logged out and unsubscribed for symbols (!)"  );

	}
	LCATCH(OnExecMsgLogout)
	return 0; 
};

int   WindowThreadProc::OnExecMsgTrade(LPGTSESSION hSession, const GTTrade32 *trade) { 
	DELEGATOR(OnExecMsgTrade(hSession,trade)); 
	return 0; 
};

int   WindowThreadProc::OnExecMsgAccount(LPGTSESSION hSession, const GTAccount32 *account) { 
	DELEGATOR(OnExecMsgAccount(hSession,account)); 
	return 0; 
};

int   WindowThreadProc::OnExecMsgOpenPosition(LPGTSESSION hSession, const GTOpenPosition32 *open) { 
	DELEGATOR(OnExecMsgOpenPosition(hSession,open)); 
	return 0; 
};

int   WindowThreadProc::OnExecMsgPending(LPGTSESSION hSession, const GTPending32 *pending) {  
	DELEGATOR(OnExecMsgPending(hSession,pending)); 
	return 0; 
};

int   WindowThreadProc::OnExecMsgSending(LPGTSESSION hSession,  const GTSending32 *sending) {  
	DELEGATOR(OnExecMsgSending(hSession,sending)); 
	return 0; 
};

int   WindowThreadProc::OnExecMsgCanceling(LPGTSESSION hSession, const GTCancel32 * cancel) { 
	DELEGATOR(OnExecMsgCanceling(hSession,cancel)); 
	return 0; 
};

int   WindowThreadProc::OnExecMsgCancel(LPGTSESSION hSession, const GTCancel32 * cancel) {  
	DELEGATOR(OnExecMsgCancel(hSession,cancel)); 
	return 0; 
};

int   WindowThreadProc::OnExecMsgReject(LPGTSESSION hSession, const GTReject32 *reject) {  
	DELEGATOR(OnExecMsgReject(hSession,reject)); 
	return 0; 
};

int   WindowThreadProc::OnExecMsgRemove(LPGTSESSION hSession, const GTRemove32 *remove) { 
	DELEGATOR(OnExecMsgRemove(hSession,remove));
	return 0; 
};

int   WindowThreadProc::OnExecMsgRejectCancel(LPGTSESSION hSession, const GTRejectCancel32 *rejectcancel)  { 
	DELEGATOR(OnExecMsgRejectCancel(hSession,rejectcancel)); 
	return 0; 
};

int   WindowThreadProc::OnExecMsgStatus(LPGTSESSION hSession, const GTStatus32 *status) { 
	DELEGATOR(OnExecMsgStatus(hSession,status));
	return 0; 
};

int WindowThreadProc::OnExecMsgState(LPGTSESSION hSession, const GTServerState32 *state)
{
	DELEGATOR(OnExecMsgState(hSession,state));
	return 0; 
}


double WindowThreadProc::returnAdoptedPrintTime(double const& ttime, char const* symbol)
{
	map<CppUtils::String, double>::iterator it = timePrintAdopter_m.find(symbol);
	if (it == timePrintAdopter_m.end()) {
		// store
		timePrintAdopter_m.insert(pair<CppUtils::String, double>(symbol, ttime));
		return ttime;
	}
	else {
	
		if (it->second < ttime) {
			// just store 
			it->second = ttime;
		}
		else {
			// adopt
			it->second += 0.002;
		}

		return it->second;

	}
}

double WindowThreadProc::returnAdoptedLevel1Time(double const& ttime, char const* symbol)
{
	map<CppUtils::String, double>::iterator it = timeLevel1Adopter_m.find(symbol);
	if (it == timeLevel1Adopter_m.end()) {
		// store
		timeLevel1Adopter_m.insert(pair<CppUtils::String, double>(symbol, ttime));
		return ttime;
	}
	else {
	
		if (it->second < ttime) {
			// just store 
			it->second = ttime;
		}
		else {
			// adopt
			it->second += 0.002;
		}

		return it->second;

	}
}

int WindowThreadProc::OnGotQuoteLevel1(LPGTSESSION hSession,  const GTLevel132 *level1)
{
	LTRY {
		// level one
		
		//LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERROKER, "Level1 time: " << level1->gtime.dwTime << " str " <<  CppUtils::getGmtTime(level1data.getTime()) );

		instance_m->tickersNumber_m++;

	

		SEND_LEVEL1_DATA(
			instance_m->returnAdoptedLevel1Time(instance_m->laser_convertToCurrentUnixDateTime(instance_m->laser_convertToUnixTime(&level1->gtime)), level1->szStock),
			instance_m->providerForRT_m,
			level1->szStock,
			level1->locBidPrice,
			level1->locBidSize,
			level1->locAskPrice,
			level1->locAskSize
		);

	}
	LCATCH(OnGotQuoteLevel1)
	return 0;

	 
}



int WindowThreadProc::OnGotQuotePrint(LPGTSESSION hSession,  const GTPrint32 *printR)
{
	// no delegating here  - no need as quotes
	LTRY {

		instance_m->tickersNumber_m++;
		CppUtils::Byte color;
		if (printR->rgbColor == PRINT_BID_COLOR) {
			color = HlpStruct::RtData::CT_Bid_Deal;
		}
		else if ( printR->rgbColor == PRINT_ASK_COLOR) {
			color = HlpStruct::RtData::CT_Ask_Deal;
		}
		else if ( printR->rgbColor == PRINT_INSIDE_COLOR) {
			color = HlpStruct::RtData::CT_Neutral;
		}
		else 
			color = HlpStruct::RtData::CT_NA; 

	


		SEND_RT_DATA(
			instance_m->returnAdoptedPrintTime(instance_m->laser_convertToCurrentUnixDateTime(instance_m->laser_convertToUnixTime(&printR->gtime)), printR->szStock),
			instance_m->providerForRT_m,
			printR->szStock,	
			printR->dblPrice,	
			printR->dblPrice,	
			printR->dwShares,
			color 
		);

		// now we have to delegate this call to broker level to check delayed orders
	
	}
	LCATCH(OnGotQuotePrint)
	return 0;


}


int WindowThreadProc::OnGotQuotePrintHistory(LPGTSESSION hSession,  const GTPrint32 *printR)
{
	
	return 0;
}

int WindowThreadProc::OnGotQuotePrintRefresh(LPGTSESSION hSession,  const GTPrint32 *printR)
{
	
	return 0;
}

int WindowThreadProc::OnGotQuotePrintDisplay(LPGTSESSION hSession,  const GTPrint32 *printR)
{

	return 0;
}

int WindowThreadProc::OnGotQuoteText(LPGTSESSION hSession,  const GTQuoteText32 * textR)
{
	
	return 0;
}



int WindowThreadProc::OnGotQuoteDisconnected(LPGTSESSION hSession)
{
	LTRY {
		SEND_RT_PROVIDER_SYNC(CppUtils::getTime(), instance_m->providerForRT_m, HlpStruct::RtData::SYNC_EVENT_PROVIDER_TRANSMIT_ERROR, "Quote disconnected" );

		
		

		instance_m->serversSettings_m.loopQuoteEntries();
		LaserApiSettings::ServerEntry se2 = instance_m->serversSettings_m.getCurrentQuoteEntry().getEntry(DEFAULT_QUOTE_SERVER_ENTRY);
		LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERWINPROC, "Quote server disconnected, trying to reconnect to: " << se2.L1_address_m.toString() );
		setQuoteConnectStatus(false, se2.L1_address_m.toString(), se2.L1_address_m.getId());

		instance_m->laser_set_address_quote(se2.L1_address_m.getIpAddress(), se2.L1_address_m.getPort());
		
	}
	LCATCH(OnGotQuoteDisconnected)
	return 0;
}

int WindowThreadProc::OnGotQuoteConnected(LPGTSESSION hSession)
{
	LTRY {
		
		SEND_RT_PROVIDER_SYNC(CppUtils::getTime(), instance_m->providerForRT_m, HlpStruct::RtData::SYNC_EVENT_PROVIDER_START, "Quote connected" );

		LaserApiSettings::ServerEntry se = instance_m->serversSettings_m.getCurrentQuoteEntry().getEntry(DEFAULT_QUOTE_SERVER_ENTRY);
		LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERWINPROC, "Quote server connected to: " << se.L1_address_m.toString()  );
		
		setQuoteConnectStatus(true, se.L1_address_m.toString(), se.L1_address_m.getId());
		

					
		DELEGATOR(OnGotQuoteConnected(hSession));
		
		
	}
	LCATCH(OnGotQuoteConnected)
	return 0;
}

int WindowThreadProc::OnGotLevel2Connected(LPGTSESSION hSession)
{
	LTRY {
		
		
		LaserApiSettings::ServerEntry  se = instance_m->serversSettings_m.getCurrentQuoteEntry().getEntry(DEFAULT_QUOTE_SERVER_ENTRY);
		setLevel2ConnectStatus(true, se.L2_address_m.toString(), se.L2_address_m.getId());
		LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERWINPROC, "Level 2 server connected to: " << se.L2_address_m.toString()  );
		
		
	}
	LCATCH(OnGotLevel2Connected)

	return 0;
}

int WindowThreadProc::OnGotLevel2Disconnected(LPGTSESSION hSession)
{
	LTRY {
	
		

		if (instance_m->serversSettings_m.isConnectLevel2()) {
			instance_m->serversSettings_m.loopQuoteEntries();
			LaserApiSettings::ServerEntry se2 = instance_m->serversSettings_m.getCurrentQuoteEntry().getEntry(DEFAULT_QUOTE_SERVER_ENTRY);

			LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERWINPROC, "Level 2 server disconnected, trying to reconnect to: " << se2.L2_address_m.toString() );
			instance_m->laser_set_address_level2(se2.L2_address_m.getIpAddress(), se2.L2_address_m.getPort());
			setLevel2ConnectStatus(false, se2.L2_address_m.toString(), se2.L2_address_m.getId());
			
		} else {
			LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERWINPROC, "Level 2 server disconnected, ignoring...");
		}
		
	}
	LCATCH(OnGotLevel2Disconnected)

	
	return 0;
}

int WindowThreadProc::OnExecMsgErrMsg(LPGTSESSION hSession, const GTErrMsg32 *errorMsg)
{
	LTRY {
		LOGEVENT(HlpStruct::CommonLog::LL_ERROR, LASERWINPROC, "Executor error message: " << errorMsg->szText << " executor address: " <<  instance_m->serversSettings_m.getCurrentExecutor().toString() );
		instance_m->signal_logged_subscribed();

		SEND_RT_PROVIDER_SYNC(CppUtils::getTime(), instance_m->providerForRT_m, HlpStruct::RtData::SYNC_EVENT_PROVIDER_TRANSMIT_ERROR, "Executor disconnected" );
		
		DELEGATOR(OnExecMsgErrMsg(hSession,errorMsg));
	}
	LCATCH(OnExecMsgErrMsg)
	
	return 0; 
}

// chart

int WindowThreadProc::OnGotChartDisconnected(LPGTSESSION hSession)
{
	LTRY {
		
		
	

		if (instance_m->serversSettings_m.isConnectChart()) {
			instance_m->serversSettings_m.loopChartServers();
			ServerAddress saddr = instance_m->serversSettings_m.getCurrentChartServer();

			LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERWINPROC, "Chart server disconnected, trying to reconnect to: " << saddr.toString()  );
			instance_m->laser_set_address_chart(saddr.getIpAddress(), saddr.getPort());
			setChartConnectStatus(false,saddr.toString(), saddr.getId());
		}
		else {
			LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERWINPROC, "Chart server disconnected, ignoring..."  );
		}
		
	}
	LCATCH(OnGotChartDisconnected)
	return 0; 
}

int WindowThreadProc::OnGotChartConnected(LPGTSESSION hSession)
{
	LTRY {

		
		LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERWINPROC, "Chart server connected to: " << instance_m->serversSettings_m.getCurrentChartServer().toString()  );
	
		setChartConnectStatus(true,instance_m->serversSettings_m.getCurrentChartServer().toString(), instance_m->serversSettings_m.getCurrentChartServer().getId());

	}
	LCATCH(OnGotChartConnected)
	return 0; 
}

int WindowThreadProc::OnGotChartRecord(LPGTSESSION hSession, const GTChart32 *gtchart)
{
	
	return 0; 
}

int WindowThreadProc::OnGotChartRecordHistory(LPGTSESSION hSession, const GTChart32 *gtchart)
{
	
	return 0; 
}

int WindowThreadProc::OnGotChartRecordRefresh(LPGTSESSION hSession, const GTChart32 *gtchart)
{
	
	return 0; 
}

int WindowThreadProc::OnGotChartRecordDisplay(LPGTSESSION hSession, const GTChart32 *gtchart)
{
	
	return 0; 
}


// stock
int WindowThreadProc::OnExecMsgIOIStatus_Stock(LPGTSTOCK hStock, const GTIOIStatus32 *ioStatus)
{
	DELEGATOR(OnExecMsgIOIStatus_Stock(hStock,ioStatus));
	return 0; 
}

int WindowThreadProc::OnExecCmd_Stock(LPGTSTOCK hStock, const LPCSTR *execCmd)
{
	DELEGATOR(OnExecCmd_Stock(hStock,execCmd));
	return 0;
}

int WindowThreadProc::OnExecMsgSending_Stock( LPGTSTOCK hStock, const GTSending32 *sending)
{
	DELEGATOR(OnExecMsgSending_Stock(hStock,sending));
	return 0;
}

int WindowThreadProc::PrePlaceOrder_Stock( LPGTSTOCK hStock,const GTOrder32 *order)
{
	DELEGATOR(PrePlaceOrder_Stock(hStock,order));
	return 0;
}
	
int WindowThreadProc::PostPlaceOrder_Stock( LPGTSTOCK hStock,const GTOrder32 *order)
{
	DELEGATOR(PostPlaceOrder_Stock(hStock,order));
	return 0;
}


int WindowThreadProc::OnTick(LPGTSESSION hSession)
{
	LTRY {
		setNumberOfTickers(instance_m->tickersNumber_m );
	}
	LCATCH(OnTick)
	
		
	return 0; 
}
  
// ------------------------

void WindowThreadProc::onInitDialog(HWND hDlg)
{
	

	try {
		set_startup_ok(false);

		if(!hSession_m) {
		
			// pass this pointer
			//hSession_m = gtCreateSessionWindow(hDlg, IDD_DIALOGMAIN);
			hSession_m = gtSubClassSessionWindow(hDlg, IDC_CUSTOM_CAPTION);

			
			if (hSession_m==NULL)
				THROW(LaserFailed, "exc_GtbCreateSessionWindow", "ctx_LaserInit", "");

			//LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERWINPROC, "gtCreateSessionWindow() - OK");

			
			
			// secondly set up callbacks

			if (gtSetSessionCallBack(getGTBSession(), GTAPI_MSG_SESSION_OnExecMsgLogin, (FARPROC)OnExecMsgLogin) != 0) 
				THROW(LaserFailed, "exc_CantSetSessionCallback", "ctx_LaserInit", "GTAPI_MSG_SESSION_OnExecMsgLogin");

			if (gtSetSessionCallBack(getGTBSession(), GTAPI_MSG_SESSION_OnExecConnected, (FARPROC)OnExecConnected) != 0) 
				THROW(LaserFailed, "exc_CantSetSessionCallback", "ctx_LaserInit", "GTAPI_MSG_SESSION_OnExecConnected");

			if (gtSetSessionCallBack(getGTBSession(), GTAPI_MSG_SESSION_OnExecDisconnected, (FARPROC)OnExecDisconnected) != 0) 
				THROW(LaserFailed, "exc_CantSetSessionCallback", "ctx_LaserInit", "GTAPI_MSG_SESSION_OnExecDisconnected");

			if (gtSetSessionCallBack(getGTBSession(), GTAPI_MSG_SESSION_OnExecMsgLoggedin, (FARPROC)OnExecMsgLoggedin) != 0) 
				THROW(LaserFailed, "exc_CantSetSessionCallback", "ctx_LaserInit", "GTAPI_MSG_SESSION_OnExecMsgLoggedin");

			//
			if (gtSetSessionCallBack(getGTBSession(), GTAPI_MSG_SESSION_OnExecMsgLogout, (FARPROC)OnExecMsgLogout) != 0) 
				THROW(LaserFailed, "exc_CantSetSessionCallback", "ctx_LaserInit", "GTAPI_MSG_SESSION_OnExecMsgLogout");

		
			if (gtSetSessionCallBack(getGTBSession(), GTAPI_MSG_SESSION_OnExecMsgOpenPosition, (FARPROC)OnExecMsgOpenPosition) != 0) 
				THROW(LaserFailed, "exc_CantSetSessionCallback", "ctx_LaserInit", "GTAPI_MSG_SESSION_OnExecMsgOpenPosition");

			if (gtSetSessionCallBack(getGTBSession(), GTAPI_MSG_SESSION_OnExecMsgTrade, (FARPROC)OnExecMsgTrade) != 0) 
				THROW(LaserFailed, "exc_CantSetSessionCallback", "ctx_LaserInit", "GTAPI_MSG_SESSION_OnExecMsgTrade");

			//

			if (gtSetSessionCallBack(getGTBSession(), GTAPI_MSG_SESSION_OnExecMsgAccount, (FARPROC)OnExecMsgAccount) != 0) 
				THROW(LaserFailed, "exc_CantSetSessionCallback", "ctx_LaserInit", "GTAPI_MSG_SESSION_OnExecMsgAccount");

			if (gtSetSessionCallBack(getGTBSession(), GTAPI_MSG_SESSION_OnExecMsgPending, (FARPROC)OnExecMsgPending) != 0) 
				THROW(LaserFailed, "exc_CantSetSessionCallback", "ctx_LaserInit", "GTAPI_MSG_SESSION_OnExecMsgPending");

			if (gtSetSessionCallBack(getGTBSession(), GTAPI_MSG_SESSION_OnExecMsgSending, (FARPROC)OnExecMsgSending) != 0) 
				THROW(LaserFailed, "exc_CantSetSessionCallback", "ctx_LaserInit", "GTAPI_MSG_SESSION_OnExecMsgSending");

			if (gtSetSessionCallBack(getGTBSession(), GTAPI_MSG_SESSION_OnExecMsgCanceling, (FARPROC)OnExecMsgCanceling) != 0) 
				THROW(LaserFailed, "exc_CantSetSessionCallback", "ctx_LaserInit", "GTAPI_MSG_SESSION_OnExecMsgCanceling");

			if (gtSetSessionCallBack(getGTBSession(), GTAPI_MSG_SESSION_OnExecMsgCancel, (FARPROC)OnExecMsgCancel) != 0) 
				THROW(LaserFailed, "exc_CantSetSessionCallback", "ctx_LaserInit", "GTAPI_MSG_SESSION_OnExecMsgCancel");

			if (gtSetSessionCallBack(getGTBSession(), GTAPI_MSG_SESSION_OnExecMsgReject, (FARPROC)OnExecMsgReject) != 0) 
				THROW(LaserFailed, "exc_CantSetSessionCallback", "ctx_LaserInit", "GTAPI_MSG_SESSION_OnExecMsgReject");

			if (gtSetSessionCallBack(getGTBSession(), GTAPI_MSG_SESSION_OnExecMsgRemove, (FARPROC)OnExecMsgRemove) != 0) 
				THROW(LaserFailed, "exc_CantSetSessionCallback", "ctx_LaserInit", "GTAPI_MSG_SESSION_OnExecMsgRemove");

			if (gtSetSessionCallBack(getGTBSession(), GTAPI_MSG_SESSION_OnExecMsgRejectCancel, (FARPROC)OnExecMsgRejectCancel) != 0) 
				THROW(LaserFailed, "exc_CantSetSessionCallback", "ctx_LaserInit", "GTAPI_MSG_SESSION_OnExecMsgRejectCancel");

			if (gtSetSessionCallBack(getGTBSession(), GTAPI_MSG_SESSION_OnExecMsgStatus, (FARPROC)OnExecMsgStatus) != 0) 
				THROW(LaserFailed, "exc_CantSetSessionCallback", "ctx_LaserInit", "GTAPI_MSG_SESSION_OnExecMsgStatus");

			if (gtSetSessionCallBack(getGTBSession(), GTAPI_MSG_SESSION_OnExecMsgState, (FARPROC)OnExecMsgState) != 0) 
				THROW(LaserFailed, "exc_CantSetSessionCallback", "ctx_LaserInit", "GTAPI_MSG_SESSION_OnExecMsgState");

			// level 1 - on demand
		
			//if (gtSetSessionCallBack(getGTBSession(), GTAPI_MSG_SESSION_OnGotQuoteLevel1, (FARPROC)OnGotQuoteLevel1) != 0) 
			//	THROW(LaserFailed, "exc_CantSetSessionCallback", "ctx_LaserInit", "GTAPI_MSG_SESSION_OnGotQuoteLevel1");
			

			if (gtSetSessionCallBack(getGTBSession(), GTAPI_MSG_SESSION_OnGotQuoteConnected, (FARPROC)OnGotQuoteConnected) != 0) 
				THROW(LaserFailed, "exc_CantSetSessionCallback", "ctx_LaserInit", "GTAPI_MSG_SESSION_OnGotQuoteConnected");

			if (gtSetSessionCallBack(getGTBSession(), GTAPI_MSG_SESSION_OnGotQuoteDisconnected, (FARPROC)OnGotQuoteDisconnected) != 0) 
				THROW(LaserFailed, "exc_CantSetSessionCallback", "ctx_LaserInit", "GTAPI_MSG_SESSION_OnGotQuoteDisconnected");

			// level2
			if (gtSetSessionCallBack(getGTBSession(), GTAPI_MSG_SESSION_OnGotLevel2Connected, (FARPROC)OnGotLevel2Connected) != 0) 
				THROW(LaserFailed, "exc_CantSetSessionCallback", "ctx_LaserInit", "GTAPI_MSG_SESSION_OnGotLevel2Connected");

			if (gtSetSessionCallBack(getGTBSession(), GTAPI_MSG_SESSION_OnGotLevel2Disconnected, (FARPROC)OnGotLevel2Disconnected) != 0) 
				THROW(LaserFailed, "exc_CantSetSessionCallback", "ctx_LaserInit", "GTAPI_MSG_SESSION_OnGotLevel2Disconnected");


			// print
			if (gtSetSessionCallBack(getGTBSession(), GTAPI_MSG_SESSION_OnGotQuotePrint, (FARPROC)OnGotQuotePrint) != 0) 
				THROW(LaserFailed, "exc_CantSetSessionCallback", "ctx_LaserInit", "GTAPI_MSG_SESSION_OnGotQuotePrint");

			if (gtSetSessionCallBack(getGTBSession(), GTAPI_MSG_SESSION_OnGotQuotePrintHistory, (FARPROC)OnGotQuotePrintHistory) != 0) 
				THROW(LaserFailed, "exc_CantSetSessionCallback", "ctx_LaserInit", "GTAPI_MSG_SESSION_OnGotQuotePrintHistory");

			if (gtSetSessionCallBack(getGTBSession(), GTAPI_MSG_SESSION_OnGotQuotePrintRefresh, (FARPROC)OnGotQuotePrintRefresh) != 0) 
				THROW(LaserFailed, "exc_CantSetSessionCallback", "ctx_LaserInit", "GTAPI_MSG_SESSION_OnGotQuotePrintRefresh");

			if (gtSetSessionCallBack(getGTBSession(), GTAPI_MSG_SESSION_OnGotQuotePrintDisplay, (FARPROC)OnGotQuotePrintDisplay) != 0) 
				THROW(LaserFailed, "exc_CantSetSessionCallback", "ctx_LaserInit", "GTAPI_MSG_SESSION_OnGotQuotePrintDisplay");

			if (gtSetSessionCallBack(getGTBSession(), GTAPI_MSG_SESSION_OnGotQuoteText, (FARPROC)OnGotQuoteText) != 0) 
				THROW(LaserFailed, "exc_CantSetSessionCallback", "ctx_LaserInit", "GTAPI_MSG_SESSION_OnGotQuoteText");



			// error
			if (gtSetSessionCallBack(getGTBSession(), GTAPI_MSG_SESSION_OnExecMsgErrMsg, (FARPROC)OnExecMsgErrMsg) != 0) 
				THROW(LaserFailed, "exc_CantSetSessionCallback", "ctx_LaserInit", "GTAPI_MSG_SESSION_OnExecMsgErrMsg");

			// chart
			
			if (gtSetSessionCallBack(getGTBSession(), GTAPI_MSG_SESSION_OnGotChartConnected, (FARPROC)OnGotChartConnected) != 0) 
				THROW(LaserFailed, "exc_CantSetSessionCallback", "ctx_LaserInit", "GTAPI_MSG_SESSION_OnGotChartConnected");

			if (gtSetSessionCallBack(getGTBSession(), GTAPI_MSG_SESSION_OnGotChartDisconnected, (FARPROC)OnGotChartDisconnected) != 0) 
				THROW(LaserFailed, "exc_CantSetSessionCallback", "ctx_LaserInit", "GTAPI_MSG_SESSION_OnGotChartDisconnected");

			if (gtSetSessionCallBack(getGTBSession(), GTAPI_MSG_SESSION_OnGotChartRecord, (FARPROC)OnGotChartRecord) != 0) 
				THROW(LaserFailed, "exc_CantSetSessionCallback", "ctx_LaserInit", "GTAPI_MSG_SESSION_OnGotChartRecord");

			if (gtSetSessionCallBack(getGTBSession(), GTAPI_MSG_SESSION_OnGotChartRecordHistory, (FARPROC)OnGotChartRecordHistory) != 0) 
				THROW(LaserFailed, "exc_CantSetSessionCallback", "ctx_LaserInit", "GTAPI_MSG_SESSION_OnGotChartRecordHistory");

			if (gtSetSessionCallBack(getGTBSession(), GTAPI_MSG_SESSION_OnGotChartRecordRefresh, (FARPROC)OnGotChartRecordRefresh) != 0) 
				THROW(LaserFailed, "exc_CantSetSessionCallback", "ctx_LaserInit", "GTAPI_MSG_SESSION_OnGotChartRecordRefresh");

			if (gtSetSessionCallBack(getGTBSession(), GTAPI_MSG_SESSION_OnGotChartRecordDisplay, (FARPROC)OnGotChartRecordDisplay) != 0) 
				THROW(LaserFailed, "exc_CantSetSessionCallback", "ctx_LaserInit", "GTAPI_MSG_SESSION_OnGotChartRecordDisplay");


			// on tick
			win_m = hDlg;
			  
			if (gtSetSessionCallBack(getGTBSession(), GTAPI_MSG_SESSION_OnTick, (FARPROC)OnTick) != 0) 
				THROW(LaserFailed, "exc_CantSetSessionCallback", "ctx_LaserInit", "GTAPI_MSG_SESSION_OnTick");
				

			//
			LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERWINPROC, "Performed initial setup");

			// raise mutex
			
			setReadyToOperate();
			setApiVersionNumber(hDlg);
			
			setExecutorConnectStatus(false, NOT_AVAILABLE, NOT_AVAILABLE);
			setQuoteConnectStatus(false, NOT_AVAILABLE, NOT_AVAILABLE );
			setChartConnectStatus(false, NOT_AVAILABLE, NOT_AVAILABLE);
			setLevel2ConnectStatus(false, NOT_AVAILABLE, NOT_AVAILABLE);

			setSubscribedSymbolsNumber(0);
			setLevel1SubscriptionStatus(false);
			setEnableLooping(true);
			
			
		}  
	}
	catch(CppUtils::Exception& e)
	{
		LOG(MSG_ERROR, LASERWINPROC, "Exception in onInitDialog(...) : " << e.message());
		LOGEVENT(HlpStruct::CommonLog::LL_ERROR, LASERWINPROC, "Exception in onInitDialog(...) : " << e.message());

	}
	catch(...) {
		LOG(MSG_ERROR, LASERWINPROC, "Unknown exception in onInitDialog(...) ");
		LOGEVENT(HlpStruct::CommonLog::LL_ERROR, LASERWINPROC, "Unknown exception in onInitDialog(...) ");

	}
	
}

void WindowThreadProc::onDestroyDialog(HWND hDlg)
{
	try {
		if(hSession_m) {

			
			gtLogout(getGTBSession());

			// disconnect
			gtDisconnectQuote(getGTBSession(), TRUE);
			gtDisconnectLevel2(getGTBSession(), TRUE);
			gtDisconnectChart(getGTBSession(), TRUE);
			

			//gtDestroySessionWindow(getGTBSession());
			gtUnsubClassSessionWindow( getGTBSession() );

			hSession_m = NULL;
			win_m = 0;

			// set up finished
			setReadyFinished();

			LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERWINPROC, "Performed shutdown");
		}
	}
	catch(CppUtils::Exception& e)
	{
		LOG(MSG_ERROR, LASERWINPROC, "Exception in onDestroyDialog(...) : " << e.message());
		LOGEVENT(HlpStruct::CommonLog::LL_ERROR, LASERWINPROC, "Exception in onDestroyDialog(...) : " << e.message());

	}
	catch(...) {
		LOG(MSG_ERROR, LASERWINPROC, "Unknown exception in onDestroyDialog(...) ");
		LOGEVENT(HlpStruct::CommonLog::LL_ERROR, LASERWINPROC, "Unknown exception in onDestroyDialog(...) ");

	}

	
	
}


void WindowThreadProc::onStartup()
{
	try {

		set_startup_ok( false );
		laser_set_address_exec( serversSettings_m.getCurrentExecutor().getIpAddress(), serversSettings_m.getCurrentExecutor().getPort());
		
		
		
		LaserApiSettings::ServerEntry se = serversSettings_m.getCurrentQuoteEntry().getEntry(DEFAULT_QUOTE_SERVER_ENTRY);

		if (serversSettings_m.isConnectLevel1()) {
			laser_set_address_quote(se.L1_address_m.getIpAddress(), se.L1_address_m.getPort());
			
		}

		
		if (serversSettings_m.isConnectLevel2()) {
			laser_set_address_level2(se.L2_address_m.getIpAddress(), se.L2_address_m.getPort());
			
		}

		
		if (!serversSettings_m.isPrintsOnly()) {
			// setup function to receive level 1
			subscribe_level1_explicit();
		}

		
		if (serversSettings_m.isConnectChart()) {
			laser_set_address_chart(serversSettings_m.getCurrentChartServer().getIpAddress(), serversSettings_m.getCurrentChartServer().getPort());
		
		}

		
		initialize_symbol_list(serversSettings_m.getSymbolList(), serversSettings_m.getProviderForRT());
		
		setExecutorConnectStatus(false, serversSettings_m.getCurrentExecutor().toString(), serversSettings_m.getCurrentExecutor().getId());
		setQuoteConnectStatus(false, serversSettings_m.getCurrentQuoteEntry().getEntry(DEFAULT_QUOTE_SERVER_ENTRY).L1_address_m.toString(), serversSettings_m.getCurrentQuoteEntry().getEntry(DEFAULT_QUOTE_SERVER_ENTRY).L1_address_m.getId() );
		setChartConnectStatus(false, serversSettings_m.getCurrentChartServer().toString(), serversSettings_m.getCurrentChartServer().getId());
		setLevel2ConnectStatus(false, serversSettings_m.getCurrentQuoteEntry().getEntry(DEFAULT_QUOTE_SERVER_ENTRY).L2_address_m.toString(), serversSettings_m.getCurrentQuoteEntry().getEntry(DEFAULT_QUOTE_SERVER_ENTRY).L2_address_m.getId());

		
		
		// eventually login
		laser_login(serversSettings_m.getUser(), serversSettings_m.getPassword());

		
		LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERWINPROC, "Performed startup");

	}
	catch(CppUtils::Exception& e)
	{
		LOG(MSG_ERROR, LASERWINPROC, "Exception in onStartup(...) : " << e.message());
		LOGEVENT(HlpStruct::CommonLog::LL_ERROR, LASERWINPROC, "Exception in onStartup(...):  " << e.message());

		signal_logged_subscribed();

	}
	catch(...) {
		LOG(MSG_ERROR, LASERWINPROC, "Unknown exception in onStartup(...) ");
		LOGEVENT(HlpStruct::CommonLog::LL_ERROR, LASERWINPROC, "Unknown exception in onStartup(...)");

		signal_logged_subscribed();

	}

}

void WindowThreadProc::onReconnectClick()
{
	try {
		instance_m->laser_force_reconnect();
	}
	catch(CppUtils::Exception& e)
	{
		LOG(MSG_ERROR, LASERWINPROC, "Exception in onReconnectClick(...) : " << e.message());
		LOGEVENT(HlpStruct::CommonLog::LL_ERROR, LASERWINPROC, "Exception in onReconnectClick(...):  " << e.message());
	}
	catch(...) {
		LOG(MSG_ERROR, LASERWINPROC, "Unknown exception in onReconnectClick(...) ");
		LOGEVENT(HlpStruct::CommonLog::LL_ERROR, LASERWINPROC, "Unknown exception in onReconnectClick(...)");
	}
}

// ---------------------------------------
LRESULT WindowThreadProc::testDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	WindowThreadProc* pThis = (WindowThreadProc*)lParam;

	
	if ( message == UWM_STARTUP_LASER ) {
			// perform some initialization
			pThis->onStartup();

			return TRUE;
	}
	else if	( message == UWM_STOP_DLG ) {
			// deinitialize
			pThis->onDestroyDialog(hDlg);

			// finish dialog
			::EndDialog(hDlg,0);
			return TRUE;
	} else if ( message == WM_INITDIALOG ) {
			pThis->onInitDialog(hDlg);
			return TRUE;
	} else if (message == WM_COMMAND) {
		if( LOWORD(wParam) == IDC_BUTTON_RECONNECT) {
      onReconnectClick();
			return TRUE;
		}
		else if( LOWORD(wParam) == IDC_LOOP_ENTRIES_CHECK) {
     
			UINT res = ::IsDlgButtonChecked(hDlg, IDC_LOOP_ENTRIES_CHECK);
			instance_m->serversSettings_m.enableLoopEntries(res == BST_CHECKED);
			

			return TRUE;
		}
	}
	
	// default
	return FALSE;
	
}

// ---------------------------------------



void WindowThreadProc::setInstance(HINSTANCE instance)
{
	hinstDll_m = instance;
	UWM_STOP_DLG = ::RegisterWindowMessage(UWM_STOP_DLG_MSG);
	UWM_STARTUP_LASER = ::RegisterWindowMessage(UWM_STARTUP_LASER_MSG);
}


// register and clears callbacks
void WindowThreadProc::registerCallback(LaserBroker& laserBroker)
{
	LOCK_FOR_SCOPE(registerCallbackMtx_m);
	callbackDelegates_m.insert(&laserBroker);
}

void WindowThreadProc::clearCallback(LaserBroker& laserBroker)
{
	LOCK_FOR_SCOPE(registerCallbackMtx_m);
	callbackDelegates_m.erase(&laserBroker);
}

// ---------------------------------------
// laser API


int WindowThreadProc::getRefCount() const
{
	LOCK_FOR_SCOPE(singletonMtx_m);
	return refCnt_m;

}


void WindowThreadProc::laser_check_session()
{
	if (getGTBSession()==NULL)
			THROW(LaserFailed, "exc_InvalidSessionHandle", "ctx_LaserSessionCheck", "");


}

void WindowThreadProc::laser_login(CppUtils::String const& user, CppUtils::String const& password)
{
	LOCK_FOR_SCOPE(singletonMtx_m);
	if (refCnt_m!=1) {
		LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERWINPROC, "Ingnore logging in as ref counter != 1");
		return;
	}

	{
		LOCK_FOR_SCOPE(*this);

		// reset if signalled
		instance_m->reset_logged_subscribed();

		laser_check_session();
		CppUtils::String u_user(user);
		CppUtils::toupper(u_user);
		int res = gtLogin (getGTBSession(), u_user.c_str(), password.c_str());

		if (res != 0)
			THROW(LaserFailed, "exc_InvalidLoginAttempt", "ctx_LaserLogin", "***/***, result: " + CppUtils::long2String(res));

		LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERWINPROC, "Sent connect message :" << "***" << " - " << "***" );
	}
}  
  

void WindowThreadProc::laser_logout()
{
	LOCK_FOR_SCOPE(singletonMtx_m);
	if (refCnt_m!=1) {
		LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERWINPROC, "Ingnore logging out as ref counter != 1");
		return;
	}

	{
		LOCK_FOR_SCOPE(*this);

		laser_check_session();
		gtLogout (getGTBSession());

		LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERWINPROC, "Sent logout message");
	}
}

void WindowThreadProc::laser_set_address_exec(CppUtils::String const& ip, int const port)
{
	LOCK_FOR_SCOPE(*this);

	laser_check_session();

	gtSetExecAddress(getGTBSession(),ip.c_str(), port );
	
}

void WindowThreadProc::laser_set_address_level2( CppUtils::String const& ip, int const port)
{
	LOCK_FOR_SCOPE(*this);

	laser_check_session();

	gtSetLevel2Address (getGTBSession(),ip.c_str(), port );
		
}

void WindowThreadProc::laser_set_address_quote(CppUtils::String const& ip, int const port)
{
	LOCK_FOR_SCOPE(*this);

	laser_check_session();

	gtSetQuoteAddress (getGTBSession(), ip.c_str(), port );
	
}

void WindowThreadProc::laser_set_address_chart(CppUtils::String const& ip, int const port)
{
	LOCK_FOR_SCOPE(*this);

	laser_check_session();
  
	gtSetChartAddress (getGTBSession(), ip.c_str(), port );
	
}

int WindowThreadProc::laser_connect_quotes()
{
	LOCK_FOR_SCOPE(*this);
	laser_check_session();

	int result = gtConnectQuotes(getGTBSession());
	return result;
}

LPGTSTOCK WindowThreadProc::laser_subscribe_symbol( CppUtils::String const& symbol)
{
	//LOCK_FOR_SCOPE(*this);

	laser_check_session();

	LPGTSTOCK stockPtr = NULL;

	stockPtr = gtCreateStock(getGTBSession(), symbol.c_str() );
	if (stockPtr == NULL)
		THROW(LaserFailed, "exc_CantCreateStock", "ctx_LaserCreateStock", symbol);

	
	if (gtSetStockCallBack  ( stockPtr, GTAPI_MSG_STOCK_OnExecMsgIOIStatus,   (FARPROC)OnExecMsgIOIStatus_Stock  )!=0)
		THROW(LaserFailed, "exc_CantSetStackCallBacl", "ctx_LaserCreateStock", "GTAPI_MSG_STOCK_OnExecMsgIOIStatus");

	
	if (gtSetStockCallBack  ( stockPtr, GTAPI_MSG_STOCK_OnExecCmd,   (FARPROC)OnExecCmd_Stock  )!=0)
		THROW(LaserFailed, "exc_CantSetStackCallBacl", "ctx_LaserCreateStock", "GTAPI_MSG_STOCK_OnExecCmd");

 
	/*
	if (gtSetStockCallBack  ( stockPtr, GTAPI_MSG_STOCK_OnExecMsgSending,   (FARPROC)OnExecMsgSending_Stock  )!=0)
		THROW(LaserFailed, "exc_CantSetStackCallBacl", "ctx_LaserCreateStock", "GTAPI_MSG_STOCK_OnExecMsgSending");

 
	if (gtSetStockCallBack  ( stockPtr, GTAPI_MSG_STOCK_PrePlaceOrder,   (FARPROC)PrePlaceOrder_Stock  )!=0)
		THROW(LaserFailed, "exc_CantSetStackCallBacl", "ctx_LaserCreateStock", "GTAPI_MSG_STOCK_PrePlaceOrder");

	if (gtSetStockCallBack  ( stockPtr, GTAPI_MSG_STOCK_PostPlaceOrder,   (FARPROC)PostPlaceOrder_Stock  )!=0)
		THROW(LaserFailed, "exc_CantSetStackCallBacl", "ctx_LaserCreateStock", "GTAPI_MSG_STOCK_PostPlaceOrder");
	*/
   	

	LOGEVENT(HlpStruct::CommonLog::LL_DEBUG, LASERWINPROC, "Subscribed for symbol: " << symbol );

	return stockPtr;
}

LPGTSTOCK WindowThreadProc::laser_get_stock_handler(CppUtils::String const& symbol)
{
	LOCK_FOR_SCOPE(*this);
	laser_check_session();

	LPGTSTOCK stockPtr = gtGetStock(getGTBSession(), symbol.c_str());
	if (stockPtr == NULL)
		THROW(LaserFailed, "exc_CantRetreiveStock", "ctx_LaserGetStock", symbol);

	return stockPtr;
}

LPGTSTOCK WindowThreadProc::laser_get_stock_handler_local(CppUtils::String const& symbol)
{
	LOCK_FOR_SCOPE(*this);
	map<CppUtils::String, LPGTSTOCK>::iterator it = subscribedSymbols_m.find(symbol);
	if (it == subscribedSymbols_m.end())
		return NULL;

	return it->second;
}

int WindowThreadProc::laser_get_print_count(LPGTSTOCK stockH)
{
	LOCK_FOR_SCOPE(*this);
	
	return gtGetPrintCount(stockH);
}

GTStock32* WindowThreadProc::laser_get_stock_structure(LPGTSTOCK stockH)
{
	LOCK_FOR_SCOPE(*this);
	return gtGetStock32(stockH);

}

void WindowThreadProc::laser_unsubscribe_symbol(CppUtils::String const& symbol)
{
	LOCK_FOR_SCOPE(*this);

	laser_check_session();

	gtDeleteStock (getGTBSession(), symbol.c_str());

	LOGEVENT(HlpStruct::CommonLog::LL_DEBUG, LASERWINPROC, "Unsubscribed symbol: " << symbol );
}

void WindowThreadProc::laser_force_reconnect()
{
	LOCK_FOR_SCOPE(*this); 
	if (hSession_m != NULL ) {
		// if connected

		LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERWINPROC, "Force reconnecting" );

		// forec disconnect 
		gtDisconnectQuote(getGTBSession(), FALSE);
		gtDisconnectLevel2(getGTBSession(), FALSE);
		gtDisconnectChart(getGTBSession(), FALSE);

		// then force connect with current addresses
		//gtConnectQuotes(getGTBSession());

	}
}


void WindowThreadProc::laser_request_open_positions(CppUtils::StringList const& symbols, GTOpenPosition32List& openList)
{	

	LOCK_FOR_SCOPE(*this);
	
	laser_check_session();
	openList.clear();

	

	for(int i = 0; i < symbols.size(); i++) {
		GTOpenPosition32* openPosPtr = gtGetStockOpenPosition32(laser_get_stock_handler( symbols[i]));

		if (openPosPtr)
			openList.push_back(*openPosPtr);
		
	};
	
		

}
void WindowThreadProc::laser_request_pending_positions(CppUtils::StringList const& symbols, GTPending32List& pendingList)
{
		LOCK_FOR_SCOPE(*this);
		laser_check_session();
		
		pendingList.clear();

		
		GTPending32 position;
		UINT pos;

		for(int i = 0; i < symbols.size(); i++) {
			LPGTSTOCK stockHndl= laser_get_stock_handler(symbols[i]);

      pos = gtGetStockFirstPending32(stockHndl, &position);
			while(pos){
					
					pendingList.push_back(position);
          pos = gtGetStockNextPending32(stockHndl, pos, &position);
					
      }

		}
		
}

void WindowThreadProc::laser_request_trade_positions(CppUtils::StringList const& symbols, GTTrade32List& tradeList)
{
	LOCK_FOR_SCOPE(*this);
	laser_check_session();

	tradeList.clear();
	GTTrade32 position;
	UINT pos;

	for(int i = 0; i < symbols.size(); i++) {
			LPGTSTOCK stockHndl= laser_get_stock_handler(symbols[i]);
			pos = gtGetStockFirstTrade32(stockHndl, &position);
			
			while(pos!=0){
					tradeList.push_back(position);
          pos = gtGetStockNextTrade32(stockHndl, pos, &position);
					
      }

	};

}


void WindowThreadProc::laser_request_open_positions(GTOpenPosition32List& openList)
{
	LOCK_FOR_SCOPE(*this);
	laser_check_session();

	openList.clear();
	GTOpenPosition32 position;

	UINT pos =  gtGetFirstOpenPosition(getGTBSession(), &position );

	while(pos){
		openList.push_back(position);
		pos = gtGetNextOpenPosition(getGTBSession(), pos, &position );
		
	}

}

void WindowThreadProc::laser_request_pending_positions(GTPending32List& pendingList)
{
	LOCK_FOR_SCOPE(*this);
	laser_check_session();

	pendingList.clear();
	GTPending32 position;

	UINT pos =  gtGetFirstPending (getGTBSession(), &position );

	while(pos){
		pendingList.push_back(position);
		pos = gtGetNextPending (getGTBSession(), pos, &position );
		
	}
}

void WindowThreadProc::laser_request_trade_positions(GTTrade32List& tradeList)
{
	LOCK_FOR_SCOPE(*this);
	laser_check_session();

	tradeList.clear();
	GTTrade32 position;

	UINT pos =  gtGetFirstTrade (getGTBSession(), &position );

	while(pos){
		tradeList.push_back(position);
		pos = gtGetNextTrade  (getGTBSession(), pos, &position );

	}
}


void WindowThreadProc::laser_place_order( LPGTSTOCK stockH, GTOrder32& order )
{
	LOCK_FOR_SCOPE(singletonMtx_m);
	laser_check_session();
	
	int res = gtPlaceOrder( stockH, &order );  
	if (res != 0) {
		THROW(LaserFailed, "exc_CantPlaceOrder", "ctx_PlaceOrder", "");
	}

	
}

void WindowThreadProc::laser_init_order(LPGTSTOCK stockH,	GTOrder32& order)
{
	LOCK_FOR_SCOPE(singletonMtx_m);
	laser_check_session();

	gtInitOrder(stockH, &order);

}

void WindowThreadProc::laser_buy(LPGTSTOCK stockH,int const nShares, double const dblPrice, MMID const method )
{
	LOCK_FOR_SCOPE(singletonMtx_m);
	laser_check_session();

	int res =  gtBuy  (stockH,nShares,dblPrice,method	) ;
	if (res !=0)
		THROW(LaserFailed, "exc_CantBuyOrder", "ctx_LaserBuy", "");



}
		
void WindowThreadProc::laser_bid(LPGTSTOCK stockH,int const nShares, double const dblPrice, MMID const method )
{
	LOCK_FOR_SCOPE(singletonMtx_m);
	laser_check_session();

	int res =  gtBid  (stockH,nShares,dblPrice,method	);
	if (res !=0)
		THROW(LaserFailed, "exc_CantBidOrder", "ctx_LaserBid", "");



}

void WindowThreadProc::laser_sell(LPGTSTOCK stockH,int const nShares, double const dblPrice, MMID const method )
{
	LOCK_FOR_SCOPE(singletonMtx_m);
	laser_check_session();

	int res =  gtSell  (stockH,nShares,dblPrice,method	);
	if (res !=0)
		THROW(LaserFailed, "exc_CantSellOrder", "ctx_LaserSell", "");



}
		
void WindowThreadProc::laser_ask(LPGTSTOCK stockH,int const nShares, double const dblPrice, MMID const method )
{
	LOCK_FOR_SCOPE(singletonMtx_m);
	laser_check_session();

	int res =  gtAsk  (stockH,nShares,dblPrice,method	);
	if (res !=0)
		THROW(LaserFailed, "exc_CantAskOrder", "ctx_LaserAsk", "");

}


// --------------

void WindowThreadProc::laser_fill_buy(LPGTSTOCK stockH,int const nShares, double const dblPrice, MMID const method, GTOrder32& order )
{
	LOCK_FOR_SCOPE(singletonMtx_m);
	laser_check_session();

	int res = gtOrderBuy (stockH, &order, nShares, dblPrice, method);

	if (res !=0)
		THROW(LaserFailed, "exc_CantFillBuyOrder", "ctx_LaserFillOrder", "");


}
		
void WindowThreadProc::laser_fill_bid(LPGTSTOCK stockH,int const nShares, double const dblPrice, MMID const method, GTOrder32& order )
{
	LOCK_FOR_SCOPE(singletonMtx_m);
	laser_check_session();

	int res = gtOrderBid (stockH, &order, nShares, dblPrice, method);

	if (res !=0)
		THROW(LaserFailed, "exc_CantFillBidOrder", "ctx_LaserFillOrder", "");
}

void WindowThreadProc::laser_fill_sell(LPGTSTOCK stockH,int const nShares, double const dblPrice, MMID const method, GTOrder32& order )
{
	LOCK_FOR_SCOPE(singletonMtx_m);
	laser_check_session();

	int res = gtOrderSell (stockH, &order, nShares, dblPrice, method);

	if (res !=0)
		THROW(LaserFailed, "exc_CantFillSellOrder", "ctx_LaserFillOrder", "");
}
		
void WindowThreadProc::laser_fill_ask(LPGTSTOCK stockH,int const nShares, double const dblPrice, MMID const method, GTOrder32& order )
{
	LOCK_FOR_SCOPE(singletonMtx_m);
	laser_check_session();

	int res = gtOrderAsk (stockH, &order, nShares, dblPrice, method);

	if (res !=0)
		THROW(LaserFailed, "exc_CantFillAskOrder", "ctx_LaserFillOrder", "");
}

// --------------
double WindowThreadProc::laser_getBestAskPrice(LPGTSTOCK stockH)
{
	LOCK_FOR_SCOPE(singletonMtx_m);
	laser_check_session();

	return gtGetBestAskLevel2Price(stockH);

}

double WindowThreadProc::laser_getBestBidPrice(LPGTSTOCK stockH)
{
	LOCK_FOR_SCOPE(singletonMtx_m);
	laser_check_session();

	return gtGetBestBidLevel2Price(stockH);
}


void WindowThreadProc::laser_cancel_order_ticket( long const ticketNo)
{
	LOCK_FOR_SCOPE(singletonMtx_m);
	laser_check_session();
	
	int res = gtCancelTicket( getGTBSession(), ticketNo);
	if (res !=0)
		THROW(LaserFailed, "exc_CantCancelOrder", "ctx_LaserCancelTicket", ticketNo);

	
 
}

void WindowThreadProc::laser_cancel_order_all( )
{
	LOCK_FOR_SCOPE(singletonMtx_m);
	laser_check_session();
	if (gtCancelAll ( getGTBSession() ) !=0) 
		THROW(LaserFailed, "exc_CantCancelOrder", "ctx_LaserCancelAll", "");
	
		
} 

void WindowThreadProc::laser_cancel_order(LPGTSTOCK stockH )
{
	LOCK_FOR_SCOPE(singletonMtx_m);
	laser_check_session();

	int res=gtCancelStockAll(stockH);

	if (res !=0)
		THROW(LaserFailed, "exc_CantCancelOrderForStock", "ctx_LaserCancelForStock", "");

}

void WindowThreadProc::laser_init_trade_sequence(long const seqId)
{
		LOCK_FOR_SCOPE(singletonMtx_m);
		laser_check_session();
		gtSetNextTraderSeqNo (getGTBSession(), seqId,1);

}

// ----------------------

GTime32* WindowThreadProc::laser_get_system_time()
{
	LOCK_FOR_SCOPE(singletonMtx_m);
	laser_check_session();
	return (GTime32*)gtGetSystemTime32(getGTBSession());
}

double WindowThreadProc::laser_convertToUnixTime(const GTime32* gtime )
{
	return (gtime->chHour * 60 * 60 + gtime->chMin * 60 + gtime->chSec);

}

double WindowThreadProc::laser_convertToCurrentUnixDateTime( double const secs )
{
	
	double result = CppUtils::roundToBeginningOfTheDay(CppUtils::getTime()) + secs;
	return result;
	
}

MMID WindowThreadProc::laser_createMMIDFromString(CppUtils::String const& mmidStr)
{
	return makemmid(mmidStr.c_str());
}

CppUtils::String WindowThreadProc::laser_MMID2String(MMID const mmid)
{
	char buf[64];
	copymmid(buf, mmid);

	return buf;
}

CppUtils::String WindowThreadProc::laser_orderPriceIndicator2String(char const priceIndic)
{
	
	switch(priceIndic) {
		case PRICE_INDICATOR_MARKET: return "PRICE_INDICATOR_MARKET";
		case PRICE_INDICATOR_LIMIT: return "PRICE_INDICATOR_LIMIT";
		case PRICE_INDICATOR_STOP: return "PRICE_INDICATOR_STOP";
		case PRICE_INDICATOR_STOPLIMIT: return "PRICE_INDICATOR_STOPLIMIT";
		case PRICE_INDICATOR_LIMIT_OR_BETTER: return "PRICE_INDICATOR_LIMIT_OR_BETTER";
		case PRICE_INDICATOR_PEGGED: return "PRICE_INDICATOR_PEGGED";
		case PRICE_INDICATOR_MARKET_ON_OPEN: return "PRICE_INDICATOR_MARKET_ON_OPEN";
		case PRICE_INDICATOR_MARKET_ON_CLOSE: return "PRICE_INDICATOR_MARKET_ON_CLOSE";
		case PRICE_INDICATOR_LIMIT_ON_OPEN: return "PRICE_INDICATOR_LIMIT_ON_OPEN";
		case PRICE_INDICATOR_LIMIT_ON_CLOSE: return "PRICE_INDICATOR_LIMIT_ON_CLOSE";
		case PRICE_INDICATOR_IO_OPEN: return "PRICE_INDICATOR_IO_OPEN";
		case PRICE_INDICATOR_IO_CLOSE: return "PRICE_INDICATOR_IO_CLOSE";
		default:
			return "UNKNOWN PRICE INDICATOR";
	}
}

CppUtils::String WindowThreadProc::laser_getErrorMesage(int nCode )
{
	
	char buf[128];
	LPCSTR out = gtGetErrorMessage  ( nCode,  buf  );
	return (out ? out : "");
   

}

double WindowThreadProc::laser_convertLaserTime(int const laserTime)
{
	if (laserTime <=0)
		return -1;

	// 133416
	int hours = int(laserTime / 10000);
	int minutes = int((laserTime-hours*10000) / 100);
	int seconds = laserTime - hours*10000 - minutes * 100;


	return (hours * 60 * 60 + minutes * 60 + seconds);

}

double WindowThreadProc::laser_convertLaserDate(int const laserDate)
{
	//  
	// 20090928

	if (laserDate <=0)
		return -1;

	int year = int(laserDate / 10000);
	int month = int((laserDate - year*10000)/100);
	int day = laserDate - year*10000 - month * 100;

	struct tm gtime;

	// 1st january
	gtime.tm_year =		year;
	gtime.tm_mon	=		month;
	gtime.tm_isdst =	0;  // ?
	gtime.tm_mday =		day;

	// begin of the day
	gtime.tm_hour =		0;
	gtime.tm_min =		0;
	gtime.tm_sec =		0;

	time_t converted = mktime(&gtime) - _timezone;

	return (double)converted;
}

double WindowThreadProc::laser_convertLaserDateTime(int const laserDate, int const laserTime)
{
	if (laserDate <=0 || laserTime<=0)
		return -1;

	int hours = int(laserTime / 10000);
	int minutes = int((laserTime-hours*10000) / 100);
	int seconds = laserTime - hours*10000 - minutes * 100;

	int year = int(laserDate / 10000);
	int month = int((laserDate - year*10000)/100);
	int day = laserDate - year*10000 - month * 100;


	struct tm gtime;
	memset(&gtime, '\0', sizeof(tm));

	// 1st january
	gtime.tm_year =		year - 1900;
	gtime.tm_mon	=		month-1;
	gtime.tm_isdst =	0;  // ?
	gtime.tm_mday =		day;

	// begin of the day
	gtime.tm_hour =		hours;
	gtime.tm_min =		minutes;
	gtime.tm_sec =		seconds;

	
	time_t converted = mktime(&gtime) - _timezone;

	return (double)converted;


}

// -----------------------------------------
WindowThreadProc* WindowThreadProc::acquireWindowThreadProcInstance()
{
	LOCK_FOR_SCOPE(singletonMtx_m);

	if (WindowThreadProc::refCnt_m++==0) {
		instance_m = new WindowThreadProc();
	}

	instance_m->tickersNumber_m = 0;
	
	

	return instance_m;
}

void WindowThreadProc::releaseWindowThreadProcInstance()
{
	LOCK_FOR_SCOPE(singletonMtx_m);

	if (--WindowThreadProc::refCnt_m==0) {
		delete instance_m;
		instance_m = NULL;
	} 
  
	
}


// --------------------------------------------

void WindowThreadProc::subscribe_all_symbols()
{
	LOCK_FOR_SCOPE(*this);

	int i = 0;
	for(map<CppUtils::String, LPGTSTOCK>::iterator it = subscribedSymbols_m.begin(); it != subscribedSymbols_m.end(); it++) {
		it->second = instance_m->laser_subscribe_symbol(it->first);
		setSubscribedSymbolsNumber(++i);
	}

	initializedAsProvider_m = true;

}

void WindowThreadProc::unsubscribe_all_symbols()
{
	LOCK_FOR_SCOPE(*this);

	for(map<CppUtils::String, LPGTSTOCK>::iterator it = subscribedSymbols_m.begin(); it != subscribedSymbols_m.end(); it++) {
		try {
			instance_m->laser_unsubscribe_symbol(it->first);
		}
		catch(...) 
		{
			// ignore as logging already exists
		}

		it->second = NULL;

		
	}

	setSubscribedSymbolsNumber(0);
	subscribedSymbols_m.clear();
	providerForRT_m.clear();
	initializedAsProvider_m = false;
}

void WindowThreadProc::initialize_symbol_list(CppUtils::StringList const &symbolList, CppUtils::String const& rtProvider)
{
	LOCK_FOR_SCOPE(*this);

	for(int i = 0; i < symbolList.size(); i++) {
		subscribedSymbols_m[symbolList[i]] = 0;
	}

	
	providerForRT_m = rtProvider;
}

void WindowThreadProc::get_subscribed_symbol_list(CppUtils::StringList &symbolList)
{
	LOCK_FOR_SCOPE(*this);
	
	symbolList.clear();
	for(map<CppUtils::String, LPGTSTOCK>::iterator it = subscribedSymbols_m.begin(); it != subscribedSymbols_m.end(); it++) {
		if (it->second != NULL) {
			symbolList.push_back(it->first);
		}
	}

	
}

bool WindowThreadProc::is_initialized_as_provider() const
{
	LOCK_FOR_SCOPE(*this);

	return initializedAsProvider_m;
}

bool WindowThreadProc::wait_logged_subscribed(int const logingtimeout_sec)
{
	return apiLogResponse_m.lock(1000 * logingtimeout_sec);
}

void WindowThreadProc::signal_logged_subscribed()
{
	apiLogResponse_m.signalEvent();
}

void WindowThreadProc::reset_logged_subscribed()
{
	if (instance_m->apiLogResponse_m.lock(0))
			instance_m->apiLogResponse_m.clearEvent();
}
bool WindowThreadProc::is_startup_ok()
{
	return startUpResult_m;
}
void WindowThreadProc::set_startup_ok(bool const result)
{
	startUpResult_m = result;
}

void WindowThreadProc::subscribe_level1_explicit()
{
	setLevel1SubscriptionStatus(true);
	if (gtSetSessionCallBack(getGTBSession(), GTAPI_MSG_SESSION_OnGotQuoteLevel1, (FARPROC)OnGotQuoteLevel1) != 0) 
			THROW(LaserFailed, "exc_CantSetSessionCallback", "ctx_LaserInit", "GTAPI_MSG_SESSION_OnGotQuoteLevel1");
}




}; // end of namespace
