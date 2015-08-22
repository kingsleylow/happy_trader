#ifndef _GERM_2CHANNEL_MM_EVENTCONTEXT_INCLUDED
#define _GERM_2CHANNEL_MM_EVENTCONTEXT_INCLUDED


#include "germ_2channel_mmdefs.hpp"

namespace AlgLib {

	 
	class Grm2ChannelMM;

	// this helper class 
	class MachineContext: public CppUtils::BaseContext<MachineContext>
	{
		public:

			struct MachineContextStructure
			{
				Grm2ChannelMM* base_m;
				CppUtils::String symbol_m;
			};

			MachineContext(void* contextPtr);

			virtual ~MachineContext();

			bool isReverse() const;

			// set of logging functions
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
			
			inline Grm2ChannelMM& getBase()
			{
				return *base_m;
			};

			
		private:

			Grm2ChannelMM* base_m;

			CppUtils::String machineSymbol_m;

			


	};

	

};

#endif