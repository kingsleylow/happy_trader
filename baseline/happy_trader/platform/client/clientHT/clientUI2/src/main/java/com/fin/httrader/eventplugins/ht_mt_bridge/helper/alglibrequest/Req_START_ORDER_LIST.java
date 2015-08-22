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
 * @author Victor_Zoubok
 * // special structure
 */
public class Req_START_ORDER_LIST extends AlgLibRequestBase {
		public static final String COMMAND = "START_ORDER_LIST";
		
		private final List<String> orderList_m = new ArrayList<String>();
		
		
		public List<String> getOrderList()
		{
				return orderList_m;
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
				JSONArray data_list_obj = new JSONArray();
			
				for(int i = 0; i < orderList_m.size(); i++) {
					
					JSONObject data_i = new JSONObject();
					data_i.put("ORDER_DATA", orderList_m.get(i));
					
					data_list_obj.add( data_i );
					
				}
				
				data_obj.put("ORDER_LIST", data_list_obj);
				json_root_obj.put("DATA", data_obj);
		}
		
}
