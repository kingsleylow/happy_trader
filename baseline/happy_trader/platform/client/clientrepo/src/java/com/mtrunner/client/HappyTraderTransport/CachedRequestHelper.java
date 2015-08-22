/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.mtrunner.client.HappyTraderTransport;

// this is intended to cache some data

import com.fin.httrader.eventplugins.ht_mt_bridge.delivery.HtMtEventWrapperPositionsHistory;
import com.fin.httrader.eventplugins.ht_mt_bridge.persistent.MainPersistManager;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.LongParam;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.request.HtGetPositionHistoryReq;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response.HtGetPositionHistoryResp;
import com.mtrunner.client.managers.HistoryPositionCache;
import com.mtrunner.client.managers.HistoryPositionCache.PositionCacheEntry;
import com.mtrunner.client.managers.SettingsManager;
import com.mtrunner.client.userlist.controller.ServerControler;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import org.apache.log4j.Logger;
import org.springframework.beans.factory.annotation.Autowired;

public class CachedRequestHelper extends RequestHelper{
	
	static Logger log_m = Logger.getLogger(CachedRequestHelper.class.getName());
	
	public CachedRequestHelper()
	{
		super();
	
	}
	
	// request only open positions - no cache
	public HtGetPositionHistoryResp positionHistoryRequestOpenPositions(
					String companyName,
					String accId,
					String serverName
	) throws Exception
	{
		
		
		String result =  makeRequestToHappyTraderServlet(	getPositionHistoryRequest(companyName, accId, serverName, MainPersistManager.QUERY_POSITION_ONLY_OPEN, -1, -1));
			HtGetPositionHistoryResp r_all = parsePositionHistoryRequest(result);
			
			log_m.debug("positionHistoryRequestOpenPositions(..) - company name: " + companyName + ", account ID: " + accId
				+ ", returned size: " + r_all.getPosHistoryObject().getPositions().size());
			
			return r_all;
	}
	
	// request open positiosn - paging
	public HtGetPositionHistoryResp positionHistoryRequestOpenPositions(
					String companyName,
					String accId,
					String serverName,
					int pagenum,
					int pageSize
	) throws Exception
	{
		HtGetPositionHistoryResp r = positionHistoryRequestOpenPositions(companyName,accId, serverName);
		
		if (pagenum <=0)
			throw new Exception("Invalid page number");
		
		if (pageSize <=0)
			pageSize = SettingsManager.getInstance().getHistoryPositionPageSize();
		
		int curidx_f = (pagenum-1)*pageSize;
		int curidx_l = pagenum * pageSize -1;
		
		HtGetPositionHistoryResp r_all = new HtGetPositionHistoryResp(HtGetPositionHistoryReq.COMMAND_NAME);
		r_all.setPosHistoryObject(new HtMtEventWrapperPositionsHistory());
			
		r_all.getPosHistoryObject().setPositionChangesProvided(r.getPosHistoryObject().getPositionChangesProvided());
		r_all.getPosHistoryObject().setAccountId(accId);
		r_all.getPosHistoryObject().setCompanyName(companyName);
		
		
		for(int i = 0; i < r.getPosHistoryObject().getPositions().size(); i++) {
					HtMtEventWrapperPositionsHistory.PositionEntry e=  r.getPosHistoryObject().getPositions().get(i);
					if (i >= curidx_f && i <= curidx_l)
						r_all.getPosHistoryObject().getPositions().add(e);
				
		}
		
		log_m.debug("positionHistoryRequestOpenPositions(..) - company name: " + companyName + ", account ID: " + accId+ ", server: " 
						+ serverName+", page: " +	pagenum+ ", returned size: " + r_all.getPosHistoryObject().getPositions().size());
		
		return r_all;
	}
					
	
	
	// request history - caching 
	// page num
	public HtGetPositionHistoryResp positionHistoryRequestHistoryPositions(
					String companyName,
					String accId,
					String serverName,
					int pagenum,
					int pageSize
	) throws Exception
	{
		
		// no caching
		HtGetPositionHistoryResp r = positionHistoryRequestHistoryPositions(companyName,accId, serverName);
		
		if (pagenum <=0)
			throw new Exception("Invalid page number");
		
		if (pageSize <=0)
			pageSize = SettingsManager.getInstance().getHistoryPositionPageSize();
		
		int curidx_f = (pagenum-1)*pageSize;
		int curidx_l = pagenum * pageSize -1;
		
		HtGetPositionHistoryResp r_all = new HtGetPositionHistoryResp(HtGetPositionHistoryReq.COMMAND_NAME);
		r_all.setPosHistoryObject(new HtMtEventWrapperPositionsHistory());
			
		r_all.getPosHistoryObject().setPositionChangesProvided(r.getPosHistoryObject().getPositionChangesProvided());
		r_all.getPosHistoryObject().setAccountId(accId);
		r_all.getPosHistoryObject().setCompanyName(companyName);
		
		
		for(int i = 0; i < r.getPosHistoryObject().getPositions().size(); i++) {
					HtMtEventWrapperPositionsHistory.PositionEntry e=  r.getPosHistoryObject().getPositions().get(i);
					if (i >= curidx_f && i <= curidx_l)
						r_all.getPosHistoryObject().getPositions().add(e);
				
		}
		
		log_m.debug("positionHistoryRequestHistoryPositions(..) - company name: " + companyName + ", account ID: " + accId+ ", page: " +
			pagenum+ ", returned size: " + r_all.getPosHistoryObject().getPositions().size());
		
		return r_all;
	}
						
	
	// request history - caching
	// no page num
	public HtGetPositionHistoryResp positionHistoryRequestHistoryPositions(
					String companyName,
					String accId,
					String serverName
	) throws Exception
	{
		// filter as open pos time
		
		// first we issue the request to all after cutOffTime
		log_m.debug("positionHistoryRequestHistoryPositions(..) - company name: " + companyName + ", account ID: " + accId + 
						", server: " + serverName);
		String key = companyName + accId + serverName;
		
		HtGetPositionHistoryResp r_all = null;
		PositionCacheEntry entry = HistoryPositionCache.getInstance().getPositionHistoryCache().get(key);
		
		// check if we need to recreate cache
		if (entry != null) {
			if (entry.lastRequest > 0) {
				if (HtDateTimeUtils.getCurGmtTime_DayBegin() > entry.lastRequest) {
					// reset
					entry = null;
					HistoryPositionCache.getInstance().getPositionHistoryCache().remove(key);
					log_m.debug("positionHistoryRequestHistoryPositions(..) - cache was reset as last request hapened yesterday: " +
									HtDateTimeUtils.time2SimpleString_Gmt(entry.lastRequest));
				}
			}
		}
		
		if (entry == null) {
			// nothing cached
			
			log_m.debug("positionHistoryRequestHistoryPositions(..) - cache was not found, making full request");
			
			String result =  makeRequestToHappyTraderServlet(	getPositionHistoryRequest( companyName, accId, serverName, MainPersistManager.QUERY_POSITION_ONLY_HISTORY, -1, -1));
			r_all = parsePositionHistoryRequest(result);
			
			entry = new PositionCacheEntry();
			entry.accountId = accId;
			entry.serverName = serverName;
			entry.companyName  = companyName;
			entry.lastRequest = HtDateTimeUtils.getCurGmtTime();
			entry.posList_m = r_all.getPosHistoryObject().getPositions();
			
			entry.calculateLastCloseTime();
			HistoryPositionCache.getInstance().getPositionHistoryCache().put(key, entry);
			
			log_m.debug("positionHistoryRequestHistoryPositions(..) - when cache created found the last close time: " + HtDateTimeUtils.time2SimpleString_Gmt(entry.maxCloseTime));
		
		} else {
	
			
			
			HtMtEventWrapperPositionsHistory.PositionEntry lastPosEntry = null;
			
			
			
			log_m.debug("positionHistoryRequestHistoryPositions(..) - cache found, last request time: " 
							+HtDateTimeUtils.time2SimpleString_Gmt(entry.lastRequest) + ", records found: " + entry.posList_m.size() +
							", last pos close time: " + HtDateTimeUtils.time2SimpleString_Gmt(entry.maxCloseTime));
			
		
			// query only the part
			String result =  makeRequestToHappyTraderServlet(	getPositionHistoryRequest( companyName, accId, serverName,
									MainPersistManager.QUERY_POSITION_ONLY_HISTORY, -1, entry.maxCloseTime));
			
			entry.lastRequest = HtDateTimeUtils.getCurGmtTime();
			
			HtGetPositionHistoryResp r_afterCutOff = parsePositionHistoryRequest(result);
			
			log_m.debug("positionHistoryRequestHistoryPositions(..) - delivered rows after cutoff time: " 
							+ r_afterCutOff.getPosHistoryObject().getPositions().size()  );
			
			// merge
			r_all = new HtGetPositionHistoryResp(HtGetPositionHistoryReq.COMMAND_NAME);
			r_all.setPosHistoryObject(new HtMtEventWrapperPositionsHistory());
			
			r_all.getPosHistoryObject().setPositionChangesProvided(r_afterCutOff.getPosHistoryObject().getPositionChangesProvided());
			r_all.getPosHistoryObject().setAccountId(accId);
			r_all.getPosHistoryObject().setCompanyName(companyName);
			
			r_all.getPosHistoryObject().getPositions().addAll(entry.posList_m);
			// assuming we have sorted list
			if (entry.posList_m.size() > 0) {
				long lastId =  entry.posList_m.get( entry.posList_m.size()-1).getHtMtEventWrapperPositionChange().getPositionUid();
				
				for(int i = 0; i < r_afterCutOff.getPosHistoryObject().getPositions().size(); i++) {
					HtMtEventWrapperPositionsHistory.PositionEntry e=  r_afterCutOff.getPosHistoryObject().getPositions().get(i);
					
					// add this
					if (e.getHtMtEventWrapperPositionChange().getPositionUid() > lastId)
						r_all.getPosHistoryObject().getPositions().add(e);
				}
			}
			else {
				// no positions in cache
				r_all.getPosHistoryObject().getPositions().addAll(r_afterCutOff.getPosHistoryObject().getPositions());
			}
			
		}
		
		return r_all;
		
		
	};
	
}
