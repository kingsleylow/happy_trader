/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.request;

import com.fin.httrader.eventplugins.ht_mt_bridge.HtMtManager;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.HtMtJsonUtils;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibrequest.Req_CLEAR_ALL_USERS;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibrequest.Req_G_KILL_MT_PROCESS;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibresponse.Resp_CLEAR_ALL_USERS;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibresponse.Resp_G_KILL_MT_PROCESS;
import com.fin.httrader.managers.RtConfigurationManager;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtUtils;
import static com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.request.HtStartInstanceReq.COMMAND_NAME;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response.HtBaseIntResp;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response.HtClearUserResp;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response.HtRestartMtResp;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.utils.HtHelperUtils;
import org.json.simple.JSONObject;
import org.json.simple.JSONValue;

/**
 *
 * @author Victor_Zoubok
 */
public class HtRestartMtReq extends HtBaseIntReq {
		public static final String COMMAND_NAME = "RESTART_MT";
		
		private final Req_G_KILL_MT_PROCESS internalrequestObj_m = new Req_G_KILL_MT_PROCESS();
		private final Resp_G_KILL_MT_PROCESS internalResponseObj_m = new Resp_G_KILL_MT_PROCESS();
		
		private  String getContext() {
				return this.getClass().getSimpleName();
		}
		
		@Override
		public HtBaseIntResp processRequest() throws Exception
		{
				HtRestartMtResp resp = new HtRestartMtResp(getCommandName());
				
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
				
				
				return resp;
		}
		
		@Override
		public void initialize(JSONObject root_json) throws Exception
		{
				// mt instnace
				String mtInstnace = HtMtJsonUtils.<String>getJsonField(root_json, "MT_INSTANCE");
				
				internalrequestObj_m.setMtInstnaceName(mtInstnace);
		}

		@Override
		public String getCommandName() {
				return COMMAND_NAME;
		}
}
