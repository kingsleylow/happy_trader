/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibout;

import com.fin.httrader.utils.HtException;
import org.json.simple.JSONObject;

/**
 *
 * @author Victor_Zoubok
 * This is base class to get packets from alg lib layer
 */
public abstract class OutPacketBase {
		
    abstract public String getCommand();
		
		private String getContext()
		{
				return OutPacketBase.class.getSimpleName();
		}
		
		public void fromJson(JSONObject result_json) throws Exception
		{
				if (result_json == null) {
						throw new HtException(getContext(), "fromJson", "Invalid JSON data");
				}
			

		}
		
		public void toJson(JSONObject json_root_obj) throws Exception
		{
				String cmd = getCommand();
				json_root_obj.put("TYPE", cmd != null ? cmd : "");
		}
}
