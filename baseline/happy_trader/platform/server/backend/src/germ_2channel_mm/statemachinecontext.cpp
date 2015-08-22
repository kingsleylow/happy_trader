#include "statemachinecontext.hpp"
#include "implement.hpp"

namespace AlgLib {

MachineContext::MachineContext(void* contextPtr):
	base_m(NULL)
{
	MachineContextStructure *ctx = (MachineContextStructure*) contextPtr;
	base_m = ctx->base_m;
	machineSymbol_m = ctx->symbol_m;
}

MachineContext::~MachineContext()
{
}

void MachineContext::on_state_changed(
			CppUtils::BaseState<MachineContext> const& exit_state, 
			CppUtils::BaseState<MachineContext> const& target_state,
			CppUtils::BaseState<MachineContext>::TransitionType const trans_type,
			CppUtils::BaseEvent const& event_i
	)
{
	
	LOG_SYMBOL_SIGNAL( base_m->getRunName(), base_m->getRtdataProvider(), machineSymbol_m, 
		"State changed from: " << exit_state.getClassName() << " to: " << target_state.getClassName() << 
		" because of event: " << event_i.getClassName() <<
		(trans_type == CppUtils::BaseState<MachineContext>::TT_Guarded ? " Guarded transition":" Triggered transition")
	);

}

void MachineContext::on_entry_action(
				CppUtils::BaseState<MachineContext> const& target_state,
				CppUtils::BaseEvent const& event_i
)
{
	LOG_SYMBOL_SIGNAL( base_m->getRunName(), base_m->getRtdataProvider(), machineSymbol_m, 
		"Entry action occured on state: " << target_state.getClassName() << " processing event: " << event_i.getClassName()
	);
}
	
void MachineContext::on_exit_action(
				CppUtils::BaseState<MachineContext> const& exit_state,
				CppUtils::BaseEvent const& event_i
)
{
	LOG_SYMBOL_SIGNAL( base_m->getRunName(), base_m->getRtdataProvider(), machineSymbol_m, 
		"Exit action occured on state: " << exit_state.getClassName() << " processing event: " << event_i.getClassName()
	);
}

bool MachineContext::isReverse() const
{
	return base_m->getTradeParam().isReverse_m;
}

}; 