#ifndef _GERCHIK_RANGE_BREAK2_IMP_INCLUDED
#define _GERCHIK_RANGE_BREAK2_IMP_INCLUDED


#include "gerchik_range_break2defs.hpp"
#include "action.hpp"
#include "state.hpp"
#include "event.hpp"
#include "signaller.hpp"
#include "order_machine.hpp"
#include "utils.hpp"
#include "metacommandproc.hpp"


#define ALG_M_RANGE_BREAK2 "ALG_M_RANGE_BREAK2"
#define RESPONSE_TAG_REMAIN_SHARES tag_m

#define ALL_SYMBOL_ID "-ALL-"
// -------------------------



#define LOG_SYMBOL(RUN_NAME,PROVIDER, L,S,X) 	\
	DRAWOBJ_TEXT(RUN_NAME,CppUtils::getTime(),ALG_M_RANGE_BREAK2, S, PROVIDER, L, "LOG", X );	\


#define LOG_COMMON(RUN_NAME,PROVIDER, L,X)			\
	DRAWOBJ_TEXT(RUN_NAME,CppUtils::getTime(),ALG_M_RANGE_BREAK2, ALL_SYMBOL_ID, PROVIDER, L, "LOG", X );	\

/*
#define LOG_MACHINE_STATE(RUN_NAME,PROVIDER, MACHINE_UID, OLD_STATE, NEW_STATE)	\
	DRAWOBJ_TEXT(RUN_NAME,CppUtils::getTime(),ALG_M_RANGE_BREAK2, ALL_SYMBOL_ID, PROVIDER, BrkLib::AP_MEDIUM, "STATE", MACHINE_UID << "," << OLD_STATE.getState().getName() << "," << NEW_STATE.getState().getName());	\
	
#define LOG_MACHINE_ORDER_DESCRIPTOR(RUN_NAME,PROVIDER, MACHINE_UID, DESCRIPTOR)	\
	DRAWOBJ_TEXT(RUN_NAME,CppUtils::getTime(),ALG_M_RANGE_BREAK2, ALL_SYMBOL_ID, PROVIDER, BrkLib::AP_MEDIUM, "ODESC", MACHINE_UID << "," << DESCRIPTOR.toString());	\

#define LOG_METAMACHINE_STATE(RUN_NAME,PROVIDER, MACHINE_UID, OLD_STATE, NEW_STATE)	\
	DRAWOBJ_TEXT(RUN_NAME,CppUtils::getTime(),ALG_M_RANGE_BREAK2, ALL_SYMBOL_ID, PROVIDER, BrkLib::AP_MEDIUM, "METASTATE", MACHINE_UID << "," << OLD_STATE.getMetacommandState().getName() << "," << NEW_STATE.getMetacommandState().getName());	\
*/


#define LLOG(RUNNAME, PROVIDER, METAUID, ORDUID, CONTENT)	\
	ostrstream sstr; sstr << CONTENT << ends; \
	MRangeBreak::log(PROVIDER,RUNNAME,METAUID,ORDUID, sstr.str());	\
	sstr.freeze(false); 

#define LLOG_META(RUNNAME, PROVIDER, METAUID, CONTENT)	\
	ostrstream sstr; sstr << CONTENT << ends; \
	MRangeBreak::log(PROVIDER,RUNNAME,METAUID, sstr.str());	\
	sstr.freeze(false); 


namespace AlgLib {

	
	// ------------------

	
	class GERCHIK_RANGE_BREAK2_EXP MRangeBreak: public AlgorithmBase, public OrderStateMachineListener {

	public:

		struct EventProcessingStruct 
		{
			EventProcessingStruct(BrkLib::BrokerConnect const& brkConnect, TradingEvent const& event):
				brokerConnect_m(brkConnect),
				event_m(event)
			{
			}

			EventProcessingStruct()
			{
			}

			BrkLib::BrokerConnect brokerConnect_m;

			TradingEvent event_m;
		};



		// ctor & dtor
		MRangeBreak( Inqueue::AlgorithmHolder& algHolder);

		virtual ~MRangeBreak();
		
		// intreface to be implemented
		virtual void onLibraryInitialized(
			HlpStruct::AlgBrokerDescriptor const& descriptor, 
			BrkLib::BrokerBase* brokerlib, 
			BrkLib::BrokerBase* brokerlib2,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		);

		virtual void onLibraryFreed();

		virtual void onDataArrived(
			HlpStruct::RtData const& rtdata, 
			Inqueue::ContextAccessor& historyaccessor, 
			HlpStruct::TradingParameters const& descriptor,
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		);

		virtual void onRtProviderSynchEvent (
			Inqueue::ContextAccessor& contextaccessor,
			HlpStruct::RtData::SynchType const synchFlg,
			double const& synchTime,
			CppUtils::String const& providerName,
			CppUtils::String const& message
		);

		virtual void onLevel2DataArrived(
			HlpStruct::RtLevel2Data const& level2data,
			Inqueue::ContextAccessor& historyaccessor, 
			HlpStruct::TradingParameters const& descriptor,
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		);

		virtual void onLevel1DataArrived(
			HlpStruct::RtLevel1Data const& level1data,
			Inqueue::ContextAccessor& historyaccessor, 
			HlpStruct::TradingParameters const& descriptor,
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		);

		virtual void onBrokerResponseArrived(
			BrkLib::BrokerConnect &brkConnect,
			BrkLib::BrokerResponseBase const& bresponse
		);

		virtual void onEventArrived(
				Inqueue::ContextAccessor& contextaccessor, 
				HlpStruct::CallingContext& callContext,
				bool& result,
				CppUtils::String &customOutputData
		);
	
		virtual void onThreadStarted(
			Inqueue::ContextAccessor& contextaccessor, bool const firstLib, bool const lastLib
		);

		virtual void onThreadFinished(
			Inqueue::ContextAccessor& contextaccessor, HlpStruct::TradingParameters const& tradeParams, bool const firstLib, bool const lastLib
		)
		{
		}


		virtual CppUtils::String const& getName() const;
		
		// -----------------------

	public:
		// this function implement event processing in a separate thread to avoid recursion
		void threadProcessingThread();

		void threadServingThread();

		void acceptNewlyArrivedEvent(TradingEvent const& te, bool const machine_must_exist, CppUtils::Uid const& parentUid);

		void ordermachine_Force_TE_InstallOrder(CppUtils::String const& orderSignallerString,CppUtils::Uid const& parentUid);

		void ordermachine_Force_TE_UserConfirmFix(CppUtils::String const& orderSignallerString,CppUtils::Uid const& parentUid);

		void ordermachine_Force_TE_CancelOrder(CppUtils::String const& orderSignallerString,CppUtils::Uid const& parentUid);

		CppUtils::String const& getInternalRunName() const
		{
			return internalRun_name_m;
		};

		CppUtils::String const& getProviderRTSymbols() const
		{
			return providerForRTSymbols_m;
		};

		// this is logging - including order id
		static void log(
			CppUtils::String const& provider, 
			CppUtils::String const& runname, 
			CppUtils::Uid const& machineUid, 
			CppUtils::Uid const& orderUid, 
			char const* content
		);

		// 
		static void log(
			CppUtils::String const& provider, 
			CppUtils::String const& runname, 
			CppUtils::Uid const& machineUid, 
			char const* content
		);

	private:

		virtual void requestIssueOrderInfo(CppUtils::Uid const& orderUid, Action const& oldaction, Action const& newaction, OrderDescriptor& orderDescriptor);
		virtual void requestCancelOrderInfo(CppUtils::Uid const& orderUid, Action const& oldaction, Action const& newaction, OrderDescriptor& orderDescriptor);
		virtual void requestRemoveDelayedOrderInfo(CppUtils::Uid const& orderUid, Action const& oldaction, Action const& newaction, OrderDescriptor& orderDescriptor);
		virtual void requestClearInfo(CppUtils::Uid const& orderUid, Action const& oldaction, Action const& newaction, OrderDescriptor& orderDescriptor);

					// this executes orders
		virtual void executeIssueOrder(CppUtils::Uid const& orderUid, Action const& action, OrderDescriptor const& orderDescriptor);
		virtual void executeCancelOrder(CppUtils::Uid const& orderUid, Action const& action, OrderDescriptor const& orderDescriptor);
		virtual void executeRemoveDelayedOrder(CppUtils::Uid const& orderUid, Action const& action, OrderDescriptor const& orderDescriptor);
		virtual void executeClear(CppUtils::Uid const& orderUid, Action const& action, OrderDescriptor const& orderDescriptor);

		virtual void onStateChangedNotification(CppUtils::Uid const& parentUid, CppUtils::Uid const& orderUid, TradingState const& oldState, TradingState const& newState);
		virtual void onOrderDescriptorChanged(CppUtils::Uid const& parentUid, CppUtils::Uid const& orderUid, OrderDescriptor const& orderDescriptor);

		virtual void machineConfirmOrderIssued(CppUtils::Uid const& parentUid, CppUtils::Uid const& orderUid, OrderDescriptor const& orderDescriptor);
		virtual void machineConfirmOrderPending(CppUtils::Uid const& parentUid, CppUtils::Uid const& orderUid, OrderDescriptor const& orderDescriptor);
		virtual void machineConfirmOrderDelayed(CppUtils::Uid const& parentUid, CppUtils::Uid const& orderUid, OrderDescriptor const& orderDescriptor);
		virtual void machineConfirmOrderOpened(CppUtils::Uid const& parentUid, CppUtils::Uid const& orderUid, OrderDescriptor const& orderDescriptor);
		virtual void machineConfirmOrderCancelled(CppUtils::Uid const& parentUid, CppUtils::Uid const& orderUid, OrderDescriptor const& orderDescriptor);
		virtual void machineConfirmErrorReturned(CppUtils::Uid const& parentUid, CppUtils::Uid const& orderUid, OrderDescriptor const& orderDescriptor);


		// generate new states if any
		void machineEventsReaction(BrkLib::BrokerConnect& brconnect, TradingEvent const& event);
	
		// this is global function to do actions
		void doAction(BrkLib::BrokerConnect& brconnect, Action const& action);


		void pushProcessingEvent(BrkLib::BrokerConnect const& brkConnect, TradingEvent const& event);
		
		// if arrived time is within working time
		bool isWithinWorkingTime(double const &ttime) const;
		
		Action resolveMachineAction(CppUtils::Uid const& uid);

		

	private:

	
		MetacommandProcessor* metacommandProcessor_m;


		// ----------------------
		// thiis is normal event processing therads
		CppUtils::Thread* thread_m;

		// event whether thread started
		CppUtils::Event threadStarted_m;

		
		// signal to shutdown
		CppUtils::Event shutDownThread_m;

		// the queue to extract processing data
		CppUtils::FifoQueue<EventProcessingStruct, 200> receivedProcessingData_m;

		// event signalling this arrived
		CppUtils::Event receivedProcessingDataArrived_m;

		// mtx securing that
		CppUtils::Mutex receivedProcessingDataMtx_m;
		
		
		// list of symbols for RT - from laser
		CppUtils::StringSet symbolLevel1_rt_m;

					
		// this is provider containing RT stocks - laser provider
		CppUtils::String providerForRTSymbols_m;

		// run name
		CppUtils::String internalRun_name_m;
	
		// mutex securing work on processing event. state change, etc...
		
		// this is state machines for each of symbol
		// this is mapped via Order UIDS - this is the only reaonable map
		map<CppUtils::Uid, OrderStateMachine> orderStateMachines_m;

		CppUtils::Mutex orderStateMachinesMtx_m;

		BrkLib::BrokerConnect* brokerConnect_m;

			
		double unixTimeBeginTrade_m;

		double unixTimeEndTrade_m;

		int hoursBegin_m;
		
		int minutesBegin_m;

		int hourEnd_m;
		
		int minutesEnd_m;
				
	};


}; // end of namespace

#endif