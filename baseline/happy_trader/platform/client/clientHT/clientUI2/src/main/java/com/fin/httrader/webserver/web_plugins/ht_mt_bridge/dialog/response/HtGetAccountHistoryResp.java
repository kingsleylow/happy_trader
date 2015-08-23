/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response;

import com.fin.httrader.eventplugins.ht_mt_bridge.delivery.HtMtEventWrapperAccountHistory;
import com.fin.httrader.eventplugins.ht_mt_bridge.delivery.HtMtEventWrapperPositionsHistory;
import org.json.simple.JSONObject;

/**
 *
 * @author Victor_Zoubok
 */
public class HtGetAccountHistoryResp extends HtBaseIntResp {
		
		public HtGetAccountHistoryResp(String requestName)
		{
				super(requestName);
		};
		
		private  HtMtEventWrapperAccountHistory accHistory_m = null;
		
		public HtMtEventWrapperAccountHistory getAccHistoryObject()
		{
				return accHistory_m;
		}
		
		public void setAccHistoryObject(HtMtEventWrapperAccountHistory posHistObj)
		{
			
				this.accHistory_m = posHistObj;
		}
		
		@Override
		public JSONObject toJson() throws Exception
		{
				JSONObject json_upper_object = toJsonBase();
				JSONObject data_json = new JSONObject();
				
				if (this.accHistory_m != null) {
						this.accHistory_m.toJson(data_json);
				}
				
				json_upper_object.put("DATA", data_json);
				
				return json_upper_object;
		}
}
