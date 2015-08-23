#include "implement.hpp"


namespace AlgLib {
AlgorithmBase *creator (Inqueue::AlgorithmHolder* algholder)
{
	return new MutualMove(*algholder);
}

void terminator (AlgorithmBase *pthis)
{
	HTASSERT(pthis);
	delete pthis;
}

AlgorithmBase::CreatorAlgorithmFun createAlgorithm = creator;

AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm = terminator;

// intreface to be implemented
MutualMove::MutualMove( Inqueue::AlgorithmHolder& algHolder):
	AlgorithmBase(algHolder)
{

}

MutualMove::~MutualMove()
{

}

void MutualMove::onLibraryInitialized(
	HlpStruct::AlgBrokerDescriptor const& descriptor, 
	BrkLib::BrokerBase* brokerlib, 
	BrkLib::BrokerBase* brokerlib2,
	CppUtils::String const& runName,
	CppUtils::String const& comment
)
{

}

void MutualMove::onLibraryFreed()
{

}

void MutualMove::onBrokerResponseArrived(
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
)
{

}

void MutualMove::onRtProviderSynchEvent (
			Inqueue::ContextAccessor& contextaccessor,
			HlpStruct::RtData::SynchType const synchFlg,
			double const& synchTime,
			CppUtils::String const& providerName,
			CppUtils::String const& message
		)
	{
	}


void MutualMove::onDataArrived(
	HlpStruct::RtData const& rtdata, 
	Inqueue::ContextAccessor& historyaccessor, 
	HlpStruct::TradingParameters const& descriptor,
	BrkLib::BrokerBase* brokerlib,
	CppUtils::String const& runName,
	CppUtils::String const& comment
)
{
	if (historyaccessor.isLast()) {
		CppUtils::String data_EURUSD, data_ES_CONT;
		int cache_es_cont = historyaccessor.cacheSymbol(rtdata.getProvider2(), "ES_CONT", HlpStruct::AP_Hour, 1);
		int cache_eurusd = historyaccessor.cacheSymbol(rtdata.getProvider2(), "EURUSD", HlpStruct::AP_Hour, 1);

		Inqueue::DataRowPtr rowPtr = historyaccessor.dataRowFirst();

		while (historyaccessor.dataRowNext(rowPtr) ) {
			HlpStruct::PriceData const& es_data = historyaccessor.getCurrentDataRow(cache_es_cont, rowPtr);
			HlpStruct::PriceData const& eurusd_data = historyaccessor.getCurrentDataRow(cache_eurusd, rowPtr);

			  
		}

		
		
	}
	
  
}
   
void MutualMove::onLevel2DataArrived(
		HlpStruct::RtLevel2Data const& level2data,
		Inqueue::ContextAccessor& historyaccessor, 
		HlpStruct::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
	)
{

}

void MutualMove::onLevel1DataArrived(
		HlpStruct::RtLevel1Data const& level1data,
		Inqueue::ContextAccessor& historyaccessor, 
		HlpStruct::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
	)
{

}

void MutualMove::onEventArrived(
				Inqueue::ContextAccessor& contextaccessor, 
				HlpStruct::CallingContext& callContext,
				bool& result,
				CppUtils::String &customOutputData
		)
{
	result = false;
}


	
void MutualMove::onThreadStarted(Inqueue::ContextAccessor& contextaccessor, bool const firstLib, bool const lastLib) 
{
};

CppUtils::String const& MutualMove::getName() const
{
	static const CppUtils::String name("[ MutualMove ]");
	return name;
}


} // end of namespace
