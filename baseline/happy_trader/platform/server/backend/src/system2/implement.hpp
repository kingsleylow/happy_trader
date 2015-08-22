#ifndef _SYSTEM2_IMPL_INCLUDED
#define _SYSTEM2_IMPL_INCLUDED


#include "system2defs.hpp"
#include "symbolcontext.hpp"
#include "commonparameters.hpp"

#define LOG_SYMBOL(RUN_NAME,PROVIDER, L,S,X) {	\
	DRAWOBJ_TEXT(RUN_NAME,CppUtils::getTime(),"SYSTEM 2", S, PROVIDER, L, "LOG", X );	\
}

#define LOG_SIGNAL(RUN_NAME,PROVIDER, L,S,X) {	\
	DRAWOBJ_TEXT(RUN_NAME,CppUtils::getTime(),"SYSTEM 2", S, PROVIDER, L, "SIGNAL", X );	\
}

#define LOG_COMMON(RUN_NAME,PROVIDER, L,X)	{		\
	DRAWOBJ_TEXT(RUN_NAME,CppUtils::getTime(),"SYSTEM 2", "-ALL-", PROVIDER, L, "LOG", X );	\
}


namespace AlgLib {
	//
	class SYSTEM2_EXP CSystem2 :public AlgorithmBase {
		//
	public:
		//
		// ctor & dtor
		CSystem2( Inqueue::AlgorithmHolder& algHolder);

		virtual ~CSystem2();
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
			Inqueue::ContextAccessor& contextaccessor, bool const firstLib, bool const lastLib
		)
		{
		}

		virtual CppUtils::String const& getName() const;




		// -----------------------
	public:

	private:

		map<CppUtils::String, SymbolContext> symbolContextMap_m;

		CppUtils::String internalRun_name_m;

		CommonParameters commonparams_m;


	private:
		

		void readInitialParameters(Inqueue::AlgBrokerDescriptor const& descriptor);


		void initSymbolContext(Inqueue::ContextAccessor& historyaccessor);

		void openLongPos(BrkLib::BrokerBase* brokerlib, 
			CppUtils::String const& runName, 
			CppUtils::String const& comment,
			CppUtils::String const& provider,
			CppUtils::String const& symbol,
			double const& orderTime,
			int const posTag,
			double const& tpPrice
		);

		void openShortPos(BrkLib::BrokerBase* brokerlib, 
			CppUtils::String const& runName, 
			CppUtils::String const& comment,
			CppUtils::String const& provider,
			CppUtils::String const& symbol,
			double const& orderTime,
			int const posTag,
			double const& tpPrice
		);

		void closePos(BrkLib::BrokerBase* brokerlib, 
			CppUtils::String const& runName, 
			CppUtils::String const& comment,
			CppUtils::String const& provider,
			CppUtils::String const& symbol,
			double const& orderTime
		);

	

	};

}; // end of namespace 


#endif // _SYSTEM2_IMPL_INCLUDED
