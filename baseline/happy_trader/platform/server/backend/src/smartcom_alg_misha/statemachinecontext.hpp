#ifndef _SMARTCOM_ALG_MISHA_EVENTCONTEXT_INCLUDED
#define _SMARTCOM_ALG_MISHA_EVENTCONTEXT_INCLUDED


#include "smartcom_alg_mishadefs.hpp"

namespace AlgLib {

	 
	class CAlgMisha;

	// this helper class 
	class MachineContext: public CppUtils::BaseContext<MachineContext>
	{
		public:

			struct MachineContextStructure
			{
				MachineContextStructure(
					CppUtils::String const& symbol, 
					CAlgMisha &base, 
					CppUtils::String const& contextName
				):
				symbol_m(symbol),
				base_m(&base),
				name_m(contextName)
				{
				}

				CAlgMisha* base_m;
				CppUtils::String symbol_m;
				CppUtils::String name_m;
			
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

			
			
			inline CAlgMisha& getBase()
			{
				return *base_m;
			};

			inline CppUtils::String const& getMachineSymbol()	const
			{
				return machineSymbol_m;
			}

			CppUtils::String getContext() const
			{
				return machineSymbol_m + " - " + "[ " + name_m + " ] ";
			};

			inline CppUtils::String const& getName() const
			{
				return name_m;
			};

			
			
		private:

			CAlgMisha* base_m;

			CppUtils::String machineSymbol_m;

			CppUtils::String name_m;

			


	};

	

};

#endif