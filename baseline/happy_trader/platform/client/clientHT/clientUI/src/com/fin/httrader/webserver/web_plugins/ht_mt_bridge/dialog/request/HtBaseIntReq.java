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
 * this is a base class
 * to organise requests from client - to happy trader
 */
public abstract class HtBaseIntReq {
		
	  public abstract String getCommandName();
		
		// process request
		public abstract HtBaseIntResp processRequest() throws Exception;
		
		// initialize from JSON string
		public abstract void initialize(JSONObject root_json) throws Exception;
		
		// some stuff if necessary
		// on error happens
		public void onErrorHappened()
		{
		}
			
		public void setSimpleResponseFlag(boolean simpleResponse)
		{
			this.isSimpleresponse_m = simpleResponse;
		}
		
		public boolean getSimpleResponseFlag()
		{
			return this.isSimpleresponse_m;
		}
		
		private boolean isSimpleresponse_m = false;
		
		
}
