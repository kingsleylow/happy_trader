/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response;

import com.fin.httrader.utils.Uid;
import org.json.simple.JSONObject;

/**
 *
 * @author Victor_Zoubok
 */
public class HtStartPollUsersResp extends HtBaseIntResp {
		public HtStartPollUsersResp(String requestName)
		{
				super(requestName);
		};
		
		// resolved instnace
		private final Uid resolvedMtInstance_m= new Uid();
		private String resolvedMtInstanceName_m = null;

		public Uid getResolvedMtInstance() {
				return resolvedMtInstance_m;
		}

		
		
		@Override
		public JSONObject toJson() throws Exception
		{
				JSONObject json_upper_object = super.toJsonBase();
				
				json_upper_object.put("MT_INSTANCE_UID", resolvedMtInstance_m.toString());
				json_upper_object.put("MT_INSTANCE_NAME", resolvedMtInstanceName_m);
				
				return json_upper_object;
		}

		public String getResolvedMtInstanceName() {
				return resolvedMtInstanceName_m;
		}

		public void setResolvedMtInstanceName(String resolvedMtInstanceName) {
				this.resolvedMtInstanceName_m = resolvedMtInstanceName;
		}
		
}
