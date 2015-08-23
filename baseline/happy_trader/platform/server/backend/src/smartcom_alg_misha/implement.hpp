#ifndef _SMARTCOM_ALG_MISHA_IMPL_INCLUDED
#define _SMARTCOM_ALG_MISHA_IMPL_INCLUDED

 // Many thanks to Zoubok Victor

#include "smartcom_alg_mishadefs.hpp"
#include "event.hpp"
#include "state.hpp"

#define SMARTCOM_ALG_MISHA "SMARTCOM_ALG_MISHA"
#define MAIN_FSM "Main FSM"
#define ORDER_FSM "Order FSM"

#define LOG_SYMBOL(RUN_NAME,PROVIDER, SYMBOL,X) 	\
	DRAWOBJ_TEXT(RUN_NAME,CppUtils::getTime(),SMARTCOM_ALG_MISHA, SYMBOL, PROVIDER, BrkLib::AP_MEDIUM, "LOG", X );	\

#define LOG_SYMBOL_SIGNAL(RUN_NAME,PROVIDER, SYMBOL,X) 	\
	DRAWOBJ_TEXT(RUN_NAME,CppUtils::getTime(),SMARTCOM_ALG_MISHA, SYMBOL, PROVIDER, BrkLib::AP_MEDIUM, "SIGNAL", X );	\

#define LOG_SYMBOL_PROBLEM(RUN_NAME,PROVIDER, SYMBOL,X) 	\
	DRAWOBJ_TEXT(RUN_NAME,CppUtils::getTime(),SMARTCOM_ALG_MISHA, SYMBOL, PROVIDER, BrkLib::AP_HIGH, "LOG", X );	\

#define LOG_COMMON(RUN_NAME,PROVIDER,LEVEL,X) 	\
	DRAWOBJ_TEXT(RUN_NAME,CppUtils::getTime(),SMARTCOM_ALG_MISHA, "-ALL-", PROVIDER, LEVEL, "LOG", X );	\


// helper to create state
#define CREATE_STATE(NAME, CLASS, MACHINE)	\
	 CppUtils::BaseState<MachineContext>* NAME = MACHINE.add<CLASS>();

namespace AlgLib {
	//

	#define TAG_CLOSE_TP  1
	#define TAG_CLOSE_SL  2


	class MyBrokerContext: public BrkLib::BrokerContext
	{
	public :
		MyBrokerContext();

		MyBrokerContext(CppUtils::BaseState<MachineContext> const& issueState);
		
		~MyBrokerContext();
		 

		virtual BrkLib::BrokerContext* clone() const;

		virtual void deallocate();

		virtual CppUtils::String toString()	const;

		CppUtils::BaseState<MachineContext> const& getState() const;

	


	private:
		
		CppUtils::BaseState<MachineContext> const* issueState_m;

		
	};
	


	struct TradeParam
	{
		enum TRDADEMODE
		{
			TM_OVERCOME_LEVEL = 1,
			TM_REFLECTION_LEVEL	= 2
		};

		TradeParam():
			tradePeriodMinutes_m(-1),
			cutoffVolume_m(-1),
			maxMinutesLevelTrack_m(-1),
			tradeVolume_m(-1),
			tp_relativeLevel_m(-1),
			sl_relativeLevel_m(-1),
			isDemoTrade_m(true),
			onlyMarketOrders_m(true),
			tradeMode_m(TM_OVERCOME_LEVEL),
			onlyOpenPosOpes_m(false)
		{
		}

		bool const isInvalid() const
		{
			return (
				tradePeriodMinutes_m <=0 || cutoffVolume_m<=0	|| tradeVolume_m <=0 || maxMinutesLevelTrack_m <=0 ||
				maxPriceDifference_m <=0 || tp_relativeLevel_m <=0 || sl_relativeLevel_m <=0
			);
		}

		CppUtils::String toString() const
		{
			CppUtils::String result = 
				"tradePeriodMinutes_m="+CppUtils::long2String(tradePeriodMinutes_m) + "\n" + 
				"maxMinutesLevelTrack_m="+CppUtils::simpleFloat2String(maxMinutesLevelTrack_m) + "\n" + 
				"maxPriceDifference_m="+ CppUtils::simpleFloat2String(maxPriceDifference_m) + "\n" + 
				"tradeVolume_m="+ CppUtils::simpleFloat2String(tradeVolume_m) + "\n" + 
				"onlyMarketOrders_m="+ CppUtils::bool2String(onlyMarketOrders_m) + "\n" + 
				"isDemoTrade_m="+ CppUtils::bool2String(isDemoTrade_m) + "\n" + 
				"tradeMode_m="+ CppUtils::long2String(tradeMode_m) + "\n" + 
				"onlyOpenPosOpes_m="+ CppUtils::bool2String(onlyOpenPosOpes_m) + "\n" + 
				"cutoffVolume_m="+CppUtils::simpleFloat2String(cutoffVolume_m);

			return result;
		}

		HlpStruct::WorkTimeHolder workTimeHolder_m;

		// trade params
		int period_m;

		int tradePeriodMinutes_m;

		double cutoffVolume_m;

		// -1 means infinit
		double maxMinutesLevelTrack_m;

		double maxPriceDifference_m;

		double tradeVolume_m;

		double tp_relativeLevel_m;

		double sl_relativeLevel_m;

		bool isDemoTrade_m;

		// only market orders
		bool onlyMarketOrders_m;

		TRDADEMODE tradeMode_m;

		// use only open pos operations
		bool onlyOpenPosOpes_m;


		

	};

	// --------------

	struct SymbolContext
	{
		SymbolContext():
			curTime_m(-1),
			prevTickPrice_m(-1),
			curTickPrice_m(-1)
		{
		};
		
		double prevTickPrice_m;
		double curTickPrice_m;
		double curTime_m;
		
	};

	// -------------------------


	class SMARTCOM_ALG_MISHA_EXP CAlgMisha :public AlgorithmBase {
		//
	public:
		//
		// ctor & dtor
		CAlgMisha( Inqueue::AlgorithmHolder& algHolder);

		virtual ~CAlgMisha();
		// intreface to be implemented

		virtual void onLibraryInitialized(
			HlpStruct::AlgBrokerDescriptor const& descriptor, 
			BrkLib::BrokerBase* brokerlib, 
			CppUtils::String const& runName,
			CppUtils::String const& comment
		);

		virtual void onLibraryFreed();

		virtual void onDataArrived(
			HlpStruct::RtData const& rtdata, 
			Inqueue::ContextAccessor& historyaccessor, 
			HlpStruct::TradingParameters const& descriptor,
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		);
		
	virtual void onRtProviderSynchEvent (
		Inqueue::ContextAccessor& contextaccessor, HlpStruct::RtData::SynchType const synchFlg,
		double const& synchTime,
		CppUtils::String const& providerName,
		CppUtils::String const& message
	);

		virtual void onLevel2DataArrived(
			HlpStruct::RtLevel2Data const& level2data,
			Inqueue::ContextAccessor& historyaccessor, 
			HlpStruct::TradingParameters const& descriptor,
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		);

		virtual void onLevel1DataArrived(
			HlpStruct::RtLevel1Data const& level1data,
			Inqueue::ContextAccessor& historyaccessor, 
			HlpStruct::TradingParameters const& descriptor,
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		);

		virtual void onBrokerResponseArrived(
			BrkLib::BrokerConnect &brkConnect,
			BrkLib::BrokerResponseBase const& bresponse
		);

		virtual void onEventArrived(Inqueue::ContextAccessor& contextaccessor,
		HlpStruct::CallingContext& callContext,
		bool& result,
		CppUtils::String& customOutputData
	);

		virtual void onThreadStarted(
			Inqueue::ContextAccessor& contextaccessor, bool const firstLib, bool const lastLib
		);

	virtual void onThreadFinished(
		Inqueue::ContextAccessor& contextaccessor, HlpStruct::TradingParameters const& tradeParams, bool const firstLib, bool const lastLib
	);



		virtual CppUtils::String const& getName() const;

		// -----------------------
	public:

		TradeParam const& getTradeParam() const {
			return tradeParam_m;
		}

		inline CppUtils::String const& getRtdataProvider() const
		{
			return rtDataProvider_m;
		}

		inline CppUtils::String const& getRunName() const
		{
			return connectRunName_m;
		}

		double getCurrentPrice(CppUtils::String const& symbol) const;


		
		// market
		void issueBuyOrder(MyBrokerContext const& ctx, CppUtils::String const& symbol,double const& volume);

		void issueSellOrder(MyBrokerContext const& ctx, CppUtils::String const& symbol, double const& volume);

		// limit open
		void issueBuyOrder_Limit(MyBrokerContext const& ctx, double const& limitPrice, CppUtils::String const& symbol,double const& volume);

		void issueSellOrder_Limit(MyBrokerContext const& ctx, double const& limitPrice, CppUtils::String const& symbol, double const& volume);

		// market close
		void issueCloseLongOrder(MyBrokerContext const& ctx, CppUtils::String const& symbol, double const& volume, CppUtils::String const& brokerPositionId);

		void issueCloseShortOrder(MyBrokerContext const& ctx, CppUtils::String const& symbol, double const& volume, CppUtils::String const& brokerPositionId);

		// limit close
		void issueCloseLongOrder_Limit(MyBrokerContext const& ctx, double const& limitPrice, CppUtils::String const& symbol, double const& volume, CppUtils::String const& brokerPositionId);

		void issueCloseShortOrder_Limit(MyBrokerContext const& ctx, double const& limitPrice, CppUtils::String const& symbol, double const& volume, CppUtils::String const& brokerPositionId);
	
		// limit close
		void issueCloseLongOrder_Stop(MyBrokerContext const& ctx, double const& stopPrice, CppUtils::String const& symbol, double const& volume, CppUtils::String const& brokerPositionId);

		void issueCloseShortOrder_Stop(MyBrokerContext const& ctx, double const& stopPrice, CppUtils::String const& symbol, double const& volume, CppUtils::String const& brokerPositionId);
	

		// limit stop close
		void issueCloseLongOrder_StopLimit(MyBrokerContext const& ctx, double const& stopPrice, double const& limitPrice, CppUtils::String const& symbol, double const& volume, CppUtils::String const& brokerPositionId);

		void issueCloseShortOrder_StopLimit(MyBrokerContext const& ctx, double const& stopPrice, double const& limitPrice, CppUtils::String const& symbol, double const& volume, CppUtils::String const& brokerPositionId);
	
		void issueCancelOrder(MyBrokerContext const& ctx, CppUtils::String const& symbol, CppUtils::String const& brokerPositionId, CppUtils::String const& brokerOrderId);
		

		// operate through delayed orders
		// this for closing TP
		void issueCloseLongOrder_Delayed_TP(MyBrokerContext const& ctx, double const& executionPrice, CppUtils::String const& symbol,double const& volume, CppUtils::String const& brokerPositionId);

		void issueCloseShortOrder_Delayed_TP(MyBrokerContext const& ctx, double const& executionPrice, CppUtils::String const& symbol, double const& volume, CppUtils::String const& brokerPositionId);


		// this for closing as SL
		void issueCloseLongOrder_Delayed_SL(MyBrokerContext const& ctx, double const& executionPrice, CppUtils::String const& symbol,double const& volume, CppUtils::String const& brokerPositionId);

		void issueCloseShortOrder_Delayed_SL(MyBrokerContext const& ctx, double const& executionPrice, CppUtils::String const& symbol, double const& volume, CppUtils::String const& brokerPositionId);


		// this checks if orders to be executed
		void checkDelayedOrderExecution(HlpStruct::RtData const& rtdata);

		void cancelAllDelayedOrders();

		// this function implement event processing in a separate thread to avoid recursion
		void helperThreadFun();


		inline CppUtils::FSMMap<	MachineContext >& getTradingMachines() {
			return tradingMachines_m;
		}

		inline CppUtils::FSMMap<	MachineContext >&  getOrderMachines() {
			return ordersMachines_m;
		}

		inline CppUtils::FSMMap<	MachineContext > const& getTradingMachines() const {
			return tradingMachines_m;
		}

		inline CppUtils::FSMMap<	MachineContext > const&  getOrderMachines() const {
			return ordersMachines_m;
		}

	private:

				

		void exportSymbol(Inqueue::ContextAccessor& historyaccessor, CppUtils::String const& symbol, int const cache_id);

		void exportSymbol(Inqueue::ContextAccessor& historyaccessor, CppUtils::String const& symbol);

		void initStateMachine(CppUtils::StringSet const& symbols);

		void deinitStateMachine();

		double calculateTradingVolume(double const& highChannel, double const& lowChannel);


		// helper thread
		// -
		CppUtils::Thread* thread_m;

		// event whether thread started
		CppUtils::Event threadStarted_m;

		// signal to shutdown
		CppUtils::Event shutDownThread_m;
		// --


		
		CppUtils::String historyDataProfile_m;

		CppUtils::String rtDataProvider_m;

		CppUtils::String barDataProvider_m;

		BrkLib::TradeDirection allowTrading_m;
		
		TradeParam tradeParam_m;

		map<CppUtils::String, SymbolContext> contextMap_m;

		CppUtils::Mutex	contextMapMtx_m;

		CppUtils::String connectRunName_m;

		CppUtils::String baseExportPath_m;

		// map containing symbols

		CppUtils::FSMMap<	MachineContext > tradingMachines_m;

		CppUtils::FSMMap<	MachineContext > ordersMachines_m;

	
		BrkLib::BrokerConnect* brokerConnect_m;

		BrkLib::BrokerConnect* brokerConnectDemo_m;

		//BrkLib::BrokerConnect* brokerConnectDummy_m;

		// list of processed symbols
		CppUtils::StringSet symbols_m;

		

	};

}; // end of namespace 


#endif // _SMARTCOM_ALG_MISHA_IMPL_INCLUDED
