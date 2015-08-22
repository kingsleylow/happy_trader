#ifndef _SMARTCOM_ALG_MISHA2_MMSTATE_INCLUDED
#define _SMARTCOM_ALG_MISHA2_MMSTATE_INCLUDED

#include "smartcom_alg_misha2defs.hpp"
#include "statemachinecontext.hpp"
#include "event.hpp"

namespace AlgLib {

// ------------------------------------------
class TS_Ready_Cancel_Limit_TP: public CppUtils::BaseState<MachineContext> {
	public:
		CTOR(TS_Ready_Cancel_Limit_TP);
};
// ------------------------------------------
class TS_Both_Pending: public CppUtils::BaseState<MachineContext> {
	public:
		CTOR(TS_Both_Pending);
};
// ------------------------------------------
class TS_Broker_Error: public CppUtils::BaseState<MachineContext> {
	public:
		CTOR(TS_Broker_Error);
};
// ------------------------------------------
class TS_Wait_Market_Close: public CppUtils::BaseState<MachineContext> {
	public:
		CTOR(TS_Wait_Market_Close);
};
// ------------------------------------------
class TS_No_Position: public CppUtils::BaseState<MachineContext> {
	public:
		CTOR(TS_No_Position);
};
// ------------------------------------------
class TS_Position_Opened: public CppUtils::BaseState<MachineContext> {
	public:
		CTOR(TS_Position_Opened);
};
// ------------------------------------------
class TS_Initial: public CppUtils::BaseState<MachineContext> {
	public:
		CTOR(TS_Initial);
};
// ------------------------------------------
class TS_Ready_Cancel_Limit: public CppUtils::BaseState<MachineContext> {
	public:
		CTOR(TS_Ready_Cancel_Limit);
};
// ------------------------------------------
class TS_Mismatched_Order_Execution: public CppUtils::BaseState<MachineContext> {
	public:
		CTOR(TS_Mismatched_Order_Execution);
};
// ------------------------------------------
class TS_Wait_Close_Pos: public CppUtils::BaseState<MachineContext> {
	public:
		CTOR(TS_Wait_Close_Pos);
};
// ------------------------------------------
class TS_Ready_Issue_Stop: public CppUtils::BaseState<MachineContext> {
	public:
		CTOR(TS_Ready_Issue_Stop);
};
// ------------------------------------------
class TS_Ready_Cancel_Stop: public CppUtils::BaseState<MachineContext> {
	public:
		CTOR(TS_Ready_Cancel_Stop);
};
// ------------------------------------------
class TS_Ready_Issue_Limit: public CppUtils::BaseState<MachineContext> {
	public:
		CTOR(TS_Ready_Issue_Limit);
};
// ------------------------------------------
class TS_Ready_Market_Close: public CppUtils::BaseState<MachineContext> {
	public:
		CTOR(TS_Ready_Market_Close);
};
// ------------------------------------------
class TS_Error: public CppUtils::BaseState<MachineContext> {
	public:
		CTOR(TS_Error);
};
// ------------------------------------------
class TS_Ready_Cancel_Stop_Market_Exit: public CppUtils::BaseState<MachineContext> {
	public:
		CTOR(TS_Ready_Cancel_Stop_Market_Exit);
};
// ------------------------------------------
class TS_Ready_Cancel_Stop_TP_Exit: public CppUtils::BaseState<MachineContext> {
	public:
		CTOR(TS_Ready_Cancel_Stop_TP_Exit);
};
// ------------------------------------------


} // end of namespace

#endif