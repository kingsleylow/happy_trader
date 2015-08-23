#include "robot.hpp"
#include "implement.hpp"

namespace AlgLib {
	TradingSequenceRobot::TradingSequenceRobot(CAlgMisha2& base):
		base_m(base),
		calculatorLevel_m(*this)
	{
		uid_m.generate();
	}

	TradingSequenceRobot::~TradingSequenceRobot()
	{
	}

	void TradingSequenceRobot::initStateMachine(CppUtils::StringSet const& symbols)
	{
		for(CppUtils::StringSet::const_iterator it = symbols.begin(); it != symbols.end(); it++) {
			MachineContext::MachineContextStructure ctx(*it, base_m, *this, MAIN_FSM);
			// machine managing basse algorithm
			CppUtils::StateMachine<MachineContext> *m = new CppUtils::StateMachine<MachineContext>(&ctx);
			tradingMachines_m.getMachineMap()[*it] = m;
		
		}

		// main FSM
		for(map<CppUtils::String, CppUtils::StateMachine<MachineContext>* >::iterator it = tradingMachines_m.getMachineMap().begin(); it != tradingMachines_m.getMachineMap().end(); it++) {
			// register states
			CppUtils::StateMachine<MachineContext> & machine = *it->second;

				


// --------------------------------------------
// BEGIN AUTOGENERATED BLOCK
// --------------------------------------------

//INIT MACHINES 
machine.add<TS_Ready_Cancel_Limit_TP>();
machine.add<TS_Both_Pending>();
machine.add<TS_Broker_Error>();
machine.add<TS_Wait_Market_Close>();
machine.add<TS_No_Position>();
machine.add<TS_Position_Opened>();
machine.add<TS_Initial>();
machine.add<TS_Ready_Cancel_Limit>();
machine.add<TS_Mismatched_Order_Execution>();
machine.add<TS_Wait_Close_Pos>();
machine.add<TS_Ready_Issue_Stop>();
machine.add<TS_Ready_Cancel_Stop>();
machine.add<TS_Ready_Issue_Limit>();
machine.add<TS_Ready_Market_Close>();
machine.add<TS_Error>();
machine.add<TS_Ready_Cancel_Stop_Market_Exit>();
machine.add<TS_Ready_Cancel_Stop_TP_Exit>();
//------------------------ 
//TS_Ready_Cancel_Limit_TP ------------------------ 
machine.state<TS_Ready_Cancel_Limit_TP>().connect<TE_Limit_TP_Cancelled>( machine.state<TS_Initial>() );
machine.state<TS_Ready_Cancel_Limit_TP>().connect<TE_Broker_Error>( machine.state<TS_Broker_Error>() );

machine.state<TS_Ready_Cancel_Limit_TP>().add_entry_action<A_Cancel_Limit_TP>();
//--------------------------- 
//TS_Both_Pending ------------------------ 
machine.state<TS_Both_Pending>().connect<TE_Level_Lost>( machine.state<TS_Ready_Cancel_Stop>() );
machine.state<TS_Both_Pending>().connect<TE_Limit_Executed>( machine.state<TS_Position_Opened>() );
machine.state<TS_Both_Pending>().connect<TE_Stop_Executed>( machine.state<TS_Error>() );

//--------------------------- 
//TS_Broker_Error ------------------------ 

machine.state<TS_Broker_Error>().add_exit_action<AE_Broker_Error>();
//--------------------------- 
//TS_Wait_Market_Close ------------------------ 
machine.state<TS_Wait_Market_Close>().connect<TE_Market_Close_Executed>( machine.state<TS_Ready_Cancel_Stop_Market_Exit>() );
machine.state<TS_Wait_Market_Close>().connect<TE_Stop_Executed>( machine.state<TS_Mismatched_Order_Execution>() );
machine.state<TS_Wait_Market_Close>().connect<TE_Limit_TP_Executed>( machine.state<TS_Mismatched_Order_Execution>() );

//--------------------------- 
//TS_No_Position ------------------------ 
machine.state<TS_No_Position>().connect<TE_Level_Detected>( machine.state<TS_Ready_Issue_Limit>() );

//--------------------------- 
//TS_Position_Opened ------------------------ 
machine.state<TS_Position_Opened>().connect<TE_Limit_TP_Pending>( machine.state<TS_Wait_Close_Pos>() );
machine.state<TS_Position_Opened>().connect<TE_Broker_Error>( machine.state<TS_Broker_Error>() );

machine.state<TS_Position_Opened>().add_entry_action<A_Issue_Limit_TP>();
//--------------------------- 
//TS_Initial ------------------------ 
machine.state<TS_Initial>().connect<TE_Unconditional>( machine.state<TS_No_Position>() );

machine.state<TS_Initial>().add_entry_action<A_Initial_Enter>();
//--------------------------- 
//TS_Ready_Cancel_Limit ------------------------ 
machine.state<TS_Ready_Cancel_Limit>().connect<TE_Limit_Cancelled>( machine.state<TS_Initial>() );
machine.state<TS_Ready_Cancel_Limit>().connect<TE_Broker_Error>( machine.state<TS_Broker_Error>() );

machine.state<TS_Ready_Cancel_Limit>().add_entry_action<A_Cancel_Limit>();
//--------------------------- 
//TS_Mismatched_Order_Execution ------------------------ 
machine.state<TS_Mismatched_Order_Execution>().connect<TE_Unconditional>( machine.state<TS_Error>() );

machine.state<TS_Mismatched_Order_Execution>().add_entry_action<A_Mismatched_Execution_Enter>();
machine.state<TS_Mismatched_Order_Execution>().add_exit_action<AE_Mismatched_Execution>();
//--------------------------- 
//TS_Wait_Close_Pos ------------------------ 
machine.state<TS_Wait_Close_Pos>().connect<TE_Level_Lost>( machine.state<TS_Ready_Market_Close>() );
machine.state<TS_Wait_Close_Pos>().connect<TE_Stop_Executed>( machine.state<TS_Ready_Cancel_Limit_TP>() );
machine.state<TS_Wait_Close_Pos>().connect<TE_Limit_TP_Executed>( machine.state<TS_Ready_Cancel_Stop_TP_Exit>() );

//--------------------------- 
//TS_Ready_Issue_Stop ------------------------ 
machine.state<TS_Ready_Issue_Stop>().connect<TE_Stop_Pending>( machine.state<TS_Both_Pending>() );
machine.state<TS_Ready_Issue_Stop>().connect<TE_Level_Lost>( machine.state<TS_Ready_Cancel_Limit>() );
machine.state<TS_Ready_Issue_Stop>().connect<TE_Broker_Error>( machine.state<TS_Broker_Error>() );

machine.state<TS_Ready_Issue_Stop>().add_entry_action<A_Issue_Stop>();
//--------------------------- 
//TS_Ready_Cancel_Stop ------------------------ 
machine.state<TS_Ready_Cancel_Stop>().connect<TE_Stop_Cancelled>( machine.state<TS_Ready_Cancel_Limit>() );
machine.state<TS_Ready_Cancel_Stop>().connect<TE_Broker_Error>( machine.state<TS_Broker_Error>() );

machine.state<TS_Ready_Cancel_Stop>().add_entry_action<A_Cancel_Stop>();
//--------------------------- 
//TS_Ready_Issue_Limit ------------------------ 
machine.state<TS_Ready_Issue_Limit>().connect<TE_Limit_Pending>( machine.state<TS_Ready_Issue_Stop>() );
machine.state<TS_Ready_Issue_Limit>().connect<TE_Broker_Error>( machine.state<TS_Broker_Error>() );

machine.state<TS_Ready_Issue_Limit>().add_entry_action<A_Issue_Limit>();
//--------------------------- 
//TS_Ready_Market_Close ------------------------ 
machine.state<TS_Ready_Market_Close>().connect<TE_Market_Close_Pending>( machine.state<TS_Wait_Market_Close>() );
machine.state<TS_Ready_Market_Close>().connect<TE_Broker_Error>( machine.state<TS_Broker_Error>() );

machine.state<TS_Ready_Market_Close>().add_entry_action<A_Issue_Market_Close>();
//--------------------------- 
//TS_Error ------------------------ 

machine.state<TS_Error>().add_exit_action<AE_Error>();
//--------------------------- 
//TS_Ready_Cancel_Stop_Market_Exit ------------------------ 
machine.state<TS_Ready_Cancel_Stop_Market_Exit>().connect<TE_Stop_Cancelled_Market_Exit>( machine.state<TS_Ready_Cancel_Limit_TP>() );
machine.state<TS_Ready_Cancel_Stop_Market_Exit>().connect<TE_Broker_Error>( machine.state<TS_Broker_Error>() );

machine.state<TS_Ready_Cancel_Stop_Market_Exit>().add_entry_action<A_Cancel_Stop_Market_Exit>();
//--------------------------- 
//TS_Ready_Cancel_Stop_TP_Exit ------------------------ 
machine.state<TS_Ready_Cancel_Stop_TP_Exit>().connect<TE_Stop_Cancelled_TP_Exit>( machine.state<TS_Initial>() );

machine.state<TS_Ready_Cancel_Stop_TP_Exit>().add_entry_action<A_Cancel_Stop_TP_Exit>();
//--------------------------- 


// --------------------------------------------
// END AUTOGENERATED BLOCK
// --------------------------------------------
// initial state - add manually
machine.add<CppUtils::InitialState<MachineContext> >();
machine.state<CppUtils::InitialState<MachineContext> >().connect(machine.state<TS_Initial>(), CppUtils::Unconditional());

			machine.init();
			
			

		}

	}

	void TradingSequenceRobot::deinitStateMachine()
	{
		tradingMachines_m.clear();

		brokerDialogReal_m.deinit();
		brokerDialogDemo_m.deinit();
	}

	void TradingSequenceRobot::init(
		HlpStruct::AlgBrokerDescriptor const& descriptor,
		BrkLib::BrokerBase* brokerlib, 
		CppUtils::String const& runName,
		CppUtils::String const& comment,
		TradeParam const& tradeParam, 
		CppUtils::StringSet const& symbols
	)
	{
		tradeParam_m = tradeParam;
		initStateMachine(symbols);

		// connect to broker
		
		brokerDialogReal_m.initAndConnect(  brokerlib, CppUtils::String(SMARTCOM_ALG_MISHA) + "_real", runName, comment, base_m.getRtDataProvider() );
	
		if (!brokerDialogReal_m.isConnected()) {
			LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  SMARTCOM_ALG_MISHA, "onLibraryInitialized(...) session cannot be established: " << brokerDialogReal_m.getConnectString());
		}

		// broker connect 2
		
		brokerDialogDemo_m.initAndConnect(  base_m.getAlgHolder().getBroker2(), CppUtils::String(SMARTCOM_ALG_MISHA) + "_simul", runName, comment, base_m.getRtDataProvider() );
		if (!brokerDialogDemo_m.isConnected()) {
			LOGEVENT(HlpStruct::CommonLog::LL_ERROR,  SMARTCOM_ALG_MISHA, "onLibraryInitialized(...) session with broker lib 2 cannot be established: " << brokerDialogDemo_m.getConnectString());
		}


		if (tradeParam_m.isInvalid())
			THROW(CppUtils::OperationFailed, "exc_TradeParamsInvalid", "ctx_onLibraryInitialized", tradeParam_m.toDescriptionString() );

		LOG_COMMON(base_m.getRunName(), base_m.getRtDataProvider(), BrkLib::AP_MEDIUM, "Trading parameters: " << tradeParam_m.toDescriptionString());

	}

	void TradingSequenceRobot::deinit()
	{
		deinitStateMachine();
	}
	
		
		// ------------

	

	

	void TradingSequenceRobot::onDataArrived(
				HlpStruct::RtData const& rtdata, 
				Inqueue::ContextAccessor& historyaccessor, 
				HlpStruct::TradingParameters const& descriptor,
				BrkLib::BrokerBase* brokerlib,
				CppUtils::String const& runName,
				CppUtils::String const& comment
			)
	{
		

		if (rtdata.getProvider2() == base_m.getRtDataProvider()) {
			
			calculatorLevel_m.onRtData(rtdata, NULL);
		}
	}
			
	void TradingSequenceRobot::onRtProviderSynchEvent (
				Inqueue::ContextAccessor& contextaccessor, HlpStruct::RtData::SynchType const synchFlg,
				double const& synchTime,
				CppUtils::String const& providerName,
				CppUtils::String const& message
			)
	{
	}

	void TradingSequenceRobot::onLevel2DataArrived(
				HlpStruct::RtLevel2Data const& level2data,
				Inqueue::ContextAccessor& historyaccessor, 
				HlpStruct::TradingParameters const& descriptor,
				BrkLib::BrokerBase* brokerlib,
				CppUtils::String const& runName,
				CppUtils::String const& comment
			)
	{
		if (level2data.getProvider2() == base_m.getRtDataProvider()) {
			calculatorLevel_m.onLevel2Data(level2data, NULL);
		}
	}

	void TradingSequenceRobot::onLevel1DataArrived(
				HlpStruct::RtLevel1Data const& level1data,
				Inqueue::ContextAccessor& historyaccessor, 
				HlpStruct::TradingParameters const& descriptor,
				BrkLib::BrokerBase* brokerlib,
				CppUtils::String const& runName,
				CppUtils::String const& comment
			)
	{
	}

	BrokerDialog& TradingSequenceRobot::resolveBrokerDialog()
	{
		return tradeParam_m.isDemoTrade_m ? brokerDialogDemo_m:brokerDialogReal_m;
	}

	
	
	



	

}; // end of ns