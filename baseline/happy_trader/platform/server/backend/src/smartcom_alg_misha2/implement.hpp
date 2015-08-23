#ifndef _SMARTCOM_ALG_MISHA2_IMPL_INCLUDED
#define _SMARTCOM_ALG_MISHA2_IMPL_INCLUDED

 // Many thanks to Zoubok Victor

#include "smartcom_alg_misha2defs.hpp"
#include "event.hpp"
#include "state.hpp"
#include "action.hpp"
#include "brokerthroughcontext.hpp"
#include "tradeparams.hpp"
#include "brokerdialog.hpp"
#include "solverimpl.hpp"
#include "globaltardingdata.hpp"
#include "robot.hpp"

#define SMARTCOM_ALG_MISHA "SMARTCOM_ALG_MISHA2"
#define MAIN_FSM "Main FSM"


#define LOG_SYMBOL(RUN_NAME,PROVIDER, SYMBOL,X) 	\
	DRAWOBJ_TEXT(RUN_NAME,CppUtils::getTime(),SMARTCOM_ALG_MISHA, SYMBOL, PROVIDER, BrkLib::AP_MEDIUM, "LOG", X );	\

#define LOG_SYMBOL_SIGNAL(RUN_NAME,PROVIDER, SYMBOL,X) 	\
	DRAWOBJ_TEXT(RUN_NAME,CppUtils::getTime(),SMARTCOM_ALG_MISHA, SYMBOL, PROVIDER, BrkLib::AP_MEDIUM, "SIGNAL", X );	\

#define LOG_SYMBOL_PROBLEM(RUN_NAME,PROVIDER, SYMBOL,X) 	\
	DRAWOBJ_TEXT(RUN_NAME,CppUtils::getTime(),SMARTCOM_ALG_MISHA, SYMBOL, PROVIDER, BrkLib::AP_HIGH, "LOG", X );	\

#define LOG_COMMON(RUN_NAME,PROVIDER,LEVEL,X) 	\
	DRAWOBJ_TEXT(RUN_NAME,CppUtils::getTime(),SMARTCOM_ALG_MISHA, "-ALL-", PROVIDER, LEVEL, "LOG", X );	\


// helper to create state



namespace AlgLib {
	//

	// -------------------------


	class SMARTCOM_ALG_MISHA2_EXP CAlgMisha2 :public AlgorithmBase {
		//
	public:
	
		//
		// ctor & dtor
		CAlgMisha2( Inqueue::AlgorithmHolder& algHolder);

		virtual ~CAlgMisha2();
		// intreface to be implemented

		virtual void onLibraryInitialized(
			HlpStruct::AlgBrokerDescriptor const& descriptor, 
			BrkLib::BrokerBase* brokerlib, 
			BrkLib::BrokerBase* brokerlib2,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		);

		virtual void onLibraryFreed();

		virtual void onDataArrived(
			HlpStruct::RtData const& rtdata, 
			Inqueue::ContextAccessor& historyaccessor, 
			HlpStruct::TradingParameters const& descriptor,
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
			HlpStruct::TradingParameters const& descriptor,
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		);

		virtual void onLevel1DataArrived(
			HlpStruct::RtLevel1Data const& level1data,
			Inqueue::ContextAccessor& historyaccessor, 
			HlpStruct::TradingParameters const& descriptor,
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
			Inqueue::ContextAccessor& contextaccessor, HlpStruct::TradingParameters const& tradeParams, bool const firstLib, bool const lastLib
		);



		virtual CppUtils::String const& getName() const;

		// -----------------------
	public:
		


		inline CppUtils::String const& getRtDataProvider() const
		{
			return rtDataProvider_m;
		}

		inline CppUtils::String const& getBarDataProvider() const
		{
			return barDataProvider_m;
		}

		inline CppUtils::String const& getRunName() const
		{
			return connectRunName_m;
		}

		inline TradingSequenceRobot& getRobot()
		{
			return robot_m;
		}
	
		// this function implement event processing in a separate thread to avoid recursion
		void helperThreadFun();
	


	private:

		void exportSynthetic(Inqueue::ContextAccessor& historyaccessor);	

		void exportAll(Inqueue::ContextAccessor& historyaccessor);

		void exportSymbol(Inqueue::ContextAccessor& historyaccessor, CppUtils::String const& symbol, int const cache_id);

		void exportSymbol(Inqueue::ContextAccessor& historyaccessor, CppUtils::String const& symbol);

		


		// helper thread
		// -
		CppUtils::Thread* thread_m;

		// event whether thread started
		CppUtils::Event threadStarted_m;

		// signal to shutdown
		CppUtils::Event shutDownThread_m;
		// --

		
		CppUtils::String historyDataProfile_m;

		CppUtils::String rtDataProvider_m;

		CppUtils::String barDataProvider_m;

		BrkLib::TradeDirection allowTrading_m;
		
		
		CppUtils::String connectRunName_m;

		CppUtils::String baseExportPath_m;

		
		
		// list of processed symbols
		CppUtils::StringSet symbols_m;

		//
		TradingSequenceRobot robot_m;



	};

}; // end of namespace 


#endif // _SMARTCOM_ALG_MISHA_IMPL_INCLUDED
