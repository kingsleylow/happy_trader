#ifndef _GERCHIK_RANGE_BREAK2_ACTION_INCLUDED
#define _GERCHIK_RANGE_BREAK2_ACTION_INCLUDED

#include "../brklib/brklib.hpp"
#include "orderdescr.hpp"

namespace AlgLib {

	
	DEFINE_ENUM_ENTRY(AS_Dummy,0 )
	DEFINE_ENUM_ENTRY(AS_Clear,1)
	DEFINE_ENUM_ENTRY(AS_IssueOrder,2 )
	DEFINE_ENUM_ENTRY(AS_CancelOrder,3 )
	DEFINE_ENUM_ENTRY(AS_RemoveDelayedOrder,4 )
		
	
	class Action :public EnumerationHelperBase{

	public:
		Action():
			EnumerationHelperBase(AS_Dummy)
		{
		}

		Action(CppUtils::EnumerationHelper const& actionId):
			EnumerationHelperBase(actionId)
		{
		}

		virtual void clear()
		{
			getMember() = AS_Dummy;
		
		}

		inline CppUtils::EnumerationHelper const& getAction() const
		{
			return getMember();
		}
	
	
			
	};


}

#endif