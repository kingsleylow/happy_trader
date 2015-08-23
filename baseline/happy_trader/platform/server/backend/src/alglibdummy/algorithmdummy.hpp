#ifndef _ALGLIBDUMMY_ALGORITHM_INCLUDED
#define _ALGLIBDUMMY_ALGORITHM_INCLUDED

#include "../brklib/brklib.hpp"
#include "alglibdummydefs.hpp"


namespace Inqueue {
	class AlgorithmHolder;
	struct AlgBrokerDescriptor;
}


/**
 This library will implement the following sustem - 
 Open position at some moment of the day, then will close this the next day
*/
namespace AlgLib {

	

	/**
		This is the base class for all the implemented algorithms 
	*/
	class ALGLIBDUMMY_EXP AlgorithmDummy: public AlgorithmBase {
		
	public:

		
		// ctor & dtor
		AlgorithmDummy( Inqueue::AlgorithmHolder& algHolder);

		virtual ~AlgorithmDummy();
		
		// interfaces to be implemented
		virtual void onLibraryInitialized(HlpStruct::AlgBrokerDescriptor const& descriptor, BrkLib::BrokerBase* brokerlib, BrkLib::BrokerBase* brokerlib2, CppUtils::String const& runName, CppUtils::String const& comment);

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

		void onEventArrived(
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
			};



		virtual CppUtils::String const& getName() const;

	private:


		void openLong(CppUtils::String const& provider, CppUtils::String const& symbol);

		void openShort(CppUtils::String const& provider,CppUtils::String const& symbol);

		void closeLong(CppUtils::String const& provider,CppUtils::String const& symbol, CppUtils::String const& brokerPositionId);

		void closeShort(CppUtils::String const& provider,CppUtils::String const& symbol, CppUtils::String const& brokerPositionId);

		bool isLong_m;

		int idx_m;

		BrkLib::BrokerConnect* brokerConnect_m;

		CppUtils::String longPositionId_m;

		CppUtils::String shortPositionId_m;

	};
};

#endif
