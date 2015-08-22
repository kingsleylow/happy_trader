/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibout;

import com.fin.httrader.eventplugins.ht_mt_bridge.helper.HtMtJsonUtils;
import static com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibout.HeartBeatOut.COMMAND;
import org.json.simple.JSONObject;

/**
 *
 * @author Victor_Zoubok
 */
public class MtStatusOut extends OutPacketBase{
		public static final String COMMAND = "MT_STATUS";
		
		public static final int MT_CONNECTED = 1;
		public static final int MT_DISCONNECTED = 2;
		
		private int status_m;
		private String mtInstance_m;
		
		public MtStatusOut()
		{
		}
		
		private String getContext()
		{
				return MtStatusOut.class.getSimpleName();
		}
		
		public String getCommand()
		{
				return COMMAND;
		}
		
		@Override
		public void fromJson(JSONObject result_json) throws Exception
		{
				super.fromJson(result_json);
				
				
				setMtInstance(HtMtJsonUtils.<String>getJsonField(result_json, "MT_INSTANCE"));
				setStatus(HtMtJsonUtils.getIntJsonField(result_json, "STATUS"));
		}
		
		@Override
		public void toJson(JSONObject json_root_obj) throws Exception
		{
				super.toJson(json_root_obj);
				
			
				
				json_root_obj.put("MT_INSTANCE", getMtInstance());
				json_root_obj.put("STATUS", getStatus());
				
				
		}

		public int getStatus() {
				return status_m;
		}

		public void setStatus(int status) {
				this.status_m = status;
		}

		public String getMtInstance() {
				return mtInstance_m;
		}

		public void setMtInstance(String mtInstance) {
				this.mtInstance_m = mtInstance;
		}

};