
#include "algorithmdummy.hpp"
#include "../inqueue/commontypes.hpp"
#include "../math/math.hpp"

//#include <sstream>

#define ALG_DUMMY "ALG_DUMMY"



namespace AlgLib {

	/**
	* Global trading parameters
	*/
	



	/**
	*/
	
	static int id_m = 0;
	
	AlgorithmBase *creator (Inqueue::AlgorithmHolder* algholder)
	{
		return new AlgorithmDummy(*algholder);
	}

	void terminator (AlgorithmBase *pthis)
	{
		HTASSERT(pthis);
		delete pthis;
	}

	AlgorithmBase::CreatorAlgorithmFun createAlgorithm = creator;

	AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm = terminator;


	// intreface to be implemented

	AlgorithmDummy::AlgorithmDummy( Inqueue::AlgorithmHolder& algHolder):
		AlgorithmBase(algHolder)
	{
	 brokerConnect_m = NULL;
	}

	AlgorithmDummy::~AlgorithmDummy()
	{
	}

	void AlgorithmDummy::onLibraryInitialized(
		HlpStruct::AlgBrokerDescriptor const& descriptor, 
		BrkLib::BrokerBase* brokerlib, 
		BrkLib::BrokerBase* brokerlib2,
		CppUtils::String const& runName,
		CppUtils::String const& comment
	)
	{
		idx_m = 0;
		isLong_m = false;

		brokerConnect_m = new BrkLib::BrokerConnect(brokerlib, ALG_DUMMY, false, false, runName.c_str(), comment.c_str());
		brokerConnect_m->connect();

		if (!brokerConnect_m->getSession().isConnected()) {
			LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  ALG_DUMMY, "onLibraryInitialized(...) session cannot be established: " << brokerConnect_m->getSession().getConnection());
		}

	}

	void AlgorithmDummy::onLibraryFreed()
	{
		if (brokerConnect_m) {
			delete brokerConnect_m;
			brokerConnect_m = NULL;
		}
	}

	void AlgorithmDummy::onBrokerResponseArrived(
			BrkLib::BrokerConnect &brkConnect,
			BrkLib::BrokerResponseBase const& bresponse
	)
	{
		if (bresponse.type_m == BrkLib::RT_BrokerResponseClosePos) {
			BrkLib::BrokerResponseClosePos const& bRespOrderClosePos = (BrkLib::BrokerResponseClosePos const&)bresponse;
			//LOG(MSG_INFO, ALG_DUMMY, "Close pos notifucation: " << bRespOrderClosePos.toString());
		}
		else if (bresponse.type_m == BrkLib::RT_BrokerResponseOrder) {
			BrkLib::BrokerResponseOrder const& bRespOrder = (BrkLib::BrokerResponseOrder const&)bresponse;
			

			BrkLib::TradeDirection direction = BrkLib::orderTypeToDirection(bRespOrder.opOrderType_m);

			if (bRespOrder.resultDetailCode_m & BrkLib::ODR_ORDER_INSTALLED) {
				

				if (bRespOrder.opOrderType_m & BrkLib::OP_BUY) {
				}
				else if (bRespOrder.opOrderType_m & BrkLib::OP_SELL) {
				}
				else if (bRespOrder.opOrderType_m & BrkLib::OP_CLOSE_LONG) {
					
				}
				else if (bRespOrder.opOrderType_m & BrkLib::OP_CLOSE_SHORT) {
					
				}
			}
			else if (bRespOrder.resultDetailCode_m & BrkLib::ODR_ORDER_EXECUTED) {
				if (bRespOrder.opOrderType_m & BrkLib::OP_BUY) {
					longPositionId_m = bRespOrder.brokerPositionID_m;
				}
				else if (bRespOrder.opOrderType_m & BrkLib::OP_SELL) {
					shortPositionId_m = bRespOrder.brokerPositionID_m;
				}
				else if (bRespOrder.opOrderType_m & BrkLib::OP_CLOSE_LONG) {
					longPositionId_m = "";
				}
				else if (bRespOrder.opOrderType_m & BrkLib::OP_CLOSE_SHORT) {
					shortPositionId_m = "";
				}

				
			}

		};
	}

	
	void AlgorithmDummy::onDataArrived(
		HlpStruct::RtData const& rtdata, 
		Inqueue::ContextAccessor& historyaccessor, 
		HlpStruct::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
	)
	{
		//if (rtdata.getSymbol2() != "EURUSD")
		//	return;
		//BrkLib::BrokerConnect brkConnect(brokerlib, "dummy", false, false, runName.c_str(), comment.c_str());
		//brkConnect.connect();
		
		//if (!brkConnect.getSession().isConnected()) {
		//	LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  "ALG_DUMMY", "onDataArrived(...) session cannot be established: " << brkConnect.getSession().getConnection());
		//	return;
		//}

		  
		int cache_id = historyaccessor.cacheSymbol(rtdata.getProvider2(), rtdata.getSymbol2(), HlpStruct::AP_Minute, 1);
		CppUtils::String dumpstr = historyaccessor.dump(cache_id, true);
		CppUtils::saveContentInFile2("c:\\temp\\dump\\dumped_"+rtdata.getSymbol2(), dumpstr );

		/*
		if (idx_m++ > 3) {

			if (isLong_m) {

				// need to close
				if (longPositionId_m.size() > 0 ) {
					// need to close
					LOGEVENT(HlpStruct::CommonLog::LL_INFO,  ALG_DUMMY, "close long...");
					closeLong(rtdata.getProvider2(), rtdata.getSymbol2(),longPositionId_m ); 
				}

				if (shortPositionId_m.size() > 0 ) {
					// need to close
					LOGEVENT(HlpStruct::CommonLog::LL_INFO,  ALG_DUMMY, "close short...");
					closeShort(rtdata.getProvider2(), rtdata.getSymbol2(),shortPositionId_m ); 
				}


				LOGEVENT(HlpStruct::CommonLog::LL_INFO,  ALG_DUMMY, "Open long...");

				openLong(rtdata.getProvider2(), rtdata.getSymbol2());
				isLong_m = false;
			}
			else {
				// need to close
				if (longPositionId_m.size() > 0 ) {
					// need to close
					LOGEVENT(HlpStruct::CommonLog::LL_INFO,  ALG_DUMMY, "close long...");
					closeLong(rtdata.getProvider2(), rtdata.getSymbol2(),longPositionId_m ); 
				}

				if (shortPositionId_m.size() > 0 ) {
					// need to close
					LOGEVENT(HlpStruct::CommonLog::LL_INFO,  ALG_DUMMY, "close short...");
					closeShort(rtdata.getProvider2(), rtdata.getSymbol2(),shortPositionId_m ); 
				}

				LOGEVENT(HlpStruct::CommonLog::LL_INFO,  ALG_DUMMY, "Open short...");

				openShort(rtdata.getProvider2(), rtdata.getSymbol2());
				isLong_m = true;
			}

			idx_m = 0;
		}
	
		*/
	}    

	void AlgorithmDummy::onRtProviderSynchEvent (
			Inqueue::ContextAccessor& contextaccessor,
			HlpStruct::RtData::SynchType const synchFlg,
			double const& synchTime,
			CppUtils::String const& providerName,
			CppUtils::String const& message
		)
	{
	}


	void AlgorithmDummy::onLevel2DataArrived(
			HlpStruct::RtLevel2Data const& level2data,
			Inqueue::ContextAccessor& historyaccessor, 
			HlpStruct::TradingParameters const& descriptor,
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		)
	{
	}

	void AlgorithmDummy::onLevel1DataArrived(
			HlpStruct::RtLevel1Data const& level1data,
			Inqueue::ContextAccessor& historyaccessor, 
			HlpStruct::TradingParameters const& descriptor,
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		)
	{
		
	
			//BrkLib::BrokerConnect brkConnect(brokerlib, "dummy", false, false, runName.c_str(), comment.c_str());
			//brkConnect.connect();
		
			//if (!brkConnect.getSession().isConnected()) {
			//		LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  "ALG_DUMMY", "onLevel1DataArrived(...) session cannot be established: " << brkConnect.getSession().getConnection());
			//	return;
			//}
	}


	void AlgorithmDummy::onEventArrived(
		Inqueue::ContextAccessor& contextaccessor, 
		HlpStruct::CallingContext& callContext,
		bool& result,
		CppUtils::String &customOutputData
	)
	{
		result = false;
	}
	
	void AlgorithmDummy::onThreadStarted(Inqueue::ContextAccessor& contextaccessor, bool const firstLib, bool const lastLib) {
		//Inqueue::ProviderSymbolList empty;
		//bool result = contextaccessor.loadDataFromMaster(empty, "laser_dath", -1, -1);
		
	};


	
	CppUtils::String const& AlgorithmDummy::getName() const
	{
		static const CppUtils::String name("[ dummy algorithm ]");
		return name;
	}

	/**
	* Helpers
	*/

	void AlgorithmDummy::openLong(CppUtils::String const& provider,CppUtils::String const& symbol)
	{
		BrkLib::Order order;

		//order.ID_m.generate();
		order.provider_m = provider;
		order.symbol_m = symbol;
		order.orderType_m = BrkLib::OP_BUY;
		order.orderValidity_m = BrkLib::OV_DAY;
		order.orVolume_m = 1;

		
		BrkLib::BrokerOperation oper(*brokerConnect_m);
		oper.unblockingIssueOrder(order);
	}

	void AlgorithmDummy::openShort(CppUtils::String const& provider,CppUtils::String const& symbol)
	{
		BrkLib::Order order;

		//order.ID_m.generate();
		order.provider_m = provider;
		order.symbol_m = symbol;
		order.orderType_m = BrkLib::OP_SELL;
		order.orderValidity_m = BrkLib::OV_DAY;
		order.orVolume_m = 1;

		
		BrkLib::BrokerOperation oper(*brokerConnect_m);
		oper.unblockingIssueOrder(order);
	}

	void AlgorithmDummy::closeLong(CppUtils::String const& provider,CppUtils::String const& symbol, CppUtils::String const& brokerPositionId)
	{
		BrkLib::Order order;

		//order.ID_m.generate();
		order.brokerPositionID_m = brokerPositionId; 
		order.provider_m = provider;
		order.symbol_m = symbol;
		order.orderType_m = BrkLib::OP_CLOSE_LONG;
		order.orderValidity_m = BrkLib::OV_DAY;
		order.orVolume_m = 1;

		
		BrkLib::BrokerOperation oper(*brokerConnect_m);
		oper.unblockingIssueOrder(order);
	}

	void AlgorithmDummy::closeShort(CppUtils::String const& provider,CppUtils::String const& symbol, CppUtils::String const& brokerPositionId)
	{
		BrkLib::Order order;

		//order.ID_m.generate();
		order.provider_m = provider;
		order.brokerPositionID_m = brokerPositionId; 
		order.symbol_m = symbol;
		order.orderType_m = BrkLib::OP_CLOSE_SHORT;
		order.orderValidity_m = BrkLib::OV_DAY;
		order.orVolume_m = 1;

		
		BrkLib::BrokerOperation oper(*brokerConnect_m);
		oper.unblockingIssueOrder(order);
	}


} // end of namespace