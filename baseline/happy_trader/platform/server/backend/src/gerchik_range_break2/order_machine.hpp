#ifndef _GERCHIK_RANGE_BREAK2_ORDER_MACHINE_INCLUDED
#define _GERCHIK_RANGE_BREAK2_ORDER_MACHINE_INCLUDED

#include "action.hpp"
#include "event.hpp"
#include "state.hpp"
#include "utils.hpp"

namespace AlgLib {

	

	// this is a class 
	class OrderStateMachineListener
	{
		// the derived must return respective context here
	public:
		// assume fill in the orderDescriptor structure
		// pass here the last orderDescriptor
		virtual void requestIssueOrderInfo(CppUtils::Uid const& orderUid, Action const& oldaction, Action const& newaction, OrderDescriptor& orderDescriptor) = 0;
		virtual void requestCancelOrderInfo(CppUtils::Uid const& orderUid, Action const& oldaction, Action const& newaction, OrderDescriptor& orderDescriptor) = 0;
		virtual void requestRemoveDelayedOrderInfo(CppUtils::Uid const& orderUid, Action const& oldaction, Action const& newaction, OrderDescriptor& orderDescriptor) = 0;
		virtual void requestClearInfo(CppUtils::Uid const& orderUid, Action const& oldaction, Action const& newaction, OrderDescriptor& orderDescriptor) = 0;

		// this executes orders
		virtual void executeIssueOrder(CppUtils::Uid const& orderUid, Action const& action, OrderDescriptor const& orderDescriptor) = 0;
		virtual void executeCancelOrder(CppUtils::Uid const& orderUid, Action const& action, OrderDescriptor const& orderDescriptor) = 0;
		virtual void executeRemoveDelayedOrder(CppUtils::Uid const& orderUid, Action const& action, OrderDescriptor const& orderDescriptor) = 0;
		virtual void executeClear(CppUtils::Uid const& orderUid, Action const& action, OrderDescriptor const& orderDescriptor) = 0;

		virtual void onStateChangedNotification(CppUtils::Uid const& parentUid, CppUtils::Uid const& orderUid, TradingState const& oldState, TradingState const& newState) = 0;
		virtual void onOrderDescriptorChanged(CppUtils::Uid const& parentUid, CppUtils::Uid const& orderUid, OrderDescriptor const& orderDescriptor) = 0;

		// exact confirmation with orders
		virtual void machineConfirmOrderIssued(CppUtils::Uid const& parentUid, CppUtils::Uid const& orderUid, OrderDescriptor const& orderDescriptor) = 0;
		virtual void machineConfirmOrderPending(CppUtils::Uid const& parentUid, CppUtils::Uid const& orderUid, OrderDescriptor const& orderDescriptor) = 0;
		virtual void machineConfirmOrderDelayed(CppUtils::Uid const& parentUid, CppUtils::Uid const& orderUid, OrderDescriptor const& orderDescriptor) = 0;
		virtual void machineConfirmOrderOpened(CppUtils::Uid const& parentUid, CppUtils::Uid const& orderUid, OrderDescriptor const& orderDescriptor) = 0;
		virtual void machineConfirmOrderCancelled(CppUtils::Uid const& parentUid, CppUtils::Uid const& orderUid, OrderDescriptor const& orderDescriptor) = 0;
		virtual void machineConfirmErrorReturned(CppUtils::Uid const& parentUid, CppUtils::Uid const& orderUid, OrderDescriptor const& orderDescriptor) = 0;

	};


	/**
	* This incapsulate order as state machine, so it is a box with input events and actions
	*/

	class OrderStateMachine {

	public:
		OrderStateMachine(CppUtils::Uid const& uid, OrderStateMachineListener* listener, CppUtils::String const& description, CppUtils::Uid const& parentUid);

		OrderStateMachine();

		~OrderStateMachine();


		void initialize(CppUtils::Uid const& uid, OrderStateMachineListener* listener, CppUtils::String const& description, CppUtils::Uid const& parentUid);

		// this is processing when new event arrived
		void newEventArrived(TradingEvent const& event_i);

		Action const& getLastAction() const
		{
			return lastAction_m;
		}

		CppUtils::Uid const& getMachineUid() const
		{
			return machineID_m;
		}

		CppUtils::String const& getDescription() const
		{
			return description_m;
		}

		CppUtils::Uid const& getParentUid() const
		{
			return parentUid_m;
		}

		

	private:

		void doAction(Action const& action);

		// when state changed
		void onStateChanged(TradingState const& oldState, TradingState const& newState, TradingEvent const& event_i);

	private:

		// current state
		TradingState lastState_m;

		// current context
		Action lastAction_m;

		OrderDescriptor lastOrderDescriptor_m;

		OrderStateMachineListener* listenerP_m;

		double creationLocalTime_m;

		CppUtils::Uid machineID_m;

		CppUtils::String description_m;

		// parent uid - optional
		CppUtils::Uid parentUid_m;


	};

};

#endif
