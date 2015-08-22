#include "metacommandproc.hpp"
#include "implement.hpp"
#include "statemachine.hpp"

#define METACOMMANDPROCESSOR "METACOMMANDPROCESSOR"

namespace AlgLib {


int bootServingThread (void *ptr)
{
		// handle run-time exceptions
#ifdef HANDLE_NATIVE_EXCEPTIONS
			Testframe::init();
#endif

			
		MetacommandProcessor* pThis = (MetacommandProcessor*)ptr; 
		try {
			pThis->threadServingThread();
		}
		catch(CppUtils::Exception& e) {
			LOG(MSG_FATAL, METACOMMANDPROCESSOR, "Serving thread fatal exception: " << e.message());
		}
#ifdef HANDLE_NATIVE_EXCEPTIONS
		catch(Testframe::Exception& e) {
			LOG(MSG_FATAL, METACOMMANDPROCESSOR, "Serving thread fatal MSVC exception: " << e.message());
		}
#endif
		catch(...) {
			LOG(MSG_FATAL, METACOMMANDPROCESSOR, "Serving thread unknown fatal exception");
		}
		return 42;

}


	MetacommandProcessor::MetacommandProcessor(MRangeBreak& baserangeBreaker):
		baserangeBreaker_m(baserangeBreaker)
	{
		servingRobotThread_m = new CppUtils::Thread(bootServingThread, this);

		if (!servingThreadStarted_m.lock(10000))
			THROW(CppUtils::OperationFailed, "exc_CannorStartProcessingThread", "ctx_MetacommandProcessorCtor", "");

	}

	MetacommandProcessor::~MetacommandProcessor()
	{
		shutDownServingThread_m.signalEvent();
		servingRobotThread_m->waitForThread();
		delete servingRobotThread_m;
		servingRobotThread_m = NULL;

		LOG(MSG_INFO, METACOMMANDPROCESSOR, "deinitialized");
	}

	void MetacommandProcessor::threadServingThread()
	{
		servingThreadStarted_m.signalEvent();

		LOG(MSG_INFO, METACOMMANDPROCESSOR, "Serving thread started");


		CppUtils::NativeEvent events[] = 
		{
			shutDownServingThread_m.getNativeEvent(),				// 0	
			servingThreadStartActivity_m.getNativeEvent()

		};

		while(true) {


			try {
				// wait some of them 
				int res = CppUtils::Event::lockForSomeEvents(events, 1);

				if (res==0) {
					// shutdown
					break;
				}
				else if (res==1) {
					

					// event arrived
					MetacommandProcessorStructure procstruct;
					{
						LOCK_FOR_SCOPE(receivedProcessingDataMtx_m);
						if (!receivedProcessingData_m.isEmpty()) {
								
							// copy event data - need to propagate furher, do not want to block
							procstruct = receivedProcessingData_m.get_front();
							receivedProcessingData_m.delete_front();
								
						};
								
						// reset event
						receivedProcessingDataArrived_m.clearEvent();

					}

					// process command
					LOG_COMMON(baserangeBreaker_m.getInternalRunName(), baserangeBreaker_m.getProviderRTSymbols(), BrkLib::AP_MEDIUM, "Command of type will be processed: " << procstruct.type_m.getName());


					CppUtils::Uid machine_uid(procstruct.param_m.getParameter("machine_uid_str").getAsString());
					CppUtils::String const& machine_descr = procstruct.param_m.getParameter("machine_descr").getAsString();
					
					CppUtils::String const& orderSignStr = procstruct.param_m.getParameter("order_sign_str").getAsString();
					CppUtils::String const& orderTPSignStr = procstruct.param_m.getParameter("order_tp_sign_str").getAsString();
					CppUtils::String const& orderSLSignStr = procstruct.param_m.getParameter("order_sl_sign_str").getAsString();

					// make actual call to state machine passing necessary event
					onNewMetacommandArrived(
						machine_uid, 
						machine_descr,
						procstruct.type_m,
						orderSignStr,
						orderTPSignStr,
						orderSLSignStr);

				}

			}
			catch(CppUtils::Exception& e) {
				LOG_COMMON(baserangeBreaker_m.getInternalRunName(), baserangeBreaker_m.getProviderRTSymbols(), BrkLib::AP_MEDIUM, "Error in serving thread: " << e.message());
			}
#ifdef HANDLE_NATIVE_EXCEPTIONS
			catch(Testframe::Exception& e) {
				LOG_COMMON(baserangeBreaker_m.getInternalRunName(), baserangeBreaker_m.getProviderRTSymbols(), BrkLib::AP_MEDIUM, "MSVC error in serving thread: " << e.message());
			}
#endif
			catch(...) {
				LOG_COMMON(baserangeBreaker_m.getInternalRunName(), baserangeBreaker_m.getProviderRTSymbols(), BrkLib::AP_MEDIUM,  "Unknown error in serving thread");
			}

		} // end loop

		LOG(MSG_INFO, METACOMMANDPROCESSOR, "Serving thread finished");


	}

	void MetacommandProcessor::runBreakThroughUp(HlpStruct::XmlParameter const& param_data)
	{
		issueMetaCommand(param_data, CT_BREAKTHROUGH_UP);
	}

	void MetacommandProcessor::runBreakThroughDown(HlpStruct::XmlParameter const& param_data)
	{
		issueMetaCommand(param_data, CT_BREAKTHROUGH_DOWN);
	}


	void MetacommandProcessor::issueMetaCommand(HlpStruct::XmlParameter const& param_data, CppUtils::EnumerationHelper const& commandType )
	{
		MetacommandProcessorStructure procStruct(commandType, param_data);
	
		{
			LOCK_FOR_SCOPE(receivedProcessingDataMtx_m);
		
			receivedProcessingData_m.push_back(procStruct);
			receivedProcessingDataArrived_m.signalEvent();
		}
	}

	void MetacommandProcessor::onNewMetacommandArrived(
		CppUtils::Uid const& machineUid, 
		CppUtils::String const& description,
		CppUtils::EnumerationHelper const& commandType,
		CppUtils::String const& orderSignStr,
		CppUtils::String const& orderTPSignStr,
		CppUtils::String const& orderSLSignStr
	)
	{
		LOCK_FOR_SCOPE(metacommandMachinesMtx_m);

		MetacommandStateMachine newMachine;
		newMachine.initialize(machineUid, this, description);
		metacommandMachines_m[machineUid] = newMachine;

		// send this event
		MetacommandEvent mte(machineUid, MAE_IssueOrder_Main);

		mte.context().commandType_m = commandType;
		mte.context().orderSignStr_m = orderSignStr;
		mte.context().orderTPSignStr_m = orderTPSignStr;
		mte.context().orderSLSignStr_m = orderSLSignStr;

		machineEventReaction(mte);

	}

	void MetacommandProcessor::machineEventReaction(MetacommandEvent const& event_i)
	{
		LOCK_FOR_SCOPE(metacommandMachinesMtx_m);

		map<CppUtils::Uid, MetacommandStateMachine>::iterator it = metacommandMachines_m.find(event_i.getRecipientUid());
		if (it == metacommandMachines_m.end()) {
			return;
		}

		// process
		MetacommandStateMachine &target_machine = it->second;
		target_machine.newEventArrived(event_i);
	}

	MetacommandAction MetacommandProcessor::resolveMachineAction(CppUtils::Uid const& uid)
	{
		LOCK_FOR_SCOPE(metacommandMachinesMtx_m);

		map<CppUtils::Uid, MetacommandStateMachine>::const_iterator it = metacommandMachines_m.find(uid);

		// return dummy action
		if (it == metacommandMachines_m.end()) 
			return MetacommandAction(MAS_Dummy);

		// return a copy
		return it->second.getLastAction();

	
	}

	MetacommandState MetacommandProcessor::resolveMachineState(CppUtils::Uid const& uid)
	{
		LOCK_FOR_SCOPE(metacommandMachinesMtx_m);

		map<CppUtils::Uid, MetacommandStateMachine>::const_iterator it = metacommandMachines_m.find(uid);

		// return dummy action
		if (it == metacommandMachines_m.end()) 
			return MetacommandState(MCS_Dummy);

		// return a copy
		return it->second.getLastState();
	}

	/**
	* Notifications
	*/

	void MetacommandProcessor::onMachineConfirmOrderIssued(CppUtils::Uid const& machineUid, CppUtils::Uid const& orderUid, OrderDescriptor const& orderDescriptor)
	{
		LLOG(baserangeBreaker_m.getInternalRunName(), baserangeBreaker_m.getProviderRTSymbols(),machineUid, orderUid, "Order issued: "<< orderDescriptor.toString());
	}

	void MetacommandProcessor::onMachineConfirmOrderPending(CppUtils::Uid const& machineUid, CppUtils::Uid const& orderUid, OrderDescriptor const& orderDescriptor)
	{
		LLOG(baserangeBreaker_m.getInternalRunName(), baserangeBreaker_m.getProviderRTSymbols(),machineUid, orderUid, "Order pending: "<< orderDescriptor.toString());
		MetacommandState curState = resolveMachineState(machineUid);

		NEW_STATE_BEGIN(curState.getMetacommandState())
				
			NEW_STATE_ENTRY_BEGIN(MCS_Order_TP_Issued)
				MetacommandEvent mte(machineUid, MAE_Order_TP_Pending);
				machineEventReaction(mte);

			NEW_STATE_ENTRY_END();

			NEW_STATE_ENTRY_BEGIN(MCS_Order_SL_Issued)
				MetacommandEvent mte(machineUid, MAE_Order_SL_Pending);
				machineEventReaction(mte);
			NEW_STATE_ENTRY_END();

		NEW_STATE_END();

		
	}

	void MetacommandProcessor::onMachineConfirmOrderDelayed(CppUtils::Uid const& machineUid, CppUtils::Uid const& orderUid, OrderDescriptor const& orderDescriptor)
	{
		LLOG(baserangeBreaker_m.getInternalRunName(), baserangeBreaker_m.getProviderRTSymbols(),machineUid, orderUid, "Order issued delayed: "<< orderDescriptor.toString());
	}

	void MetacommandProcessor::onMachineConfirmOrderOpened(CppUtils::Uid const& machineUid, CppUtils::Uid const& orderUid, OrderDescriptor const& orderDescriptor)
	{
		LLOG(baserangeBreaker_m.getInternalRunName(), baserangeBreaker_m.getProviderRTSymbols(),machineUid, orderUid, "Order opened: "<< orderDescriptor.toString());
		MetacommandState curState = resolveMachineState(machineUid);

		NEW_STATE_BEGIN(curState.getMetacommandState())
				
			NEW_STATE_ENTRY_BEGIN(MCS_Order_Main_Issued)
				MetacommandEvent mte(machineUid, MAE_Order_Main_Open);
				machineEventReaction(mte);

			NEW_STATE_ENTRY_END();

			

		NEW_STATE_END();
	}

	void MetacommandProcessor::onMachineConfirmOrderCancelled(CppUtils::Uid const& machineUid, CppUtils::Uid const& orderUid, OrderDescriptor const& orderDescriptor)
	{
		LLOG(baserangeBreaker_m.getInternalRunName(), baserangeBreaker_m.getProviderRTSymbols(),machineUid, orderUid, "Order cancelled: "<< orderDescriptor.toString());
		MetacommandState curState = resolveMachineState(machineUid);

		NEW_STATE_BEGIN(curState.getMetacommandState())
				
			NEW_STATE_ENTRY_BEGIN(MCS_Order_Main_Issued)
				MetacommandEvent mte(machineUid, MAE_Order_Main_Cancel);
				machineEventReaction(mte);

			NEW_STATE_ENTRY_END();

			NEW_STATE_ENTRY_BEGIN(MCS_Order_TP_Issued)
				MetacommandEvent mte(machineUid, MAE_Order_TP_Cancel);
				machineEventReaction(mte);
			NEW_STATE_ENTRY_END();

			NEW_STATE_ENTRY_BEGIN(MCS_Order_SL_Issued)
				MetacommandEvent mte(machineUid, MAE_Order_SL_Cancel);
				machineEventReaction(mte);
			NEW_STATE_ENTRY_END();

		NEW_STATE_END();
	}

	void MetacommandProcessor::onMachineConfirmErrorReturned(CppUtils::Uid const& machineUid, CppUtils::Uid const& orderUid, OrderDescriptor const& orderDescriptor)
	{
		LLOG(baserangeBreaker_m.getInternalRunName(), baserangeBreaker_m.getProviderRTSymbols(),machineUid, orderUid, "Order error: "<< orderDescriptor.toString());
		MetacommandState curState = resolveMachineState(machineUid);

		NEW_STATE_BEGIN(curState.getMetacommandState())
				
			NEW_STATE_ENTRY_BEGIN(MCS_Order_Main_Issued)
				MetacommandEvent mte(machineUid, MAE_Order_Main_Error);
				machineEventReaction(mte);

			NEW_STATE_ENTRY_END();

			NEW_STATE_ENTRY_BEGIN(MCS_Order_TP_Issued)
				MetacommandEvent mte(machineUid, MAE_Order_TP_Error);
				machineEventReaction(mte);
			NEW_STATE_ENTRY_END();

			NEW_STATE_ENTRY_BEGIN(MCS_Order_SL_Issued)
				MetacommandEvent mte(machineUid, MAE_Order_SL_Error);
				machineEventReaction(mte);
			NEW_STATE_ENTRY_END();

		NEW_STATE_END();

	}

	// from metacommand machine
	void MetacommandProcessor::onStateChangedNotification(CppUtils::Uid const& machineUid, MetacommandState const& oldState, MetacommandState const& newState)
	{
		LLOG_META(baserangeBreaker_m.getInternalRunName(), baserangeBreaker_m.getProviderRTSymbols(),machineUid, "state changed: ["<< oldState.getMetacommandState().getName() << "] -> [" << newState.getMetacommandState().getName() << "]");
		
	}

	void MetacommandProcessor::ordermachine_Force_TE_InstallOrder(CppUtils::String const& orderSignallerString, CppUtils::Uid const& machineUid)
	{
		baserangeBreaker_m.ordermachine_Force_TE_InstallOrder(orderSignallerString, machineUid);
	}


}; // end of namespace