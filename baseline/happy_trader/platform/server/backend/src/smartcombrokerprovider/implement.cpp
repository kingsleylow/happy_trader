#include "implement.hpp"

#define SMARTCOMPROXY "SMARTCOMPROXY"

#define TRY \
	try 

#ifdef HANDLE_NATIVE_EXCEPTIONS

#define CATCH_ALL	\
	catch(CppUtils::Exception& e)	\
	{	\
	LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  SMARTCOMPROXY, "Exception occured in callback from smartcom_proxy: " << e.message() << " - " << e.arg() << " - " << e.context());	\
	}	\
	catch(Testframe::Exception& e) {	\
	LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  SMARTCOMPROXY, "MSVC exception occured in callback from smartcom_proxy: " << e.message());	\
	}	\
	catch(...)	\
	{	\
	LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  SMARTCOMPROXY, "Unknown exception occured in callback from smartcom_proxy");	\
	};

#else

#define CATCH_ALL	\
	catch(CppUtils::Exception& e)	\
	{	\
	LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  SMARTCOMPROXY, "Exception occured in callback from smartcom_proxy: " << e.message() << " - " << e.arg() << " - " << e.context());	\
	}	\
	catch(...)	\
	{	\
	LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  SMARTCOMPROXY, "Unknown exception occured in callback from smartcom_proxy");	\
	};

#endif




namespace BrkLib {

	SmartComCallbackModule _AtlModule;

	// ----------------------------
	// DllMain
	// To register new window class 
	extern "C" {    

		BOOL APIENTRY DllMain(HINSTANCE hinstDll, DWORD dwReasion, LPVOID lpReserved) {
			if(dwReasion == DLL_PROCESS_ATTACH) {

			}

			return TRUE;
		}

	}; 

	// ------------------------



	// --------------------------
	/**
	terminator and creator functions
	*/
	BrokerBase *creator (Inqueue::AlgorithmHolder* algholder)
	{
		// register event receiver singleton
		return new SmartComBroker(*algholder);

	}

	void terminator (BrokerBase *pthis)
	{
		HTASSERT(pthis);
		delete pthis;

	}

	BrkLib::BrokerBase::CreatorBrokerFun createBroker = creator;

	BrkLib::BrokerBase::DestroyerBrokerFun destroyBroker = terminator;

	// --------------------------


	// --------------------------

	int HistoryRetrivalStructure::cookie_m = 0;

	SmartComBroker::SmartComBroker( Inqueue::AlgorithmHolder& algHolder):
	BrokerBase(algHolder),
	smartComEngine_m(NULL)
	{
	}

	SmartComBroker::~SmartComBroker()
	{
	}

	void SmartComBroker::onLibraryInitialized(HlpStruct::AlgBrokerDescriptor const& descriptor)
	{

		// register COM library here
		registerComLibrary(SMART_COM_ENGINE_DLL);

		// get some parameters
	}

	void SmartComBroker::onLibraryFreed()
	{

	}

	CppUtils::String const& SmartComBroker::getName() const
	{
		static CppUtils::String str("smartcom broker lib");
		return str;
	}


	double SmartComBroker::getBrokerSystemTime()
	{
		return -1;
	}

	// create and destroy session objects
	BrokerSession& SmartComBroker::createSessionObject(CppUtils::String const& connectstring, CppUtils::String const& runName, CppUtils::String const& comment)
	{
		LOCK_FOR_SCOPE(*this);

		map<CppUtils::String, SmartComBrokerSession*>::iterator it = sessions_m.find(connectstring);

		if (it==sessions_m.end()) {
			SmartComBrokerSession* session = new SmartComBrokerSession(connectstring, *this);

			sessions_m[ connectstring ] = session;
			sessions2_m[session->getUid()] = session;

			// notify
			sendSessionCreateEvent(*session, runName, comment);

			return *session;
		}
		else
			return *it->second;
	}

	BrokerSession& SmartComBroker::resolveSessionObject(CppUtils::Uid const& uid)
	{
		LOCK_FOR_SCOPE(*this);

		map<CppUtils::Uid, SmartComBrokerSession*>::iterator it = sessions2_m.find(uid);

		if (it==sessions2_m.end())
			THROW(CppUtils::OperationFailed, "exc_NotFound", "ctx_resolveSessionObject", uid.toString());

		return *it->second;
	}

	void SmartComBroker::getAvailableSessions(CppUtils::UidList& activeSesionsUids) const
	{
		LOCK_FOR_SCOPE(*this);

		activeSesionsUids.clear();

		for(map<CppUtils::Uid, SmartComBrokerSession*>::const_iterator it = sessions2_m.begin(); it != sessions2_m.end(); it++) {
			activeSesionsUids.push_back(it->first);
		}
	}

	void SmartComBroker::deleteSessionObject(BrokerSession& session, CppUtils::String const& runName)
	{
		LOCK_FOR_SCOPE(*this);
		SmartComBrokerSession* ssession = (SmartComBrokerSession*) &session;

		if (ssession) {

			// remove from the list if exist
			int cnt = sessions_m.erase( ssession->getConnection() );

			if (cnt > 0) {
				sessions2_m.erase( ssession->getUid() );

				// notify
				sendSessionDestroyEvent(session, runName);

				delete ssession;
			}
		}
	}


	// connect 
	void SmartComBroker::connectToBroker(BrokerSession& session, CppUtils::String const& runName)
	{
		// do nothing as connection is expected to be alive
	}

	// disconnect
	void SmartComBroker::disconnectFromBroker(BrokerSession& session, CppUtils::String const& runName)
	{
		// do nothing as connection is expected to be alive
	}


	bool SmartComBroker::unblockingIssueOrder(BrkLib::BrokerSession &session, BrkLib::Order const& order)
	{
		//LOCK_FOR_SCOPE(*this);

		if (!order.ID_m.isValid()) {
			return false;
		}

		bool result = false;
		int cookie = getOrderCookie();
		CppUtils::String reason;				   

		BrkLib::OrderType ordertype = BrkLib::OP_DUMMY;

		// register
		OrderHolder oholder((SmartComBrokerSession*)&session, order);

		sessionsForCookies_m[ cookie ] = oholder;

		try {


			if (order.orderType_m & BrkLib::OP_BUY) {
				sessionsForCookies_m[ cookie ].orderType_m = (ordertype=BrkLib::OP_BUY);
				issueBuyOrderHelper(cookie, session, order);
			}
			else if (order.orderType_m & BrkLib::OP_SELL) {
				sessionsForCookies_m[ cookie ].orderType_m = (ordertype= BrkLib::OP_SELL);
				issueSellOrderHelper(cookie, session, order);
			}
			else if (order.orderType_m & BrkLib::OP_BUY_LIMIT) {
				sessionsForCookies_m[ cookie ].orderType_m = (ordertype= BrkLib::OP_BUY_LIMIT);
				issueBuyLimitOrderHelper(cookie, session, order);
			}
			else if (order.orderType_m & BrkLib::OP_SELL_LIMIT) {
				sessionsForCookies_m[ cookie ].orderType_m = (ordertype= BrkLib::OP_SELL_LIMIT);
				issueSellLimitOrderHelper(cookie, session, order);
			}
			else if (order.orderType_m & BrkLib::OP_BUY_STOP) {
				sessionsForCookies_m[ cookie ].orderType_m = (ordertype= BrkLib::OP_BUY_STOP);
				issueBuyStopOrderHelper(cookie, session, order);
			}
			else if (order.orderType_m & BrkLib::OP_SELL_STOP) {
				sessionsForCookies_m[ cookie ].orderType_m = (ordertype= BrkLib::OP_SELL_STOP);
				issueSellStopOrderHelper(cookie, session, order);
			}
			else if (order.orderType_m & BrkLib::OP_SELL_STOP_LIMIT) {
				sessionsForCookies_m[ cookie ].orderType_m = (ordertype= BrkLib::OP_SELL_STOP_LIMIT);
				issueSellStopLimitOrderHelper(cookie, session, order);
			}
			else if (order.orderType_m & BrkLib::OP_BUY_STOP_LIMIT) {
				sessionsForCookies_m[ cookie ].orderType_m = (ordertype= BrkLib::OP_BUY_STOP_LIMIT);
				issueBuyStopLimitOrderHelper(cookie, session, order);
			}
			else if (order.orderType_m & BrkLib::OP_ORDER_CANCEL) {
				sessionsForCookies_m[ cookie ].orderType_m =  (ordertype=BrkLib::OP_ORDER_CANCEL);
				// store cancellations
				cancellations_m[order.brokerOrderID_m] = cookie;
				issueCancelOrderHelper(cookie, session, order);
			}
			else {
				if (!useOnlyOpenPosCommands()) {

					if (order.orderType_m & BrkLib::OP_CLOSE_SHORT) {
						sessionsForCookies_m[ cookie ].orderType_m = (ordertype= BrkLib::OP_CLOSE_SHORT);
						issueCloseShortOrderHelper(cookie, session, order);
					}
					else if (order.orderType_m & BrkLib::OP_CLOSE_LONG) {
						sessionsForCookies_m[ cookie ].orderType_m = (ordertype= BrkLib::OP_CLOSE_LONG);
						issueCloseLongOrderHelper(cookie, session, order);
					}
					else if (order.orderType_m & BrkLib::OP_CLOSE_SHORT_LIMIT) {
						sessionsForCookies_m[ cookie ].orderType_m = (ordertype= BrkLib::OP_CLOSE_SHORT_LIMIT);
						issueCloseShortLimitOrderHelper(cookie, session, order);
					}
					else if (order.orderType_m & BrkLib::OP_CLOSE_LONG_LIMIT) {
						sessionsForCookies_m[ cookie ].orderType_m = (ordertype= BrkLib::OP_CLOSE_LONG_LIMIT);
						issueCloseLongLimitOrderHelper(cookie, session, order);
					}
					else if (order.orderType_m & BrkLib::OP_CLOSE_SHORT_STOP) {
						sessionsForCookies_m[ cookie ].orderType_m = (ordertype= BrkLib::OP_CLOSE_SHORT_STOP);
						issueCloseShortStopOrderHelper(cookie, session, order);
					}
					else if (order.orderType_m & BrkLib::OP_CLOSE_LONG_STOP) {
						sessionsForCookies_m[ cookie ].orderType_m = (ordertype= BrkLib::OP_CLOSE_LONG_STOP);
						issueCloseLongStopOrderHelper(cookie, session, order);
					}
					else if (order.orderType_m & BrkLib::OP_CLOSE_SHORT_STOP_LIMIT) {
						sessionsForCookies_m[ cookie ].orderType_m = (ordertype= BrkLib::OP_CLOSE_SHORT_STOP_LIMIT);
						issueCloseShortStopLimitOrderHelper(cookie, session, order);
					}
					else if (order.orderType_m & BrkLib::OP_CLOSE_LONG_STOP_LIMIT) {
						sessionsForCookies_m[ cookie ].orderType_m =  (ordertype=BrkLib::OP_CLOSE_LONG_STOP_LIMIT);
						issueCloseLongStopLimitOrderHelper(cookie, session, order);
					}
					else
						THROW(CppUtils::OperationFailed, "exc_InvalidOrderType", "ctx_unblockingIssueOrder", order.ID_m.toString())
				}
				else
					THROW(CppUtils::OperationFailed, "exc_CanUseOnlyOpenPosNotation", "ctx_unblockingIssueOrder", order.ID_m.toString());
			}


			result = true;
		}
		catch(CppUtils::Exception& e)
		{
			result =  false;
			reason.append("Exception occured: ");
			reason.append(e.message() + " - " + e.arg() + " - " + e.context());
		}
#ifdef HANDLE_NATIVE_EXCEPTIONS
		catch(Testframe::Exception& e) {
			result =  false;
			reason.append("MSVC exception occured: ");
			reason.append(e.message());
		}
#endif
		catch(...)
		{
			reason.append("Unknown exception occured");
			result = false;
		}

		if (!result) {
			sendErrorResponse(cookie, reason, -1, ordertype );
		}


		return result;
	}

	void SmartComBroker::getBrokerResponseList(BrkLib::BrokerSession &session) const
	{
	}

	void SmartComBroker::getBrokerResponseList(BrkLib::BrokerSession &session, BrkLib::OperationResult const or) const
	{
	}

	void SmartComBroker::getBrokerPositionList(int const posStateQuery, BrokerSession &session, PositionList &posList)
	{
		
	}

	
	void SmartComBroker::onDataArrived(	HlpStruct::RtData const& rtdata)
	{
	}

	void SmartComBroker::initializeBacktest(
		BrkLib::SimulationProfileData const& simulProfileData, 
		BrokerBase::ProviderSymbolCookieMap const& providerSymbolCookieMap 
		)
	{

	}

	void SmartComBroker::initializeBacktest(BrkLib::SimulationProfileData const& simulProfileData )
	{
	}



	bool SmartComBroker::isBacktestLibrary() const
	{
		return false;
	}

	bool SmartComBroker::getTickInfo(
		BrkLib::BrokerSession &session, 
		CppUtils::String const& symbolNum, 
		CppUtils::String const& symbolDenom
		)
	{
		return false;
	}


	// subscribe for nes retreival
	bool SmartComBroker::subscribeForNews(
		BrkLib::BrokerSession &session
		)
	{
		return false;
	}

	// returns last error information
	bool SmartComBroker::getLastErrorInfo(
		BrkLib::BrokerSession &session
		) 
	{
		return false;
	}

	// subscribe for quotes - they will arrive via onBrokerResponseArrived
	bool SmartComBroker::subscribeForQuotes(
		BrkLib::BrokerSession &session,
		CppUtils::String const& symbolNum, 
		CppUtils::String const& symbolDenom
		)
	{
		return false;
	}

	// returns the history from the server
	bool SmartComBroker::getHistoryInfo(
		BrkLib::BrokerSession &session,
		CppUtils::String const& symbolNum, 
		CppUtils::String const& symbolDenom,
		HlpStruct::AggregationPeriods aggrPeriod,
		int const multfactor
		)
	{
		return false;
	}



	void SmartComBroker::onCustomBrokerEventArrived(
		HlpStruct::CallingContext& callContext,
		CppUtils::String &customOutputData
		)
	{

		// through this we will receive event to manage our vroker

		HlpStruct::XmlParameter xmlparam_status;
		xmlparam_status.getName() = "smartcombrokerprovider_com_status";
		xmlparam_status.getParameter("code").getAsInt(true) = 0;
		xmlparam_status.getParameter("status").getAsString(true) = "OK";

		if (callContext.getCallCustomName().size() <=0) {
			LOGEVENT(HlpStruct::CommonLog::LL_WARN,  SMARTCOMPROXY, "Empty onCustomBrokerEventArrived event data");
			return;
		}


		CppUtils::String const& data = callContext.getParameter<BrkLib::String_Proxy>("custom_data").get();

		HlpStruct::XmlParameter param_data;
		HlpStruct::deserializeXmlParameter(param_data, data);

		// reset
		callContext.getParameter<BrkLib::String_Proxy>("custom_data").get() = "";
		CppUtils::String const& operation = callContext.getCallCustomName();

		try {
			if (operation == "subscribe_get_my_trades") {
				int poll_period_sec = param_data.getParameter("poll_period_sec").getAsInt();

				HRESULT hr = smartComEngine_m->subscribeGetMyTrades((long)poll_period_sec );
				if (!SUCCEEDED(hr))
					THROW(CppUtils::OperationFailed, "msg_SubscribeGetMyTradesCallFailed", "ctx_onCustomBrokerEventArrived", "");

			}
			else if (operation == "unsubscribe_get_my_trades") {

				HRESULT hr = smartComEngine_m->unsubscribeGetMyTrades();
				if (!SUCCEEDED(hr))
					THROW(CppUtils::OperationFailed, "msg_UnsubscribeGetMyTradesCallFailed", "ctx_onCustomBrokerEventArrived", "");

			}
			else if (operation == "get_my_trades") {
				HRESULT hr = smartComEngine_m->getMyTradesOnce();

				if (!SUCCEEDED(hr))
					THROW(CppUtils::OperationFailed, "msg_GetMyTradesCallFailed", "ctx_onCustomBrokerEventArrived", "");

				LOGEVENT(HlpStruct::CommonLog::LL_INFO,  SMARTCOMPROXY, "GetMyTrades(...) executed once" );

			}
			else if (operation == "connect") {


				CppUtils::String const& rt_provider = param_data.getParameter("rt_provider").getAsString();
				CppUtils::String const& rt_provider_bar_postfix = param_data.getParameter("rt_provider_bar_postfix").getAsString();
				CppUtils::String const& host = param_data.getParameter("host").getAsString();
				CppUtils::String const& login = param_data.getParameter("login").getAsString();
				CppUtils::StringList const& portfolio_list = param_data.getParameter("portfolio_list").getAsStringList();
				int accompany_ticks_with_bars = param_data.getParameter("accompany_ticks_with_bars").getAsInt();
				short port = param_data.getParameter("port").getAsInt();
				int smartcom_log_level = param_data.getParameter("smartcom_log_level").getAsInt();

				CComSafeArray<BSTR> list_of_portfolio_b;
				initStringSafeArray(list_of_portfolio_b, portfolio_list);


				// need to connect - may connect directly here
				HRESULT hr = smartComEngine_m->connect(
					CComBSTR(rt_provider.c_str()),
					CComBSTR(rt_provider_bar_postfix.c_str()),
					accompany_ticks_with_bars != 0 ? VARIANT_TRUE : VARIANT_FALSE,
					CComBSTR(host.c_str()),
					(short)port,
					CComBSTR(login.c_str()),
					CComBSTR(param_data.getParameter("password").getAsString().c_str()),
					(LPSAFEARRAY)list_of_portfolio_b,
					smartcom_log_level
					);


				if (!SUCCEEDED(hr))
					THROW(CppUtils::OperationFailed, "msg_ConnectCallFailed", "ctx_onCustomBrokerEventArrived", "");


				LOGEVENT(HlpStruct::CommonLog::LL_INFO,  SMARTCOMPROXY, "Connect executed: "	<<
					" RT Provider: " << rt_provider <<
					", RT Provider Bar Postfix: " << rt_provider_bar_postfix <<
					", Host: " << host <<
					", Port: " << port <<
					", Login: " << login <<
					", Log level: " << smartcom_log_level
					);

			}
			else if (callContext.getCallCustomName() == "disconnect") {
				// this queries all available symbols
				HRESULT hr = smartComEngine_m->disconnect();


				if (!SUCCEEDED(hr))
					THROW(CppUtils::OperationFailed, "msg_Disconnect", "ctx_onCustomBrokerEventArrived", "");


				LOGEVENT(HlpStruct::CommonLog::LL_DEBUG,  SMARTCOMPROXY, "Disconnect executed");
			}
			else if (callContext.getCallCustomName() == "get_print_history") {
				CppUtils::StringList const& list_of_symbols = param_data.getParameter("symbol_list").getAsStringList();
				double from_date = param_data.getParameter("history_tick_request_from_date").getAsDate();
				int from_count = param_data.getParameter("history_tick_request_count").getAsInt();



				CComSafeArray<BSTR> list_of_symbols_b;
				initStringSafeArray(list_of_symbols_b, list_of_symbols);

				if (from_date >=0)
					from_date /= 1000.0;
				else
					from_date = -1;

				DATE from_date_ot = CppUtils::convertUnixTimeToOleTime(from_date);
				HRESULT hr = smartComEngine_m->getPrintHistory(
					(LPSAFEARRAY)list_of_symbols_b, 
					from_date_ot,
					from_count
					);


				if (!SUCCEEDED(hr))
					THROW(CppUtils::OperationFailed, "exc_GetPrintHistoryCallFailed", "ctx_onCustomBrokerEventArrived", "");


				LOGEVENT(HlpStruct::CommonLog::LL_INFO,  SMARTCOMPROXY, "get Print History executed");
			}
			else if (callContext.getCallCustomName() == "get_bar_history") {

				CppUtils::StringList const& list_of_symbols = param_data.getParameter("symbol_list").getAsStringList();
				double from_date = param_data.getParameter("bars_request_from_date").getAsDate();
				int from_count = param_data.getParameter("bars_request_count").getAsInt();
				HlpStruct::AggregationPeriods aggr_period_native = (HlpStruct::AggregationPeriods)param_data.getParameter("aggr_period").getAsInt();
				int mult_factor = param_data.getParameter("mult_factor").getAsInt();
				if (mult_factor < 0)
					mult_factor = 1;
				
				//LOG(MSG_INFO, "foo", "mult factor: " <<	 mult_factor <<	 ", aggr_period_native: " << aggr_period_native );

				StBarInterval interval;
				switch(aggr_period_native) {
					case HlpStruct::AP_NoAggr:
						interval = StBarInterval_Tick;
						break;
					case HlpStruct::AP_Minute:
						if (mult_factor == 1)
							interval = StBarInterval_1Min;
						else if (mult_factor == 5)
							interval = StBarInterval_5Min;
						else if (mult_factor == 10)
							interval = StBarInterval_10Min;
						else if (mult_factor == 15)
							interval = StBarInterval_15Min;
						else if (mult_factor == 30)
							interval = StBarInterval_30Min;
						else
							interval = StBarInterval_1Min;
						break;													 
					case HlpStruct::AP_Hour:
						if (mult_factor == 1)
							interval = StBarInterval_60Min;
						else if (mult_factor == 2)
							interval = StBarInterval_2Hour;
						else if (mult_factor == 4)
							interval = StBarInterval_4Hour;
						else
							interval = StBarInterval_60Min;
						break;
					case HlpStruct::AP_Day:
						interval = StBarInterval_Day;
						break;
					case HlpStruct::AP_Week:
						interval = StBarInterval_Week;
						break;
					case HlpStruct::AP_Month:
						interval = StBarInterval_Month;
						break;
					case HlpStruct::AP_Year:
						interval = StBarInterval_Year;
						break;
					default:
						THROW(CppUtils::OperationFailed, "exc_GetBarHistoryCallFailed", "ctx_InvalidAggregationInterval", (long)aggr_period_native);
				}

				CComSafeArray<BSTR> list_of_symbols_b;
				initStringSafeArray(list_of_symbols_b, list_of_symbols);

				if (from_date >=0)
					from_date /= 1000.0;
				else
					from_date = -1;


				HistoryRetrivalTask ftsk; 
				{
					LOCK_FOR_SCOPE(hstRetrStruct_m.hstRetrTaskMtx_m);
					hstRetrStruct_m.hstRetrMap_m.clear();
					
					for (int i = 0; i < list_of_symbols_b.GetCount(); i++) {
					
						HistoryRetrivalTask tsk(interval, list_of_symbols_b.GetAt(i), from_count, CppUtils::convertUnixTimeToOleTime(from_date), HistoryRetrivalStructure::cookie_m++);
					
						if (i==0) {
							ftsk = tsk;
							continue;
						}

						hstRetrStruct_m.hstRetrMap_m[tsk.cookie_m] = tsk;
					}

					
				}


				// just first
				
				CppUtils::sleep(2.0);

				HRESULT hr = smartComEngine_m->getBarsHistory(
					ftsk.symbol_m, 
					ftsk.interval_m,
					ftsk.from_date_m,
					ftsk.from_count_m,
					ftsk.cookie_m
				);											   

				if (FAILED(hr)) {
					LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  SMARTCOMPROXY, "Cannot start history retrieve for symbol: " << CW2A(ftsk.symbol_m)); 
				}
				

				LOGEVENT(HlpStruct::CommonLog::LL_INFO,  SMARTCOMPROXY, "get_bar_history(...) pending, date: " << CppUtils::getGmtTime2(from_date) << 
					", bar count: " << from_count << ", interval: " << interval);
				
			}
			else if (callContext.getCallCustomName() == "subscribe_prints") {
				CppUtils::StringList const& list_of_symbols = param_data.getParameter("symbol_list").getAsStringList();

				CComSafeArray<BSTR> list_of_symbols_b;
				initStringSafeArray(list_of_symbols_b, list_of_symbols);

				HRESULT hr = smartComEngine_m->subscribeForTicks((LPSAFEARRAY)list_of_symbols_b);
				if (!SUCCEEDED(hr))
					THROW(CppUtils::OperationFailed, "exc_SubscribePrintsCallFailed", "ctx_onCustomBrokerEventArrived", "");


				LOGEVENT(HlpStruct::CommonLog::LL_INFO,  SMARTCOMPROXY, "Subscribe prints executed");
			}
			else if (callContext.getCallCustomName() == "unsubscribe_prints") {
				CppUtils::StringList const& list_of_symbols = param_data.getParameter("symbol_list").getAsStringList();
				CComSafeArray<BSTR> list_of_symbols_b;
				initStringSafeArray(list_of_symbols_b, list_of_symbols);

				HRESULT hr = smartComEngine_m->unsubscribeForTicks((LPSAFEARRAY)list_of_symbols_b);
				if (!SUCCEEDED(hr))
					THROW(CppUtils::OperationFailed, "exc_UnsubscribePrintsCallFailed", "ctx_onCustomBrokerEventArrived", "");



				LOGEVENT(HlpStruct::CommonLog::LL_INFO,  SMARTCOMPROXY, "Unsubscribe prints executed");
			}
			else if (callContext.getCallCustomName() == "subscribe_level1") {
				CppUtils::StringList const& list_of_symbols = param_data.getParameter("symbol_list").getAsStringList();
				CComSafeArray<BSTR> list_of_symbols_b;
				initStringSafeArray(list_of_symbols_b, list_of_symbols);

				HRESULT hr = smartComEngine_m->subscribeForLevel1((LPSAFEARRAY)list_of_symbols_b);
				if (!SUCCEEDED(hr))
					THROW(CppUtils::OperationFailed, "exc_SubscribeLevel1CallFailed", "ctx_onCustomBrokerEventArrived", "");

				LOGEVENT(HlpStruct::CommonLog::LL_INFO,  SMARTCOMPROXY, "Subscribe level1 executed");
			}
			else if (callContext.getCallCustomName() == "unsubscribe_level1") {
				CppUtils::StringList const& list_of_symbols = param_data.getParameter("symbol_list").getAsStringList();
				CComSafeArray<BSTR> list_of_symbols_b;
				initStringSafeArray(list_of_symbols_b, list_of_symbols);

				HRESULT hr = smartComEngine_m->unsubscribeForLevel1((LPSAFEARRAY)list_of_symbols_b);
				if (!SUCCEEDED(hr))
					THROW(CppUtils::OperationFailed, "exc_UnsubscribeLevel1CallFailed", "ctx_onCustomBrokerEventArrived", "");

				LOGEVENT(HlpStruct::CommonLog::LL_INFO,  SMARTCOMPROXY, "Unsubscribe level1 executed");
			}
			// level2
			else if (callContext.getCallCustomName() == "subscribe_level2") {
				CppUtils::StringList const& list_of_symbols = param_data.getParameter("symbol_list").getAsStringList();
				CComSafeArray<BSTR> list_of_symbols_b;
				initStringSafeArray(list_of_symbols_b, list_of_symbols);

				HRESULT hr = smartComEngine_m->subscribeForLevel2((LPSAFEARRAY)list_of_symbols_b);
				if (!SUCCEEDED(hr))
					THROW(CppUtils::OperationFailed, "exc_SubscribeLevel1CallFailed", "ctx_onCustomBrokerEventArrived", "");

				LOGEVENT(HlpStruct::CommonLog::LL_INFO,  SMARTCOMPROXY, "Subscribe level2 executed");
			}
			else if (callContext.getCallCustomName() == "unsubscribe_level2") {
				CppUtils::StringList const& list_of_symbols = param_data.getParameter("symbol_list").getAsStringList();
				CComSafeArray<BSTR> list_of_symbols_b;
				initStringSafeArray(list_of_symbols_b, list_of_symbols);

				HRESULT hr = smartComEngine_m->unsubscribeForLevel2((LPSAFEARRAY)list_of_symbols_b);
				if (!SUCCEEDED(hr))
					THROW(CppUtils::OperationFailed, "exc_UnsubscribeLevel1CallFailed", "ctx_onCustomBrokerEventArrived", "");

				LOGEVENT(HlpStruct::CommonLog::LL_INFO,  SMARTCOMPROXY, "Unsubscribe level2 executed");
			}
			else if (callContext.getCallCustomName() == "query_all_symbols") {
				// this queries all available symbols
				HRESULT hr = smartComEngine_m->querySymbols();
				if (!SUCCEEDED(hr))
					THROW(CppUtils::OperationFailed, "exc_QuerySymbolsFailed", "ctx_onCustomBrokerEventArrived", "");


				LOGEVENT(HlpStruct::CommonLog::LL_INFO,  SMARTCOMPROXY, "Query all symbols executed");
			}
		}
		catch(CppUtils::OperationFailed& e)
		{
			// in case of exception
			LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  SMARTCOMPROXY, "On operation: " << operation << " exception occured: " << e.message());

			xmlparam_status.getParameter("code").getAsInt(true) = -1;
			xmlparam_status.getParameter("status").getAsString(true) = e.message();

		}
#ifdef HANDLE_NATIVE_EXCEPTIONS
		catch(Testframe::Exception& e) {
			LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  SMARTCOMPROXY, "On operation: " << operation << " MSVC exception occured: " << e.message());

			xmlparam_status.getParameter("code").getAsInt(true) = -2;
			xmlparam_status.getParameter("status").getAsString(true) = e.message();
		}
#endif
		catch(...)
		{
			// in case of exception
			LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  SMARTCOMPROXY, "On operation: " << operation << " unknown exception occured" );

			xmlparam_status.getParameter("code").getAsInt(true) = -3;
			xmlparam_status.getParameter("status").getAsString(true) = "Unknown failure";
		}

		HlpStruct::serializeXmlParameter(xmlparam_status, customOutputData);
	
	}

	void SmartComBroker::onThreadStarted(bool const firstLib, bool const lastLib
		)
	{
		// init COM from the first call
		if (firstLib) {
			operationsInit();
		}

		/*
		// resolve singleton
		CMainSmartComEngine* engine = CppUtils::getRootObject<CMainSmartComEngine>();
		if (engine == NULL) {
		engine = new CMainSmartComEngine();
		CppUtils::RootObjectManager::singleton().registerRootObject(CMainSmartComEngine::getRootObjectName(), engine );

		}  

		smartComEngine_m = engine; 
		*/

		CComBSTR clsid_MainSmartComEngine_bstr;
		::StringFromCLSID(__uuidof(SmartComEngine::MainSmartComEngine), &clsid_MainSmartComEngine_bstr);

		CppUtils::String clsid_MainSmartComEngine_str;
		if (clsid_MainSmartComEngine_bstr.Length() > 0) {
			clsid_MainSmartComEngine_str = CW2A(clsid_MainSmartComEngine_bstr);
		}

		if (!SUCCEEDED(CoCreateInstance(__uuidof(SmartComEngine::MainSmartComEngine) /*CLSID_MainSmartComEngine*/, NULL, CLSCTX_INPROC_SERVER, __uuidof(SmartComEngine::IMainSmartComEngine), (void**)&smartComEngine_m)))	
			THROW(CppUtils::OperationFailed, "exc_cannotCreateInstance", "ctx_LibraryInitialized",  clsid_MainSmartComEngine_str);

		IMainSmartComEngineCallback* pThis = (IMainSmartComEngineCallback*)this;
		if (!SUCCEEDED(smartComEngine_m->libInit(pThis, &subscriptionCookie_m)))
			THROW(CppUtils::OperationFailed, "exc_CannotInitSmartComEngine", "ctx_LibraryInitialized", "CLSID_MainSmartComEngine" );


		/*
		// GIT
		if (!SUCCEEDED(CoCreateInstance(CLSID_StdGlobalInterfaceTable,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_IGlobalInterfaceTable,
			(void **)&globalGIT_m))
		)	 {
			THROW(CppUtils::OperationFailed, "exc_CannotCreateGIT", "ctx_LibraryInitialized", "");
		}


		if (!SUCCEEDED(globalGIT_m->RegisterInterfaceInGlobal(
			smartComEngine_m,
			__uuidof(SmartComEngine::IMainSmartComEngine),
			&smartComEnginePtrCookie_m	))
		)	{
			THROW(CppUtils::OperationFailed, "exc_CannotRegisterInGIT", "ctx_LibraryInitialized", "");
		}
		*/

		LOGEVENT( HlpStruct::CommonLog::LL_INFO, SMARTCOMPROXY, "Started, found helper smartcom engine lib: {" << clsid_MainSmartComEngine_str << "}");

	};

	void SmartComBroker::onThreadFinished(HlpStruct::TradingParameters const& tradeParams, bool const firstLib, bool const lastLib)
	{
		if (smartComEngine_m != NULL) {

			/*
			if (!SUCCEEDED(globalGIT_m->RevokeInterfaceFromGlobal(smartComEnginePtrCookie_m))) {
				LOGEVENT( HlpStruct::CommonLog::LL_INFO, SMARTCOMPROXY, "Cannot revoke from GIT");
			}
			

			globalGIT_m = NULL;
			smartComEnginePtrCookie_m = 0;
			*/		

			smartComEngine_m->libDeinit(subscriptionCookie_m);
			subscriptionCookie_m = -1;


			//if (smartComEngine_m->getRefCount() == 0) {
			//	delete smartComEngine_m;
			//}

			smartComEngine_m = NULL;

		}

		LOGEVENT( HlpStruct::CommonLog::LL_INFO, SMARTCOMPROXY, "Finished");

		// deinit COM on the last call
		if (lastLib) {
			operationsDeinit();
		}
	};

	// --------------------------------------
	// CALLBACKS
	// --------------------------------------


	// --------------------------------------

	CppUtils::String SmartComBroker::extractPortfolioFromOrder(BrkLib::Order const& order)
	{
		// PORTFOLIO=....
		CppUtils::String ainfo = order.additionalInfo_m;
		CppUtils::toupper( ainfo);
		
		CppUtils::StringList tokens;
		CppUtils::tokenize(ainfo, tokens, "=");

		int idx = 0;
		CppUtils::String var_name, var_value;
		while(idx < tokens.size()) {
			var_name = tokens[idx];
			var_value= tokens[idx+1];

			if (var_name == "PORTFOLIO")
				return var_value;
			idx  = idx + 2;
		};

		return "";
	
	}

	void SmartComBroker::initStringSafeArray(CComSafeArray<BSTR>& safearray, CppUtils::StringList const& sourceList)
	{
		safearray.Destroy();
		safearray.Create(sourceList.size(),0);

		for(int i = 0; i < sourceList.size(); i++) {
			safearray.SetAt(i, CComBSTR(sourceList.at(i).c_str()).Detach(), 0 );
		}  
	}

	BrkLib::OrderType SmartComBroker::smartComOperationFlagsToOrderType(bool const use_only_open, int const opType, int const action, int const parentOrderType)
	{
	

		BrkLib::OrderType result = BrkLib::OP_DUMMY;
		bool pos_open;
		
		if (parentOrderType & BrkLib::OP_BUY ||
			parentOrderType & BrkLib::OP_SELL	 ||
			parentOrderType & BrkLib::OP_BUY_LIMIT ||
			parentOrderType & BrkLib::OP_SELL_LIMIT	||
			parentOrderType & BrkLib::OP_BUY_STOP	 ||
			parentOrderType & BrkLib::OP_SELL_STOP ||
			parentOrderType & BrkLib::OP_BUY_STOP_LIMIT	||
			parentOrderType & BrkLib::OP_SELL_STOP_LIMIT 
			) {
				pos_open = true;
		}
		else { 
			
			if (!use_only_open)	{
				if (parentOrderType & BrkLib::OP_CLOSE_LONG ||
					parentOrderType & BrkLib::OP_CLOSE_SHORT	 ||
					parentOrderType & BrkLib::OP_CLOSE_SHORT_LIMIT ||
					parentOrderType & BrkLib::OP_CLOSE_LONG_LIMIT	 ||
					parentOrderType & BrkLib::OP_CLOSE_SHORT_STOP	 ||
					parentOrderType & BrkLib::OP_CLOSE_LONG_STOP	 ||
					parentOrderType & BrkLib::OP_CLOSE_SHORT_STOP_LIMIT	||
					parentOrderType & BrkLib::OP_CLOSE_LONG_STOP_LIMIT
					) {
						pos_open = false;
				}
				else {
					LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  SMARTCOMPROXY, "Invalid parent order type");
					return result;
				}
			}
			else {
				LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  SMARTCOMPROXY, "We are allowed to use only open pos commands");
				return result;
			}
		}

		if (!use_only_open) {

			if (action == StOrder_Action_Buy) {
				// open here long	
				if (opType == StOrder_Type_Market) {
					result = (pos_open ? BrkLib::OP_BUY : BrkLib::OP_CLOSE_SHORT);
				}
				else if (opType == StOrder_Type_Limit) {

					result = (pos_open ? BrkLib::OP_BUY_LIMIT : BrkLib::OP_CLOSE_SHORT_LIMIT);
				}
				else if (opType == StOrder_Type_Stop) {

					result = (pos_open ? BrkLib::OP_BUY_STOP : BrkLib::OP_CLOSE_SHORT_STOP);
				}
				else if (opType == StOrder_Type_StopLimit) {
					result = (pos_open ? BrkLib::OP_BUY_STOP_LIMIT : BrkLib::OP_CLOSE_SHORT_STOP_LIMIT);
				}

			}
			else if (action == StOrder_Action_Sell) {
				// close long
				if (opType == StOrder_Type_Market) {
					result = (pos_open ? BrkLib::OP_SELL : BrkLib::OP_CLOSE_LONG);
				}
				else if (opType == StOrder_Type_Limit) {

					result = (pos_open ? BrkLib::OP_SELL_LIMIT : BrkLib::OP_CLOSE_LONG_LIMIT);
				}
				else if (opType == StOrder_Type_Stop) {

					result = (pos_open ? BrkLib::OP_SELL_STOP : BrkLib::OP_CLOSE_LONG_STOP);
				}
				else if (opType == StOrder_Type_StopLimit) {
					result = (pos_open ? BrkLib::OP_SELL_STOP_LIMIT : BrkLib::OP_CLOSE_LONG_STOP_LIMIT);
				}

			}	 else {
				LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  SMARTCOMPROXY, "Invalid response order type");
			}
		}
		else {
			// open pos notation only

			if (action == StOrder_Action_Buy) {
				// open here long	
				if (opType == StOrder_Type_Market) {
					result = BrkLib::OP_BUY;
				}
				else if (opType == StOrder_Type_Limit) {

					result = BrkLib::OP_BUY_LIMIT;
				}
				else if (opType == StOrder_Type_Stop) {

					result = BrkLib::OP_BUY_STOP;
				}
				else if (opType == StOrder_Type_StopLimit) {
					result = BrkLib::OP_BUY_STOP_LIMIT;
				}

			}
			else if (action == StOrder_Action_Sell) {
				// close long
				if (opType == StOrder_Type_Market) {
					result = BrkLib::OP_SELL;
				}
				else if (opType == StOrder_Type_Limit) {

					result = BrkLib::OP_SELL_LIMIT ;
				}
				else if (opType == StOrder_Type_Stop) {

					result = BrkLib::OP_SELL_STOP;
				}
				else if (opType == StOrder_Type_StopLimit) {
					result = BrkLib::OP_SELL_STOP_LIMIT;
				}

			}	 else {
				LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  SMARTCOMPROXY, "Invalid response order type (open pos notation only)");
			}
		}



		return result;
	}

	long SmartComBroker::getOrderCookie()
	{
		static CppUtils::Mutex mtx;
		// begin with 100
		static long idx = 100;
		LOCK_FOR_SCOPE(mtx)
		{
			++idx;
			return idx;	
		}
	};

	void SmartComBroker::operationsInit()
	{
		// must be here only COINIT_MULTITHREADED as main STA is in another thread/place!
		::CoInitializeEx(NULL, COINIT_MULTITHREADED );
		//::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED );
		
		LOGEVENT( HlpStruct::CommonLog::LL_INFO, SMARTCOMPROXY, "COM initialized as COINIT_MULTITHREADED");


	}

	void SmartComBroker::operationsDeinit()
	{
		::CoUninitialize();
		LOGEVENT( HlpStruct::CommonLog::LL_INFO, SMARTCOMPROXY, "COM deinitialized");
	}

	void SmartComBroker::registerComLibrary(CppUtils::String const& dllName)
	{

		void* libPtr = CppUtils::dynlibOpen( dllName);
		if (!libPtr)
			THROW(CppUtils::OperationFailed, "exc_loadLibFailed", "ctx_registerCOMLib", dllName +", exception: "+ CppUtils::getOSError());


		void* libFun  =  CppUtils::dynlibGetSym( libPtr, "DllRegisterServer" );
		if (!libFun) {
			CppUtils::dynlibClose(libPtr);
			THROW(CppUtils::OperationFailed, "exc_cannotResolve_DllRegisterServer", "ctx_registerCOMLib",dllName +", exception: "+ CppUtils::getOSError());
		}

		// HRESULT (STDAPICALLTYPE* DllRegisterServer_Fptr)(void) = (Pfn)libFun;
		Pfn DllRegisterServer_Fptr = (Pfn)libFun;



		// call this function
		try {
			HRESULT r = DllRegisterServer_Fptr();

			if (SUCCEEDED(r)) {
				LOGEVENT( HlpStruct::CommonLog::LL_INFO, SMARTCOMPROXY, "Registered COM library successfully: " << SMART_COM_ENGINE_DLL);
			} else {
				LOGEVENT( HlpStruct::CommonLog::LL_ERROR, SMARTCOMPROXY, "DllRegisterServer returned: " << r << "for: " << dllName);
			}
		}
		catch(...)
		{
			LOGEVENT( HlpStruct::CommonLog::LL_ERROR, SMARTCOMPROXY, "Unknown problems in registering COM library: " << dllName);
		}



		CppUtils::dynlibClose(libPtr);


	}

	void SmartComBroker::issueBuyOrderHelper(int const cookie, BrkLib::BrokerSession &session, BrkLib::Order const& order)
	{

		HRESULT hr = smartComEngine_m->issueOrder(
			CComBSTR(extractPortfolioFromOrder(order).c_str()),
			CComBSTR(order.symbol_m.c_str()),
			StOrder_Action_Buy,
			StOrder_Type_Market,
			getOrderExpirationTime(order),
			0, // market order
			order.orVolume_m,
			0, // matrket order
			cookie,
			(IMainSmartComEngineCallback*)this
			);

		if (FAILED(hr))
			THROW(CppUtils::OperationFailed, "exc_CannotIssueBuyOrder", "cxt_issueBuyOrderHelper", "");

	}

	void SmartComBroker::issueSellOrderHelper(int const cookie, BrkLib::BrokerSession &session, BrkLib::Order const& order)
	{
		HRESULT hr = smartComEngine_m->issueOrder(
				CComBSTR(extractPortfolioFromOrder(order).c_str()),
			CComBSTR(order.symbol_m.c_str()),
			//StOrder_Action_Short,
			StOrder_Action_Sell,
			StOrder_Type_Market,
			getOrderExpirationTime(order),
			0, // market order
			order.orVolume_m,
			0, // matrket order
			cookie,
			(IMainSmartComEngineCallback*)this
			);

		if (FAILED(hr))
			THROW(CppUtils::OperationFailed, "exc_CannotIssueSellOrder", "cxt_issueSellOrderHelper", "");
	}

	void SmartComBroker::issueBuyLimitOrderHelper(int const cookie, BrkLib::BrokerSession &session, BrkLib::Order const& order)
	{
		HRESULT hr = smartComEngine_m->issueOrder(
			CComBSTR(extractPortfolioFromOrder(order).c_str()),
			CComBSTR(order.symbol_m.c_str()),
			StOrder_Action_Buy,
			StOrder_Type_Limit,
			getOrderExpirationTime(order),
			order.orPrice_m, // limit order
			order.orVolume_m,
			0, // limit order
			cookie,
			(IMainSmartComEngineCallback*)this
			);

		if (FAILED(hr))
			THROW(CppUtils::OperationFailed, "exc_CannotIssueBuyLimitOrder", "cxt_issueBuyLimitOrderHelper", "");
	}

	void SmartComBroker::issueSellLimitOrderHelper(int const cookie, BrkLib::BrokerSession &session, BrkLib::Order const& order)
	{
		HRESULT hr = smartComEngine_m->issueOrder(
				CComBSTR(extractPortfolioFromOrder(order).c_str()),
			CComBSTR(order.symbol_m.c_str()),
			//StOrder_Action_Short,
			StOrder_Action_Sell,
			StOrder_Type_Limit,
			getOrderExpirationTime(order),
			order.orPrice_m, // limit order
			order.orVolume_m,
			0, // limit order
			cookie,
			(IMainSmartComEngineCallback*)this
			);

		if (FAILED(hr))
			THROW(CppUtils::OperationFailed, "exc_CannotIssueSellLimitOrder", "cxt_issueSellLimitOrderHelper", "");
	}

	void SmartComBroker::issueBuyStopOrderHelper(int const cookie, BrkLib::BrokerSession &session, BrkLib::Order const& order)
	{
		HRESULT hr = smartComEngine_m->issueOrder(
				CComBSTR(extractPortfolioFromOrder(order).c_str()),
			CComBSTR(order.symbol_m.c_str()),
			StOrder_Action_Buy,
			StOrder_Type_Stop,
			getOrderExpirationTime(order),
			0, // stop order
			order.orVolume_m,
			order.orStopPrice_m, // stop price
			cookie,
			(IMainSmartComEngineCallback*)this
			);

		if (FAILED(hr))
			THROW(CppUtils::OperationFailed, "exc_CannotIssueBuyStopOrder", "cxt_issueBuyStopOrderHelper", "");

	}

	void SmartComBroker::issueSellStopOrderHelper(int const cookie, BrkLib::BrokerSession &session, BrkLib::Order const& order)
	{
		HRESULT hr = smartComEngine_m->issueOrder(
				CComBSTR(extractPortfolioFromOrder(order).c_str()),
			CComBSTR(order.symbol_m.c_str()),
			//StOrder_Action_Short,
			StOrder_Action_Sell,
			StOrder_Type_Stop,
			getOrderExpirationTime(order),
			0, // stop order
			order.orVolume_m,
			order.orStopPrice_m, // stop price
			cookie,
			(IMainSmartComEngineCallback*)this
			);

		if (FAILED(hr))
			THROW(CppUtils::OperationFailed, "exc_CannotIssueSellStopOrder", "cxt_issueSellStopOrderHelper", "");

	}

	void SmartComBroker::issueBuyStopLimitOrderHelper(int const cookie, BrkLib::BrokerSession &session, BrkLib::Order const& order)
	{
		HRESULT hr = smartComEngine_m->issueOrder(
				CComBSTR(extractPortfolioFromOrder(order).c_str()),
			CComBSTR(order.symbol_m.c_str()),
			StOrder_Action_Buy,
			StOrder_Type_StopLimit,
			getOrderExpirationTime(order),
			order.orPrice_m, 
			order.orVolume_m,
			order.orStopPrice_m, 
			cookie,
			(IMainSmartComEngineCallback*)this
			);

		if (FAILED(hr))
			THROW(CppUtils::OperationFailed, "exc_CannotIssueBuyStopLimitOrder", "cxt_issueBuyStopLimitOrderHelper", "");
	}

	void SmartComBroker::issueSellStopLimitOrderHelper(int const cookie, BrkLib::BrokerSession &session, BrkLib::Order const& order)
	{
		HRESULT hr = smartComEngine_m->issueOrder(
				CComBSTR(extractPortfolioFromOrder(order).c_str()),
			CComBSTR(order.symbol_m.c_str()),
			//StOrder_Action_Short,
			StOrder_Action_Sell,
			StOrder_Type_StopLimit,
			getOrderExpirationTime(order),
			order.orPrice_m, 
			order.orVolume_m,
			order.orStopPrice_m, 
			cookie,
			(IMainSmartComEngineCallback*)this
			);

		if (FAILED(hr))
			THROW(CppUtils::OperationFailed, "exc_CannotIssueSellStopLimitOrder", "cxt_issueSellStopLimitOrderHelper", "");
	}



	void SmartComBroker::issueCloseShortOrderHelper(int const cookie, BrkLib::BrokerSession &session, BrkLib::Order const& order)
	{
		HRESULT hr = smartComEngine_m->issueOrder(
				CComBSTR(extractPortfolioFromOrder(order).c_str()),
			CComBSTR(order.symbol_m.c_str()),
			//StOrder_Action_Cover,
			StOrder_Action_Buy,
			StOrder_Type_Market,
			getOrderExpirationTime(order),
			0, // market order
			order.orVolume_m,
			0, // matrket order
			cookie,
			(IMainSmartComEngineCallback*)this
			);

		if (FAILED(hr))
			THROW(CppUtils::OperationFailed, "exc_CannotIssueCloseShortOrder", "cxt_issueCloseShortOrderHelper", "");
	}

	void SmartComBroker::issueCloseLongOrderHelper(int const cookie, BrkLib::BrokerSession &session, BrkLib::Order const& order)
	{
		HRESULT hr = smartComEngine_m->issueOrder(
				CComBSTR(extractPortfolioFromOrder(order).c_str()),
			CComBSTR(order.symbol_m.c_str()),
			StOrder_Action_Sell,
			StOrder_Type_Market,
			getOrderExpirationTime(order),
			0, // market order
			order.orVolume_m,
			0, // matrket order
			cookie,
			(IMainSmartComEngineCallback*)this
			);


		if (FAILED(hr))
			THROW(CppUtils::OperationFailed, "exc_CannotIssueCloseLongOrder", "cxt_issueCloseLongOrderHelper", "");
	}

	void SmartComBroker::issueCloseShortLimitOrderHelper(int const cookie, BrkLib::BrokerSession &session, BrkLib::Order const& order)
	{
		HRESULT hr = smartComEngine_m->issueOrder(
				CComBSTR(extractPortfolioFromOrder(order).c_str()),
			CComBSTR(order.symbol_m.c_str()),
			//StOrder_Action_Cover,
			StOrder_Action_Buy,
			StOrder_Type_Limit,
			getOrderExpirationTime(order),
			order.orPrice_m, // limit order
			order.orVolume_m,
			0, // limit order
			cookie,
			(IMainSmartComEngineCallback*)this
			);

		if (FAILED(hr))
			THROW(CppUtils::OperationFailed, "exc_CannotIssueCloseShortLimitOrder", "cxt_issueCloseShortLimitOrderHelper", "");
	}

	void SmartComBroker::issueCloseLongLimitOrderHelper(int const cookie, BrkLib::BrokerSession &session, BrkLib::Order const& order)
	{
		HRESULT hr = smartComEngine_m->issueOrder(
				CComBSTR(extractPortfolioFromOrder(order).c_str()),
			CComBSTR(order.symbol_m.c_str()),
			StOrder_Action_Sell,
			StOrder_Type_Limit,
			getOrderExpirationTime(order),
			order.orPrice_m, // limit order
			order.orVolume_m,
			0, // limit order
			cookie,
			(IMainSmartComEngineCallback*)this
			);

		if (FAILED(hr))
			THROW(CppUtils::OperationFailed, "exc_CannotIssueCloseLongLimitOrder", "cxt_issueCloseLongLimitOrderHelper", "");
	}


	void SmartComBroker::issueCloseShortStopOrderHelper(int const cookie, BrkLib::BrokerSession &session, BrkLib::Order const& order)
	{
		HRESULT hr = smartComEngine_m->issueOrder(
				CComBSTR(extractPortfolioFromOrder(order).c_str()),
			CComBSTR(order.symbol_m.c_str()),
			//StOrder_Action_Cover,
			StOrder_Action_Buy,
			StOrder_Type_Stop,
			getOrderExpirationTime(order),
			0, // stop order
			order.orVolume_m,
			order.orStopPrice_m, // stop price
			cookie,
			(IMainSmartComEngineCallback*)this
			);

		if (FAILED(hr))
			THROW(CppUtils::OperationFailed, "exc_CannotIssueCloseShortStopOrder", "cxt_issueCloseShortStopOrderHelper", "");
	}

	void SmartComBroker::issueCloseLongStopOrderHelper(int const cookie, BrkLib::BrokerSession &session, BrkLib::Order const& order)
	{
		HRESULT hr = smartComEngine_m->issueOrder(
				CComBSTR(extractPortfolioFromOrder(order).c_str()),
			CComBSTR(order.symbol_m.c_str()),
			StOrder_Action_Sell,
			StOrder_Type_Stop,
			getOrderExpirationTime(order),
			0, // stop order
			order.orVolume_m,
			order.orStopPrice_m, // stop price
			cookie,
			(IMainSmartComEngineCallback*)this
			);

		if (FAILED(hr))
			THROW(CppUtils::OperationFailed, "exc_CannotIssueCloseLongStopOrder", "cxt_issueCloseLongStopOrderHelper", "");
	}

	void SmartComBroker::issueCloseShortStopLimitOrderHelper(int const cookie, BrkLib::BrokerSession &session, BrkLib::Order const& order)
	{
		HRESULT hr = smartComEngine_m->issueOrder(
				CComBSTR(extractPortfolioFromOrder(order).c_str()),
			CComBSTR(order.symbol_m.c_str()),
			//StOrder_Action_Cover,
			StOrder_Action_Buy,
			StOrder_Type_StopLimit,
			getOrderExpirationTime(order),
			order.orPrice_m, 
			order.orVolume_m,
			order.orStopPrice_m, 
			cookie,
			(IMainSmartComEngineCallback*)this
			);

		if (FAILED(hr))
			THROW(CppUtils::OperationFailed, "exc_CannotIssueCloseShortLimitStopOrder", "cxt_issueCloseShortLimitStopOrderHelper", "");
	}

	void SmartComBroker::issueCloseLongStopLimitOrderHelper(int const cookie, BrkLib::BrokerSession &session, BrkLib::Order const& order)
	{
		HRESULT hr = smartComEngine_m->issueOrder(
				CComBSTR(extractPortfolioFromOrder(order).c_str()),
			CComBSTR(order.symbol_m.c_str()),
			StOrder_Action_Sell,
			StOrder_Type_StopLimit,
			getOrderExpirationTime(order),
			order.orPrice_m, 
			order.orVolume_m,
			order.orStopPrice_m, 
			cookie,
			(IMainSmartComEngineCallback*)this
			);

		if (FAILED(hr))
			THROW(CppUtils::OperationFailed, "exc_CannotIssueCloseLongLimitStopOrder", "cxt_issueCloseLongLimitStopOrderHelper", "");
	}


	void SmartComBroker::issueCancelOrderHelper(int const cookie, BrkLib::BrokerSession &session, BrkLib::Order const& order)
	{
		if (order.brokerOrderID_m.size() > 0) {

			LOGEVENT(HlpStruct::CommonLog::LL_INFO,  SMARTCOMPROXY, "Cancel order with id: " << order.brokerOrderID_m);

			HRESULT hr = smartComEngine_m->cancelOrder(
					CComBSTR(extractPortfolioFromOrder(order).c_str()),
				CComBSTR(order.symbol_m.c_str()), 
				CComBSTR(order.brokerOrderID_m.c_str()),
				cookie,
				(IMainSmartComEngineCallback*)this
				);

			if (FAILED(hr))
				THROW(CppUtils::OperationFailed, "exc_CannotIssueCancelOrder", "cxt_issueCancelOrderHelper", "");

		} else {
			LOGEVENT(HlpStruct::CommonLog::LL_INFO,  SMARTCOMPROXY, "Cancel all orders");

			// cancel all orders
			HRESULT hr = smartComEngine_m->cancelAllOrders(
				cookie,
				(IMainSmartComEngineCallback*)this
				);

			if (FAILED(hr))
				THROW(CppUtils::OperationFailed, "exc_CannotIssueCancelAllOrder", "cxt_issueCancelOrderHelper", "");

		}

	}


	int SmartComBroker::getOrderExpirationTime(BrkLib::Order const& order) const
	{
		// this is how 
		int oFlag = (int)order.orderValidity_m ;

		if (oFlag == OV_DAY)
			return StOrder_Validity_Day;
		else if (oFlag == OV_GTC)
			return StOrder_Validity_Gtc;
		else
			THROW(CppUtils::OperationFailed, "exc_InvalidOrderValidity", "ctx_getOrderExpirationTime", (long)order.orderValidity_m );
	}



	void SmartComBroker::propagateThroughAllSessions(BrokerResponseBase const& resp)
	{
		//LOCK_FOR_SCOPE(*this);
		for(map<CppUtils::Uid, SmartComBrokerSession*>::iterator it = sessions2_m.begin(); it != sessions2_m.end(); it++) {
			BrokerBase::onBrokerResponseArrived(*it->second, resp );
		}
	}

	void SmartComBroker::prepareBrokerResponseOrder_FromOrder(BrkLib::Order const& order, BrkLib::BrokerResponseOrder& orderResp) const
	{
		orderResp.parentID_m = order.ID_m;
		orderResp.provider_m = order.provider_m;
		orderResp.symbol_m = order.symbol_m;


	}

	void SmartComBroker::sendErrorResponse(
		int const cookie, 
		CppUtils::String const& reason, 
		int const errcode,
		BrkLib::OrderType const ordertype
		)
	{
		BrokerResponseOrder bresp;
		bresp.ID_m.generate();


		bresp.brokerInternalCode_m = errcode;
		bresp.resultCode_m = PT_GENERAL_ERROR_HAPPENED;
		bresp.resultDetailCode_m = ODR_GENERAL_ERROR;
		bresp.brokerComment_m = reason;
		bresp.opOrderType_m = ordertype;
		bresp.brokerIssueTime_m = CppUtils::getTime();

		map<long, OrderHolder>::const_iterator it = sessionsForCookies_m.find(cookie);
		if (it != sessionsForCookies_m.end()) {

			prepareBrokerResponseOrder_FromOrder(it->second.order_m, bresp);

			// here is only cause context
			bresp.context_m = it->second.order_m.context_m;
			BrokerBase::onBrokerResponseArrived(*it->second.session_m, bresp );
		}
		else {
			LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  SMARTCOMPROXY, "Cannot find parent session for cookie: " << cookie);
		}


	}

	// ---------------------------------------
	// callbacks
	// ---------------------------------------

	HRESULT SmartComBroker::onOrderSucceded(
		long cookie, 
		BSTR symbol,
		long state,
		long action,
		long type,
		long validity,
		double price,
		double amount,
		double stop,
		double filled,
		double datetime,
		BSTR orderid,
		BSTR orderno
		)
	{
		TRY {
			BrokerResponseOrder bresp;
			bresp.ID_m.generate();


			//LOCK_FOR_SCOPE(*this); 



			map<long, OrderHolder>::const_iterator it = sessionsForCookies_m.find(cookie);
			if (it != sessionsForCookies_m.end() ) {
				OrderHolder const& orderHolder_i = it->second;
				prepareBrokerResponseOrder_FromOrder(orderHolder_i.order_m, bresp);

				// state
				switch(state) {
					case StOrder_State_ContragentReject: 
						bresp.resultCode_m = PT_GENERAL_ERROR_HAPPENED;
						bresp.resultDetailCode_m = ODR_GENERAL_ERROR;
						bresp.brokerComment_m = "ContragentReject";

						break;
					case StOrder_State_Submited: 
						bresp.resultCode_m = PT_ORDER_ESTABLISHED;
						bresp.resultDetailCode_m = ODR_ORDER_SUBMITTED;
						bresp.brokerComment_m = "Submitted";

						break;
					case StOrder_State_Pending: 
						bresp.resultCode_m = PT_ORDER_ESTABLISHED;
						bresp.resultDetailCode_m = ODR_ORDER_PENDING;
						bresp.brokerComment_m = "Pending";

						break;
					case StOrder_State_Open: 

						bresp.resultCode_m = PT_ORDER_ESTABLISHED;
						bresp.resultDetailCode_m = ODR_ORDER_INSTALLED;
						bresp.brokerComment_m = "Open";


						break;
					case StOrder_State_Expired: 
						bresp.resultCode_m = PT_ORDER_CANCELLED;
						bresp.resultDetailCode_m = ODR_OK;
						bresp.brokerComment_m = "Expired";

						break;
					case StOrder_State_Cancel: 
						bresp.resultCode_m = PT_ORDER_CANCELLED;
						bresp.resultDetailCode_m = ODR_OK;
						bresp.brokerComment_m = "Cancel";

						break;
					case StOrder_State_Filled: 

						bresp.resultCode_m = PT_ORDER_ESTABLISHED;
						bresp.resultDetailCode_m = ODR_ORDER_EXECUTED;
						bresp.brokerComment_m = "Filled";
												
						//LOG(MSG_INFO, "foo", "ODR_ORDER_EXECUTED - filled" );
						// we have AddTrade
						return S_OK;

						break;
					case StOrder_State_Partial: 
						bresp.resultCode_m = PT_ORDER_ESTABLISHED;
						bresp.resultDetailCode_m = ODR_ORDER_EXECUTED;
						bresp.brokerComment_m = "Partial";

						//LOG(MSG_INFO, "foo", "ODR_ORDER_EXECUTED - partial" );
						// we have AddTrade
						return S_OK;

						break;
					case StOrder_State_ContragentCancel:
						bresp.resultCode_m = PT_GENERAL_ERROR_HAPPENED;
						bresp.resultDetailCode_m = ODR_GENERAL_ERROR;
						bresp.brokerComment_m = "ContragentCancel";

						break;
					case StOrder_State_SystemReject: 
						bresp.resultCode_m = PT_GENERAL_ERROR_HAPPENED;
						bresp.resultDetailCode_m = ODR_GENERAL_ERROR;
						bresp.brokerComment_m = "SystemReject";

						break;
					case StOrder_State_SystemCancel: 
						bresp.resultCode_m = PT_ORDER_CANCELLED;
						bresp.resultDetailCode_m = ODR_OK;
						bresp.brokerComment_m = "SystemCancel";	 

						break;
					default: 
						break;
				}	



				bresp.brokerIssueTime_m = CppUtils::getTime();
				bresp.opTime_m = CppUtils::convertOleTimeToUnix(datetime);
				bresp.brokerOrderID_m = CW2A(orderid);
				bresp.opOrderType_m = smartComOperationFlagsToOrderType( useOnlyOpenPosCommands(), type, action, orderHolder_i.orderType_m );
				bresp.context_m = orderHolder_i.order_m.context_m;


				if (bresp.resultCode_m == PT_ORDER_CANCELLED) {
					// must be order cause reason
					map<CppUtils::String, long>::const_iterator it2= cancellations_m.find(bresp.brokerOrderID_m);
					if (it2 != cancellations_m.end()) {
						// need to find the order who caused cancellation
						map<long, OrderHolder>::const_iterator it3 = sessionsForCookies_m.find(it2->second);
						if (it3 != sessionsForCookies_m.end()) {
							bresp.causeContext_m = it3->second.order_m.context_m;
							LOGEVENT(HlpStruct::CommonLog::LL_DEBUG,  SMARTCOMPROXY, "On cancellation cause context was found for order id:" << bresp.brokerOrderID_m);
						}
					}
				}

				bresp.opPrice_m = price;
				bresp.opStopPrice_m  = stop;
				// based on input order

				CppUtils::String orderno_s = CW2A(orderno);
				if (bresp.resultDetailCode_m == ODR_ORDER_INSTALLED)	 {
					// filter out if invalid orderno
					if (orderno_s.size() <=0 || orderno_s == "0")
						return S_OK;
				}

				bresp.brokerComment_m.append(" [orderid=");
				bresp.brokerComment_m.append(CW2A(orderid));
				bresp.brokerComment_m.append("] [orderno=");
				bresp.brokerComment_m.append(orderno_s);
				bresp.brokerComment_m.append("] [order type=");
				bresp.brokerComment_m.append(BrkLib::orderTypeBitwise2String(bresp.opOrderType_m));
				bresp.brokerComment_m.append("]"); 


				if (validity == StOrder_Validity_Day)
					bresp.opOrderValidity_m = BrkLib::OV_DAY;

				if (validity == StOrder_Validity_Gtc)
					bresp.opOrderValidity_m = BrkLib::OV_GTC;


				// go with volume
				bresp.opRemainingVolume_m =  filled;


				BrokerBase::onBrokerResponseArrived(*it->second.session_m, bresp );
			} else {
				LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  SMARTCOMPROXY, "Cannot find parent session for cookie: " << cookie);
			}


		}
		CATCH_ALL
			return S_OK;
	}

	HRESULT SmartComBroker::onOrderFailed(long cookie, BSTR orderid, BSTR reason)
	{
		TRY {
			BrokerResponseOrder bresp;
			bresp.ID_m.generate();

			//LOCK_FOR_SCOPE(*this);

			map<long, OrderHolder>::const_iterator it = sessionsForCookies_m.find(cookie);
			if (it != sessionsForCookies_m.end() ) {
				OrderHolder const& oholder = it->second;
				prepareBrokerResponseOrder_FromOrder(oholder.order_m, bresp);
				bresp.resultCode_m = PT_GENERAL_ERROR_HAPPENED;
				bresp.resultDetailCode_m = ODR_GENERAL_ERROR;

				// here we have base context as no causeing context
				bresp.context_m = oholder.order_m.context_m;

				bresp.brokerComment_m.append("[");
				bresp.brokerComment_m.append(CW2A(reason, 1251));
				bresp.brokerComment_m.append("] [orderid=");
				bresp.brokerComment_m.append(CW2A(orderid));
				bresp.brokerComment_m.append("]");

				// this is dependiong on position type of parent order
				bresp.opOrderType_m = oholder.orderType_m;
				bresp.brokerOrderID_m = CW2A(orderid);
				bresp.brokerIssueTime_m = CppUtils::getTime();

				BrokerBase::onBrokerResponseArrived(*it->second.session_m, bresp );
			} else {
				LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  SMARTCOMPROXY, "Cannot find parent session for cookie: " << cookie);
			}
		}
		CATCH_ALL
			return S_OK;
	}


	HRESULT SmartComBroker::onConnect()
	{
		TRY {
			
		}
		CATCH_ALL
			return S_OK;
	}

	HRESULT SmartComBroker::onDisconnect(BSTR reason)
	{
		TRY {

		}
		CATCH_ALL

			return S_OK;
	}

	HRESULT SmartComBroker::onSetMyTrade(
			BSTR portfolio, 
			BSTR symbol, 
			double datetime, 
			double price, 
			double volume, 
			BSTR tradeno, 
			long buysell, 
			BSTR orderno,
			BSTR orderid
		)
	{
		
		TRY {
			
			BrokerResponsePositionOperation brespPosOp;
			brespPosOp.ID_m.generate();

			brespPosOp.resultCode_m = PT_GENERAL_INFO;
			brespPosOp.resultDetailCode_m = ODR_OK;

			brespPosOp.brokerOrderID_m = CW2A(orderid);
			brespPosOp.brokerPositionID_m = CW2A(tradeno);
			brespPosOp.symbol_m = CW2A(symbol);
			brespPosOp.brokerIssueTime_m = CppUtils::getTime();
		
			brespPosOp.price_m = price;
			brespPosOp.volume_m = volume;

			if (buysell == StOrder_Action_Buy) {
				brespPosOp.direction_m = BrkLib::TD_LONG;	  
			}
			else if (buysell == StOrder_Action_Sell) {
				brespPosOp.direction_m = BrkLib::TD_SHORT;
			}
			else {
				LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  SMARTCOMPROXY, "Invalid type of bysell in onSetMyTrade(...)");
				return S_OK;
			}

			/*
			LOG(MSG_INFO, "foo", "onSetMyTrade(...) orderid: " << brespPosOp.brokerOrderID_m <<
				" position id: " << brespPosOp.brokerPositionID_m << " symbol: " << brespPosOp.symbol_m <<
				" volume: " << volume << " price: " << price << 
				" direction: " << BrkLib::tradeDirection2String(brespPosOp.direction_m));
			*/

			propagateThroughAllSessions(brespPosOp);
			
		}
		CATCH_ALL

		return S_OK;
	}
																					 
	HRESULT SmartComBroker::onAddTrade(
		long cookie,
		BSTR portfolio,
		BSTR symbol,
		BSTR orderno,
		BSTR orderid,
		double price,
		double amount,
		double datetime,
		BSTR tradeno
		)
	{
		TRY {

			BrokerResponseOrder bresp;
			bresp.ID_m.generate();


			//LOCK_FOR_SCOPE(*this); 



			map<long, OrderHolder>::const_iterator it = sessionsForCookies_m.find(cookie);
			if (it != sessionsForCookies_m.end() ) {
				OrderHolder const& orderHolder_i = it->second;
				prepareBrokerResponseOrder_FromOrder(orderHolder_i.order_m, bresp);

				// resolved parent order
				bresp.resultCode_m = PT_ORDER_ESTABLISHED;
				bresp.resultDetailCode_m = ODR_ORDER_EXECUTED;
				bresp.brokerComment_m = "AddTrade";
				bresp.context_m = orderHolder_i.order_m.context_m;

				bresp.brokerIssueTime_m = CppUtils::getTime();
				bresp.opTime_m = CppUtils::convertOleTimeToUnix(datetime);
				bresp.opPrice_m = price; 
				bresp.opVolume_m = abs(amount);

				bresp.brokerOrderID_m = CW2A(orderid);
				bresp.brokerPositionID_m = CW2A(tradeno);

				//LOG(MSG_INFO, "foo", "brespoposid: " << bresp.brokerPositionID_m );

				bresp.opOrderType_m= BrkLib::orderTypeClearance(orderHolder_i.orderType_m);
				if (bresp.opOrderType_m == BrkLib::OP_DUMMY) {
					LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  SMARTCOMPROXY, "Invalid response type installed by order with orderid " << orderid);
					return S_OK;
				}


				bresp.brokerComment_m.append(" [orderid=");
				bresp.brokerComment_m.append(CW2A(orderid));
				bresp.brokerComment_m.append("] [orderno=");
				bresp.brokerComment_m.append(CW2A(orderno));
				bresp.brokerComment_m.append("]");

				BrokerBase::onBrokerResponseArrived(*it->second.session_m, bresp );


			}

		}
		CATCH_ALL

			return S_OK;
	}

	HRESULT SmartComBroker::onFinishBarsHistory(
			int cookie,
			BSTR symbol,
			VARIANT_BOOL result,
			long count,
			IDispatch* eventClientPtr
		)

	{
	
		// finished one
		LOGEVENT(HlpStruct::CommonLog::LL_INFO,  SMARTCOMPROXY, "onFinishBarsHistory(...), symbol: " << CW2A(symbol) << 
			", cookie: " << cookie <<
			", count: " << count
		);
						
		HistoryRetrivalTask e;
		bool proceed = false;
		{
			LOCK_FOR_SCOPE(hstRetrStruct_m.hstRetrTaskMtx_m);

			if (hstRetrStruct_m.hstRetrMap_m.size() > 0) {
					
				proceed = true;

				// copy
				e = hstRetrStruct_m.hstRetrMap_m.begin()->second;

				// remove
				hstRetrStruct_m.hstRetrMap_m.erase( e.cookie_m );
				
			}
		}

		if (proceed) {

			CComQIPtr<SmartComEngine::IMainSmartComEngine> ptr = eventClientPtr;
			if (ptr.p == NULL) {
				LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  SMARTCOMPROXY, "Invalid client ptr"); 
				return S_OK;
			}

			HRESULT hr = ptr->getBarsHistory(
				e.symbol_m, 
				e.interval_m,
				e.from_date_m,
				e.from_count_m,
				e.cookie_m
			);											   

			if (FAILED(hr)) {
				LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  SMARTCOMPROXY, "Cannot start history retrieve for symbol: " << CW2A(e.symbol_m)); 
			}
			else {
				LOGEVENT(HlpStruct::CommonLog::LL_INFO,  SMARTCOMPROXY, "Requested history for symbol: " << CW2A(e.symbol_m) <<
					", interval: " << e.interval_m <<
					", from date: " << CppUtils::getGmtTime2(CppUtils::convertOleTimeToUnix(e.from_date_m)) <<
					", from count: " << e.from_count_m <<
					", cookie: " << e.cookie_m
				); 

			}

			

		}

												    
		return S_OK;
	}

	
	HRESULT SmartComBroker::onTimer_1Sec()
	{ 
	
		/*
		HistoryRetrivalTask* e = returnNextHistTask();
	
		if (e) {
			CComPtr<SmartComEngine::IMainSmartComEngine> p = SmartComBroker::getMarhshalledPointer_smartComEngine();

			HRESULT hr = p->getBarsHistory(
				e->symbol_m, 
				e->interval_m,
				e->from_date_m,
				e->from_count_m,
				e->cookie_m
			);											   

			if (FAILED(hr)) {
				LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  SMARTCOMPROXY, "Cannot start history retrieve for symbol: " << CW2A(e->symbol_m)); 
			}
			else {
				LOGEVENT(HlpStruct::CommonLog::LL_INFO,  SMARTCOMPROXY, "Requested history for symbol: " << CW2A(e->symbol_m) <<
					", interval: " << e->interval_m <<
					", from date: " << CppUtils::getGmtTime2(CppUtils::convertOleTimeToUnix(e->from_date_m)) <<
					", from count: " << e->from_count_m <<
					", cookie: " << e->cookie_m
				); 

			}
		
			delete e;
		}

		*/
		
		
		return S_OK;
	};

	/*
	CComPtr<SmartComEngine::IMainSmartComEngine> SmartComBroker::getMarhshalledPointer_smartComEngine()
	{
		if (globalGIT_m == NULL)
			THROW(CppUtils::OperationFailed, "exc_GITisNULL", "ctx_getMarhshalledPointer_smartComEngine", "");

		CComPtr<SmartComEngine::IMainSmartComEngine> srvmarshalled;
		HRESULT hr = globalGIT_m->GetInterfaceFromGlobal(smartComEnginePtrCookie_m, __uuidof(SmartComEngine::IMainSmartComEngine), (void**)&srvmarshalled);

		if (!SUCCEEDED(hr))
			THROW(CppUtils::OperationFailed, "exc_CannotResolveGITPtr", "ctx_getMarhshalledPointer_smartComEngine", "");

		if (srvmarshalled== NULL)
			THROW(CppUtils::OperationFailed, "exc_GITPtrIsNull", "ctx_getMarhshalledPointer_smartComEngine", "");

		return srvmarshalled;
	}
	*/
}; // end of namespace