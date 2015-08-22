/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.request;

import com.fin.httrader.eventplugins.ht_mt_bridge.HtMtManager;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.HtMtJsonUtils;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibrequest.Req_START_POLLING_USERS;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibresponse.Resp_G_GET_ALL_METADATA;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibresponse.Resp_START_POLLING_USERS;
import com.fin.httrader.managers.RtConfigurationManager;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response.HtBaseIntResp;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response.HtStartPollUsersResp;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.utils.HtHelperUtils;
import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.JSONValue;


/**
 *
 * @author Victor_Zoubok
 */
public class HtStartPollUsersReq extends HtBaseIntReq {
		public static final String COMMAND_NAME = "POLL_USERS_REQ";
		
		private final Req_START_POLLING_USERS internalrequestObj_m = new Req_START_POLLING_USERS();
		private final Resp_START_POLLING_USERS internalResponseObj_m = new Resp_START_POLLING_USERS();
		
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
					// prepare request
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
				
				
				// return back 
				HtStartPollUsersResp resp = new HtStartPollUsersResp(getCommandName());
				resp.getResolvedMtInstance().fromUid(internalResponseObj_m.getMt_instance_uid());
				resp.setResolvedMtInstanceName(internalResponseObj_m.getMt_instance_name());
				
				return resp;
		}
		
		@Override
		public void initialize(JSONObject root_json) throws Exception
		{
				// initialize user data
				
				// mt instane
				String mtInstnace = (String)root_json.get("MT_INSTANCE");
				internalrequestObj_m.setMtInstnaceName(mtInstnace);
				
				
				JSONObject data_json = HtMtJsonUtils.<JSONObject>getJsonField(root_json, "DATA");
				JSONArray user_list_json = HtMtJsonUtils.<JSONArray>getJsonField(data_json, "USER_LIST");
				
					
				for(int i = 0; i < user_list_json.size(); i++) {
						JSONObject user_obj = (JSONObject)user_list_json.get(i);
						
					Req_START_POLLING_USERS.MtUser user = new Req_START_POLLING_USERS.MtUser();
					
					Req_START_POLLING_USERS.MtUser.convertFromJson(user, user_obj);
					if (!user.isValid())
								throw new HtException(getContext(), "initialize", "Invalid user data");
						
					
					internalrequestObj_m.getUsers().add(user);
				}
				
			
				
		}
		
}
