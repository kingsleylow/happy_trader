#ifndef _GERCHIK_RANGE_BREAK_ORDERDESCR_INCLUDED
#define _GERCHIK_RANGE_BREAK_ORDERDESCR_INCLUDED

#include "../brklib/brklib.hpp"
#include "utils.hpp"


namespace AlgLib {

	class OrderDescriptor {

	public:

		OrderDescriptor()
		{
			clear();
		}

		void clear()
		{
			orderInstallTime_m = -1;
			orderInstallPrice_m = -1;
			orderExecuteTime_m = -1;
			orderExecutePrice_m = -1;
			orderCancelTime_m = -1;
			orderErrorTime_m = -1;
			//isLong_m = DIRECTION_DUMMY;
			//volume_m  = -1;
			orderID_m.invalidate();
			errorReason_m.clear();
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
			}

			if (isErrored()) {
				result += "error time: " + CppUtils::getGmtTime(orderErrorTime_m) + "\n";
				result += "reson: " + CppUtils::getGmtTime(orderErrorTime_m) + "\n";
			}

			//result += CppUtils::String("direction: ") + resolveDirectionConstant(isLong_m) + "\n";
			//result += CppUtils::String("volume: ") + CppUtils::float2String(volume_m, -1, 4) + "\n";
			result += CppUtils::String("order ID: ") + orderID_m.toString() + "\n";

			result += "ORDER DESCRIPTOR END\n";

			return result;
		}

		// -------------

		double orderInstallTime_m;

		double orderInstallPrice_m;

		double orderExecuteTime_m;

		double orderExecutePrice_m;

		double orderCancelTime_m;

		double orderErrorTime_m;

		//int isLong_m;

		//double volume_m;

		CppUtils::Uid orderID_m;

		CppUtils::String errorReason_m;



	};

};

#endif