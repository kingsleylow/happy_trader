#ifndef _GERM_2CHANNEL_MM_IMPL_INCLUDED
#define _GERM_2CHANNEL_MM_IMPL_INCLUDED

 // Many thanks to Zoubok Victor

#include "germ_2channel_mmdefs.hpp"
#include "event.hpp"
#include "state.hpp"


#define GERM_2CHANNEL_MM "GERMAN 2 CHANNEL MM"

#define LOG_SYMBOL(RUN_NAME,PROVIDER, SYMBOL,X) 	\
	DRAWOBJ_TEXT(RUN_NAME,CppUtils::getTime(),GERM_2CHANNEL_MM, SYMBOL, PROVIDER, BrkLib::AP_MEDIUM, "LOG", X );	\

#define LOG_SYMBOL_SIGNAL(RUN_NAME,PROVIDER, SYMBOL,X) 	\
	DRAWOBJ_TEXT(RUN_NAME,CppUtils::getTime(),GERM_2CHANNEL_MM, SYMBOL, PROVIDER, BrkLib::AP_MEDIUM, "SIGNAL", X );	\

#define LOG_SYMBOL_PROBLEM(RUN_NAME,PROVIDER, SYMBOL,X) 	\
	DRAWOBJ_TEXT(RUN_NAME,CppUtils::getTime(),GERM_2CHANNEL_MM, SYMBOL, PROVIDER, BrkLib::AP_HIGH, "LOG", X );	\

#define LOG_COMMON(RUN_NAME,PROVIDER,LEVEL,X) 	\
	DRAWOBJ_TEXT(RUN_NAME,CppUtils::getTime(),GERM_2CHANNEL_MM, "-ALL-", PROVIDER, LEVEL, "LOG", X );	\


#define LOG_STATE_MACHINE(RUN_NAME,PROVIDER,SYMBOL,X) 	\
	DRAWOBJ_TEXT(RUN_NAME,CppUtils::getTime(),GERM_2CHANNEL_MM, SYMBOL, PROVIDER, BrkLib::AP_MEDIUM, "STATE MACHINE", X );	\


namespace AlgLib {
	//

	enum TradeDirection
	{
		TDNONE = 0,
		TDLONG = 1,
		TDSHORT =2,
		TDBOTH = 3
	};

	struct TradeParam
	{
		TradeParam():
			period_m(-1),
			periodStop_m(-1),
			schift_m(-1),
			isReverse_m(false),
			koeff_m(-1),
			minLots_m(-1),
			maxLots_m(-1),
			risk_m(-1),
			accountSize_m(-1),
			tradePeriodMinutes_m(-1)
		{
		}

		bool const isInvalid() const
		{
			return (
				period_m <= 0 || periodStop_m <= 0 ||  
				koeff_m <= 0 || minLots_m <=0 || maxLots_m <= 0 || risk_m<=0 || 
				accountSize_m <=0 || tradePeriodMinutes_m <=0
			);
		}

		CppUtils::String toString() const
		{
			CppUtils::String result = 
				"period_m="+CppUtils::long2String(period_m) + " \n" +
				"periodStop_m="+CppUtils::long2String(periodStop_m)+ " \n" +
				"schift_m="+CppUtils::float2String(schift_m)+ " \n" +
				"isReverse_m="+(isReverse_m ? "true":"false")+ " \n" +
				"koeff_m="+CppUtils::float2String(periodStop_m)+ " \n" +
				"minLots_m="+CppUtils::float2String(minLots_m)+ " \n" +
				"maxLots_m="+CppUtils::float2String(maxLots_m)+ " \n" +
				"risk_m="+CppUtils::float2String(risk_m)+"\n"+
				"accountSize_m="+CppUtils::float2String(accountSize_m)+"\n"+
				"tradePeriodMinutes_m="+CppUtils::long2String(tradePeriodMinutes_m);

			return result;
		}

		HlpStruct::WorkTimeHolder workTimeHolder_m;

		// trade params
		int period_m;

		int periodStop_m;

		double schift_m;

		bool isReverse_m;
		double koeff_m;

		double minLots_m;

		double maxLots_m;

		double risk_m;

		double accountSize_m;

		int tradePeriodMinutes_m;

	};

	// ------------------------------------

	struct SymbolContext
	{
		SymbolContext():
			highChannel_m(-1),
			lowChannel_m(-1),
			highStopChannel_m(-1),
			lowStopChannel_m(-1),
			time_m(-1)
		{
		};

		double highChannel_m;

		double lowChannel_m;

		double highStopChannel_m;

		double lowStopChannel_m;

		double time_m;

	};

	// -------------------------------------




	class GERM_2CHANNEL_MM_EXP Grm2ChannelMM :public AlgorithmBase {
		//
	public:
		//
		// ctor & dtor
		Grm2ChannelMM( Inqueue::AlgorithmHolder& algHolder);

		virtual ~Grm2ChannelMM();
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

		void issueBuyOrder(CppUtils::String const& symbol,double const& volume);

		void issueSellOrder(CppUtils::String const& symbol, double const& volume);

		void issueCloseLongOrder(CppUtils::String const& symbol, double const& volume, CppUtils::String const& brokerOrderId);

		void issueCloseShortOrder(CppUtils::String const& symbol, double const& volume, CppUtils::String const& brokerOrderId);
	
	


	private:

		void exportSymbol(Inqueue::ContextAccessor& historyaccessor, CppUtils::String const& symbol, int const cache_id);

		void exportSymbol(Inqueue::ContextAccessor& historyaccessor, CppUtils::String const& symbol);

		void initStateMachine(CppUtils::StringSet const& symbols);

		void deinitStateMachine();

		double calculateTradingVolume(double const& highChannel, double const& lowChannel);

		// this version simply process event
		template<class E_TYPE>
		void processEvent(CppUtils::String const& symbol, CppUtils::BaseEvent& pev);

		// this version passed E_TARGET_TYPE as a recipient for this event, thus E_TARGET_TYPE will have access via mailed_event_arrived to this pev
		template<class E_TYPE, class E_TARGET_TYPE>
		void processEvent(CppUtils::String const& symbol, CppUtils::BaseEvent& pev);

		CppUtils::String historyDataProfile_m;

		CppUtils::String rtDataProvider_m;

		CppUtils::String barDataProvider_m;

		TradeDirection allowTrading_m;
		
		TradeParam tradeParam_m;

		map<CppUtils::String, SymbolContext> contextMap_m;

		CppUtils::String connectRunName_m;

		CppUtils::String baseExportPath_m;

		// map containing symbols
		map<CppUtils::String, CppUtils::StateMachine<MachineContext>* > tradingMachines_m;

		CppUtils::Mutex stateMachineMtx_m;

		BrkLib::BrokerConnect* brokerConnect_m;
		

	};

}; // end of namespace 


#endif // _GERM_2CHANNEL_MM_IMPL_INCLUDED
