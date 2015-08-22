/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibout;

import com.fin.httrader.eventplugins.ht_mt_bridge.helper.mt.Mtproxy_Data;
import com.fin.httrader.utils.HtException;
import org.json.simple.JSONObject;

/**
 *
 * @author Victor_Zoubok
 */
public class MtDataPacketOut extends OutPacketBase{
		
		public static final String COMMAND = "FULL_MT_PACKET";
		
		private final Mtproxy_Data mtData_m = new Mtproxy_Data();
		
			
		public MtDataPacketOut()
		{
		};
		
		public Mtproxy_Data getMtData()
		{
				return mtData_m;
		}
		
		private String getContext()
		{
				return MtDataPacketOut.class.getSimpleName();
		}
		
		public String getCommand()
		{
				return COMMAND;
		}
		
		@Override
		public void fromJson(JSONObject result_json) throws Exception
		{
				super.fromJson(result_json);
				
				
				JSONObject mtdata_json = (JSONObject)result_json.get("EVENT_DATA");
				getMtData().fromJson(mtdata_json);

		}
		
		@Override
		public void toJson(JSONObject json_root_obj) throws Exception
		{
				super.toJson(json_root_obj);
				
				JSONObject mtdata_json = new JSONObject();
				getMtData().toJson(mtdata_json);
				
				json_root_obj.put("EVENT_DATA", mtdata_json);
		}
		
		
}
