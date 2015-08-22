/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibresponse;

import com.fin.httrader.eventplugins.ht_mt_bridge.helper.HtMtJsonUtils;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.Uid;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import org.json.simple.JSONArray;
import org.json.simple.JSONObject;

/**
 *
 * @author Victor_Zoubok this are helper structures to wrap data obtained from
 * AlgLib layer // in the form of responses
 */
public class Resp_G_GET_ALL_METADATA extends AlgLibResponseBase {

		private String getContext()
		{
				return Resp_G_GET_ALL_METADATA.class.getSimpleName();
		}
		
		private final List<MtEntry> data_m = new ArrayList<MtEntry>();
		private final Map<String, String> eventPluginParams_m = new HashMap<String, String>();
		

		public List<MtEntry> getData() {
				return data_m;
		}
		
		public Map<String, String> getEventPluginParams() {
				return eventPluginParams_m;
		}
		
		public static class MtEntry {

				public MtEntry() {
				}
				
				
				private boolean connected_m = false;
				private String name_m;
				private long lastHeartBeat_m  =-1;
				private boolean loadBalanced_m = false;
				final private Uid terminalUid_m = new Uid();
				private int pendingSize_m = -1;
				private long lastRestartTime_m = -1;
				private int restartCount_m = -1;
				private int pid_m = -1;
				

				public boolean isConnected() {
						return connected_m;
				}

				public void setConnected(boolean connected_m) {
						this.connected_m = connected_m;
				}

				public String getName() {
						return name_m;
				}

				public void setName(String name_m) {
						this.name_m = name_m;
				}

				public long getLastHeartBeat() {
						return lastHeartBeat_m;
				}

				public void setLastHeartBeat(long lastHeartBeat_m) {
						this.lastHeartBeat_m = lastHeartBeat_m;
				}

				public boolean isLoadBalanced() {
						return loadBalanced_m;
				}

				public void setLoadBalanced(boolean loadBalanced_m) {
						this.loadBalanced_m = loadBalanced_m;
				}

				public Uid getTerminalUid() {
						return terminalUid_m;
				}
				
				public int getPendingSize() {
						return this.pendingSize_m;
				}

				public void setPendingSize(int pendingSize) {
						pendingSize_m = pendingSize;
				}
				
				public long getLastRestartTime() {
						return lastRestartTime_m;
				}

				public void setLastRestartTime(long lastRestartTime) {
						this.lastRestartTime_m = lastRestartTime;
				}

				public int getRestartCount() {
						return restartCount_m;
				}

				public void setRestartCount(int restartCount) {
						this.restartCount_m = restartCount;
				}

				public int getPid() {
						return pid_m;
				}

				public void setPid(int pid) {
						this.pid_m = pid;
				}


				
				// helpers
				public static void convertToJson(MtEntry e,JSONObject je) throws Exception
				{
						je.put("CONNECTED",  Boolean.valueOf( e.isConnected()));
						je.put("NAME", e.getName());
						je.put("LOAD_BALANCED", Boolean.valueOf( e.isLoadBalanced()));
						je.put("TERMINAL_UID", e.getTerminalUid().toString());
						je.put("LAST_HEARTBEAT",  e.getLastHeartBeat() );
						je.put("PENDING_SIZE",  e.getPendingSize());
						
						je.put("LAST_RESTART_TIME", e.getLastRestartTime());
						je.put("RESTART_COUNT", e.getRestartCount());
						je.put("PID", e.getPid());
				}
				
				public static void convertFromJson(MtEntry e, JSONObject je) throws Exception
				{
						e.setConnected( (Boolean)je.get("CONNECTED"));
						e.setName( (String)je.get("NAME"));
						e.setLoadBalanced( (Boolean)je.get("LOAD_BALANCED") );
						e.setLastHeartBeat(  HtDateTimeUtils.convertUinxSecToUnixMsec( ((Number)je.get("LAST_HEARTBEAT")).longValue()) );
						e.getTerminalUid().fromString((String)je.get("TERMINAL_UID"));
						e.setPendingSize(  HtMtJsonUtils.getIntJsonField(je, "PENDING_SIZE"));
						
						e.setLastRestartTime(HtDateTimeUtils.convertUinxSecToUnixMsec( ((Number)je.get("LAST_RESTART_TIME")).longValue()) );
						e.setRestartCount(HtMtJsonUtils.getIntJsonField(je, "RESTART_COUNT"));
						e.setPid(HtMtJsonUtils.getIntJsonField(je, "PID"));
						
				}

				
				
		};

		public Resp_G_GET_ALL_METADATA() {
		}
		
		// 
		@Override
		public void fromJson(JSONObject result_json) throws Exception
		{
				
				super.fromJson(result_json);
				data_m.clear();
				eventPluginParams_m.clear();
				
				JSONObject mt_data = (JSONObject)result_json.get("DATA");
				if (mt_data == null)
						throw new HtException(getContext(), "fromJson", "Invalid JSON data:  <DATA>");
				
				JSONArray mtinst_arr_json =  HtMtJsonUtils.<JSONArray>getJsonField(mt_data, "MT_INSTANCES");
				JSONArray evplug_arr_json =  HtMtJsonUtils.<JSONArray>getJsonField(mt_data, "EVENT_PLUGIN_PARAMETERS");
				
				for(int i = 0; i < evplug_arr_json.size(); i++) {
						JSONObject ev_i = (JSONObject)evplug_arr_json.get(i);
						
						// shouls have a single key
						Iterator<String> it_key = ev_i.keySet().iterator();
						if (it_key.hasNext()) {
						
								String name = it_key.next();
								if (!HtUtils.nvl(name)) {
										ev_i.get(name);
										
										eventPluginParams_m.put(name, (String)ev_i.get(name));		
								}
						}
						
										
				}
				
				for(int i = 0; i < mtinst_arr_json.size(); i++) {
						JSONObject mt_i = (JSONObject)mtinst_arr_json.get(i);
						
						MtEntry e= new MtEntry();
						MtEntry.convertFromJson(e, mt_i);
					
						data_m.add(e);
				}
		}

}
