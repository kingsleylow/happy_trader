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
	//if (MAIN_FSM == getName())
	//	return;

	LOG_SYMBOL( base_m->getRunName(), base_m->getRtdataProvider(), machineSymbol_m, 
		getContext() << " State changed from: " << exit_state.toString() << " to: " << target_state.toString() << 
		" because of event: " << event_i.toString() <<
		(trans_type == CppUtils::BaseState<MachineContext>::TT_Guarded ? " ,Guarded transition":" Triggered transition")
	);


	/*
	if (target_state.getClassName2() == "TS_LevelTrack" ) {
		if (event_i.getClassId() == TE_Detected_Big_Volume::CLASS_ID) {
			LOG_SYMBOL( base_m->getRunName(), base_m->getRtdataProvider(), machineSymbol_m, 
				"H: start listening: " << event_i.toString() );
		}
	}
	if (exit_state.getClassName2() == "TS_LevelTrack") {
		if (event_i.getClassId() == TE_Price_Go_Far::CLASS_ID ||
			event_i.getClassId() == TE_Reset::CLASS_ID ||
			event_i.getClassId() == TE_Detect_Time_Excess::CLASS_ID
			) {
			LOG_SYMBOL( base_m->getRunName(), base_m->getRtdataProvider(), machineSymbol_m, 
				"H: stop listening because of: " << event_i.getClassName() << " ->\n" << event_i.toString()  );
		}

	}
	*/
	

}

void MachineContext::on_entry_action(
				CppUtils::BaseState<MachineContext> const& target_state,
				CppUtils::BaseEvent const& event_i
)
{
	
	//if (MAIN_FSM == getName())
	//	return;


	LOG_SYMBOL( base_m->getRunName(), base_m->getRtdataProvider(), machineSymbol_m, 
		getContext() << " Entry action occured on state: " << target_state.toString() << " ,event: " << event_i.toString()
	);
	
}
	
void MachineContext::on_exit_action(
				CppUtils::BaseState<MachineContext> const& exit_state,
				CppUtils::BaseEvent const& event_i
)
{
	
	//if (MAIN_FSM == getName())
	//	return;


	LOG_SYMBOL_SIGNAL( base_m->getRunName(), base_m->getRtdataProvider(), machineSymbol_m, 
		getContext() << " Exit action occured on state: " << exit_state.toString() << " ,event: " << event_i.toString()
	);
	
}





}; 