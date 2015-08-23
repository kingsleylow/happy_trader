#ifndef _MGERCHIK_BROKERCONTEXT_INCLUDED
#define _MGERCHIK_BROKERCONTEXT_INCLUDED

#include "../brklib/brklib.hpp"
#include "action.hpp"

namespace AlgLib {

		// this is helper structure to hold broker context
	struct BrokerContext
	{
		
		BrokerContext():
			opBuyLimit_m(-1),
			opSellLimit_m(-1),
			close1LongPrice_m(-1),
			close1ShortPrice_m(-1),
			volume_m(-1),
			close2LongPrice_m(-1),
			close2ShortPrice_m(-1),
			brokerOpenPrice_m(-1),
			brokerClosePrice_m(-1),
			brokerInstallPrice_m(-1),
			isLong_m(false),
			posOpenTime_m(-1),
			posInstallTime_m(-1)
		{
		}

		void clear()
		{
			opBuyLimit_m  = -1;
			opSellLimit_m  =-1;
			close1LongPrice_m = -1;
			close1ShortPrice_m = -1;
			volume_m = -1;
			close2LongPrice_m = -1;
			close2ShortPrice_m = -1;
			brokerOpenPrice_m = -1;
			brokerClosePrice_m = -1;
			brokerInstallPrice_m = -1;
			isLong_m = false;
			posOpenTime_m = -1;
			posInstallTime_m = -1;



			action_m.clear();
		}
	public:

		// this is to simulate tradig - open position price which is calculated every step
		// actually this must be calculated upon signal retreival
		double opBuyLimit_m;

		double opSellLimit_m;

		// close price
		double close1LongPrice_m;

		double close1ShortPrice_m;

		// close 2 prices
		double close2LongPrice_m;

		double close2ShortPrice_m;
			
		// action caused that context - e.i - this is the curren t opertaion agains broker
		Action action_m;

		double volume_m;

		// this is the actual open and close prices - e.i. the prices broker gave
		
		double brokerInstallPrice_m; // for pending orders

		double brokerOpenPrice_m;

		double brokerClosePrice_m;

		bool isLong_m; // if the pos was long ?

		// position open time
		double posOpenTime_m;

		// install time
		double posInstallTime_m;

	public:

		CppUtils::String toString() const
		{
			CppUtils::String result = "BROKER CONTEXT: \n";

			result += "opBuyLimit_m=" + CppUtils::float2String(opBuyLimit_m,-1,4) + "\n";
			result += "opSellLimit_m=" + CppUtils::float2String(opSellLimit_m,-1,4) + "\n";
			result += "close1LongPrice_m=" + CppUtils::float2String(close1LongPrice_m,-1,4) + "\n";
			result += "close1ShortPrice_m=" + CppUtils::float2String(close1ShortPrice_m,-1,4) + "\n";
			result += "close2LongPrice_m=" + CppUtils::float2String(close2LongPrice_m,-1,4) + "\n";
			result += "close2ShortPrice_m=" + CppUtils::float2String(close2ShortPrice_m,-1,4) + "\n";
			
			result += "brokerInstallPrice_m=" + CppUtils::float2String(brokerInstallPrice_m,-1,4) + "\n";
			result += "brokerOpenPrice_m=" + CppUtils::float2String(brokerOpenPrice_m,-1,4) + "\n";
			result += "brokerClosePrice_m=" + CppUtils::float2String(brokerClosePrice_m,-1,4) + "\n";

			result += "posOpenTime_m=" + CppUtils::float2String(posOpenTime_m,-1,4) + "\n";
			result += "posInstallTime_m=" + CppUtils::float2String(posInstallTime_m,-1,4) + "\n";

			result += "isLong_m=" + CppUtils::String((isLong_m ? "true":"false")) + "\n";

			result += "volume_m=" + CppUtils::float2String(volume_m,-1,4) + "\n";
			result += "action_m=" + action_m.toString() + "\n";
			result += "BROKER CONTEXT END\n";

			return result;
		}
	
	};




}; // end of namepsace

#endif