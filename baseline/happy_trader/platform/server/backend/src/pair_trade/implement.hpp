#ifndef _PAIR_TRADE_IMPL_INCLUDED
#define _PAIR_TRADE_IMPL_INCLUDED

 // Many thanks to Zoubok Victor

#include "pair_tradedefs.hpp"
#include "event.hpp"
#include "state.hpp"
#include "action.hpp"
#include "brokerthroughcontext.hpp"
#include "tradeparams.hpp"
#include "brokerdialog.hpp"
#include "solverimpl.hpp"
#include "globaltardingdata.hpp"
#include "robot.hpp"
#include "commonstruct.hpp"

//#include "include/muParser.h"

#define PAIR_TRADE "PAIR_TRADE"
#define MAIN_FSM "Main FSM"


#define LOG_SYMBOL(RUN_NAME,PROVIDER, SYMBOL,X) 	\
	DRAWOBJ_TEXT(RUN_NAME,CppUtils::getTime(),PAIR_TRADE, SYMBOL, PROVIDER, BrkLib::AP_MEDIUM, "LOG", X );	\

#define LOG_SYMBOL_SIGNAL(RUN_NAME,PROVIDER, SYMBOL,X) 	\
	DRAWOBJ_TEXT(RUN_NAME,CppUtils::getTime(),PAIR_TRADE, SYMBOL, PROVIDER, BrkLib::AP_MEDIUM, "SIGNAL", X );	\

#define LOG_SYMBOL_PROBLEM(RUN_NAME,PROVIDER, SYMBOL,X) 	\
	DRAWOBJ_TEXT(RUN_NAME,CppUtils::getTime(),PAIR_TRADE, SYMBOL, PROVIDER, BrkLib::AP_HIGH, "LOG", X );	\

#define LOG_COMMON(RUN_NAME,PROVIDER,LEVEL,X) 	\
	DRAWOBJ_TEXT(RUN_NAME,CppUtils::getTime(),PAIR_TRADE, "-ALL-", PROVIDER, LEVEL, "LOG", X );	\


// helper to create state



namespace AlgLib {
	//


			
	// -------------------------


	class PAIR_TRADE_EXP CPairTrade :public AlgorithmBase {
		//
	public:

		

		static CppUtils::String const& getRootObjectName();
		//
		// ctor & dtor
		CPairTrade( Inqueue::AlgorithmHolder& algHolder);

		virtual ~CPairTrade();
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
		


		inline CppUtils::String const& getRtdataProvider_Smartcom() const
		{
			return rtDataProvider_Smartcom_m;
		}

		inline CppUtils::String const& getBarDataProvider_Smartcom() const
		{
			return barDataProvider_Smartcom_m;
		}

		
		inline CppUtils::String const& getBarDataProvider_Finam() const
		{
			return barDataProvider_Finam_m;
		}
		

		inline CppUtils::String const& getRunName() const
		{
			return connectRunName_m;
		}

		inline CppUtils::String const& getBaseExportPath() const
		{
			return baseExportPath_m;
		}

	
		
		// this function implement event processing in a separate thread to avoid recursion
		void helperThreadFun();

		inline TradingSequenceRobot& getRobot()
		{
			return robot_m;
		}
	


	private:

		void exportSynthetic(Inqueue::ContextAccessor& historyaccessor,CppUtils::String const& expression_s);	

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

		CppUtils::String rtDataProvider_Smartcom_m;

		CppUtils::String barDataProvider_Smartcom_m;

		CppUtils::String barDataProvider_Finam_m;

				

		CppUtils::String connectRunName_m;

		CppUtils::String baseExportPath_m;

		// trading robot - e.i. lightweight algorithm with own set of parameters
		TradingSequenceRobot robot_m;



	};

}; // end of namespace 


#endif // _SMARTCOM_ALG_MISHA_IMPL_INCLUDED
