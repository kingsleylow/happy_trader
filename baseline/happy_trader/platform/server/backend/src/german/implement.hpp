#ifndef _MGERCHIK_IMP_INCLUDED
#define _MGERCHIK_IMP_INCLUDED

#include "../brklib/brklib.hpp"
#include "germandefs.hpp"


// ----------------------------------

namespace AlgLib {

	enum TradeState {
		TSG_Initial =		0,
		TSG_Pending =		1,
		TSG_OpenShort =	2,
		TSG_OpenLong =	3
	};

	CppUtils::String getTradeState(TradeState const ts)
	{
		switch(ts) {
			case TSG_Initial: return "POSITION INITIAL";
			case TSG_Pending: return "POSITION PENDING";
			case TSG_OpenShort: return "POSITION OPEN SHORT";
			case TSG_OpenLong: return "POSITION OPEN LONG";
			default: return "POSITION N/A";
		} 
	}

	// position state
	struct TradeFactors
	{
		TradeFactors():
			kPvol_m(-1),	
			pivot1_m(-1),
			pivot2_m(-1),
			parab_m(-1)
		{
		}

		inline bool isValid() const
		{
			return (kPvol_m >= 0 && pivot1_m >=0 && pivot2_m >= 0 && parab_m >= 0);
		}

		double kPvol_m;

		double pivot1_m;

		double pivot2_m;

		double parab_m;

	};


	struct TradeParams
	{
		TradeParams():
		tradeAggrPeriod_m(Inqueue::AP_Dummy),
		tradeMultFactor_m(-1),
		RSI_PERIOD_G_m(-1),
		PARABOLIC_ACEL_UP_m(-1),
		PARABOLIC_ACEL_DOWN_m(-1),
		PARABOLIC_ACEL_MAX_m(-1)
		{
		}

		int getMaxDepth() const
		{
			return max(RSI_PERIOD_G_m+1,20);
		}

		Inqueue::AggregationPeriods tradeAggrPeriod_m;

		int tradeMultFactor_m;

		double PARABOLIC_ACEL_UP_m; 
 
		double PARABOLIC_ACEL_DOWN_m;
 
		double PARABOLIC_ACEL_MAX_m;

		int RSI_PERIOD_G_m;
	};

	
	class GERMAN_EXP MGerman: public AlgorithmBase {

	public:

		// ctor & dtor
		MGerman( Inqueue::AlgorithmHolder& algHolder);

		virtual ~MGerman();
		
		// intreface to be implemented
		virtual void onLibraryInitialized(
			Inqueue::AlgBrokerDescriptor const& descriptor, 
			BrkLib::BrokerBase* brokerlib, 
			CppUtils::String const& runName,
			CppUtils::String const& comment
		);

		virtual void onLibraryFreed();

		virtual void onDataArrived(
			HlpStruct::RtData const& rtdata, 
			Inqueue::ContextAccessor& historyaccessor, 
			Inqueue::TradingParameters const& descriptor,
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		);

		virtual void onRtProviderSynchEvent (
			Inqueue::ContextAccessor& contextaccessor,
			HlpStruct::RtData::SynchType const synchFlg,
			double const& synchTime,
			CppUtils::String const& providerName,
			CppUtils::String const& message
		);

		virtual void onLevel2DataArrived(
			HlpStruct::RtLevel2Data const& level2data,
			Inqueue::ContextAccessor& historyaccessor, 
			Inqueue::TradingParameters const& descriptor,
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		);

		virtual void onLevel1DataArrived(
			HlpStruct::RtLevel1Data const& level1data,
			Inqueue::ContextAccessor& historyaccessor, 
			Inqueue::TradingParameters const& descriptor,
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		);

		virtual void onBrokerResponseArrived(
			BrkLib::BrokerConnect &brkConnect,
			BrkLib::BrokerResponseBase const& bresponse
		);

		virtual void onEventArrived(
				Inqueue::ContextAccessor& contextaccessor, 
				HlpStruct::CallingContext& callContext,
				bool& result,
				CppUtils::String &customOutputData
		);
	
		virtual void onThreadStarted(
			Inqueue::ContextAccessor& contextaccessor, bool const firstLib, bool const lastLib
		);

		virtual void onThreadFinished(
			Inqueue::ContextAccessor& contextaccessor, bool const firstLib, bool const lastLib
		)
		{
		}


		virtual CppUtils::String const& getName() const;
		
		// -----------------------

	public:
	
	private:

		

		TradeParams tradeParams_m;
				
		CppUtils::String path_wl_ds_m;

		// this is a member because
		//Math::WLDRSIMathEngine* pEngineRSI_m;

		map<CppUtils::String, TradeFactors> prevSignals_m;

		
		// this flag showing trading state ( e.i. this is the state machine flag)
		map<CppUtils::String, TradeState> tradeState_m;

		map<CppUtils::String, CppUtils::String> openPosId_m;

		//CppUtils::String runName_m;
				
	};


}; // end of namespace

#endif