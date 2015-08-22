#ifndef _GERCHIK_RANGE_BREAK2_METACOMMANDMACHINE_INCLUDED
#define _GERCHIK_RANGE_BREAK2_METACOMMANDMACHINE_INCLUDED

#include "gerchik_range_break2defs.hpp"
#include "metacommand_action.hpp"
#include "metacommand_event.hpp"
#include "metacommand_state.hpp"
#include "metacommanddescriptor.hpp"

namespace AlgLib {






	// --------------------------
	// 


	class MetacommandStateMachineListener
	{
	public:
		virtual void onStateChangedNotification(CppUtils::Uid const& machineUid, MetacommandState const& oldState, MetacommandState const& newState) = 0;

		// must be executed by metacommand processsor
		virtual void ordermachine_Force_TE_InstallOrder(CppUtils::String const& orderSignallerString, CppUtils::Uid const& machineUid) = 0;


	};

	/**
	* This class incapsulates state machine for metacommnds (sequence of orders)
	*/



	class MetacommandStateMachine {
	public:
		MetacommandStateMachine();

		~MetacommandStateMachine();

		void initialize(CppUtils::Uid const& uid, MetacommandStateMachineListener* listener, CppUtils::String const& description);

		void newEventArrived(MetacommandEvent const& event_i);

		CppUtils::Uid const& getMachineUid() const
		{
			return machineID_m;
		}

		CppUtils::String const& getDescription() const
		{
			return description_m;
		}

		MetacommandAction const& getLastAction() const
		{
			return lastAction_m;
		}

		MetacommandState const& getLastState() const
		{
			return lastState_m;
		};

	private:

		void doAction(MetacommandAction const& action);

		// when state changed
		void onStateChanged(MetacommandState const& oldState, MetacommandState const& newState, MetacommandEvent const& event_i);

	private:

		// current state
		MetacommandState lastState_m;

		// current action
		MetacommandAction lastAction_m;

		double creationLocalTime_m;

		CppUtils::Uid machineID_m;

		CppUtils::String description_m;

		MetacommandStateMachineListener* listenerP_m;

		// member variables
		MetacommadDescriptor descriptor_m;
	};

};

#endif