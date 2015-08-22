/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.eventplugins.ht_mt_bridge;

import com.fin.httrader.eventplugins.ht_mt_bridge.delivery.HtMtEventWrapperAccountChange;
import com.fin.httrader.eventplugins.ht_mt_bridge.delivery.HtMtEventWrapperAccountHistory;
import com.fin.httrader.eventplugins.ht_mt_bridge.delivery.HtMtEventWrapperAccountMetainfoChange;
import com.fin.httrader.eventplugins.ht_mt_bridge.delivery.HtMtEventWrapperBalanceChange;
import com.fin.httrader.eventplugins.ht_mt_bridge.delivery.HtMtEventWrapperBase;
import com.fin.httrader.eventplugins.ht_mt_bridge.delivery.HtMtEventWrapperHeartbeat;
import com.fin.httrader.eventplugins.ht_mt_bridge.delivery.HtMtEventWrapperPositionChange;
import com.fin.httrader.eventplugins.ht_mt_bridge.delivery.HtMtEventWrapperPositionsHistory;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.InitializationParameters;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.RunningParameters;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibout.MtDataPacketOut;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibout.MtStatusOut;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibrequest.Req_G_GET_ALL_METADATA;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibrequest.Req_START_ORDER_LIST;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibresponse.Resp_G_GET_ALL_METADATA;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibresponse.Resp_START_ORDER_LIST;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.mt.Mtproxy_Data;
import com.fin.httrader.eventplugins.ht_mt_bridge.mtstruct_db.*;
import com.fin.httrader.eventplugins.ht_mt_bridge.persistent.MainPersistManager;
import com.fin.httrader.eventplugins.ht_mt_bridge.persistent.TransactionManager;
import com.fin.httrader.hlpstruct.PosState;
import com.fin.httrader.managers.RtConfigurationManager;
import com.fin.httrader.managers.RtGlobalEventManager;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.IntParam;
import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.utils.hlpstruct.HtPair;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.utils.HtHelperUtils;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.logging.Level;
import org.hibernate.Session;
import org.json.simple.JSONObject;
import org.json.simple.JSONValue;



/**
 *
 * @author Victor_Zoubok base c lass to rpovide all API
 */
public class HtMtManager {

		private static volatile HtMtManager instance_m = null;

		MainPersistManager persistmanager_m = new MainPersistManager();
		
	
		// init params are deals as atomic structure
		private final InitializationParameters initParams_m = new InitializationParameters();
		private final RunningParameters runningParams_m = new RunningParameters();
		
		
		
		// ---------------------
		public static HtMtManager getInstance() {
				if (instance_m == null) {
						synchronized (HtMtManager.class) {
								if (instance_m == null) {
										instance_m = new HtMtManager();
								}
						}
				}
				
				return instance_m;
		}

		// ----------------
		private String getContext() {
				return getClass().getSimpleName();
		}

		public HtMtManager() {

		}

		// initialize
		public synchronized void initialize(
			Map<String, String> initdata,
			String eventPluginId
		
	) throws Exception {
				
				getInitParams().initialize(
					initdata.get(HtMtBridgeEventProcessor.SERVERNAME_PARAM),
					initdata.get(HtMtBridgeEventProcessor.ALGLIB_PARAM),
					eventPluginId,
					HtUtils.parseInt(  initdata.get(HtMtBridgeEventProcessor.TREADID_PARAM) )
				);
				
				getRunningParams().initFromParamMap(initdata);
				
				
				if (HtUtils.nvl(getInitParams().getCurrentServerName()) || 
					HtUtils.nvl(getInitParams().getCurrentAlgLibName()) || 
					getInitParams().getCurrentThread() <=0)
						throw new HtException(getContext(), "initialize", "Invalid parameters");
				
				
				persistmanager_m.initialize();
				XmlEvent.createSimpleLog("", getContext(), CommonLog.LL_INFO, "Initialized running parameters: " +  getRunningParams().toString());
				
		}

		
		public RunningParameters getRunningParams()
		{
				return this.runningParams_m;
		}
		
		public synchronized void deinitialize() {
				try {
						initParams_m.clear();
						
						persistmanager_m.deinitialize();
				} catch (Throwable e) {
						HtLog.log(Level.WARNING, getContext(), "deinitialize", "Exception: " + e.getMessage());
				}
		}

		public boolean isInitialized()
		{
				return (initParams_m.getCurrentThread() != -1);
		}
		
		public InitializationParameters getInitParams()
		{
				return initParams_m;
				
		}
		
		/*
		public void processConnectEvent(String data) throws Exception
		{
				MtStatusOut status = new MtStatusOut();
				JSONObject root_json = (JSONObject) JSONValue.parseWithException(data);
				status.fromJson(root_json);
				
				if (status.getStatus() == MtStatusOut.MT_CONNECTED)
						HtLog.log(Level.FINE, getContext(), "processConnectEvent", "MT layer connected: " + status.getMtInstance());
				
				if (status.getStatus() == MtStatusOut.MT_DISCONNECTED)
						HtLog.log(Level.FINE, getContext(), "processConnectEvent", "MT layer disconnected: " + status.getMtInstance() );
		}
		*/
		
		// jeneric data which parse JSON and push everuthing into DV
		// note that this is to be executed in a single thread
		// as we have only one writer
		public void putJsonData(String data) throws Exception {
				// need to parse this
				
				//HtLog.log(Level.INFO, getContext(), "putJsonData", "Data: \n" + data + "\n");
			
				boolean proocessAccountUpdate = this.getRunningParams().getProcessAccountUpdate();
				boolean proocessBalanceUpdate = this.getRunningParams().getProcessBalanceUpdate();
						
						
				MtDataPacketOut pout = new MtDataPacketOut();
				JSONObject root_json = (JSONObject) JSONValue.parseWithException(data);

				pout.fromJson(root_json);

				// get into RAW data
				// our main data
				Mtproxy_Data mtData = pout.getMtData();
				HtMtBalanceUpdate balanceupd = null;
				
				if (proocessBalanceUpdate) {
						balanceupd = new HtMtBalanceUpdate();
				}
				HtMtServers server = new HtMtServers();
				HtMtAccountMetainfo accminfo = new HtMtAccountMetainfo();
				
				HtMtAccountUpdate accupdt = null;
				if (proocessAccountUpdate) {
						accupdt = new HtMtAccountUpdate();
				}
				HtMtCompanyMetainfo compminfo = new HtMtCompanyMetainfo();
				List<HtMtPosition> positions = new ArrayList<HtMtPosition>();

				mtData.parseData(
					server, 
					accminfo, 
					accupdt, 
					compminfo, 
					positions, 
					balanceupd
				);

			
				boolean success = false;
				TransactionManager trx = new TransactionManager(persistmanager_m.getSessionFactory().getCurrentSession());
				
				try {
						trx.startTransaction();

						boolean comp_minfo_chnaged = persistmanager_m.save_HtMtCompanyMetainfo(trx, compminfo);
						boolean acc_minfo_chnaged = persistmanager_m.save_HtMtAccountMetainfo(trx, compminfo, accminfo);

						if (comp_minfo_chnaged || acc_minfo_chnaged) {

								HtMtEventWrapperAccountMetainfoChange e = new HtMtEventWrapperAccountMetainfoChange();
								e.setAccountId(accminfo.getAccountId());
								e.setCompanyName(compminfo.getCompanyName());
								e.setCurrency(accminfo.getCurrency());
								e.setHolderName(accminfo.getHolderName());
								e.setLeverage(accminfo.getLeverage());
								e.setServer(accminfo.getServer());

								pushEvent(e);
						}
						
					

						// account update
						if (proocessAccountUpdate) {
								HtMtEventWrapperAccountChange e_acc = persistmanager_m.save_HtMtAccountUpdate(trx, accupdt, accminfo, compminfo);
								if (e_acc != null) 
										pushEvent(e_acc);
						}
						

						if (proocessBalanceUpdate) {
								HtMtEventWrapperBalanceChange e_balc = persistmanager_m.save_HtBalanceUpdate(trx, balanceupd, accminfo,compminfo);

								if (e_balc != null) 
										pushEvent(e_balc);
						}
						

						for (int i = 0; i < positions.size(); i++) {
								HtMtPosition pos_i = positions.get(i);

								HtMtEventWrapperPositionChange e_posc = persistmanager_m.save_HtMtPosition(trx, pos_i, accminfo, compminfo);
								if (e_posc != null) {
										pushEvent(e_posc);
										
										// just an example
										//createOrderCommandBackForMt(e_posc);
								}
								

						}
						
						

						success = true;

				} catch (Throwable e) {
						trx.rollBackTransaction();
						throw new HtException(getContext(), "putJsonData", "Error on saving data: " + e.getMessage() + ", rolling back...");
						
				} finally {
						if (success) {
								trx.commitTransaction();
						}
				}
				
				

		}

		// heartbeat data
		public void putHeartBeat() throws Exception {
				// need to warp and send as another event
				HtMtEventWrapperHeartbeat tb = new HtMtEventWrapperHeartbeat();
				pushEvent(tb);
		}

		// query open or close or all positions
		public HtMtEventWrapperPositionsHistory queryPositions(
			String accountId, 
			String companyName,
			String serverName,
			int flag_state, 
			long openTimeFilter, 
			long closeTimeFilter,
			int flag
		) throws Exception {
				
				TransactionManager trx = new TransactionManager(persistmanager_m.getSessionFactory().getCurrentSession());
				HtMtEventWrapperPositionsHistory r = new HtMtEventWrapperPositionsHistory();
				r.setCompanyName(companyName);
				r.setAccountId(accountId);
				
				try {
						trx.startTransaction();
						persistmanager_m.queryPositions(trx, r, accountId, companyName, serverName, flag_state, openTimeFilter, closeTimeFilter, flag);
				} 
				catch (Throwable e) 
				{
						throw new HtException(getContext(), "queryPositions", "Error: " + e.getMessage());
				} finally 
				{
							trx.commitTransaction();
				}
				
				return r;
		}

		public HtMtEventWrapperAccountHistory queryAccountDetails(String accountId, String companyName, int flag) throws Exception {
				HtMtEventWrapperAccountHistory r = new HtMtEventWrapperAccountHistory();
				TransactionManager trx = new TransactionManager(persistmanager_m.getSessionFactory().getCurrentSession());
				
				try {
						trx.startTransaction();
						persistmanager_m.queryAccountDetails(trx, r, accountId, companyName, flag);
				} 
				catch (Throwable e) 
				{
						throw new HtException(getContext(), "queryAccountDetails", "Error: " + e.getMessage());
				} finally 
				{
							trx.commitTransaction();
				}
				
				return r;
		}
		
		// delegate event call
		// to currently loaded algorithm
		public String sendCustomEventToHtMtAlgorithm(
					String custom_name,
					String custom_data,
					long timeout_sec ) throws Exception
		{
				StringBuilder srv = new StringBuilder();
				StringBuilder alglib = new StringBuilder();
				
				IntParam tid = new IntParam();
				
				this.getInitParams().getAllData(srv, alglib, null, tid);
			
				/*
				return HtCommandProcessor.instance().get_HtServerProxy().remote_SendCustomEvent_toAlgorithm(
					srv.toString(),
					alglib.toString(),
					tid.getInt(),
					custom_name,
					custom_data,
					timeout_sec
				);
				*/
				return HtCommandProcessor.instance().get_HtServerProxy().remote_SendCustomEvent_toAlgorithm_direct(
					srv.toString(),
					alglib.toString(),
					tid.getInt(),
					custom_name,
					custom_data,
					timeout_sec
				);
		}
		
		

		
		/**
		 * Helpers
		 */
		private void pushEvent(HtMtEventWrapperBase eventdata) throws Exception {
				// this pushes event data later
				if (eventdata != null) {
						XmlEvent xmlevent = new XmlEvent();
						xmlevent.setType(XmlEvent.DT_Java_Object);
						xmlevent.setEventType(XmlEvent.ET_Java_Object_Common);

						xmlevent.setJavaObject(eventdata);
						// push this event
						RtGlobalEventManager.instance().pushCommonEvent(xmlevent);

				}
		}
		
		// will propagate order commands for MT
		private void createOrderCommandBackForMt(HtMtEventWrapperPositionChange e_posc) throws Exception
		{
				Req_START_ORDER_LIST internalrequestObj = new Req_START_ORDER_LIST();
				Resp_START_ORDER_LIST internalResponseObj  = new Resp_START_ORDER_LIST();
				
				internalrequestObj.setMtInstnaceName("mt1");
				internalrequestObj.getOrderList().add("Dummy command as from:" + e_posc.getAccountId());
				
				JSONObject custom_data_json = new JSONObject();
				internalrequestObj.toJson(custom_data_json);
				
				try {
						String ret_data = sendCustomEventToHtMtAlgorithm(
							internalrequestObj.getMtCommand(), 
							JSONValue.toJSONString(custom_data_json), 
							RtConfigurationManager.StartUpConst.WAIT_RESPONSE_FROM_TRADING_SERVER_SEC);


						internalResponseObj.fromJson((JSONObject)JSONValue.parseWithException(ret_data));
							// check internal response status
						HtHelperUtils.checkAlgLibResponse(internalResponseObj);
				}
				catch(Throwable e)
				{
						HtLog.log(Level.WARNING, getContext(), "createOrderCommandBackForMt", "Exception on pushing order data: " + e.toString());
				}
				
		}
}
