/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibrequest;

import static com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibrequest.Req_START_POLLING_USERS.COMMAND;
import java.util.ArrayList;
import java.util.List;
import org.json.simple.JSONArray;
import org.json.simple.JSONObject;

/**
 *
 * update access time for users
 * for MT
 */
public class Req_UPDATE_USER_ACCESS_TIME extends AlgLibRequestBase {
		public static final String COMMAND = "UPDATE_USER_ACCESS_TIME";
		
		private final List<String> users_m = new ArrayList<String>();
		
		public Req_UPDATE_USER_ACCESS_TIME()
		{
			
		}
		
		public List<String> getUsers()
		{
				return users_m;
		}
		
		@Override
		public String getMtCommand()
		{
				return COMMAND;
		}
		
		@Override
		public void toJson(JSONObject json_root_obj) throws Exception
		{
				super.toJson(json_root_obj);
				
				
				// 
					
				JSONObject data_obj = new JSONObject();
				JSONArray user_list_obj = new JSONArray();
				
				
				for(int i = 0; i < users_m.size(); i++) {
					String user_i = users_m.get(i);
					
					JSONObject user_obj = new JSONObject();
					user_obj.put("NAME", user_i);
					
					user_list_obj.add( user_obj );
					
				}
				
				data_obj.put("USER_LIST", user_list_obj);
				json_root_obj.put("DATA", data_obj);
		}
}
