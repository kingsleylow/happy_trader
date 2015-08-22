/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response;

import com.fin.httrader.eventplugins.ht_mt_bridge.delivery.HtMtEventWrapperPositionsHistory;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibresponse.Resp_G_GET_ALL_METADATA;
import org.json.simple.JSONArray;
import org.json.simple.JSONObject;

/**
 *
 * @author Victor_Zoubok
 */
public class HtGetPositionHistoryResp extends HtBaseIntResp {
		
		public HtGetPositionHistoryResp(String requestName)
		{
				super(requestName);
		};
		
		private  HtMtEventWrapperPositionsHistory posHistory_m = null;
		
		public HtMtEventWrapperPositionsHistory getPosHistoryObject()
		{
				return posHistory_m;
		}
		
		public void setPosHistoryObject(HtMtEventWrapperPositionsHistory posHistObj)
		{
				this.posHistory_m = posHistObj;
		}
		
		@Override
		public JSONObject toJson() throws Exception
		{
				JSONObject json_upper_object = toJsonBase();
				JSONObject data_json = new JSONObject();
				
				if (this.posHistory_m != null) {
						this.posHistory_m.toJson(data_json);
				}
				
				json_upper_object.put("DATA", data_json);
				
				return json_upper_object;
		}
		
}
