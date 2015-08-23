#ifndef _GERCHIK_RANGE_BREAK2_STATE_INCLUDED
#define _GERCHIK_RANGE_BREAK2_STATE_INCLUDED

#include "../brklib/brklib.hpp"
#include "utils.hpp"


namespace AlgLib {

	DEFINE_ENUM_ENTRY(TS_Dummy,0 )
	DEFINE_ENUM_ENTRY(TS_Initial,1 )
	DEFINE_ENUM_ENTRY(TS_Issued,2 )
	DEFINE_ENUM_ENTRY(TS_Pending,3 )
	DEFINE_ENUM_ENTRY(TS_Open,4 )
	DEFINE_ENUM_ENTRY(TS_Cancelling,5 )
	DEFINE_ENUM_ENTRY(TS_Inconsistent,6 )
	DEFINE_ENUM_ENTRY(TS_Delayed,7 )

	class TradingState: public EnumerationHelperBase {
	public:

		TradingState():
			EnumerationHelperBase(TS_Initial)
		{
		}

		TradingState(CppUtils::EnumerationHelper const& state  ):
			EnumerationHelperBase(state)
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

		virtual void clear()
		{
			getMember() = TS_Initial;
		}
		
		inline CppUtils::EnumerationHelper const& getState() const
		{
			return getMember();
		}

		inline CppUtils::EnumerationHelper& getState()
		{
			return  getMember();
		}

		// members
	private:
			
		CppUtils::EnumerationHelper state_m;
		
	};

} // end of namespace

#endif