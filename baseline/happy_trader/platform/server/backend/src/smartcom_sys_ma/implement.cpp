#include "implement.hpp"
// Many thanks to Zoubok Victor for programming this

namespace AlgLib {


	map<CppUtils::String, TradeContext> SCSysExample::userContextMap_m;
	CppUtils::Mutex SCSysExample::userContextMapMtx_m;


	AlgorithmBase *creator (Inqueue::AlgorithmHolder* algholder)
	{
		return new SCSysExample(*algholder);
	}

	void terminator (AlgorithmBase *pthis)
	{
		assert(pthis);
		delete pthis;
	}

	AlgorithmBase::CreatorAlgorithmFun createAlgorithm = creator;

	AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm = terminator;

	// intreface to be implemented
	SCSysExample::SCSysExample( Inqueue::AlgorithmHolder& algHolder):
	AlgorithmBase(algHolder),
	brokerConnect_m(NULL)
	{

	}

	SCSysExample::~SCSysExample()
	{

	}

	void SCSysExample::onLibraryInitialized(
		HlpStruct::AlgBrokerDescriptor const& descriptor, 
		BrkLib::BrokerBase* brokerlib, 
		CppUtils::String const& runName,
		CppUtils::String const& comment
		)
	{


		brokerConnect_m = NULL;

		// read params
		historyDataProfile_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "HISTORY_DATA_PROFILE");

		//rtDataProvider_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "RT_PROVIDER");

		//barDataProvider_m = GET_ALG_TRADE_STRING(descriptor.initialAlgBrkParams_m, "RT_BAR_PROVIDER");


		brokerConnect_m = new BrkLib::BrokerConnect(brokerlib, SMARTCOM_SYS_MA, false, false, runName.c_str(), comment.c_str());
		brokerConnect_m->connect();

		if (!brokerConnect_m->getSession().isConnected()) {
			LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  SMARTCOM_SYS_MA, "onLibraryInitialized(...) session cannot be established: " << brokerConnect_m->getSession().getConnection());
		}

	}

	void SCSysExample::onLibraryFreed()
	{
		 if (brokerConnect_m) {
			delete brokerConnect_m;
			brokerConnect_m = NULL;
		}
	}

	void SCSysExample::onBrokerResponseArrived(
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
		)
	{
		if (bresponse.type_m == BrkLib::RT_BrokerResponseOrder) {
			BrkLib::BrokerResponseOrder const& bRespOrder = (BrkLib::BrokerResponseOrder const&)bresponse;
	

			BrkLib::TradeDirection direction = BrkLib::orderTypeToDirection(bRespOrder.opOrderType_m);

			if (bRespOrder.resultDetailCode_m & BrkLib::ODR_ORDER_INSTALLED) {
				positions_m[bRespOrder.symbol_m] = "";
			}
			else if (bRespOrder.resultDetailCode_m & BrkLib::ODR_ORDER_EXECUTED) {
				positions_m[bRespOrder.symbol_m] = bRespOrder.brokerPositionID_m;
			}

		};

		 
	}

	void SCSysExample::onDataArrived(
		HlpStruct::RtData const& rtdata, 
		Inqueue::ContextAccessor& historyaccessor, 
		HlpStruct::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
		)
	{

		BrkLib::BrokerConnect brkConnect(brokerlib, "dummy", false, false, runName.c_str(), comment.c_str());
		brkConnect.connect();
		



		// just calculate 2 MAS
		//if (rtdata.getProvider2() == barDataProvider_m) {

		CppUtils::String const& sessionId = GET_ALG_TRADE_STRING(descriptor, "_SESSION_ID");

		if (sessionId.size() <=0) {
			LOGEVENT(HlpStruct::CommonLog::LL_WARN,  SMARTCOM_SYS_MA, "Invalid session ID");
			return;
		}

		TradeContext ctx;
		{
			LOCK_FOR_SCOPE(userContextMapMtx_m);

			ctx = userContextMap_m[sessionId];
			initThreadParameters(	ctx, descriptor, runName );

		}

		if (ctx.userId_m <=0)	{
			LOG_COMMON(runName,  CppUtils::getTime(), BrkLib::AP_CRITICAL,"Unrecognized user for thread: " << historyaccessor.getThreadId());
			return;
		}

		// find our symbol - i.e. provider + symbol
		CppUtils::StringSet::const_iterator it = ctx.symbols_m.find(rtdata.getProvider2() + ":" + rtdata.getSymbol2());
		if (it != ctx.symbols_m.end()) {

			int cache_symbol_id = historyaccessor.cacheSymbol(rtdata.getProvider2(), rtdata.getSymbol2(), HlpStruct::AP_Minute,  ctx.tradePeriodMinutes_m);

			// need to recalculate max & mins
			if (cache_symbol_id <= 0) {

				LOG_COMMON(runName,  CppUtils::getTime(), BrkLib::AP_CRITICAL, "Cannot cache symbol: " << rtdata.getProvider2() << " - " << rtdata.getSymbol2() << " for thread: " << historyaccessor.getThreadId());
				return;
			}

			// get last unclosed symbol
			HlpStruct::PriceData const& unclosed = historyaccessor.getUnclosedCandle(cache_symbol_id);


			if (unclosed.isNewPeriod()) {

				// 
				// -------------------------------------
				// last
				Inqueue::DataPtr ptr= historyaccessor.dataLast( cache_symbol_id );
				historyaccessor.dataPrev(cache_symbol_id,ptr );


				if (!ptr.isValid())
					return;

				// last candle
				HlpStruct::PriceData const& pdata = historyaccessor.getCurrentPriceData(cache_symbol_id, ptr);


				// here we perform calculations
				ctx.prevMa1_m = ctx.curMa1_m;
				ctx.prevMa2_m = ctx.curMa2_m;

				Math::MovAverMathEngine<Math::PriceSeriesProvider> maeng1(ctx.ma1_m);
				ctx.curMa1_m =	historyaccessor.callMathEngine<double, Math::P_CloseBid, Math::MovAverMathEngine<Math::PriceSeriesProvider> >(cache_symbol_id, maeng1);

				Math::MovAverMathEngine<Math::PriceSeriesProvider> maeng2(ctx.ma2_m);
				ctx.curMa2_m =	historyaccessor.callMathEngine<double, Math::P_CloseBid, Math::MovAverMathEngine<Math::PriceSeriesProvider> >(cache_symbol_id, maeng2);

				

				int flag = 0;
				// detect
				if ((ctx.prevMa1_m > ctx.prevMa2_m ) && (ctx.curMa1_m <= ctx.curMa2_m)) {
					LOG_SYMBOL(runName, rtdata.getTime(), rtdata.getProvider2(), rtdata.getSymbol2(), "MA 2 more MA 1 intersect");
					flag = -1;

					
				}
				else if ((ctx.prevMa1_m < ctx.prevMa2_m ) && (ctx.curMa1_m >= ctx.curMa2_m)) {
					LOG_SYMBOL(runName, rtdata.getTime(), rtdata.getProvider2(), rtdata.getSymbol2(), "MA 1 more MA 2 intersect");
					flag = 1;

				
				}

				// notify cur MA
				DRAWOBJ_LINEINDIC_3(runName,rtdata.getTime(), SMARTCOM_SYS_MA, rtdata.getSymbol2(), rtdata.getProvider2(), 
					"MA1", ctx.curMa1_m, -1, "MA2", ctx.curMa2_m, -1, "FLAG", flag, -1);


		 }
		}
		//}

		// store context
		{
			LOCK_FOR_SCOPE(userContextMapMtx_m);
			userContextMap_m[sessionId] = ctx;
		}

	}

	void SCSysExample::onRtProviderSynchEvent (
		Inqueue::ContextAccessor& contextaccessor, HlpStruct::RtData::SynchType const synchFlg,
		double const& synchTime,
		CppUtils::String const& providerName,
		CppUtils::String const& message
		)
	{
		// RT provider events - must check for sisconnections
		if (synchFlg==HlpStruct::RtData::SYNC_EVENT_PROVIDER_START) {

			LOGEVENT(HlpStruct::CommonLog::LL_INFO,  SMARTCOM_SYS_MA, "SYNC_EVENT_PROVIDER_START");
		}
		else if (synchFlg==HlpStruct::RtData::SYNC_EVENT_PROVIDER_FINISH) {

			LOGEVENT(HlpStruct::CommonLog::LL_INFO,  SMARTCOM_SYS_MA, "SYNC_EVENT_PROVIDER_FINISH");

		}
		else if (synchFlg==HlpStruct::RtData::SYNC_EVENT_PROVIDER_TRANSMIT_ERROR) {
			LOGEVENT(HlpStruct::CommonLog::LL_INFO,  SMARTCOM_SYS_MA, "SYNC_EVENT_PROVIDER_TRANSMIT_ERROR");



		}
		else if (synchFlg==HlpStruct::RtData::SYNC_EVENT_CUSTOM_MESSAGE) {

			LOGEVENT(HlpStruct::CommonLog::LL_INFO,  SMARTCOM_SYS_MA, "SYNC_EVENT_CUSTOM_MESSAGE");
		}
	}

	void SCSysExample::onLevel2DataArrived(
		HlpStruct::RtLevel2Data const& level2data,
		Inqueue::ContextAccessor& historyaccessor, 
		HlpStruct::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
		)
	{

	}

	void SCSysExample::onLevel1DataArrived(
		HlpStruct::RtLevel1Data const& level1data,
		Inqueue::ContextAccessor& historyaccessor, 
		HlpStruct::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
		)
	{

	}

	void SCSysExample::onEventArrived(
		Inqueue::ContextAccessor& contextaccessor, 
		HlpStruct::CallingContext& callContext,
		bool& result,
		CppUtils::String& customOutputData
		)
	{
		customOutputData = "FAILED";
		CppUtils::String const& command = callContext.getCallCustomName();

		CppUtils::String const& data = callContext.getParameter<BrkLib::String_Proxy>("custom_data").get();

		// parse as simple structure
		HlpStruct::XmlParameter xmlparam_data;
		HlpStruct::HumanSerializer::deserializeXmlParameter(data, xmlparam_data);

		if (command=="hallo") {
			// n/a
			LOGEVENT(HlpStruct::CommonLog::LL_INFO,  SMARTCOM_SYS_MA, "Hallo event received");
		}
		else if (command=="load_data") {
			bool loaded = contextaccessor.loadDataFromMaster(historyDataProfile_m, false);

			if (!loaded)
				THROW(CppUtils::OperationFailed, "exc_CannotLoadHistoryData", "ctx_onEventArrived", "");


			LOGEVENT(HlpStruct::CommonLog::LL_INFO,  SMARTCOM_SYS_MA, "Loaded history OK");
			//LOG_COMMON(connectRunName_m,rtDataProvider_m, BrkLib::AP_MEDIUM, "Loaded history OK, working with minutes period: " << tradePeriodMinutes_m);
		}
		else if (command=="deinit_data") {


		}
		else if (command=="init_data") {

		}
		else if (command=="com_example") {
			LOG(MSG_INFO, SMARTCOM_SYS_MA, "Started command for thread: " << contextaccessor.getThreadId() );
			CppUtils::sleep(15);
			LOG(MSG_INFO, SMARTCOM_SYS_MA, "Stopped command for thread:" <<  contextaccessor.getThreadId() );
			
			//LOGEVENT(HlpStruct::CommonLog::LL_INFO,  SMARTCOM_SYS_MA, "Command received: " + command);
		}
		else
			THROW(CppUtils::OperationFailed, "exc_InvalidCommand", "ctx_onEventArrived", command);

		// no exceptions
		customOutputData ="OK: " + CppUtils::long2String(contextaccessor.getThreadId()) ;

	}

	void SCSysExample::onThreadStarted(
		Inqueue::ContextAccessor& contextaccessor, bool const firstLib, bool const lastLib
		)
	{

	}

	void SCSysExample::onThreadFinished(
		Inqueue::ContextAccessor& contextaccessor, HlpStruct::TradingParameters const& tradeParams, bool const firstLib, bool const lastLib
		)
	{
		CppUtils::String const& sessionId = GET_ALG_TRADE_STRING_NR(tradeParams, "_SESSION_ID");
		if (sessionId.size() > 0) {
			LOCK_FOR_SCOPE(userContextMapMtx_m);
			userContextMap_m.erase(sessionId);

			LOGEVENT(HlpStruct::CommonLog::LL_INFO,  SMARTCOM_SYS_MA, "Removed context for session: " << sessionId);
		}
	}

	CppUtils::String const& SCSysExample::getName() const
	{
		static const CppUtils::String name("[ alg lib: SCSysExample ]");
		return name;
	}

	/**
	*  HELPERS
	*/

	void SCSysExample::initThreadParameters(TradeContext& ctx, HlpStruct::TradingParameters const& descriptor, CppUtils::String const& runName)
	{
		// alg/brk parameters are to be loaded to a separate thread


		if (ctx.tradePeriodMinutes_m < 0) {
			

			ctx.sessionId_m = GET_ALG_TRADE_STRING(descriptor, "_SESSION_ID");
			ctx.tradePeriodMinutes_m = GET_ALG_TRADE_INT(descriptor, "TRADE_PERIOD_MINUTES");

			// symbols as subset of allowed
			CppUtils::String const& trade_symbols = GET_ALG_TRADE_STRING(descriptor, "TRADE_SYMBOLS");
			CppUtils::StringList slist;
			CppUtils::parseCommaSeparated(',',trade_symbols, slist);
			CppUtils::merge(ctx.symbols_m, slist );


			// send available symbols
			for(CppUtils::StringSet::const_iterator it = ctx.symbols_m.begin(); it != ctx.symbols_m.end(); it++) {
				LOG_COMMON(runName,  CppUtils::getTime(), BrkLib::AP_MEDIUM,	  "Symbol passed: " << *it);
			}



			ctx.ma1_m = GET_ALG_TRADE_INT(descriptor, "MA1");
			ctx.ma2_m = GET_ALG_TRADE_INT(descriptor, "MA2");
			ctx.userId_m = GET_ALG_TRADE_INT(descriptor, "_USER_ID_");
			ctx.userName_m = GET_ALG_TRADE_STRING(descriptor, "_USER_NAME_");
			

			LOG_COMMON(runName,  CppUtils::getTime(), BrkLib::AP_MEDIUM,	  "MA1: " << ctx.ma1_m << " MA2: " << ctx.ma2_m << " User ID: " << ctx.userId_m
				<< " User Name: " << ctx.userName_m << " session ID: " << ctx.sessionId_m);

			LOGEVENT(HlpStruct::CommonLog::LL_INFO,  SMARTCOM_SYS_MA, "Registered context for session: " << ctx.sessionId_m);
		}


	}

	void SCSysExample::openLong(CppUtils::String const& provider,CppUtils::String const& symbol)
	{
		BrkLib::Order order;

		order.ID_m.generate();
		order.provider_m = provider;
		order.symbol_m = symbol;
		order.orderType_m = BrkLib::OP_BUY;
		order.orderValidity_m = BrkLib::OV_DAY;
		order.orVolume_m = 1;

		
		BrkLib::BrokerOperation oper(*brokerConnect_m);
		oper.unblockingIssueOrder(order);
	}

	void SCSysExample::openShort(CppUtils::String const& provider,CppUtils::String const& symbol)
	{
		BrkLib::Order order;

		order.ID_m.generate();
		order.provider_m = provider;
		order.symbol_m = symbol;
		order.orderType_m = BrkLib::OP_SELL;
		order.orderValidity_m = BrkLib::OV_DAY;
		order.orVolume_m = 1;

		
		BrkLib::BrokerOperation oper(*brokerConnect_m);
		oper.unblockingIssueOrder(order);
	}

	void SCSysExample::closeLong(CppUtils::String const& provider,CppUtils::String const& symbol, CppUtils::String const& brokerPositionId)
	{
		BrkLib::Order order;

		order.ID_m.generate();
		order.provider_m = provider;
		order.symbol_m = symbol;
		order.orderType_m = BrkLib::OP_CLOSE_LONG;
		order.orderValidity_m = BrkLib::OV_DAY;
		order.orVolume_m = 1;

		
		BrkLib::BrokerOperation oper(*brokerConnect_m);
		oper.unblockingIssueOrder(order);
	}

	void SCSysExample::closeShort(CppUtils::String const& provider,CppUtils::String const& symbol, CppUtils::String const& brokerPositionId)
	{
		BrkLib::Order order;

		order.ID_m.generate();
		order.provider_m = provider;
		order.symbol_m = symbol;
		order.orderType_m = BrkLib::OP_CLOSE_SHORT;
		order.orderValidity_m = BrkLib::OV_DAY;
		order.orVolume_m = 1;

		
		BrkLib::BrokerOperation oper(*brokerConnect_m);
		oper.unblockingIssueOrder(order);
	}

} // end of namespace

