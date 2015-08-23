/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response;

import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibresponse.Resp_G_GET_ALL_METADATA;
import com.fin.httrader.utils.HtUtils;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import org.json.simple.JSONArray;
import org.json.simple.JSONObject;

/**
 *
 * @author Victor_Zoubok
 */
public class HtGetStatusResp extends HtBaseIntResp {
		
		public HtGetStatusResp(String requestName)
		{
				super(requestName);
		};
		
		private final List<Resp_G_GET_ALL_METADATA.MtEntry> metainfo_m = new ArrayList<Resp_G_GET_ALL_METADATA.MtEntry>();
		private final Map<String, String> eventPluginParams_m = new HashMap<String, String>();
		
		public List<Resp_G_GET_ALL_METADATA.MtEntry> getMetainfo()
		{
				return metainfo_m;
		}
		
		public Map<String, String> getEventPluginParams()
		{
				return eventPluginParams_m;
		}
		
		@Override
		public JSONObject toJson() throws Exception
		{
				JSONObject json_upper_object = toJsonBase();
				JSONArray entries_json = new JSONArray();
				JSONArray event_plugin_json = new JSONArray();
				
				
				for(Resp_G_GET_ALL_METADATA.MtEntry e: metainfo_m) {
						JSONObject entryElem = new JSONObject();
						Resp_G_GET_ALL_METADATA.MtEntry.convertToJson(e, entryElem );
						
						entries_json.add(entryElem);
				}
				
				for (String name : eventPluginParams_m.keySet()) {
						String value = eventPluginParams_m.get( name );
						
						JSONObject ev_json_elem = new JSONObject();
						
						if (!HtUtils.nvl(name)) {
								ev_json_elem.put(name, value);
								event_plugin_json.add(ev_json_elem);
						}
						
				}
				
				json_upper_object.put("METAINFO_DATA", entries_json);
				json_upper_object.put("EVENT_PLUGIN_DATA", event_plugin_json);
				
				
				return json_upper_object;
		}
		
}
