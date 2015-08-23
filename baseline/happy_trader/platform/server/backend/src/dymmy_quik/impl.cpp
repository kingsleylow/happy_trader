#include "impl.hpp"
#include "../inqueue/commontypes.hpp"
#include "../math/math.hpp"

#define ALG_DUMMY_QUIK "ALG_DUMMY_QUIK"

namespace AlgLib {
	// --------------
	// createor & terminator functions
	AlgorithmBase *creator (Inqueue::AlgorithmHolder* algholder)
	{
		return new AlgorithQuikDummy(*algholder);
	}

	void terminator (AlgorithmBase *pthis)
	{
		assert(pthis);
		delete pthis;
	}

	AlgorithmBase::CreatorAlgorithmFun createAlgorithm = creator;

	AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm = terminator;

	// --------------
	// ctor & dtor
	AlgorithQuikDummy::AlgorithQuikDummy( Inqueue::AlgorithmHolder& algHolder):
		AlgorithmBase(algHolder)
	{
	}

	AlgorithQuikDummy::~AlgorithQuikDummy()
	{
	}

	// --------------
	// interfaces

	void AlgorithQuikDummy::onLibraryInitialized(
		Inqueue::AlgBrokerDescriptor const& descriptor, 
		BrkLib::BrokerBase* brokerlib, 
		CppUtils::String const& runName, 
		CppUtils::String const& comment
	)
	{
		

		BrkLib::BrokerConnect brkConnect(brokerlib, "dummy", false, false, runName.c_str(), comment.c_str());
		brkConnect.connect();

		BrkLib::BrokerOperation oper(brkConnect);

		BrkLib::Order order;
		oper.unblockingIssueOrder(order);
		
	}

	void AlgorithQuikDummy::onLibraryFreed()
	{
	}

	void AlgorithQuikDummy::onDataArrived(
			HlpStruct::RtData const& rtdata, 
			Inqueue::ContextAccessor& historyaccessor, 
			Inqueue::TradingParameters const& descriptor,
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		)
	{
		LOGEVENTA(HlpStruct::CommonLog::LL_INFO,  ALG_DUMMY_QUIK, "RT data arrived" ,	
			"Rt data:" << rtdata.toString() );
		
	}

	void AlgorithQuikDummy::onLevel2DataArrived(
			HlpStruct::RtLevel2Data const& level2data,
			Inqueue::ContextAccessor& historyaccessor, 
			Inqueue::TradingParameters const& descriptor,
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		)
	{
			LOGEVENTA(HlpStruct::CommonLog::LL_INFO,  ALG_DUMMY_QUIK, "Level2 data arrived" ,	
				"Level2 data:" << level2data.toString() );
		
	}

	void AlgorithQuikDummy::onLevel1DataArrived(
			HlpStruct::RtLevel1Data const& level1data,
			Inqueue::ContextAccessor& historyaccessor, 
			Inqueue::TradingParameters const& descriptor,
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		)
	{
			LOGEVENTA(HlpStruct::CommonLog::LL_INFO,  ALG_DUMMY_QUIK, "Level1 data arrived" ,	
				"Level1 data:" << level1data.toString() );
		
	}

	void AlgorithQuikDummy::onBrokerResponseArrived(
			BrkLib::BrokerConnect &brkConnect,
			BrkLib::BrokerResponseBase const& bresponse
		)
	{
	}

	bool AlgorithQuikDummy::onEventArrived(HlpStruct::EventData const &eventdata, Inqueue::ContextAccessor& contextaccessor, HlpStruct::EventData& response)
	{
		return false;
	}

	bool AlgorithQuikDummy::onThreadStarted(Inqueue::ContextAccessor& contextaccessor) {
		return false;
	};
	
	CppUtils::String const& AlgorithQuikDummy::getName() const
	{
		static const CppUtils::String name("dummy quik algorithm");
		return name;
	}


}; // END OF NAMESPACE