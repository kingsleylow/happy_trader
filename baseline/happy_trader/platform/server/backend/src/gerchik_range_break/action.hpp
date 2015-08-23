#ifndef _GERCHIK_RANGE_BREAK_ACTION_INCLUDED
#define _GERCHIK_RANGE_BREAK_ACTION_INCLUDED

#include "../brklib/brklib.hpp"
#include "utils.hpp"

namespace AlgLib {

	


	
	DEFINE_ENUM_ENTRY(AS_Dummy,0 )
	DEFINE_ENUM_ENTRY(AS_IssueEnterBothOrder,1 )
	DEFINE_ENUM_ENTRY(AS_IssueCancelEnterOtherSideOrder,2 )
	DEFINE_ENUM_ENTRY(AS_IssueSLTPOrder,3 )
	DEFINE_ENUM_ENTRY(AS_IssueCancelSLOrder,4 )
	DEFINE_ENUM_ENTRY(AS_IssueCancelBothOrder,5 )
	DEFINE_ENUM_ENTRY(AS_IssueCancelTPOrder,6 )
	DEFINE_ENUM_ENTRY(AS_ClearBrokerContext,7 )
	
	
	struct Action {

		
		Action():
			action_m(AS_Dummy),
			opSLInstallPrice_m(-1),
			opTPInstallPrice_m(-1),
			opBothSideLongInstallPrice_m(-1),
			opBothSideShortInstallPrice_m(-1),
			volume_m(-1)
		{
		}

		

		// if is long is not relevant
		Action(CppUtils::EnumerationHelper const& action, CppUtils::String const& symbol):
			action_m(action),
			symbol_m( symbol),
			opSLInstallPrice_m(-1),
			opTPInstallPrice_m(-1),
			opBothSideLongInstallPrice_m(-1),
			opBothSideShortInstallPrice_m(-1),
			volume_m(-1)
		{
		}

		void clear()
		{
			opSLInstallPrice_m = -1;
			opTPInstallPrice_m = -1;
			opBothSideLongInstallPrice_m = -1;
			opBothSideShortInstallPrice_m = -1;

			action_m = AS_Dummy;
			symbol_m.clear();
			volume_m = -1;
		

		}

		CppUtils::String toString() const {
			CppUtils::String result  ="ACTION: \n";
			
			result += " [ "+ symbol_m + " ] " + action_m.getName(); 
	
			// some vars
			result += "\nACTION VARIABLES [ :\n";
			
			result += CppUtils::String("opSLInstallPrice_m=") + CppUtils::float2String(opSLInstallPrice_m,-1,4)+"\n";
			result += CppUtils::String("opTPInstallPrice_m=") + CppUtils::float2String(opTPInstallPrice_m,-1,4)+"\n";
			result += CppUtils::String("opBothSideLongInstallPrice_m=") + CppUtils::float2String(opBothSideLongInstallPrice_m,-1,4)+"\n";
			result += CppUtils::String("opBothSideShortInstallPrice_m=") + CppUtils::float2String(opBothSideShortInstallPrice_m,-1,4)+"\n";

			result += CppUtils::String("volume_m=") + CppUtils::float2String(volume_m,-1,4)+"\n";
		
						
			result += " ] ACTION END\n";
		

			return result;
		}
	
		// members
	public:
		//AS action_m;
		CppUtils::EnumerationHelper action_m;

		CppUtils::String symbol_m;

		// some vars - 
		// entre limit prices
		double opSLInstallPrice_m;

		double opTPInstallPrice_m;

		double opBothSideLongInstallPrice_m;

		double opBothSideShortInstallPrice_m;

		double volume_m;

	


			
	};


}

#endif