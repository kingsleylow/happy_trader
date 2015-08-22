#ifndef _MGERCHIK_ACTION_INCLUDED
#define _MGERCHIK_ACTION_INCLUDED

#include "../brklib/brklib.hpp"

namespace AlgLib {
	struct Action {
		enum AS {
			AS_Dummy=0,
			AS_IssueCloseLong = 1,
			AS_IssueCloseShort =2,

			AS_IssueCancelLong = 3,
			AS_IssueCancelShort =4,

			AS_IssueClose2Long = 5,
			AS_IssueClose2Short = 6,

			AS_IssueOpenLong =	7,
			AS_IssueOpenShort	=	8,

			AS_ClearBrokerContext = 9,

			AS_IssueClose3Long =	10,
			AS_IssueClose3Short	=	11
		};

		Action():
			action_m(AS_Dummy),
			opPrice_m(-1),
			volume_m(-1)
		{
		}

		Action(AS const action, CppUtils::String const& symbol):
			action_m(action),
			symbol_m( symbol),
			opPrice_m(-1),
			volume_m(-1)
		{
		}

		void clear()
		{
			opPrice_m = -1;
			volume_m = -1;
			action_m = AS_Dummy;
			symbol_m.clear();

		}

		CppUtils::String toString() const {
			CppUtils::String result  ="ACTION: \n";

			switch(action_m) {
				case AS_Dummy: result = " [ "+ symbol_m + " ] " + "AS_Dummy"; break;
				
				case AS_IssueCloseLong: result += " [ "+  symbol_m + " ] " + "AS_IssueCloseLong"; break;
				case AS_IssueCloseShort: result += " [ "+  symbol_m + " ] " + "AS_IssueCloseShort"; break;
				
				case AS_IssueCancelLong: result += " [ "+  symbol_m + " ] " + "AS_IssueCancelLong"; break;
				case AS_IssueCancelShort: result += " [ "+  symbol_m + " ] " + "AS_IssueCancelShort"; break;

				case AS_IssueClose2Long: result += " [ "+  symbol_m + " ] " + "AS_IssueClose2Long"; break;
				case AS_IssueClose2Short: result += " [ "+  symbol_m + " ] " + "AS_IssueClose2Short"; break;

				case AS_IssueOpenLong: result += " [ "+  symbol_m + " ] " + "AS_IssueOpenLong"; break;
				case AS_IssueOpenShort: result += " [ "+  symbol_m + " ] " + "AS_IssueOpenShort"; break;

				case AS_ClearBrokerContext: result += " [ "+  symbol_m + " ] " + "AS_ClearBrokerContext"; break;

				case AS_IssueClose3Long: result += " [ "+  symbol_m + " ] " + "AS_IssueClose3Long"; break;
				case AS_IssueClose3Short: result += " [ "+  symbol_m + " ] " + "AS_IssueClose3Short"; break;

				default:
					result += " [ "+ symbol_m + " ] " + "N/A"; 
			}

			result += "\n";

			// some vars
			result += CppUtils::String("opPrice_m=") + CppUtils::float2String(opPrice_m,-1,4)+"\n";
			result += CppUtils::String("volume_m=") + CppUtils::float2String(volume_m,-1,4)+"\n";
			result += "ACTION END\n";
		

			return result;
		}
	
		// members
	public:
		AS action_m;

		CppUtils::String symbol_m;

		// some vars - 
		// entre limit prices
		double opPrice_m;

		double volume_m;
	
	};


}

#endif