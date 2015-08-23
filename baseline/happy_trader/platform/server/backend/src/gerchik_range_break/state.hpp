#ifndef _GERCHIK_RANGE_BREAK_STATE_INCLUDED
#define _GERCHIK_RANGE_BREAK_STATE_INCLUDED

#include "../brklib/brklib.hpp"


namespace AlgLib {

	DEFINE_ENUM_ENTRY(TS_Dummy,0 )
	DEFINE_ENUM_ENTRY(TS_Initial,1 )
	DEFINE_ENUM_ENTRY(TS_Ready_Enter_Both,2 )
	DEFINE_ENUM_ENTRY(TS_Pending_Both,3 )
	DEFINE_ENUM_ENTRY(TS_Ready_Cancel_OtherSide,4 )
	DEFINE_ENUM_ENTRY(TS_Ready_Enter_TP_SL,5 )
	DEFINE_ENUM_ENTRY(TS_Pending_TP_SL,6 )
	DEFINE_ENUM_ENTRY(TS_Ready_Cancel_SL,7 )
	DEFINE_ENUM_ENTRY(TS_Ready_Cancel_TP,8 )
	DEFINE_ENUM_ENTRY(TS_Ready_Cancel_Both,9 )
	DEFINE_ENUM_ENTRY(TS_Inconsistent,10 )

	struct TradingState {
		TradingState():
			state_m(TS_Initial)
		{
		}
	

		// -----------------

		bool isIdle() const
		{
			if (state_m==TS_Initial || state_m == TS_Dummy)
				return true;
			else
				return false;
		}

		CppUtils::String toString() const {

			CppUtils::String result = "STATE: \n";
			result += " [ "+ symbol_m + " ] " + state_m.getName(); 
		
			return result;
		}

		// members
	public:

			
		CppUtils::String symbol_m;

		CppUtils::EnumerationHelper state_m;
		
	};

} // end of namespace

#endif