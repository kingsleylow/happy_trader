#ifndef _SMARTCOM_ALG_MISHA_EVENT_INCLUDED
#define _SMARTCOM_ALG_MISHA_EVENT_INCLUDED

#include "smartcom_alg_mishadefs.hpp"
#include "statemachinecontext.hpp"



namespace AlgLib {

	class TE_On_Timer: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE_On_Timer);

		
	};

	class TE_On_Tick_Data: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE_On_Tick_Data);

		double curPrice_m;
		double prevPrice_m;
	};

	class TE_On_Level2_Data: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE_On_Tick_Data);

		double curTickPrice_m;
		HlpStruct::RtLevel2Data level2_m;

	};


	// events for main machine
	class TE_Detected_Big_Volume: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE_Detected_Big_Volume);

		// override
		virtual CppUtils::String toString() const
		{
			CppUtils::String result;
			result += " [ " + getClassName2() + " ]\n"; 
			result += "trackingPrice_m=" + CppUtils::simpleFloat2String(trackingPrice_m) + "\n";
			result += "trackingVolume_m=" + CppUtils::simpleFloat2String(trackingVolume_m) + "\n";
			result += "volumeDetectTime_m=" + CppUtils::getGmtTime2(volumeDetectTime_m) + "\n";

			return result;
		}
		
		double volumeDetectTime_m;
		double trackingPrice_m;
		double trackingVolume_m;

	};

	class TE_Price_Go_Far: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE_Price_Go_Far);

		virtual CppUtils::String toString() const
		{
			CppUtils::String result;
			result += " [ " + getClassName2() + " ]\n"; 
			result += "curPrice_m=" + CppUtils::simpleFloat2String(curPrice_m) + "\n";
			result += "trackingPrice_m=" + CppUtils::simpleFloat2String(trackingPrice_m) + "\n";
		
			return result;
		}

		double curPrice_m;
		double trackingPrice_m;
		
	};

	class TE_Reset: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE_Reset);

	
		
	};

	class TE_Price_Crossed_Up: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE_Price_Crossed_Up);

		
		
	};

	class TE_Price_Crossed_Down: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE_Price_Crossed_Down);

		

	};

	class TE_Confirm_Orders_Setup_Long: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE_Confirm_Orders_Setup_Long);

		
	};

	class TE_Confirm_Orders_Setup_Short: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE_Confirm_Orders_Setup_Short);

		
	};

	class TE_Confirm_Orders_Setup_Error_Long: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE_Confirm_Orders_Setup_Error_Long);

		
	};

	class TE_Confirm_Orders_Setup_Error_Short: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE_Confirm_Orders_Setup_Error_Short);

		

	};

	class TE_Clear_Long: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE_Clear_Long);
	};

	class TE_Clear_Short: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE_Clear_Short);
	};

	

	class TE_Manual_Fix: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE_Manual_Fix);
	};

	class TE_Detect_Time_Excess: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE_Detect_Time_Excess);

		virtual CppUtils::String toString() const
		{
			CppUtils::String result;
			result += " [ " + getClassName2() + " ]\n"; 
			result += "secElapsed_m=" + CppUtils::simpleFloat2String(secElapsed_m) + "\n";
				
			return result;
		}

		double secElapsed_m;
	};

	// -------------------------------------------------
	// events for nested machine responsible for broker
	// -------------------------------------------------

	class TE2_Start_Signal: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE2_Start_Signal);

		virtual CppUtils::String toString() const
		{
			CppUtils::String result;
			result += " [ " + getClassName2() + " ]\n"; 
			result += "direction_m=" + BrkLib::tradeDirection2String(direction_m) + "\n";
			return result;
		};


		// the only event point with direction
		int direction_m;
	};

	class TE2_Confirm_Order_Issued_Open: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE2_Confirm_Order_Issued_Open);

		// when this is pending order we can detect order ID by which we will be identifying pending orders
		CppUtils::String brokerOrderId_m;

		
	};

	

	class TE2_Confirm_Order_Executed_Open: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE2_Confirm_Order_Executed_Open);

	
		CppUtils::String brokerOrderId_m;
		CppUtils::String brokerPositionId_m;
		double executionPrice_m;
	};

	class TE2_Confirm_Order_Issued_TP: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE2_Confirm_Order_Issued_TP);

		CppUtils::String brokerOrderId_m;
		
	};

	class TE2_Confirm_Order_Issued_SL: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE2_Confirm_Order_Issued_SL);

	
		CppUtils::String brokerOrderId_m;
		
	};

	// sometimes SMARTCOM do not give install notification so try to rely on pending
	class TE2_Confirm_Order_Pending_SL: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE2_Confirm_Order_Pending_SL);

	
		CppUtils::String brokerOrderId_m;
		
	};

	class TE2_Confirm_Order_Executed_TP: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE2_Confirm_Order_Executed_TP);
	};

	class TE2_Confirm_Order_Executed_SL: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE2_Confirm_Order_Executed_SL);
	};

	

	class TE2_Confirm_Order_Error_SL: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE2_Confirm_Order_Error_SL);

		CppUtils::String toString()	const
		{
			CppUtils::String result;
			result += " [ " + getClassName2() + " ]\n"; 
			result += "brokerOrderId_m=" + brokerOrderId_m + "\n";
			result += "reason=" + reason + "\n";

			return result;
		}

		CppUtils::String brokerOrderId_m;
		CppUtils::String reason;
		
	
	};

	class TE2_Confirm_Order_Error_Open: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE2_Confirm_Order_Error_Open);

		CppUtils::String toString()	const
		{
			CppUtils::String result;
			result += " [ " + getClassName2() + " ]\n"; 
			result += "brokerOrderId_m=" + brokerOrderId_m + "\n";
			result += "reason=" + reason + "\n";

			return result;
		}

		CppUtils::String brokerOrderId_m;
		CppUtils::String reason;
	
	};

	

	class TE2_Confirm_Order_Error_TP: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE2_Confirm_Order_Error_TP);

		CppUtils::String toString()	const
		{
			CppUtils::String result;
			result += " [ " + getClassName2() + " ]\n"; 
			result += "brokerOrderId_m=" + brokerOrderId_m + "\n";
			result += "reason=" + reason + "\n";

			return result;
		}

		CppUtils::String brokerOrderId_m;
		CppUtils::String reason;
	
	};

	

	class TE2_Confirm_Order_Error_Cancel_TP: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE2_Confirm_Order_Error_Cancel_TP);

		CppUtils::String toString()	const
		{
			CppUtils::String result;
			result += " [ " + getClassName2() + " ]\n"; 
			result += "brokerOrderId_m=" + brokerOrderId_m + "\n";
			result += "reason=" + reason + "\n";

			return result;
		}

		CppUtils::String brokerOrderId_m;
		CppUtils::String reason;
	
	};

	class TE2_Confirm_Order_Error_Cancel_SL: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE2_Confirm_Order_Error_Cancel_SL);

		CppUtils::String toString()	const
		{
			CppUtils::String result;
			result += " [ " + getClassName2() + " ]\n"; 
			result += "brokerOrderId_m=" + brokerOrderId_m + "\n";
			result += "reason=" + reason + "\n";

			return result;
		}

		CppUtils::String brokerOrderId_m;
		CppUtils::String reason;
	
	};

	class TE2_Confirm_Order_Cancelled_TP: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE2_Confirm_Order_Cancelled_TP);
	};

	class TE2_Confirm_Order_Cancelled_SL: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE2_Confirm_Order_Cancelled_SL);
	};

	class TE2_Common_Error_SL: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE2_Common_Error_SL);

		CppUtils::String brokerOrderId_m;
		CppUtils::String reason;

	};

	class TE2_Error_Invalid_Stop_Price_SL: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE2_Error_Invalid_Stop_Price_SL);

		CppUtils::String brokerOrderId_m;
		CppUtils::String reason;
		
	};

	class TE2_Confirm_Order_Executed_Market_Close: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE2_Confirm_Order_Executed_Market_Close);

	
	};

	class TE2_Confirm_Order_Error_Market_Close: public CppUtils::BaseEvent
	{
		public:
		CTOR(TE2_Confirm_Order_Error_Market_Close);

		CppUtils::String reason_m;
	};

};

#endif