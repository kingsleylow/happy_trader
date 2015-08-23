#include "metacommand_machine.hpp"
#include "statemachine.hpp"

namespace AlgLib {
	MetacommandStateMachine::MetacommandStateMachine():
		listenerP_m(NULL)
	{
	}

	MetacommandStateMachine::~MetacommandStateMachine()
	{
	}

	void MetacommandStateMachine::initialize(CppUtils::Uid const& uid, MetacommandStateMachineListener* listener, CppUtils::String const& description)
	{
		machineID_m = uid;
		description_m = description;
		listenerP_m = listener;
		creationLocalTime_m = CppUtils::getTime();

		lastState_m = MCS_Initial;
		lastAction_m.clear();
	}

	void MetacommandStateMachine::newEventArrived(MetacommandEvent const& event_i)
	{
		MetacommandState newState = lastState_m;

		OLD_STATE_BEGIN(lastState_m.getMetacommandState())
			// --------
			OLD_STATE_ENTRY_BEGIN(MCS_Initial)

				NEW_EVENT_BEGIN(event_i.getMetacommandEvent())
					NEW_EVENT_ENTRY_BEGIN(MAE_IssueOrder_Main) 
						
						descriptor_m.orderSignStr_m = event_i.context().orderSignStr_m;
						descriptor_m.orderTPSignStr_m = event_i.context().orderTPSignStr_m;
						descriptor_m.orderSLSignStr_m = event_i.context().orderSLSignStr_m;
						descriptor_m.commandType_m = event_i.context().commandType_m;

						descriptor_m.parse();

						newState.getMetacommandState() = MCS_Order_Main_Issued;
					NEW_EVENT_ENTRY_END()

				NEW_EVENT_END()

			OLD_STATE_ENTRY_END()
			// --------
			OLD_STATE_ENTRY_BEGIN(MCS_Order_Main_Issued)
				NEW_EVENT_BEGIN(event_i.getMetacommandEvent())
					NEW_EVENT_ENTRY_BEGIN(MAE_Order_Main_Open) 
						newState.getMetacommandState() = MCS_Order_TP_Issued;
					NEW_EVENT_ENTRY_END()

					NEW_EVENT_ENTRY_BEGIN(MAE_Order_Main_Cancel) 
						newState.getMetacommandState() = MCS_Initial;
					NEW_EVENT_ENTRY_END()

					NEW_EVENT_ENTRY_BEGIN(MAE_Order_Main_Error) 
						newState.getMetacommandState() = MCS_Inconsistent;
					NEW_EVENT_ENTRY_END()
				NEW_EVENT_END()
			OLD_STATE_ENTRY_END()

			// --------
			OLD_STATE_ENTRY_BEGIN(MCS_Order_TP_Issued)
				NEW_EVENT_BEGIN(event_i.getMetacommandEvent())
					NEW_EVENT_ENTRY_BEGIN(MAE_Order_TP_Pending) 
						newState.getMetacommandState() = MCS_Order_SL_Issued;
					NEW_EVENT_ENTRY_END()

					NEW_EVENT_ENTRY_BEGIN(MAE_Order_TP_Cancel) 
						newState.getMetacommandState() = MCS_Inconsistent;
					NEW_EVENT_ENTRY_END()

					NEW_EVENT_ENTRY_BEGIN(MAE_Order_TP_Error) 
						newState.getMetacommandState() = MCS_Inconsistent;
					NEW_EVENT_ENTRY_END()
				NEW_EVENT_END()
			OLD_STATE_ENTRY_END()

			// --------
			OLD_STATE_ENTRY_BEGIN(MCS_Order_SL_Issued)
				NEW_EVENT_BEGIN(event_i.getMetacommandEvent())
					NEW_EVENT_ENTRY_BEGIN(MAE_Order_SL_Pending) 
						newState.getMetacommandState() = MCS_Order_TP_SL_Pending;
					NEW_EVENT_ENTRY_END()

					NEW_EVENT_ENTRY_BEGIN(MAE_Order_SL_Cancel) 
						newState.getMetacommandState() = MCS_Inconsistent;
					NEW_EVENT_ENTRY_END()

					NEW_EVENT_ENTRY_BEGIN(MAE_Order_SL_Error) 
						newState.getMetacommandState() = MCS_Inconsistent;
					NEW_EVENT_ENTRY_END()
				NEW_EVENT_END()
			OLD_STATE_ENTRY_END()

			// --------
			OLD_STATE_ENTRY_BEGIN(MCS_Order_TP_SL_Pending)
			OLD_STATE_ENTRY_END()

			// --------
			OLD_STATE_ENTRY_BEGIN(MCS_Inconsistent)
			OLD_STATE_ENTRY_END()
	
		OLD_STATE_END()

		if (newState.getMetacommandState() != lastState_m.getMetacommandState()) {
			
			onStateChanged( lastState_m, newState, event_i);
			
			// TODO logging
			lastState_m = newState;
		}

	}

	
	// when state changed
	void MetacommandStateMachine::onStateChanged(MetacommandState const& oldState, MetacommandState const& newState, MetacommandEvent const& event_i)
	{
		if (listenerP_m)
			listenerP_m->onStateChangedNotification(machineID_m, oldState, newState );

		// all entries
		OLD_STATE_ENTRY_ALL()

			NEW_STATE_BEGIN(newState.getMetacommandState())
				NEW_STATE_ENTRY_BEGIN(MCS_Initial)
					MetacommandAction action(MAS_Clear);
					doAction(action);
				NEW_STATE_ENTRY_END();

			NEW_STATE_END();
		OLD_STATE_ENTRY_ALL_END()

		// 
		OLD_STATE_BEGIN(oldState.getMetacommandState())
			OLD_STATE_ENTRY_BEGIN(MCS_Initial)
				NEW_STATE_BEGIN(newState.getMetacommandState())
					
					NEW_STATE_ENTRY_BEGIN(MCS_Order_Main_Issued)
						
						MetacommandAction action(MAS_Issue_Order_Main);
						doAction(action);
						
					NEW_STATE_ENTRY_END()

				NEW_STATE_END();
			OLD_STATE_ENTRY_END()

			// ---------

			OLD_STATE_ENTRY_BEGIN(MCS_Order_Main_Issued)
				NEW_STATE_BEGIN(newState.getMetacommandState())
					
					NEW_STATE_ENTRY_BEGIN(MCS_Order_TP_Issued)
						
						MetacommandAction action(MAS_Issue_Order_TP);
						doAction(action);
						
					NEW_STATE_ENTRY_END()

				NEW_STATE_END();
			OLD_STATE_ENTRY_END()

			// ---------

			OLD_STATE_ENTRY_BEGIN(MCS_Order_TP_Issued)
				NEW_STATE_BEGIN(newState.getMetacommandState())
					
					NEW_STATE_ENTRY_BEGIN(MCS_Order_SL_Issued)
						
						MetacommandAction action(MAS_Issue_Order_SL);
						doAction(action);
						
					NEW_STATE_ENTRY_END()

				NEW_STATE_END();
			OLD_STATE_ENTRY_END()

			// ---------

			// ---------

			// ---------

		OLD_STATE_END()
	}

	void MetacommandStateMachine::doAction(MetacommandAction const& action)
	{
		// before store last action
		lastAction_m = action;

		ACTION_BEGIN(action.getMetacommandAction())

			ACTION_ENTRY_BEGIN(MAS_Issue_Order_Main)
			if (listenerP_m) {
				listenerP_m->ordermachine_Force_TE_InstallOrder(descriptor_m.orderSignStr_m, getMachineUid());
			}
			ACTION_ENTRY_END()
			// ---------

			ACTION_ENTRY_BEGIN(MAS_Issue_Order_TP)
			if (listenerP_m) {
				listenerP_m->ordermachine_Force_TE_InstallOrder(descriptor_m.orderTPSignStr_m, getMachineUid());
			}
			ACTION_ENTRY_END()

			// ---------

			ACTION_ENTRY_BEGIN(MAS_Issue_Order_SL)
			if (listenerP_m) {
				listenerP_m->ordermachine_Force_TE_InstallOrder(descriptor_m.orderSLSignStr_m, getMachineUid());
			}
			ACTION_ENTRY_END()

			// ---------

			ACTION_ENTRY_BEGIN(MAS_Clear)
			descriptor_m.clear();
			if (listenerP_m) {
				
			}
			ACTION_ENTRY_END()

			// ---------

			// ---------


		ACTION_END();
	}

};

