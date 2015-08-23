/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response;

import org.json.simple.JSONObject;

/**
 *
 * base class for encapuslating responses
 */
public class HtBaseIntResp {
		
		String requestname_m = null;
		
		public HtBaseIntResp(String requestName)
		{
				requestname_m = requestName;
		};
		
		public JSONObject toJsonBase() throws Exception
		{
				JSONObject json_upper_object = new JSONObject();
				json_upper_object.put("REQUEST_NAME", requestname_m );
				
				return json_upper_object;
		}
		
		public JSONObject toJson() throws Exception
		{
				JSONObject json_upper_object = toJsonBase();
				return json_upper_object;
		}
}
