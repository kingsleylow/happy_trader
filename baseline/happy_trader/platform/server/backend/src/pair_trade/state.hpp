#ifndef _PAIR_TARDE_STATE_INCLUDED
#define _PAIR_TARDE_STATE_INCLUDED

#include "pair_tradedefs.hpp"
#include "statemachinecontext.hpp"
#include "event.hpp"

namespace AlgLib {

// ------------------------------------------
class TS_Completed: public CppUtils::BaseState<MachineContext> {
	public:
		CTOR(TS_Completed);
};
// ------------------------------------------
class TS_Some_Positions_Opened: public CppUtils::BaseState<MachineContext> {
	public:
		CTOR(TS_Some_Positions_Opened);
};
// ------------------------------------------
class TS_Ready_Open: public CppUtils::BaseState<MachineContext> {
	public:
		CTOR(TS_Ready_Open);
};
// ------------------------------------------
class TS_Initial: public CppUtils::BaseState<MachineContext> {
	public:
		CTOR(TS_Initial);
};
// ------------------------------------------
class TS_Error: public CppUtils::BaseState<MachineContext> {
	public:
		CTOR(TS_Error);
};
// ------------------------------------------
class TS_Open_Short: public CppUtils::BaseState<MachineContext> {
	public:
		CTOR(TS_Open_Short);
};
// ------------------------------------------
class TS_Open_Long: public CppUtils::BaseState<MachineContext> {
	public:
		CTOR(TS_Open_Long);
};
// ------------------------------------------
class TS_Open_Nothing: public CppUtils::BaseState<MachineContext> {
	public:
		CTOR(TS_Open_Nothing);
};
// ------------------------------------------


} // end of namespace

#endif