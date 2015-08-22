
#include "algorithm.hpp"
#include "../inqueue/inqueue.hpp"
#include "../inqueue/commontypes.hpp"
#include "../brklib/brklib.hpp"
#include "../brklib/proxy.hpp"

#define ALGBASE "ALGBASE"

namespace AlgLib {

	AlgorithmBase::AlgorithmBase(Inqueue::AlgorithmHolder& algHolder ):
		algHolder_m(algHolder),
		state_m(0),
		outproc_m(*CppUtils::getRootObject<Inqueue::OutProcessor>())
	{
	}
		
	AlgorithmBase::~AlgorithmBase()
	{
	}

		

	HlpStruct::GlobalStorage& AlgorithmBase::getGlobalStorage()
	{
		return globalStorage_m;
	}
	
	bool AlgorithmBase::processAlgorithmRelatedEvent(
			HlpStruct::EventData const &request, 
			Inqueue::ContextAccessor& contextaccessor, 
			HlpStruct::EventData& response
		)
	{
		response.getEventType() = HlpStruct::EventData::ET_AlgorithmBrokerEventResp_FromAlgorithm;
		response.setType(HlpStruct::EventData::DT_CommonXml);

		// set parent UID
		response.setParentUid(request.getEventUid());

		HlpStruct::XmlParameter& respxml = response.getAsXmlParameter();

		bool result = true;
		HlpStruct::CallingContext callContext;

		try {

			// get request XMl reference
			HlpStruct::XmlParameter const& reqxml = request.getAsXmlParameter();

			// prepare for the call
			BrkLib::BrokerProxyBaseCreator creator;
			HlpStruct::convertCallingContextFromXmlParameter(creator, callContext, reqxml);

			// need to determine whether we are coling correct thread ???
			CppUtils::String const& algLibPair = callContext.getParameter<BrkLib::String_Proxy const>("alg_lib_pair").get();

			if (algLibPair.size() <= 0 || algLibPair!=getAlgHolder().getAlgBrokerDescriptor().name_m) {
				// out 
				THROW(CppUtils::OperationFailed, "exc_InvalidAlgLibPairName", "ctx_processAlgorithmRelatedEvent", algLibPair + " instead of " + getAlgHolder().getAlgBrokerDescriptor().name_m);
				//LOG( MSG_WARN, ALGBASE, "Ignoring call to algoritm layer '" << algLibPair << "' as this is " << getAlgHolder().getAlgBrokerDescriptor().name_m);

				// do not raise response event
				//return false;
			}

			if (callContext.getCallName() == "remote_SendCustomEvent") {
				// call common
				CppUtils::String customOutData;
				onEventArrived(contextaccessor, callContext, result, customOutData);


				callContext.getParameter<BrkLib::String_Proxy>("out_return_data").get() = customOutData;
			}
			else
				THROW(CppUtils::OperationFailed, "exc_InvalidCallNameForAlgorithm", "ctx_processAlgorithmRelatedEvent", callContext.getCallName());
		
		}
		catch(CppUtils::Exception& e) {
				callContext.getResultReason() = "Exception: " + e.message() + " - " + e.context() + " - " + e.arg();
				callContext.getResultCode() = -1;

		}
#ifdef HANDLE_NATIVE_EXCEPTIONS
		catch(Testframe::Exception& e) {
				callContext.getResultReason() = "MSVC Exception: " + e.message();
				callContext.getResultCode()= -2;
		}
#endif
		catch(...) {
				callContext.getResultReason() = "Unknown exception";
				callContext.getResultCode() = -3;
		}

		// create response
		HlpStruct::convertCallingContextToXmlParameter(callContext, respxml);
		
		// always true to get the result
		return true;

	}
	
	
} // end of namespace