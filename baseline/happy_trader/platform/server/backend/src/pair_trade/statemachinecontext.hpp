#ifndef _PAIR_TARDE_EVENTCONTEXT_INCLUDED
#define _PAIR_TARDE_EVENTCONTEXT_INCLUDED


#include "pair_tradedefs.hpp"

namespace AlgLib {

	 
	class CPairTrade;
	class TradingSequenceRobot;

	// this helper class 
	class MachineContext: public CppUtils::BaseContext<MachineContext>
	{
		public:

			struct MachineContextStructure
			{
				MachineContextStructure(
					CPairTrade &base, 
					TradingSequenceRobot& robot,
					CppUtils::String const& contextName
				):
					base_m(&base),
					name_m(contextName),
					robot_m(&robot)
				{
				}

				TradingSequenceRobot* robot_m;
				CPairTrade* base_m;
				CppUtils::String name_m;
			
			};

			MachineContext(void* contextPtr);

			virtual ~MachineContext();

			bool isReverse() const;

			// set of logging functions

			virtual void on_fatal_error(
				CppUtils::StateMachineException const& e
			);

			virtual void on_state_changed(
				CppUtils::BaseState<MachineContext> const& exit_state, 
				CppUtils::BaseState<MachineContext> const& target_state,
				CppUtils::BaseState<MachineContext>::TransitionType const trans_type,
				CppUtils::BaseEvent const& event_i
			);

			virtual void on_entry_action(
				CppUtils::BaseState<MachineContext> const& target_state,
				CppUtils::BaseEvent const& event_i
			);
	
			virtual void on_exit_action(
				CppUtils::BaseState<MachineContext> const& exit_state,
				CppUtils::BaseEvent const& event_i
			);

			virtual void on_entry_action_execute(
				CppUtils::BaseState<MachineContext> const& target_state,
				CppUtils::BaseEvent const& event_i,
				CppUtils::BaseAction<MachineContext> const& action
			);
		

			virtual void on_exit_action_execute(
				CppUtils::BaseState<MachineContext> const& exit_state,
				CppUtils::BaseEvent const& event_i,
				CppUtils::BaseAction<MachineContext> const& action
			);
	

			virtual void on_delayed_event_register(CppUtils::BaseEvent const& event_i);

			virtual void on_direct_event_process(CppUtils::BaseEvent const& event_i);
		
			virtual void on_delayed_event_process(CppUtils::BaseEvent const& event_i);
		
			virtual void MachineContext::on_transition_action(CppUtils::BaseTransitionAction<MachineContext> const& transition_action,  CppUtils::BaseEvent const& eventToProcess);
			
			inline CPairTrade& getBase()
			{
				return *base_m;
			};

			inline TradingSequenceRobot& getRobot()
			{
				return *robot_m;
			};

		

			CppUtils::String getContext() const
			{
				return "[ " + name_m + " ] ";
			};

			inline CppUtils::String const& getName() const
			{
				return name_m;
			};

			
			
			
		private:

			TradingSequenceRobot* robot_m;

			CPairTrade* base_m;

			CppUtils::String name_m;

			


	};

	

};

#endif