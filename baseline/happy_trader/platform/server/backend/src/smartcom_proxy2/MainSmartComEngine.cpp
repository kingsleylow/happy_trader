// MainSmartComEngine.cpp : Implementation of CMainSmartComEngine

#include "smartcom_proxy2defs.h"
#include "MainSmartComEngine.h"

// testframe handlers
#ifdef HANDLE_NATIVE_EXCEPTIONS
#include "shared/testframe/src/testsysdep.hpp"
#include "shared/testframe/src/testexcept.hpp"

#endif

// algorithm holder structure
#include "platform\server\backend\src\inqueue\inqueue.hpp"
#include "platform\server\backend\src\inqueue\commontypes.hpp"
#include "platform\server\backend\src\inqueue\outproc.hpp"

#include "../alglib/alglib.hpp"
#include "../brklib/brklib.hpp"







#define SMARTCOM "SMARTCOM"
#define INIT_TOUT 20

// synch types
#define SYNC_EVENT_PROVIDER_START_CONST  1
#define SYNC_EVENT_PROVIDER_FINISH_CONST 2
#define SYNC_EVENT_PROVIDER_TRANSMIT_ERROR_CONST 3
#
// this will be used to initializa all trading stuff here
#define SYNC_EVENT_CUSTOM_MESSAGE_CONST 4 


#define DELEGATE_ALL_CLIENTS(FUN)	\
	{	/*LOCK_FOR_SCOPE(mtxCallbacks_m);*/ \
	for(map<long, IMainSmartComEngineCallback*>::iterator it = callbacks_m.begin(); it != callbacks_m.end(); it++)	\
		{	\
		IMainSmartComEngineCallback* ptr = it->second;	\
		ptr->FUN;	\
		}	\
	};

#define DELEGATE_COOKIE_CLIENTS(COOKIE, FUN)	\
	{	\
		map<long, IMainSmartComEngineCallback*>::iterator it = orderSources_m.end();	\
		{	\
			LOCK_FOR_SCOPE(mtxOrderSources_m);	\
			it = orderSources_m.find(COOKIE);	\
		}	\
		if (it != orderSources_m.end()) {	\
			IMainSmartComEngineCallback* ptr = it->second;	\
			ptr->FUN;	\
		};	\
  };



namespace SmartComHelperLib {


	long CMainSmartComEngine::subscrCounter_m = 0;

	// CMainSmartComEngine

	int bootThread (void *ptr)
	{
		// handle run-time exceptions
#ifdef HANDLE_NATIVE_EXCEPTIONS
		Testframe::init();
#endif

		CMainSmartComEngine* pThis = (CMainSmartComEngine*)ptr; 

		try {
			pThis->threadRun();
		}
		catch(CppUtils::Exception& e) {
			LOG(MSG_ERROR, SMARTCOM, "Fatal exception: " << e.message());

		}
#ifdef HANDLE_NATIVE_EXCEPTIONS
		catch(Testframe::Exception& e) {
			LOG(MSG_ERROR, SMARTCOM, "Fatal MSVC exception: " << e.message());

		}
#endif
		catch(...) {
			LOG(MSG_ERROR, SMARTCOM, "Unknown fatal exception");

		}

		return 42;

	}



	CMainSmartComEngine::CMainSmartComEngine():
	refCount_m(0),
		IStServerPtrCookie_m(0),
		dlg_m(*this),
		accompanyTicks1minBar_m(false),
		threadId_m(0),
		thread_m(NULL)
		//makeShortSell_m(false)
	{
		NOTIFICATION(N_OK, "Idle", "В ленивом состоянии :)");
		LOG( MSG_INFO, SMARTCOM, "Engine created");
	}

	CMainSmartComEngine::~CMainSmartComEngine()
	{
		LOG( MSG_INFO, SMARTCOM, "Engine released");
	}


	// 
	void CMainSmartComEngine::threadRun()
	{
		LOG( MSG_INFO, SMARTCOM, "Starting working thread" );


		try {
			HRESULT r = ::CoInitialize(NULL);
			if (!SUCCEEDED(r))
				THROW(CppUtils::OperationFailed, "exc_CannotCoInitialize", "ctx_CMainSmartComEngine_threadRun", "" );

			threadId_m = ::GetCurrentThreadId();

			MSG iMsg;
			BOOL bRet;			  
			// create MSG queue
			::PeekMessage(&iMsg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

			try {											 
				initialize();
				NOTIFICATION1(N_OK, "Up and running");
			}
			catch(CppUtils::Exception& e)
			{
				LOG(MSG_ERROR, SMARTCOM, "Exception on initialization: " << e.message());
				NOTIFICATION(N_ERROR, "Error on initialization", e.message() << " - " << e.arg() << " - " << e.context() );
			}
#ifdef HANDLE_NATIVE_EXCEPTIONS
			catch(Testframe::Exception& e) {
				LOG(MSG_ERROR, SMARTCOM, "Fatal MSVC exception on initialization: " << e.message());
				NOTIFICATION(N_ERROR, "Fatal MSVC exception on initialization", e.message().c_str());
			}
#endif
			catch(...)
			{
				LOG(MSG_ERROR, SMARTCOM, "Unknown exception on initialization");
				NOTIFICATION1(N_ERROR, "Unknow exception on initialization" );
			}


			while( (bRet = ::GetMessage( &iMsg, NULL, 0, 0 )) != 0)
			{ 
				if (bRet == -1)
				{
					// handle the error and possibly exit
					break;
				}										 
				else  
				{
					::DispatchMessage(&iMsg); 
				}
			};


		}
		catch(CppUtils::Exception& e) {
			LOG(MSG_ERROR, SMARTCOM, "Fatal exception in message thread: " << e.message());
			NOTIFICATION(N_ERROR, "Fatal exception in message thread", e.message().c_str());
		}
#ifdef HANDLE_NATIVE_EXCEPTIONS
		catch(Testframe::Exception& e) {
			LOG(MSG_ERROR, SMARTCOM, "Fatal MSVC exception in message thread: " << e.message());
			NOTIFICATION(N_ERROR, "Fatal MSVC exception in message thread", e.message().c_str());
		}
#endif
		catch(...) {
			LOG(MSG_ERROR, SMARTCOM, "Unknown fatal exception in message thread");
			NOTIFICATION1(N_ERROR, "Unknown fatalexception in message thread");
		}

		// need to notify we are down
		NOTIFICATION1(N_OK, "Idle");

		// ignore any errors
		// in the future probably log it
		try {
			deinitialize();
		}
		catch(CppUtils::Exception& e)
		{
			LOG(MSG_ERROR, SMARTCOM, "Exception on deinitialization: " << e.message());
			NOTIFICATION(N_ERROR, "Error on deinitialization", e.message() << " - " << e.arg() << " - " << e.context() );
		}
#ifdef HANDLE_NATIVE_EXCEPTIONS
		catch(Testframe::Exception& e) {
			LOG(MSG_ERROR, SMARTCOM, "Fatal MSVC exception on deinitialization: " << e.message());
			NOTIFICATION(N_ERROR, "Fatal MSVC exception on deinitialization", e.message().c_str());
		}
#endif
		catch(...)
		{
			LOG(MSG_ERROR, SMARTCOM, "Unknown fatal exception on deinitialization");
			NOTIFICATION1(N_ERROR, "Unknown fatal exception on deinitialization");
		}



		::CoUninitialize();


		LOG( MSG_INFO, SMARTCOM, "Finishing working thread" );

	}




	// ------------------------------
	// THIS CLASS INTERFACE METHODS
	// ------------------------------


	HRESULT CMainSmartComEngine::libInit(IMainSmartComEngineCallback* eventClient, long* cookie)
	{
		//LOCK_FOR_SCOPE(*this);
		// need to subscribe
		*cookie = ++subscrCounter_m;

		{
			LOCK_FOR_SCOPE(mtxCallbacks_m);
			callbacks_m[ *cookie ] = eventClient;

		}

		if (refCount_m++==0) {

			connected_m = false;

			// clear if signalled
			if (initialized_m.lock(0))
				initialized_m.clearEvent();


			// initialize thread
			thread_m = new CppUtils::Thread(bootThread, this);


			// wait until initialized
			if (!initialized_m.lock(INIT_TOUT * 1000)) {
				LOG( MSG_INFO, SMARTCOM, "Error engine initialization");
				return E_FAIL;

			}

			LOG( MSG_INFO, SMARTCOM, "Trying to gracefully disconnect");

			silentlyDisconnectBeforeStartEnd();

			LOG( MSG_INFO, SMARTCOM, "Engine initialized");
		}

		return S_OK;

	}

	HRESULT CMainSmartComEngine::libDeinit(long cookie)
	{
		//LOCK_FOR_SCOPE(*this);

		{
			LOCK_FOR_SCOPE(mtxCallbacks_m);
			callbacks_m.erase(cookie);

		}

		if (--refCount_m <= 0) {
			try {


				LOG( MSG_INFO, SMARTCOM, "Trying to gracefully disconnect");
				silentlyDisconnectBeforeStartEnd();


				if (threadId_m == 0)
					THROW(CppUtils::OperationFailed, "exc_CannotResolveThreadId", "ctx_destroyEngine", CppUtils::getOSError());

				::PostThreadMessage(threadId_m, WM_QUIT, 0, 0);

				if (!thread_m->waitForThread(INIT_TOUT * 1000)) {
					LOG(MSG_FATAL, SMARTCOM, "Cannot stop working thread, aborting..." );
				}
				else {
					delete thread_m;
				}

				thread_m = NULL;


			}
			catch(...)
			{
				// ignore
			}

			LOG( MSG_INFO, SMARTCOM, "Engine deinitialized");

			return S_OK;
		}

		return S_FALSE;

	}




	HRESULT CMainSmartComEngine::connect( 
		BSTR rtProvider, 
		BSTR rtProviderBarPostfix, 
		VARIANT_BOOL accompanyTicks1minBar, 
		BSTR host, 
		SHORT port, 
		BSTR login, 
		BSTR password,
		SAFEARRAY * portfolioList,
		long loglevel
		)
	{
		//LOCK_FOR_SCOPE(*this);


		if (!checkLayerInitialized()) 	
			return E_FAIL;

		// configure
		CComPtr<IStServer> srvmarshalled = getMarhshalledPointer_IStServer();

		CppUtils::String base = CppUtils::extractFilePathFromName(CppUtils::Log::getLogFileName());

		int loglevel_i;

		if (loglevel >=6)
			loglevel_i = 6;
		else if (loglevel <= 2)
			loglevel_i = 2;
		else
			loglevel_i = (int)loglevel;

		CppUtils::String serverCfg = "logFilePath=" + base + ";logLevel="+CppUtils::long2String(loglevel_i);
		CppUtils::String clientCfg = "logFilePath=" + base + ";logLevel="+CppUtils::long2String(loglevel_i);

		NOTIFICATION(N_OK, "Server cfg string: ", serverCfg.c_str() );
		NOTIFICATION(N_OK, "Client cfg string: ", clientCfg.c_str() );
		NOTIFICATION(N_OK, "Login: ", bstrToString(login) );
		//NOTIFICATION(N_OK, "Password: ", bstrToString(password) );
		NOTIFICATION(N_OK, "Password: ", "***");

		//LOG(MSG_INFO, SMARTCOM, "Server config string: " << serverCfg);
		//LOG(MSG_INFO, SMARTCOM, "Client config string: " << clientCfg);


		HRESULT hr = srvmarshalled->ConfigureServer(CComBSTR(serverCfg.c_str()));
		if (!SUCCEEDED(hr))	{
			NOTIFICATION1(N_ERROR, "Cannot call ConfigureServer(...), will not proceed");
			return hr;
		}


		hr = srvmarshalled->ConfigureClient(CComBSTR(clientCfg.c_str()));
		if (!SUCCEEDED(hr)) {
			NOTIFICATION1(N_ERROR, "Cannot call ConfigureClient(...), will not proceed");
			return hr;
		}



		subscribedForPortfolio_m = false;
		subscribedForGetMyTradesPollSec_m = -1;
		connected_m = false;

		//makeShortSell_m = (makeShortSell == VARIANT_TRUE);

		rtProvider_m = bstrToString(rtProvider);
		rtProviderBars_m = rtProvider_m + bstrToString(rtProviderBarPostfix);

		CComSafeArray<BSTR> portfolio_list_b;
		hr = portfolio_list_b.Attach(portfolioList);
		if (FAILED(hr)) {
			NOTIFICATION1(N_ERROR, "Cannot collect portfolio list");
			return hr;
		}

		if (portfolio_list_b.GetCount() <=0) {
			NOTIFICATION1(N_ERROR, "Portfolio list is empty!");
			return E_FAIL;

		}

		activePortfolios_m.clear();
		for(int i =0 ; i < portfolio_list_b.GetCount(); i++) {
			CComBSTR port_i(portfolio_list_b.GetAt(i));

			activePortfolios_m.push_back(this->bstrToString(port_i));
		}

		

		accompanyTicks1minBar_m = (accompanyTicks1minBar == VARIANT_TRUE);

		// all these methods called from different thread, so need to marshall
		hr = srvmarshalled->connect(host, port, login, password);

		dlg_m.initialize(bstrToString(login).c_str());	

		// in case of reconnect request we must re-subscribe again for our stiff
		NOTIFICATION1(N_OK, "Checking if we must subscribe for ticks/bars, etc...");

		CppUtils::StringSet level1Symbols, tickSymbols, level2Symbols;
		getSubscribedSymbols(level1Symbols, tickSymbols, level2Symbols );

		if (level1Symbols.size() > 0)	{
			NOTIFICATION1(N_OK, "Level 1 subscribed symbols list detected, repeating subscription" );
			for(CppUtils::StringSet::const_iterator it = level1Symbols.begin(); it != level1Symbols.end(); it++) {
					HRESULT hr = srvmarshalled->ListenQuotes(CComBSTR(it->c_str()));
				 if (!SUCCEEDED(hr)) {
					 NOTIFICATION(N_ERROR, "Error Level 1 re-subscribing symbol", *it);
				 } else {
					 NOTIFICATION(N_OK, "Level 1 re-subscribed OK", *it);
				 }
			}
		}

		if (tickSymbols.size() > 0) {
			NOTIFICATION1(N_OK, "Tick symbols detected, repeating subscription" );
			for(CppUtils::StringSet::const_iterator it = tickSymbols.begin(); it != tickSymbols.end(); it++) {
				 
				 HRESULT hr = srvmarshalled->ListenTicks(CComBSTR(it->c_str()));
				 if (!SUCCEEDED(hr)) {
					 NOTIFICATION(N_ERROR, "Error Tick re-subscribing symbol", *it);
				 } else {
					 NOTIFICATION(N_OK, "Tick re-subscribed OK", *it);
				 }
			}
		}

		
		if (level2Symbols.size() > 0) {
			NOTIFICATION1(N_OK, "Level 2 symbols detected, repeating subscription" );
			for(CppUtils::StringSet::const_iterator it = level2Symbols.begin(); it != level2Symbols.end(); it++) {
				 HRESULT hr = srvmarshalled->ListenBidAsks(CComBSTR(it->c_str()));
				 if (!SUCCEEDED(hr)) {
					 NOTIFICATION(N_ERROR, "Error Level 2 re-subscribing symbol", *it);
				 } else {
					 NOTIFICATION(N_OK, "Level 2 re-subscribed OK", *it);
				 }
			}

		}																		  

		return hr;
	}

	HRESULT CMainSmartComEngine::disconnect()
	{
		//LOCK_FOR_SCOPE(*this);

		if (!checkLayerInitialized()) 	
			return E_FAIL;


		CComPtr<IStServer> srvmarshalled = getMarhshalledPointer_IStServer();

		HRESULT hr = srvmarshalled->disconnect();

		return hr;
	}

	HRESULT CMainSmartComEngine::subscribeForTicks(SAFEARRAY * symbols)
	{
		//LOCK_FOR_SCOPE(*this);
		if (!(checkLayerInitialized() && checkLayerConnected())) 	
			return E_FAIL;

		CComSafeArray<BSTR> list_of_symbols_b;
		HRESULT hr = list_of_symbols_b.Attach(symbols);
		if (FAILED(hr))
			return hr;

		CComPtr<IStServer> srvmarshalled = getMarhshalledPointer_IStServer();



		for (int i = 0; i < list_of_symbols_b.GetCount(); i++) {
			CComBSTR elem_i(list_of_symbols_b.GetAt(i));

			// use it
			HRESULT hr = srvmarshalled->ListenTicks(elem_i);
			if (!SUCCEEDED(hr)) {
				NOTIFICATION(N_ERROR, "Error tick subscribing symbol", bstrToString(elem_i));
			} else {
				{
					LOCK_FOR_SCOPE(sybscribedSymbolsMtx_m);
					subscribedSymbolsAsTicks_m.insert(bstrToString(elem_i));
				}

				NOTIFICATION(N_OK, "Subscribed for ticks for symbol",bstrToString(elem_i));
			}

			
		}

		list_of_symbols_b.Detach();
		return S_OK;
	}

	HRESULT CMainSmartComEngine::unsubscribeForTicks(SAFEARRAY * symbols)
	{
		//LOCK_FOR_SCOPE(*this);
		if (!(checkLayerInitialized() && checkLayerConnected())) 	
			return E_FAIL;


		CComSafeArray<BSTR> list_of_symbols_b;
		HRESULT hr = list_of_symbols_b.Attach(symbols);
		if (FAILED(hr))
			return hr;

		CComPtr<IStServer> srvmarshalled = getMarhshalledPointer_IStServer();

		for (int i = 0; i < list_of_symbols_b.GetCount(); i++) {
			CComBSTR elem_i(list_of_symbols_b.GetAt(i));

			// use it
			HRESULT hr = srvmarshalled->CancelTicks(elem_i);
			if (!SUCCEEDED(hr)) {
				NOTIFICATION(N_ERROR, "Error tick unsubscribing symbol", bstrToString(elem_i));
			} else {
				NOTIFICATION(N_OK, "Unsubscribed from tick for symbol",bstrToString(elem_i));
			}

			{
				LOCK_FOR_SCOPE(sybscribedSymbolsMtx_m);
				subscribedSymbolsAsTicks_m.erase(bstrToString(elem_i));
			}
		}

		list_of_symbols_b.Detach();
		return S_OK;
	}

	HRESULT CMainSmartComEngine::querySymbols()
	{
		//LOCK_FOR_SCOPE(*this);
		if (!(checkLayerInitialized() && checkLayerConnected())) 	
			return E_FAIL;


		CComPtr<IStServer> srvmarshalled = getMarhshalledPointer_IStServer();

		dlg_m.clearSymbolList();
		dlg_m.addSymbolHeader(
			"Номер инструмента в справочнике",
			"Всего инструментов в справочнике",
			"Код ЦБ из таблицы котировок SmartTrade",
			"Краткое наименование",
			"Полное наименование",
			"Код типа из справочника SmartTrade",
			"Точность цены",
			"Размер лота ценных бумаг",
			"Цена минимального шага",
			"Минимальный шаг цены",
			"ISN",
			"Наименование площадки",
			"Дата экспирации",
			"Дней до экспирации"
			);

		HRESULT hr = srvmarshalled->GetSymbols();
		return hr;
	}

	HRESULT CMainSmartComEngine::getPrintHistory( 
		SAFEARRAY * symbols,
		DATE from_date,
		long from_count
		)
	{
		//LOCK_FOR_SCOPE(*this);
		if (!checkLayerInitialized()) 	
			return E_FAIL;

		if (!checkLayerConnected())
			return E_FAIL;


		CComSafeArray<BSTR> list_of_symbols_b;
		HRESULT hr = list_of_symbols_b.Attach(symbols);
		if (FAILED(hr))
			return hr;

		CComPtr<IStServer> srvmarshalled = getMarhshalledPointer_IStServer();



		for (int i = 0; i < list_of_symbols_b.GetCount(); i++) {
			CComBSTR elem_i(list_of_symbols_b.GetAt(i));

			// use it
			HRESULT hr = srvmarshalled->GetTrades(elem_i,from_date,from_count );
			if (!SUCCEEDED(hr)) {
				NOTIFICATION(N_ERROR, "Error get tick history for symbol", bstrToString(elem_i));

			}

		}

		list_of_symbols_b.Detach();
		return S_OK;
	}

	HRESULT CMainSmartComEngine::subscribeForLevel1 ( SAFEARRAY * symbols)
	{
		//LOCK_FOR_SCOPE(*this);
		if (!(checkLayerInitialized() && checkLayerConnected())) 	
			return E_FAIL;

		CComSafeArray<BSTR> list_of_symbols_b;
		HRESULT hr = list_of_symbols_b.Attach(symbols);
		if (FAILED(hr))
			return hr;

		CComPtr<IStServer> srvmarshalled = getMarhshalledPointer_IStServer();



		for (int i = 0; i < list_of_symbols_b.GetCount(); i++) {
			CComBSTR elem_i(list_of_symbols_b.GetAt(i));

			// use it
			HRESULT hr = srvmarshalled->ListenQuotes(elem_i);
			if (!SUCCEEDED(hr)) {
				NOTIFICATION(N_ERROR, "Error LEVEL 1 subscribing symbol",bstrToString(elem_i));
			}
			else {
				{
					LOCK_FOR_SCOPE(sybscribedSymbolsMtx_m);
					subscribedSymbolsAsLevel1_m.insert(bstrToString(elem_i));
				}

				NOTIFICATION(N_OK, "Subscribed LEVEL 1 for symbol",bstrToString(elem_i));
			}

			
		}

		list_of_symbols_b.Detach();
		return S_OK;


	}

	HRESULT CMainSmartComEngine::unsubscribeForLevel1 ( SAFEARRAY * symbols)
	{
		//LOCK_FOR_SCOPE(*this);
		if (!(checkLayerInitialized() && checkLayerConnected())) 	
			return E_FAIL;


		CComSafeArray<BSTR> list_of_symbols_b;
		HRESULT hr = list_of_symbols_b.Attach(symbols);
		if (FAILED(hr))
			return hr;

		CComPtr<IStServer> srvmarshalled = getMarhshalledPointer_IStServer();


		for (int i = 0; i < list_of_symbols_b.GetCount(); i++) {
			CComBSTR elem_i(list_of_symbols_b.GetAt(i));


			// use it
			HRESULT hr = srvmarshalled->CancelQuotes(elem_i);
			if (!SUCCEEDED(hr)) {
				NOTIFICATION(N_ERROR, "Error LEVEL 1 unsubscribing symbol", bstrToString(elem_i));
			} else {
				NOTIFICATION(N_OK, "Unsubscribed from LEVEL 1 for symbol",bstrToString(elem_i));
			}

			{
				LOCK_FOR_SCOPE(sybscribedSymbolsMtx_m);
				subscribedSymbolsAsLevel1_m.erase(bstrToString(elem_i));
			}
		}

		list_of_symbols_b.Detach();
		return S_OK;
	}

	HRESULT CMainSmartComEngine::subscribeForLevel2 ( SAFEARRAY * symbols)
	{
		//LOCK_FOR_SCOPE(*this);
		if (!(checkLayerInitialized() && checkLayerConnected())) 	
			return E_FAIL;

		CComSafeArray<BSTR> list_of_symbols_b;
		HRESULT hr = list_of_symbols_b.Attach(symbols);
		if (FAILED(hr))
			return hr;

		CComPtr<IStServer> srvmarshalled = getMarhshalledPointer_IStServer();



		for (int i = 0; i < list_of_symbols_b.GetCount(); i++) {
			CComBSTR elem_i(list_of_symbols_b.GetAt(i));

			// use it
			HRESULT hr = srvmarshalled->ListenBidAsks(elem_i);
			if (!SUCCEEDED(hr)) {
				NOTIFICATION(N_ERROR, "Error LEVEL 2 subscribing symbol",bstrToString(elem_i));
			}
			else {
				{
					LOCK_FOR_SCOPE(sybscribedSymbolsMtx_m);
					subscribedSymbolsAsLevel2_m.insert(bstrToString(elem_i));
				}

				NOTIFICATION(N_OK, "Subscribed for LEVEL 2 for symbol",bstrToString(elem_i));
			}

			
		}

		list_of_symbols_b.Detach();
		return S_OK;
	}

	HRESULT CMainSmartComEngine::unsubscribeForLevel2 ( SAFEARRAY * symbols)
	{
		//LOCK_FOR_SCOPE(*this);
		if (!(checkLayerInitialized() && checkLayerConnected())) 	
			return E_FAIL;


		CComSafeArray<BSTR> list_of_symbols_b;
		HRESULT hr = list_of_symbols_b.Attach(symbols);
		if (FAILED(hr))
			return hr;

		CComPtr<IStServer> srvmarshalled = getMarhshalledPointer_IStServer();


		for (int i = 0; i < list_of_symbols_b.GetCount(); i++) {
			CComBSTR elem_i(list_of_symbols_b.GetAt(i));


			// use it
			HRESULT hr = srvmarshalled->CancelBidAsks(elem_i);
			if (!SUCCEEDED(hr)) {
				NOTIFICATION(N_ERROR, "Error LEVEL 2 unsubscribing symbol", bstrToString(elem_i));
			} else {
				NOTIFICATION(N_OK, "Unsubscribed from LEVEL 2 for symbol",bstrToString(elem_i));
			}

			{
				LOCK_FOR_SCOPE(sybscribedSymbolsMtx_m);
				subscribedSymbolsAsLevel2_m.erase(bstrToString(elem_i));
			}
		}

		list_of_symbols_b.Detach();
		return S_OK;
	}



	HRESULT CMainSmartComEngine::getPortfolioList( )
	{
		//LOCK_FOR_SCOPE(*this);
		if (!(checkLayerInitialized() && checkLayerConnected())) 	
			return E_FAIL;


		CComPtr<IStServer> srvmarshalled = getMarhshalledPointer_IStServer();



		dlg_m.clearPortfolioList();

		HRESULT hr = srvmarshalled->GetPrortfolioList();
		return hr;
	}

	HRESULT CMainSmartComEngine::issueOrder( 
		BSTR portfolio, 
		BSTR symbol,
		int order_action,
		int order_type,
		int validity,
		double price,
		double amount,
		double stop,
		long cookie,
		IMainSmartComEngineCallback* eventClient
		)
	{
		//LOCK_FOR_SCOPE(*this);

		if (!(checkLayerInitialized() && checkLayerConnected() && checkPassedPortfolio(portfolio))) 	
			return E_FAIL;

		

		CComPtr<IStServer> srvmarshalled = getMarhshalledPointer_IStServer();

		// register order issuer
		{
			LOCK_FOR_SCOPE(mtxOrderSources_m)
				orderSources_m[cookie] = eventClient;
		}


		//int adjusted_order_action = order_action;

		/*
		if (!makeShortSell_m) {
		// here we use only StOrder_Action_Buy & StOrder_Action_Sell
		if (order_action == StOrder_Action_Short)
		adjusted_order_action = StOrder_Action_Sell;
		else if (order_action == StOrder_Action_Cover)
		adjusted_order_action = StOrder_Action_Buy;
		}
		*/


		NOTIFICATION(N_OK, "issueOrder(...)", 
			"symbol: "		<<	bstrToString(symbol) <<
			",action: "		<<	tradeAction2String(order_action) << 
			",type: "			<<	tradeType2String(order_type) <<
			",portfolio: " << bstrToString(portfolio) <<
			",validity: " <<	orderValidity2String(validity) <<
			",price: "		<<	price << 
			",amount: "		<<	amount << 
			",stop: "			<<	stop << 
			",cookie: "		<<	cookie
			);




		HRESULT hr = srvmarshalled->PlaceOrder(
			portfolio,
			symbol,
			(StOrder_Action)order_action,
			(StOrder_Type)order_type,
			(StOrder_Validity)validity,
			price,
			amount,
			stop,
			cookie
			);


		return hr;
	}

	HRESULT CMainSmartComEngine::checkConnectStatus()
	{
		//LOCK_FOR_SCOPE(*this);

		if (!checkLayerInitialized()) 	
			return E_FAIL;

		CComPtr<IStServer> srvmarshalled = getMarhshalledPointer_IStServer();

		VARIANT_BOOL connected;
		HRESULT hr = srvmarshalled->IsConnected(&connected);

		if (FAILED(hr))
			return E_FAIL;

		if (connected == VARIANT_FALSE)
			return E_FAIL;

		return S_OK;


	}

	HRESULT CMainSmartComEngine::cancelOrder(
		BSTR portfolio,
		BSTR symbol, 
		BSTR orderId, 
		long cookie,
		IMainSmartComEngineCallback* eventClient
		)
	{
		//LOCK_FOR_SCOPE(*this);

		if (!(checkLayerInitialized() && checkLayerConnected() && checkPassedPortfolio(portfolio))) 	
			return E_FAIL;

		

		CComPtr<IStServer> srvmarshalled = getMarhshalledPointer_IStServer();

		// register order issuer
		{
			LOCK_FOR_SCOPE(mtxOrderSources_m)
				orderSources_m[cookie] = eventClient;
		}

		NOTIFICATION(N_OK, "CancelOrder(...)", 
			" symbol: " << bstrToString(symbol) <<
			" ,orderid: " << bstrToString(orderId)	<<
			"	,portfolio: " << bstrToString(portfolio) <<
			", cookie: " <<	 cookie
			);

		HRESULT hr = srvmarshalled->CancelOrder(
			portfolio,
			symbol,	
			orderId
		);

		return hr;
	}

	HRESULT CMainSmartComEngine::cancelAllOrders(long cookie, IMainSmartComEngineCallback* eventClient)
	{
		//LOCK_FOR_SCOPE(*this);

		if (!(checkLayerInitialized() && checkLayerConnected())) 	
			return E_FAIL;

		

		CComPtr<IStServer> srvmarshalled = getMarhshalledPointer_IStServer();

		// register order issuer
		{
			LOCK_FOR_SCOPE(mtxOrderSources_m)
				orderSources_m[cookie] = eventClient;
		}

		NOTIFICATION1(N_OK, "CancelAllOrders(...)"	);


		HRESULT hr = srvmarshalled->CancelAllOrders();

		return hr;

	}

	HRESULT CMainSmartComEngine::getBarsHistory( 
		BSTR symbol,
		int interval,
		DATE from_date,
		long from_count,
		int cookie
	)
	{
		//LOCK_FOR_SCOPE(*this);


		if (!(checkLayerInitialized() && checkLayerConnected())) 	
			return E_FAIL;

		CComPtr<IStServer> srvmarshalled = getMarhshalledPointer_IStServer();


		// when subscribed just mark this task as pending
		registerHistoryBarRetrievalTask(symbol, interval, cookie );

																						  
		// use it													    
		HRESULT hr = srvmarshalled->GetBars(symbol, (StBarInterval)interval, from_date, from_count );
		if (!SUCCEEDED(hr)) {
				NOTIFICATION(N_ERROR, "Error get bar history for symbol", bstrToString(symbol));

				// unsubscribe this if error
				freeHistoryBarRetrievalTasksForcibly(symbol, interval);

		}
									 

		NOTIFICATION1(N_OK, "History bars for symbol was asked, date: " << CppUtils::getGmtTime2(CppUtils::convertOleTimeToUnix(from_date)) << 
			", bar count: " << from_count <<
			", interval: " << interval <<
			", symbol: " << bstrToString(symbol)
		);


		return S_OK;
	}

	HRESULT CMainSmartComEngine::getMyTradesOnce()
	{
		//LOCK_FOR_SCOPE(*this);

		if (!(checkLayerInitialized() && checkLayerConnected())) 	
			return E_FAIL;
		

		CComPtr<IStServer> srvmarshalled = getMarhshalledPointer_IStServer();

		for(int i = 0; i < activePortfolios_m.size(); i++) {
			HRESULT hr = srvmarshalled->GetMyTrades(CComBSTR(activePortfolios_m[i].c_str()));

			if (!SUCCEEDED(hr)) {
				NOTIFICATION1(N_ERROR, "Error getMyTrades(...) once for portfolio: " << activePortfolios_m[i].c_str());
			}
			else {
				NOTIFICATION1(N_OK, "getMyTrades(...) sent OK for portfolio: " << activePortfolios_m[i].c_str());
			}
		}

		return S_OK;
	}

	HRESULT	CMainSmartComEngine::subscribeGetMyTrades(long pollPeriodSec)
	{
		//LOCK_FOR_SCOPE(*this);

		if (!(checkLayerInitialized() && checkLayerConnected())) 	
			return E_FAIL;


		{
			LOCK_FOR_SCOPE(subscribedForGetMyTradesMtx_m) 
			subscribedForGetMyTradesPollSec_m = pollPeriodSec;
			subscribedForGetMyTradesCurSec_m = subscribedForGetMyTradesPollSec_m;
		}

		NOTIFICATION1(N_OK, "Subscribed for getMyTrades(...), period sec: " << pollPeriodSec );
		return S_OK;
	}

	HRESULT	CMainSmartComEngine::unsubscribeGetMyTrades()
	{
		//LOCK_FOR_SCOPE(*this);

		if (!(checkLayerInitialized() && checkLayerConnected())) 	
			return E_FAIL;

		{
			LOCK_FOR_SCOPE(subscribedForGetMyTradesMtx_m)
				subscribedForGetMyTradesPollSec_m = -1;
		}

		NOTIFICATION1(N_OK, "Unsubscribed from getMyTrades(...)");
		return S_OK;
	}

	// ----------------

	void CMainSmartComEngine::getSubscribedSymbols(CppUtils::StringSet& level1Symbols, CppUtils::StringSet& ticksSymbols, CppUtils::StringSet& level2Symbols)
	{
		LOCK_FOR_SCOPE(sybscribedSymbolsMtx_m);

		level1Symbols = subscribedSymbolsAsLevel1_m;
		ticksSymbols = subscribedSymbolsAsTicks_m;
		level2Symbols = subscribedSymbolsAsLevel2_m;
	}

	// ------------------------------ 
	//
	// ------------------------------





	// ------------------------------
	// THIS CLASS CALLSBACK
	// ------------------------------




	STDMETHODIMP CMainSmartComEngine::Connected()
	{

		NOTIFICATION1(N_OK, "Connected");

		onConnected();

		Inqueue::OutProcessor::send_rt_provider_synch(CppUtils::getTime(),rtProvider_m.c_str(), SYNC_EVENT_PROVIDER_START_CONST, "SmartCom provider connected" );

		dlg_m.setConnectionStatus(true);
		DELEGATE_ALL_CLIENTS(onConnect());

		return S_OK;
	}

	STDMETHODIMP CMainSmartComEngine::Disconnected(BSTR reason)
	{

		if (!disconnectEvent_m.lock(0))
			disconnectEvent_m.signalEvent();


		onDisconnected();
		NOTIFICATION(N_OK, "Disconnected", bstrToString(reason));
		dlg_m.setConnectionStatus(false);


		Inqueue::OutProcessor::send_rt_provider_synch(CppUtils::getTime(),rtProvider_m.c_str(), SYNC_EVENT_PROVIDER_FINISH_CONST, "SmartCom provider disconnected" );

		DELEGATE_ALL_CLIENTS(onDisconnect(reason));

		return S_OK;
	}

	STDMETHODIMP CMainSmartComEngine::UpdateQuote(
		BSTR symbol, 
		DATE datetime, 
		double open, 
		double high, 
		double low, 
		double close, 
		double last, 
		double volume, 
		double size, 
		double bid, 
		double ask, 
		double bidsize, 
		double asksize, 
		double open_int, 
		double go_buy, 
		double go_sell, 
		double go_base, 
		double go_base_backed, 
		double high_limit, 
		double low_limit, 
		long trading_status, 
		double volat, 
		double theor_price
		)
	{


		Inqueue::OutProcessor::send_level1_data(
			CppUtils::convertOleTimeToUnix(datetime),
			rtProvider_m.c_str(), 
			CW2A(symbol), 
			bid,
			bidsize,
			ask,
			asksize,
			open_int
			);


		return S_OK;
	}
	STDMETHODIMP CMainSmartComEngine::UpdateBidAsk(BSTR symbol, 
		long row, 
		long nrows, 
		double bid, 
		double bidsize, 
		double ask, 
		double asksize
		)
	{

		// LEVEL2
		CppUtils::String symbol_s = bstrToString(symbol);



		map<CppUtils::String, Level2Data>::iterator it = level2data_m.find(symbol_s);
		if (it==level2data_m.end()) {

			level2data_m[symbol_s].level2_m.setProvider(rtProvider_m.c_str());

			it = level2data_m.find(symbol_s);
			HTASSERT(it!=level2data_m.end());

			it->second.level2_m.setSymbol(symbol_s.c_str());
		}


		Level2Data& resolved = it->second;

		if (row == 0) {
			// reset
			resolved.levelSorted_m.clear();

			// best ask and best bid
		}


		Level2EntryExtended entry_bid(row == 0, bidsize,bid,false,HlpStruct::Level2Entry::IS_BID );
		Level2EntryExtended entry_ask(row == 0, asksize,ask,false,HlpStruct::Level2Entry::IS_ASK);

		// need to collect this in order
		resolved.levelSorted_m.insert(pair<double, Level2EntryExtended>(bid,entry_bid ));	
		resolved.levelSorted_m.insert(pair<double, Level2EntryExtended>(ask,entry_ask ));	


		//LOG(MSG_INFO, SMARTCOM, "Symbol: " << symbol_s << " bid: " << bid << ", ask: " << ask << ", row id: " << row << ", max rows: " << nrows );

		if (row >= (nrows-1)) {


			resolved.level2_m.init(resolved.levelSorted_m.size());
			resolved.level2_m.getTime() = CppUtils::getTime();

			// copy and send
			int idx=0;
			for (multimap<double, Level2EntryExtended>::iterator it2 = resolved.levelSorted_m.begin(); it2 != resolved.levelSorted_m.end(); it2++) {
				if (idx >= resolved.levelSorted_m.size()) {
					LOG(MSG_WARN, SMARTCOM, "Symbol: " << symbol_s << " Level 2 entry outside bounds of: " << resolved.levelSorted_m.size() );
					break;
				}

				resolved.level2_m.getLevel2Entry(idx) = it2->second.entry_m;
				if (it2->second.isBest_m) {
					if (it2->second.entry_m.getType() == HlpStruct::Level2Entry::IS_BID)
						resolved.level2_m.setBestBidIndex( idx);

					if (it2->second.entry_m.getType() == HlpStruct::Level2Entry::IS_ASK)
						resolved.level2_m.setBestAskIndex( idx);
				}

				idx++;

			}

			Inqueue::OutProcessor::send_level2_data(resolved.level2_m);	


		}

		return S_OK;
	}
	STDMETHODIMP CMainSmartComEngine::AddTick(
		BSTR symbol, 
		DATE datetime, 
		double price, 
		double volume, 
		BSTR tradeno, 
		StOrder_Action action
		)
	{
		CppUtils::String symbol_c = bstrToString(symbol);
		struct tm tm_struct;

		double utime;
		CppUtils::convertOleTimeToUnix(datetime, utime, tm_struct );

		if (accompanyTicks1minBar_m) {
			// if we overcome 1 minute
			map<CppUtils::String, CppUtils::UnsignedLongLong>::iterator it = symbolSequenceNum_m.find(symbol_c);

			// sequence number
			CppUtils::UnsignedLongLong new_period = (tm_struct.tm_year + 1900) * (CppUtils::UnsignedLongLong)10000000 + tm_struct.tm_yday*10000 + tm_struct.tm_hour * 100 + tm_struct.tm_min;

			// generate the rounded fmt time
			tm_struct.tm_isdst = 0;
			tm_struct.tm_wday = -1;
			tm_struct.tm_yday = -1;
			tm_struct.tm_sec = 0;

			double candletime =  (double)mktime(&tm_struct) - (double)_timezone;



			if ( it != symbolSequenceNum_m.end()) {

				CppUtils::UnsignedLongLong& old_period = it->second;
				if (new_period > old_period ) {

					// get last bar

					double datetime_future = CppUtils::roundToBeginningOfTheDay(CppUtils::getTime()) + 1*24*60*60;
					double datetime_future_ole = CppUtils::convertUnixTimeToOleTime(datetime_future);

					//NOTIFICATION(N_ERROR, "Requesting last bar", 
					//	"Symbol: " << symbol_c <<
					//	", time: " << CppUtils::getGmtTime(datetime_future)
					//);

					HRESULT hr = IStServerPtr_m->GetBars(symbol, StBarInterval_1Min, datetime_future_ole, 2 );
					if (hr != S_OK) {
						NOTIFICATION(N_ERROR, "Cannot return last bar", symbol_c.c_str() );
						LOG(MSG_WARN, SMARTCOM, "Cannot return last bar for: " << symbol_c.c_str() );
					};

					// update
					old_period = new_period;
				}
			} else {
				// empty map
				symbolSequenceNum_m[symbol_c] = new_period;
			}
		}



		Inqueue::OutProcessor::send_rt_data_operation(
			utime, 
			rtProvider_m.c_str(), 
			symbol_c.c_str(), 
			price, 
			price, 
			volume, 
			-1, 
			smartComActionToNativeOpType(action)
			);


		return S_OK;
	}
	STDMETHODIMP CMainSmartComEngine::AddTickHistory(long row, 
		long nrows, 
		BSTR symbol, 
		DATE datetime, 
		double price, 
		double volume, 
		BSTR tradeno, 
		StOrder_Action action
		)
	{


		Inqueue::OutProcessor::send_rt_data_operation(
			CppUtils::convertOleTimeToUnix(datetime), 
			rtProvider_m.c_str(), 
			CW2A(symbol), 
			price, 
			price, 
			volume, 
			-1, 
			smartComActionToNativeOpType(action)
			);


		return S_OK;
	}
	STDMETHODIMP CMainSmartComEngine::AddBar(
		long row, 
		long nrows, 
		BSTR symbol, 
		StBarInterval interval, 
		DATE datetime, 
		double open, 
		double high, 
		double low, 
		double close, 
		double volume, 
		double open_int
		)
	{

		// calculate if we need to pass all rows - if we have pending task
		int pending_tasks_count = getHistoryBarRetrievalTaskNumber(symbol, interval);
		int new_count = -1;

		bool pass_this_bar = false;
		if (pending_tasks_count == 0) {

			// leave only the last row
			if (row >= (nrows-1))
				pass_this_bar = true;
		}
		else {
			// have some task
			pass_this_bar = true;

			// adjust count- last row
			if (row >= (nrows-1)) {
				new_count=freeHistoryBarRetrievalTask((row+1), pending_tasks_count, symbol, interval );
			}

		}								  

		//NOTIFICATION(N_ERROR, "AddBAr(...): ", "symbol: " << this->bstrToString(symbol) << ", row: " << row << ", nrows: " <<nrows
		//	<<",pending_tasks_count: " << pending_tasks_count<<", new_count: " << new_count );

		if (!pass_this_bar)
			return S_OK;

		double utime = prepareGetBarTime(interval, datetime );

		if (utime <= 0) {
			NOTIFICATION(N_ERROR, "Invalid bar time: ",bstrToString(symbol) << " - " << CppUtils::long2String(interval));
			// cannot process bars
			return S_OK;
		}


		//LOG(MSG_INFO, "candle: ->", "Row:" << row << " - " << CppUtils::getGmtTime(utime) <<
		//	", open: " << CppUtils::float2String(open, -1,4 ) << ", high: " << CppUtils::float2String(high, -1,4 )  <<
		//	", low: " << CppUtils::float2String(low, -1,4 )  << ", close: " << CppUtils::float2String(close, -1,4 )  << 
		//	", volume: " << CppUtils::float2String(volume, -1,4 ) <<
		//	", open int: " <<	 open_int
		//	);

		// when we return history								  
		/*
		Inqueue::OutProcessor::send_rt_data_ohcl(
		utime, 
		rtProviderBars_m.c_str(), 
		CW2A(symbol), 
		open,
		high,
		low,
		close,
		volume
		);
		*/

		CppUtils::String symbol_s = bstrToString(symbol);
		pushOHCLToCacheOrPropagate(	symbol_s, utime, open, high,low,close,volume,open_int); 

		if(new_count==0) {
			releaseCachedRetrivalAndSend(symbol_s);
		}

		return S_OK;
	}
	STDMETHODIMP CMainSmartComEngine::SetPortfolio(
		BSTR portfolio, 
		double cash, 
		double leverage, 
		double comission, 
		double saldo
		)
	{

		NOTIFICATION(N_OK, "SetPortfolio(..)", 
			"portfolio: " << bstrToString(portfolio) <<
			", cash: " << cash <<
			", leverage: " << leverage <<
			", comission: " << comission <<
			", saldo: " << saldo
		);
		return S_OK;
	}
	STDMETHODIMP CMainSmartComEngine::AddTrade(
		BSTR portfolio, 
		BSTR symbol, 
		BSTR orderNo, 
		double price, 
		double amount, 
		DATE datetime, 
		BSTR tradeno
		)
	{
		InternalTrade t(portfolio, symbol, orderNo, price,amount,datetime,tradeno );
		CppUtils::String orderNo_s = bstrToString(orderNo);

		registerInternalTrade(orderNo_s, t );
		generateAddTrade(orderNo_s);

		NOTIFICATION(N_OK, "AddTrade", 
			"portfolio: " <<	bstrToString(portfolio) <<
			",symbol: "		<<	bstrToString(symbol) <<
			",orderno: "		<<	orderNo_s <<
			",price: "		<<	price << 
			",amount: "		<<	amount << 
			",datetime: "		<<	CppUtils::getGmtTime(CppUtils::convertOleTimeToUnix(datetime) )	<<
			",tradeno: "		<<	bstrToString(tradeno)
			);																		  

		return S_OK;
	}

	STDMETHODIMP CMainSmartComEngine::UpdateOrder(
		BSTR portfolio, 
		BSTR symbol, 
		StOrder_State state, 
		StOrder_Action action, 
		StOrder_Type type, 
		StOrder_Validity validity, 
		double price, 
		double amount, 
		double stop, 
		double filled, 
		DATE datetime, 
		BSTR orderid, 
		BSTR orderno, 
		long status_mask, 
		long cookie
		)
	{

		// pretty important function :)
		InternalOrder iostruct(symbol, state, action, type, validity, price, amount, stop, filled, datetime, orderid, orderno );


		// store 
		CppUtils::String orderNo_s =  bstrToString(orderno);
		CppUtils::String orderId_s = bstrToString(orderid);

		// first try to register cookie if not
		//registerResponseCookie(orderId_s, cookie);

		// this all notifications regarding UpdateOrder
		registerInternalResponse(orderId_s, iostruct );
		generateOrderSucceded(orderId_s);

		// this is fired only when before addTrade was issued
		registerOrderNoOrderIdMap(orderNo_s,orderId_s); 
		generateAddTrade(orderNo_s);


		// need to track this
		NOTIFICATION(N_OK, "UpdateOrder", 
			"portfolio: " <<	bstrToString(portfolio) <<
			",symbol: "		<<	bstrToString(symbol) <<
			",state: "		<<	orderState2String(state) <<
			",action: "		<<	tradeAction2String(action) << 
			",type: "			<<	tradeType2String(type) << 
			",validity: " <<	orderValidity2String(validity) <<
			",price: "		<<	price << 
			",amount: "		<<	amount << 
			",stop: "			<<	stop << 
			",filled: "		<<	filled << 
			",datetime: " <<	CppUtils::getGmtTime(CppUtils::convertOleTimeToUnix(datetime)) << 
			",orderid: "	<<	orderId_s << 
			",orderno: "	<<	orderNo_s	<<
			",cookie: "	<<	cookie <<
			",status_mask: "	<<	status_mask
			);


		return S_OK;
	}
	STDMETHODIMP CMainSmartComEngine::UpdatePosition(
		BSTR portfolio, 
		BSTR symbol, 
		double avprice, 
		double amount, 
		double planned
		)
	{
		NOTIFICATION(N_OK, "UpdatePosition", 
			"portfolio: " <<	bstrToString(portfolio) <<
			",symbol: "		<<	bstrToString(symbol) <<
			",avprice: "		<<	avprice << 
			",amount: "		<<	amount << 
			",planned: "		<<	planned 
			);

		return S_OK;
	}

	STDMETHODIMP CMainSmartComEngine::AddSymbol(
		long row, 
		long nrows, 
		BSTR symbol, 
		BSTR short_name, 
		BSTR long_name, 
		BSTR type, 
		long decimals, 
		long lot_size, 
		double punkt, 
		double step, 
		BSTR sec_ext_id, 
		BSTR sec_exch_name, 
		DATE expiry_date, 
		double days_before_expiry, 
		double strike

		)
	{

		dlg_m.addSymbolEntry(
			row, 
			nrows, 
			CW2A(symbol), 
			CW2A(short_name, 1251), 
			CW2A(long_name), 
			CW2A(type), 
			decimals, 
			lot_size, 
			punkt, 
			step, 
			CW2A(sec_ext_id), 
			CW2A(sec_exch_name), 
			CppUtils::convertOleTimeToUnix(expiry_date), 
			days_before_expiry
			);
		return S_OK;
	}

	STDMETHODIMP CMainSmartComEngine::OrderSucceeded(long cookie, BSTR orderid)
	{
		// probably no need but might be need to register
		CppUtils::String orderId_s =	bstrToString(orderid);
		registerResponseCookie(orderId_s, cookie);

		generateOrderSucceded(orderId_s);

		NOTIFICATION(N_OK, "Order succeeded", "Cookie: " << cookie << ", orderid: " << orderId_s);
		return S_OK;
	}

	STDMETHODIMP CMainSmartComEngine::OrderFailed(long cookie, BSTR orderid, BSTR reason)
	{
		// failure pass directly form here
		DELEGATE_COOKIE_CLIENTS(cookie, onOrderFailed(cookie, orderid, reason));

		NOTIFICATION(N_ERROR, "Order failed",  
			"Cookie: " << cookie << 
			", orderid: " << bstrToString(orderid) << 
			", reason: " << bstrToString(reason, 1251) );

		return S_OK;
	}
	STDMETHODIMP CMainSmartComEngine::AddPortfolio(long row, 
		long nrows, 
		BSTR portfolioName, 
		BSTR portfolioExch, 
		StPortfolioStatus portfolioStatus
		)
	{
		dlg_m.addPortfolioRow(row, nrows, bstrToString(portfolioName).c_str(), bstrToString(portfolioExch).c_str());

		return S_OK;
	}
	STDMETHODIMP CMainSmartComEngine::SetSubscribtionCheckReult(long result)
	{
		return S_OK;
	}

	STDMETHODIMP CMainSmartComEngine::SetMyTrade(
		long row, 
		long nrows, 
		BSTR portfolio, 
		BSTR symbol, 
		DATE datetime, 
		double price, 
		double volume, 
		BSTR tradeno, 
		StOrder_Action buysell, 
		BSTR orderno
		)
	{
		// resolve 
		CppUtils::String orderNo_s =  bstrToString(orderno);
		CppUtils::StringMap::const_iterator ig = orderIdOrderNoMap_m.find( orderNo_s );

		// not found
		CppUtils::String orderId_s;
		if (ig!=orderIdOrderNoMap_m.end())
			orderId_s = ig->second;
		
			

		
		DELEGATE_ALL_CLIENTS(
			onSetMyTrade(
				portfolio,
				symbol,
				datetime, 
				price, 
				volume, 
				tradeno, 
				buysell, 
				orderno,
				CComBSTR(orderId_s.c_str())
			)
		);
		

	
		/*
		NOTIFICATION(N_OK, "SetMyTrade(...)", 
			"portfolio: " << bstrToString(portfolio) <<
			", symbol: " << bstrToString(symbol) <<
			", datetime: " << CppUtils::getGmtTime(CppUtils::convertOleTimeToUnix(datetime)) << 
			", price: " << price <<
			", volume: " << volume <<
			", tradeno: " << bstrToString(tradeno) <<
			", buysell: " << tradeAction2String(buysell) <<
			", orderno: " <<  bstrToString(orderno)
		);
		*/

		return S_OK;
	}

	STDMETHODIMP CMainSmartComEngine::SetMyOrder(
		long row, 
		long nrows, 
		BSTR portfolio, 
		BSTR symbol, 
		StOrder_State state, 
		StOrder_Action action, 
		StOrder_Type type, 
		StOrder_Validity validity, 
		double price, 
		double amount, 
		double stop, 
		double filled, 
		DATE datetime, 
		BSTR id, 
		BSTR no, 
		long cookie
		)
	{
		return S_OK;
	}

	STDMETHODIMP CMainSmartComEngine::SetMyClosePos(
		long row, 
		long nrows, 
		BSTR portfolio, 
		BSTR symbol, 
		double amount, 
		double price_buy, 
		double price_sell, 
		DATE postime, 
		BSTR order_open, 
		BSTR order_close
		)
	{
		return S_OK;
	}

	STDMETHODIMP CMainSmartComEngine::OrderCancelSucceeded(BSTR orderid)
	{
		NOTIFICATION(N_OK, "OrderCancelSucceeded", "orderid: " << bstrToString(orderid)  );
		return S_OK;
	}


	STDMETHODIMP CMainSmartComEngine::OrderCancelFailed(BSTR orderid)
	{
		NOTIFICATION(N_ERROR, "OrderCancelFailed", "orderid: " << bstrToString(orderid)  );
		return S_OK;
	}


	STDMETHODIMP CMainSmartComEngine::OrderMoveSucceeded(BSTR orderid)
	{

		NOTIFICATION(N_OK, "OrderMoveSucceeded", "orderid: " << bstrToString(orderid)  );
		return S_OK;
	}
	STDMETHODIMP CMainSmartComEngine::OrderMoveFailed(BSTR orderid)
	{
		NOTIFICATION(N_OK, "OrderMoveFailed", "orderid: " << bstrToString(orderid)  );
		return S_OK;
	}



	// ----------------------------------------
	// HELPERS
	// ----------------------------------------

	CComPtr<IStServer> CMainSmartComEngine::getMarhshalledPointer_IStServer()
	{
		if (globalGIT_m == NULL)
			THROW(CppUtils::OperationFailed, "exc_GITisNULL", "ctx_getMarhshalledPointer_IStServer", "");

		CComPtr<IStServer> srvmarshalled;
		HRESULT hr = globalGIT_m->GetInterfaceFromGlobal(IStServerPtrCookie_m, IID_IStServer, (void**)&srvmarshalled);

		if (!SUCCEEDED(hr))
			THROW(CppUtils::OperationFailed, "exc_CannotResolveGITPtr", "ctx_getMarhshalledPointer_IStServer", "");

		if (srvmarshalled== NULL)
			THROW(CppUtils::OperationFailed, "exc_GITPtrIsNull", "ctx_getMarhshalledPointer_IStServer", "");

		return srvmarshalled;
	}



	int CMainSmartComEngine::smartComActionToNativeOpType(int const& action)
	{
		int operation;
		if (action == StOrder_Action_Buy) {
			operation = HlpStruct::RtData::OT_Buy;
		}
		else if (action == StOrder_Action_Sell) {
			operation = HlpStruct::RtData::OT_Sell;
		}
		else if (action == StOrder_Action_Short) {
			operation = HlpStruct::RtData::OT_Short;
		}
		else if (action == StOrder_Action_Cover) {
			operation = HlpStruct::RtData::OT_Cover;
		}
		else
			operation = HlpStruct::RtData::OT_NA;

		return operation;
	}



	CppUtils::String CMainSmartComEngine::tradeAction2String(int const action)
	{
		switch(action) {
			case StOrder_Action_Buy: return "Buy";
			case StOrder_Action_Sell: return "Sell";
			case StOrder_Action_Short: return "Short";
			case StOrder_Action_Cover: return "Cover";
			default: return "N/A";
		}	
	}

	CppUtils::String CMainSmartComEngine::tradeType2String(int const otype)
	{
		switch(otype) {
			case StOrder_Type_Market: return "Market";
			case StOrder_Type_Limit: return "Limit";
			case StOrder_Type_Stop: return "Stop";
			case StOrder_Type_StopLimit: return "StopLimit";
			default: return "N/A";
		}	
	}

	CppUtils::String CMainSmartComEngine::orderValidity2String(int const validity)
	{
		switch(validity) {
			case StOrder_Validity_Day: return "DAY";
			case StOrder_Validity_Gtc: return "GTC";
			default: return "N/A";
		}	
	}

	CppUtils::String CMainSmartComEngine::orderState2String(int const state)
	{
		switch(state) {
			case StOrder_State_ContragentReject: return "ContragentReject";
			case StOrder_State_Submited: return "Submited";
			case StOrder_State_Pending: return "Pending";
			case StOrder_State_Open: return "Open";
			case StOrder_State_Expired: return "Expired";
			case StOrder_State_Cancel: return "Cancel";
			case StOrder_State_Filled: return "Filled";
			case StOrder_State_Partial: return "Partial";
			case StOrder_State_ContragentCancel: return "ContragentCancel";
			case StOrder_State_SystemReject: return "SystemReject";
			case StOrder_State_SystemCancel: return "SystemCancel";
			default: return "N/A";
		}	
	}

	CppUtils::String CMainSmartComEngine::bstrToString(BSTR value, int const encoding)
	{
		char* val_c = NULL;

		if (encoding <=0) 
			val_c = _strdup(CW2A(value));
		else
			val_c = _strdup(CW2A(value,encoding));

		CppUtils::String result = (val_c != NULL ? val_c : "" );
		if (val_c != NULL)
			free(val_c);
		return result;

	}

	CppUtils::String CMainSmartComEngine::createGetBarFunctorKey(BSTR symbol, long interval)
	{
		CppUtils::String result = CW2A(symbol);
		result.append(CppUtils::long2String(interval));

		return result;
	}

	double CMainSmartComEngine::prepareGetBarTime(long interval, DATE datetime)
	{
		// приходит время закрытия а нам нужно время открытия

		double toffset = 0;
		if (interval == StBarInterval_1Min)
			toffset = 60;
		else if (interval == StBarInterval_5Min)
			toffset = 5*60;
		else if (interval == StBarInterval_10Min)
			toffset = 10*60;
		else if (interval == StBarInterval_15Min)
			toffset = 15*60;
		else if (interval == StBarInterval_30Min)
			toffset = 30*60;
		else if (interval == StBarInterval_60Min)
			toffset = 60*60;
		else if (interval == StBarInterval_2Hour)
			toffset = 2*60*60;
		else if (interval == StBarInterval_4Hour)
			toffset = 4*60*60;																   
		else {

			return -1.0;
		}


		double utime = CppUtils::convertOleTimeToUnix(datetime) - toffset;
		return utime;
	}


	void CMainSmartComEngine::silentlyDisconnectBeforeStartEnd()
	{
		try {
			CComPtr<IStServer> srvmarshalled = getMarhshalledPointer_IStServer();

			VARIANT_BOOL is_connected  = VARIANT_FALSE;
			HRESULT hr = srvmarshalled->IsConnected(&is_connected);
			if (FAILED(hr))
				return;

			if (is_connected == VARIANT_TRUE) {


				if (disconnectEvent_m.lock(0))
					disconnectEvent_m.clearEvent();

				HRESULT hr = srvmarshalled->disconnect();

				// need to wait
				if (!FAILED(hr)) {


					// wait 10 secs
					if (!disconnectEvent_m.lock(5000))
						THROW(CppUtils::OperationFailed, "exc_CannotGracefullyDisconnect", "ctx_silentlyDisconnectBeforeEnd", "");

				}

			}

		}
		catch(...)
		{
		}
	}

	void CMainSmartComEngine::notification(CppUtils::EnumerationHelper const& code, char const* str, char const* arg  )
	{

		if (code == N_OK)
			Inqueue::OutProcessor::sendSimpleCommonLoggingEvent(SMARTCOM, code.getName2() + ":" + str, arg);
		else
			Inqueue::OutProcessor::sendSimpleErrorLoggingEvent(SMARTCOM, code.getName2() + ":" + str, arg);

		CppUtils::String str_f(" [ ");
		str_f.append(code.getName());
		str_f.append(" ] ");
		str_f.append(str ? str : "");

		if (arg != NULL) {
			str_f.append(" ( ");
			str_f.append(arg);
			str_f.append(" ) ");
		}

		dlg_m.addLogEntry(str_f);


	}

	void CMainSmartComEngine::notification(CppUtils::EnumerationHelper const& code, CppUtils::String const& str, CppUtils::String const& arg )
	{
		notification(code, str.c_str(), arg.c_str());
	}

	void CMainSmartComEngine::initialize()
	{

		// if signalled - clear
		if (initialized_m.lock(0))
			initialized_m.clearEvent();

		if (!SUCCEEDED(IStServerPtr_m.CreateInstance(CLSID_StServer)))
			THROW(CppUtils::OperationFailed, "exc_CannotCreateInstance_CLSID_StServer", "ctx_init", "");

		if (!SUCCEEDED(DispEventAdvise(IStServerPtr_m, &DIID__IStClient)))
			THROW(CppUtils::OperationFailed, "exc_CannotAdvideToEvents", "ctx_init", "");


		// request versions
		CComQIPtr<ISmartComVersion> verObjPtr = IStServerPtr_m;


		if (verObjPtr.p != NULL) {
			CComBSTR verStringServer, verStringClient;
			verObjPtr->GetServerVersionString(&verStringServer);
			verObjPtr->GetStClientVersionString(&verStringClient);

			dlg_m.setUpInitialData(bstrToString(verStringClient), bstrToString(verStringServer));
		}


		if (!SUCCEEDED(CoCreateInstance(CLSID_StdGlobalInterfaceTable,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_IGlobalInterfaceTable,
			(void **)&globalGIT_m)))
			THROW(CppUtils::OperationFailed, "exc_CannotCreateGIT", "ctx_init", "");





		if (!SUCCEEDED(globalGIT_m->RegisterInterfaceInGlobal(
			IStServerPtr_m,
			IID_IStServer,
			&IStServerPtrCookie_m	))
			)
			THROW(CppUtils::OperationFailed, "exc_CannotRegisterInGIT", "ctx_init", "");



		// create dialog
		dlg_m.Create(NULL);
		dlg_m.ShowWindow(SW_SHOWNORMAL);


		// if not signalled - signal
		if (!initialized_m.lock(0))
			initialized_m.signalEvent();


	} 

	void CMainSmartComEngine::deinitialize()
	{


		// destroy dialog
		if (dlg_m.IsWindow()) {
			dlg_m.DestroyWindow();
		}

		// unadvise
		if(!SUCCEEDED( DispEventUnadvise(IStServerPtr_m, &DIID__IStClient) )) {
			LOG( MSG_ERROR, SMARTCOM, "Cannot unadviseFromEvents");
		}

		if (globalGIT_m == NULL) {
			LOG( MSG_ERROR, SMARTCOM, "GIT is NULL, ignoring...");
		} else {
			if (!SUCCEEDED(globalGIT_m->RevokeInterfaceFromGlobal(IStServerPtrCookie_m))) {
				LOG( MSG_ERROR, SMARTCOM, "Cannot revoke from GIT");
			}
		}

		globalGIT_m = NULL;

		IStServerPtrCookie_m = 0;
		IStServerPtr_m = NULL;

	} 

	void CMainSmartComEngine::onConnected()
	{
		CppUtils::StringSet successfullySubscribed;
		for(int i = 0 ;i < activePortfolios_m.size() ; i++) {

			HRESULT hr = IStServerPtr_m->ListenPortfolio(CComBSTR(activePortfolios_m[i].c_str()));

			if (FAILED(hr)) {
				NOTIFICATION(N_ERROR, "Cannot listen for portfolio", activePortfolios_m[i].c_str() );
			}
			else {
				successfullySubscribed.insert(activePortfolios_m[i]);
				NOTIFICATION(N_OK, "Listen for portfolio", activePortfolios_m[i].c_str() );
			}
		}

		if (successfullySubscribed.size() == activePortfolios_m.size())	{
			subscribedForPortfolio_m = true;
		}
		else {
			NOTIFICATION1(N_ERROR, "Some passed portfolios ws not subscribed, cancelling..." );
			for(int i = 0; i < activePortfolios_m.size(); i++) {
				if (successfullySubscribed.find( activePortfolios_m[ i ] ) != successfullySubscribed.end()) {
					// found, cancelling
					NOTIFICATION(N_OK, "Unsubscribing for portfolio", activePortfolios_m[i].c_str() );
					IStServerPtr_m->CancelPortfolio(CComBSTR(activePortfolios_m[ i ].c_str()));					
				}
			}

		}

		connected_m = true;

	}

	void CMainSmartComEngine::onDisconnected()
	{
		// enforce to unsubscribe all - neglect all errors
		for(int i = 0 ;i < activePortfolios_m.size() ; i++) {
			HRESULT hr = IStServerPtr_m->CancelPortfolio(CComBSTR(activePortfolios_m[i].c_str()));

			if (FAILED(hr)) {
				NOTIFICATION(N_ERROR, "Cannot unsubscribe for portfolio", activePortfolios_m[i].c_str() );
			}
			else {
				
				NOTIFICATION(N_OK, "Unsubscribed for portfolio", activePortfolios_m[i].c_str() );
			}
		
		}

		subscribedForPortfolio_m = false;
		connected_m = false;


	}

	bool CMainSmartComEngine::checkLayerInitialized()
	{
		if (!initialized_m.lock(0)) {
			NOTIFICATION1(N_ERROR, "Layer not initialized" );
			return false;
		}

		return true;
	}

	bool CMainSmartComEngine::checkLayerConnected()
	{
		if (!connected_m) {
			NOTIFICATION1(N_ERROR, "Layer not connected" );
			return false;
		}

		if (!subscribedForPortfolio_m) {
			NOTIFICATION1(N_ERROR, "Layer not subscribed for portfolios" );
			return false;
		}

		return true;
	}

	bool CMainSmartComEngine::checkPassedPortfolio(BSTR portfolio)
	{
		
		
		if (::SysStringLen(portfolio) == 0 )
			return false;

		for(int i = 0; i < activePortfolios_m.size(); i++) {
			if (activePortfolios_m[i] == bstrToString(portfolio))
				return true;
		}

		return false;


	}

	void CMainSmartComEngine::registerInternalResponse(CppUtils::String const& orderId_s, InternalOrder const& orderUpdateStruct)
	{

		map<CppUtils::String, InternalOrderList>::iterator it = arrivedOrders_m.find(orderId_s);
		if (it != arrivedOrders_m.end() ) {
			it->second.push_back(orderUpdateStruct); 
		}
		else {
			InternalOrderList new_list(1);
			new_list[0] = orderUpdateStruct;

			arrivedOrders_m[orderId_s] = new_list;
		}
	}

	void CMainSmartComEngine::registerInternalTrade(CppUtils::String const& orderNo_s, InternalTrade const& tradeStruct)
	{

		map<CppUtils::String, InternalTradesList>::iterator it = arrivedTrades_OrderNos_m.find(orderNo_s);
		if (it != arrivedTrades_OrderNos_m.end() ) {
			it->second.push_back(tradeStruct); 
		}
		else {
			InternalTradesList new_list(1);
			new_list[0] = tradeStruct;

			arrivedTrades_OrderNos_m[orderNo_s] = new_list;
		}
	}

	void CMainSmartComEngine::registerResponseCookie(CppUtils::String const& orderId_s, long const cookie)
	{
		// cookie can't be less or equal zero
		if (cookie <=0)
			return;

		if(orderCookie_m.find(orderId_s) == orderCookie_m.end())
			orderCookie_m[orderId_s] = cookie;
	}

	void CMainSmartComEngine::registerOrderNoOrderIdMap(CppUtils::String const& orderNo_s, CppUtils::String const& orderId_s)
	{
		HTASSERT(orderId_s.size() > 0);

		if(orderNo_s.size() > 0)
			orderIdOrderNoMap_m[orderNo_s] = orderId_s;
	}

	void CMainSmartComEngine::generateAddTrade(CppUtils::String const& orderNo_s)
	{
		if (orderNo_s.size() <=0)
			return;

		// resolve somehow order id	from orderNo
		CppUtils::StringMap::const_iterator ig = orderIdOrderNoMap_m.find( orderNo_s );

		// not found
		if (ig==orderIdOrderNoMap_m.end()) 
			return;


		CppUtils::String const& orderId_s = ig->second;

		map<CppUtils::String, long>::iterator ir = orderCookie_m.find(orderId_s);
		if (ir != orderCookie_m.end()) {
			map<CppUtils::String, InternalTradesList>::iterator ito = arrivedTrades_OrderNos_m.find(orderNo_s);

			if (ito != arrivedTrades_OrderNos_m.end()) {
				InternalTradesList& olist = ito->second;

				for(int i = 0; i < olist.size(); i++) {
					InternalTrade& trade_i = olist[ i ];

					if (!trade_i.isSent_m) {

						DELEGATE_COOKIE_CLIENTS(ir->second, 
							onAddTrade(
							ir->second,
							trade_i.portfolio_m,
							trade_i.symbol_m,
							trade_i.orderNo_m,
							CComBSTR(orderId_s.c_str()),
							trade_i.price_m,
							trade_i.amount_m,
							trade_i.datetime_m,
							trade_i.tradeNo_m
							)
							);
						//
						trade_i.isSent_m = true;
						NOTIFICATION(N_OK, "AddTrade propagated",bstrToString(trade_i.symbol_m) << 
							", orderid: " << orderId_s <<
							", orderno: " << orderNo_s <<
							", cookie: " << ir->second
							);

					}

				}
			}

		}

	}

	void CMainSmartComEngine::generateOrderSucceded(CppUtils::String const& orderId_s)
	{

		map<CppUtils::String, long>::iterator ir = orderCookie_m.find(orderId_s);

		if (ir != orderCookie_m.end()) {
			// we have here cookie resolved, read all sequence of InternalOrder
			map<CppUtils::String, InternalOrderList>::iterator ito = arrivedOrders_m.find(orderId_s);

			if (ito != arrivedOrders_m.end()) {
				// found
				InternalOrderList& olist = ito->second;

				// delegate
				for(int i = 0; i < olist.size(); i++) {
					InternalOrder& order_i = olist[i];
					if (!order_i.isSent_m) {
						DELEGATE_COOKIE_CLIENTS(ir->second, 
							onOrderSucceded(
							ir->second,
							order_i.symbol_m,
							order_i.state_m,
							order_i.action_m,
							order_i.type_m,
							order_i.validity_m,
							order_i.price_m,
							order_i.amount_m,
							order_i.stop_m,
							order_i.filled_m,
							order_i.datetime_m,
							order_i.orderid_m,
							order_i.orderno_m 
							)
							);
						//
						order_i.isSent_m = true;

						NOTIFICATION(N_OK, "UpdateOrder propagated",bstrToString(order_i.symbol_m) << 
							", orderid: " << orderId_s <<
							", cookie: " << ir->second <<
							", action: " << tradeAction2String(order_i.action_m) <<
							", state: " << orderState2String(order_i.state_m) << 
							", type: " << tradeType2String(order_i.type_m)
							);
					}

				}
			}

		}

	}

	void CMainSmartComEngine::registerHistoryBarRetrievalTask(BSTR symbol, long const interval, int const cookie)
	{
		{

			LOCK_FOR_SCOPE(getBarHistoryPendingTaskMtx_m);
			CppUtils::String pkey = createGetBarFunctorKey(symbol, interval);
			map<CppUtils::String, int>::iterator it = getBarHistoryPendingTask_m.find(pkey);

			if (it == getBarHistoryPendingTask_m.end()) 
				getBarHistoryPendingTask_m[pkey] = 1;
			else
				++(it->second);

			NOTIFICATION(N_OK, "Registered history retrieval: ", 
				"symbol: " << bstrToString(symbol) << 
				", interval: " << CppUtils::long2String(interval) <<
				", cookie: " << cookie
			);

			// acquire cached retreival
			acquireCachedRetrival(	bstrToString(symbol), cookie );

		}
	}

	int CMainSmartComEngine::freeHistoryBarRetrievalTask(long const cbars, int const pending_task_count, BSTR symbol, long interval)
	{
		int new_count = pending_task_count -1;
		// last row
		CppUtils::String pkey = createGetBarFunctorKey(symbol, interval);
		LOCK_FOR_SCOPE(getBarHistoryPendingTaskMtx_m);

		if (pending_task_count >= 1)
			getBarHistoryPendingTask_m[pkey] = new_count;
		else {
			getBarHistoryPendingTask_m.erase(pkey);
		}

		NOTIFICATION(N_OK, "Finish history retrieval: ", bstrToString(symbol) << ", interval: " << CppUtils::long2String(interval)<< ", bars: " << cbars);

		return new_count;
	}

	int CMainSmartComEngine::getHistoryBarRetrievalTaskNumber(BSTR symbol, long interval)
	{

		CppUtils::String pkey = createGetBarFunctorKey(symbol, interval);
		{
			LOCK_FOR_SCOPE(getBarHistoryPendingTaskMtx_m);
			map<CppUtils::String, int>::iterator it = getBarHistoryPendingTask_m.find(pkey);

			if (it != getBarHistoryPendingTask_m.end())
				return it->second;
			else
				return 0;

		}


	}

	void CMainSmartComEngine::freeHistoryBarRetrievalTasksForcibly()
	{
		LOCK_FOR_SCOPE(getBarHistoryPendingTaskMtx_m);
		getBarHistoryPendingTask_m.clear();

		NOTIFICATION1(N_OK, "Clear all history retrieval tasks forcibly");

		releaseCachedRetrivalAndSendAllCachedSymbols();
	}

	void CMainSmartComEngine::freeHistoryBarRetrievalTasksForcibly(BSTR symbol, long interval)
	{

		CppUtils::String pkey = createGetBarFunctorKey(symbol, interval);
		LOCK_FOR_SCOPE(getBarHistoryPendingTaskMtx_m);

		getBarHistoryPendingTask_m.erase(pkey);

		NOTIFICATION(N_OK, "Clear history retrieval tasks forcibly: ", bstrToString(symbol) << ", interval: " << CppUtils::long2String(interval));

		releaseCachedRetrivalAndSendAllCachedSymbols();
	}

	void CMainSmartComEngine::onTimer_1_sec()
	{
		// from here we can poll pos


		bool make_call =  false;
		{
			LOCK_FOR_SCOPE(subscribedForGetMyTradesMtx_m);
			if(subscribedForGetMyTradesPollSec_m > 0) {
				if (--subscribedForGetMyTradesCurSec_m<=0) {
					make_call = true;
					subscribedForGetMyTradesCurSec_m = subscribedForGetMyTradesPollSec_m;
				}
			}
			
		}

		if (make_call) {
			//LOCK_FOR_SCOPE(*this);

			if (checkLayerInitialized() && checkLayerConnected()) { 	

				// check portfolio
				for(int i = 0; i < activePortfolios_m.size(); i++) {

					CComPtr<IStServer> srvmarshalled = getMarhshalledPointer_IStServer();
					HRESULT hr = srvmarshalled->GetMyTrades(CComBSTR(activePortfolios_m[i].c_str()));

					if (!SUCCEEDED(hr)) {
						NOTIFICATION1(N_ERROR, "Error getMyTrades(...) call in onTimer_1_sec(...) for portfolio: " << activePortfolios_m[i].c_str());
					}																																													  
				}
				
			}
			else {
				NOTIFICATION1(N_ERROR, "Subscribed for getMyTrades(...) but not connected or initialized, unsubscribing");
				
				{
					LOCK_FOR_SCOPE(subscribedForGetMyTradesMtx_m) 
					subscribedForGetMyTradesPollSec_m = -1;
				}
			}

		}

		// propagate
		DELEGATE_ALL_CLIENTS(onTimer_1Sec());
	}

	//

	void CMainSmartComEngine::generate_Simulation_Bars_OnStart()
	{
		// go through all symbols and return simulatin bar
		{
				LOCK_FOR_SCOPE(sybscribedSymbolsMtx_m);
				simulator_m.init(subscribedSymbolsAsTicks_m, rtProviderBars_m);
	
		}

		double lt= simulator_m.getLastTimeStamp();
		if (lt < 0)
			NOTIFICATION1(N_ERROR, "1Min bars cannot start as no history was read");

		NOTIFICATION1(N_OK, "1Min bars simulation started, begin generating from time: " << CppUtils::getGmtTime(lt));
	}

	void CMainSmartComEngine::generate_Simulation_Bars_OnFinish()
	{
		
		NOTIFICATION1(N_OK, "1Min bars simulation finished");
	}

	void CMainSmartComEngine::generate_Simulation_Bars_1Min()
	{
		simulator_m.generateNewCandle();
			
		// do generate
		//NOTIFICATION1(N_OK, "Generating simulating ticks");
	}


	void CMainSmartComEngine::pushOHCLToCacheOrPropagate(
		CppUtils::String const& symbol, 
		double const& datetime, 
		double const& open, 
		double const& high, 
		double const& low, 
		double const& close, 
		double const& volume,
		double const& open_int
	)
	{
		LOCK_FOR_SCOPE(historyContextMtx_m);

		Candle cndl(datetime, open, high, low, close, volume, open_int);

		map<CppUtils::String, DataPropagationContext >::iterator it = historyContext_m.find( symbol );
		if (it != historyContext_m.end()) {
			if (!it->second.directPropagation_m ) {
				
				it->second.delayedHistory_m[datetime] = cndl;

				//NOTIFICATION1(N_OK, "This rt/history bar is just delegated to cache for symbol: " << symbol);

				return;
			}
		}

		//
		Inqueue::OutProcessor::send_rt_data_ohcl(
			datetime, 
			rtProviderBars_m.c_str(), 
			symbol.c_str(), 
			open,
			high,
			low,
			close,
			volume
			);

		// save to cache
		//lastCandle_m[symbol][datetime] = cndl;

		//LOG(MSG_INFO, SMARTCOM, "Sending real: " << CppUtils::getGmtTime(datetime) << " " << symbol ); 
	}

	void CMainSmartComEngine::releaseCachedRetrivalAndSendAllCachedSymbols()
	{
		LOCK_FOR_SCOPE(historyContextMtx_m);
		for(map<CppUtils::String, DataPropagationContext >::iterator it = historyContext_m.begin(); it != historyContext_m.end(); it++) 
		{
			releaseCachedRetrivalAndSend(it->first);
		}
	}

	void CMainSmartComEngine::releaseCachedRetrivalAndSend(CppUtils::String const& symbol)
	{
		LOCK_FOR_SCOPE(historyContextMtx_m);

		map<CppUtils::String, DataPropagationContext >::iterator it = historyContext_m.find( symbol );
		if (it != historyContext_m.end()) {
			it->second.directPropagation_m = true;

			if(it->second.delayedHistory_m.size() > 0) {
				saveDelayedHistoryToFile(it->second.delayedHistory_m, symbol, it->second.cookie_m);
				it->second.delayedHistory_m.clear();

			} else {
				saveNothing(symbol, it->second.cookie_m);
			}
		}
		//


	}

	void CMainSmartComEngine::acquireCachedRetrival(CppUtils::String const& symbol, int const cookie)
	{
		LOCK_FOR_SCOPE(historyContextMtx_m);

		historyContext_m[symbol].symbol_m = symbol;
		historyContext_m[symbol].delayedHistory_m.clear();
		historyContext_m[symbol].directPropagation_m = false;
		historyContext_m[symbol].cookie_m = cookie;

		
	}

	void CMainSmartComEngine::saveDelayedHistoryToFile(map<double, Candle> const& delayedHistory, CppUtils::String const& symbol, int const cookie)
	{



		double first_time= delayedHistory.begin()->second.datetime_m;
		double last_time = delayedHistory.rbegin()->second.datetime_m;

		CppUtils::Uid uid;
		uid.generate();

		CppUtils::String fileName;
		fileName.append(CppUtils::getTempPath()).append(PATH_SEPARATOR_STR).append(SMARTCOM_TEMP_DIR);
		CppUtils::makeDir(fileName);

		fileName.append(PATH_SEPARATOR_STR);
		fileName.append(uid.toString() + ".tmpcache");

		fstream fs;
		fs.open(fileName.c_str(), ios::out | ios::trunc);

		for(map<double, Candle>::const_iterator i2 = delayedHistory.begin(); i2 != delayedHistory.end(); i2++) {

			//
			fs << CppUtils::getGmtTime2(i2->second.datetime_m) << "," <<
				rtProviderBars_m.c_str() << "," <<
				symbol.c_str() << "," <<
				CppUtils::float2String(i2->second.open_m, -1, 5)	 << "," <<
				CppUtils::float2String(i2->second.high_m, -1, 5)	 << "," <<
				CppUtils::float2String(i2->second.low_m, -1, 5)	 << "," <<
				CppUtils::float2String(i2->second.close_m, -1, 5)	 << "," <<
				CppUtils::float2String(i2->second.volume_m, -1, 5)	 << 
				/* "," << 	CppUtils::float2String(i2->second.open_int_m, -1, 5)	<< */ endl;


			// save for future usage
			simulator_m.addSymbolCandle(symbol, i2->second);
		
		}

		fs.flush();
		fs.close();


		NOTIFICATION1(N_OK, "Save delayed history for symbol: " << symbol << 
			" saving bars count: " <<  delayedHistory.size() <<
			" from datetime: " << CppUtils::getGmtTime2(first_time) <<
			" to datetime: " << CppUtils::getGmtTime2(last_time)
			);

		// now need to send a confirmation message that we are ok with history
		CppUtils::String message = "MSG:HISTORY_SAVED\nUID:"+uid.toString()+"\nSYMBOL:"+symbol;
		Inqueue::OutProcessor::send_rt_provider_synch(	CppUtils::getTime(),rtProvider_m.c_str(), SYNC_EVENT_CUSTOM_MESSAGE_CONST, message.c_str());	

		// propagate
		
		
		CComQIPtr<IMainSmartComEngine> thisPtr;
		HRESULT hr = QueryInterface(IID_IMainSmartComEngine, (void**)&thisPtr);
		if (!SUCCEEDED(hr))
			return;

		CComQIPtr<IDispatch> thisPtr_d = thisPtr;

		DELEGATE_ALL_CLIENTS(
			onFinishBarsHistory( cookie, CComBSTR(symbol.c_str()), VARIANT_TRUE, delayedHistory.size(),  thisPtr_d)
		);
		

	}																	   

	void CMainSmartComEngine::saveNothing(CppUtils::String const& symbol, int const cookie)
	{
		NOTIFICATION1(N_ERROR, "No history will be saved" );

		CppUtils::String message = "MSG:HISTORY_NOT_SAVED\nSYMBOL:"+symbol;
		Inqueue::OutProcessor::send_rt_provider_synch(	CppUtils::getTime(),rtProvider_m.c_str(), SYNC_EVENT_CUSTOM_MESSAGE_CONST, message.c_str());	

		// propagate			    
		
		CComQIPtr<IMainSmartComEngine> thisPtr;
		HRESULT hr = QueryInterface(IID_IMainSmartComEngine, (void**)&thisPtr);
		if (!SUCCEEDED(hr))
			return;

		CComQIPtr<IDispatch> thisPtr_d = thisPtr;

		DELEGATE_ALL_CLIENTS(
			onFinishBarsHistory( cookie, CComBSTR(symbol.c_str()), VARIANT_FALSE, 0,thisPtr_d )
		);
		
	}

};