#ifndef _SMARTCOM_ALG_MISHA2_ACTION_INCLUDED
#define _SMARTCOM_ALG_MISHA2_ACTION_INCLUDED

#include "smartcom_alg_misha2defs.hpp"
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


//--- ENTRY ACTION ---------------
class A_Cancel_Limit_TP: public CppUtils::BaseEnterAction<MachineContext>, public BrokerResponseRecipient {
	public:
		CTOR(A_Cancel_Limit_TP);
	// --- enter map--
	BEGIN_EVENT_MAP()
		EVENT_MAP(TE_Stop_Executed,TE_Stop_Executed_EnterHandler)
		EVENT_MAP(TE_Stop_Cancelled_Market_Exit,TE_Stop_Cancelled_Market_Exit_EnterHandler)
	END_EVENT_MAP()

	// ------------- enter handlers ------------

	bool TE_Stop_Executed_EnterHandler(TE_Stop_Executed const& e);
	bool TE_Stop_Cancelled_Market_Exit_EnterHandler(TE_Stop_Cancelled_Market_Exit const& e);

	// broker response handler
	virtual void onCallbackReceived(
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
	);

	
};

//--- EXIT ACTION ---------------
class AE_Broker_Error: public CppUtils::BaseExitAction<MachineContext>, public BrokerResponseRecipient {
	public:
		CTOR(AE_Broker_Error);
	// --- exit map--
	BEGIN_EVENT_MAP()
	END_EVENT_MAP()

	// ------------- exit handlers ------------

	

};

//--- ENTRY ACTION ---------------
class A_Issue_Limit_TP: public CppUtils::BaseEnterAction<MachineContext>, public BrokerResponseRecipient {
	public:
		CTOR(A_Issue_Limit_TP);
	// --- enter map--
	BEGIN_EVENT_MAP()
		EVENT_MAP(TE_Limit_Executed,TE_Limit_Executed_EnterHandler)
	END_EVENT_MAP()

	// ------------- enter handlers ------------

	bool TE_Limit_Executed_EnterHandler(TE_Limit_Executed const& e);

	// broker response handler
	virtual void onCallbackReceived(
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
	);
};

//--- ENTRY ACTION ---------------
class A_Initial_Enter: public CppUtils::BaseEnterAction<MachineContext>, public BrokerResponseRecipient {
	public:
		CTOR(A_Initial_Enter);
	// --- enter map--
	BEGIN_EVENT_MAP()
		EVENT_MAP(TE_Limit_Cancelled,TE_Limit_Cancelled_EnterHandler)
		EVENT_MAP(TE_Limit_TP_Cancelled,TE_Limit_TP_Cancelled_EnterHandler)
		EVENT_MAP(TE_Stop_Cancelled_TP_Exit,TE_Stop_Cancelled_TP_Exit_EnterHandler)
	END_EVENT_MAP()

	// ------------- enter handlers ------------

	bool TE_Stop_Cancelled_TP_Exit_EnterHandler(TE_Stop_Cancelled_TP_Exit const& e);
	bool TE_Limit_Cancelled_EnterHandler(TE_Limit_Cancelled const& e);
	bool TE_Limit_TP_Cancelled_EnterHandler(TE_Limit_TP_Cancelled const& e);

	// default
	virtual bool executeDefaultHandler(CppUtils::BaseEvent const& eventToProcess);
};


//--- ENTRY ACTION ---------------
class A_Cancel_Limit: public CppUtils::BaseEnterAction<MachineContext>, public BrokerResponseRecipient {
	public:
		CTOR(A_Cancel_Limit);
	// --- enter map--
	BEGIN_EVENT_MAP()
		EVENT_MAP(TE_Level_Lost,TE_Level_Lost_EnterHandler)
		EVENT_MAP(TE_Stop_Cancelled,TE_Stop_Cancelled_EnterHandler)
	END_EVENT_MAP()

	// ------------- enter handlers ------------

	bool TE_Stop_Cancelled_EnterHandler(TE_Stop_Cancelled const& e);
	bool TE_Level_Lost_EnterHandler(TE_Level_Lost const& e);

	// broker response handler
	virtual void onCallbackReceived(
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
	);
};

//--- ENTRY ACTION ---------------
class A_Mismatched_Execution_Enter: public CppUtils::BaseEnterAction<MachineContext>, public BrokerResponseRecipient {
	public:
		CTOR(A_Mismatched_Execution_Enter);
	// --- enter map--
	BEGIN_EVENT_MAP()
		EVENT_MAP(TE_Stop_Executed,TE_Stop_Executed_EnterHandler)
		EVENT_MAP(TE_Limit_TP_Executed,TE_Limit_TP_Executed_EnterHandler)
	END_EVENT_MAP()

	// ------------- enter handlers ------------

	bool TE_Stop_Executed_EnterHandler(TE_Stop_Executed const& e);
	bool TE_Limit_TP_Executed_EnterHandler(TE_Limit_TP_Executed const& e);
};

//--- EXIT ACTION ---------------
class AE_Mismatched_Execution: public CppUtils::BaseExitAction<MachineContext>, public BrokerResponseRecipient {
	public:
		CTOR(AE_Mismatched_Execution);
	// --- exit map--
	BEGIN_EVENT_MAP()
		EVENT_MAP(TE_Unconditional, TE_Unconditional_ExitHandler)
	END_EVENT_MAP()

	// ------------- exit handlers ------------

	bool TE_Unconditional_ExitHandler(TE_Unconditional const& e);
	
};

//--- ENTRY ACTION ---------------
class A_Issue_Stop: public CppUtils::BaseEnterAction<MachineContext>, public BrokerResponseRecipient {
	public:
		CTOR(A_Issue_Stop);
	// --- enter map--
	BEGIN_EVENT_MAP()
		EVENT_MAP(TE_Limit_Pending,TE_Limit_Pending_EnterHandler)
	END_EVENT_MAP()

	// ------------- enter handlers ------------

	bool TE_Limit_Pending_EnterHandler(TE_Limit_Pending const& e);

	// broker response handler
	virtual void onCallbackReceived(
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
	);
};

//--- ENTRY ACTION ---------------
class A_Cancel_Stop: public CppUtils::BaseEnterAction<MachineContext>, public BrokerResponseRecipient {
	public:
		CTOR(A_Cancel_Stop);
	// --- enter map--
	BEGIN_EVENT_MAP()
		EVENT_MAP(TE_Level_Lost,TE_Level_Lost_EnterHandler)
	END_EVENT_MAP()

	// ------------- enter handlers ------------

	bool TE_Level_Lost_EnterHandler(TE_Level_Lost const& e);

	// broker response handler
	virtual void onCallbackReceived(
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
	);
};

//--- ENTRY ACTION ---------------
class A_Issue_Limit: public CppUtils::BaseEnterAction<MachineContext>, public BrokerResponseRecipient {
	public:
		CTOR(A_Issue_Limit);
	// --- enter map--
	BEGIN_EVENT_MAP()
		EVENT_MAP(TE_Level_Detected,TE_Level_Detected_EnterHandler)
	END_EVENT_MAP()

	// ------------- enter handlers ------------

	bool TE_Level_Detected_EnterHandler(TE_Level_Detected const& e);

	// broker response handler
	virtual void onCallbackReceived(
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
	);
};

//--- ENTRY ACTION ---------------
class A_Issue_Market_Close: public CppUtils::BaseEnterAction<MachineContext>, public BrokerResponseRecipient {
	public:
		CTOR(A_Issue_Market_Close);
	// --- enter map--
	BEGIN_EVENT_MAP()
		EVENT_MAP(TE_Level_Lost,TE_Level_Lost_EnterHandler)
	END_EVENT_MAP()

	// ------------- enter handlers ------------

	bool TE_Level_Lost_EnterHandler(TE_Level_Lost const& e);

	// broker response handler
	virtual void onCallbackReceived(
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
	);
};

//--- EXIT ACTION ---------------
class AE_Error: public CppUtils::BaseExitAction<MachineContext>, public BrokerResponseRecipient {
	public:
		CTOR(AE_Error);
	// --- exit map--
	BEGIN_EVENT_MAP()
	END_EVENT_MAP()

	// ------------- exit handlers ------------



};

//--- ENTRY ACTION ---------------
class A_Cancel_Stop_Market_Exit: public CppUtils::BaseEnterAction<MachineContext>, public BrokerResponseRecipient {
	public:
		CTOR(A_Cancel_Stop_Market_Exit);
	// --- enter map--
	BEGIN_EVENT_MAP()
		EVENT_MAP(TE_Market_Close_Executed,TE_Market_Close_Executed_EnterHandler)
	END_EVENT_MAP()

	// ------------- enter handlers ------------

	bool TE_Market_Close_Executed_EnterHandler(TE_Market_Close_Executed const& e);

	// broker response handler
	virtual void onCallbackReceived(
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
	);

};

//--- ENTRY ACTION ---------------
class A_Cancel_Stop_TP_Exit: public CppUtils::BaseEnterAction<MachineContext>, public BrokerResponseRecipient {
	public:
		CTOR(A_Cancel_Stop_TP_Exit);
	// --- enter map--
	BEGIN_EVENT_MAP()
		EVENT_MAP(TE_Limit_TP_Executed,TE_Limit_TP_Executed_EnterHandler)
	END_EVENT_MAP()

	// ------------- enter handlers ------------

	bool TE_Limit_TP_Executed_EnterHandler(TE_Limit_TP_Executed const& e);

	// broker response handler
	virtual void onCallbackReceived(
		BrkLib::BrokerConnect &brkConnect,
		BrkLib::BrokerResponseBase const& bresponse
	);
};



};	// end of namespace


#endif