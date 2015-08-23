/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.request;

import com.fin.httrader.eventplugins.ht_mt_bridge.HtMtManager;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.HtMtConstants;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response.HtBaseIntResp;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response.HtStopInstanceResp;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.utils.HtHelperUtils;
import org.json.simple.JSONObject;

/**
 *
 * stop instance
 */
public class HtStopInstanceReq extends HtBaseIntReq {
		public static final String COMMAND_NAME = "STOP_INSTANCE_REQ";
		
		private  String getContext() {
				return this.getClass().getSimpleName();
		}
		
		@Override
		public String getCommandName()
		{
				return COMMAND_NAME;
		}
		
		@Override
		public HtBaseIntResp processRequest() throws Exception
		{
				HtStopInstanceResp resp = new HtStopInstanceResp(getCommandName());
				
						
				try {
						if (!HtMtManager.getInstance().isInitialized())
								throw new HtException(getContext(), "processRequest", "Cannot resolve actual server name because event plugin is already stopped: " + HtMtConstants.EVENT_PLUGIN_M);
						
						HtHelperUtils.stopServer(HtMtManager.getInstance().getInitParams().getCurrentServerName());
				}
				catch(Throwable e)
				{
						XmlEvent.createSimpleLog("", getContext(), CommonLog.LL_ERROR, "Cannot shutdown the main server because of: " + e.getMessage() + ", ignoring...");
				}
				
				try {
						HtHelperUtils.checkEventPluginStopped(HtMtConstants.EVENT_PLUGIN_M);
				}
				catch(Throwable e)
				{
						XmlEvent.createSimpleLog("", getContext(), CommonLog.LL_ERROR, "Cannot shutdown the event plugin because of: " + e.getMessage() + ", ignoring...");
				}
				
				
				
				return resp;
				
		}
		
		@Override
		public void initialize(JSONObject root_json) throws Exception
		{
		}
		
		/**
		 * Helpers
		 */
		
		
		
}
