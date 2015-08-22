#include "implement.hpp"
 // Many thanks to Zoubok Victor for programming this

namespace AlgLib {
AlgorithmBase *creator (Inqueue::AlgorithmHolder* algholder)
{
	return new CSmartcomAlgStub(*algholder);
}

void terminator (AlgorithmBase *pthis)
{
	assert(pthis);
	delete pthis;
}

AlgorithmBase::CreatorAlgorithmFun createAlgorithm = creator;

AlgorithmBase::DestroyerAlgorithmFun destroyAlgorithm = terminator;

// intreface to be implemented
CSmartcomAlgStub::CSmartcomAlgStub( Inqueue::AlgorithmHolder& algHolder):
	AlgorithmBase(algHolder)
{

}

CSmartcomAlgStub::~CSmartcomAlgStub()
{

}

void CSmartcomAlgStub::onLibraryInitialized(
	Inqueue::AlgBrokerDescriptor const& descriptor, 
	BrkLib::BrokerBase* brokerlib, 
	CppUtils::String const& runName,
	CppUtils::String const& comment
)
{
		LOGEVENT(HlpStruct::CommonLog::LL_DEBUG,  SMARTCOM_ALG_STUB, "Library initialized"	);
}

void CSmartcomAlgStub::onLibraryFreed()
{

}

void CSmartcomAlgStub::onBrokerResponseArrived(
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
)
{

}

void CSmartcomAlgStub::onDataArrived(
	HlpStruct::RtData const& rtdata, 
	Inqueue::ContextAccessor& historyaccessor, 
	Inqueue::TradingParameters const& descriptor,
	BrkLib::BrokerBase* brokerlib,
	CppUtils::String const& runName,
	CppUtils::String const& comment
)
{

}
		
void CSmartcomAlgStub::onRtProviderSynchEvent (
		Inqueue::ContextAccessor& contextaccessor, HlpStruct::RtData::SynchType const synchFlg,
		double const& synchTime,
		CppUtils::String const& providerName,
		CppUtils::String const& message
)
{
	 // RT provider events - must check for sisconnections
		if (synchFlg==HlpStruct::RtData::SYNC_EVENT_PROVIDER_START) {
			LOGEVENT(HlpStruct::CommonLog::LL_DEBUG,  SMARTCOM_ALG_STUB, "RT Provider started: " << providerName	);
			
		}
		else if (synchFlg==HlpStruct::RtData::SYNC_EVENT_PROVIDER_FINISH) {
			
			LOGEVENT(HlpStruct::CommonLog::LL_DEBUG,  SMARTCOM_ALG_STUB, "RT Provider finished: " << providerName	);

			// reset everything
			bool result;
			CppUtils::String log;
			contextaccessor.requestRunExternalPackage("smartcom_main_starter.jar", true, result, log);

		}
		else if (synchFlg==HlpStruct::RtData::SYNC_EVENT_PROVIDER_TRANSMIT_ERROR) {
		
			LOGEVENT(HlpStruct::CommonLog::LL_DEBUG,  SMARTCOM_ALG_STUB, "RT Provider transmit error: " << providerName << " - " << message	);
			
		}
		else if (synchFlg==HlpStruct::RtData::SYNC_EVENT_CUSTOM_MESSAGE) {
			
			LOGEVENT(HlpStruct::CommonLog::LL_DEBUG,  SMARTCOM_ALG_STUB, "RT Provider custom message: " << providerName << " - " << message	);
		}
}
   
void CSmartcomAlgStub::onLevel2DataArrived(
		HlpStruct::RtLevel2Data const& level2data,
		Inqueue::ContextAccessor& historyaccessor, 
		Inqueue::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
	)
{

}

void CSmartcomAlgStub::onLevel1DataArrived(
		HlpStruct::RtLevel1Data const& level1data,
		Inqueue::ContextAccessor& historyaccessor, 
		Inqueue::TradingParameters const& descriptor,
		BrkLib::BrokerBase* brokerlib,
		CppUtils::String const& runName,
		CppUtils::String const& comment
	)
{

}

void CSmartcomAlgStub::onEventArrived(
	Inqueue::ContextAccessor& contextaccessor, 
	HlpStruct::CallingContext& callContext,
	bool& result,
	CppUtils::String& customOutputData
)
{
	// just stub also
		customOutputData = "FAILED";
		CppUtils::String const& command = callContext.getCallCustomName();

		CppUtils::String const& data = callContext.getParameter<BrkLib::String_Proxy>("custom_data").get();

		// parse as simple structure
		HlpStruct::XmlParameter xmlparam_data;
		HlpStruct::HumanSerializer::deserializeXmlParameter(data, xmlparam_data);

		if (command=="hallo") {
					// n/a
		}
		else if (command=="load_data") {
			LOGEVENT(HlpStruct::CommonLog::LL_DEBUG,  SMARTCOM_ALG_STUB, "[load_data] is not necessary here"	);
		}
		else if (command=="get_symbol_list") {
			// resolve loaded symbol list
			
			// resolve all

		}
		else
			THROW(CppUtils::OperationFailed, "exc_InvalidCommand", "ctx_onEventArrived", command);

		// no exceptions
		customOutputData ="OK";
}
	
	void CSmartcomAlgStub::onThreadStarted(
		Inqueue::ContextAccessor& contextaccessor, bool const firstLib, bool const lastLib
	)
{
		LOGEVENT(HlpStruct::CommonLog::LL_DEBUG,  SMARTCOM_ALG_STUB, "Working thread started"	);
}

void CSmartcomAlgStub::onThreadFinished(
	Inqueue::ContextAccessor& contextaccessor, Inqueue::TradingParameters const& tradeParams, bool const firstLib, bool const lastLib
)
{

}

CppUtils::String const& CSmartcomAlgStub::getName() const
{
	static const CppUtils::String name("[ alg lib: CSmartcomAlgStub ]");
	return name;
}


} // end of namespace
