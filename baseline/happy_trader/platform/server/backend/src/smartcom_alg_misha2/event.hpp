#ifndef _SMARTCOM_ALG_MISHA2_EVENT_INCLUDED
#define _SMARTCOM_ALG_MISHA2_EVENT_INCLUDED

#include "smartcom_alg_misha2defs.hpp"
#include "statemachinecontext.hpp"



namespace AlgLib {

class TE_Stop_Cancelled: public CppUtils::BaseEvent {
	public:
		CTOR(TE_Stop_Cancelled);
};
// ------------------------------------------
class TE_Market_Close_Pending: public CppUtils::BaseEvent {
	public:
		CTOR(TE_Market_Close_Pending);
};
// ------------------------------------------
class TE_Limit_Executed: public CppUtils::BaseEvent {
	public:
		CTOR(TE_Limit_Executed);
};
// ------------------------------------------
class TE_Stop_Cancelled_TP_Exit: public CppUtils::BaseEvent {
	public:
		CTOR(TE_Stop_Cancelled_TP_Exit);
};
// ------------------------------------------
class TE_Stop_Cancelled_Market_Exit: public CppUtils::BaseEvent {
	public:
		CTOR(TE_Stop_Cancelled_Market_Exit);
};
// ------------------------------------------
class TE_Limit_TP_Cancelled: public CppUtils::BaseEvent {
	public:
		CTOR(TE_Limit_TP_Cancelled);
};
// ------------------------------------------
class TE_Level_Lost: public CppUtils::BaseEvent {
	public:
		CTOR(TE_Level_Lost);
};
// ------------------------------------------
class TE_Limit_Cancelled: public CppUtils::BaseEvent {
	public:
		CTOR(TE_Limit_Cancelled);
};
// ------------------------------------------
class TE_Unconditional: public CppUtils::BaseEvent {
	public:
		CTOR(TE_Unconditional);
};
// ------------------------------------------
class TE_Level_Detected: public CppUtils::BaseEvent {
	public:
		CTOR(TE_Level_Detected);
		BEGIN_PARAM_ENTRIES_DECLARE(TE_Level_Detected)
				PARAM_ENTRY_DECLARE(BrkLib::TradeDirection, direction_m);	
				PARAM_ENTRY_DECLARE(double, detectedPriceLevel_m);	
		END_PARAM_ENTRIES_DECLARE()

		virtual CppUtils::String toString() const
		{
			return	CppUtils::BaseEvent::toString() + toDescriptionString(); 
		}
};
// ------------------------------------------
class TE_Stop_Executed: public CppUtils::BaseEvent {
	public:
		CTOR(TE_Stop_Executed);
};
// ------------------------------------------
class TE_Limit_TP_Pending: public CppUtils::BaseEvent {
	public:
		CTOR(TE_Limit_TP_Pending);
};
// ------------------------------------------
class TE_Stop_Pending: public CppUtils::BaseEvent {
	public:
		CTOR(TE_Stop_Pending);
};
// ------------------------------------------
class TE_Limit_TP_Executed: public CppUtils::BaseEvent {
	public:
		CTOR(TE_Limit_TP_Executed);
};
// ------------------------------------------
class TE_Broker_Error: public CppUtils::BaseEvent {
	public:
		CTOR(TE_Broker_Error);
};
// ------------------------------------------
class TE_Market_Close_Executed: public CppUtils::BaseEvent {
	public:
		CTOR(TE_Market_Close_Executed);
};
// ------------------------------------------
class TE_Limit_Pending: public CppUtils::BaseEvent {
	public:
		CTOR(TE_Limit_Pending);
};
// ------------------------------------------

};

#endif