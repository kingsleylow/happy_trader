
#ifndef _ALGLIBDUMMY_QUIK_ALGORITHM_INCLUDED
#define _ALGLIBDUMMY_QUIK_ALGORITHM_INCLUDED

#include "../brklib/brklib.hpp"
#include "dymmy_quikdefs.hpp"

namespace Inqueue {
	class AlgorithmHolder;
	struct AlgBrokerDescriptor;
}

namespace AlgLib {

	class DYMMY_QUIK_EXP AlgorithQuikDummy: public AlgorithmBase {
	public:
		AlgorithQuikDummy( Inqueue::AlgorithmHolder& algHolder);

		virtual ~AlgorithQuikDummy();

		// --------------

		// interfaces to be implemented
		virtual void onLibraryInitialized(Inqueue::AlgBrokerDescriptor const& descriptor, BrkLib::BrokerBase* brokerlib, CppUtils::String const& runName, CppUtils::String const& comment);

		virtual void onLibraryFreed();

		virtual void onDataArrived(
			HlpStruct::RtData const& rtdata, 
			Inqueue::ContextAccessor& historyaccessor, 
			Inqueue::TradingParameters const& descriptor,
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
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

		virtual bool onEventArrived(HlpStruct::EventData const &eventdata, Inqueue::ContextAccessor& contextaccessor, HlpStruct::EventData& response);
	
		virtual bool onThreadStarted(
			Inqueue::ContextAccessor& contextaccessor
		);


		virtual CppUtils::String const& getName() const;

	private:

	};
};

#endif