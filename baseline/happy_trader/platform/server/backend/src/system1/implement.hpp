#ifndef _SYSTEM1_IMPL_INCLUDED
#define _SYSTEM1_IMPL_INCLUDED


#include "system1defs.hpp"
#include "symbolcontext.hpp"
#include "commonparameters.hpp"

#define LOG_SYMBOL(RUN_NAME,PROVIDER, L,S,X) {	\
	DRAWOBJ_TEXT(RUN_NAME,CppUtils::getTime(),"SYSTEM 1", S, PROVIDER, L, "LOG", X );	\
}

#define LOG_SIGNAL(RUN_NAME,PROVIDER, L,S,X) {	\
	DRAWOBJ_TEXT(RUN_NAME,CppUtils::getTime(),"SYSTEM 1", S, PROVIDER, L, "SIGNAL", X );	\
}

#define LOG_COMMON(RUN_NAME,PROVIDER, L,X)	{		\
	DRAWOBJ_TEXT(RUN_NAME,CppUtils::getTime(),"SYSTEM 1", "-ALL-", PROVIDER, L, "LOG", X );	\
}

namespace AlgLib {
	//
	class SYSTEM1_EXP System1 :public AlgorithmBase {
		//
	public:
		//
		// ctor & dtor
		System1( Inqueue::AlgorithmHolder& algHolder);

		virtual ~System1();
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
			Inqueue::ContextAccessor& contextaccessor,
			HlpStruct::RtData::SynchType const synchFlg,
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
			Inqueue::ContextAccessor& contextaccessor, bool const firstLib, bool const lastLib
		)
		{
		}

		virtual CppUtils::String const& getName() const;

		// -----------------------
	public:

	private:
		void initSymbolContext(Inqueue::ContextAccessor& historyaccessor);

		void readInitialParameters(Inqueue::AlgBrokerDescriptor const& descriptor);

	private:

		map<CppUtils::String, SymbolContext> symbolContextMap_m;

		CppUtils::String internalRun_name_m;

		CommonParameters commonparams_m;

	};

}; // end of namespace 


#endif // _SYSTEM1_IMPL_INCLUDED
