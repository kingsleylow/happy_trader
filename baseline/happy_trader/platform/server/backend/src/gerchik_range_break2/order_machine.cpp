#include "order_machine.hpp"
#include "statemachine.hpp"

namespace AlgLib {

	OrderStateMachine::OrderStateMachine(CppUtils::Uid const& uid, OrderStateMachineListener* listener, CppUtils::String const& description,CppUtils::Uid const& parentUid):
		machineID_m(uid),
		listenerP_m(listener),
		creationLocalTime_m( CppUtils::getTime()),
		description_m(description),
		parentUid_m(parentUid)
	{
		
		if (listenerP_m)
			listenerP_m->onStateChangedNotification(getParentUid(),machineID_m, TradingState(TS_Dummy), lastState_m);
		
	}

	OrderStateMachine::OrderStateMachine():
		listenerP_m(NULL)
	{
		
	}

	OrderStateMachine::~OrderStateMachine()
	{
	}

	void OrderStateMachine::initialize(CppUtils::Uid const& uid, OrderStateMachineListener* listener, CppUtils::String const& description,CppUtils::Uid const& parentUid)
	{
		machineID_m = uid;
		description_m = description;
		listenerP_m = listener;

		creationLocalTime_m = CppUtils::getTime();
		lastState_m = TS_Initial;
		lastAction_m.clear();
		lastOrderDescriptor_m.clear();

		parentUid_m = parentUid;


		if (listenerP_m)
			listenerP_m->onStateChangedNotification(getParentUid(),machineID_m, TradingState(TS_Dummy), lastState_m);

	}


	

	void OrderStateMachine::newEventArrived(TradingEvent const& event_i)
	{
		
		TradingState newState = lastState_m;

		OLD_STATE_BEGIN(lastState_m.getState())
			OLD_STATE_ENTRY_BEGIN(TS_Initial)
				
				NEW_EVENT_BEGIN(event_i.getEvent())
					NEW_EVENT_ENTRY_BEGIN(TE_InstallOrder) 
						newState.getState() = TS_Issued;
						
						// init order data from signaller
						lastOrderDescriptor_m.symbol_m = event_i.context().signaller_m.symbol_m;
						lastOrderDescriptor_m.volume_m = event_i.context().signaller_m.volume_m;
						lastOrderDescriptor_m.orderMethod_m = event_i.context().signaller_m.orderMethod_m;
						lastOrderDescriptor_m.orderPlace_m = event_i.context().signaller_m.orderPlace_m;
						lastOrderDescriptor_m.additionalOrderInfo_m = event_i.context().signaller_m.additionalOrderInfo_m;
						if (event_i.context().signaller_m.orderType_m==BrkLib::OP_BUY || event_i.context().signaller_m.orderType_m==BrkLib::OP_BUY_LIMIT ||  event_i.context().signaller_m.orderType_m==BrkLib::OP_BUY_STOP) {
							lastOrderDescriptor_m.direction_m = DIRECTION_LONG;
						}
						else if (event_i.context().signaller_m.orderType_m==BrkLib::OP_SELL || event_i.context().signaller_m.orderType_m==BrkLib::OP_SELL_LIMIT ||  event_i.context().signaller_m.orderType_m==BrkLib::OP_SELL_STOP) {
							lastOrderDescriptor_m.direction_m = DIRECTION_SHORT;
						}
						else
							lastOrderDescriptor_m.direction_m = DIRECTION_DUMMY;

						
						lastOrderDescriptor_m.orderID_m = event_i.context().signaller_m.orderUid_m;
						lastOrderDescriptor_m.orderType_m = event_i.context().signaller_m.orderType_m;
						lastOrderDescriptor_m.orderIssuePrice_m = event_i.context().signaller_m.opIssuePrice_m;
						lastOrderDescriptor_m.orderIsDelayedLocally_m = event_i.context().signaller_m.orderIsDelayedLocally_m;
						//
						
						
						if (listenerP_m) {
							listenerP_m->machineConfirmOrderIssued(getParentUid(), machineID_m, lastOrderDescriptor_m);
						}
					NEW_EVENT_ENTRY_END()
				NEW_EVENT_END()

			OLD_STATE_ENTRY_END()

			// ------------------------

			OLD_STATE_ENTRY_BEGIN(TS_Issued)
				
				NEW_EVENT_BEGIN(event_i.getEvent())
					NEW_EVENT_ENTRY_BEGIN(TE_BrokerConfirm_Pending) 
						newState.getState() = TS_Pending;
						lastOrderDescriptor_m.orderInstallTime_m = event_i.context().installTime_m;
						lastOrderDescriptor_m.orderInstallPrice_m = event_i.context().installPrice_m;
						lastOrderDescriptor_m.brokerPositionId_m = event_i.context().brokerPositionId_m;
						if (listenerP_m) {
							listenerP_m->onOrderDescriptorChanged(getParentUid(),machineID_m, lastOrderDescriptor_m);
							listenerP_m->machineConfirmOrderPending(getParentUid(), machineID_m, lastOrderDescriptor_m);
						}
					NEW_EVENT_ENTRY_END()
					NEW_EVENT_ENTRY_BEGIN(TE_BrokerReturnError) 
						newState.getState() = TS_Initial;
						lastOrderDescriptor_m.orderErrorTime_m = event_i.context().errorTime_m;
						lastOrderDescriptor_m.errorReason_m = event_i.context().errorReason_m;
						if (listenerP_m) {
							listenerP_m->onOrderDescriptorChanged(getParentUid(),machineID_m, lastOrderDescriptor_m);
							listenerP_m->machineConfirmErrorReturned(getParentUid(), machineID_m, lastOrderDescriptor_m);
						}
					NEW_EVENT_ENTRY_END()
					NEW_EVENT_ENTRY_BEGIN(TE_BrokerConfirm_Delayed) 
						newState.getState() = TS_Delayed;
						if (listenerP_m)
							listenerP_m->machineConfirmOrderDelayed(getParentUid(), machineID_m, lastOrderDescriptor_m);
					NEW_EVENT_ENTRY_END()
					
				NEW_EVENT_END()

			OLD_STATE_ENTRY_END()

			// ------------------------

			OLD_STATE_ENTRY_BEGIN(TS_Pending)
				
				NEW_EVENT_BEGIN(event_i.getEvent())
					NEW_EVENT_ENTRY_BEGIN(TE_BrokerConfirm_Open) 
						newState.getState() = TS_Open;
						lastOrderDescriptor_m.orderExecutePrice_m = event_i.context().executePrice_m;
						lastOrderDescriptor_m.orderExecuteTime_m = event_i.context().executeTime_m;
						lastOrderDescriptor_m.brokerPositionId_m = event_i.context().brokerPositionId_m;
						if (listenerP_m) {
							listenerP_m->onOrderDescriptorChanged(getParentUid(),machineID_m, lastOrderDescriptor_m);
							listenerP_m->machineConfirmOrderOpened(getParentUid(), machineID_m, lastOrderDescriptor_m);
						}
					NEW_EVENT_ENTRY_END()
					NEW_EVENT_ENTRY_BEGIN(TE_BrokerReturnError) 
						newState.getState() = TS_Initial;
						lastOrderDescriptor_m.orderErrorTime_m = event_i.context().errorTime_m;
						lastOrderDescriptor_m.errorReason_m = event_i.context().errorReason_m;
						if (listenerP_m) {
							listenerP_m->onOrderDescriptorChanged(getParentUid(),machineID_m, lastOrderDescriptor_m);
							listenerP_m->machineConfirmErrorReturned(getParentUid(), machineID_m, lastOrderDescriptor_m);
						}

					NEW_EVENT_ENTRY_END()
					NEW_EVENT_ENTRY_BEGIN(TE_UserCancel) 
						newState.getState() = TS_Cancelling;
					NEW_EVENT_ENTRY_END()
				NEW_EVENT_END()

			OLD_STATE_ENTRY_END()

			// ------------------------

			OLD_STATE_ENTRY_BEGIN(TS_Delayed)
				NEW_EVENT_BEGIN(event_i.getEvent())
					NEW_EVENT_ENTRY_BEGIN(TE_BrokerConfirm_Pending) 
						newState.getState() = TS_Pending;
						lastOrderDescriptor_m.orderInstallTime_m = event_i.context().installTime_m;
						lastOrderDescriptor_m.orderInstallPrice_m = event_i.context().installPrice_m;
						lastOrderDescriptor_m.brokerPositionId_m = event_i.context().brokerPositionId_m;
						if (listenerP_m) {
							listenerP_m->onOrderDescriptorChanged(getParentUid(),machineID_m, lastOrderDescriptor_m);
							listenerP_m->machineConfirmOrderPending(getParentUid(), machineID_m, lastOrderDescriptor_m);
						}
					NEW_EVENT_ENTRY_END()

					NEW_EVENT_ENTRY_BEGIN(TE_UserCancel) 
						newState.getState() = TS_Cancelling;
					NEW_EVENT_ENTRY_END()

				NEW_EVENT_END()

			OLD_STATE_ENTRY_END()

			// ------------------------

			OLD_STATE_ENTRY_BEGIN(TS_Open)
			OLD_STATE_ENTRY_END()

			// ------------------------

			OLD_STATE_ENTRY_BEGIN(TS_Cancelling)

				NEW_EVENT_BEGIN(event_i.getEvent())
					NEW_EVENT_ENTRY_BEGIN(TE_BrokerConfirm_Cancelled) 
						newState.getState() = TS_Initial;
						lastOrderDescriptor_m.orderCancelPrice_m = event_i.context().cancelPrice_m;
						lastOrderDescriptor_m.orderCancelTime_m = event_i.context().cancelTime_m;
						if (listenerP_m) {
							listenerP_m->onOrderDescriptorChanged(getParentUid(),machineID_m, lastOrderDescriptor_m);
							listenerP_m->machineConfirmOrderCancelled(getParentUid(), machineID_m, lastOrderDescriptor_m);
						}
						
					NEW_EVENT_ENTRY_END()
					NEW_EVENT_ENTRY_BEGIN(TE_BrokerReturnError) 
						newState.getState() = TS_Inconsistent;
						lastOrderDescriptor_m.orderErrorTime_m = event_i.context().errorTime_m;
						lastOrderDescriptor_m.errorReason_m = event_i.context().errorReason_m;
						if (listenerP_m) {
							listenerP_m->onOrderDescriptorChanged(getParentUid(),machineID_m, lastOrderDescriptor_m);
							listenerP_m->machineConfirmErrorReturned(getParentUid(), machineID_m, lastOrderDescriptor_m);
						}
					NEW_EVENT_ENTRY_END()
				NEW_EVENT_END()

			OLD_STATE_ENTRY_END()

			// ------------------------

			OLD_STATE_ENTRY_BEGIN(TS_Inconsistent)
				NEW_EVENT_BEGIN(event_i.getEvent())
					NEW_EVENT_ENTRY_BEGIN(TE_UserConfirmFix) 
						newState.getState() = TS_Initial;
					NEW_EVENT_ENTRY_END()
				NEW_EVENT_END()

			OLD_STATE_ENTRY_END()

			// ------------------------

		OLD_STATE_END()

		if (newState.getState() != lastState_m.getState()) {
			
			onStateChanged( lastState_m, newState, event_i);
			
			// TODO logging
			lastState_m = newState;
		}

	}

	// ------------------------
	// helpers

	void OrderStateMachine::onStateChanged(TradingState const& oldState, TradingState const& newState, TradingEvent const& event_i)
	{
		if (listenerP_m)
			listenerP_m->onStateChangedNotification(getParentUid(),machineID_m, oldState, newState);
		// here our action when we arrive to TS_Initial from ALL states
		OLD_STATE_ENTRY_ALL()

			NEW_STATE_BEGIN(newState.getState())
				NEW_STATE_ENTRY_BEGIN(TS_Initial)
				
					// clear context
					Action action(AS_Clear);
					if (listenerP_m)
						listenerP_m->requestClearInfo(machineID_m, lastAction_m, action, lastOrderDescriptor_m);
					doAction(action);

				NEW_STATE_ENTRY_END();
			NEW_STATE_END();
		OLD_STATE_ENTRY_ALL_END()
		
			
		// here when we arrive from oldState to newState

		OLD_STATE_BEGIN(oldState.getState())
			
			OLD_STATE_ENTRY_BEGIN(TS_Initial)
				NEW_STATE_BEGIN(newState.getState())
					
					NEW_STATE_ENTRY_BEGIN(TS_Issued)
						
						Action action(AS_IssueOrder);
						if (listenerP_m)
							listenerP_m->requestIssueOrderInfo(machineID_m,lastAction_m, action, lastOrderDescriptor_m);
						doAction(action);
						
					NEW_STATE_ENTRY_END()

				NEW_STATE_END();
			OLD_STATE_ENTRY_END()

			// -----------------

			OLD_STATE_ENTRY_BEGIN(TS_Issued)
				NEW_STATE_BEGIN(newState.getState())
					
					NEW_STATE_ENTRY_BEGIN(TS_Cancelling)
						
						Action action(AS_CancelOrder);
						if (listenerP_m)
							listenerP_m->requestCancelOrderInfo(machineID_m,lastAction_m, action, lastOrderDescriptor_m);
						doAction(action);
						
					NEW_STATE_ENTRY_END()
					

				NEW_STATE_END();
			OLD_STATE_ENTRY_END()

			// -----------------

			OLD_STATE_ENTRY_BEGIN(TS_Pending)
				NEW_STATE_BEGIN(newState.getState())
					
					NEW_STATE_ENTRY_BEGIN(TS_Cancelling)
						
						Action action(AS_CancelOrder);
						if (listenerP_m)
							listenerP_m->requestCancelOrderInfo(machineID_m,lastAction_m, action, lastOrderDescriptor_m);
						doAction(action);
						
					NEW_STATE_ENTRY_END()
					

				NEW_STATE_END();
			OLD_STATE_ENTRY_END()

			// -----------------

			OLD_STATE_ENTRY_BEGIN(TS_Delayed)
				NEW_STATE_BEGIN(newState.getState())
					
					NEW_STATE_ENTRY_BEGIN(TS_Cancelling)
						
						Action action(AS_RemoveDelayedOrder);
						if (listenerP_m)
							listenerP_m->requestRemoveDelayedOrderInfo(machineID_m,lastAction_m, action, lastOrderDescriptor_m);
						doAction(action);
						
					NEW_STATE_ENTRY_END()
					

				NEW_STATE_END();
			OLD_STATE_ENTRY_END()

			// -----------------

			OLD_STATE_ENTRY_BEGIN(TS_Open)
			OLD_STATE_ENTRY_END()

			// -----------------

			OLD_STATE_ENTRY_BEGIN(TS_Cancelling)
				
			OLD_STATE_ENTRY_END()

			// -----------------

			OLD_STATE_ENTRY_BEGIN(TS_Inconsistent)
			OLD_STATE_ENTRY_END()

			// -----------------

		OLD_STATE_END()
		
	}


	void OrderStateMachine::doAction(Action const& action)
	{

		// do action

		// before store last action
		lastAction_m = action;
		
		ACTION_BEGIN(action.getAction())
			ACTION_ENTRY_BEGIN(AS_Dummy)
			ACTION_ENTRY_END()

			ACTION_ENTRY_BEGIN(AS_Clear)
				if (listenerP_m)
					listenerP_m->executeClear(machineID_m, action,lastOrderDescriptor_m);
			ACTION_ENTRY_END()

			ACTION_ENTRY_BEGIN(AS_IssueOrder)
				if (listenerP_m)
					listenerP_m->executeIssueOrder(machineID_m, action,lastOrderDescriptor_m);
			ACTION_ENTRY_END()

			ACTION_ENTRY_BEGIN(AS_CancelOrder)
				if (listenerP_m)
					listenerP_m->executeCancelOrder(machineID_m, action,lastOrderDescriptor_m);
			ACTION_ENTRY_END()

			ACTION_ENTRY_BEGIN(AS_RemoveDelayedOrder)
				if (listenerP_m)
					listenerP_m->executeRemoveDelayedOrder(machineID_m, action,lastOrderDescriptor_m);
			ACTION_ENTRY_END()

				

		ACTION_END();
		
			
	}

	

};