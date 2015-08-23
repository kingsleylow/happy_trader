/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.request;

import com.fin.httrader.configprops.HtAlgBrkPairProp;
import com.fin.httrader.configprops.HtEventPluginProp;
import com.fin.httrader.configprops.HtServerProp;
import com.fin.httrader.eventplugins.ht_mt_bridge.HtMtBridgeEventProcessor;
import com.fin.httrader.eventplugins.ht_mt_bridge.HtMtManager;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.HtMtJsonUtils;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibrequest.Req_G_GET_ALL_METADATA;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibresponse.Resp_G_GET_ALL_METADATA;
import com.fin.httrader.hlpstruct.ThreadDescriptor;
import com.fin.httrader.managers.RtConfigurationManager;
import com.fin.httrader.managers.RtTradingServerManager;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.model.HtServerProxy;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.HtMtConstants;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response.HtBaseIntResp;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response.HtGetStatusResp;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response.HtStartInstanceResp;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.utils.HtHelperUtils;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.logging.Level;
import org.json.simple.JSONObject;
import org.json.simple.JSONValue;

/**
 *
 * start instance
 */
public class HtStartInstanceReq extends HtBaseIntReq {

		public static final String COMMAND_NAME = "START_INSTANCE_REQ";
		private boolean isDebug_m = false;
		
		private String settingsFile_m = null;
		private String actualSrvName_m = null;
		private boolean createForce_m = false; // flag showing that we must forcible recreate entries
		//private String actualAlgLibPairName_m = null;

		public String getContext() {
				return this.getClass().getSimpleName();
		}

		@Override
		public String getCommandName() {
				return COMMAND_NAME;
		}

		@Override
		public void initialize(JSONObject root_json) throws Exception {
				isDebug_m = HtMtJsonUtils.<Boolean>getJsonField(root_json, "DEBUG");
				settingsFile_m = HtMtJsonUtils.<String>getJsonField(root_json, "SETTINGS_FILE");
				createForce_m = HtMtJsonUtils.<Boolean>getJsonField(root_json, "FORCE_CREATE");

		}

		@Override
		public HtBaseIntResp processRequest() throws Exception {

				HtStartInstanceResp resp = new HtStartInstanceResp(getCommandName());

				boolean sucess = false;

				// need to start
				try {
						performStart();
						sucess = true;
				} catch (Throwable e) {
						throw new HtException(getContext(), "processRequest", "Cannot boot because of: " + e.toString()+ ", will shutdown...");
						//XmlEvent.createSimpleLog("", getContext(), CommonLog.LL_ERROR, "Cannot boot because of: " + e.getMessage()+ ", will shutdown...");
				} finally {
						if (!sucess) {
								performShutdown();
						}
				}

				return resp;
		}

		/**
		 * Helpers
		 */
		private void performShutdown() {
				try {
						if (!HtUtils.nvl(this.actualSrvName_m)) {
								HtHelperUtils.stopServer(this.actualSrvName_m);
						} else {
								throw new HtException(getContext(), "performShutdown", "Cannot resolve actual server name, ignoring...");
						}

				} catch (Throwable e) {
						XmlEvent.createSimpleLog("", getContext(), CommonLog.LL_ERROR, "Cannot shutdown the main server because of: " + e.getMessage() + ", ignoring...");
				}

				try {
						HtHelperUtils.checkEventPluginStopped(HtMtConstants.EVENT_PLUGIN_M);
				} catch (Throwable e) {
						XmlEvent.createSimpleLog("", getContext(), CommonLog.LL_ERROR, "Cannot shutdown the event plugin because of: " + e.getMessage() + ", ignoring...");
				}
		}

		private void performStart() throws Exception {
				//this.actualSrvName_m = HtMtConstants.SRV_NAME_M;
				//this.actualAlgLibPairName_m = HtMtConstants.ALGLIB_NAME_M;

				//if (isDebug_m) {
						//this.actualSrvName_m = this.actualSrvName_m + "_d";
						//this.actualAlgLibPairName_m = this.actualAlgLibPairName_m + "_d";
				//}

				if (HtMtManager.getInstance().isInitialized()) {
						throw new HtException(getContext(), "processRequest", "Already initialized");
				}

				// need to store server name
				this.actualSrvName_m = HtHelperUtils.checkSrvCreated(
					HtMtConstants.SRV_NAME_M, 
					HtMtConstants.SRV_NAME_BASE_EXE, 
					settingsFile_m,
					isDebug_m,
					createForce_m
				);
				HtHelperUtils.checkServerIsStarted(this.actualSrvName_m);
				
				// check if alglib pair created
				String algLibPair_resolved = HtHelperUtils.checkAlgLibPairCreated(
					HtMtConstants.ALGLIB_NAME_M, 
					HtMtConstants.ALGLIB_DLL, 
					isDebug_m,
					this.settingsFile_m,
					createForce_m
				);
					
				HtHelperUtils.loadAndResolveAlgLibPair(this.actualSrvName_m, algLibPair_resolved);
				int tid = HtHelperUtils.loadAndResolveAlgorithBrokerThread(this.actualSrvName_m, algLibPair_resolved);

				// need to ask for additional parameters
				
				
				// get metadata
				
				Req_G_GET_ALL_METADATA internalrequestObj = new Req_G_GET_ALL_METADATA();
				Resp_G_GET_ALL_METADATA internalResponseObj  = new Resp_G_GET_ALL_METADATA();
				
				JSONObject custom_data_json = new JSONObject();
				internalrequestObj.toJson(custom_data_json);
		
				String get_minfo_command_data =  HtCommandProcessor.instance().get_HtServerProxy().remote_SendCustomEvent_toAlgorithm(
					this.actualSrvName_m,
					algLibPair_resolved,
					tid,
					internalrequestObj.getMtCommand(),
					JSONValue.toJSONString(custom_data_json), 
					RtConfigurationManager.StartUpConst.WAIT_RESPONSE_FROM_TRADING_SERVER_SEC
				);
				
				internalResponseObj.fromJson((JSONObject)JSONValue.parseWithException(get_minfo_command_data));
					// check internal response status
				HtHelperUtils.checkAlgLibResponse(internalResponseObj);
				
				// check event plugin is created
				String eventPlugin_resolved = HtHelperUtils.checkEventPluginCreated(HtMtConstants.EVENT_PLUGIN_M,createForce_m);

				HtHelperUtils.checkEventPluginStarted(
					eventPlugin_resolved,
					this.actualSrvName_m,
					algLibPair_resolved,
					tid,
					internalResponseObj.getEventPluginParams());
		}

		/**
		 * Helpers
		 */
		
		
}
