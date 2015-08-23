/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response;

import org.json.simple.JSONObject;

/**
 *
 * @author Victor_Zoubok
 */
public class HtRestartMtResp extends HtBaseIntResp {
		public HtRestartMtResp(String requestName)
		{
				super(requestName);
		};
		
		
		private boolean isKilled_m;

		public boolean isIsKilled() {
				return isKilled_m;
		}

		public void setIsKilled(boolean isKilled) {
				this.isKilled_m = isKilled;
		}
		
		@Override
		public JSONObject toJson() throws Exception
		{
				JSONObject json_upper_object = super.toJsonBase();
				json_upper_object.put("IS_KILLED", isIsKilled());
				return json_upper_object;
		}
}
