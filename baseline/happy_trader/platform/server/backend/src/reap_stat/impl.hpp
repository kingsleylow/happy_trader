#ifndef _REAP_STAT_IMP_INCLUDED
#define _REAP_STAT_IMP_INCLUDED

#include "../brklib/brklib.hpp"
#include "reap_statdefs.hpp"
#include "reap_stat.hpp"


namespace AlgLib {
	class REAP_STAT_EXP ReapStat: public AlgorithmBase {

		public:

		
		// ctor & dtor
		ReapStat( Inqueue::AlgorithmHolder& algHolder);

		virtual ~ReapStat();
		
		// intreface to be implemented
		virtual void onLibraryInitialized(
			Inqueue::AlgBrokerDescriptor const& descriptor, 
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		);

		virtual void onLibraryFreed();

		// this will be called only 
		virtual void onBrokerResponseArrived(
			BrkLib::BrokerConnect &brkConnect,
			BrkLib::BrokerResponseBase const& bresponse
		);


		// note that this is called from any thread which is subscribed as algrorith thread
		// 

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
			Inqueue::TradingParameters const& descriptor,
			CppUtils::String const& runName,
			CppUtils::String const& comment
	);
	
	
	
		virtual bool onEventArrived(HlpStruct::EventData const &eventdata, HlpStruct::EventData& response);


		virtual CppUtils::String const& getName() const;
	};

}; // end of namespace 

#endif