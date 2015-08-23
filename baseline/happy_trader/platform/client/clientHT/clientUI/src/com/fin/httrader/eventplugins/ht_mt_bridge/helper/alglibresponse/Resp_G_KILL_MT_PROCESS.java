/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibresponse;

import com.fin.httrader.eventplugins.ht_mt_bridge.helper.HtMtJsonUtils;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibrequest.AlgLibRequestBase;
import static com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibrequest.Req_G_GET_ALL_METADATA.COMMAND;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtUtils;
import java.util.Iterator;
import org.json.simple.JSONArray;
import org.json.simple.JSONObject;

/**
 *
 * @author Victor_Zoubok
 */
public class Resp_G_KILL_MT_PROCESS extends AlgLibResponseBase {
		
		private String getContext()
		{
				return Resp_G_KILL_MT_PROCESS.class.getSimpleName();
		}
		
		private boolean isKilled_m;

		public boolean isIsKilled() {
				return isKilled_m;
		}

		public void setIsKilled(boolean isKilled) {
				this.isKilled_m = isKilled;
		}
		
		@Override
		public void fromJson(JSONObject result_json) throws Exception
		{
				
				super.fromJson(result_json);
				
				this.isKilled_m = HtMtJsonUtils.<Boolean>getJsonField(result_json, "IS_KILLED");
			
		}
		
}
