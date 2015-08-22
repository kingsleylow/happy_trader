#ifndef _PAIR_TRADE_EVENT_INCLUDED
#define _PAIR_TRADE_EVENT_INCLUDED

#include "pair_tradedefs.hpp"
#include "statemachinecontext.hpp"
#include "commonstruct.hpp"



namespace AlgLib {

// ------------------------------------------

class TE_Signal_Short: public CppUtils::BaseEvent {
	public:
		CTOR(TE_Signal_Short);
		
};
// ------------------------------------------
class TE_Executed_All: public CppUtils::BaseEvent {
	public:
		CTOR(TE_Executed_All);
};
// ------------------------------------------
class TE_Signal_Long: public CppUtils::BaseEvent {
	public:
		CTOR(TE_Signal_Long);
		
};
// ------------------------------------------
class TE_Confirm_Open_Short: public CppUtils::BaseEvent {
	public:
		CTOR(TE_Confirm_Open_Short);
};
// ------------------------------------------
class TE_Confirm_Open_Nothing: public CppUtils::BaseEvent {
	public:
		CTOR(TE_Confirm_Open_Nothing);
};
// ------------------------------------------
class TE_Execute_Signal: public CppUtils::BaseEvent {
	public:
		CTOR(TE_Execute_Signal);
};
// ------------------------------------------
class TE_Confirm_Open_Long: public CppUtils::BaseEvent {
	public:
		CTOR(TE_Confirm_Open_Long);
};
// ------------------------------------------
class TE_Completed_Short: public CppUtils::BaseEvent {
	public:
		CTOR(TE_Completed_Short);
};

// ------------------------------------------

class TE_Stop_Trade: public CppUtils::BaseEvent {
	public:
		CTOR(TE_Stop_Trade);
};

// ------------------------------------------
class TE_Completed_Long: public CppUtils::BaseEvent {
	public:
		CTOR(TE_Completed_Long);
};
// ------------------------------------------
class TE_Broker_Error: public CppUtils::BaseEvent {
	public:
		CTOR(TE_Broker_Error);

		BEGIN_PARAM_ENTRIES_DECLARE(TE_Broker_Error)
			PARAM_ENTRY_DECLARE(CppUtils::String, errorReason_m);	
		END_PARAM_ENTRIES_DECLARE()

		virtual CppUtils::String toString() const
		{
			return	CppUtils::BaseEvent::toString() + toDescriptionString(); 
		}
};

// ------------------------------------------
class TE_Timeout: public CppUtils::BaseEvent {
	public:
		CTOR(TE_Timeout);
};

// ------------------------------------------

};

#endif