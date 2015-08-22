#ifndef _SMARTCOM_BROKER_ALG_STUB_IMPL_INCLUDED
#define _SMARTCOM_BROKER_ALG_STUB_IMPL_INCLUDED

 // Many thanks to Zoubok Victor

#include "smartcom_broker_alg_stubdefs.hpp"
#define SMARTCOM_ALG_STUB "SMARTCOM_ALG_STUB"

namespace AlgLib {
	//
	class SMARTCOM_BROKER_ALG_STUB_EXP CSmartcomAlgStub :public AlgorithmBase {
		//
	public:
		//
		// ctor & dtor
		CSmartcomAlgStub( Inqueue::AlgorithmHolder& algHolder);

		virtual ~CSmartcomAlgStub();
		// intreface to be implemented

		virtual void onLibraryInitialized(
			Inqueue::AlgBrokerDescriptor const& descriptor, 
			BrkLib::BrokerBase* brokerlib, 
			CppUtils::String const& runName,
			CppUtils::String const& comment
		);

		virtual void onLibraryFreed();

		virtual void onDataArrived(
			HlpStruct::RtData const& rtdata, 
			Inqueue::ContextAccessor& historyaccessor, 
			Inqueue::TradingParameters const& descriptor,
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		);
		
	virtual void onRtProviderSynchEvent (
		Inqueue::ContextAccessor& contextaccessor, HlpStruct::RtData::SynchType const synchFlg,
		double const& synchTime,
		CppUtils::String const& providerName,
		CppUtils::String const& message
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

		virtual void onEventArrived(Inqueue::ContextAccessor& contextaccessor,
		HlpStruct::CallingContext& callContext,
		bool& result,
		CppUtils::String& customOutputData
	);

		virtual void onThreadStarted(
			Inqueue::ContextAccessor& contextaccessor, bool const firstLib, bool const lastLib
		);

	virtual void onThreadFinished(
		Inqueue::ContextAccessor& contextaccessor, Inqueue::TradingParameters const& tradeParams, bool const firstLib, bool const lastLib
	);



		virtual CppUtils::String const& getName() const;

		// -----------------------
	public:

	private:

	};

}; // end of namespace 


#endif // _SMARTCOM_BROKER_ALG_STUB_IMPL_INCLUDED
