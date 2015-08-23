/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.eventplugins.ht_mt_bridge.helper.mt;

import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.Uid;
import org.json.simple.JSONObject;

/**
 *
 * @author Victor_Zoubok
 */
public abstract class MTProxyPacketBase {
		
		private Uid mtUid_m = new Uid();
		
		public MTProxyPacketBase()
		{
		}
		
		private String getContext()
		{
				return MTProxyPacketBase.class.getSimpleName();
		}
		
		abstract public String getCommand();
		 
		public Uid getMtUid()
		{
				return mtUid_m;
		}
		
		public void fromJson(JSONObject result_json) throws Exception
		{
				if (result_json == null) {
						throw new HtException(getContext(), "fromJson", "Invalid JSON data");
				}
				
				// skip command UID
				getMtUid().fromString( (String)result_json.get("MT_UID") );

		}
		
		public void toJson(JSONObject json_root_obj) throws Exception
		{
				String mt_inst_name = getCommand();
				json_root_obj.put("COMMAND", mt_inst_name != null ? mt_inst_name : "");
				json_root_obj.put("MT_UID", getMtUid().toString());
		}
}
