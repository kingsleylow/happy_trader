#include "laserbroker.hpp"
#include "lasersession.hpp"
#include "windowproc.hpp"
#include "laserexcept.hpp"

#define LASERROKER "LASERBROKER"


//#define PROVIDER "laser_provider"


namespace BrkLib {
#define LTRY  try 

#define LCATCH(FUN) \
			catch(CppUtils::Exception& e)	\
			{	\
				LOG(MSG_ERROR, LASERROKER, "Exception " << #FUN  << " " << e.message());	\
				LOGEVENT(HlpStruct::CommonLog::LL_ERROR, LASERROKER,  "Exception " << #FUN  << " - " << e.message() << " - " << e.arg());	\
			}	\
			catch(...) {	\
				LOG(MSG_ERROR, LASERROKER,  "Unknown exception " << #FUN );	\
				LOGEVENT(HlpStruct::CommonLog::LL_ERROR, LASERROKER,  "Unknown exception " << #FUN );	\
			};


// ----------------------------

	long orderGlobalID_g = 3;

	CppUtils::Mutex orderGlobalIDMtx_g;
	
// ----------------------------
// DllMain
// To register new window class 
extern "C" {    

	BOOL APIENTRY DllMain(HINSTANCE hinstDll, DWORD dwReasion, LPVOID lpReserved) {
		if(dwReasion == DLL_PROCESS_ATTACH) {
	
			WindowThreadProc::setInstance(hinstDll);
			
		}

		return TRUE;
	}

};

// --------------------------
	/**
	 terminator and creator functions
	*/
	BrokerBase *creator (Inqueue::AlgorithmHolder* algholder)
	{
		return new LaserBroker(*algholder);
	}

	void terminator (BrokerBase *pthis)
	{
		assert(pthis);
		delete pthis;
	}
	
	BrokerBase::CreatorBrokerFun createBroker = creator;

	BrokerBase::DestroyerBrokerFun destroyBroker = terminator;

// --------------------------

	
	LaserBroker::LaserBroker( Inqueue::AlgorithmHolder& algHolder):
		BrokerBase(algHolder),
		windowThreadProc_m(0)
	{
		// init this singleton
		windowThreadProc_m = WindowThreadProc::acquireWindowThreadProcInstance();
		WindowThreadProc::registerCallback(*this);

	}

	LaserBroker::~LaserBroker()
	{
		// remove callback
		WindowThreadProc::clearCallback(*this);

		// try to release
		WindowThreadProc::releaseWindowThreadProcInstance();
		  
	}

	void LaserBroker::onLibraryInitialized(Inqueue::AlgBrokerDescriptor const& descriptor)
	{  

		// do not do anything if we are alreday logged and subscribed 
		try {
		
			// read list of stocks in either case
			CppUtils::String const& symbols_level1_file = descriptor.initialAlgBrkParams_m.brokerParams_m.getParameter("LEVEL1_LIST_FILE").getAsString();
			CppUtils::StringList symbolsLevel1;
			if (CppUtils::fileExists(symbols_level1_file)) {
				
				parseFileSymbolList(symbolsLevel1, symbols_level1_file);
			};

			LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERROKER, "Total amount of susbscribed RT symbols is " << symbolsLevel1.size());

			//onlyManagePositions_m = GET_BRK_TRADE_BOOL(descriptor.initialAlgBrkParams_m, "MANAGE_POS_ONLY");

			if (windowThreadProc_m->is_initialized_as_provider()) {
				LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERROKER, "Skipped initialization as assume it is already inited as RT Provider");
				return;
			}

			
			

			// get parameter for subscribing to data

			// if we don't need level 1 - only prints
			bool laser_prints_only = GET_BRK_TRADE_BOOL(descriptor.initialAlgBrkParams_m, "LASER_PRINTS_ONLY");
			bool connect_level1 = GET_BRK_TRADE_BOOL(descriptor.initialAlgBrkParams_m, "CONNECT_LEVEL1");
			bool connect_level2 = GET_BRK_TRADE_BOOL(descriptor.initialAlgBrkParams_m, "CONNECT_LEVEL2");
			bool connect_chart = GET_BRK_TRADE_BOOL(descriptor.initialAlgBrkParams_m, "CONNECT_CHART");
			
			
			CppUtils::String const &user = GET_BRK_TRADE_STRING(descriptor.initialAlgBrkParams_m, "LASER_USER");
			CppUtils::String const &password = GET_BRK_TRADE_STRING(descriptor.initialAlgBrkParams_m, "LASER_PASSWORD");
			int logingtimeout_sec = GET_BRK_TRADE_INT(descriptor.initialAlgBrkParams_m, "LOGIN_TOUT_SEC");
			CppUtils::String providerForRT = descriptor.initialAlgBrkParams_m.brokerParams_m.getParameter("PROVIDER_FOR_RT").getAsString();

			// parse genesis.ini
			CppUtils::String genesisIniFile = GET_BRK_TRADE_STRING(descriptor.initialAlgBrkParams_m, "GENESIS_INI_FILE");
			windowThreadProc_m->getSettings().initialize(
				genesisIniFile, 
				user, 
				password, 
				logingtimeout_sec, 
				laser_prints_only, 
				connect_level1,
				connect_level2,
				connect_chart,
				providerForRT, 
				symbolsLevel1
			);

			
			
			// this is provider for RT feed
			
			LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERROKER, "Got initialization data - RT Provider Name: " << providerForRT <<
				" user: " << "***" << "\n" <<
				" password: " << "***" << "\n" <<
				" Genesis INI file: " << genesisIniFile << "\n" <<
				" quote server will provide prints only: " << (laser_prints_only ? "true": "false" ) << "\n" <<
				" symbol list size: " << symbolsLevel1.size()
			);

			windowThreadProc_m->startupLaserAPI();
			


			// here need to wait until our log-on procedure finishes

			// wait - need to finsih this to avoid unload
			windowThreadProc_m->wait_logged_subscribed(windowThreadProc_m->getSettings().getLoginTimeout());
			if (!windowThreadProc_m->is_startup_ok()) {
				THROW(LaserFailed, "exc_CannotStartupLaserLibrary", "ctx_LibraryInitialized", "");
			}
			
		}
		catch(CppUtils::Exception& e) {
			// shutdow window
			windowThreadProc_m->shutDownLaserAPI();
			throw;
		}
		catch(...)
		{
			// shutdow window
			windowThreadProc_m->shutDownLaserAPI();
			throw;
		}

		
	}  
 
	void LaserBroker::onLibraryFreed()
	{

		windowThreadProc_m->laser_logout();
		
	}

	CppUtils::String const& LaserBroker::getName() const
	{
		static CppUtils::String sname("[ laser broker implementation ]");
		return sname;
	}

	// ------------------------------------

	double LaserBroker::getBrokerSystemTime()
	{
		return windowThreadProc_m->laser_convertToCurrentUnixDateTime(windowThreadProc_m->laser_convertToUnixTime(windowThreadProc_m->laser_get_system_time()));
	}

	// create and destroy session objects
	BrokerSession& LaserBroker::createSessionObject(
		CppUtils::String const& connectstring, 
		CppUtils::String const& runName, 
		CppUtils::String const& comment
	)
	{  
		LOCK_FOR_SCOPE(*this);

		map<CppUtils::String, LaserSession*>::iterator it = sessions_m.find(connectstring);

		if (it==sessions_m.end()) {
			LaserSession* laserSession = new LaserSession(connectstring, *this, *windowThreadProc_m);
			
			sessions_m[ connectstring ] = laserSession;
			sessions2_m[laserSession->getUid()] = laserSession;
			
			// notify
			sendSessionCreateEvent(*laserSession, runName, comment);

			return *laserSession;
		}
		else
			return *it->second;
	

	}

	BrokerSession& LaserBroker::resolveSessionObject(CppUtils::Uid const& uid)
	{
		LOCK_FOR_SCOPE(*this);

		map<CppUtils::Uid, LaserSession*>::iterator it = sessions2_m.find(uid);

		if (it==sessions2_m.end())
			THROW(CppUtils::OperationFailed, "exc_NotFound", "ctx_resolveSessionObject", uid.toString());

		return *it->second;
	
	}

	void LaserBroker::getAvailableSessions(CppUtils::UidList& activeSesionsUids) const
	{
		LOCK_FOR_SCOPE(*this);

		activeSesionsUids.clear();

		for(map<CppUtils::Uid, LaserSession*>::const_iterator it = sessions2_m.begin(); it != sessions2_m.end(); it++) {
			activeSesionsUids.push_back(it->first);
		}
		
	}

	
	void LaserBroker::deleteSessionObject(BrokerSession& session, CppUtils::String const& runName)
	{
		LOCK_FOR_SCOPE(*this);
		LaserSession* laserSession = (LaserSession*) &session;

		if (laserSession) {
			
			// remove from the list if exist
			int cnt = sessions_m.erase( laserSession->getConnection() );

			if (cnt > 0) {
				sessions2_m.erase( laserSession->getUid() );

				// notify
				sendSessionDestroyEvent(session, runName);

				delete laserSession;
			}
		}

		
	} 
	

	// connect 
	void LaserBroker::connectToBroker(BrokerSession& session, CppUtils::String const& runName)
	{
		// do nothing as connection is expected to be alive
	}

		// disconnect
	void LaserBroker::disconnectFromBroker(BrokerSession& session, CppUtils::String const& runName)
	{
			// do nothing as connection is expected to be alive
	}
   

	bool LaserBroker::unblockingIssueOrder(BrkLib::BrokerSession &session, BrkLib::Order const& order)
	{
		try {
			LaserSession& lsession = (LaserSession&)session;
			
			if (order.orderType_m & BrkLib::OP_BUY) {
				GTOrder32 order32;
				double price = 0;
				MMID method, place;
				LPGTSTOCK symbolHndl;
				bool is_stop_market;

				long timeInForce; // from additional info


				prepareOrderCall(		order, 	order32,	symbolHndl,	price,	method,	place, timeInForce, is_stop_market	);

				
				// mutexted !!!
				{
					LOCK_FOR_SCOPE(orderGlobalIDMtx_g);
					orderGlobalID_g++;

					windowThreadProc_m->laser_init_trade_sequence(orderGlobalID_g);

					// initialize some user data
					lsession.registerParentOrder(orderGlobalID_g, order.ID_m);

					// market buy - preparation call
					
					
					//int res = windowThreadProc_m->laser_buy(symbolHndl, order.orVolume_m, price, method);
					//int res = windowThreadProc_m->laser_bid(symbolHndl, order.orVolume_m, price, method);
					//if (res != 0) {
					//	THROW(CppUtils::OperationFailed, "exc_CantBuy", "ctx_UnblockingIssueOrder", "");
					//}

					windowThreadProc_m->laser_fill_buy(symbolHndl, order.orVolume_m, price, method, order32);
					
				}

				
				
				// init
				//order32.bIsldAutoRoute  = FALSE;
				//order32.bArcaAutoRoute  = FALSE;

				order32.dwTimeInForce = timeInForce;
				order32.method = method;
				order32.chPriceIndicator = PRICE_INDICATOR_MARKET; // market
				order32.dwShare = order.orVolume_m;
				order32.chSide = 'B';
				order32.place = place;
				order32.dblPrice = price;

				
							
     		windowThreadProc_m->laser_place_order(symbolHndl, order32);
				


			}
			else if (order.orderType_m & BrkLib::OP_SELL) {
		
				GTOrder32 order32;
				double price = 0;
				MMID method, place;
				LPGTSTOCK symbolHndl;
				bool is_stop_market;
				long timeInForce; // from additional info


				prepareOrderCall(		order, 	order32,	symbolHndl,	price,	method,	place, timeInForce, is_stop_market	);

				// mutexted !!!
				{
					LOCK_FOR_SCOPE(orderGlobalIDMtx_g);
					orderGlobalID_g++;

					windowThreadProc_m->laser_init_trade_sequence(orderGlobalID_g);

					// initialize some user data
					lsession.registerParentOrder(orderGlobalID_g, order.ID_m);

					// market buy - preparation call
					
					//int res = windowThreadProc_m->laser_sell(symbolHndl, order.orVolume_m, price, method);
					//int res = windowThreadProc_m->laser_ask(symbolHndl, order.orVolume_m, price, method);
					//if (res != 0) {
					//	THROW(CppUtils::OperationFailed, "exc_CantBuy", "ctx_UnblockingIssueOrder", "");
					//}

					windowThreadProc_m->laser_fill_sell(symbolHndl, order.orVolume_m, price, method, order32);
					
				}

				
				
				// init
				//order32.bIsldAutoRoute  = FALSE;
				//order32.bArcaAutoRoute  = FALSE;

				order32.dwTimeInForce = timeInForce;
				order32.method = method;
				order32.chPriceIndicator = PRICE_INDICATOR_MARKET; // market
				order32.dwShare = order.orVolume_m;
				order32.chSide = 'S';
				order32.place = place;
				order32.dblPrice = price;

				
							
     		windowThreadProc_m->laser_place_order(symbolHndl, order32);
				
				


			}
			else if (order.orderType_m & BrkLib::OP_BUY_LIMIT) 	{
				GTOrder32 order32;
				double price = 0;
				MMID method, place;
				LPGTSTOCK symbolHndl;
				bool is_stop_market;
				long timeInForce; // from additional info


				prepareOrderCall(		order, 	order32,	symbolHndl,	price,	method,	place, timeInForce, is_stop_market	);

				// mutexted !!!
				{
					LOCK_FOR_SCOPE(orderGlobalIDMtx_g);
					orderGlobalID_g++;

					windowThreadProc_m->laser_init_trade_sequence(orderGlobalID_g);

					// initialize some user data
					lsession.registerParentOrder(orderGlobalID_g, order.ID_m);

					// market buy - preparation call
					
					
					//int res = windowThreadProc_m->laser_buy(symbolHndl, order.orVolume_m, price, method);
					//int res = windowThreadProc_m->laser_bid(symbolHndl, order.orVolume_m, price, method);
					//if (res != 0) {
					//	THROW(CppUtils::OperationFailed, "exc_CantBuy", "ctx_UnblockingIssueOrder", "");
					//}

					windowThreadProc_m->laser_fill_ask(symbolHndl, order.orVolume_m, price, method, order32);
					 
				}

				
				
				// init
				//order32.bIsldAutoRoute  = FALSE;
				//order32.bArcaAutoRoute  = FALSE;

				order32.dwTimeInForce = timeInForce;
				order32.method = method;
				order32.chPriceIndicator = PRICE_INDICATOR_LIMIT; // market
				order32.dwShare = order.orVolume_m;
				order32.chSide = 'B';
				order32.place = place;
				order32.dblPrice = price;

				if (order32.dblPrice <=0) {
					LOGEVENT(HlpStruct::CommonLog::LL_WARN, LASERROKER, "Are you sure that for OP_BUY_LIMIT order price is invalid?");
				}

				
							
     		windowThreadProc_m->laser_place_order(symbolHndl, order32);
				

			}
			else if (order.orderType_m & BrkLib::OP_SELL_LIMIT) {
				GTOrder32 order32;
				double price = 0;
				MMID method, place;
				LPGTSTOCK symbolHndl;
				bool is_stop_market;
				long timeInForce; // from additional info


				prepareOrderCall(		order, 	order32,	symbolHndl,	price,	method,	place, timeInForce, is_stop_market	);

				// mutexted !!!
				{
					LOCK_FOR_SCOPE(orderGlobalIDMtx_g);
					orderGlobalID_g++;

					windowThreadProc_m->laser_init_trade_sequence(orderGlobalID_g);

					// initialize some user data
					lsession.registerParentOrder(orderGlobalID_g, order.ID_m);

					// market buy - preparation call
					
					
					//int res = windowThreadProc_m->laser_buy(symbolHndl, order.orVolume_m, price, method);
					//int res = windowThreadProc_m->laser_ask(symbolHndl, order.orVolume_m, price, method);
					//if (res != 0) {
					//	THROW(CppUtils::OperationFailed, "exc_CantBuy", "ctx_UnblockingIssueOrder", "");
					//}
					windowThreadProc_m->laser_fill_bid(symbolHndl, order.orVolume_m, price, method, order32);
					
				}

				
				
				// init
				//order32.bIsldAutoRoute  = FALSE;
				//order32.bArcaAutoRoute  = FALSE;

				order32.dwTimeInForce = timeInForce;
				order32.method = method;
				order32.chPriceIndicator = PRICE_INDICATOR_LIMIT; // market
				order32.dwShare = order.orVolume_m;
				order32.chSide = 'S';
				order32.place = place;
				order32.dblPrice = price;

				if (order32.dblPrice <=0) {
					LOGEVENT(HlpStruct::CommonLog::LL_WARN, LASERROKER, "Are you sure that for OP_SELL_LIMIT order price is invalid?");
				}

				
							
     		windowThreadProc_m->laser_place_order(symbolHndl, order32);
				

			}
			// STOP
			else if (order.orderType_m & BrkLib::OP_BUY_STOP) 	{
				GTOrder32 order32;
				double price = 0;
				MMID method, place;
				LPGTSTOCK symbolHndl;
				bool is_stop_market;
				long timeInForce; // from additional info


				prepareOrderCall(		order, 	order32,	symbolHndl,	price,	method,	place, timeInForce, is_stop_market	);

				// mutexted !!!
				{
					LOCK_FOR_SCOPE(orderGlobalIDMtx_g);
					orderGlobalID_g++;

					windowThreadProc_m->laser_init_trade_sequence(orderGlobalID_g);

					// initialize some user data
					lsession.registerParentOrder(orderGlobalID_g, order.ID_m);

					// market buy - preparation call
					
					
					//int res = windowThreadProc_m->laser_buy(symbolHndl, order.orVolume_m, price, method);
					//int res = windowThreadProc_m->laser_bid(symbolHndl, order.orVolume_m, price, method);
					//if (res != 0) {
					//	THROW(CppUtils::OperationFailed, "exc_CantBuy", "ctx_UnblockingIssueOrder", "");
					//}

					windowThreadProc_m->laser_fill_ask(symbolHndl, order.orVolume_m, price, method, order32);
					 
				}

				
				
				// init
				//order32.bIsldAutoRoute  = FALSE;
				//order32.bArcaAutoRoute  = FALSE;

				order32.dwTimeInForce = timeInForce;
				order32.method = method;
				order32.dwShare = order.orVolume_m;
				order32.chSide = 'B';
				order32.place = place;
				order32.dblPrice = price;

				// stop limit or market stop
				if (is_stop_market) {
					order32.chPriceIndicator = PRICE_INDICATOR_STOP;
				}
				else {

					order32.chPriceIndicator = PRICE_INDICATOR_STOPLIMIT;
					// so far the same
					order32.dblStopLimitPrice = order32.dblPrice;
				}

				if (order32.dblPrice <=0) {
					LOGEVENT(HlpStruct::CommonLog::LL_WARN, LASERROKER, "Are you sure that for OP_BUY_STOP order price is invalid?");
				}

				
							
     		windowThreadProc_m->laser_place_order(symbolHndl, order32);
				

			}
			else if (order.orderType_m & BrkLib::OP_SELL_STOP) {
				GTOrder32 order32;
				double price = 0;
				MMID method, place;
				LPGTSTOCK symbolHndl;
				bool is_stop_market;
				long timeInForce; // from additional info


				prepareOrderCall(		order, 	order32,	symbolHndl,	price,	method,	place, timeInForce, is_stop_market	);

				// mutexted !!!
				{
					LOCK_FOR_SCOPE(orderGlobalIDMtx_g);
					orderGlobalID_g++;

					windowThreadProc_m->laser_init_trade_sequence(orderGlobalID_g);

					// initialize some user data
					lsession.registerParentOrder(orderGlobalID_g, order.ID_m);

					// market buy - preparation call
					
					
					//int res = windowThreadProc_m->laser_buy(symbolHndl, order.orVolume_m, price, method);
					//int res = windowThreadProc_m->laser_ask(symbolHndl, order.orVolume_m, price, method);
					//if (res != 0) {
					//	THROW(CppUtils::OperationFailed, "exc_CantBuy", "ctx_UnblockingIssueOrder", "");
					//}
					windowThreadProc_m->laser_fill_bid(symbolHndl, order.orVolume_m, price, method, order32);
					
				}

				
				
				// init
				//order32.bIsldAutoRoute  = FALSE;
				//order32.bArcaAutoRoute  = FALSE;

				order32.dwTimeInForce = timeInForce;
				order32.method = method;
				order32.dwShare = order.orVolume_m;
				order32.chSide = 'S';
				order32.place = place;
				order32.dblPrice = price;

				// stop limit or market stop
				if (is_stop_market) {
					order32.chPriceIndicator = PRICE_INDICATOR_STOP;
				}
				else {

					order32.chPriceIndicator = PRICE_INDICATOR_STOPLIMIT;
					// so far the same
					order32.dblStopLimitPrice = order32.dblPrice;
				}

				if (order32.dblPrice <=0) {
					LOGEVENT(HlpStruct::CommonLog::LL_WARN, LASERROKER, "Are you sure that for OP_SELL_STOP order price is invalid?");
				}

				
							
     		windowThreadProc_m->laser_place_order(symbolHndl, order32);
				

			}
			//
			else if (order.orderType_m & BrkLib::OP_CLOSE) {
				// close market order - e.i. need to sell the same amount so we will not use 
				THROW(CppUtils::OperationFailed, "exc_CantCallCloseOnPos", "ctx_unblockingIssueOrder", "");
				

			}
			else if (order.orderType_m & BrkLib::OP_CANCEL) {
				// if ticket is not set - cancel all
				if (order.brokerPositionID_m.size() > 0) {
					long orderTicket = atol(order.brokerPositionID_m.c_str());
					windowThreadProc_m->laser_cancel_order_ticket( orderTicket );
				}
				else {

					// if we have symbol entered - cancle only all for that symbol otherwise cancell all
					if (order.symbolNum_m.size() >=0) {
						// cancel for symbol
						LPGTSTOCK symbolHndl = windowThreadProc_m->laser_get_stock_handler(order.symbolNum_m);

						windowThreadProc_m->laser_cancel_order(symbolHndl);

					}
					else {
						// cancel all
						windowThreadProc_m->laser_cancel_order_all();
					}
				}
				
			}
			else {
				THROW(CppUtils::OperationFailed, "exc_OrderNotSupported", "ctx_unblockingIssueOrder", CppUtils::long2String(order.orderType_m));
			}
		}
		catch(BrkLib::LaserFailed &e)
		{
			// intercept here to provide more adequate error report
			BrokerResponseOrder brokerResponse;
	
			brokerResponse.ID_m.generate();
			brokerResponse.symbolNum_m = order.symbolNum_m;
			brokerResponse.brokerComment_m = brokerResponse.symbolNum_m + " - [ " + e.context() + " - " + e.message() + e.arg() + " ] ";
				
			brokerResponse.resultCode_m = PT_GENERAL_ERROR_HAPPENED;
			brokerResponse.resultDetailCode_m = ODR_GENERAL_ERROR;
			brokerResponse.parentID_m = order.ID_m;
			brokerResponse.opOpenTime_m = getBrokerSystemTime();


			// propagate based on dwTraderSeqNo
			propagateResponseAmongSessions(brokerResponse);

			LOGEVENT(HlpStruct::CommonLog::LL_ERROR, LASERROKER, "unblockingIssueOrder(...) symbol: " << order.symbolNum_m << 
				" ID: " << order.ID_m.toString() << " text: " << brokerResponse.brokerComment_m );

		}


		return true;
	}

	void LaserBroker::getBrokerResponseList(BrkLib::BrokerSession &session) const
	{
		THROW(CppUtils::OperationFailed, "exc_NotSupported", "ctx_GetBrokerResponseList", "");
	}
		
	void LaserBroker::getBrokerResponseList(BrkLib::BrokerSession &session, BrkLib::OperationResult const or) const
	{
		THROW(CppUtils::OperationFailed, "exc_NotSupported", "ctx_GetBrokerResponseList", "");
	}
		
	BrkLib::PositionList const& LaserBroker::getBrokerPositionList(int const posState, BrkLib::BrokerSession &session)
	{
		
		LaserSession& lsession = (LaserSession&)session;
		lsession.positionList_m.clear();

		
		if (posState & QUERY_PENDING) {
		
			
			GTPending32List pendingList;
			
			// pending
			CppUtils::StringList symbolsLevel1;
			windowThreadProc_m->get_subscribed_symbol_list(symbolsLevel1);

			windowThreadProc_m->laser_request_pending_positions(symbolsLevel1, pendingList);


			
			//
			for(int i = 0; i < pendingList.size(); i++) {
				Position standardPos;
				GTPending32& pending32 = pendingList[i];
				
				// only ticket ID is a valid order ID
				standardPos.brokerPositionID_m = CppUtils::long2String(pending32.dwTicketNo);
				standardPos.symbolNum_m = pending32.szStock;
				standardPos.place_m = windowThreadProc_m->laser_MMID2String(pending32.place);
				standardPos.method_m = windowThreadProc_m->laser_MMID2String(pending32.method);
				standardPos.volume_m = pending32.nPendShares;
				standardPos.installPrice_m = pending32.dblPendPrice;
				standardPos.installTime_m = windowThreadProc_m->laser_convertLaserDateTime(pending32.nPendDate, pending32.nPendTime);
				

				if(pending32.chPendSide =='B')
					standardPos.lastPosType_m = POS_LIMIT_BUY;
				else if (pending32.chPendSide=='S')
					standardPos.lastPosType_m = POS_LIMIT_SELL;
				else if (pending32.chPendSide=='T')
					standardPos.lastPosType_m = POS_LIMIT_SELL; // short sell
				
				standardPos.posState_m = STATE_PENDING;
		
				standardPos.comment_m = "[ Laser: pending ]";
				if (pending32.chPendSide=='T')
					standardPos.comment_m += " [ SHORT SELL ]";

				lsession.positionList_m.push_back(standardPos);
			}
			

			// these are commulative positions
			/*

			GTOpenPosition32List openList;
			windowThreadProc_m->laser_request_open_positions(symbolsLevel1_m, openList);

			for(int i = 0; i < openList.size(); i++) {
				Position standardPos;
				GTOpenPosition32& open32 = openList[i];
				
				// only ticket ID is a valid order ID
				standardPos.symbolNum_m = open32.szStock;
				standardPos.volume_m = open32.nOpenShares;
				standardPos.installPrice_m = open32.dblOpenPrice;
				standardPos.installTime_m = windowThreadProc_m->laser_convertLaserDateTime(open32.nOpenDate, open32.nOpenTime);
				

				if(open32.chOpenSide =='B')
					standardPos.lastPosType_m = POS_BUY;
				else if (open32.chOpenSide=='S')
					standardPos.lastPosType_m = POS_SELL;
				else if (open32.chOpenSide=='T')
					standardPos.lastPosType_m = POS_SHORT_SELL;
				
				standardPos.posState_m = STATE_PENDING;
		
				standardPos.comment_m = "Laser: open";
				lsession.positionList_m.push_back(standardPos);
			}

			*/
			
		}
		
		
		if (posState & QUERY_OPEN) {
		
			
			GTTrade32List tradeList;

			// these are separate orders !!!

			CppUtils::StringList symbolsLevel1;
			windowThreadProc_m->get_subscribed_symbol_list(symbolsLevel1);

			windowThreadProc_m->laser_request_trade_positions(symbolsLevel1, tradeList);


			for(int i = 0; i < tradeList.size(); i++) {
				Position standardPos;
				GTTrade32& trade32 = tradeList[i];
				
				// only ticket ID is a valid order ID
				standardPos.brokerPositionID_m = CppUtils::long2String(trade32.dwTicketNo);
				standardPos.symbolNum_m = trade32.szStock;
				standardPos.place_m = windowThreadProc_m->laser_MMID2String(trade32.place);
				standardPos.method_m = windowThreadProc_m->laser_MMID2String(trade32.method);
				standardPos.volume_m = trade32.nExecShares;
				standardPos.executePrice_m = trade32.dblExecPrice;
				standardPos.executeTime_m = windowThreadProc_m->laser_convertLaserDateTime(trade32.nExecDate, trade32.nExecTime);

				if(trade32.chExecSide =='B')
					standardPos.lastPosType_m = POS_BUY;
				else if (trade32.chExecSide=='S')
					standardPos.lastPosType_m = POS_SELL;
				else if (trade32.chExecSide=='T')
					standardPos.lastPosType_m = POS_SELL;
				
				
				standardPos.posState_m = STATE_OPEN; 
				standardPos.comment_m = CppUtils::String("[ Laser: trade ]") + "[ execfirm: " + windowThreadProc_m->laser_MMID2String(trade32.execfirm) +" ]";
				
				if (trade32.chExecSide=='T')
					standardPos.comment_m += " [ SHORT SELL ]";

				
				lsession.positionList_m.push_back(standardPos);
			}
			
		}
		
		if (posState & QUERY_HISTORY) {

		}
		
		
		
		return lsession.positionList_m;

	}

	void LaserBroker::updatePositionTagFields(BrkLib::BrokerSession &session, CppUtils::Uid const posUid, int const tag1, int const tag2, int const posState)
	{
	}
		
	void LaserBroker::onDataArrived(	HlpStruct::RtData const& rtdata)
	{
		// this function is to be implemented only for backtessting (ask ZVV for details :))
		// thus here:
		assert( false );
	}
		
	void LaserBroker::initializeBacktest(
			Inqueue::SimulationProfileData const& simulProfileData, 
			BrokerBase::ProviderSymbolCookieMap const& providerSymbolCookieMap 
		)
	{
		// this function is to be implemented only for backtessting as providerSymbolCookieMap used for fast acess to simulProfileData\
		// thus here:
		// Not implemented
		assert(false);
	}
		
	void LaserBroker::initializeBacktest(Inqueue::SimulationProfileData const& simulProfileData )
	{
		// this function is to be implemented only for backtessting in RT mode (ask ZVV for details :))
		// thus here:
		// Not implemented
		assert(false);
	}



	bool LaserBroker::isBacktestLibrary() const
	{
			// it's RT library, direct connect to quik
			return false;
	}

	bool LaserBroker::getTickInfo(
			BrkLib::BrokerSession &session, 
			CppUtils::String const& symbolNum, 
			CppUtils::String const& symbolDenom
		)
		{
			// No-op
			return false;
		}

		
		// subscribe for nes retreival
		bool LaserBroker::subscribeForNews(
			BrkLib::BrokerSession &session
		)
		{
			// No-op
			return false;
		}

		// returns last error information
		bool LaserBroker::getLastErrorInfo(
			BrkLib::BrokerSession &session
		)
		{
			// No-op
			return false;
		}

		// subscribe for quotes - they will arrive via onBrokerResponseArrived
		bool LaserBroker::subscribeForQuotes(
			BrkLib::BrokerSession &session,
			CppUtils::String const& symbolNum, 
			CppUtils::String const& symbolDenom
		)
		{
			// No-op
			return false;
		}

		// returns the history from the server
		bool LaserBroker::getHistoryInfo(
			BrkLib::BrokerSession &session,
			CppUtils::String const& symbolNum, 
			CppUtils::String const& symbolDenom,
			Inqueue::AggregationPeriods aggrPeriod,
			int const multfactor
		)
		{
			// No-op
			return false;
		}

		// ---------------------------------------

		
		
// -----------------------------


// callbacks
/*
int LaserBroker::OnExecMsgLogin(LPGTSESSION hSession, BOOL bLogin)
{
	LTRY{

		LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERROKER, "OnExecMsgLogin() Result: " << bLogin   );

	}
	LCATCH(OnExecMsgLogin)
	return 0; 
}

int LaserBroker::OnExecConnected(LPGTSESSION hSession)
{ 
	LTRY{

		LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERROKER, "Connected"  );

	}
	LCATCH(OnExecConnected)
	return 0; 
};

int  LaserBroker::OnExecDisconnected(LPGTSESSION hSession){

	LTRY {

		LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERROKER, "Disconnected"  );
	}
	LCATCH(OnExecDisconnected)
	return 0; 

};
*/

/*
int  LaserBroker::OnExecMsgLoggedin(LPGTSESSION hSession)
{ 
	LTRY {

		LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERROKER, "Logged in and subscribed for symbols (!)"  );

		createStockForSymbols();
		
		// here we logged and subscribed
		loggedAndSubscribed_m.signalEvent();
	}
	LCATCH(OnExecMsgLoggedin)

	return 0; 
};   

int  LaserBroker::OnExecMsgLogout(LPGTSESSION hSession){
	LTRY {

		LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERROKER, "Logged out and desubscribed for symbols"  );

		removeStocksForSymbols();
	}
	LCATCH(OnExecMsgLogout)
	return 0; 
};
*/


int  LaserBroker::OnExecMsgTrade(LPGTSESSION hSession, const GTTrade32 *trade)
{ 
	LTRY {

		BrokerResponseOrder brokerResponse;
	
		bool go_on = false;

		// despite we have market or limit order this event is issued when order is executed!!!
		// for installation see pending event
		if (trade->chPriceIndicator==PRICE_INDICATOR_MARKET) {
			brokerResponse.resultCode_m = PT_ORDER_ESTABLISHED;
			brokerResponse.resultDetailCode_m = ODR_OK;
			brokerResponse.resultDetailCode_m |= ODR_ORDER_EXECUTED;
			
			go_on = true;
			
		}
		else if (trade->chPriceIndicator==PRICE_INDICATOR_LIMIT) {
			brokerResponse.resultCode_m = PT_ORDER_ESTABLISHED;
			brokerResponse.resultDetailCode_m = ODR_OK;
			brokerResponse.resultDetailCode_m |= ODR_ORDER_EXECUTED;

			go_on = true;

		}
		else if (trade->chPriceIndicator==PRICE_INDICATOR_STOP) {
			brokerResponse.resultCode_m = PT_ORDER_ESTABLISHED;
			brokerResponse.resultDetailCode_m = ODR_OK;
			brokerResponse.resultDetailCode_m |= ODR_ORDER_EXECUTED;

			go_on = true;

		}
		else if (trade->chPriceIndicator==PRICE_INDICATOR_STOPLIMIT) {
			brokerResponse.resultCode_m = PT_ORDER_ESTABLISHED;
			brokerResponse.resultDetailCode_m = ODR_OK;
			brokerResponse.resultDetailCode_m |= ODR_ORDER_EXECUTED;

			go_on = true;

		}
		else {
			THROW(CppUtils::OperationFailed, "exc_NotSupported", "ctx_Laser_OnExecMsgTrade", "");
		}
		
		if (go_on) {
			brokerResponse.ID_m.generate();
			brokerResponse.symbolNum_m = trade->szStock;

			// exec 
			brokerResponse.opVolume_m = trade->nExecShares;
			brokerResponse.opOpenPrice_m = trade->dblExecPrice;

			brokerResponse.brokerPositionID_m = CppUtils::long2String(trade->dwTicketNo);
					
			brokerResponse.opOpenTime_m = windowThreadProc_m->laser_convertLaserDateTime(trade->nExecDate, trade->nExecTime);

			// here are orders executed thus only buy or have we have here
			// because we see only executed positions !!!
			//brokerResponse.positionType_m = priceIndicatorToStandardPosType(trade->chPriceIndicator, trade->chExecSide);
			brokerResponse.positionType_m = (trade->chExecSide == 'B' ? POS_BUY : POS_SELL);

			// this is amount of remaining shares
			// 0 means order is actually opened fully
			brokerResponse.RESPONSE_TAG_REMAIN_SHARES = trade->nExecRemainShares;
			// propa
			propagateResponseAmongSessions(brokerResponse, false, trade->dwTraderSeqNo);
		}

		
		// hope this is the event we need to catch to confirm order was executed (either market or limit)
		LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERROKER, "OnExecMsgTrade()  sequence ID: " << trade->dwTraderSeqNo <<
				" ticket ID: " << trade->dwTicketNo <<
				" stock: " << trade->szStock <<
				" method: " << windowThreadProc_m->laser_MMID2String(trade->method) <<
				" place: " << windowThreadProc_m->laser_MMID2String(trade->place) <<
				" price indic: " << windowThreadProc_m->laser_orderPriceIndicator2String(trade->chPriceIndicator) <<
				" pending side: " << trade->chPendSide <<
				" exec side: " << trade->chExecSide <<
				" pending price: " << trade->dblPendPrice <<
				" entry price: " << trade->dblEntryPrice <<
				" exec price: " << trade->dblExecPrice <<
				" entry shares: " << trade->nEntryShares <<
				" exec shares: " << trade->nExecShares <<
				" exec remain shares: " << trade->nExecRemainShares <<
				" exec time str: " << CppUtils::getGmtTime(windowThreadProc_m->laser_convertLaserDateTime(trade->nExecDate, trade->nExecTime)
			) 

		);

	}
	LCATCH(OnExecMsgTrade)
	return 0;  
};

int  LaserBroker::OnExecMsgAccount(LPGTSESSION hSession, const GTAccount32 *account)
{

	LTRY {
		

		BrokerResponseEquity requity;
		requity.curPositionReportProfit_m = account->dblCurrentEquity;
		
		ostrstream sstr; 
		sstr << "Account ID:" << account->szAccountID <<
			" account name: " << account->szAccountName <<
			" current long: " << account->dblCurrentLong <<
			" current short: " << account->dblCurrentShort <<
			" p/l realized: " << account->dblPLRealized <<
			" current tickets: " << account->nCurrentTickets <<
			" current shares: " << account->nCurrentShares <<
			" current cancel: " << account->nCurrentCancel <<
			" current partial fills: " << account->nCurrentPartialFills <<
			" current equity value: " << account->dblCurrentEquity <<
			" current equity amount: " << account->dblCurrentAmount << ends;
		
		requity.brokerComment_m = sstr.str();
		sstr.freeze( false ); 
		
			 
		propagateResponseAmongSessions(requity);

		
		LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERROKER, 
			"OnExecMsgAccount() Account ID:" << account->szAccountID <<
			" account name: " << account->szAccountName <<
			" current long: " << account->dblCurrentLong <<
			" current short: " << account->dblCurrentShort <<
			" p/l realized: " << account->dblPLRealized <<
			" current tickets: " << account->nCurrentTickets <<
			" current shares: " << account->nCurrentShares <<
			" current cancel: " << account->nCurrentCancel <<
			" current partial fills: " << account->nCurrentPartialFills <<
			" current equity value: " << account->dblCurrentEquity <<
			" current equity amount: " << account->dblCurrentAmount
		);
		
	}  
	LCATCH(OnExecMsgAccount)
	return 0;
};

// --------------
// presumingly these are collbacks after order
int  LaserBroker::OnExecMsgOpenPosition(LPGTSESSION hSession, const GTOpenPosition32 *open)
{ 
	LTRY {

		LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERROKER, "OnExecMsgOpenPosition() " << 
			" stock: " << open->szStock <<
			" open price: " << open->dblOpenPrice <<
			" open shares: " << open->nOpenShares <<
			" open full time str: " << CppUtils::getGmtTime(windowThreadProc_m->laser_convertLaserDateTime(open->nOpenDate, open->nOpenTime )) <<
			" open side: " << open->chOpenSide <<
			" currency rate: " << open->dblCurrencyRate
		);

	}
	LCATCH(OnExecMsgOpenPosition)
	return 0; 
};

int  LaserBroker::OnExecMsgPending(LPGTSESSION hSession, const GTPending32 *pending)
{ 
	LTRY {


		BrokerResponseOrder brokerResponse;
	
		bool go_on = false;

		// despite we have market or limit order this event is issued when order is installed!!!
		// for installation see pending event
		if (pending->chPriceIndicator==PRICE_INDICATOR_MARKET) {
			brokerResponse.resultCode_m = PT_ORDER_ESTABLISHED;
			brokerResponse.resultDetailCode_m = ODR_OK;
			brokerResponse.resultDetailCode_m |= ODR_ORDER_INSTALLED;
			
			brokerResponse.positionType_m = (pending->chPendSide == 'B' ? POS_LIMIT_BUY : POS_LIMIT_SELL);

			go_on = true;
			
		}
		else if (pending->chPriceIndicator==PRICE_INDICATOR_LIMIT) {
			brokerResponse.resultCode_m = PT_ORDER_ESTABLISHED;
			brokerResponse.resultDetailCode_m = ODR_OK;
			brokerResponse.resultDetailCode_m |= ODR_ORDER_INSTALLED;

			brokerResponse.positionType_m = (pending->chPendSide == 'B' ? POS_LIMIT_BUY : POS_LIMIT_SELL);

			go_on = true;

		}
		else if (pending->chPriceIndicator==PRICE_INDICATOR_STOP) {
			brokerResponse.resultCode_m = PT_ORDER_ESTABLISHED;
			brokerResponse.resultDetailCode_m = ODR_OK;
			brokerResponse.resultDetailCode_m |= ODR_ORDER_INSTALLED;

			brokerResponse.positionType_m = (pending->chPendSide == 'B' ? POS_STOP_BUY : POS_STOP_SELL);

			go_on = true;

		}
		else if (pending->chPriceIndicator==PRICE_INDICATOR_STOPLIMIT) {
			brokerResponse.resultCode_m = PT_ORDER_ESTABLISHED;
			brokerResponse.resultDetailCode_m = ODR_OK;
			brokerResponse.resultDetailCode_m |= ODR_ORDER_INSTALLED;

			brokerResponse.positionType_m = (pending->chPendSide == 'B' ? POS_STOP_BUY : POS_STOP_SELL);

			go_on = true;

		}
		else {
			THROW(CppUtils::OperationFailed, "exc_NotSupported", "ctx_Laser_OnExecMsgTrade", "");
		}
		
		if (go_on) {
			brokerResponse.ID_m.generate();
			brokerResponse.symbolNum_m = pending->szStock;

			// exec 
			brokerResponse.opVolume_m = pending->nPendShares;
			brokerResponse.opOpenPrice_m = pending->dblPendPrice;
			
			brokerResponse.brokerPositionID_m = CppUtils::long2String(pending->dwTicketNo);
					
			brokerResponse.opOpenTime_m = windowThreadProc_m->laser_convertLaserDateTime(pending->nPendDate, pending->nPendTime);
			
			
			
			// propa
			propagateResponseAmongSessions(brokerResponse, false, pending->dwTraderSeqNo);
		}

		LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERROKER, "OnExecMsgPending() sequence ID: " << pending->dwTraderSeqNo <<
				" ticket ID: " << pending->dwTicketNo <<
				" stock: " << pending->szStock <<
				" method: " << windowThreadProc_m->laser_MMID2String(pending->method) <<
				" place: " << windowThreadProc_m->laser_MMID2String(pending->place) <<
				" side: " << pending->chPendSide <<
				" shares: " << pending->nPendShares <<
				" pending price: " << pending->dblPendPrice <<
				" entry price: " << pending->dblEntryPrice <<
				" price indicator: " << windowThreadProc_m->laser_orderPriceIndicator2String(pending->chPriceIndicator) <<
				" pending full time str: " << CppUtils::getGmtTime(windowThreadProc_m->laser_convertLaserDateTime(pending->nPendDate, pending->nPendTime )) <<
				" entry TIF: " <<  pending->nEntryTIF <<
				" pending TIF: " << pending->nPendTIF
		);
	}
	LCATCH(OnExecMsgPending)
	return 0; 
};

int  LaserBroker::OnExecMsgSending( LPGTSESSION hSession, const GTSending32 *sending)
{ 
	LTRY {

		// notify
	LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERROKER, "OnExecMsgSending() sequence ID: " << sending->dwTraderSeqNo << 
		" ticket ID: " << sending->dwTicketNo <<
		" stock: " << sending->szStock <<
		" method: " << windowThreadProc_m->laser_MMID2String(sending->method) <<
		" place: " << windowThreadProc_m->laser_MMID2String(sending->place) <<
		" entry side: " << sending->chEntrySide <<
		" entry shares: " << sending->nEntryShares <<
		" entry price: " << sending->dblEntryPrice <<
		" dwUserData: " << sending->dwUserData <<
		" lpUserData: " << long(sending->lpUserData) <<
		" entry TIF: " << sending->nEntryTIF
		);

			

	}
	LCATCH(OnExecMsgSending)
	return 0; 
};

int  LaserBroker::OnExecMsgCanceling(LPGTSESSION hSession, const GTCancel32 * cancel)
		 { 
			 LTRY {
					LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERROKER, "OnExecMsgCanceling() sequence ID: " << cancel->dwTraderSeqNo << 
						" ticket ID: " << cancel->dwTicketNo <<
						" stock: " << cancel->szStock <<
						" method: " << windowThreadProc_m->laser_MMID2String(cancel->method) <<
						" place: " << windowThreadProc_m->laser_MMID2String(cancel->place) << 
						" price indicator: " << windowThreadProc_m->laser_orderPriceIndicator2String(cancel->chPriceIndicator) <<
						" pending side: " << cancel->chPendSide <<
						" cancel shares: " << cancel->nCancelShares <<
						" cancel price: " << cancel->dblCancelPrice <<
						" pending price: " << cancel->dblPendPrice <<
						" cancel remain shares: " << cancel->nCancelRemainShares <<
						" reason: '" << cancel->szCancelReason << "'" << 
						" canceller: " << cancel->nCanceler
					);
			 }
			 LCATCH(OnExecMsgCanceling)
			 return 0; 
};

int  LaserBroker::OnExecMsgCancel(LPGTSESSION hSession, const GTCancel32 * cancel)
{ 
LTRY {
		BrokerResponseOrder brokerResponse;
	
		brokerResponse.ID_m.generate();
		brokerResponse.symbolNum_m = cancel->szStock;
		brokerResponse.opVolume_m = cancel->nCancelShares;
		brokerResponse.opClosePrice_m = cancel->dblCancelPrice;

		brokerResponse.brokerPositionID_m = CppUtils::long2String(cancel->dwTicketNo);
		
	
		brokerResponse.resultCode_m = PT_ORDER_CANCELLED;
		brokerResponse.resultDetailCode_m = ODR_OK;

		brokerResponse.opCloseTime_m = windowThreadProc_m->laser_convertLaserDateTime(cancel->nCancelDate, cancel->nCancelTime);
		
		// cnacelled are only limit
		//brokerResponse.positionType_m = priceIndicatorToStandardPosType(cancel->chPriceIndicator, cancel->chPendSide);
		brokerResponse.positionType_m = (cancel->chPendSide == 'B' ? POS_LIMIT_BUY : POS_LIMIT_SELL);
		
		// propa
		propagateResponseAmongSessions(brokerResponse, false, cancel->dwTraderSeqNo);


	LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERROKER, "OnExecMsgCancel() sequence ID: " << cancel->dwTraderSeqNo << 
		" ticket ID: " << cancel->dwTicketNo <<
		" stock: " << cancel->szStock <<
		" method: " << windowThreadProc_m->laser_MMID2String(cancel->method) <<
		" place: " << windowThreadProc_m->laser_MMID2String(cancel->place) << 
		" price indicator: " << windowThreadProc_m->laser_orderPriceIndicator2String(cancel->chPriceIndicator) <<
		" pending side: " << cancel->chPendSide <<
		" cancel shares: " << cancel->nCancelShares <<
		" cancel price: " << cancel->dblCancelPrice <<
		" pending price: " << cancel->dblPendPrice <<
		" cancel reamin shares: " << cancel->nCancelRemainShares <<
		" reason: '" << cancel->szCancelReason << "'" << 
		" canceller: " << cancel->nCanceler <<
		" datetime: " << CppUtils::getAscTime(brokerResponse.opCloseTime_m)
	);
}
LCATCH(OnExecMsgCancel)
return 0; 
};

int  LaserBroker::OnExecMsgReject(LPGTSESSION hSession, const GTReject32 *reject)
{ 
	LTRY {
		BrokerResponseOrder brokerResponse;
	
		brokerResponse.ID_m.generate();
		brokerResponse.symbolNum_m = reject->szStock;
		brokerResponse.opVolume_m = reject->nRejectShares;
		brokerResponse.opOpenPrice_m = reject->dblPendPrice;

		brokerResponse.brokerPositionID_m = CppUtils::long2String(reject->dwTicketNo);
		
		

		brokerResponse.resultCode_m = PT_ORDER_CANCELLED;
		brokerResponse.resultDetailCode_m = ODR_GENERAL_ERROR;

		brokerResponse.opCloseTime_m = windowThreadProc_m->laser_convertLaserDateTime(reject->nRejectDate, reject->nRejectTime);
		
		//brokerResponse.positionType_m = priceIndicatorToStandardPosType(reject->chPriceIndicator, reject->chPendSide);
		brokerResponse.positionType_m = (reject->chPendSide == 'B' ? POS_LIMIT_BUY : POS_LIMIT_SELL);

		// propa
		propagateResponseAmongSessions(brokerResponse, false, reject->dwTraderSeqNo);

		LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERROKER, "OnExecMsgReject() sequence ID:"  << reject->dwTraderSeqNo << 
						" ticket ID: " << reject->dwTicketNo <<
						" stock: " << reject->szStock <<
						" method: " << windowThreadProc_m->laser_MMID2String(reject->method) <<
						" place: " << windowThreadProc_m->laser_MMID2String(reject->place) <<
						" datetime: " << CppUtils::getAscTime(brokerResponse.opCloseTime_m)
		);
	}
	LCATCH(OnExecMsgReject)
	return 0; 
};

int  LaserBroker::OnExecMsgRemove(LPGTSESSION hSession, const GTRemove32 *remove)
{ 
	LTRY {

		BrokerResponseOrder brokerResponse;
	
		brokerResponse.ID_m.generate();
		brokerResponse.symbolNum_m = remove->szStock;
		brokerResponse.opVolume_m = remove->nPendShares;
		brokerResponse.opOpenPrice_m = remove->dblPendPrice;

		brokerResponse.brokerPositionID_m = CppUtils::long2String(remove->dwTicketNo);
		
		
		brokerResponse.resultCode_m = PT_ORDER_CANCELLED;
		brokerResponse.resultDetailCode_m = ODR_GENERAL_ERROR;

		brokerResponse.opCloseTime_m = windowThreadProc_m->laser_convertLaserDateTime(remove->nRemoveDate, remove->nRemoveTime);

		// remove only pending
		//brokerResponse.positionType_m = priceIndicatorToStandardPosType(remove->chPriceIndicator, remove->chPendSide);
		brokerResponse.positionType_m = (remove->chPendSide == 'B' ? POS_LIMIT_BUY : POS_LIMIT_SELL);


		// propa
		propagateResponseAmongSessions(brokerResponse, false, remove->dwTraderSeqNo);

		LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERROKER, "OnExecMsgRemove() sequence ID: " << remove->dwTraderSeqNo << 
						" ticket ID: " << remove->dwTicketNo <<
						" stock: " << remove->szStock <<
						" method: " << windowThreadProc_m->laser_MMID2String(remove->method) <<
						" place: " << windowThreadProc_m->laser_MMID2String(remove->place) <<
						" datetime: " << CppUtils::getAscTime(brokerResponse.opCloseTime_m)
		);
	}
	LCATCH(OnExecMsgRemove)
	return 0; 
};

int  LaserBroker::OnExecMsgRejectCancel(LPGTSESSION hSession, const GTRejectCancel32 *rejectcancel)
{
	LTRY {

		BrokerResponseOrder brokerResponse;
	
		brokerResponse.ID_m.generate();
		brokerResponse.symbolNum_m = rejectcancel->szStock;
		brokerResponse.opVolume_m = rejectcancel->nPendShares;
		brokerResponse.opOpenPrice_m = rejectcancel->dblPendPrice;

		brokerResponse.brokerPositionID_m = CppUtils::long2String(rejectcancel->dwTicketNo);
		
		
		brokerResponse.resultCode_m = PT_ORDER_CANCELLED;
		brokerResponse.resultDetailCode_m = ODR_GENERAL_ERROR;

		brokerResponse.opCloseTime_m = windowThreadProc_m->laser_convertLaserDateTime(rejectcancel->nRejectDate, rejectcancel->nRejectTime);
		//brokerResponse.positionType_m = priceIndicatorToStandardPosType(rejectcancel->chPriceIndicator, rejectcancel->chPendSide);

		brokerResponse.positionType_m = (rejectcancel->chPendSide == 'B' ? POS_BUY : POS_SELL);


		
		// propa
		propagateResponseAmongSessions(brokerResponse, false, rejectcancel->dwTraderSeqNo);


		LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERROKER, "OnExecMsgRejectCancel() sequence ID: " << rejectcancel->dwTraderSeqNo << 
						" ticket ID: " << rejectcancel->dwTicketNo <<
						" stock: " << rejectcancel->szStock <<
						" method: " << windowThreadProc_m->laser_MMID2String(rejectcancel->method) <<
						" place: " << windowThreadProc_m->laser_MMID2String(rejectcancel->place) <<
						" datetime: " << CppUtils::getAscTime(brokerResponse.opCloseTime_m)
		);
	}
	LCATCH(OnExecMsgRejectCancel)
	return 0;
};

// stock
int LaserBroker::OnExecMsgIOIStatus_Stock(LPGTSTOCK hStock, const GTIOIStatus32 *ioStatus)
{
	LTRY {
		LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERROKER, "OnExecMsgIOIStatus_Stock() stock: " <<  ioStatus->szStock <<
			" ticket ID: " << ioStatus->dwTicketNo <<
			" shares: " << ioStatus->dwShares
		);
	}
	LCATCH(OnExecMsgIOIStatus_Stock)
	return 0;

}

int LaserBroker::OnExecCmd_Stock(LPGTSTOCK hStock, const LPCSTR *execCmd)
{
	LTRY {
		LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERROKER, "OnExecCmd_Stock() "  );
	}
	LCATCH(OnExecCmd_Stock)
	return 0;
}

int LaserBroker::OnExecMsgSending_Stock( LPGTSTOCK hStock, const GTSending32 *sending)
{
	LTRY {
		LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERROKER, "OnExecMsgSending_Stock() ");
	}
	LCATCH(OnExecMsgSending_Stock)
	return 0; 
}
 

int LaserBroker::PrePlaceOrder_Stock( LPGTSTOCK hStock,const GTOrder32 *order)
{
	LTRY {
		
		// here we will sign up our tracking shit!!!!
		

		LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERROKER, "PrePlaceOrder_Stock() "  );
	}

	LCATCH(PrePlaceOrder_Stock)
	return 0;
}


int LaserBroker::PostPlaceOrder_Stock( LPGTSTOCK hStock,const GTOrder32 *order)
{
	LTRY {
		// here we will clean our tracking stuff shit!!!!

		

		LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERROKER, "PostPlaceOrder_Stock() "  );
	}
	LCATCH(PostPlaceOrder_Stock)
	return 0;
}


// -----------

int  LaserBroker::OnExecMsgStatus(LPGTSESSION hSession, const GTStatus32 *status)
{
	LTRY {
		LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERROKER, "OnExecMsgStatus() status: " << status->szStatus );

	}
	LCATCH(OnExecMsgState)
	return 0; 
};

int LaserBroker::OnExecMsgState(LPGTSESSION hSession, const GTServerState32 *state)
{
	LTRY {

		LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERROKER, "OnExecMsgState() server: " << state->szServer << " connect status: " << state->nConnect  );
	}
	LCATCH(OnExecMsgState)
	return 0;
}

/*
int LaserBroker::OnGotQuoteLevel1(LPGTSESSION hSession,  const GTLevel132 *level1)
{
	if (onlyManagePositions_m)
		return 0;

	LTRY {
		// level one
		
		//LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERROKER, "Level1 time: " << level1->gtime.dwTime << " str " <<  CppUtils::getGmtTime(level1data.getTime()) );

		SEND_LEVEL1_DATA(
			windowThreadProc_m->laser_convertToCurrentUnixDateTime(windowThreadProc_m->laser_convertToUnixTime(&level1->gtime)),
			providerForRT_m,
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
*/

/*
int LaserBroker::OnGotQuotePrint(LPGTSESSION hSession,  const GTPrint32 *printR)
{
	if (onlyManagePositions_m)
		return 0;

	LTRY {

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
			windowThreadProc_m->laser_convertToCurrentUnixDateTime(windowThreadProc_m->laser_convertToUnixTime(&printR->gtime)),
			providerForRT_m,	printR->szStock,	printR->dblPrice,	printR->dblPrice,	printR->dwShares,	color );
		

	}
	LCATCH(OnGotQuotePrint)
	return 0;
}
*/

int LaserBroker::OnExecMsgErrMsg(LPGTSESSION hSession, const GTErrMsg32 *errorMsg)
{
LTRY {

		// notify here about error
		// not too much info here
		BrokerResponseOrder brokerResponse;
	
		brokerResponse.ID_m.generate();
		brokerResponse.symbolNum_m = errorMsg->szStock;
		brokerResponse.brokerComment_m = brokerResponse.symbolNum_m + " - [" + errorMsg->szText + "]";
			
		brokerResponse.resultCode_m = PT_GENERAL_ERROR_HAPPENED;
		brokerResponse.resultDetailCode_m = ODR_GENERAL_ERROR;

		brokerResponse.opOpenTime_m = getBrokerSystemTime();


		// propagate based on dwTraderSeqNo
		propagateResponseAmongSessions(brokerResponse, false, errorMsg->dwOrderSeqNo);

		LOGEVENT(HlpStruct::CommonLog::LL_ERROR, LASERROKER, "OnExecMsgErrMsg() symbol: " << errorMsg->szStock << 
			" text: " << errorMsg->szText << " code: " << errorMsg->nErrCode << " sequence ID: " << errorMsg->dwOrderSeqNo <<
			" error msg: '" << windowThreadProc_m->laser_getErrorMesage(errorMsg->nErrCode) << "'"
		);
}
LCATCH(OnExecMsgErrMsg)
return 0; 
}


int LaserBroker::OnGotQuoteConnected(LPGTSESSION hSession)
{
	  
	// check for print counts
	/*
	CppUtils::StringList symbols;
	windowThreadProc_m->get_subscribed_symbol_list(symbols);

	LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERROKER, "Attempting to read history..."  );

	
	for(int i = 0; i < symbols.size(); i++) {
		LPGTSTOCK pStock = windowThreadProc_m->laser_get_stock_handler(symbols[i]);
		if (pStock == NULL)
			LOGEVENT(HlpStruct::CommonLog::LL_ERROR, LASERROKER, "For symbol: " << symbols[i] << " cannot resolve stock handler"  );

		//int count = windowThreadProc_m->laser_get_print_count(pStock);
		//LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERROKER, "For symbol: " << symbols[i] << " prints count: " << count  );
		
		GTStock32* stock32p = windowThreadProc_m->laser_get_stock_structure(pStock);
		//LOGEVENT(HlpStruct::CommonLog::LL_INFO, LASERROKER, "Acquired stock structure: " << stock32p->m_szStock );
	
		int cnt = gtGetPrintCount(pStock);
		GTLevel232* ptr =  (GTLevel232*)gtGetBestBidLevel2Item(pStock);

		int foo  = 0;


	}
	*/
	

	return 0; 
}

// chart
/*
int LaserBroker::OnGotChartDisconnected(LPGTSESSION hSession)
{
	LTRY {
	}
	LCATCH(OnGotChartDisconnected)

	return 0; 
}

int LaserBroker::OnGotChartConnected(LPGTSESSION hSession)
{
	LTRY {
	}
	LCATCH(OnGotChartConnected)

	return 0; 
}

int LaserBroker::OnGotChartRecord(LPGTSESSION hSession, const GTChart32 *gtchart)
{
	LTRY {
	}
	LCATCH(OnGotChartRecord)

	return 0; 
}

int LaserBroker::OnGotChartRecordHistory(LPGTSESSION hSession, const GTChart32 *gtchart)
{
	LTRY {
	}
	LCATCH(OnGotChartRecordHistory)

	return 0;
}

int LaserBroker::OnGotChartRecordRefresh(LPGTSESSION hSession, const GTChart32 *gtchart)
{
	LTRY {
	}
	LCATCH(OnGotChartRecordRefresh)
	return 0;
}

int LaserBroker::OnGotChartRecordDisplay(LPGTSESSION hSession, const GTChart32 *gtchart)
{
	// better if from here we will send necessary historical data (according to the list provided)

	LTRY {
	}
	LCATCH(OnGotChartRecordDisplay)
	return 0;
}
*/

// ----------------------------------------------

void LaserBroker::parseFileSymbolList(CppUtils::StringList& symbols, CppUtils::String const& fileName)
{
	CppUtils::String content;
	if (!CppUtils::readContentFromFile2(fileName, content))
		THROW(CppUtils::OperationFailed, "exc_CannotReadFile", "ctx_parseSymbolFile", fileName);

	CppUtils::tokenize(content, symbols, "\n");

}



void LaserBroker::propagateResponseAmongSessions(BrokerResponseBase& brokerResponse, bool const remove, long const sequenceID)
{
	LOCK_FOR_SCOPE(*this);
		
	for (map<CppUtils::String, LaserSession*>::iterator it = sessions_m.begin(); it!=sessions_m.end();it++) {

		// check if this is valid ans skip if order does not contain our order ID 
		CppUtils::Uid const& parentUid = it->second->resolveParentUid(sequenceID);
		if (parentUid.isValid()) {

			brokerResponse.parentID_m = parentUid;
			BrokerBase::onBrokerResponseArrived(*it->second, brokerResponse);

			if (remove) {
				it->second->removeParentOrder(sequenceID);
			}

		}
	}
}

void LaserBroker::propagateResponseAmongSessions(BrokerResponseBase& brokerResponse)
{
	LOCK_FOR_SCOPE(*this);
		
	for (map<CppUtils::String, LaserSession*>::iterator it = sessions_m.begin(); it!=sessions_m.end();it++) {
		BrokerBase::onBrokerResponseArrived(*it->second, brokerResponse);
	}
}


/*
PosType LaserBroker::priceIndicatorToStandardPosType(int priceIndic, int chSide) const
{
	switch(priceIndic) {
		case PRICE_INDICATOR_MARKET: 
			return chSide == 'B'? POS_BUY : POS_SELL;
			break;
		case PRICE_INDICATOR_LIMIT: 
			return chSide == 'B'?POS_LIMIT_BUY : POS_LIMIT_SELL;
			break;
		case PRICE_INDICATOR_STOP: 
			return chSide == 'B' ? POS_STOP_BUY : POS_STOP_SELL;
			break;
		case PRICE_INDICATOR_STOPLIMIT: 
	
		case PRICE_INDICATOR_LIMIT_OR_BETTER:
	
		case PRICE_INDICATOR_PEGGED: 
		
		case PRICE_INDICATOR_MARKET_ON_OPEN: 
		
		case PRICE_INDICATOR_MARKET_ON_CLOSE: 
			
		case PRICE_INDICATOR_LIMIT_ON_OPEN: 

		case PRICE_INDICATOR_LIMIT_ON_CLOSE: 

		case PRICE_INDICATOR_IO_OPEN: 

		case PRICE_INDICATOR_IO_CLOSE: 

		default:
			return POS_DUMMY;
	}
}

*/

void LaserBroker::prepareOrderCall(
			BrkLib::Order const& order, 
			GTOrder32& order32,
			LPGTSTOCK& symbolHndl,
			double& price,
			MMID& method,
			MMID& place,
			long& timeInForce,
			bool& is_stop_market
		)
{
		// time in force
		CppUtils::StringList result;
		CppUtils::tokenize(order.additionalInfo_m, result, "=;");
		
		is_stop_market = false;

		if (result.size() > 0) {

			CppUtils::String tinforce_s;
			CppUtils::String stop_market_s;

			for(int i = 0; i < result.size(); i++) {
				if ( _stricmp(result[i].c_str(), OPTION_TIME_IN_FORCE)==0) {
					int t = i+1;
					
					if (t < result.size()) {
						tinforce_s = result[t];
						break;
					}

				}
				else if ( _stricmp(result[i].c_str(), OPTION_STOP_MKT)==0) {
					int t = i+1;
					
					if (t < result.size()) {
						stop_market_s = result[t];
						break;
					}

				}

			}

			if (_stricmp(stop_market_s.c_str(),"true"))
				is_stop_market = true;
			else
				is_stop_market = false;

			if (_stricmp(tinforce_s.c_str(),"TIF_IOC")==0)
				timeInForce = TIF_IOC;
			else if (_stricmp(tinforce_s.c_str(),"TIF_DAY")==0)
				timeInForce = TIF_DAY;
			else if (_stricmp(tinforce_s.c_str(),"TIF_MGTC")==0)
				timeInForce = TIF_MGTC;
			else if (_stricmp(tinforce_s.c_str(),"TIF_SGTC")==0)
				timeInForce = TIF_SGTC;
			else if (_stricmp(tinforce_s.c_str(),"TIF_99994")==0)
				timeInForce = TIF_99994;
			else if (_stricmp(tinforce_s.c_str(),"TIF_MDAY")==0)
				timeInForce = TIF_MDAY;
			else if (_stricmp(tinforce_s.c_str(),"TIF_SDAY")==0)
				timeInForce = TIF_SDAY;
			else
				// in seconds
				timeInForce = atoi(tinforce_s.c_str());
		}
		else
			timeInForce = TIF_DAY;

		// 
		// init order
		symbolHndl = windowThreadProc_m->laser_get_stock_handler(order.symbolNum_m);
		windowThreadProc_m->laser_init_order(symbolHndl, order32);

		
		method = METHOD_UNKNOWN;
		place = MMID_DEFAULT;
		


		// set up price
		
		if (order.orPriceType_m==BrkLib::PC_ZERO )
			price = 0;
		else if (order.orPriceType_m==BrkLib::PC_BEST_ASK)
			price = windowThreadProc_m->laser_getBestAskPrice(symbolHndl);
		else if (order.orPriceType_m==BrkLib::PC_BEST_BID)
			price = windowThreadProc_m->laser_getBestBidPrice(symbolHndl);
		else if (order.orPriceType_m==BrkLib::PC_INPUT)
			price = order.orPrice_m;
		else
			price = 0;
		
		method = (order.method_m.size() > 0 ? windowThreadProc_m->laser_createMMIDFromString(order.method_m) : METHOD_UNKNOWN);
		place = (order.place_m.size() > 0 ? windowThreadProc_m->laser_createMMIDFromString(order.place_m) : MMID_DEFAULT);

		
		

}

}; // end of namesapce 