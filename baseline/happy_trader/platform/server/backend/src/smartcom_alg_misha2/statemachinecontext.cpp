#include "statemachinecontext.hpp"
#include "implement.hpp"


namespace AlgLib {

MachineContext::MachineContext(void* contextPtr):
	base_m(NULL)
{
	MachineContextStructure *ctx = (MachineContextStructure*) contextPtr;
	base_m = ctx->base_m;
	machineSymbol_m = ctx->symbol_m;
	name_m = ctx->name_m;
	robot_m = ctx->robot_m;
}

MachineContext::~MachineContext()
{
}

void MachineContext::on_fatal_error(
				CppUtils::StateMachineException const& e
)
{
	LOG_SYMBOL_SIGNAL( base_m->getRunName(), base_m->getRtDataProvider(), machineSymbol_m, 
		getContext() << " Fatal error: " << EXC2STR(e) << " processing will be stopped" );
}

void MachineContext::on_state_changed(
			CppUtils::BaseState<MachineContext> const& exit_state, 
			CppUtils::BaseState<MachineContext> const& target_state,
			CppUtils::BaseState<MachineContext>::TransitionType const trans_type,
			CppUtils::BaseEvent const& event_i
	)
{
	
	if (target_state.getClassId() == TS_Error::CLASS_ID || 
		target_state.getClassId() == TS_Mismatched_Order_Execution::CLASS_ID ||
		TE_Broker_Error::CLASS_ID == target_state.getClassId() 
	) {
		LOG_SYMBOL_SIGNAL( base_m->getRunName(), base_m->getRtDataProvider(), machineSymbol_m, 
			getContext() << " State changed from: [" << exit_state.toString() << "] to: [" << target_state.toString() << 
			"] event: [" << event_i.toString() <<  "]"
		);
	}
	else {
	
		LOG_SYMBOL( base_m->getRunName(), base_m->getRtDataProvider(), machineSymbol_m, 
			getContext() << " State changed from: [" << exit_state.toString() << "] to: [" << target_state.toString() << 
			"] event: [" << event_i.toString() <<  "]"
		);
	}

}

void MachineContext::on_entry_action(
				CppUtils::BaseState<MachineContext> const& target_state,
				CppUtils::BaseEvent const& event_i
)
{
	
	
	/*
	LOG_SYMBOL( base_m->getRunName(), base_m->getRtdataProvider(), machineSymbol_m, 
		getContext() << " Entry action occured on state: " << target_state.toString() << " ,event: " << event_i.toString()
	);
	*/
	
}
	
void MachineContext::on_exit_action(
				CppUtils::BaseState<MachineContext> const& exit_state,
				CppUtils::BaseEvent const& event_i
)
{
	
	
	/*
	LOG_SYMBOL_SIGNAL( base_m->getRunName(), base_m->getRtdataProvider(), machineSymbol_m, 
		getContext() << " Exit action occured on state: " << exit_state.toString() << " ,event: " << event_i.toString()
	);
	*/
	
}

void MachineContext::on_entry_action_execute(
	CppUtils::BaseState<MachineContext> const& target_state,
	CppUtils::BaseEvent const& event_i,
	CppUtils::BaseAction<MachineContext> const& action
			)
{

	

	LOG_SYMBOL( base_m->getRunName(), base_m->getRtDataProvider(), machineSymbol_m, 
		getContext() << "Entry action: " << action.toString()	<<
		" Context: [ \n" << getRobot().getGlobalTradeContext().get(getMachineSymbol()).toDescriptionString() << " ] "
	);
	
}																					   
		
void MachineContext::on_exit_action_execute(
	CppUtils::BaseState<MachineContext> const& exit_state,
	CppUtils::BaseEvent const& event_i,
	CppUtils::BaseAction<MachineContext> const& action
			)
{
	
	LOG_SYMBOL( base_m->getRunName(), base_m->getRtDataProvider(), machineSymbol_m, 
		getContext() << "Exit action: " << action.toString() <<
		" Context: [ \n" << getRobot().getGlobalTradeContext().get(getMachineSymbol()).toDescriptionString() << " ] "
	);												   
}

void MachineContext::on_delayed_event_register(CppUtils::BaseEvent const& event_i)
{
	LOG_SYMBOL( base_m->getRunName(), base_m->getRtDataProvider(), getMachineSymbol(), 
		getContext() << " Event registered: " << event_i.toString()
	);
}

void MachineContext::on_direct_event_process(CppUtils::BaseEvent const& event_i)
{
	
	LOG_SYMBOL( base_m->getRunName(), base_m->getRtDataProvider(), getMachineSymbol(), 
		getContext() << " Event directly processing: " << event_i.toString()
	);
	
}

void MachineContext::on_delayed_event_process(CppUtils::BaseEvent const&event_i)
{
	
	LOG_SYMBOL( base_m->getRunName(), base_m->getRtDataProvider(), getMachineSymbol(), 
		getContext() << " Delayed event processing: " << event_i.toString()
	);
	
}






}; 