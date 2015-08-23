/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.request;

import com.fin.httrader.eventplugins.ht_mt_bridge.HtMtManager;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibrequest.Req_G_GET_ALL_METADATA;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibrequest.Req_START_POLLING_USERS;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibresponse.Resp_G_GET_ALL_METADATA;
import com.fin.httrader.managers.RtConfigurationManager;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response.HtBaseIntResp;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response.HtGetStatusResp;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.utils.HtHelperUtils;
import org.json.simple.JSONObject;
import org.json.simple.JSONValue;

/**
 *
 * @author Victor_Zoubok
 */
public class HtGetStatusReq extends HtBaseIntReq {
		public static final String COMMAND_NAME = "GET_STATUS_REQ";
		
		private final Req_G_GET_ALL_METADATA internalrequestObj_m = new Req_G_GET_ALL_METADATA();
		private final Resp_G_GET_ALL_METADATA internalResponseObj_m  = new Resp_G_GET_ALL_METADATA();
		
		@Override
		public String getCommandName()
		{
				return COMMAND_NAME;
		}
		
		@Override
		public HtBaseIntResp processRequest() throws Exception
		{
				
				
				// need to send this response
				JSONObject custom_data_json = new JSONObject();
				internalrequestObj_m.toJson(custom_data_json);
				
				// object is ready to be sent to next layer
				String result_data = HtMtManager.getInstance().sendCustomEventToHtMtAlgorithm(
					internalrequestObj_m.getMtCommand(), 
					JSONValue.toJSONString(custom_data_json), 
					RtConfigurationManager.StartUpConst.WAIT_RESPONSE_FROM_TRADING_SERVER_SEC
				);
				
				
				// obtain response
				internalResponseObj_m.fromJson((JSONObject)JSONValue.parseWithException(result_data));
				
					// check internal response status
				HtHelperUtils.checkAlgLibResponse(internalResponseObj_m);
				
				// need to parse response
				HtGetStatusResp resp  =new HtGetStatusResp(getCommandName());
				resp.getMetainfo().addAll(  internalResponseObj_m.getData() );
				resp.getEventPluginParams().putAll(internalResponseObj_m.getEventPluginParams() );
				
				return resp;
		}
		
		@Override
		public void initialize(JSONObject root_json) throws Exception
		{
				// nothining to init
		}
}
