#include "btbroker.hpp"
#include "btsession.hpp"

namespace BrkLib {

	/**
	 terminator and creator functions
	*/
	BrokerBase *creator (Inqueue::AlgorithmHolder* algholder)
	{
		return new BtBroker(*algholder);
	}

	void terminator (BrokerBase *pthis)
	{
		assert(pthis);
		delete pthis;
	}
	
	BrokerBase::CreatorBrokerFun createBroker = creator;

	BrokerBase::DestroyerBrokerFun destroyBroker = terminator;

	// -----------------------------------------

	BtBroker::BtBroker( Inqueue::AlgorithmHolder& algHolder):
		BrokerBase(algHolder)
	{
	}

	BtBroker::~BtBroker()
	{
	}

	void BtBroker::onLibraryInitialized(Inqueue::AlgBrokerDescriptor const& descriptor)
	{
	
	}

	void BtBroker::onLibraryFreed()
	{
	}
		
	CppUtils::String const& BtBroker::getName() const
	{
		static CppUtils::String sname("Backtest Broker Library");
		return sname;
	}

	// -------------------------------------

	double BtBroker::getBrokerSystemTime()
	{
		return CppUtils::getTime();
	}

	// create and destroy session objects
	BrokerSession& BtBroker::createSessionObject(CppUtils::String const& connectstring, CppUtils::String const& runName, CppUtils::String const& comment)
	{
		LOCK_FOR_SCOPE(sessionsMtx_m);

		map<CppUtils::String, BtSession*>::iterator it = sessions_m.find(connectstring);

		if (it==sessions_m.end()) {
			BtSession* btSession = new BtSession(connectstring, *this);
			sessions_m[ connectstring ] = btSession;
			sessions2_m[btSession->getUid()] = btSession;

			// notify
			sendSessionCreateEvent(*btSession, runName, comment);
			return *btSession;
		}
		else
			return *it->second;
	}

	BrokerSession& BtBroker::resolveSessionObject(CppUtils::Uid const& uid)
	{
		LOCK_FOR_SCOPE(sessionsMtx_m);

		map<CppUtils::Uid, BtSession*>::iterator it = sessions2_m.find(uid);

		if (it==sessions2_m.end())
			THROW(CppUtils::OperationFailed, "exc_NotFound", "ctx_resolveSessionObject", uid.toString());

		return *it->second;
	}

	void BtBroker::getAvailableSessions(CppUtils::UidList& activeSesionsUids) const
	{
		LOCK_FOR_SCOPE(sessionsMtx_m);
		activeSesionsUids.clear();

		for(map<CppUtils::Uid, BtSession*>::const_iterator it = sessions2_m.begin(); it != sessions2_m.end(); it++) {
			activeSesionsUids.push_back(it->first);
		}

	}

	void BtBroker::deleteSessionObject(BrokerSession& session, CppUtils::String const& runName)
	{
		LOCK_FOR_SCOPE(sessionsMtx_m);

		BtSession* btSession = (BtSession*) &session;

		if (btSession) {
			// remove from the list if exist

		
			int cnt = sessions_m.erase(btSession->getConnection());
			if (cnt > 0) {

				sessions2_m.erase(btSession->getUid());
			
				// notify
				sendSessionDestroyEvent(*btSession, runName);

				delete btSession;
			}
		}
	}

		
	void BtBroker::connectToBroker(BrokerSession& session, CppUtils::String const& runName)
	{
		BtSession& btSession = (BtSession&)session;
		btSession.connect();

		// do not send connect event - to save time and resources
		//sendSessionConnectEvent(btSession, runName);
	}

		
	void BtBroker::disconnectFromBroker(BrokerSession& session, CppUtils::String const& runName)
	{
		BtSession& btSession = (BtSession&)session;
		btSession.disconnect();

		// do not send disconnect event - to save time and resources
		//sendSessionDisconnectEvent(btSession, runName);
	}


	bool BtBroker::unblockingIssueOrder(BrkLib::BrokerSession &session, BrkLib::Order const& order)
	{
			if (session.isConnected()) {
				LOCK_FOR_SCOPE(pendingOrdesMtx_m);

				BtSession& btSession = (BtSession&)session;

				// set up last order issue time
				//order.orderIssueTime_m = ;


				// resolve cookie
				// but do not force if not found
				order.cookie_symb_prov_m = resolveProviderSymbolCookieNoExcept(order.provider_m, order.symbolNum_m+order.symbolDenom_m );

				//PendingCommand cmd(order, btSession);
				//pendingOrders_m.push(cmd);

				doProcessPendingOrderHelper(session, order);

				return true;
			}

			// failure
			return false;
	}

	void BtBroker::getBrokerResponseList(BrkLib::BrokerSession &session) const
	{
			// no-op
		THROW(CppUtils::OperationFailed, "exc_NotSupported", "ctx_GetBrokerResponseList", "");
	}
		
	void BtBroker::getBrokerResponseList(BrkLib::BrokerSession &session, BrkLib::OperationResult const or) const
	{
			// no-op
		THROW(CppUtils::OperationFailed, "exc_NotSupported", "ctx_GetBrokerResponseList", "");
	}
		
	BrkLib::PositionList const& BtBroker::getBrokerPositionList(int const posState, BrkLib::BrokerSession &session)
	{
		if (session.isConnected()) {
			BtSession& btSession = (BtSession&)session;

			LOCK_FOR_SCOPE(positionsMtx_m);
			btSession.positionList_m.clear();

			if (posState & QUERY_PENDING) {
				// copy this to session object
				for(map<CppUtils::Uid, Position>::const_iterator it = pendingPositions_m.begin(); it!= pendingPositions_m.end(); it++)
				{
					btSession.positionList_m.push_back(it->second);
				}
			}
			
			if (posState & QUERY_OPEN) {
				for(map<CppUtils::Uid, Position>::const_iterator it = openedPositions_m.begin(); it!= openedPositions_m.end(); it++)
				{
					btSession.positionList_m.push_back(it->second);
				}
			}
			
			if (posState & QUERY_HISTORY) {
				for(map<CppUtils::Uid, Position>::const_iterator it = historyPositions_m.begin(); it!= historyPositions_m.end(); it++)
				{
					btSession.positionList_m.push_back(it->second);
				}
			}
			

			return btSession.positionList_m;

		}

		THROW(CppUtils::OperationFailed, "exc_NotConnected", "ctx_GetBrokerResponseList", "");

	}

	void BtBroker::updatePositionTagFields(BrkLib::BrokerSession &session, CppUtils::Uid const posUid, int const tag1, int const tag2, int const posState)
	{
		if (session.isConnected()) {
			BtSession& btSession = (BtSession&)session;
			LOCK_FOR_SCOPE(positionsMtx_m);

			if (posState & QUERY_PENDING) {
				map<CppUtils::Uid, Position>::iterator it = pendingPositions_m.find(posUid);
				if (it==pendingPositions_m.end())
					THROW(CppUtils::OperationFailed, "exc_CanbotFindPosition", "ctx_UpdatePosTags", posUid.toString());

				it->second.tag_m = tag1;
				it->second.tag2_m = tag2;
			}

			if (posState & QUERY_OPEN) {
				map<CppUtils::Uid, Position>::iterator it = openedPositions_m.find(posUid);
				if (it==openedPositions_m.end())
					THROW(CppUtils::OperationFailed, "exc_CanbotFindPosition", "ctx_UpdatePosTags", posUid.toString());

				it->second.tag_m = tag1;
				it->second.tag2_m = tag2;
			}

			if (posState & QUERY_HISTORY) {
				map<CppUtils::Uid, Position>::iterator it = historyPositions_m.find(posUid);
				if (it==historyPositions_m.end())
					THROW(CppUtils::OperationFailed, "exc_CanbotFindPosition", "ctx_UpdatePosTags", posUid.toString());

				it->second.tag_m = tag1;
				it->second.tag2_m = tag2;
			}


			return;
		}

		THROW(CppUtils::OperationFailed, "exc_NotConnected", "ctx_UpdatePosTags", "");
	}
		
	void BtBroker::onDataArrived(	HlpStruct::RtData const& rtdata )
	{
		LOCK_FOR_SCOPE(pendingOrdesMtx_m);
		{
			LOCK_FOR_SCOPE(priceDataMtx_m);

			// before process pending orders for the sessions
			// this must be done inside unblockingIssueOrder, but to avoid state - machine problems
			// it is delayed just before new data arrival.

			// this passes simulation metadata pointer that is guaranteed to be valid 
			

			//doProcessPendingOrders();
			


			// for fast access no more additional checks		
			assert(rtdata.getProviderSymbolCookie() < priceDataFast_m.size());
			PriceData& pdata2 = priceDataFast_m.at(rtdata.getProviderSymbolCookie());

			
			//
			pdata2.curPrice_m.ask_m = rtdata.getRtData().ask_m;
			pdata2.curPrice_m.bid_m = rtdata.getRtData().bid_m;
			pdata2.curPrice_m.time_m = rtdata.getTime();


			if (pdata2.curPrice_m.ask_m < 0 || pdata2.curPrice_m.bid_m <0)
				THROW(CppUtils::OperationFailed, "exc_InvalidBidAskPrices", "ctx_onDataArrived", "");

			executeCurrentOrders();
			
		}

	}

	void BtBroker::initializeBacktest(Inqueue::SimulationProfileData const& simulProfileData )
	{
		// no-op
		assert(false);
	}

	void BtBroker::initializeBacktest(
		Inqueue::SimulationProfileData const& simulProfileData, 
		BrokerBase::ProviderSymbolCookieMap const& providerSymbolCookieMap 
	)
	{
		simulProfileData_m = simulProfileData;
		providerSymbolCookieMap_m = providerSymbolCookieMap;


		int maxCookie=0;
		for(BrokerBase::ProviderSymbolCookieMap::const_iterator it=providerSymbolCookieMap.begin();
			it != providerSymbolCookieMap.end(); it++) 
		{
			CppUtils::String const& provider = it->first;
			map<CppUtils::String, int > const& symbMap  = it->second;

			for(map<CppUtils::String, int >::const_iterator it2 = symbMap.begin(); it2!=symbMap.end();it2++) {
					CppUtils::String const& symbol = it2->first;
					int cookie = it2->second;

					if (maxCookie < cookie)
						maxCookie = cookie;

					//
					BrkLib::SymbolTradingInfo const& symbTradeInfo = simulProfileData_m.symbolsTradingInfo_m[provider][symbol];

			}

		}
		
		// will be index access
		priceDataFast_m.resize(maxCookie + 1);

		// fast symbol info accessor
		symbTradingInfoFast_m.resize(maxCookie + 1);

    // iterate again to create fast access structure
		for(BrokerBase::ProviderSymbolCookieMap::const_iterator it=providerSymbolCookieMap.begin();
			it != providerSymbolCookieMap.end(); it++) 
		{
			CppUtils::String const& provider = it->first;
			map<CppUtils::String, int > const& symbMap  = it->second;

			for(map<CppUtils::String, int >::const_iterator it2 = symbMap.begin(); it2!=symbMap.end();it2++) {
					CppUtils::String const& symbol = it2->first;
					int cookie = it2->second;

					//
					BrkLib::SymbolTradingInfo const& symbTradeInfo = simulProfileData_m.symbolsTradingInfo_m[provider][symbol];
					symbTradingInfoFast_m[cookie] = symbTradeInfo;

			}

		}

			
		
	}
		
	bool BtBroker::isBacktestLibrary() const
	{
		return true;
	}

	bool BtBroker::getTickInfo(
			BrkLib::BrokerSession &session, 
			CppUtils::String const& symbolNum, 
			CppUtils::String const& symbolDenom
		)
	{
		// No-op
		return false;
	}

		
		// subscribe for nes retreival
	bool BtBroker::subscribeForNews(
			BrkLib::BrokerSession &session
		)
	{
		 // No-op
		return false;
	}

		// returns last error information
	bool BtBroker::getLastErrorInfo(
			BrkLib::BrokerSession &session
		)
	{
		// No-op
		return false;
	}

		// subscribe for quotes - they will arrive via onBrokerResponseArrived
	bool BtBroker::subscribeForQuotes(
			BrkLib::BrokerSession &session,
			CppUtils::String const& symbolNum, 
			CppUtils::String const& symbolDenom
		)
	{
		 // No-op
			return false;
	}

		// returns the history from the server
	bool BtBroker::getHistoryInfo(
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

	/**
	* Helpers
	*/

	/*
	PriceData const& BtBroker::resolvePriceData(CppUtils::String const& provider, CppUtils::String const& symbol) const
	{
		map< CppUtils::String, map<CppUtils::String, PriceData > >::const_iterator it = 
			priceData_m.find(provider);

		if (it==priceData_m.end())
			THROW(CppUtils::OperationFailed, "exc_ProviderNotregistred", "ctx_ResolvePriceData", provider);

		map<CppUtils::String, PriceData >::const_iterator it2 = it->second.find(symbol);

		if (it2==it->second.end())
			THROW(CppUtils::OperationFailed, "exc_SymbolNotRegistred", "ctx_ResolvePriceData", symbol);

		PriceData const& pd = it2->second;
		if (pd.curPrice_m.bid_m < 0 || pd.curPrice_m.ask_m < 0)
			THROW(CppUtils::OperationFailed, "exc_BidAndAskMustBeValid", "ctx_ResolvePriceData", provider + " - " + symbol );

		return pd;

	}
	*/

	PriceData const& BtBroker::resolveFastPriceData(int const cookie) const
	{
		// for fast access no more additional checks		
		assert(cookie > 0 && cookie < priceDataFast_m.size());

		return priceDataFast_m.at(cookie);
	}

	int BtBroker::resolveProviderSymbolCookie(CppUtils::String const& provider, CppUtils::String const& symbol) const
	{
		BrokerBase::ProviderSymbolCookieMap::const_iterator it = providerSymbolCookieMap_m.find(provider);

		if (it==providerSymbolCookieMap_m.end())
			THROW(CppUtils::OperationFailed, "exc_ProviderNotRegistred", "ctx_resolveProviderSymbolCookie", provider);


		map<CppUtils::String, int >::const_iterator it2 = it->second.find(symbol);

		if (it2==it->second.end())
			THROW(CppUtils::OperationFailed, "exc_SymbolNotRegistred", "ctx_resolveProviderSymbolCookie", symbol);

		return it2->second;


	}

	int BtBroker::resolveProviderSymbolCookieNoExcept(CppUtils::String const& provider, CppUtils::String const& symbol) const
	{
		if (provider.size() <=0 || symbol.size() <=0)
			return -1;


		BrokerBase::ProviderSymbolCookieMap::const_iterator it = providerSymbolCookieMap_m.find(provider);

		if (it==providerSymbolCookieMap_m.end())
			return -1;


		map<CppUtils::String, int >::const_iterator it2 = it->second.find(symbol);

		if (it2==it->second.end())
			return -1;

		return it2->second;


	}


	Position& BtBroker::locateActivePosition(CppUtils::Uid const& uid)
	{
			
			map<CppUtils::Uid, Position>::iterator it = pendingPositions_m.find(uid);

			if (it==pendingPositions_m.end()) {
				map<CppUtils::Uid, Position>::iterator it2 = openedPositions_m.find(uid);
				
				if (it2==openedPositions_m.end()) {
					THROW(CppUtils::OperationFailed, "exc_cannotFindPosition", "ctx_locateActivePosition", uid.toString());
				}
				else {
					return it2->second;
				}

			}
			else {
				return it->second;
			}
	}

	/*
	SymbolTradingInfo const& BtBroker::resolveTradingInfo(CppUtils::String const& provider, CppUtils::String const& symbol) const
	{
		

		//static SymbolTradingInfo defaultInfo;
		map< CppUtils::String, map<CppUtils::String, SymbolTradingInfo > >::const_iterator it = 
			simulProfileData_m.symbolsTradingInfo_m.find(provider);

		if (it==simulProfileData_m.symbolsTradingInfo_m.end())
			THROW(CppUtils::OperationFailed, "exc_ProviderNotregistred", "ctx_ResolveTradingInfo", provider);
			//return defaultInfo;

		map<CppUtils::String, SymbolTradingInfo >::const_iterator it2 = it->second.find(symbol);

		if (it2==it->second.end())
			THROW(CppUtils::OperationFailed, "exc_SymbolNotRegistred", "ctx_ResolveTradingInfo", symbol);
			//return defaultInfo;

		return it2->second;
	}
	*/

	SymbolTradingInfo const& BtBroker::resolveFastTradingInfo(int const cookie) const
	{
		// for fast access no more additional checks		
		assert(cookie > 0 && cookie < symbTradingInfoFast_m.size());

		return symbTradingInfoFast_m.at(cookie);
	}

	bool BtBroker::isMetaInfoExists(CppUtils::String const& provider, CppUtils::String const& symbol) const
	{
	

		map< CppUtils::String, map<CppUtils::String, SymbolTradingInfo > >::const_iterator it = 
			simulProfileData_m.symbolsTradingInfo_m.find(provider);

		if (it==simulProfileData_m.symbolsTradingInfo_m.end())
			return false;

		map<CppUtils::String, SymbolTradingInfo >::const_iterator it2 = it->second.find(symbol);

		if (it2==it->second.end())
			return false;

		return true;


	}
	   

	
	void BtBroker::prepareResponseHeader(Order const& order, BrokerResponseBase& result)
	{
		result.ID_m.generate();
		result.parentID_m = order.ID_m;
	}

	void BtBroker::prepareBrokerResponseOrderHeader(
		SymbolTradingInfo const& tinfo,
		Order const& order, 
		BrokerResponseOrder& oresult, 
		double const& brokerIssueTime,
		double const& openTime,
		double const& openPrice,
		PosType const posType,
		double const& volume,
		double const& expirTime
	)
	{
		oresult.signDigits_m = tinfo.signDigits_m;
		oresult.pointValue_m = tinfo.pointValue_m;

		oresult.symbolNum_m = order.symbolNum_m;
		oresult.symbolDenom_m = order.symbolDenom_m;
		oresult.provider_m = order.provider_m;

		oresult.brokerIssueTime_m = brokerIssueTime;
		oresult.opVolume_m = volume;
		oresult.opOpenTime_m = openTime;
		oresult.opOpenPrice_m = openPrice;
		oresult.opExpirationTime_m = expirTime;

		oresult.positionType_m = posType;

		// copy comment
		oresult.brokerComment_m = order.comment_m;
		// and tag
		oresult.tag_m = order.tag_m;
		oresult.tag2_m = order.tag2_m;

				
	}

	void BtBroker::initPosCommonValues(
		Position& pos, 
		BrokerResponseOrder& oresult, 
		CppUtils::Uid const& newUid, 
		BtSession& session
	)
	{
			pos.ID_m = newUid;
		
			pos.takeProfitPrice_m = oresult.opTPprice_m;
			pos.stopLossPrice_m = oresult.opSLprice_m;

			pos.volume_m = oresult.opVolume_m;
			pos.provider_m = oresult.provider_m;
			pos.symbolNum_m = oresult.symbolNum_m;
			pos.symbolDenom_m = oresult.symbolDenom_m;
			pos.brokerPositionID_m = oresult.brokerPositionID_m;
			pos.signDigits_m = oresult.signDigits_m;
			pos.pointValue_m = oresult.pointValue_m;

			pos.cookie_symb_prov_m = resolveProviderSymbolCookie( pos.provider_m, pos.symbolNum_m + pos.symbolDenom_m );

			// copy arbitrary tag
			pos.tag_m = oresult.tag_m;
			pos.tag2_m = oresult.tag2_m;

			pos.sessionPtr_m = &session;
	}

	void BtBroker::execPosition(Position& pos, double const &execTime, double const &execPrice, PosState const newState, PosType const newType)
	{
		pos.executeTime_m = execTime;
		if (execPrice > 0)
			pos.executePrice_m = execPrice;
		pos.posState_m = newState;
		pos.lastPosType_m = newType;
	}

	void BtBroker::closePosition(Position& pos, double const &closeTime, double const &closePrice, PosState const newState)
	{
		pos.closeTime_m = closeTime;
		pos.closePrice_m = closePrice;
		pos.posState_m = newState;
	}


	/**
	*/
	
	void BtBroker::processHandshake(Order const& order, BtSession& session, BrokerResponseBase& result)
	{
		prepareResponseHeader(order, result);

		result.resultCode_m = PT_HANDSHAKEARRIVED;
		result.brokerIssueTime_m = -1;
		result.resultDetailCode_m = ODR_OK;
		result.brokerComment_m = "Handshake of backtest library: "+order.comment_m;
	}

	void BtBroker::getSymbolMetainfo(Order const& order, BtSession& session, BrokerResponseMetaInfo& brespMinfo)
	{
		prepareResponseHeader(order, brespMinfo);

		brespMinfo.symbolNum_m = order.symbolNum_m;
		brespMinfo.symbolDenom_m = order.symbolDenom_m;
		brespMinfo.resultCode_m = PT_GENERAL_INFO;

		//
		bool isExist= isMetaInfoExists(order.provider_m, sumNumDenom(order));

		if (!isExist) {
			brespMinfo.resultDetailCode_m = ODR_GENERAL_ERROR;
			return;
		}

	
		// resolve this again if necessary
		if (order.cookie_symb_prov_m < 0)
			order.cookie_symb_prov_m = resolveProviderSymbolCookie( order.provider_m, sumNumDenom(order) );

		SymbolTradingInfo const& sinfo = resolveFastTradingInfo(order.cookie_symb_prov_m);
		
		brespMinfo.resultDetailCode_m = ODR_OK;
		
		brespMinfo.spread_m = sinfo.spread_m;
		brespMinfo.signDigits_m = sinfo.signDigits_m;
		brespMinfo.pointValue_m = sinfo.pointValue_m;
	}

	bool BtBroker::checkTPSLtoChange(  
		Order const& order,
		BrokerResponseOrder& result,
		SymbolTradingInfo const& tinfo, 
		PriceData const& pdata,
		bool const islong,
		double const& newTpPrice,
		double const& newSlPrice
	)
	{
		double openprice, closeprice, measuredprice_open, measuredprice_close;
		bool doSetSl = (order.orderType_m & BrkLib::OP_STOPLOSS_SET) && newSlPrice > 0;
		bool doSetTp = (order.orderType_m & BrkLib::OP_TAKEPROFIT_SET) && newTpPrice > 0;

		if (islong) {
			// long position
			openprice = pdata.curPrice_m.ask_m;
			closeprice = pdata.curPrice_m.bid_m;

			if (doSetTp)
				measuredprice_open = newTpPrice - openprice;

			if (doSetSl)
				measuredprice_close = closeprice - newSlPrice;
		}
		else {
			// short position
			openprice = pdata.curPrice_m.bid_m;
			closeprice = pdata.curPrice_m.ask_m;

			if (doSetTp)
				measuredprice_open =  openprice- newTpPrice;
			
			if (doSetSl)
				measuredprice_close = newSlPrice - closeprice;
			
		}

		if (doSetTp) {
			// check we may open position, assuming we have unlimited margin 
			if (measuredprice_open < (tinfo.minTakeProfitDistance_m + tinfo.slippage_m)) {
				// cannot open position
				result.resultDetailCode_m |= ODR_INVALID_TAKEPROFIT_PRICE;
			}
			else {
				result.resultDetailCode_m |= ODR_TAKEPROFIT_CHANGED_OK;
				result.opTPprice_m = newTpPrice;
			}
		}

		if (doSetSl) {
			if (measuredprice_close < (tinfo.minStopLossDistance_m + tinfo.slippage_m)) {
				// cannot open position
				result.resultDetailCode_m |= ODR_INVALID_STOPLOSS_PRICE;
			}
			else {
				result.resultDetailCode_m |= ODR_STOPLOSS_CHANGED_OK;
				result.opSLprice_m = newSlPrice;
			}
		}

		if ((result.resultDetailCode_m & ODR_INVALID_TAKEPROFIT_PRICE) ||
			(result.resultDetailCode_m & ODR_INVALID_STOPLOSS_PRICE) 
		)
			return false;
		else
			return true;
	}

	bool BtBroker::checkExpirDatetoChange(
			Order const& order,
			BrokerResponseOrder& result,
			SymbolTradingInfo const& tinfo, 
			PriceData const& pdata
	)
	{
		if (order.orderType_m & BrkLib::OP_EXPIR_SET) {
			if (order.orExpirationTime_m > 0 && order.orExpirationTime_m > pdata.curPrice_m.time_m) {
				result.resultDetailCode_m |= BrkLib::ODR_EXPIRATION_CHANGED_OK;
				return true;
			}
			else {
				result.resultDetailCode_m |= BrkLib::ODR_INVALID_EXPIRATION;
				return false;
			}
		}

		return true;
	}
	
	void BtBroker::openBuyPos(Order const& order, BtSession& session, BrokerResponseOrder& oresult)
	{
		prepareResponseHeader(order, oresult);
				
		//CppUtils::String symbol = order.symbolNum_m+order.symbolDenom_m;

		PriceData const& pdata = resolveFastPriceData(order.cookie_symb_prov_m );


		if (!pdata.curPrice_m.isValid())
			THROW(CppUtils::OperationFailed, "exc_InvalidCurrentPrice", "ctx_OpenBuyPos", "");

		
		//SymbolTradingInfo const& tinfo = resolveTradingInfo(order.provider_m, symbol);
		SymbolTradingInfo const& tinfo = resolveFastTradingInfo(order.cookie_symb_prov_m);
		
		// check we may open position, assuming we have unlimited margin 
		bool res = checkTPSLtoChange(order, oresult, tinfo, pdata, true, order.orTPprice_m, order.orSLprice_m );
		bool checkExpir = checkExpirDatetoChange(order, oresult, tinfo, pdata);
		
		prepareBrokerResponseOrderHeader(tinfo, order, oresult, pdata.curPrice_m.time_m, 
			pdata.curPrice_m.time_m, pdata.curPrice_m.ask_m, POS_BUY, order.orVolume_m, order.orExpirationTime_m);

		oresult.brokerPositionID_m = order.ID_m.toString();

		// success
		if (!res) {
			oresult.resultCode_m = PT_CANNOT_ESTABLISH_ORDER;
			return;
		}
		
		if (!checkExpir) {
			oresult.resultCode_m = PT_CANNOT_ESTABLISH_ORDER;
			return;
		}
		
		
		oresult.resultCode_m = PT_ORDER_ESTABLISHED;
		oresult.resultDetailCode_m |= ODR_ORDER_EXECUTED;
		oresult.resultDetailCode_m |= ORD_EXECUTE_PRICE_CHANGED_OK;
		
		

		Position pos;
		// position
		LOCK_FOR_SCOPE(positionsMtx_m) {

			initPosCommonValues(pos, oresult, order.ID_m, session);
			
			pos.installPrice_m = oresult.opOpenPrice_m;
			pos.executePrice_m = oresult.opOpenPrice_m;
			pos.installTime_m  = oresult.opOpenTime_m;
			pos.executeTime_m = oresult.opOpenTime_m;

			pos.posState_m = STATE_OPEN;
			pos.lastPosType_m = POS_BUY;
			
			pos.expirTime_m = order.orExpirationTime_m;

			openedPositions_m[pos.ID_m] =pos;

		}
	}

	void BtBroker::openSellPos(Order const& order, BtSession& session, BrokerResponseOrder& oresult)
	{
		prepareResponseHeader(order, oresult);
			
		
		//CppUtils::String symbol = order.symbolNum_m+order.symbolDenom_m;

		PriceData const& pdata = resolveFastPriceData(order.cookie_symb_prov_m );

		if (!pdata.curPrice_m.isValid())
			THROW(CppUtils::OperationFailed, "exc_InvalidCurrentPrice", "ctx_openSellPos", "");

		
		//SymbolTradingInfo const& tinfo = resolveTradingInfo(order.provider_m, symbol);
		SymbolTradingInfo const& tinfo = resolveFastTradingInfo(order.cookie_symb_prov_m);
				
		// check we may open position, assuming we have unlimited margin 
		bool res = checkTPSLtoChange( order, oresult, tinfo, pdata, false, order.orTPprice_m, order.orSLprice_m );
		bool checkExpir = checkExpirDatetoChange(order, oresult, tinfo, pdata);

		prepareBrokerResponseOrderHeader(tinfo, order, oresult, pdata.curPrice_m.time_m, 
			pdata.curPrice_m.time_m, pdata.curPrice_m.bid_m, POS_SELL, order.orVolume_m, order.orExpirationTime_m);

		oresult.brokerPositionID_m = order.ID_m.toString();
	

		// success
		if (!res) {
			oresult.resultCode_m = PT_CANNOT_ESTABLISH_ORDER;
			return;
		}
		
		if (!checkExpir) {
			oresult.resultCode_m = PT_CANNOT_ESTABLISH_ORDER;
			return;
		}

		// not that for market orders we are ignoring order.opPrice just getting current bid or ask price
		

	
		oresult.resultCode_m = PT_ORDER_ESTABLISHED;
		oresult.resultDetailCode_m |= ODR_ORDER_EXECUTED;
		oresult.resultDetailCode_m |= ORD_EXECUTE_PRICE_CHANGED_OK;
		
		

		Position pos;
		// position
		LOCK_FOR_SCOPE(positionsMtx_m) {
			
			initPosCommonValues(pos, oresult, order.ID_m, session);

			pos.installPrice_m = oresult.opOpenPrice_m;
			pos.executePrice_m = oresult.opOpenPrice_m;
			pos.installTime_m  = oresult.opOpenTime_m;
			pos.executeTime_m = oresult.opOpenTime_m;

			
			pos.posState_m = STATE_OPEN;
			pos.lastPosType_m = POS_SELL;

			pos.expirTime_m = order.orExpirationTime_m;

			openedPositions_m[pos.ID_m] =pos;

		}
	}

	void BtBroker::installBuyLimit(Order const& order, BtSession& session, BrokerResponseOrder& oresult)
	{
		prepareResponseHeader(order, oresult);
		
		//CppUtils::String symbol = order.symbolNum_m+order.symbolDenom_m;

		PriceData const& pdata = resolveFastPriceData(order.cookie_symb_prov_m );

		if (!pdata.curPrice_m.isValid())
			THROW(CppUtils::OperationFailed, "exc_InvalidCurrentPrice", "ctx_installBuyLimit", "");

		
		//SymbolTradingInfo const& tinfo = resolveTradingInfo(order.provider_m, symbol);
		SymbolTradingInfo const& tinfo = resolveFastTradingInfo(order.cookie_symb_prov_m);
		
		// check we may open position, assuming we have unlimited margin 
		bool res = checkTPSLtoChange( order, oresult, tinfo, pdata, true, order.orTPprice_m, order.orSLprice_m );
		bool checkExpir = checkExpirDatetoChange(order, oresult, tinfo, pdata);

		prepareBrokerResponseOrderHeader(tinfo, order, oresult, pdata.curPrice_m.time_m, 
			pdata.curPrice_m.time_m, order.orPrice_m, POS_LIMIT_BUY, order.orVolume_m, order.orExpirationTime_m);

		oresult.brokerPositionID_m = order.ID_m.toString();

		// success
		if (!res) {
			oresult.resultCode_m = PT_CANNOT_ESTABLISH_ORDER;
			return;
		}

		if (!checkExpir) {
			oresult.resultCode_m = PT_CANNOT_ESTABLISH_ORDER;
			return;
		}

		// check whether we may set up buy limit order
		if (order.orPrice_m <= 0) {
			oresult.resultCode_m = PT_CANNOT_ESTABLISH_ORDER;
			oresult.resultDetailCode_m |= ODR_INVALID_PRICE;
			return;
		}

		if ((pdata.curPrice_m.ask_m - order.orPrice_m) < (tinfo.minLimitOrderDistance_m+tinfo.slippage_m)) {
			oresult.resultCode_m = PT_CANNOT_ESTABLISH_ORDER;
			oresult.resultDetailCode_m |= ODR_INVALID_PRICE;

			return;
		}
			
		// success
		

		
		oresult.resultCode_m = PT_ORDER_ESTABLISHED;
		oresult.resultDetailCode_m |= ODR_ORDER_INSTALLED;
		oresult.resultDetailCode_m |= ORD_EXECUTE_PRICE_CHANGED_OK;
		
		
		Position pos;
		// position
		LOCK_FOR_SCOPE(positionsMtx_m) {

			initPosCommonValues(pos, oresult, order.ID_m, session);
			
			// current price
			pos.installPrice_m = pdata.curPrice_m.ask_m;
			pos.installTime_m  = oresult.opOpenTime_m;

			// future execution price
			pos.executePrice_m = order.orPrice_m;

			
			pos.posState_m = STATE_PENDING;
			pos.lastPosType_m = POS_LIMIT_BUY;

			pos.expirTime_m = order.orExpirationTime_m;

			pendingPositions_m[pos.ID_m] =pos;

		}

	}

	void BtBroker::installSellLimit(Order const& order, BtSession& session, BrokerResponseOrder& oresult)
	{
		prepareResponseHeader(order, oresult);
	
		//CppUtils::String symbol = order.symbolNum_m+order.symbolDenom_m;

		PriceData const& pdata = resolveFastPriceData(order.cookie_symb_prov_m );

		if (!pdata.curPrice_m.isValid())
			THROW(CppUtils::OperationFailed, "exc_InvalidCurrentPrice", "ctx_installSellLimit", "");

		
		//SymbolTradingInfo const& tinfo = resolveTradingInfo(order.provider_m, symbol);
		SymbolTradingInfo const& tinfo = resolveFastTradingInfo(order.cookie_symb_prov_m);
		
		// check we may open position, assuming we have unlimited margin 
		bool res = checkTPSLtoChange(order, oresult, tinfo, pdata, false, order.orTPprice_m, order.orSLprice_m );
		bool checkExpir = checkExpirDatetoChange(order, oresult, tinfo, pdata);

		prepareBrokerResponseOrderHeader(tinfo, order, oresult, pdata.curPrice_m.time_m, 
			pdata.curPrice_m.time_m, order.orPrice_m, POS_LIMIT_SELL, order.orVolume_m, order.orExpirationTime_m);

		oresult.brokerPositionID_m = order.ID_m.toString();
	

		// success
		if (!res) {
			oresult.resultCode_m = PT_CANNOT_ESTABLISH_ORDER;
			return;
		}

		if (!checkExpir) {
			oresult.resultCode_m = PT_CANNOT_ESTABLISH_ORDER;
			return;
		}

		// check whether we may set up buy limit order
		if (order.orPrice_m <= 0) {
			oresult.resultCode_m = PT_CANNOT_ESTABLISH_ORDER;
			oresult.resultDetailCode_m |= ODR_INVALID_PRICE;
			return;
		}

		if ((order.orPrice_m - pdata.curPrice_m.bid_m) < (tinfo.minLimitOrderDistance_m+tinfo.slippage_m)) {
			oresult.resultCode_m = PT_CANNOT_ESTABLISH_ORDER;
			oresult.resultDetailCode_m |= ODR_INVALID_PRICE;

			return;
		}
			
		// success
		

		
		oresult.resultCode_m = PT_ORDER_ESTABLISHED;
		oresult.resultDetailCode_m |= ODR_ORDER_INSTALLED;
		oresult.resultDetailCode_m |= ORD_EXECUTE_PRICE_CHANGED_OK;
		
		

		Position pos;
		// position
		LOCK_FOR_SCOPE(positionsMtx_m) {
			initPosCommonValues(pos, oresult, order.ID_m,  session);
			
			// current price
			pos.installPrice_m = pdata.curPrice_m.bid_m;
			pos.installTime_m  = oresult.opOpenTime_m;

			// future execution price
			pos.executePrice_m = order.orPrice_m;
			

			pos.posState_m = STATE_PENDING;
			pos.lastPosType_m = POS_LIMIT_SELL;

			pos.expirTime_m = order.orExpirationTime_m;

			pendingPositions_m[pos.ID_m] =pos;

		}
	}

	void BtBroker::installBuyStop(Order const& order, BtSession& session, BrokerResponseOrder& oresult)
	{
		prepareResponseHeader(order, oresult);
		
		//CppUtils::String symbol = order.symbolNum_m+order.symbolDenom_m;

		PriceData const& pdata = resolveFastPriceData(order.cookie_symb_prov_m );

		if (!pdata.curPrice_m.isValid())
			THROW(CppUtils::OperationFailed, "exc_InvalidCurrentPrice", "ctx_installBuyStop", "");

		
		//SymbolTradingInfo const& tinfo = resolveTradingInfo(order.provider_m, symbol);
		SymbolTradingInfo const& tinfo = resolveFastTradingInfo(order.cookie_symb_prov_m);
		
		// check we may open position, assuming we have unlimited margin 
		bool res = checkTPSLtoChange( order, oresult, tinfo, pdata, true, order.orTPprice_m, order.orSLprice_m );
		bool checkExpir = checkExpirDatetoChange(order, oresult, tinfo, pdata);

		prepareBrokerResponseOrderHeader(tinfo, order, oresult, pdata.curPrice_m.time_m, 
			pdata.curPrice_m.time_m, order.orPrice_m, POS_STOP_BUY, order.orVolume_m, order.orExpirationTime_m);

		oresult.brokerPositionID_m = order.ID_m.toString();
	


		// success
		if (!res) {
			oresult.resultCode_m = PT_CANNOT_ESTABLISH_ORDER;
			return;
		}

		if (!checkExpir) {
			oresult.resultCode_m = PT_CANNOT_ESTABLISH_ORDER;
			return;
		}

		// check whether we may set up buy limit order
		if (order.orPrice_m <= 0) {
			oresult.resultCode_m = PT_CANNOT_ESTABLISH_ORDER;
			oresult.resultDetailCode_m |= ODR_INVALID_PRICE;
			return;
		}

		if ((order.orPrice_m - pdata.curPrice_m.ask_m ) < (tinfo.minStopOrderDistance_m+tinfo.slippage_m)) {
			oresult.resultCode_m = PT_CANNOT_ESTABLISH_ORDER;
			oresult.resultDetailCode_m |= ODR_INVALID_PRICE;

			return;
		}
			
		// success
		

		
		oresult.resultCode_m = PT_ORDER_ESTABLISHED;
		oresult.resultDetailCode_m |= ODR_ORDER_INSTALLED;
		oresult.resultDetailCode_m |= ORD_EXECUTE_PRICE_CHANGED_OK;
		
		
		Position pos;
		// position
		LOCK_FOR_SCOPE(positionsMtx_m) {
		initPosCommonValues(pos, oresult, order.ID_m,  session);

			// current price
			pos.installPrice_m = pdata.curPrice_m.ask_m;
			pos.installTime_m  = oresult.opOpenTime_m;

			// future execution price
			//pos.executePrice_m = order.orPrice_m;
			pos.executePrice_m = order.orStopPrice_m;
		
			pos.posState_m = STATE_PENDING;
			pos.lastPosType_m = POS_STOP_BUY;

			pos.expirTime_m = order.orExpirationTime_m;

			pendingPositions_m[pos.ID_m] =pos;

		}

	}

	void BtBroker::installSellStop(Order const& order, BtSession& session, BrokerResponseOrder& oresult)
	{
		prepareResponseHeader(order, oresult);
		
		//CppUtils::String symbol = order.symbolNum_m+order.symbolDenom_m;

		PriceData const& pdata = resolveFastPriceData(order.cookie_symb_prov_m );

		if (!pdata.curPrice_m.isValid())
			THROW(CppUtils::OperationFailed, "exc_InvalidCurrentPrice", "ctx_installSellStop", "");

		
		//SymbolTradingInfo const& tinfo = resolveTradingInfo(order.provider_m, symbol);
		SymbolTradingInfo const& tinfo = resolveFastTradingInfo(order.cookie_symb_prov_m);
		
		// check we may open position, assuming we have unlimited margin 
		bool res = checkTPSLtoChange( order, oresult, tinfo, pdata, false, order.orTPprice_m, order.orSLprice_m );
		bool checkExpir = checkExpirDatetoChange(order, oresult, tinfo, pdata);

		prepareBrokerResponseOrderHeader(tinfo, order, oresult, pdata.curPrice_m.time_m, 
			pdata.curPrice_m.time_m, order.orPrice_m, POS_STOP_SELL, order.orVolume_m, order.orExpirationTime_m);

		oresult.brokerPositionID_m = order.ID_m.toString();

		// success
		if (!res) {
			oresult.resultCode_m = PT_CANNOT_ESTABLISH_ORDER;
			return;
		}

		if (!checkExpir) {
			oresult.resultCode_m = PT_CANNOT_ESTABLISH_ORDER;
			return;
		}

		// check whether we may set up buy limit order
		if (order.orPrice_m <= 0) {
			oresult.resultCode_m = PT_CANNOT_ESTABLISH_ORDER;
			oresult.resultDetailCode_m |= ODR_INVALID_PRICE;
			return;
		}

		if ((pdata.curPrice_m.bid_m - order.orPrice_m ) < (tinfo.minStopOrderDistance_m+tinfo.slippage_m)) {
			oresult.resultCode_m = PT_CANNOT_ESTABLISH_ORDER;
			oresult.resultDetailCode_m |= ODR_INVALID_PRICE;

			return;
		}
			
		// success
		

		
		oresult.resultCode_m = PT_ORDER_ESTABLISHED;
		oresult.resultDetailCode_m |= ODR_ORDER_INSTALLED;
		oresult.resultDetailCode_m |= ORD_EXECUTE_PRICE_CHANGED_OK;
				

		Position pos;
		// position
		LOCK_FOR_SCOPE(positionsMtx_m) {
			initPosCommonValues(pos, oresult,order.ID_m,  session);

			// current price
			pos.installPrice_m = pdata.curPrice_m.bid_m;
			pos.installTime_m  = oresult.opOpenTime_m;

			// future execution price
			//pos.executePrice_m = order.orPrice_m;
			pos.executePrice_m = order.orStopPrice_m;
			
			pos.posState_m = STATE_PENDING;
			pos.lastPosType_m = POS_STOP_SELL;

			pos.expirTime_m = order.orExpirationTime_m;

			pendingPositions_m[pos.ID_m] =pos;

		}
	}

	void BtBroker::changeSLTPExecExpir(Order const& order, BtSession& session, BrokerResponseOrder& oresult)
	{
		prepareResponseHeader(order, oresult);
		//CppUtils::String symbol = order.symbolNum_m+order.symbolDenom_m;

		PriceData const& pdata = resolveFastPriceData(order.cookie_symb_prov_m );

		if (!pdata.curPrice_m.isValid())
			THROW(CppUtils::OperationFailed, "exc_InvalidCurrentPrice", "ctx_changeSL", "");

		
		//SymbolTradingInfo const& tinfo = resolveTradingInfo(order.provider_m, symbol);
		SymbolTradingInfo const& tinfo = resolveFastTradingInfo(order.cookie_symb_prov_m);

		LOCK_FOR_SCOPE(positionsMtx_m) {


			// navigate active position
			Position& activePosition = locateActivePosition(CppUtils::Uid(order.brokerPositionID_m));

			// store current params
			

			// note these are old values, executim prive must be adopted if successfully changed
			prepareBrokerResponseOrderHeader(tinfo, order, oresult, pdata.curPrice_m.time_m, 
					pdata.curPrice_m.time_m, order.orPrice_m, activePosition.lastPosType_m, activePosition.volume_m, order.orExpirationTime_m);

			oresult.brokerPositionID_m = activePosition.ID_m.toString();

			// expiration
			if (order.orderType_m & BrkLib::OP_EXPIR_SET) {
				bool expirResult = checkExpirDatetoChange( order, oresult, tinfo, pdata );
				if (!expirResult) {
						oresult.resultCode_m = PT_CANNOT_MODIFY_ORDER;
						return;
				}

				oresult.resultCode_m = PT_ORDER_MODIFIED;
				activePosition.expirTime_m = order.orExpirationTime_m;
	
			}; //

			// whether we can change execute price
			if (order.orderType_m & BrkLib::OP_EXEC_PRICE_SET) {
				if (activePosition.posState_m != STATE_PENDING) {
					oresult.resultCode_m = PT_CANNOT_MODIFY_ORDER;
					return;
				}

				// invalid price
				if (order.orPrice_m <= 0) {
					oresult.resultCode_m = PT_CANNOT_MODIFY_ORDER;
					oresult.resultDetailCode_m |= ODR_INVALID_PRICE;
					return;
				}

				// try to change exec price
				if (activePosition.lastPosType_m==POS_LIMIT_BUY) {
					if ((pdata.curPrice_m.ask_m - order.orPrice_m) < (tinfo.minLimitOrderDistance_m+tinfo.slippage_m)) {
						oresult.resultCode_m = PT_CANNOT_MODIFY_ORDER;
						oresult.resultDetailCode_m |= ODR_INVALID_PRICE;

						return;
					}
				}
				else if (activePosition.lastPosType_m==POS_LIMIT_SELL) {
					if ((order.orPrice_m - pdata.curPrice_m.bid_m) < (tinfo.minLimitOrderDistance_m+tinfo.slippage_m)) {
						oresult.resultCode_m = PT_CANNOT_MODIFY_ORDER;
						oresult.resultDetailCode_m |= ODR_INVALID_PRICE;

						return;
					}
				}
				else if (activePosition.lastPosType_m==POS_STOP_BUY) {
					if ((order.orPrice_m - pdata.curPrice_m.ask_m ) < (tinfo.minStopOrderDistance_m+tinfo.slippage_m)) {
						oresult.resultCode_m = PT_CANNOT_MODIFY_ORDER;
						oresult.resultDetailCode_m |= ODR_INVALID_PRICE;

						return;
					}
				}
				else if (activePosition.lastPosType_m==POS_STOP_SELL) {
					if ((pdata.curPrice_m.bid_m - order.orPrice_m ) < (tinfo.minStopOrderDistance_m+tinfo.slippage_m)) {
						oresult.resultCode_m = PT_CANNOT_MODIFY_ORDER;
						oresult.resultDetailCode_m |= ODR_INVALID_PRICE;

						return;
					}
				}
				else 
					assert(false);
				
				// successfully modified
				oresult.resultCode_m = PT_ORDER_MODIFIED;
				oresult.resultDetailCode_m |= ORD_EXECUTE_PRICE_CHANGED_OK;
				activePosition.executePrice_m = order.orPrice_m;
			
			};

			
			

			// change TP SL
			// try to set SL
			if (order.orderType_m & BrkLib::OP_STOPLOSS_SET /*&&  (order.orSLprice_m != activePosition.stopLossPrice_m)*/) {
				// indicates that we changed the price
				// check we may open position, assuming we have unlimited margin 
				bool res = checkTPSLtoChange(order, oresult, tinfo, pdata, isLong(activePosition.lastPosType_m), -1, order.orSLprice_m );
				
				
				// success
				if (!res) {
					oresult.resultCode_m = PT_CANNOT_MODIFY_ORDER;
					return;
				}

				// success
				activePosition.stopLossPrice_m = oresult.opSLprice_m;
				oresult.resultCode_m = PT_ORDER_MODIFIED;

			}

			// try to set TP
			// try to set SL
			if (order.orderType_m & BrkLib::OP_TAKEPROFIT_SET /*&&  (order.orTPprice_m != activePosition.takeProfitPrice_m)*/) {
				// indicates that we changed the price
				// check we may open position, assuming we have unlimited margin 
				bool res = checkTPSLtoChange(order, oresult, tinfo, pdata, isLong(activePosition.lastPosType_m), order.orTPprice_m, -1 );
			
				
				// success
				if (!res) {
					oresult.resultCode_m = PT_CANNOT_MODIFY_ORDER;
					return;
				}

				// success !!
				activePosition.takeProfitPrice_m = oresult.opTPprice_m;
				oresult.resultCode_m = PT_ORDER_MODIFIED;

			}
			//
		

		}
	};
	

	
	void BtBroker::cancelOrder(Order const& order, BtSession& session, BrokerResponseOrder& oresult)
	{
		// this cancels the order if this is not executed

		prepareResponseHeader(order, oresult);
		
		//CppUtils::String symbol = order.symbolNum_m+order.symbolDenom_m;

		PriceData const& pdata = resolveFastPriceData(order.cookie_symb_prov_m );

		if (!pdata.curPrice_m.isValid())
			THROW(CppUtils::OperationFailed, "exc_InvalidCurrentPrice", "ctx_changeTP", "");
		
		//SymbolTradingInfo const& tinfo = resolveTradingInfo(order.provider_m, symbol);
		SymbolTradingInfo const& tinfo = resolveFastTradingInfo(order.cookie_symb_prov_m);
	
		oresult.signDigits_m = tinfo.signDigits_m;
		oresult.pointValue_m = tinfo.pointValue_m;

		oresult.symbolNum_m = order.symbolNum_m;
		oresult.symbolDenom_m = order.symbolDenom_m;
		oresult.provider_m = order.provider_m;
		oresult.brokerIssueTime_m = pdata.curPrice_m.time_m;
		


		LOCK_FOR_SCOPE(positionsMtx_m) {
			// find pending position

			CppUtils::Uid uid(order.brokerPositionID_m);
			map<CppUtils::Uid, Position>::iterator it = pendingPositions_m.find(uid);
			
		

			if (it==pendingPositions_m.end()) {
				// not found
				oresult.resultCode_m = PT_CANNOT_CANCEL_ORDER;
				oresult.resultDetailCode_m |= ORD_INVALID_ORDER;
				return;
			}

			// 
			Position& pendingPos = it->second;
			oresult.brokerPositionID_m = pendingPos.ID_m.toString();
			pendingPos.closeTime_m = pdata.curPrice_m.time_m;


			oresult.opSLprice_m = pendingPos.stopLossPrice_m;
			oresult.opTPprice_m = pendingPos.takeProfitPrice_m;

			oresult.opVolume_m = pendingPos.volume_m;
			oresult.opOpenTime_m = pendingPos.installTime_m;
			oresult.opOpenPrice_m = pendingPos.installPrice_m;
			oresult.positionType_m = pendingPos.lastPosType_m;
			oresult.opCloseTime_m = pendingPos.closeTime_m;
			oresult.closeReason_m = REASON_CANCEL;
			

			pendingPos.posState_m = STATE_HISTORY;

			//close reason
			
			pendingPos.closeReason_m = REASON_CANCEL;

			// write as history
			historyPositions_m[ uid ] = pendingPos;

			oresult.resultCode_m  = PT_ORDER_CANCELLED;
			oresult.resultDetailCode_m = ODR_OK;

			oresult.brokerComment_m = order.comment_m;
			oresult.tag_m = order.tag_m;
			oresult.tag2_m = order.tag2_m;
			

			// remove
			pendingPositions_m.erase(uid); 
				
		}

	}

	void BtBroker::closeOrder(Order const& order, BtSession& session, BrokerResponseOrder& oresult)
	{
		// this closes the order

		prepareResponseHeader(order, oresult);
		//CppUtils::String symbol = order.symbolNum_m+order.symbolDenom_m;

		PriceData const& pdata = resolveFastPriceData(order.cookie_symb_prov_m );

		if (!pdata.curPrice_m.isValid())
			THROW(CppUtils::OperationFailed, "exc_InvalidCurrentPrice", "ctx_changeTP", "");
		
		//SymbolTradingInfo const& tinfo = resolveTradingInfo(order.provider_m, symbol);
		SymbolTradingInfo const& tinfo = resolveFastTradingInfo(order.cookie_symb_prov_m);
		
		oresult.signDigits_m = tinfo.signDigits_m;
		oresult.pointValue_m = tinfo.pointValue_m;

		oresult.symbolNum_m = order.symbolNum_m;
		oresult.symbolDenom_m = order.symbolDenom_m;
		oresult.provider_m = order.provider_m;
		oresult.brokerIssueTime_m = pdata.curPrice_m.time_m;



		LOCK_FOR_SCOPE(positionsMtx_m) {
			// find pending position

			CppUtils::Uid uid(order.brokerPositionID_m);
			map<CppUtils::Uid, Position>::iterator it = openedPositions_m.find(uid);
		

			if (it==openedPositions_m.end()) {
					oresult.resultCode_m = PT_CANNOT_CLOSE_ORDER;
					oresult.resultDetailCode_m |= ORD_INVALID_ORDER;
					return;
			}
		
			Position& activePos = it->second;
			oresult.brokerPositionID_m = activePos.ID_m.toString();
			activePos.posState_m = STATE_HISTORY;

			// determine close price
			if (activePos.lastPosType_m == POS_BUY) {
				activePos.closePrice_m =  pdata.curPrice_m.bid_m - tinfo.slippage_m;
			}
			else if (activePos.lastPosType_m == POS_SELL) {
				activePos.closePrice_m =  pdata.curPrice_m.ask_m + tinfo.slippage_m;
			}
			else 
				// all living orders are either POS_BUY or POS_SELL
				// 
				assert(false);


			//



			activePos.closeTime_m = pdata.curPrice_m.time_m;		

			oresult.opOpenTime_m = activePos.executeTime_m;
			oresult.opOpenPrice_m = activePos.executePrice_m;
			
			oresult.opTPprice_m = activePos.takeProfitPrice_m;
			oresult.opSLprice_m  =activePos.stopLossPrice_m;
			oresult.opVolume_m = activePos.volume_m;
			oresult.positionType_m = activePos.lastPosType_m;
			oresult.opComission_m = tinfo.comission_m;

			oresult.opCloseTime_m = activePos.closeTime_m;
			oresult.opClosePrice_m = activePos.closePrice_m;
			oresult.closeReason_m = REASON_NORMAL;

			oresult.brokerComment_m = order.comment_m;
			oresult.tag_m = order.tag_m;
			oresult.tag2_m = order.tag2_m;

						
			//close reason
			activePos.closeReason_m = REASON_NORMAL;

			// write as history
			historyPositions_m[ uid ] = activePos;
			
			// return status ok
			oresult.resultCode_m  = PT_ORDER_CLOSED;
			oresult.resultDetailCode_m = ODR_OK;

			// remove
			openedPositions_m.erase(uid);
		}
	}

	//

	void BtBroker::doProcessPendingOrderHelper(BrkLib::BrokerSession &session, Order const& order)
	{
		

		//try {

		BrkLib::BtSession &btsession = (BrkLib::BtSession &)session;

			//
			if (!order.ID_m.isValid()) {
				THROW(CppUtils::OperationFailed, "exc_InvalidOrderID", "ctx_ProcessPendingOrders", "");
			}

			//
			if (order.orderType_m & BrkLib::OP_HANDSHAKE) {
				// handshake
				BrokerResponseOrder brokerResponse;
				
				processHandshake(order, btsession, brokerResponse);
				onBrokerResponseArrived(btsession, brokerResponse);
			}
			else if (order.orderType_m & BrkLib::OP_GET_SYMBOL_METAINFO) {
				
				BrokerResponseMetaInfo brespMinfo;

				getSymbolMetainfo(order, btsession, brespMinfo);
				onBrokerResponseArrived(btsession, brespMinfo);
			}
			else if (order.orderType_m & BrkLib::OP_BUY) {
				BrokerResponseOrder brokerResponse;
				openBuyPos(order, btsession, brokerResponse);
				onBrokerResponseArrived(btsession, brokerResponse);
			}
			else if(	order.orderType_m & BrkLib::OP_SELL ) {
				BrokerResponseOrder brokerResponse;
				openSellPos(order, btsession, brokerResponse);
				onBrokerResponseArrived(btsession, brokerResponse);
			}
			else if( order.orderType_m & BrkLib::OP_BUY_LIMIT ) {
				BrokerResponseOrder brokerResponse;
				installBuyLimit(order, btsession, brokerResponse);
				onBrokerResponseArrived(btsession, brokerResponse);
			}
			else if(order.orderType_m & BrkLib::OP_SELL_LIMIT ) {
				BrokerResponseOrder brokerResponse;
				installSellLimit(order, btsession, brokerResponse);
				onBrokerResponseArrived(btsession, brokerResponse);
			}
			else if (	order.orderType_m & BrkLib::OP_BUY_STOP ) {
				BrokerResponseOrder brokerResponse;
				installBuyStop(order, btsession, brokerResponse);
				onBrokerResponseArrived(btsession, brokerResponse);
			}
			else if (	order.orderType_m & BrkLib::OP_SELL_STOP ) {
				BrokerResponseOrder brokerResponse;
				installSellStop(order, btsession, brokerResponse);
				onBrokerResponseArrived(btsession, brokerResponse);
			}
			
			else if (	order.orderType_m & BrkLib::OP_STOPLOSS_SET	|| 
								order.orderType_m & BrkLib::OP_TAKEPROFIT_SET ||
								order.orderType_m & BrkLib::OP_EXEC_PRICE_SET ||
								order.orderType_m & BrkLib::OP_EXPIR_SET
			) {
				BrokerResponseOrder brokerResponse;
				changeSLTPExecExpir(order, btsession, brokerResponse);
				onBrokerResponseArrived(btsession, brokerResponse);
			}
			else if (order.orderType_m & BrkLib::OP_CANCEL) {
				BrokerResponseOrder brokerResponse;
				cancelOrder(order, btsession, brokerResponse);
				onBrokerResponseArrived(btsession, brokerResponse);
			}
			else if (order.orderType_m & BrkLib::OP_CLOSE) {
				BrokerResponseOrder brokerResponse;
				closeOrder(order, btsession, brokerResponse);
				onBrokerResponseArrived(btsession, brokerResponse);
			}
			else
				THROW(CppUtils::OperationFailed, "exc_InvalidOrder", "ctx_ProcessPendingOrders", order.ID_m.toString());
		//}
		//catch(CppUtils::OperationFailed& e)
		//{
		//}
		//catch(...)
		//{
		//}

	}
	/*
	void BtBroker::doProcessPendingOrders()
	{
		while(!pendingOrders_m.empty()) {
			PendingCommand& cmd = pendingOrders_m.front();
			Order& order = cmd.order_m;

			//
			if (!order.ID_m.isValid()) {
				THROW(CppUtils::OperationFailed, "exc_InvalidOrderID", "ctx_ProcessPendingOrders", "");
			}

			//
			if (order.orderType_m & BrkLib::OP_HANDSHAKE) {
				// handshake
				BrokerResponseOrder brokerResponse;
				
				processHandshake(order, cmd.btSession_m, brokerResponse);
				onBrokerResponseArrived(cmd.btSession_m, brokerResponse);
			}
			else if (order.orderType_m & BrkLib::OP_GET_SYMBOL_METAINFO) {
				
				BrokerResponseMetaInfo brespMinfo;

				getSymbolMetainfo(order, cmd.btSession_m, brespMinfo);
				onBrokerResponseArrived(cmd.btSession_m, brespMinfo);
			}
			else if (order.orderType_m & BrkLib::OP_BUY) {
				BrokerResponseOrder brokerResponse;
				openBuyPos(order, cmd.btSession_m, brokerResponse);
				onBrokerResponseArrived(cmd.btSession_m, brokerResponse);
			}
			else if(	order.orderType_m & BrkLib::OP_SELL ) {
				BrokerResponseOrder brokerResponse;
				openSellPos(order, cmd.btSession_m, brokerResponse);
				onBrokerResponseArrived(cmd.btSession_m, brokerResponse);
			}
			else if( order.orderType_m & BrkLib::OP_BUY_LIMIT ) {
				BrokerResponseOrder brokerResponse;
				installBuyLimit(order, cmd.btSession_m, brokerResponse);
				onBrokerResponseArrived(cmd.btSession_m, brokerResponse);
			}
			else if(order.orderType_m & BrkLib::OP_SELL_LIMIT ) {
				BrokerResponseOrder brokerResponse;
				installSellLimit(order, cmd.btSession_m, brokerResponse);
				onBrokerResponseArrived(cmd.btSession_m, brokerResponse);
			}
			else if (	order.orderType_m & BrkLib::OP_BUY_STOP ) {
				BrokerResponseOrder brokerResponse;
				installBuyStop(order, cmd.btSession_m, brokerResponse);
				onBrokerResponseArrived(cmd.btSession_m, brokerResponse);
			}
			else if (	order.orderType_m & BrkLib::OP_SELL_STOP ) {
				BrokerResponseOrder brokerResponse;
				installSellStop(order, cmd.btSession_m, brokerResponse);
				onBrokerResponseArrived(cmd.btSession_m, brokerResponse);
			}
			
			else if (	order.orderType_m & BrkLib::OP_STOPLOSS_SET	|| 
								order.orderType_m & BrkLib::OP_TAKEPROFIT_SET ||
								order.orderType_m & BrkLib::OP_EXEC_PRICE_SET ||
								order.orderType_m & BrkLib::OP_EXPIR_SET
			) {
				BrokerResponseOrder brokerResponse;
				changeSLTPExecExpir(order, cmd.btSession_m, brokerResponse);
				onBrokerResponseArrived(cmd.btSession_m, brokerResponse);
			}
			else if (order.orderType_m & BrkLib::OP_CANCEL) {
				BrokerResponseOrder brokerResponse;
				cancelOrder(order, cmd.btSession_m, brokerResponse);
				onBrokerResponseArrived(cmd.btSession_m, brokerResponse);
			}
			else if (order.orderType_m & BrkLib::OP_CLOSE) {
				BrokerResponseOrder brokerResponse;
				closeOrder(order, cmd.btSession_m, brokerResponse);
				onBrokerResponseArrived(cmd.btSession_m, brokerResponse);
			}
			else
				THROW(CppUtils::OperationFailed, "exc_InvalidOrder", "ctx_ProcessPendingOrders", order.ID_m.toString());



			//
			pendingOrders_m.pop();
		}
	}
	*/


	void BtBroker::executeCurrentOrders()
	{
		// this goes through all the positions and determine - either to execute pending, close current, etc...

		LOCK_FOR_SCOPE(positionsMtx_m) {
			
			
			CppUtils::UidList pending, active;
			CppUtils::getKeys<CppUtils::Uid, Position>(pending, pendingPositions_m);
			
			// pending positions
			for(int i = 0; i < pending.size(); i++) {
				Position& pendingPos =  pendingPositions_m[ pending[ i ] ];

				//CppUtils::String symbol = pendingPos.symbolNum_m + pendingPos.symbolDenom_m;
				PriceData const& pdata = resolveFastPriceData(pendingPos.cookie_symb_prov_m );

				if (!pdata.curPrice_m.isValid())
					THROW(CppUtils::OperationFailed, "exc_InvalidCurrentPrice", "ctx_executeCurrentOrders", "");
		
				
				//SymbolTradingInfo const& tinfo = resolveTradingInfo(pendingPos.provider_m, symbol);
				SymbolTradingInfo const& tinfo = resolveFastTradingInfo(pendingPos.cookie_symb_prov_m);

				// check whether we can execute current orders and check whether we can adjsut TP & SL
				
				PosState oldsate = pendingPos.posState_m;
				
				// expiration
				if (pendingPos.expirTime_m >0 && pendingPos.expirTime_m <= pdata.curPrice_m.time_m) {
					
					// 
					BrokerResponseOrder brokerResponse;

					brokerResponse.ID_m.generate();

					// parent ID
					//brokerResponse.parentID_m = pendingPos.ID_m;
					brokerResponse.signDigits_m = tinfo.signDigits_m;
					brokerResponse.pointValue_m = tinfo.pointValue_m;
					
					brokerResponse.brokerIssueTime_m = pdata.curPrice_m.time_m;
					brokerResponse.symbolNum_m = pendingPos.symbolNum_m;
					brokerResponse.symbolDenom_m = pendingPos.symbolDenom_m;
					brokerResponse.provider_m = pendingPos.provider_m;

					brokerResponse.brokerPositionID_m = pendingPos.ID_m.toString();
					brokerResponse.resultCode_m  = PT_ORDER_CANCELLED;
					brokerResponse.resultDetailCode_m = ODR_OK;
			
					brokerResponse.positionType_m = pendingPos.lastPosType_m;

					brokerResponse.opSLprice_m = pendingPos.stopLossPrice_m;
					brokerResponse.opTPprice_m = pendingPos.takeProfitPrice_m;

					brokerResponse.opOpenPrice_m = pendingPos.installPrice_m;
					brokerResponse.opOpenTime_m = pendingPos.installTime_m;
					brokerResponse.opVolume_m = pendingPos.volume_m;

				
					brokerResponse.opCloseTime_m = pendingPos.closeTime_m;
					brokerResponse.closeReason_m = REASON_CANCEL;

					brokerResponse.tag_m = pendingPos.tag_m;
					brokerResponse.tag2_m = pendingPos.tag2_m;

			
					// this must not be NULL
					assert(pendingPos.sessionPtr_m);
					
					// propagate
					onBrokerResponseArrived(*pendingPos.sessionPtr_m, brokerResponse);

					//
					pendingPos.closeTime_m = pdata.curPrice_m.time_m;
					pendingPos.posState_m = STATE_HISTORY;

					//close reason
					pendingPos.closeReason_m = REASON_CANCEL;

					
					// remove from list
					pendingPositions_m.erase( pending[ i ] );
					// and mark as a history
					historyPositions_m[ pending[ i ] ] = pendingPos;
					
					// continue;
					continue;
				}

				if (pendingPos.lastPosType_m==POS_LIMIT_BUY) {
					
					if (pdata.curPrice_m.ask_m <= pendingPos.executePrice_m) {
						// round to market price

						execPosition(
							pendingPos, 
							pdata.curPrice_m.time_m, 
							tinfo.executePosExactPrices_m ? -1 : pdata.curPrice_m.ask_m,
							STATE_OPEN,
							POS_BUY
						);
						

					}


				}
				else if (pendingPos.lastPosType_m==POS_LIMIT_SELL) {
					if (pdata.curPrice_m.bid_m >= pendingPos.executePrice_m) {
						// round to market price

						execPosition(
							pendingPos, 
							pdata.curPrice_m.time_m, 
							tinfo.executePosExactPrices_m ? -1 : pdata.curPrice_m.bid_m,
							STATE_OPEN,
							POS_SELL
						);
						
					}

				}
				else if (pendingPos.lastPosType_m==POS_STOP_BUY) {
					if (pdata.curPrice_m.ask_m >= pendingPos.executePrice_m) {

						execPosition(
							pendingPos, 
							pdata.curPrice_m.time_m, 
							tinfo.executePosExactPrices_m ? -1 : pdata.curPrice_m.ask_m,
							STATE_OPEN,
							POS_BUY
						);

						

					}

				}
				else if (pendingPos.lastPosType_m==POS_STOP_SELL) {
					if (pdata.curPrice_m.bid_m <= pendingPos.executePrice_m) {

						execPosition(
							pendingPos, 
							pdata.curPrice_m.time_m, 
							tinfo.executePosExactPrices_m ? -1 : pdata.curPrice_m.bid_m,
							STATE_OPEN,
							POS_SELL
						);
					
					}
				}
				else
					// here there is only pending positions
					assert( false );
				

				if (oldsate != pendingPos.posState_m) {

					
					BrokerResponseOrder brokerResponse;

					brokerResponse.ID_m.generate();

					// parent ID
					//brokerResponse.parentID_m = pendingPos.ID_m;
					brokerResponse.signDigits_m = tinfo.signDigits_m;
					brokerResponse.pointValue_m = tinfo.pointValue_m;

					brokerResponse.brokerIssueTime_m = pdata.curPrice_m.time_m;
					brokerResponse.symbolNum_m = pendingPos.symbolNum_m;
					brokerResponse.symbolDenom_m = pendingPos.symbolDenom_m;
					brokerResponse.provider_m = pendingPos.provider_m;

					brokerResponse.brokerPositionID_m = pendingPos.ID_m.toString();
					brokerResponse.resultCode_m = PT_ORDER_ESTABLISHED;
					brokerResponse.resultDetailCode_m |= ODR_ORDER_EXECUTED;
					brokerResponse.positionType_m = (isLong(pendingPos.lastPosType_m) ? POS_BUY:POS_SELL);

					brokerResponse.opSLprice_m = pendingPos.stopLossPrice_m;
					brokerResponse.opTPprice_m = pendingPos.takeProfitPrice_m;

					brokerResponse.opOpenPrice_m = pendingPos.executePrice_m;
					brokerResponse.opOpenTime_m = pendingPos.executeTime_m;
					brokerResponse.opVolume_m = pendingPos.volume_m;

					brokerResponse.tag_m = pendingPos.tag_m;
					brokerResponse.tag2_m = pendingPos.tag2_m;
			
					// this must not be NULL
					assert(pendingPos.sessionPtr_m);
					
					// propagate
					onBrokerResponseArrived(*pendingPos.sessionPtr_m, brokerResponse);
					

					//
					openedPositions_m[ pending[ i ] ] = pendingPos;
					pendingPositions_m.erase( pending[ i ] );
					

				}
			
			} // end loop


			// now get active positions and see whether we may need to do something with them
			// like close because of stop loss or take profit 
			CppUtils::getKeys<CppUtils::Uid, Position>(active, openedPositions_m);
			for(int i = 0; i < active.size(); i++) {
				Position& activePos =  openedPositions_m[ active[ i ] ];

				//CppUtils::String symbol = activePos.symbolNum_m + activePos.symbolDenom_m;

				PriceData const& pdata = resolveFastPriceData(activePos.cookie_symb_prov_m );

				if (!pdata.curPrice_m.isValid())
					THROW(CppUtils::OperationFailed, "exc_InvalidCurrentPrice", "ctx_executeCurrentOrders", "");
		
				
				//SymbolTradingInfo const& tinfo = resolveTradingInfo(activePos.provider_m, symbol);
				SymbolTradingInfo const& tinfo = resolveFastTradingInfo(activePos.cookie_symb_prov_m);

				// 
				if (activePos.lastPosType_m==POS_BUY) {
					
					// expiration
					if (activePos.expirTime_m >0 && activePos.expirTime_m <= pdata.curPrice_m.time_m) {
						
						// closing with market price
						closePosition(
							activePos, 
							pdata.curPrice_m.time_m, 
							pdata.curPrice_m.bid_m - tinfo.slippage_m, 
							STATE_HISTORY);

						//close reason
						activePos.closeReason_m = REASON_EXPIRATION;
					}

					if (activePos.stopLossPrice_m> 0 && activePos.stopLossPrice_m>= pdata.curPrice_m.bid_m) {
						// closing
						closePosition(
							activePos, 
							pdata.curPrice_m.time_m, 
							tinfo.executePosExactPrices_m ? activePos.stopLossPrice_m: (pdata.curPrice_m.bid_m - tinfo.slippage_m), 
							STATE_HISTORY);

						//close reason
						activePos.closeReason_m = REASON_SL;

					}

					if (activePos.takeProfitPrice_m> 0 && activePos.takeProfitPrice_m <= pdata.curPrice_m.bid_m) {
						// closing
						closePosition(
							activePos, 
							pdata.curPrice_m.time_m, 
							tinfo.executePosExactPrices_m ? activePos.takeProfitPrice_m: (pdata.curPrice_m.bid_m - tinfo.slippage_m), 
							STATE_HISTORY);

						//close reason
						activePos.closeReason_m = REASON_TP;

					}


				}
				else if (activePos.lastPosType_m==POS_SELL) {

					// expiration
					if (activePos.expirTime_m >0 && activePos.expirTime_m <= pdata.curPrice_m.time_m) {
						
						// closing with market price
						closePosition(
							activePos, 
							pdata.curPrice_m.time_m, 
							pdata.curPrice_m.ask_m + tinfo.slippage_m, 
							STATE_HISTORY);

						//close reason
						activePos.closeReason_m = REASON_EXPIRATION;
					}

					if (activePos.stopLossPrice_m> 0 && activePos.stopLossPrice_m<= pdata.curPrice_m.ask_m) {
						// closing
						closePosition(
							activePos, 
							pdata.curPrice_m.time_m, 
							tinfo.executePosExactPrices_m ? activePos.stopLossPrice_m: (pdata.curPrice_m.ask_m + tinfo.slippage_m), 
							STATE_HISTORY);

						//close reason
						activePos.closeReason_m = REASON_SL;
					
					}

					if (activePos.takeProfitPrice_m> 0 && activePos.takeProfitPrice_m >= pdata.curPrice_m.ask_m) {
						// closing
						closePosition(
							activePos,
							pdata.curPrice_m.time_m, 
							tinfo.executePosExactPrices_m? activePos.takeProfitPrice_m: (pdata.curPrice_m.ask_m + tinfo.slippage_m), 
							STATE_HISTORY);

						//close reason
						activePos.closeReason_m = REASON_TP;

					}

				}
				else
					assert(false);

				if (activePos.posState_m == STATE_HISTORY) {
					// TODO!!! - create broker response


					BrokerResponseOrder brokerResponse;

					brokerResponse.ID_m.generate();

					// parent ID
					//brokerResponse.parentID_m = activePos.ID_m;
					brokerResponse.signDigits_m = tinfo.signDigits_m;
					brokerResponse.pointValue_m = tinfo.pointValue_m;

					brokerResponse.brokerIssueTime_m = pdata.curPrice_m.time_m;
					brokerResponse.symbolNum_m = activePos.symbolNum_m;
					brokerResponse.symbolDenom_m = activePos.symbolDenom_m;
					brokerResponse.provider_m = activePos.provider_m;

					brokerResponse.brokerPositionID_m = activePos.ID_m.toString();
					brokerResponse.resultCode_m = PT_ORDER_CLOSED;
					brokerResponse.resultDetailCode_m |= ODR_OK;
					brokerResponse.positionType_m = (isLong(activePos.lastPosType_m) ? POS_BUY:POS_SELL);
		
					brokerResponse.opOpenPrice_m = activePos.executePrice_m;
					brokerResponse.opOpenTime_m = activePos.executeTime_m;
					brokerResponse.opVolume_m = activePos.volume_m;

					brokerResponse.opSLprice_m = activePos.stopLossPrice_m;
					brokerResponse.opTPprice_m = activePos.takeProfitPrice_m;

					brokerResponse.opComission_m = tinfo.comission_m;

					brokerResponse.opCloseTime_m = activePos.closeTime_m;
					brokerResponse.opClosePrice_m = activePos.closePrice_m;

					brokerResponse.closeReason_m = activePos.closeReason_m;
					brokerResponse.opExpirationTime_m = activePos.expirTime_m;

					brokerResponse.tag_m = activePos.tag_m;
					brokerResponse.tag2_m = activePos.tag2_m;

			
					// this must not be NULL
					assert(activePos.sessionPtr_m);
					
					// propagate
					onBrokerResponseArrived(*activePos.sessionPtr_m, brokerResponse);
					

					//
					historyPositions_m[ active[ i ]] = activePos;

					// delete
					openedPositions_m.erase(active[ i ] );
				}


			} // end loop

			
		} // end mtx
	}


}; // end of namespace