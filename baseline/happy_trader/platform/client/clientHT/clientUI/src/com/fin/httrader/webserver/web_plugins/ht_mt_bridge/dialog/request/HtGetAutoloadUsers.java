/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.request;

import com.fin.httrader.eventplugins.ht_mt_bridge.HtMtManager;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.HtMtJsonUtils;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibrequest.Req_CLEAR_ALL_USERS;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibrequest.Req_G_QUERY_AUTOLOAD_USERS;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibresponse.Resp_CLEAR_ALL_USERS;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibresponse.Resp_G_QUERY_AUTOLOAD_USERS;
import com.fin.httrader.managers.RtConfigurationManager;
import static com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.request.HtClearUsersReq.COMMAND_NAME;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response.HtBaseIntResp;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response.HtClearUserResp;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response.HtGetAutoloadUsersResp;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.utils.HtHelperUtils;
import org.json.simple.JSONObject;
import org.json.simple.JSONValue;

/**
 *
 * @author Administrator
 */
public class HtGetAutoloadUsers extends HtBaseIntReq {
	public static final String COMMAND_NAME = "GET_AUTOLOAD_USERS_REQ";
		
		private final Req_G_QUERY_AUTOLOAD_USERS internalrequestObj_m = new Req_G_QUERY_AUTOLOAD_USERS();
		private final Resp_G_QUERY_AUTOLOAD_USERS internalResponseObj_m = new Resp_G_QUERY_AUTOLOAD_USERS();
		
		private String getContext()
		{
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
				HtGetAutoloadUsersResp resp = new HtGetAutoloadUsersResp(getCommandName());
				
				// work
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
				
				resp.getInstances().addAll(internalResponseObj_m.getInstances());
				
				return resp;
		}
		
		@Override
		public void initialize(JSONObject root_json) throws Exception
		{
			
		}
}
