#ifndef _BRKLIBLASER_WINDOWPROC_INCLUDED
#define _BRKLIBLASER_WINDOWPROC_INCLUDED

#include "brkliblaserdefs.hpp"
#include "resource1.h"
#include "laserbroker.hpp"
#include "gtapib_struct.hpp"




namespace BrkLib {
	class WindowThreadProc : private CppUtils::Mutex {
	public:

	
		void threadRun();

		// called to force to close LASE dlg window and stop running thread
		void shutDownLaserAPI();

		// called to finish initialization - e.i. login, etc...
		void startupLaserAPI();

		// helpers
		void setReadyToOperate();

		void setReadyFinished();

		inline LPGTSESSION getGTBSession()
		{ 
			return hSession_m;
		}
	 
	 static void setInstance(HINSTANCE instance);

	 // register and clears callbacks
	 static void registerCallback(LaserBroker& laserBroker);

	 static void clearCallback(LaserBroker& laserBroker);

	private:

		// member callback
		static LRESULT CALLBACK testDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

		void onInitDialog(HWND hDlg);

		void onDestroyDialog(HWND hDlg);

		void onStartup(); // this is a handler of UWM_STARTUP_LASER - e.i. do a lot of initialization

		static void onReconnectClick();

	public:

		// callbacks

		static int CALLBACK OnExecMsgLogin(LPGTSESSION hSession, BOOL bLogin);
		
		static int CALLBACK OnExecConnected(LPGTSESSION hSession);

		static int CALLBACK OnExecDisconnected(LPGTSESSION hSession);

		static int CALLBACK OnExecMsgLoggedin(LPGTSESSION hSession);   

		static int CALLBACK OnExecMsgLogout(LPGTSESSION hSession );

		static int CALLBACK OnExecMsgTrade(LPGTSESSION hSession, const GTTrade32 *trade);

		static int CALLBACK OnExecMsgAccount(LPGTSESSION hSession, const GTAccount32 *account);

		static int CALLBACK OnExecMsgOpenPosition(LPGTSESSION hSession, const GTOpenPosition32 *open);

		static int CALLBACK OnExecMsgPending(LPGTSESSION hSession, const GTPending32 *pending);

		static int CALLBACK OnExecMsgSending( LPGTSESSION hSession, const GTSending32 *sending);

		static int CALLBACK OnExecMsgCanceling(LPGTSESSION hSession, const GTCancel32 * cancel);

		static int CALLBACK OnExecMsgCancel(LPGTSESSION hSession, const GTCancel32 * cancel);

		static int CALLBACK OnExecMsgReject(LPGTSESSION hSession, const GTReject32 *reject);

		static int CALLBACK OnExecMsgRemove(LPGTSESSION hSession, const GTRemove32 *remove);

		static int CALLBACK OnExecMsgRejectCancel(LPGTSESSION hSession, const GTRejectCancel32 *rejectcancel) ;

		static int CALLBACK OnExecMsgStatus(LPGTSESSION hSession, const GTStatus32 *status);

		static int CALLBACK OnExecMsgState(LPGTSESSION hSession, const GTServerState32 *state);

		static int CALLBACK OnGotQuoteLevel1(LPGTSESSION hSession,  const GTLevel132 *level1);

		// ---- PRINTS ----
		static int CALLBACK OnGotQuotePrint(LPGTSESSION hSession,  const GTPrint32 *printR);

		static int CALLBACK OnGotQuotePrintHistory(LPGTSESSION hSession,  const GTPrint32 *printR);

		static int CALLBACK OnGotQuotePrintRefresh(LPGTSESSION hSession,  const GTPrint32 *printR);

		static int CALLBACK OnGotQuotePrintDisplay(LPGTSESSION hSession,  const GTPrint32 *printR);

		static int CALLBACK OnGotQuoteText(LPGTSESSION hSession,  const GTQuoteText32 * textR);

		// ----------------

		static int CALLBACK OnGotQuoteDisconnected(LPGTSESSION hSession);

		static int CALLBACK OnGotQuoteConnected(LPGTSESSION hSession);

		// level 2
		static int CALLBACK OnGotLevel2Connected(LPGTSESSION hSession);

		static int CALLBACK OnGotLevel2Disconnected(LPGTSESSION hSession);

			
		static int CALLBACK OnExecMsgErrMsg(LPGTSESSION hSession, const GTErrMsg32 *errorMsg);

		// chart callbacks
		static int CALLBACK OnGotChartDisconnected(LPGTSESSION hSession);

		static int CALLBACK OnGotChartConnected(LPGTSESSION hSession);

		static int CALLBACK OnGotChartRecord(LPGTSESSION hSession, const GTChart32 *gtchart);

		static int CALLBACK OnGotChartRecordHistory(LPGTSESSION hSession, const GTChart32 *gtchart);

		static int CALLBACK OnGotChartRecordRefresh(LPGTSESSION hSession, const GTChart32 *gtchart);

		static int CALLBACK OnGotChartRecordDisplay(LPGTSESSION hSession, const GTChart32 *gtchart);

		// tick
		static int CALLBACK OnTick(LPGTSESSION hSession);

				

		// stock callbscks
		static int CALLBACK OnExecMsgIOIStatus_Stock(LPGTSTOCK hStock, const GTIOIStatus32 *ioStatus);

		static int CALLBACK OnExecCmd_Stock(LPGTSTOCK hStock, const LPCSTR *execCmd);

		
		static int CALLBACK OnExecMsgSending_Stock( LPGTSTOCK hStock, const GTSending32 *sending);
	
		static int CALLBACK PrePlaceOrder_Stock( LPGTSTOCK hStock,const GTOrder32 *order);
		
		static int CALLBACK PostPlaceOrder_Stock( LPGTSTOCK hStock,const GTOrder32 *order);

		
		
	public:
		// helpers

		LaserApiSettings& getSettings()
		{
			return serversSettings_m;
		};

		// get reference count
		int getRefCount() const;

		void laser_check_session();

		void laser_login(CppUtils::String const& user, CppUtils::String const& password);

		void laser_logout();
		
		void laser_set_address_exec( CppUtils::String const& ip, int const port);

		void laser_set_address_level2( CppUtils::String const& ip, int const port);

		void laser_set_address_quote( CppUtils::String const& ip, int const port);

		void laser_set_address_chart( CppUtils::String const& ip, int const port);

		int laser_connect_quotes();

		LPGTSTOCK laser_subscribe_symbol(	CppUtils::String const& symbol);

		LPGTSTOCK laser_get_stock_handler(CppUtils::String const& symbol);

		LPGTSTOCK laser_get_stock_handler_local(CppUtils::String const& symbol);

		int laser_get_print_count(LPGTSTOCK stockH); 

		GTStock32* laser_get_stock_structure(LPGTSTOCK stockH);

		void laser_unsubscribe_symbol(CppUtils::String const& symbol);

		// force all to be reconnected
		void laser_force_reconnect();

		//
		void laser_request_open_positions(CppUtils::StringList const& symbols, GTOpenPosition32List& openList);

		void laser_request_pending_positions(CppUtils::StringList const& symbols, GTPending32List& pendingList);

		void laser_request_trade_positions(CppUtils::StringList const& symbols, GTTrade32List& tradeList);

		//

		void laser_request_open_positions(GTOpenPosition32List& openList);

		void laser_request_pending_positions(GTPending32List& pendingList);

		void laser_request_trade_positions(GTTrade32List& tradeList);



		void laser_place_order(	LPGTSTOCK stockH, GTOrder32& order);

		void laser_init_order(	LPGTSTOCK stockH, GTOrder32& order);

		//
		void laser_buy(LPGTSTOCK stockH,int const nShares, double const dblPrice, MMID const method );
		
		void laser_bid(LPGTSTOCK stockH,int const nShares, double const dblPrice, MMID const method );

		void laser_sell(LPGTSTOCK stockH,int const nShares, double const dblPrice, MMID const method );
		
		void laser_ask(LPGTSTOCK stockH,int const nShares, double const dblPrice, MMID const method );

		//

		void laser_fill_buy(LPGTSTOCK stockH,int const nShares, double const dblPrice, MMID const method, GTOrder32& order );
		
		void laser_fill_bid(LPGTSTOCK stockH,int const nShares, double const dblPrice, MMID const method, GTOrder32& order );

		void laser_fill_sell(LPGTSTOCK stockH,int const nShares, double const dblPrice, MMID const method, GTOrder32& order );
		
		void laser_fill_ask(LPGTSTOCK stockH,int const nShares, double const dblPrice, MMID const method, GTOrder32& order );

		//

		double laser_getBestAskPrice(LPGTSTOCK stockH);

		double laser_getBestBidPrice(LPGTSTOCK stockH);

		//
		void laser_cancel_order_ticket( long const ticketNo);

		void laser_cancel_order_all( );

		// cancel all for symbol
		void laser_cancel_order(LPGTSTOCK stockH );

		void laser_init_trade_sequence(long const seqId);

		GTime32* laser_get_system_time();

		// time routines
		// this convert gtime to unix econds
		double laser_convertToUnixTime(const GTime32* gtime );

		// this get the begin of the current day and add hours and seconds provided by secs
		double laser_convertToCurrentUnixDateTime( double const secs );

		MMID laser_createMMIDFromString(CppUtils::String const& mmidStr);

		CppUtils::String laser_MMID2String(MMID const mmid);

		CppUtils::String laser_orderPriceIndicator2String(char const priceIndic);

		CppUtils::String laser_getErrorMesage(int nCode );

		// convert to unix from laser standards
		double laser_convertLaserTime(int const laserTime);

		double laser_convertLaserDate(int const laserDate);

		double laser_convertLaserDateTime(int const laserDate, int const laserTime);

		// main functions to subscribe / unsubscribe
		void subscribe_all_symbols();

		void unsubscribe_all_symbols();

		void initialize_symbol_list(CppUtils::StringList const &symbolList, CppUtils::String const& rtProvider);

		void get_subscribed_symbol_list(CppUtils::StringList &symbolList);
	
		bool is_initialized_as_provider() const;

		// these are ---
		bool wait_logged_subscribed(int const logingtimeout_sec);

		void signal_logged_subscribed();

		void reset_logged_subscribed();

    bool is_startup_ok();

		void set_startup_ok(bool const result);

		// ------------

		void subscribe_level1_explicit();


	public:

		static WindowThreadProc* acquireWindowThreadProcInstance();

		static void releaseWindowThreadProcInstance();

		

		
private:

		
		static WindowThreadProc* instance_m;

		static CppUtils::Mutex singletonMtx_m;

		static int refCnt_m;

		// instance of application
		static HINSTANCE hinstDll_m;

	
		// mutex securing callbacks
		static CppUtils::Mutex registerCallbackMtx_m;

		// set storing callbacks
		static set<LaserBroker*> callbackDelegates_m;

		// window handle
		static HWND win_m;

	private:

		static void setEnableLooping(bool const isEnable);

		static void setLevel1SubscriptionStatus(bool is_level1_subscribed);

		static void setSubscribedSymbolsNumber(int symbNumber);


		static void setExecutorConnectStatus(bool is_connected, CppUtils::String const strAddr, CppUtils::String const& srvId);

		static void setQuoteConnectStatus(bool is_connected, CppUtils::String const strAddr, CppUtils::String const& srvId);

		static void setLevel2ConnectStatus(bool is_connected, CppUtils::String const strAddr, CppUtils::String const& srvId);

		static void setChartConnectStatus(bool is_connected, CppUtils::String const strAddr, CppUtils::String const& srvId);

		static void setItemInteger(HWND win, int resource_id, int const value);

		static void setNumberOfTickers(unsigned long const num);

	

		inline double returnAdoptedPrintTime(double const& ttime, char const* symbol);

		inline double returnAdoptedLevel1Time(double const& ttime, char const* symbol);

		
		
	private:

		// private ctor & dtor
		WindowThreadProc( );

		virtual ~WindowThreadProc();

		

		void setApiVersionNumber(HWND dlg);

		
		// members

		CppUtils::Thread* thread_m;

		// event showing that helper window is created
		CppUtils::Event wndCreated_m;

		

		// event finishing shutdown
		CppUtils::Event wndFinished_m;

		
		// current thread id
		DWORD threadId_m;

		// provider 
		CppUtils::String providerForRT_m;


		map<CppUtils::String, LPGTSTOCK> subscribedSymbols_m;


		// session
		LPGTSESSION hSession_m;

		bool initializedAsProvider_m;

		// event showing that we are logged and subscribed
		CppUtils::Event apiLogResponse_m;	

		// to make sure it's not hanging
		unsigned long tickersNumber_m;

		// this is  a special map to adopt the time of tickers because a lot of data goes into one second
		// and we need a finer granularity
		map<CppUtils::String, double> timePrintAdopter_m;

		map<CppUtils::String, double> timeLevel1Adopter_m;

		// genesis.ini holder
		LaserApiSettings serversSettings_m;

		bool startUpResult_m;



	};

	
	

}; // end of namespace

#endif