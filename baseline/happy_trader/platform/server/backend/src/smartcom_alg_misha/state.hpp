#ifndef _SMARTCOM_ALG_MISHA_MMSTATE_INCLUDED
#define _SMARTCOM_ALG_MISHA_MMSTATE_INCLUDED

#include "smartcom_alg_mishadefs.hpp"
#include "statemachinecontext.hpp"
#include "event.hpp"

namespace AlgLib {


	// states for main machine
	class TS_Initial: public CppUtils::BaseState<MachineContext>
	{
	public:
		CTOR(TS_Initial);

		virtual bool entry_action(CppUtils::BaseEvent const& event);
	
	};	

	class TS_Inconsistent: public CppUtils::BaseState<MachineContext>
	{
	public:
		CTOR(TS_Inconsistent);

		
	};

	class TS_LevelTrack: public CppUtils::BaseState<MachineContext>
	{
	public:
		CTOR(TS_LevelTrack);

		virtual bool entry_action(CppUtils::BaseEvent const& event);
		
		

		double volumeDetectTime_m;
		double trackingPrice_m;
		double trackingVolume_m;

	};

	class TS_IssueBuy: public CppUtils::BaseState<MachineContext>
	{
	public:
		CTOR(TS_IssueBuy);

		// here we must communicate with order another machine
		virtual bool entry_action(CppUtils::BaseEvent const& event);
		
		
	};

	class TS_IssueSell: public CppUtils::BaseState<MachineContext>
	{
	public:
		CTOR(TS_IssueSell);	

		// here we must communicate with order another machine
		virtual bool entry_action(CppUtils::BaseEvent const& event);
		
		
	};

	class TS_Fish_Long: public CppUtils::BaseState<MachineContext>
	{
	public:
		CTOR(TS_Fish_Long);	

		
	};

	class TS_Fish_Short: public CppUtils::BaseState<MachineContext>
	{
	public:
		CTOR(TS_Fish_Short);

		
	};

	// states for nested machine
	// -------------------------------------------

	class TS2_Initial: public CppUtils::BaseState<MachineContext>
	{
	public:
		CTOR(TS2_Initial);

		
	};

	class TS2_Ready_Start: public CppUtils::BaseState<MachineContext>
	{
	public:
		CTOR(TS2_Ready_Start);

		virtual bool entry_action(CppUtils::BaseEvent const& event);
		
		virtual CppUtils::String toString() const
		{
			CppUtils::String result;
			result += " [ " + getClassName2() + " ]\n"; 
			result += "direction_m=" + BrkLib::tradeDirection2String(direction_m) + "\n";
			return result;
		};
	
		// the only point to use as direction
		int direction_m;
	};

	class TS2_Inconsistent: public CppUtils::BaseState<MachineContext>
	{
	public:
		CTOR(TS2_Inconsistent);

		virtual bool entry_action(CppUtils::BaseEvent const& event);
		
	
	};

	class TS2_Pending: public CppUtils::BaseState<MachineContext>
	{
	public:
		CTOR(TS2_Pending);

	
	};

	class TS2_In_Position: public CppUtils::BaseState<MachineContext>
	{
	public:
		CTOR(TS2_In_Position);

		virtual bool entry_action(CppUtils::BaseEvent const& event);
		
		
		virtual CppUtils::String toString() const
		{
			CppUtils::String result;
			result += " [ " + getClassName2() + " ]\n"; 
			result += "sl_brokerOrderId_m=" + sl_brokerOrderId_m + "\n";
			return result;
		};

		
		CppUtils::String sl_brokerOrderId_m;
	};

	class TS2_SL_Pending: public CppUtils::BaseState<MachineContext>
	{
	public:
		CTOR(TS2_SL_Pending);

		virtual bool entry_action(CppUtils::BaseEvent const& event);
	
		virtual CppUtils::String toString() const
		{
			CppUtils::String result;
			result += " [ " + getClassName2() + " ]\n"; 
			result += "brokerOpenPositionId_m=" + brokerOpenPositionId_m + "\n";
			result += "openPrice_m=" + CppUtils::simpleFloat2String(openPrice_m) + "\n";
			return result;
		};

		CppUtils::String brokerOpenPositionId_m; // our open position
		double openPrice_m;
		
	};

	class TS2_Ready: public CppUtils::BaseState<MachineContext>
	{
	public:
		CTOR(TS2_Ready);

		virtual bool entry_action(CppUtils::BaseEvent const& event);
	
		virtual CppUtils::String toString() const
		{
			CppUtils::String result;
			result += " [ " + getClassName2() + " ]\n"; 
			result += "tp_brokerOrderId_m=" + tp_brokerOrderId_m + "\n";
			return result;
		};

		CppUtils::String tp_brokerOrderId_m;
		
	};

	class TS2_Need_Clear_TP: public CppUtils::BaseState<MachineContext>
	{
	public:
		CTOR(TS2_Need_Clear_TP);

		virtual bool entry_action(CppUtils::BaseEvent const& event);
		
	
	};

	class TS2_Need_Clear_SL: public CppUtils::BaseState<MachineContext>
	{
	public:
		CTOR(TS2_Need_Clear_SL);

		virtual bool entry_action(CppUtils::BaseEvent const& event);
		
	
	};

	class TS2_Trade_Finished: public CppUtils::BaseState<MachineContext>
	{
	public:
		CTOR(TS2_Trade_Finished);

		virtual bool entry_action(CppUtils::BaseEvent const& event);
		
	
	};

	class TS2_Check_SL_Error: public CppUtils::BaseState<MachineContext>
	{
	public:
		CTOR(TS2_Check_SL_Error);

		virtual bool entry_action(CppUtils::BaseEvent const& event);
		
	
	};

	class TS2_Close_By_Market: public CppUtils::BaseState<MachineContext>
	{
	public:
		CTOR(TS2_Close_By_Market);

		virtual bool entry_action(CppUtils::BaseEvent const& event);
		
	
	};



} // end of namespace

#endif