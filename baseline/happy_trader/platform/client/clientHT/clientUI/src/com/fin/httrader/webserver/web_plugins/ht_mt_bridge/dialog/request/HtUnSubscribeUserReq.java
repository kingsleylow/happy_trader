/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.request;

import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response.HtBaseIntResp;
import org.json.simple.JSONObject;

/**
 *
 * @author Victor_Zoubok
 */
public class HtUnSubscribeUserReq extends HtBaseIntReq {
		public static final String COMMAND_NAME = "UNSUBSCRIBE_USER_REQ";
		
		@Override
		public String getCommandName()
		{
				return COMMAND_NAME;
		}
		
		@Override
		public HtBaseIntResp processRequest() throws Exception
		{
				return null;
		}
		
		@Override
		public void initialize(JSONObject root_json) throws Exception
		{
				
		}
}
