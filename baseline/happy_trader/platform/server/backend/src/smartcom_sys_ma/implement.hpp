#ifndef _SMARTCOM_SYS_MA_IMPL_INCLUDED
#define _SMARTCOM_SYS_MA_IMPL_INCLUDED

 // Many thanks to Zoubok Victor

#include "smartcom_sys_madefs.hpp"

#define SMARTCOM_SYS_MA "SMARTCOM_SYS_MA"

#define LOG_SYMBOL(RUN_NAME, TIME, PROVIDER, SYMBOL,X) 	\
	DRAWOBJ_TEXT(RUN_NAME,TIME,SMARTCOM_SYS_MA, SYMBOL, PROVIDER, BrkLib::AP_MEDIUM, "LOG", X );	



#define LOG_COMMON(RUN_NAME, TIME, LEVEL,X) 	\
	DRAWOBJ_TEXT(RUN_NAME,TIME,SMARTCOM_SYS_MA, "-ALL-", "log_provider", LEVEL, "LOG", X );	



namespace AlgLib {

	struct TradeContext
	{
		TradeContext()
		{
			clear();

		}

		void clear()
		{
			tradePeriodMinutes_m = -1;
			ma1_m = -1;
			ma2_m = -1;
			prevMa1_m = -1;
			prevMa2_m = -1;
			curMa1_m = -1;
			curMa2_m = -1;
			userId_m = -1;
			sessionId_m = "";
			userName_m = "";
			symbols_m.clear();
		}

		// can be different for each thread
		int tradePeriodMinutes_m;
		
		CppUtils::StringSet symbols_m;

		int ma1_m;

		int ma2_m;

		double prevMa1_m;

		double prevMa2_m;

		double curMa1_m;

		double curMa2_m;

		int userId_m;

		CppUtils::String userName_m;

		CppUtils::String sessionId_m;

	};


	//
	class SMARTCOM_SYS_MA_EXP SCSysExample :public AlgorithmBase {
		//
	public:
		//
		// ctor & dtor
		SCSysExample( Inqueue::AlgorithmHolder& algHolder);

		virtual ~SCSysExample();
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

	private:

		void initThreadParameters(TradeContext& ctx, HlpStruct::TradingParameters const& descriptor, CppUtils::String const& runName);

		CppUtils::String historyDataProfile_m;

		static map<CppUtils::String, TradeContext> userContextMap_m;

		static CppUtils::Mutex userContextMapMtx_m;

		//CppUtils::String rtDataProvider_m;

		//CppUtils::String barDataProvider_m;

		void openLong(CppUtils::String const& provider, CppUtils::String const& symbol);

		void openShort(CppUtils::String const& provider,CppUtils::String const& symbol);

		void closeLong(CppUtils::String const& provider,CppUtils::String const& symbol, CppUtils::String const& brokerPositionId);

		void closeShort(CppUtils::String const& provider,CppUtils::String const& symbol, CppUtils::String const& brokerPositionId);

		BrkLib::BrokerConnect* brokerConnect_m;

		CppUtils::StringMap positions_m;

			

	
	};

}; // end of namespace 


#endif // _SMARTCOM_SYS_MA_IMPL_INCLUDED
