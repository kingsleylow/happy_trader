/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response;

import com.fin.httrader.eventplugins.ht_mt_bridge.delivery.HtMtEventWrapperBase;
import java.util.ArrayList;
import java.util.List;
import org.json.simple.JSONArray;
import org.json.simple.JSONObject;

/**
 *
 * @author Victor_Zoubok
 */
public class HtSubscribeUserResp extends HtBaseIntResp {
		
		// it does need to have some structure wrapping
		// either single chnage
		// or history plus chnage struct
		// or heart beat
		
		// this is the list
		private List<HtMtEventWrapperBase> dataHistoryList_m = null;
		
		// this is the last event to wrap
		HtMtEventWrapperBase dataEvent_m = null;
		private int lastCookieId_m = -1;
		private int pendingSize_m = -1;
		private long sequenceId_m = -1;
		private String mtInstance_m = null;
		
		
		public HtSubscribeUserResp(String requestName)
		{
				super(requestName);
		};
		
		public String getMtInstnace() {
					return mtInstance_m;
		}
		
		public void setMtInstnace( String mtInstance) {
					mtInstance_m = mtInstance;
		}
		
		public long getSequenceId() {
					return sequenceId_m;
		}
		
		public void setSequenceId( long seqId) {
					sequenceId_m = seqId;
		}
		
		public List<HtMtEventWrapperBase> getDataHistoryList() {
					return dataHistoryList_m;
		}
		
		public void setDataHistoryList( List<HtMtEventWrapperBase> historyList) {
					dataHistoryList_m  = historyList;
		}
		
		public HtMtEventWrapperBase getLastData() {
					return dataEvent_m;
		}
		
		public void setLastData( HtMtEventWrapperBase lastdata) {
					dataEvent_m  = lastdata;
		}
		
		public int getLastCookieId() {
				return lastCookieId_m;
		}

		public void setLastCookieId(int lastCookieId) {
				this.lastCookieId_m = lastCookieId;
		}

		public int getPendingSize() {
				return pendingSize_m;
		}

		public void setPendingSize(int pendingSize) {
				this.pendingSize_m = pendingSize;
		}
		
		@Override
		public JSONObject toJson() throws Exception
		{
				JSONObject json_upper_object = toJsonBase();
				JSONArray data_json_arr = new JSONArray();
				
				if (dataHistoryList_m != null) {
						for(int i = 0; i < dataHistoryList_m.size(); i++) {
								HtMtEventWrapperBase delivery_i = dataHistoryList_m.get(i);
								
								JSONObject delivery_i_json = new JSONObject();
								delivery_i.toJson(delivery_i_json);
								
								data_json_arr.add( delivery_i_json );
						}
				}
				
				json_upper_object.put("HISTORY_DATA", data_json_arr);
				
				JSONObject json_object_curdata = new JSONObject();
				if (dataEvent_m != null) {
						dataEvent_m.toJson(json_object_curdata);
				}
				json_upper_object.put("LIVE_DATA", json_object_curdata);
				json_upper_object.put("COOKIE_ID", this.lastCookieId_m); // returns cookie to access same queue the next time
				json_upper_object.put("PENDING_SIZE", this.pendingSize_m); // just pending size of the queue
				json_upper_object.put("SEQUENCE_ID", this.sequenceId_m); // XML event sequence ID
				json_upper_object.put("MT_INSTANCE", this.mtInstance_m);
				
				return json_upper_object;
		}

		
}
