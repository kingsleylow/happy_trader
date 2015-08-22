#ifndef _GAP_DETECTOR_IMP_INCLUDED
#define _GAP_DETECTOR_IMP_INCLUDED

#include "gap_detectordefs.hpp"


namespace AlgLib {


	struct Symbol 
	{
		Symbol():
			openToday_m(-1),
			closePrevDay_m(-1),
			volumeFactor_m(-1)
		{
		}

		double closePrevDay_m;

		double openToday_m;

		double volumeFactor_m;
	};

	class GAP_DETECTOR_EXP GapDetector: public AlgorithmBase {

	public:

		// ctor & dtor
		GapDetector( Inqueue::AlgorithmHolder& algHolder);

		virtual ~GapDetector();
		
		// intreface to be implemented
		virtual void onLibraryInitialized(
			HlpStruct::AlgBrokerDescriptor const& descriptor, 
			BrkLib::BrokerBase* brokerlib, 
			BrkLib::BrokerBase* brokerlib2,
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

		virtual void GapDetector::onEventArrived(
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

		

		map<CppUtils::String, Symbol> symbolEntries_m;

		// list of symbols to process
		CppUtils::StringSet checkedSymbols_m;	

		CppUtils::String provider_m;

		double unixTimeBeginMonitor_m;

		int hoursBeginMonitor_m;
		
		int minutesBeginMonitor_m;

	
   
				
	};
};

#endif
