#ifndef _ALGLIBMT2_ALGORITHM_INCLUDED
#define _ALGLIBMT2_ALGORITHM_INCLUDED

#include "../brklib/brklib.hpp"
#include "alglibmetatrader2defs.hpp"
#include "commonstruct.hpp"
//#include "persistent.hpp"

#include "exchange_packets_base.hpp"
#include "exchange_packets_mt.hpp"
#include "exchange_packet_alglib_mt_response.hpp"
#include "exchange_packet_alglib_mt_request.hpp"
#include "exchange_packet_alglib_out.hpp"
#include "shared.hpp"
#include "settingsmanager.hpp"
#include "mtworkermanager.hpp"
#include "ConsoleLogger.h"



#define RETURN_MT_DATA_ACCEPTED "OK"
#define MT4_DELIVERY_ENVELOPE_EVENT  "mt4_delivery_envelope"
#define MT4_HEARTBEAT_EVENT  "mt4_heartbeat"
#define MT4_CONNECT_STATUS_EVENT  "mt4_connect_status"




//#define MAX_ATTEMPTS_TO_KILL_MT 60



namespace Inqueue {
	class AlgorithmHolder;
	struct AlgBrokerDescriptor;
}


#define DEBUG_LOG(BASE, MSG) {			\
	AlgorithmMt const& pThis = BASE;	\
	if (pThis.getSettingsManager().getSettings().allowdebugEvents_m) {  \
		std::ostringstream ress;    \
		ress <<  CppUtils::getAscTime() << " [ " << CppUtils::getTID() << " ] "<< MSG << endl;	\
		pThis.propagateDebugOutput(ress.str().c_str());	\
	}	\
};

/**
 This library will implement the following sustem - 
 Open position at some moment of the day, then will close this the next day
*/
namespace AlgLib {
	
	class AlgorithmMt;

	// functor removing old entries from all queues
	class QueuePurgeFunctor: public CppUtils::ServiceThread::ServiceFunction, public CppUtils::RefCounted
	{
	public:
		QueuePurgeFunctor(AlgorithmMt& base, int const checkInterval);
		virtual double operator() () const;

		inline int const getCheckInterval() const
		{
			return checkInterval_m;
		};

	private:
		AlgorithmMt& base_m;
		int checkInterval_m;
		
	};

	// ----------------------

	class ZombieKillerFunctor: public CppUtils::ServiceThread::ServiceFunction, public CppUtils::RefCounted
	{
		public:
			ZombieKillerFunctor(AlgorithmMt& base, int const checkInterval);
			virtual double operator() () const;

			inline int const getCheckInterval() const
			{
				return checkInterval_m;
			};

		private:
			AlgorithmMt& base_m;
			int checkInterval_m;
	};

	/*
	class ReadMTContentFunctor: public CppUtils::ServiceThread::ServiceFunction, public CppUtils::RefCounted
	{
		public:
			ReadMTContentFunctor(AlgorithmMt& base, int const checkInterval);
			virtual double operator() () const;

			inline int const getCheckInterval() const
			{
				return checkInterval_m;
			};

		private:
			AlgorithmMt& base_m;
			int checkInterval_m;
	};
	*/

	// this is sending heartbeats to HT Java layer functor
	class HeartBeatFunctor: public CppUtils::ServiceThread::ServiceFunction, public CppUtils::RefCounted
	{
	public:
		HeartBeatFunctor(AlgorithmMt& base);
		virtual double operator() () const;

	private:
		int heartBeatInterval_m;
		AlgorithmMt& base_m;
	};

	/*
	// this functor is responsible for removing stale MT4 connections
	class MTConnectorCheckerFunctor: public CppUtils::ServiceThread::ServiceFunction, public CppUtils::RefCounted
	{
	public:
		MTConnectorCheckerFunctor(AlgorithmMt& base);
		virtual double operator() () const;

	private:
		AlgorithmMt& base_m;
	};
	*/

	/**
		This is the base class for all the implemented algorithms 
	*/
	
	class ALGLIBMETATRADER2_EXP AlgorithmMt: public AlgorithmBase
	{
		
		friend class HeartBeatFunctor;
		//friend class MTConnectorCheckerFunctor;
		friend class QueuePurgeFunctor;
		friend class MtRunManager;

	public:

		enum EventPacketType {
			EPT_HEARTBEAT = 1,
			EPT_POSITION_LIST = 2
		};
		
		// ctor & dtor
		AlgorithmMt( Inqueue::AlgorithmHolder& algHolder);

		virtual ~AlgorithmMt();
		
		// interfaces to be implemented
		virtual void onLibraryInitialized(HlpStruct::AlgBrokerDescriptor const& descriptor, BrkLib::BrokerBase* brokerlib, BrkLib::BrokerBase* brokerlib2, CppUtils::String const& runName, CppUtils::String const& comment);

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

		void onEventArrived(
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
			);

		virtual CppUtils::String const& getName() const;

		void sendMTConnectStatus(bool const connected, CppUtils::String const& mtname);

		void sendRawDataFromMT(MtProxy_AlgLib_Dialog::Mtproxy_Data const& mtproxydata_act) const;

		void sendHeartBeat() const;

		void processMTOutput() const;

		// kill MTs which were spawned not by us
		void killZombieMts() const;


		void propagateDebugOutput(char const* str) const;

		inline SettingsManager const& getSettingsManager() const {
			return settingsManager_m;
		}

		void getPositionListForAccount(CppUtils::String const& account, BrkLib::PositionList &positionList);
		
		void mtReaderThreadRun();

	private:



		// very important fun
		// if START_POLLING_USERS is provided without MT instance we try to find the most available instance
		void resolveAvailableMTInstance(CppUtils::String& instanceName, CppUtils::String const& companyId);



		CppUtils::RefCountPtr<AlgLib::AlgLib_Event_Dialog_Response::Resp_START_ORDER_LIST>
			n_processArrivedEvent_START_ORDER_LIST(AlgLib_Event_Dialog_Request::AlgLibRequestBase const& req);

		CppUtils::RefCountPtr< AlgLib_Event_Dialog_Response::Resp_G_GET_ALL_METADATA> 
			n_processArrivedEvent_G_GET_ALL_METADATA(AlgLib_Event_Dialog_Request::AlgLibRequestBase const& req);

		CppUtils::RefCountPtr<AlgLib_Event_Dialog_Response::Resp_G_QUERY_AUTOLOAD_USERS>
			n_processArrivedEvent_G_QUERY_AUTOLOAD_USERS(AlgLib_Event_Dialog_Request::AlgLibRequestBase const& req);

		CppUtils::RefCountPtr< AlgLib_Event_Dialog_Response::Resp_START_POLLING_USERS> 
			n_processArrivedEvent_START_POLLING_USERS(AlgLib_Event_Dialog_Request::AlgLibRequestBase const& req);
		
		CppUtils::RefCountPtr< AlgLib_Event_Dialog_Response::Resp_REQUEST_POLLING_USER_LIST>
			n_processArrivedEvent_REQUEST_POLLING_USER_LIST(AlgLib_Event_Dialog_Request::AlgLibRequestBase const& req);

		CppUtils::RefCountPtr< AlgLib_Event_Dialog_Response::Resp_CLEAR_ALL_USERS>
			n_processArrivedEvent_CLEAR_ALL_USERS(AlgLib_Event_Dialog_Request::AlgLibRequestBase const& req);

		CppUtils::RefCountPtr< AlgLib_Event_Dialog_Response::Resp_REQUEST_MT_STATUS>
			n_processArrivedEvent_REQUEST_MT_STATUS(AlgLib_Event_Dialog_Request::AlgLibRequestBase const& req);

		CppUtils::RefCountPtr< AlgLib_Event_Dialog_Response::Resp_UPDATE_USER_ACCESS_TIME>
			n_processArrivedEvent_UPDATE_USER_ACCESS_TIME(AlgLib_Event_Dialog_Request::AlgLibRequestBase const& req);

				
	private:


		BrkLib::BrokerConnect* brokerConnect_m;
		
		CppUtils::RefCountPtr<HeartBeatFunctor> functor_m;

		//CppUtils::RefCountPtr<MTConnectorCheckerFunctor> connCheckerFunctor_m;

		CppUtils::RefCountPtr<QueuePurgeFunctor> queuesPurgeFunctor_m;

		//CppUtils::RefCountPtr<ReadMTContentFunctor> readMtDataFunctor_m;

		CppUtils::RefCountPtr<ZombieKillerFunctor> zombieMt4KillerFunctor_m;

		//CppUtils::String settingsFile_m;

		SettingsManager settingsManager_m;

		MTWorkerThreadManager workerThreadManager_m;

		CConsoleLoggerEx* helperLoggerP_m;

		// event whether therad started
		CppUtils::Event threadStarted_m;

		CppUtils::Thread* thread_m;

		CppUtils::Event shutDownThread_m;

		



	};
};

#endif
