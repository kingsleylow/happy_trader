#include "broker.hpp"
#include "../inqueue/inqueue.hpp"
//#include "../inqueue/commontypes.hpp"
#include "../alglib/alglib.hpp"




#define BRKLIB "BRKLIB"
namespace BrkLib {

	BrokerBase::BrokerBase(Inqueue::AlgorithmHolder& algHolder ):
		algHolder_m(algHolder),
		brokerMode_m(BM_NOTHING),
		outproc_m(*CppUtils::getRootObject<Inqueue::OutProcessor>())
	{
		
	}
		
	BrokerBase::~BrokerBase()
	{
	}


	
	void BrokerBase::onBrokerResponseArrived(
			BrkLib::BrokerSession &session,
			BrkLib::BrokerResponseBase const& bresponse
		)
	{												   
		// make a call to algorithm implementation of this function
		AlgLib::AlgorithmBase* algP = getAlgHolder().getAlgorithm();
		
		if (algP) {
			// logging  
			LOGEVENTA( HlpStruct::CommonLog::LL_DEBUGDETAIL, BRKLIB, "Broker response arrived", session.getBroker().getName() << bresponse.toString());
		
			// and send the respective event
			ISEVENTSUBSCRIBED_BEGIN(HlpStruct::EventData::ET_BrokerResponseEventResp)
				HlpStruct::EventData response;
				HlpStruct::XmlParameter xmlparam;

				response.getEventType() = HlpStruct::EventData::ET_BrokerResponseEventResp;
				response.setType(HlpStruct::EventData::DT_CommonXml);
			
				bresponse.toXmlParameter(xmlparam);
				response.getAsXmlParameter() = xmlparam;
				// set up the name
				response.setSource(session.getUid().toString());
			
			ISEVENTSUBSCRIBED_END(response);


			// resolve BrokerConnect object here
		
			BrokerConnect brkConnect(session, false, false);

			// some check fro consistency
			if (bresponse.resultCode_m != BrkLib::PT_DUMMY) {
				if (bresponse.resultDetailCode_m == BrkLib::ODR_DUMMY) {
					// cannot be like this
					THROW(CppUtils::OperationFailed, "exc_InvalidBrokerResponse", "ctx_onBrokerResponseArrived", bresponse.ID_m.toString());

				}
			}

			algP->onBrokerResponseArrived(brkConnect, bresponse);
			
		}

		
	}

	void BrokerBase::sendSessionCreateEvent(BrokerSession const& session, CppUtils::String const& runName, CppUtils::String const& comment)
	{
		ISEVENTSUBSCRIBED_BEGIN(HlpStruct::EventData::ET_BrokerSessionEvent)
				HlpStruct::EventData response;

				response.getEventType() = HlpStruct::EventData::ET_BrokerSessionEvent;
				response.setType(HlpStruct::EventData::DT_CommonXml);
			

				// time when session established
				response.getAsXmlParameter().getParameter("time").getAsDouble(true) = CppUtils::getTime();
				// whether this is simulation library
				response.getAsXmlParameter().getParameter("is_simul").getAsInt(true) = isBacktestLibrary();

				// this is new entry - must insert 
				response.getAsXmlParameter().getParameter("operation").getAsInt(true) = BrokerSession::SET_Create;
				response.getAsXmlParameter().getParameter("connect_string").getAsString(true) = session.getConnection();
				response.getAsXmlParameter().getParameter("thread").getAsInt(true) = CppUtils::getThreadHandle();
		
				response.getAsXmlParameter().getParameter("session_name").getAsString(true) = runName;
				response.getAsXmlParameter().getParameter("comment").getAsString(true) = comment;

				response.getAsXmlParameter().getParameter("alg_brk_pair").getAsString(true) = 
					getAlgHolder().getAlgBrokerDescriptor().name_m;
							
				response.getAsXmlParameter().getParameter("is_simulation").getAsInt(true) = isBacktestLibrary();

				// session inique if
				response.setSource(session.getUid().toString());
				
			ISEVENTSUBSCRIBED_END(response);
	}

	void BrokerBase::sendSessionConnectEvent(BrokerSession const& session, CppUtils::String const& runName)
	{
		ISEVENTSUBSCRIBED_BEGIN(HlpStruct::EventData::ET_BrokerSessionEvent)
			HlpStruct::EventData response;


			response.getEventType() = HlpStruct::EventData::ET_BrokerSessionEvent;
			response.setType(HlpStruct::EventData::DT_CommonXml);
		
			// time when session connected
			response.getAsXmlParameter(true).getParameter("time").getAsDouble(true) = CppUtils::getTime();
			// this is old entry - must update 
			response.getAsXmlParameter().getParameter("operation").getAsInt(true) = BrokerSession::SET_Connect;
			response.getAsXmlParameter().getParameter("thread").getAsInt(true) = CppUtils::getThreadHandle();
			response.getAsXmlParameter().getParameter("session_name").getAsString(true) = runName;
			
			//

			response.getAsXmlParameter().getParameter("alg_brk_pair").getAsString(true) = 
					getAlgHolder().getAlgBrokerDescriptor().name_m;
		
			//response.getAsXmlParameter().getParameter("is_simulation").getAsInt(true) = isBacktestLibrary();

			// session inique if
			response.setSource(session.getUid().toString());
			
		ISEVENTSUBSCRIBED_END(response);
	}

	void BrokerBase::sendSessionDisconnectEvent(BrokerSession const& session, CppUtils::String const& runName)
	{
		ISEVENTSUBSCRIBED_BEGIN(HlpStruct::EventData::ET_BrokerSessionEvent)
			HlpStruct::EventData response;


			response.getEventType() = HlpStruct::EventData::ET_BrokerSessionEvent;
			response.setType(HlpStruct::EventData::DT_CommonXml);
		
			// time when session connected
			response.getAsXmlParameter(true).getParameter("time").getAsDouble(true) = CppUtils::getTime();
			// this is old entry - must update 
			response.getAsXmlParameter().getParameter("operation").getAsInt(true) = BrokerSession::SET_Disconnect;
			response.getAsXmlParameter().getParameter("thread").getAsInt(true) = CppUtils::getThreadHandle();
			response.getAsXmlParameter().getParameter("session_name").getAsString(true) = runName;
			
			//

			response.getAsXmlParameter().getParameter("alg_brk_pair").getAsString(true) = 
					getAlgHolder().getAlgBrokerDescriptor().name_m;
		
			//response.getAsXmlParameter().getParameter("is_simulation").getAsInt(true) = isBacktestLibrary();

			// session inique if
			response.setSource(session.getUid().toString());
			
		ISEVENTSUBSCRIBED_END(response);
	}

	void BrokerBase::sendSessionDestroyEvent(BrokerSession const& session, CppUtils::String const& runName)
	{
		ISEVENTSUBSCRIBED_BEGIN(HlpStruct::EventData::ET_BrokerSessionEvent)
			HlpStruct::EventData response;


			response.getEventType() = HlpStruct::EventData::ET_BrokerSessionEvent;
			response.setType(HlpStruct::EventData::DT_CommonXml);
		
			// time when session connected
			response.getAsXmlParameter(true).getParameter("time").getAsDouble(true) = CppUtils::getTime();
			// this is old entry - must update 
			response.getAsXmlParameter().getParameter("operation").getAsInt(true) = BrokerSession::SET_Destroy;
			response.getAsXmlParameter().getParameter("thread").getAsInt(true) = CppUtils::getThreadHandle();
			response.getAsXmlParameter().getParameter("session_name").getAsString(true) = runName;
			
			//
			
			response.getAsXmlParameter().getParameter("alg_brk_pair").getAsString(true) = 
					getAlgHolder().getAlgBrokerDescriptor().name_m;
		
			//response.getAsXmlParameter().getParameter("is_simulation").getAsInt(true) = isBacktestLibrary();

			// session inique if  
			response.setSource(session.getUid().toString());
			
		ISEVENTSUBSCRIBED_END(response);
	}


	/**
	*/

	bool BrokerBase::processBrokerRelatedEvent(HlpStruct::EventData const &request, HlpStruct::EventData &response )
	{

		response.getEventType() = HlpStruct::EventData::ET_AlgorithmBrokerEventResp_FromBroker;
		response.setType(HlpStruct::EventData::DT_CommonXml);

		// set parent UID
		response.setParentUid(request.getEventUid());

		
		//
		
		HlpStruct::XmlParameter& respxml = response.getAsXmlParameter();

		HlpStruct::CallingContext callContext;
		try {

				// get request XMl reference
				HlpStruct::XmlParameter const& reqxml = request.getAsXmlParameter();

				// prepare for the call
				BrokerProxyBaseCreator creator;
				HlpStruct::convertCallingContextFromXmlParameter(creator, callContext, reqxml);

				// need to determine whether we are coling correct thread ???
				CppUtils::String const& algLibPair = callContext.getParameter<String_Proxy const>("alg_lib_pair").get();

				if (algLibPair.size() <= 0 || algLibPair!=getAlgHolder().getAlgBrokerDescriptor().name_m) {
					// out 
					//THROW(CppUtils::OperationFailed, "exc_InvalidAlgLibPairName", "ctx_BrokerRelatedEvent", algLibPair);
					LOG( MSG_WARN, BRKLIB, "Ignoring call to broker layer '" << algLibPair << "' as this is " << getAlgHolder().getAlgBrokerDescriptor().name_m);

					// do not raise response event
					return false;
				}

				  
				if (callContext.getCallName() == "remote_createBrokerConnectionObject") {

					// create connect object or resolve
					BrokerConnect brkConnect(
						getAlgHolder().getBroker(),
						callContext.getParameter<String_Proxy const>("connect_string").get().c_str(), 
						false, 
						false, 
						callContext.getParameter<String_Proxy const>("run_name").get().c_str(),
						callContext.getParameter<String_Proxy const>("run_comment").get().c_str()
					);

					// pass connection UID back and the connection status
					callContext.getParameter<Uid_Proxy>("uid").get() = brkConnect.getSession().getUid();
					callContext.getParameter<Bool_Proxy>("is_connected").get() = brkConnect.getSession().isConnected();
					callContext.getParameter<Int_Proxy>("state").get() = brkConnect.getSession().getState();
				
					
				}
				else if (callContext.getCallName() == "remote_deleteBrokerConnectionObject") {
					// resolve connection object via session UID
					CppUtils::Uid const& uid = callContext.getParameter<Uid_Proxy const>("uid").get();
	
					BrokerSession& brksession = resolveSessionObject(uid);

					// create session and disconned this via destructor
					BrokerConnect brkConnect(brksession, true, true);
					
				}
				else if (callContext.getCallName() == "remote_returnAvailableBrokerConnections") {
					CppUtils::UidList sessions;
					CppUtils::StringList connectStrings, runNames, runComments;
					vector<bool> connectStatuses;
					CppUtils::IntList states;

					getAvailableSessions(sessions);
					
										
					for(int i = 0; i < sessions.size(); i++) {
						BrokerSession& brksession = resolveSessionObject(sessions[i]);
						connectStrings.push_back(brksession.getConnection());
						connectStatuses.push_back(brksession.isConnected());
						states.push_back(brksession.getState());
						runNames.push_back(brksession.getRunName());
						runComments.push_back( brksession.getRunComment());
						
					}
					
					callContext.getParameter<UidList_Proxy>("uid_sessions").get() = sessions;
					callContext.getParameter<StringList_Proxy>("connect_string_sessions").get() = connectStrings;
					callContext.getParameter<BoolList_Proxy>("connect_statuses_sessions").get() = connectStatuses;
					callContext.getParameter<IntList_Proxy>("states_sessions").get() = states;
					callContext.getParameter<StringList_Proxy>("run_names_sessions").get() = runNames;
					callContext.getParameter<StringList_Proxy>("run_comments_sessions").get() = runComments;
					


				}
				else if (callContext.getCallName() == "remote_connectBrokerConnectionObject") {
			

					CppUtils::Uid const& uid = callContext.getParameter<Uid_Proxy const>("uid").get();

					BrokerSession& brksession = resolveSessionObject(uid);
					// attempt to connect
					// create session and sisconned this via destructor
					BrokerConnect brkConnect(brksession, false, false);

					brkConnect.connect();

					if (!brkConnect.getSession().isConnected())
						THROW(CppUtils::OperationFailed, "exc_CannotConnectSesion", "ctx_remote_connectBrokerConnectionObject", brkConnect.getSession().getUid().toString());
					
					callContext.getParameter<Bool_Proxy>("is_connected").get() = brkConnect.getSession().isConnected();
					callContext.getParameter<Int_Proxy>("state").get() = brkConnect.getSession().getState();
					callContext.getParameter<String_Proxy>("run_name").get() = brkConnect.getSession().getRunName();
					callContext.getParameter<String_Proxy>("run_comment").get() = brkConnect.getSession().getRunComment();
					callContext.getParameter<String_Proxy>("connect_string").get() = brkConnect.getSession().getConnection();



				}
				else if (callContext.getCallName() == "remote_disconnectBrokerConnectionObject") {
					CppUtils::Uid const& uid = callContext.getParameter<Uid_Proxy const>("uid").get();

					BrokerSession& brksession = resolveSessionObject(uid);
					// attempt to connect
					// create session and sisconned this via destructor
					BrokerConnect brkConnect(brksession, false, false);

					brkConnect.disconnect();
					callContext.getParameter<Bool_Proxy>("is_connected").get() = brkConnect.getSession().isConnected();
					callContext.getParameter<Int_Proxy>("state").get() = brkConnect.getSession().getState();
					callContext.getParameter<String_Proxy>("run_name").get() = brkConnect.getSession().getRunName();
					callContext.getParameter<String_Proxy>("run_comment").get() = brkConnect.getSession().getRunComment();
					callContext.getParameter<String_Proxy>("connect_string").get() = brkConnect.getSession().getConnection();

				}
				else if (callContext.getCallName() == "remote_rebounceBrokerConnection") {
					// resolve connection object via session UID
					CppUtils::Uid const& uid = callContext.getParameter<Uid_Proxy const>("uid").get();

					BrokerSession& brksession = resolveSessionObject(uid);

					// is connected
					bool is_connected = brksession.isConnected();

					// copy strings as this is deleted
					CppUtils::String runName = brksession.getRunName();
					CppUtils::String runComment = brksession.getRunComment();
					CppUtils::String connectString = brksession.getConnection();

					// delete
					{
						// create session and delete this via destructor
						BrokerConnect brkConnect(brksession, true, true);
					}

					
					// create
					BrokerConnect brkConnectNew(
						getAlgHolder().getBroker(),
						connectString.c_str(), 
						false, 
						false, 
						runName.c_str(),
						runComment.c_str()
					);

					if (is_connected) {
						// connect
						BrokerConnect brkConnect(brkConnectNew.getSession(), false, false);

						brkConnect.connect();
						
						if (!brkConnect.getSession().isConnected())
							THROW(CppUtils::OperationFailed, "exc_CannotConnectSesion", "ctx_remote_rebounceBrokerConnection", brkConnect.getSession().getUid().toString());

					} 
					 
					
				}
				else if (callContext.getCallName() == "remote_op_getBrokerPositionList") {
					// get broker position list
					CppUtils::Uid const& uid = callContext.getParameter<Uid_Proxy const>("uid").get();
					PosStateQuery pos_state_query = (PosStateQuery)callContext.getParameter<Int_Proxy const>("pos_state").get();

					BrokerSession& brksession = resolveSessionObject(uid);
					
					BrokerConnect brkConnect(brksession, false, false);
					BrokerOperation oper(brkConnect);

					// return
					PositionList posList;
					oper.getBrokerPositionList(pos_state_query, posList);
					callContext.getParameter<PositionList_Proxy>("position_list").get() = posList;

				}
				else if (callContext.getCallName() == "remote_op_unblockingIssueOrder") {
					// get broker position list
					CppUtils::Uid const& uid = callContext.getParameter<Uid_Proxy const>("uid").get();
					Order const& order = callContext.getParameter<Order_Proxy const>("order").get();

					BrokerSession& brksession = resolveSessionObject(uid);
					
					BrokerConnect brkConnect(brksession, false, false);
					BrokerOperation oper(brkConnect);
					// return
					 

					bool result = oper.unblockingIssueOrder(order);
					callContext.getParameter<Bool_Proxy>("order_issue_result").get() = result;

				}
				else if (callContext.getCallName() == "remote_BrokerCustomEvent") {
					// THIS IS SPECIALLY GOES TO DERIVED
					CppUtils::String customOutData;
					onCustomBrokerEventArrived(callContext, customOutData);
					callContext.getParameter<BrkLib::String_Proxy>("out_return_data").get() = customOutData;

					// clear custom data as no need !!!
					// and clear to avoid problems on returning
					callContext.removeParameter("custom_data");

				}
				else
					THROW(CppUtils::OperationFailed, "exc_InvalidCallName", "ctx_BrokerRelatedEvent", callContext.getCallName());

				// assume everything is fine
				callContext.getResultCode() = 0;

		
		}
		catch(CppUtils::Exception& e) {
				callContext.getResultReason() = "Exception: " + e.message() + " - " + e.context() + " - " + e.arg();
				callContext.getResultCode() = -1;

		}
#ifdef HANDLE_NATIVE_EXCEPTIONS
		catch(Testframe::Exception& e) {
				callContext.getResultReason() = "MSVC Exception: " + e.message();
				callContext.getResultCode()= -2;

				
		}
#endif
		catch(...) {
				callContext.getResultReason() = "Unknown exception";
				callContext.getResultCode() = -3;
		}

		// create response
		HlpStruct::convertCallingContextToXmlParameter(callContext, respxml);
		
		return true;

	}

	void BrokerBase::registerDelayedOrder(
			BrokerSession &session, 
			DelayedOrderType const type, 
			double const& execPrice, 
			Order const& order,
			CppUtils::String const& comment
		)
	{
		LOCK_FOR_SCOPE(delayedOrdersMtx_m);

		DelayedOrder &newdorder = delayedOrders_m[ order.ID_m ];
		newdorder.order_m = order;
		newdorder.execType_m = type;
		newdorder.executionPrice_m = execPrice;
		newdorder.comment_m = comment;

		// send notofication

		LOGEVENT(HlpStruct::CommonLog::LL_INFO, BRKLIB, "Registered delayed order with order: " << newdorder.toString() );

		// event
		BrkLib::BrokerResponseDelayedOrder brokerRespDO(
			DORT_REGISTER_ORDER, 
			type, 
			order, 
			execPrice,
			comment
		);

		brokerRespDO.parentID_m = order.ID_m;
		brokerRespDO.context_m =	order.context_m;
	
		onBrokerResponseArrived(session, brokerRespDO);


	}

	
	bool BrokerBase::removeDelayedOrder(BrokerSession &session, CppUtils::Uid const& uid)
	{
		LOCK_FOR_SCOPE(delayedOrdersMtx_m);

		
		map<CppUtils::Uid, DelayedOrder>::iterator it = delayedOrders_m.find(uid);
		if (it != delayedOrders_m.end()) {

			LOGEVENT(HlpStruct::CommonLog::LL_INFO, BRKLIB, "Removing delayed order: " <<  it->second.toString());

			BrkLib::BrokerResponseDelayedOrder brokerRespDO(
				DORT_UNREGISTER_ORDER, 
				it->second.execType_m, 
				it->second.order_m, 
				it->second.executionPrice_m,
				it->second.comment_m
			);

			brokerRespDO.parentID_m = it->second.order_m.ID_m;
			brokerRespDO.context_m =	it->second.order_m.context_m;
			onBrokerResponseArrived(session, brokerRespDO);

			delayedOrders_m.erase( uid );

			return true;
	
		}

		return false;


	}

	void BrokerBase::removeAllDelayedOrders(BrokerSession &session)
	{
		LOCK_FOR_SCOPE(delayedOrdersMtx_m);

		LOGEVENT(HlpStruct::CommonLog::LL_INFO, BRKLIB, "Removing all delayed orders");

		for(map<CppUtils::Uid, DelayedOrder>::iterator it = delayedOrders_m.begin(); it != delayedOrders_m.end(); it++ ) {
			BrkLib::BrokerResponseDelayedOrder brokerRespDO(
				DORT_UNREGISTER_ORDER, 
				it->second.execType_m, 
				it->second.order_m, 
				it->second.executionPrice_m,
				it->second.comment_m
			);

			brokerRespDO.parentID_m = it->second.order_m.ID_m;
			brokerRespDO.context_m =	it->second.order_m.context_m;
			onBrokerResponseArrived(session, brokerRespDO);
		}

		delayedOrders_m.clear();
	}

	void BrokerBase::onDelayedOrderExecutionCheck(
		BrokerSession& session, 
		double const& currentPrice, 
		CppUtils::String const& provider,
		CppUtils::String const& symbol
	)
	{
	
		// need to allow recursive calls
		vector<DelayedOrder> to_process;
		
		{
			LOCK_FOR_SCOPE(delayedOrdersMtx_m);
		  
			

			for(map<CppUtils::Uid, DelayedOrder>::iterator it = delayedOrders_m.begin(); it != delayedOrders_m.end(); ) {
				
				DelayedOrder const& order_i = it->second;

				if (order_i.order_m.provider_m != provider || order_i.order_m.symbol_m != symbol) {
					++it;
					continue;
				}

										
				if (order_i.executionPrice_m > 0 && order_i.execType_m == DO_EXEC_PRICE_MORE && currentPrice >= order_i.executionPrice_m)  {

					to_process.push_back(order_i);
					delayedOrders_m.erase(it++);

				}
				else if (order_i.executionPrice_m > 0 && order_i.execType_m == DO_EXEC_PRICE_LESS && currentPrice <= order_i.executionPrice_m) {
					
					to_process.push_back(order_i);
					delayedOrders_m.erase(it++);
					
				}
				else
					 ++it;

			
			} // end loop

			// remove
			for(int i = 0; i < to_process.size(); i++)	 {
					DelayedOrder const& order_i = to_process[i];

					LOGEVENT(HlpStruct::CommonLog::LL_INFO, BRKLIB, "Delayed order is going to be executed: " << order_i.toString() << " price reached: " << currentPrice );
					
					BrkLib::BrokerResponseDelayedOrder brokerRespDO(
						DORT_ISSUE_ORDER, 
						order_i.execType_m, 
						order_i.order_m, 
						order_i.executionPrice_m,
						order_i.comment_m
					);

					brokerRespDO.parentID_m = order_i.order_m.ID_m;
					brokerRespDO.context_m =	order_i.order_m.context_m;
					onBrokerResponseArrived(session, brokerRespDO);

					// having called from here we can be sure that delayedOrders_m can be changed using recursive calls
					// like removeAllDelayedOrders(...) or removeDelayedOrder(...)
					unblockingIssueOrder(session, order_i.order_m);
			}


		} // end mtx

		

	}

	
	void BrokerBase::setUpMode(int mode)
	{
		brokerMode_m = mode;
	}

}; // end of namespace