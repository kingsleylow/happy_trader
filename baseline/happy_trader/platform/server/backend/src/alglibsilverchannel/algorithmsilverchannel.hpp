#ifndef _ALGLIBSILVERCHANNEL_ALGORITHM_INCLUDED
#define _ALGLIBSILVERCHANNEL_ALGORITHM_INCLUDED

#include "../brklib/brklib.hpp"
#include "alglibsilverchanneldefs.hpp"


// dummy algorithm machine states
#define STATE_DUMMY					0
#define STATE_NO_OPEN_POS		1
#define STATE_SHORT_OPENED	2
#define STATE_LONG_OPENED		3

// dummy algorithm machine events
#define EVENT_NO_ACTION									0
#define EVENT_ORDER_CLOSED							1
#define EVENT_ORDER_OPENED							2
#define EVENT_LONG_SIGNAL								3
#define EVENT_SHORT_SIGNAL							4


// dummy algorithm machine activities
#define DO_QUERY_FOR_POS	1
#define DO_OPEN_LONG_POS	2
#define DO_OPEN_SHORT_POS	3
#define DO_CLOSE_POS			4

// global IDS
#define GLB_MA_SHORT	1
#define GLB_MA_LONG		2


namespace AlgLib {
	
	class ALGLIBSILVERCHANNEL_EXP AlgorithmSilverChannel: public AlgorithmBase {
		
	public:

		
		// ctor & dtor
		AlgorithmSilverChannel( Inqueue::AlgorithmHolder& algHolder);

		virtual ~AlgorithmSilverChannel();
		
		// intreface to be implemented
		virtual void onLibraryInitialized(Inqueue::AlgBrokerDescriptor const& descriptor, BrkLib::BrokerBase* brokerlib, CppUtils::String const& runName, CppUtils::String const& comment);

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

	private:

		

		// checks for automat state
		void checkState(BrkLib::BrokerConnect& brkConnect);

		// passed as parameter
		Inqueue::AggregationPeriods aggrPeriod_m;

		// mult factor
		int multFactor_m;

		CppUtils::String posId_m;

		bool forceSendConnectEvent_m;
		
		
	};
};

#endif
