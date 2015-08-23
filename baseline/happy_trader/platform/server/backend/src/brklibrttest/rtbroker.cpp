#include "rtbroker.hpp"
#include "rtsession.hpp"

#define RTBROKER "RTBROKER"
namespace BrkLib {

	/**
	terminator and creator functions
	*/
	BrokerBase *creator (Inqueue::AlgorithmHolder* algholder)
	{
		return new RtBroker(*algholder);
	}

	void terminator (BrokerBase *pthis)
	{
		assert(pthis);
		delete pthis;
	}

	BrokerBase::CreatorBrokerFun createBroker = creator;

	BrokerBase::DestroyerBrokerFun destroyBroker = terminator;

	// -----------------------------------------

	RtBroker::RtBroker( Inqueue::AlgorithmHolder& algHolder):
	BrokerBase(algHolder),
	engine_m(*this)
	{
		
	}

	RtBroker::~RtBroker()
	{
	}

	void RtBroker::onLibraryInitialized(HlpStruct::AlgBrokerDescriptor const& descriptor)
	{
		BRK_LOG(getName().c_str(), BRK_MSG_INFO, RTBROKER, "Пилять, сколько можно программить, стартовали" );
	}

	void RtBroker::onLibraryFreed()
	{
		BRK_LOG(getName().c_str(), BRK_MSG_INFO, RTBROKER, "Остановились!" );
	}

	CppUtils::String const& RtBroker::getName() const
	{
		static CppUtils::String sname("Backtest Broker Library");
		return sname;
	}

	// -------------------------------------

	double RtBroker::getBrokerSystemTime()
	{
		return CppUtils::getTime();
	}

	// create and destroy session objects
	BrokerSession& RtBroker::createSessionObject(CppUtils::String const& connectstring, CppUtils::String const& runName, CppUtils::String const& comment)
	{
		LOCK_FOR_SCOPE(sessionsMtx_m);

		map<CppUtils::String, RtSession*>::iterator it = sessions_m.find(connectstring);

		if (it==sessions_m.end()) {
			RtSession* session = new RtSession(connectstring, *this);
			sessions_m[ connectstring ] = session;
			sessions2_m[session->getUid()] = session;

			// notify
			sendSessionCreateEvent(*session, runName, comment);
			return *session;
		}
		else
			return *it->second;
	}

	BrokerSession& RtBroker::resolveSessionObject(CppUtils::Uid const& uid)
	{
		LOCK_FOR_SCOPE(sessionsMtx_m);

		map<CppUtils::Uid, RtSession*>::iterator it = sessions2_m.find(uid);

		if (it==sessions2_m.end())
			THROW(CppUtils::OperationFailed, "exc_NotFound", "ctx_resolveSessionObject", uid.toString());

		return *it->second;
	}

	void RtBroker::getAvailableSessions(CppUtils::UidList& activeSesionsUids) const
	{
		LOCK_FOR_SCOPE(sessionsMtx_m);
		activeSesionsUids.clear();

		for(map<CppUtils::Uid, RtSession*>::const_iterator it = sessions2_m.begin(); it != sessions2_m.end(); it++) {
			activeSesionsUids.push_back(it->first);
		}

	}

	void RtBroker::deleteSessionObject(BrokerSession& session, CppUtils::String const& runName)
	{
		LOCK_FOR_SCOPE(sessionsMtx_m);

		RtSession* rtSession = (RtSession*) &session;

		if (rtSession) {
			// remove from the list if exist


			int cnt = sessions_m.erase(rtSession->getConnection());
			if (cnt > 0) {

				sessions2_m.erase(rtSession->getUid());

				// notify
				sendSessionDestroyEvent(*rtSession, runName);

				delete rtSession;
			}
		}
	}


	void RtBroker::connectToBroker(BrokerSession& session, CppUtils::String const& runName)
	{
		RtSession& rtSession = (RtSession&)session;
		rtSession.connect();

		// do not send connect event - to save time and resources
		sendSessionConnectEvent(rtSession, runName);
	}


	void RtBroker::disconnectFromBroker(BrokerSession& session, CppUtils::String const& runName)
	{
		RtSession& rtSession = (RtSession&)session;
		rtSession.disconnect();

		// do not send disconnect event - to save time and resources
		sendSessionDisconnectEvent(rtSession, runName);
	}


	bool RtBroker::unblockingIssueOrder(BrkLib::BrokerSession &session, BrkLib::Order const& order)
	{
		if (session.isConnected()) {
			
			RtSession& rtSession = (RtSession&)session;

			issueOrder( session, order );
				
			
			return true;
		}

		// failure
		return false;
	}

	void RtBroker::getBrokerResponseList(BrkLib::BrokerSession &session) const
	{
		// no-op
		THROW(CppUtils::OperationFailed, "exc_NotSupported", "ctx_GetBrokerResponseList", "");
	}

	void RtBroker::getBrokerResponseList(BrkLib::BrokerSession &session, BrkLib::OperationResult const or) const
	{
		// no-op
		THROW(CppUtils::OperationFailed, "exc_NotSupported", "ctx_GetBrokerResponseList", "");
	}

	void RtBroker::getBrokerPositionList(int const posStateQuery, BrkLib::BrokerSession &session, PositionList &posList)
	{
		posList.clear();

		if (posStateQuery & QUERY_OPEN)
			engine_m.getPositionHolder().queryOpenPositions(posList);
		
		if (posStateQuery & QUERY_HISTORY)
			engine_m.getPositionHolder().queryHistoryPositions(posList);

	}


	void RtBroker::onDataArrived(	HlpStruct::RtData const& rtdata )
	{
		engine_m.onNewPricedataArrived(	rtdata.getProvider(), rtdata.getSymbol(), rtdata.getRtData().bid_m, rtdata.getRtData().ask_m, rtdata.getTime() );
	}

	void RtBroker::initializeBacktest(
		BrkLib::SimulationProfileData const& simulProfileData, 
		BrokerBase::ProviderSymbolCookieMap const& providerSymbolCookieMap 
		)
	{
	
		engine_m.initialize( simulProfileData, getMode() & BM_ONE_POSITION_PER_SYMBOL, !(getMode() & BM_USE_CLOSE_POS_COMMANDS) );
	}

	void RtBroker::initializeBacktest(BrkLib::SimulationProfileData const& simulProfileData )
	{

		engine_m.initialize( simulProfileData, getMode() & BM_ONE_POSITION_PER_SYMBOL, !(getMode() & BM_USE_CLOSE_POS_COMMANDS) );
	}

	bool RtBroker::isBacktestLibrary() const
	{
		return true;
	}

	bool RtBroker::getTickInfo(
		BrkLib::BrokerSession &session, 
		CppUtils::String const& symbolNum, 
		CppUtils::String const& symbolDenom
		)
	{
		// No-op
		return false;
	}


	// subscribe for nes retreival
	bool RtBroker::subscribeForNews(
		BrkLib::BrokerSession &session
		)
	{
		// No-op
		return false;
	}

	// returns last error information
	bool RtBroker::getLastErrorInfo(
		BrkLib::BrokerSession &session
		)
	{
		// No-op
		return false;
	}

	// subscribe for quotes - they will arrive via onBrokerResponseArrived
	bool RtBroker::subscribeForQuotes(
		BrkLib::BrokerSession &session,
		CppUtils::String const& symbolNum, 
		CppUtils::String const& symbolDenom
		)
	{
		// No-op
		return false;
	}

	// returns the history from the server
	bool RtBroker::getHistoryInfo(
		BrkLib::BrokerSession &session,
		CppUtils::String const& symbolNum, 
		CppUtils::String const& symbolDenom,
		HlpStruct::AggregationPeriods aggrPeriod,
		int const multfactor
		)
	{
		// No-op
		return false;
	}

	

	/**
	* Helpers
	*/



	void RtBroker::prepareResponseHeader(Order const& order, BrokerResponseBase& result)
	{
		result.ID_m.generate();
		result.parentID_m = order.ID_m;
		result.context_m = order.context_m;
		
	}

	

	void RtBroker::processHandshake(Order const& order, RtSession& session, BrokerResponseBase& result)
	{
		prepareResponseHeader(order, result);

		result.resultCode_m = PT_HANDSHAKEARRIVED;
		result.brokerIssueTime_m = CppUtils::getTime();
		result.resultDetailCode_m = ODR_OK;
		result.brokerComment_m = "Handshake of backtest library: [ "+order.comment_m + " ]";
	}

	void RtBroker::getSymbolMetainfo(Order const& order, RtSession& session, BrokerResponseMetaInfo& brespMinfo)
	{
		prepareResponseHeader(order, brespMinfo);
		brespMinfo.symbol_m = order.symbol_m;
		brespMinfo.resultCode_m = PT_GENERAL_INFO;

		//
		bool isExist= engine_m.existFastTradingInfo(order.provider_m, order.symbol_m );

		if (!isExist) {
			brespMinfo.resultDetailCode_m = ODR_GENERAL_ERROR;
			return;
		}

		SymbolTradingInfo const& sinfo = engine_m.resolveFastTradingInfo(order.provider_m, order.symbol_m);

		brespMinfo.resultDetailCode_m = ODR_OK;

		brespMinfo.spread_m = sinfo.spread_m;
		brespMinfo.signDigits_m = sinfo.signDigits_m;
		brespMinfo.pointValue_m = sinfo.pointValue_m;
	}


	
	void RtBroker::sendErrorResponse(Order const& order, RtSession& session, BrokerResponseOrder& result, int const errcode, CppUtils::String const& reason)
	{
		//LOG(MSG_WARN, RTBROKER, "Returning error: " << reason << ", code: " << errcode );
		prepareResponseHeader(order, result);
		

		result.brokerOrderID_m = order.brokerOrderID_m;
		result.symbol_m = order.symbol_m;
	
		result.provider_m = order.provider_m;
		result.opOrderType_m = order.orderType_m;

		try {
			result.opTime_m = engine_m.resolveFastPriceData(order.provider_m, order.symbol_m).time_m;
		}
		catch(CppUtils::Exception& e)
		{
			// time is not available
			result.opTime_m = -1;
		}

		result.brokerIssueTime_m = CppUtils::getTime();
		result.opVolume_m = order.orVolume_m;
		result.opPrice_m = order.orPrice_m;
		result.opStopPrice_m = order.orStopPrice_m;
		result.opRemainingVolume_m = 0;
		result.context_m = order.context_m;

		result.brokerInternalCode_m = errcode;
		result.resultCode_m = PT_GENERAL_ERROR_HAPPENED;
		result.resultDetailCode_m = ODR_GENERAL_ERROR;
		result.brokerComment_m = reason;
	}
	

	

	
	void RtBroker::issueOrder(BrkLib::BrokerSession &session, Order const& order)
	{
		BrkLib::RtSession &rtsession = (BrkLib::RtSession &)session;

		bool result = true;
		CppUtils::String reason;
		

		try {
			
			if (!order.ID_m.isValid()) {
				THROW_BRK(getName().c_str(), "exc_InvalidOrderID", "ctx_issueOrder", "");
			}

			//
			if (order.orderType_m & BrkLib::OP_HANDSHAKE) {
				// handshake
				BrokerResponseOrder brokerResponse;

				processHandshake(order, rtsession, brokerResponse);
				onBrokerResponseArrived(rtsession, brokerResponse);
			}
			else if (order.orderType_m & BrkLib::OP_GET_SYMBOL_METAINFO) {

				BrokerResponseMetaInfo brespMinfo;

				getSymbolMetainfo(order, rtsession, brespMinfo);
				onBrokerResponseArrived(rtsession, brespMinfo);
			}
			else {
				engine_m.onNewOrderArrived( order, rtsession );
			}
		}
		catch(CppUtils::Exception& e)
		{
			result = false;
			reason.append("Exception occured: ");
		
			reason += EXC2STR(e);
		}
		catch(...)
		{
			result = false;
			reason.append("Unknown exception occured");
		}

		if (!result) {
			// TODO
			BrokerResponseOrder brokerResponse;
			BRK_LOG(getName().c_str(), BRK_MSG_ERROR, RTBROKER, "Error occured: " << reason);
			
			sendErrorResponse(order,rtsession, brokerResponse, -1, reason );
			onBrokerResponseArrived(rtsession, brokerResponse);
		}

	}

	//


	void RtBroker::sendResponseClosePos(
		BrkLib::RtSession & rtsession,
		double const& ttime,
		CppUtils::String const& symbol,
		CppUtils::String const& provider,
		TradeDirection const td,
		double const& openPrice,
		double const& closePrice,
		int const volume,
		CppUtils::String const& brokerPositionId,
		double const& posOpenTime,
		double const& posCloseTime,
		SymbolTradingInfo const& tinfo
	)
	{
		BrokerResponseClosePos brokerClosePosresp;
		brokerClosePosresp.ID_m.generate();
		brokerClosePosresp.brokerIssueTime_m = CppUtils::getTime();
		brokerClosePosresp.symbol_m = symbol;
		brokerClosePosresp.provider_m = provider;
		brokerClosePosresp.tradeDirection_m = td;
		brokerClosePosresp.volume_m = volume;
		brokerClosePosresp.brokerPositionID_m = brokerPositionId;
		brokerClosePosresp.priceOpen_m = openPrice;
		brokerClosePosresp.priceClose_m = closePrice;
		brokerClosePosresp.posCloseTime_m = posCloseTime;
		brokerClosePosresp.posOpenTime_m = posOpenTime;

		// todo  - must provide relative
		brokerClosePosresp.comissionAbs_m	= tinfo.comission_m;
		brokerClosePosresp.comissionPct_m = tinfo.comissionPct_m;

		onBrokerResponseArrived(rtsession, brokerClosePosresp);
	}


	void RtBroker::sendResponseOnPendingOperation(
			BrkLib::RtSession & rtsession,
			CppUtils::String const& brokerPositionId,
			double const& price,
			double const& stopprice,
			BrkLib::OrderType const orderType,
			double const& ttime,
			int const volume,
			OperationResult const resultCode_m,
			int const resultDetailCode,
			CppUtils::String const& symbol,
			CppUtils::String const& provider,
			Order const& order,
			Order const* causeOrder,
			char const * brokerComment 
	)
	{
		
		BrokerResponseOrder brokerResponse;
		brokerResponse.ID_m.generate();
		brokerResponse.brokerPositionID_m = brokerPositionId;
		brokerResponse.opPrice_m = price;
		brokerResponse.opStopPrice_m = stopprice;
		brokerResponse.opOrderType_m = orderType;
		brokerResponse.opVolume_m = volume;
		brokerResponse.brokerIssueTime_m = CppUtils::getTime();
		brokerResponse.opTime_m = ttime;

		brokerResponse.resultCode_m = resultCode_m;
		brokerResponse.resultDetailCode_m = resultDetailCode;
		
		brokerResponse.symbol_m = symbol;
		brokerResponse.provider_m = provider;

	
		brokerResponse.parentID_m = order.ID_m;
		brokerResponse.brokerOrderID_m = order.ID_m.toString();
		brokerResponse.context_m = order.context_m;
		if (order.context_m.getContext() == NULL) {
			LOG(MSG_WARN, "sendResponseOnPendingOperation", "NULL context");
			BRK_LOG(getName().c_str(), BRK_MSG_WARN, RTBROKER, "NULL context" );
			
		}
		if (brokerComment != NULL) {
			brokerResponse.brokerComment_m.append("[").append(brokerComment).append("]");
		}

		if (causeOrder)
			brokerResponse.causeContext_m = causeOrder->context_m;
		
		onBrokerResponseArrived(rtsession, brokerResponse);
		
	}



}; // end of namespace