#ifndef _GERCHIK_RANGE_BREAK2_METACOMMANDPROC_INCLUDED
#define _GERCHIK_RANGE_BREAK2_METACOMMANDPROC_INCLUDED

#include "gerchik_range_break2defs.hpp"
#include "orderdescr.hpp"
#include "metacommand_machine.hpp"

namespace AlgLib {


class MRangeBreak;


	
// this class is developed to process metacommands related to different combination of orders
class MetacommandProcessor: public MetacommandStateMachineListener
{

	public:

	
		struct MetacommandProcessorStructure {
			MetacommandProcessorStructure():
				type_m(CT_BREAKTHROUGH_DUMMY)
			{
			}

			MetacommandProcessorStructure(CppUtils::EnumerationHelper const& type, HlpStruct::XmlParameter const& param):
				type_m(type),
				param_m(param)
			{
			}

			
			CppUtils::EnumerationHelper type_m;
			HlpStruct::XmlParameter param_m;
		};

		MetacommandProcessor(MRangeBreak& baserangeBreaker);
		
		~MetacommandProcessor();


		// thread function
		void threadServingThread();

		// initiate berakthrough
		void runBreakThroughUp(HlpStruct::XmlParameter const& param_data);
		void runBreakThroughDown(HlpStruct::XmlParameter const& param_data);
		
		// notifications
		void onMachineConfirmOrderIssued(CppUtils::Uid const& machineUid, CppUtils::Uid const& orderUid, OrderDescriptor const& orderDescriptor);
		void onMachineConfirmOrderPending(CppUtils::Uid const& machineUid, CppUtils::Uid const& orderUid, OrderDescriptor const& orderDescriptor);
		void onMachineConfirmOrderDelayed(CppUtils::Uid const& machineUid, CppUtils::Uid const& orderUid, OrderDescriptor const& orderDescriptor);
		void onMachineConfirmOrderOpened(CppUtils::Uid const& machineUid, CppUtils::Uid const& orderUid, OrderDescriptor const& orderDescriptor);
		void onMachineConfirmOrderCancelled(CppUtils::Uid const& machineUid, CppUtils::Uid const& orderUid, OrderDescriptor const& orderDescriptor);
		void onMachineConfirmErrorReturned(CppUtils::Uid const& machineUid, CppUtils::Uid const& orderUid, OrderDescriptor const& orderDescriptor);

		// notifications from metacommand machine
		virtual void onStateChangedNotification(CppUtils::Uid const& machineUid, MetacommandState const& oldState, MetacommandState const& newState);

		// sends command to order machine
		virtual void ordermachine_Force_TE_InstallOrder(CppUtils::String const& orderSignallerString, CppUtils::Uid const& machineUid);
		

	private:

		void issueMetaCommand(HlpStruct::XmlParameter const& param_data, CppUtils::EnumerationHelper const& commandType );

		// this creates new machine and initialize this
		void onNewMetacommandArrived(
			CppUtils::Uid const& machineUid, 
			CppUtils::String const& description,
			CppUtils::EnumerationHelper const& commandType,
			CppUtils::String const& orderSignStr,
			CppUtils::String const& orderTPSignStr,
			CppUtils::String const& orderSLSignStr
		);

		void machineEventReaction(MetacommandEvent const& event_i);

		MetacommandAction resolveMachineAction(CppUtils::Uid const& uid);

		MetacommandState resolveMachineState(CppUtils::Uid const& uid);

		// this is metathread - to process order chunks
		// this works with chunks of threads
		CppUtils::Thread* servingRobotThread_m;

		CppUtils::Event servingThreadStarted_m;

		CppUtils::Event shutDownServingThread_m;
		
		CppUtils::Event servingThreadStartActivity_m;

		MRangeBreak& baserangeBreaker_m; 

		// the queue to extract processing data
		CppUtils::FifoQueue<MetacommandProcessorStructure, 200> receivedProcessingData_m;

		// event signalling this arrived
		CppUtils::Event receivedProcessingDataArrived_m;

		// mtx securing that
		CppUtils::Mutex receivedProcessingDataMtx_m;

		// the map of metacommand machines
		map<CppUtils::Uid, MetacommandStateMachine> metacommandMachines_m;

		// mutex securing metacommand machines
		CppUtils::Mutex metacommandMachinesMtx_m;

		
};

}; // end of namespace
#endif
