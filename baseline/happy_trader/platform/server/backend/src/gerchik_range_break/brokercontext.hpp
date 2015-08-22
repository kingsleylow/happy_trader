#ifndef _GERCHIK_RANGE_BREAK_BROKERCONTEXT_INCLUDED
#define _GERCHIK_RANGE_BREAK_BROKERCONTEXT_INCLUDED

#include "gerchik_range_breakdefs.hpp"
//#include "../brklib/brklib.hpp"
#include "action.hpp"
#include "utils.hpp"
#include "orderdescr.hpp"


namespace AlgLib {

	// direction of trade
	DEFINE_ENUM_ENTRY(DIRECTION_DUMMY,	-1 )
	DEFINE_ENUM_ENTRY(DIRECTION_SHORT,	1 )
	DEFINE_ENUM_ENTRY(DIRECTION_LONG,		2 )

		// this is helper structure to hold broker context
	struct BrokerContext
	{
		
		BrokerContext()
		{
			clear();
		}

		void clear()
		{
			
			action_m.clear();
			orderWorkLong_m.clear();
			
			orderWorkShort_m.clear();

			orderSL_m.clear();

			orderTP_m.clear();

			directionOfTrade_m = DIRECTION_DUMMY;
			
		}

		OrderDescriptor & resolveWorkingOrderViaId(CppUtils::Uid const& uid)
		{
			if (orderWorkLong_m.orderID_m == uid)
				return orderWorkLong_m;
			else if (orderWorkShort_m.orderID_m == uid)
				return orderWorkShort_m;
			else
				THROW(CppUtils::OperationFailed, "exc_CannotResolveOrderContextViaResponseID", "ctx_ResolveWorkingOrderContext", uid.toString() );
		}

		OrderDescriptor & resolveSLTPOrderViaId(CppUtils::Uid const& uid)
		{
			if (orderSL_m.orderID_m == uid)
				return orderSL_m;
			else if (orderTP_m.orderID_m == uid)
				return orderTP_m;
			else
				THROW(CppUtils::OperationFailed, "exc_CannotResolveOrderContextViaResponseID", "ctx_ResolveSLTPOrderContext", uid.toString() );
		}

	public:
			
		// action caused that context - e.i - this is the curren t opertaion agains broker
		Action action_m;

		// working orders
		OrderDescriptor orderWorkLong_m;

		OrderDescriptor orderWorkShort_m;

		OrderDescriptor orderSL_m;

		OrderDescriptor orderTP_m;

		// shoes the main direction of trade
		// so if long - e.i TP is long SL is short
		CppUtils::EnumerationHelper directionOfTrade_m;

	public:

		CppUtils::String toString() const
		{
			CppUtils::String result = "BROKER CONTEXT: \n";

			result += "orderWorkLong_m=" + orderWorkLong_m.toString() + "\n";
			result += "orderWorkShort_m=" + orderWorkShort_m.toString() + "\n";
			result += "orderSL_m=" + orderSL_m.toString() + "\n";
			result += "orderTP_m=" + orderTP_m.toString() + "\n";
			result += CppUtils::String("directionOfTrade_m=") +  directionOfTrade_m.getName() + "\n";

			result += "action_m=" + action_m.toString() + "\n";
			result += "BROKER CONTEXT END\n";

			return result;
		}
	
	};




}; // end of namepsace

#endif