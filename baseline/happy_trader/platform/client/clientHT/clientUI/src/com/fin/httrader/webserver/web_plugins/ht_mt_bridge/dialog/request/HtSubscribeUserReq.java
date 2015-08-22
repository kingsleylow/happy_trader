/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.request;

import com.fin.httrader.eventplugins.ht_mt_bridge.HtMtManager;
import com.fin.httrader.eventplugins.ht_mt_bridge.delivery.HtMtEventWrapperAccountChange;
import com.fin.httrader.eventplugins.ht_mt_bridge.delivery.HtMtEventWrapperAccountHistory;
import com.fin.httrader.eventplugins.ht_mt_bridge.delivery.HtMtEventWrapperAccountMetainfoChange;
import com.fin.httrader.eventplugins.ht_mt_bridge.delivery.HtMtEventWrapperBalanceChange;
import com.fin.httrader.eventplugins.ht_mt_bridge.delivery.HtMtEventWrapperBase;
import com.fin.httrader.eventplugins.ht_mt_bridge.delivery.HtMtEventWrapperHeartbeat;
import com.fin.httrader.eventplugins.ht_mt_bridge.delivery.HtMtEventWrapperPositionChange;
import com.fin.httrader.eventplugins.ht_mt_bridge.delivery.HtMtEventWrapperPositionsHistory;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.HtMtJsonUtils;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibrequest.Req_START_POLLING_USERS;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibrequest.Req_UPDATE_USER_ACCESS_TIME;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibresponse.Resp_START_POLLING_USERS;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibresponse.Resp_UPDATE_USER_ACCESS_TIME;
import com.fin.httrader.interfaces.HtLongPollManagerEventFilter;
import com.fin.httrader.managers.RtConfigurationManager;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response.HtBaseIntResp;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response.HtGetAccountHistoryResp;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response.HtGetPositionHistoryResp;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response.HtSubscribeUserResp;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.utils.HtHelperUtils;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.logging.Level;
import org.json.simple.JSONObject;
import org.json.simple.JSONValue;

/**
 *
 * subscribe for some events + obtaine initail chunk of data
 */
public class HtSubscribeUserReq extends HtBaseIntReq {

		public static final String COMMAND_NAME = "SUBSCRIBE_USER_REQ";
		public static final int QUEUE_TOUT_MSEC = 2000; 

		// this is global cookie counter
		private static final AtomicInteger nextCookie_m = new AtomicInteger(0);

	
		
				
		String companyName_m = null;
		String accountName_m = null;
		String accountSever_m = null;
		String userPassword_m = null;
		String mtInstnace_m = null;
		
		// to obtain hostory
		// if they are NULL we don't need to get history
		private HtGetAccountHistoryReq internalAccHistRequest_m = null;
		private HtGetPositionHistoryReq internalPosHistRequest_m = null;

		// this keeps track of the latets event from the queue
		// when -1 the queue is initialized
		private int last_cookie_m = -1;

		// flags to show which event details we expect to see
		// note that there are global setting that override
		// HtMtManager
		//
		//private boolean proocessAccountUpdate_m = false;
		//private boolean proocessBalanceUpdate_m = false;
		private String getContext() {
				return this.getClass().getSimpleName();
		}
		
		public int getCurrentCookie()
		{
				return nextCookie_m.get();
		}

		@Override
		public String getCommandName() {
				return COMMAND_NAME;
		}

		@Override
		public void onErrorHappened()
		{
				HtLog.log(Level.INFO, getContext(), "onErrorHappened", "Clearing long poll manager for queue with id: "+
					getCurrentCookie() );
				try {
						
						HtCommandProcessor.instance().get_HtLongPollProxy().remote_removeEventQueue(getCurrentCookie());
				}
				catch(Throwable e)
				{
						HtLog.log(Level.WARNING, getContext(), "onErrorHappened", "Exception: " + e.getMessage() + ", ignoring...");
				}
		}
		
		
		@Override
		public HtBaseIntResp processRequest() throws Exception {
				HtSubscribeUserResp resp = new HtSubscribeUserResp(getCommandName());
				
				if (!HtMtManager.getInstance().isInitialized())
						throw new HtException(getContext(), "processRequest", "HtMtManager not initialized");
				
				int old_cookie = last_cookie_m;
				
				if (old_cookie < 0) {
						this.mtInstnace_m =  pollUser(this.companyName_m, this.accountName_m, this.userPassword_m, this.mtInstnace_m, accountSever_m );
						
				}
				
				// must be valid
				if (HtUtils.nvl(this.mtInstnace_m))
							throw new HtException(getContext(), "processRequest", "invalid MT instance");
				
				resp.setMtInstnace(this.mtInstnace_m);

				// then we need to init the queue 
				if (last_cookie_m < 0) {
						initQueue();
						// init cookie name
						last_cookie_m = nextCookie_m.get();
				}
				
				// after subscription obtain history
				if (old_cookie < 0)
						obtainHistory(resp);
				
				waitForEvent(resp);
				
				// probably the best place to update user access time
				confirmMTToKeepUserAlive(mtInstnace_m, accountName_m);
				
				return resp;
		}

		@Override
		public void initialize(JSONObject root_json) throws Exception {

				// set this
				// TODO
				companyName_m = HtMtJsonUtils.<String>getJsonField(root_json, "COMPANY_NAME");
				accountName_m = HtMtJsonUtils.<String>getJsonField(root_json, "ACCOUNT_NAME");
				userPassword_m = HtMtJsonUtils.<String>getJsonField(root_json, "ACCOUNT_PASSWORD");
				accountSever_m = HtMtJsonUtils.<String>getJsonField(root_json, "SERVER");
				
			
				mtInstnace_m = HtMtJsonUtils.<String>getJsonField(root_json, "MT_INSTANCE");
				
				
				// accept 
				//objects
				// can be NULL
				JSONObject history_acc_request = (JSONObject) root_json.get("HISTORY_ACCOUNT_REQUEST");
				JSONObject history_pos_request = (JSONObject) root_json.get("HISTORY_POSITION_REQUEST");
				// if new polling then PREV_COOKIE == -1, else this is OLD COOKIE
				last_cookie_m = HtMtJsonUtils.getIntJsonField(root_json, "PREV_COOKIE");

				if (last_cookie_m >= 0) {
						if (history_acc_request != null) {
								throw new HtException(getContext(), "initialize", "Cannot fetch account history as initial request already done.");
						}

						if (history_pos_request != null) {
								throw new HtException(getContext(), "initialize", "Cannot fetch position history as initial request already done.");
						}
				}

				if (history_acc_request != null) {
						internalAccHistRequest_m = new HtGetAccountHistoryReq();
						internalAccHistRequest_m.initialize(history_acc_request);
				}

				if (history_pos_request != null) {
						internalPosHistRequest_m = new HtGetPositionHistoryReq();
						internalPosHistRequest_m.initialize(history_pos_request);
				}

		}

		/**
		 * helpers
		 */
		
		private void confirmMTToKeepUserAlive(String mtInstnace, String accountName ) throws Exception
		{
				Req_UPDATE_USER_ACCESS_TIME req = new Req_UPDATE_USER_ACCESS_TIME();
				req.getUsers().add(accountName);
				req.setMtInstnaceName(mtInstnace);
				
				JSONObject custom_data_json = new JSONObject();
				req.toJson(custom_data_json);
				
				String result_data = HtMtManager.getInstance().sendCustomEventToHtMtAlgorithm(
							req.getMtCommand(), 
							JSONValue.toJSONString(custom_data_json), 
							RtConfigurationManager.StartUpConst.WAIT_RESPONSE_FROM_TRADING_SERVER_SEC
				);
				
				Resp_UPDATE_USER_ACCESS_TIME resp = new Resp_UPDATE_USER_ACCESS_TIME();
				resp.fromJson((JSONObject)JSONValue.parseWithException(result_data));
				
				// just check the result
				HtHelperUtils.checkAlgLibResponse(resp);
					
				
		}
		
		private String pollUser(String companyName, String accountName, String password, String mtInstance, String accountSever) throws Exception
		{
						Req_START_POLLING_USERS internalPollRequest = new Req_START_POLLING_USERS();
						
						// can be null
						internalPollRequest.setMtInstnaceName(mtInstance);
				
						Req_START_POLLING_USERS.MtUser user =  new Req_START_POLLING_USERS.MtUser();

						user.setLifeTime(HtMtManager.getInstance().getRunningParams().getMaxDelayPollQueueAliveSec());
						user.setUserName(accountName);
						user.setPassword(password);
						user.setServer(accountSever);
						
						
						if (!user.isValid())
								throw new HtException(getContext(), "pollUser", "Invalid user data");
						
						internalPollRequest.getUsers().add(user);
						internalPollRequest.setCompanyName(companyName);

						JSONObject custom_data_json = new JSONObject();
						internalPollRequest.toJson(custom_data_json);

						// object is ready to be sent to next layer
						String result_data = HtMtManager.getInstance().sendCustomEventToHtMtAlgorithm(
							internalPollRequest.getMtCommand(), 
							JSONValue.toJSONString(custom_data_json), 
							RtConfigurationManager.StartUpConst.WAIT_RESPONSE_FROM_TRADING_SERVER_SEC
						);

						Resp_START_POLLING_USERS pollresp = new Resp_START_POLLING_USERS();
						// obtain response
						pollresp.fromJson((JSONObject)JSONValue.parseWithException(result_data));
						
						// just check the result
						HtHelperUtils.checkAlgLibResponse(pollresp);
						
						// pass MT instance
						return pollresp.getMt_instance_name();
						
						
		}
		
		private void obtainHistory(HtSubscribeUserResp resp) throws Exception
		{
				if (internalAccHistRequest_m != null || internalPosHistRequest_m != null) {

						List<HtMtEventWrapperBase> history_list = new ArrayList<HtMtEventWrapperBase>();

						if (internalAccHistRequest_m != null) {
								HtGetAccountHistoryResp accResp = (HtGetAccountHistoryResp) internalAccHistRequest_m.processRequest();
								HtMtEventWrapperAccountHistory accDelivery = accResp.getAccHistoryObject();

								
								history_list.add(accDelivery);
						}

						if (internalPosHistRequest_m != null) {
								HtGetPositionHistoryResp posResp = (HtGetPositionHistoryResp) internalPosHistRequest_m.processRequest();
								HtMtEventWrapperPositionsHistory posDelivery = posResp.getPosHistoryObject();

								history_list.add(posDelivery);
						}

						// init history list
						resp.setDataHistoryList(history_list);
				}

		}
		
		private void initQueue() throws Exception
		{
				int new_cookie = nextCookie_m.incrementAndGet();
				HtLog.log(Level.INFO, getContext(), "initQueue", "Initializing new queue with cookie: " + new_cookie);
				
				final String  accountId_s = this.accountName_m;
				final String companyName_s = this.companyName_m;

				// create a filter to obtain obly necessaru XML events
				HtLongPollManagerEventFilter filter = new HtLongPollManagerEventFilter() {

						@Override
						public boolean match(XmlEvent xml_event){
								if (xml_event.getEventType() == XmlEvent.ET_Java_Object_Common && 
									xml_event.getType() == XmlEvent.DT_Java_Object) {
										Object obj = xml_event.getJavaObject();
										if (obj == null)
												return false;
										
										if (obj instanceof HtMtEventWrapperBase) {
												
												// must match account name
												// and company name
												HtMtEventWrapperBase inst = (HtMtEventWrapperBase)obj;
												try {
														return inst.routEventFor(accountId_s, companyName_s);
																												}
												catch(Throwable e)
												{
														HtLog.log(Level.WARNING, "HtLongPollManagerEventFilter", "match", "Exception on routing events: " + e.getMessage());
														XmlEvent.createSimpleLog("", "HtLongPollManagerEventFilter", CommonLog.LL_WARN, "Exception on routing events: " + e.getMessage());
												}
												
												return false;
										}
								}

								return false;
						}
				};
				
				if (!HtMtManager.getInstance().isInitialized())
						throw new HtException(getContext(), "initQueue", "HtMtManager not initialized");
				
				HtCommandProcessor.instance().get_HtLongPollProxy().remote_initEventQueue(
							new_cookie,
							XmlEvent.ET_Java_Object_Common,
							HtMtManager.getInstance().getRunningParams().getMaxDelayPollQueueAliveSec(),
							filter);

		}
		
		void waitForEvent(HtSubscribeUserResp resp) throws Exception
		{
				while (true) {
						XmlEvent xml_event = HtCommandProcessor.instance().get_HtLongPollProxy().
							remote_popQueueElement(last_cookie_m, QUEUE_TOUT_MSEC);
						
						if (xml_event == null)
								continue;
							
					
						HtMtEventWrapperBase received_obj = (HtMtEventWrapperBase)xml_event.getJavaObject();
						
						// init response object
						resp.setLastCookieId(last_cookie_m);
						resp.setLastData(received_obj);
						resp.setPendingSize(HtCommandProcessor.instance().get_HtLongPollProxy().remote_getQueuePendingSize(last_cookie_m));
						resp.setSequenceId( xml_event.getSequenceNumber() );
							
						break;
						
				}
		}
}
