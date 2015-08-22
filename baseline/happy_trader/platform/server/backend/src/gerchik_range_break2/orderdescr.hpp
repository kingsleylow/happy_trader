#ifndef _GERCHIK_RANGE_BREAK2_ORDERDESCR_INCLUDED
#define _GERCHIK_RANGE_BREAK2_ORDERDESCR_INCLUDED

#include "../brklib/brklib.hpp"
#include "utils.hpp"


namespace AlgLib {

	// direction of trade
	DEFINE_ENUM_ENTRY(DIRECTION_DUMMY,	-1 )
	DEFINE_ENUM_ENTRY(DIRECTION_SHORT,	1 )
	DEFINE_ENUM_ENTRY(DIRECTION_LONG,		2 )

	class OrderDescriptor {

	public:

		OrderDescriptor()
		{
			clear();
		}

		void clear()
		{
			orderIssuePrice_m = -1;
			orderIssueTime_m = -1;
			orderInstallTime_m = -1;
			orderInstallPrice_m = -1;
			orderExecuteTime_m = -1;
			orderExecutePrice_m = -1;
			orderCancelTime_m = -1;
			orderCancelPrice_m = -1;
			orderErrorTime_m = -1;
			direction_m = DIRECTION_DUMMY;
			volume_m  = -1;
			orderID_m.invalidate();
			errorReason_m.clear();
			orderIsDelayedLocally_m = false;
		};

		inline bool isInstalled() const
		{
			return orderInstallTime_m > 0;
		}

		inline bool isExecuted() const
		{
			return orderExecuteTime_m > 0;
		}

		inline bool isCancelled() const
		{
			return orderCancelTime_m > 0;
		};

		inline bool isErrored() const
		{
			return orderErrorTime_m > 0;
		}

		CppUtils::String toString() const
		{
			CppUtils::String result = "ORDER DESCRIPTOR: \n";
			result += "symbol: " + symbol_m + "\n";

			result += "issue price: " + CppUtils::float2String(orderIssuePrice_m, -1, 4) + "\n";
			result += "issue time: " + CppUtils::getGmtTime(orderIssueTime_m) + "\n";

			if (isInstalled()) {
				result += "install time: " + CppUtils::getGmtTime(orderInstallTime_m) + "\n";
				result += "install price: " + CppUtils::float2String(orderInstallPrice_m, -1, 4) + "\n";
			}
			else {
				result += "not installed: \n";
			}
			
			if (isExecuted()) {
				result += "execute time: " + CppUtils::getGmtTime(orderExecuteTime_m) + "\n";
				result += "execute price: " + CppUtils::float2String(orderExecutePrice_m, -1, 4) + "\n";
			}
			else {
				result += "not executed: \n";
			}

			if (isCancelled()) {
				result += "cancel time: " + CppUtils::getGmtTime(orderCancelTime_m) + "\n";
				result += "cancel price: " + CppUtils::float2String(orderCancelPrice_m, -1, 4) + "\n";
			}

			if (isErrored()) {
				result += "error time: " + CppUtils::getGmtTime(orderErrorTime_m) + "\n";
				result += "reson: " + CppUtils::getGmtTime(orderErrorTime_m) + "\n";
			}

			result += CppUtils::String("broker position ID: ") + brokerPositionId_m + "\n";
			result += CppUtils::String("direction: ") + direction_m.getName() + "\n";
			result += CppUtils::String("order method: ") + orderMethod_m + "\n";
			result += CppUtils::String("order place: ") + orderPlace_m + "\n";
			result += CppUtils::String("additional order info: ") + additionalOrderInfo_m + "\n";
			//result += CppUtils::String("order type: ") + BrkLib::getOrderTypeName(orderType_m)+ "\n";
			result += CppUtils::String("volume: ") + CppUtils::float2String(volume_m, -1, 4) + "\n";
			result += CppUtils::String("order ID: ") + orderID_m.toString() + "\n";
			result += CppUtils::String("order is delayed locally: ") + (orderIsDelayedLocally_m ? "true":"false") + "\n";

			result += "ORDER DESCRIPTOR END\n";

			return result;
		}

		// -------------

		// this will be passed as order issue price
		double orderIssuePrice_m;
		
		double orderIssueTime_m;

		double orderInstallTime_m;

		double orderInstallPrice_m;

		double orderExecuteTime_m;

		double orderExecutePrice_m;

		double orderCancelTime_m;

		double orderCancelPrice_m;

		double orderErrorTime_m;

		CppUtils::String errorReason_m;

		BrkLib::OrderType orderType_m;

		CppUtils::EnumerationHelper direction_m;

		CppUtils::String orderMethod_m;

		CppUtils::String orderPlace_m;

		CppUtils::String additionalOrderInfo_m;

		bool orderIsDelayedLocally_m;

		double volume_m;

		CppUtils::Uid orderID_m;

		CppUtils::String brokerPositionId_m;

		CppUtils::String symbol_m;

	};

};

#endif