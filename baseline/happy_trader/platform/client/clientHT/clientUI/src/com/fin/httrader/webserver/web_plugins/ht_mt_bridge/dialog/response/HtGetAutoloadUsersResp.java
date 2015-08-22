/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response;

import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibresponse.Resp_G_GET_ALL_METADATA;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibresponse.Resp_G_QUERY_AUTOLOAD_USERS;
import java.util.ArrayList;
import java.util.List;
import org.json.simple.JSONArray;
import org.json.simple.JSONObject;

/**
 *
 * @author Administrator
 */
public class HtGetAutoloadUsersResp extends HtBaseIntResp {
	
	private final List<Resp_G_QUERY_AUTOLOAD_USERS.MtInstance> instances_m = new ArrayList<Resp_G_QUERY_AUTOLOAD_USERS.MtInstance>();
	
	public List<Resp_G_QUERY_AUTOLOAD_USERS.MtInstance> getInstances() {
		return instances_m;
	}
	
	public HtGetAutoloadUsersResp(String requestName)
	{
			super(requestName);
	};
	
	@Override
		public JSONObject toJson() throws Exception
		{
				JSONObject json_upper_object = toJsonBase();
				JSONArray json_inst_array = new JSONArray();
				
				
				for(int i = 0; i < instances_m.size(); i++) {
					JSONObject inst_json_obj = new JSONObject();
					Resp_G_QUERY_AUTOLOAD_USERS.MtInstance inst = instances_m.get(i);
					Resp_G_QUERY_AUTOLOAD_USERS.MtInstance.convertToJson(inst, inst_json_obj);
					
					json_inst_array.add(inst_json_obj);
				}
				
				json_upper_object.put("DATA", json_inst_array);
				
				return json_upper_object;
		}
	
}
