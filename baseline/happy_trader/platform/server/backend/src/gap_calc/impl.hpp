#ifndef _GAP_CALC_IMP_INCLUDED
#define _GAP_CALC_IMP_INCLUDED

#include "gap_calcdefs.hpp"
#include "symbolstruct.hpp"

namespace AlgLib {


	class GAP_CALC_EXP GapCalculator: public AlgorithmBase {

	public:

		// ctor & dtor
		GapCalculator( Inqueue::AlgorithmHolder& algHolder);

		virtual ~GapCalculator();
		
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

		// calculation function
		// it calculates probable profit according the system
		// and returns some parameters
		void returnProfitPerRun(
			bool const is_short,
			bool const singleton,
			double const& stolLossLevel,
			double const& volumeFactorFilter, 
			double const& gapFilter, 
			Inqueue::ContextAccessor& historyaccessor,
			CalcResult& result
		);

		void createReportToTheFile(CppUtils::String const& filename, vector<CalcResult> const& resultlist);
	
	private:

		CppUtils::StringSet checkedSymbols_m;	

		CppUtils::String provider_m;

		// testing dates
		double beginTestingDate_m;

		double endTestingDate_m;

		// to speed up
		CppUtils::IntList cachedIds_m;

		
	
   
				
	};
};

#endif
