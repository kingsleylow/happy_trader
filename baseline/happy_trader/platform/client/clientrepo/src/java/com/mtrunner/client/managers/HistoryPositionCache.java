/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.mtrunner.client.managers;

import com.fin.httrader.eventplugins.ht_mt_bridge.delivery.HtMtEventWrapperPositionsHistory;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;



public class HistoryPositionCache {
	
	public static class PositionCacheEntry {
		public String companyName;
		public String accountId;
		public String serverName;
		public long lastRequest = -1;
		public long maxCloseTime=-1;
		public List<HtMtEventWrapperPositionsHistory.PositionEntry> posList_m;
		
		public void calculateLastCloseTime()
		{
			Comparator<HtMtEventWrapperPositionsHistory.PositionEntry> cmp = new Comparator<HtMtEventWrapperPositionsHistory.PositionEntry>() {
        @Override
        public int compare(HtMtEventWrapperPositionsHistory.PositionEntry o1, HtMtEventWrapperPositionsHistory.PositionEntry o2) {
           Long o1l= o1.getHtMtEventWrapperPositionChange().getCloseTime();
					 Long ol2 =o2.getHtMtEventWrapperPositionChange().getCloseTime();
					 
					 return o1l.compareTo(ol2);
        }
			};
			
			if (posList_m.size() > 0) {
				HtMtEventWrapperPositionsHistory.PositionEntry me = Collections.max(posList_m,cmp);
				maxCloseTime = me.getHtMtEventWrapperPositionChange().getCloseTime();
			}
		}
		
	}
	
	private Map<String, PositionCacheEntry> cache_m = new HashMap();
	static final private HistoryPositionCache instance_m = new HistoryPositionCache();
	
	public static HistoryPositionCache getInstance()
	{
		return instance_m;
	}

	public Map<String, PositionCacheEntry> getPositionHistoryCache()
	{
		return cache_m;
	}

}
