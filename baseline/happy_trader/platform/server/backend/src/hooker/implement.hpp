#ifndef _HOOKER_IMP_INCLUDED
#define _HOOKER_IMP_INCLUDED

#include "../brklib/brklib.hpp"
#include "hookerdefs.hpp"

namespace AlgLib {

	class HOOKER_EXP GerhikHooker: public AlgorithmBase {

	public:

		// ctor & dtor
		GerhikHooker( Inqueue::AlgorithmHolder& algHolder);

		virtual ~GerhikHooker();
		
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
			Inqueue::ContextAccessor& contextaccessor,
			HlpStruct::RtData::SynchType const synchFlg,
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
			Inqueue::ContextAccessor& contextaccessor, HlpStruct::TradingParameters const& tradeParams, bool const firstLib, bool const lastLib
		)
		{
		}

		



		virtual CppUtils::String const& getName() const;
		
		// -----------------------

	public:
	
	private:

		// hours of working
		int hoursBeginMonitor_m;
		
		int minutesBeginMonitor_m;

		int hoursBegin_m;
		
		int minutesBegin_m;

		int hourEnd_m;

		int minutesEnd_m;

		int maPeriod_m;


		// hooker params
		double hiLowHookerCriteriaPct_m;

		double bodyLehgthHookerCriteriaPct_m;

		// unix seconds after day begin of our significan points
		double unixTimeBeginMonitor_m;

		double unixTimeBeginTrade_m;

		double unixTimeEndTrade_m;

		CppUtils::StringSet tradedSymbols_m;

		CppUtils::String pathToExport_m;

		// period of caching....
		int PERIOD_MIN_m;

		// if true - raise only both signals
		bool RAISE_ONLY_BOTH_m;

		// if raise consolidation alerts
		bool RAISE_CONSOLIDATION_m;

		// the number of even bars to raise signal
		int EVEN_BARS_COUNT_m;

	

				
	};

};

#endif