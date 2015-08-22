#ifndef _PAIR_TRADE_ACTION_INCLUDED
#define _PAIR_TRADE_ACTION_INCLUDED

#include "pair_tradedefs.hpp"
#include "statemachinecontext.hpp"
#include "event.hpp"
#include "brokerthroughcontext.hpp"


namespace AlgLib {

/**
* Base class to delegate broker responses
*/

class BrokerResponseRecipient
{
	public:

	virtual void onCallbackReceived(
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
	)
	{
	};

	static void onUnknownCallbackreceived(
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
	);

};


// -----------------------------

//--- EXIT ACTION ---------------
class AE_Init_Context: public CppUtils::BaseExitAction<MachineContext> {
	public:
		CTOR(AE_Init_Context);
	// --- exit map--
	BEGIN_EVENT_MAP()
		EVENT_MAP(TE_Confirm_Open_Long, TE_Confirm_Open_Long_ExitHandler)
		EVENT_MAP(TE_Confirm_Open_Short, TE_Confirm_Open_Short_ExitHandler)
		EVENT_MAP(TE_Confirm_Open_Nothing, TE_Confirm_Open_Nothing_ExitHandler)
	END_EVENT_MAP()

	// ------------- exit handlers ------------

	bool TE_Confirm_Open_Short_ExitHandler(TE_Confirm_Open_Short const& e);
	bool TE_Confirm_Open_Nothing_ExitHandler(TE_Confirm_Open_Nothing const& e);
	bool TE_Confirm_Open_Long_ExitHandler(TE_Confirm_Open_Long const& e);

private:
	void clear();
};


//--- ENTRY ACTION ---------------
class A_Completed: public CppUtils::BaseEnterAction<MachineContext>, public BrokerResponseRecipient {
	public:
		CTOR(A_Completed);
	// --- enter map--
	BEGIN_EVENT_MAP()
		EVENT_MAP(TE_Executed_All,TE_Executed_All_EnterHandler)
	END_EVENT_MAP()

	// ------------- enter handlers ------------

	bool TE_Executed_All_EnterHandler(TE_Executed_All const& e);
};

//--- TRANSITION ACTION ---------------
class AT_Check_All_Executed: public CppUtils::BaseTransitionAction<MachineContext>, public BrokerResponseRecipient {
	public:
		CTOR(AT_Check_All_Executed);
		virtual bool execute(CppUtils::BaseEvent const& eventToProcess);
};

//--- ENTRY ACTION ---------------
class A_Issue_Pair_Open: public CppUtils::BaseEnterAction<MachineContext>, public BrokerResponseRecipient {
	public:
		CTOR(A_Issue_Pair_Open);
	// --- enter map--
	BEGIN_EVENT_MAP()
		EVENT_MAP(TE_Signal_Long,TE_Signal_Long_EnterHandler)
		EVENT_MAP(TE_Signal_Short,TE_Signal_Short_EnterHandler)
	END_EVENT_MAP()

	// ------------- enter handlers ------------

	bool TE_Signal_Short_EnterHandler(TE_Signal_Short const& e);
	bool TE_Signal_Long_EnterHandler(TE_Signal_Long const& e);

	// broker response handler
	virtual void onCallbackReceived(
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
	);

private:
	void openPos();
};

//--- ENTRY ACTION ---------------
class A_Notify_User_Error: public CppUtils::BaseEnterAction<MachineContext>, public BrokerResponseRecipient {
	public:
		CTOR(A_Notify_User_Error);
	// --- enter map--
	BEGIN_EVENT_MAP()
		EVENT_MAP(TE_Broker_Error,TE_Broker_Error_EnterHandler)
		EVENT_MAP(TE_Broker_Error,TE_Broker_Error_EnterHandler)
		EVENT_MAP(TE_Stop_Trade,TE_Stop_Trade_EnterHandler)
		EVENT_MAP(TE_Stop_Trade,TE_Stop_Trade_EnterHandler)
	END_EVENT_MAP()

	// ------------- enter handlers ------------

	bool TE_Broker_Error_EnterHandler(TE_Broker_Error const& e);
	bool TE_Stop_Trade_EnterHandler(TE_Stop_Trade const& e);
};

//--- EXIT ACTION ---------------
class AE_TS_Open_Short_Exit: public CppUtils::BaseExitAction<MachineContext>, public BrokerResponseRecipient {
	public:
		CTOR(AE_TS_Open_Short_Exit);
	// --- exit map--
	BEGIN_EVENT_MAP()
		EVENT_MAP(TE_Signal_Long, TE_Signal_Long_ExitHandler)
		EVENT_MAP(TE_Stop_Trade, TE_Stop_Trade_ExitHandler)
	END_EVENT_MAP()

	// ------------- exit handlers ------------

	bool TE_Signal_Long_ExitHandler(TE_Signal_Long const& e);
	bool TE_Stop_Trade_ExitHandler(TE_Stop_Trade const& e);
};

//--- EXIT ACTION ---------------
class AE_TS_Open_Long_Exit: public CppUtils::BaseExitAction<MachineContext>, public BrokerResponseRecipient {
	public:
		CTOR(AE_TS_Open_Long_Exit);
	// --- exit map--
	BEGIN_EVENT_MAP()
		EVENT_MAP(TE_Signal_Short, TE_Signal_Short_ExitHandler)
		EVENT_MAP(TE_Stop_Trade, TE_Stop_Trade_ExitHandler)
	END_EVENT_MAP()

	// ------------- exit handlers ------------

	bool TE_Signal_Short_ExitHandler(TE_Signal_Short const& e);
	bool TE_Stop_Trade_ExitHandler(TE_Stop_Trade const& e);
};

//--- EXIT ACTION ---------------
class AE_TS_Open_Nothing_Exit: public CppUtils::BaseExitAction<MachineContext>, public BrokerResponseRecipient {
	public:
		CTOR(AE_TS_Open_Nothing_Exit);
	// --- exit map--
	BEGIN_EVENT_MAP()
		EVENT_MAP(TE_Signal_Long, TE_Signal_Long_ExitHandler)
		EVENT_MAP(TE_Signal_Short, TE_Signal_Short_ExitHandler)
		EVENT_MAP(TE_Stop_Trade, TE_Stop_Trade_ExitHandler)
	END_EVENT_MAP()

	// ------------- exit handlers ------------

	bool TE_Signal_Short_ExitHandler(TE_Signal_Short const& e);
	bool TE_Signal_Long_ExitHandler(TE_Signal_Long const& e);
	bool TE_Stop_Trade_ExitHandler(TE_Stop_Trade const& e);
};



//----------------------------------------------------------------

//--- TRANSITION ACTION ---------------
class AT_Store_Pending_Signal: public CppUtils::BaseTransitionAction<MachineContext>, public BrokerResponseRecipient {
	public:
		CTOR(AT_Store_Pending_Signal);
		virtual bool execute(CppUtils::BaseEvent const& eventToProcess);
};


//----------------------------------------------------------------

//--- ENTRY ACTION ---------------
class A_Check_Pending_Orders_TS_Open_Short: public CppUtils::BaseEnterAction<MachineContext>, public BrokerResponseRecipient {
	public:
		CTOR(A_Check_Pending_Orders_TS_Open_Short);
	// --- enter map--
	BEGIN_EVENT_MAP()
		EVENT_MAP(TE_Confirm_Open_Short,TE_Confirm_Open_Short_EnterHandler)
		EVENT_MAP(TE_Completed_Short,TE_Completed_Short_EnterHandler)
	END_EVENT_MAP()

	// ------------- enter handlers ------------

	bool TE_Completed_Short_EnterHandler(TE_Completed_Short const& e);
	bool TE_Confirm_Open_Short_EnterHandler(TE_Confirm_Open_Short const& e);
};


//----------------------------------------------------------------

//--- ENTRY ACTION ---------------
class A_Check_Pending_Orders_TS_Open_Long: public CppUtils::BaseEnterAction<MachineContext>,public BrokerResponseRecipient {
	public:
		CTOR(A_Check_Pending_Orders_TS_Open_Long);
	// --- enter map--
	BEGIN_EVENT_MAP()
		EVENT_MAP(TE_Confirm_Open_Long,TE_Confirm_Open_Long_EnterHandler)
		EVENT_MAP(TE_Completed_Long,TE_Completed_Long_EnterHandler)
	END_EVENT_MAP()

	// ------------- enter handlers ------------

	bool TE_Completed_Long_EnterHandler(TE_Completed_Long const& e);
	bool TE_Confirm_Open_Long_EnterHandler(TE_Confirm_Open_Long const& e);
};


//----------------------------------------------------------------

//--- ENTRY ACTION ---------------
class A_Check_Pending_Orders_TS_Open_Nothing: public CppUtils::BaseEnterAction<MachineContext>,public BrokerResponseRecipient {
	public:
		CTOR(A_Check_Pending_Orders_TS_Open_Nothing);
	// --- enter map--
	BEGIN_EVENT_MAP()
		EVENT_MAP(TE_Confirm_Open_Nothing,TE_Confirm_Open_Nothing_EnterHandler)
	END_EVENT_MAP()

	// ------------- enter handlers ------------

	bool TE_Confirm_Open_Nothing_EnterHandler(TE_Confirm_Open_Nothing const& e);
};

//----------------------------------------------------------------


};	// end of namespace


#endif