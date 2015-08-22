/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.eventplugins.ht_mt_bridge.delivery;

import com.fin.httrader.eventplugins.ht_mt_bridge.helper.HtMtJsonUtils;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.mt.MTProxyPacketBase;
import com.fin.httrader.utils.HtException;
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
 * this is position history
 */
public class HtMtEventWrapperPositionsHistory extends HtMtEventWrapperBase{
		
		private boolean positionChnagesProvided_m = false;
		private String companyName;
		private String accountId;
		//final private Map<Long, PositionEntry> positionHistory_m = new HashMap<Long, PositionEntry>();
		final private List<PositionEntry> positionHistory_m = new ArrayList<PositionEntry>();
	
		private String getContext()
		{
				return this.getClass().getSimpleName();
		}
		
		public HtMtEventWrapperPositionsHistory() {
				type_m = HtMtEventWrapperBase.TYPE_POSITION_HISTORY;
		}
		
		public static class PositionChange
		{
				private long stopLoss = -1;
				private long takeProfit = -1;
				private long profit = -1;

				public long getStopLoss() {
						return stopLoss;
				}

				public void setStopLoss(long stopLoss) {
						this.stopLoss = stopLoss;
				}

				public long getTakeProfit() {
						return takeProfit;
				}

				public void setTakeProfit(long takeProfit) {
						this.takeProfit = takeProfit;
				}

				public long getProfit() {
						return profit;
				}

				public void setProfit(long profit) {
						this.profit = profit;
				}
				
				void toJson(JSONObject upper_level_json) throws Exception
				{
						upper_level_json.put("PROFIT", profit);
						upper_level_json.put("SL", stopLoss);
						upper_level_json.put("TP", takeProfit);
						
				}
				
				void fromJson(JSONObject upper_level_json) throws Exception
				{
					profit = HtMtJsonUtils.getLongJsonField(upper_level_json, "PROFIT");
					stopLoss = HtMtJsonUtils.getLongJsonField(upper_level_json, "SL");
					takeProfit = HtMtJsonUtils.getLongJsonField(upper_level_json, "TP");
					
						
				}
		};
		
		public static class PositionEntry {
				// position and list of changes
				private final HtMtEventWrapperPositionChange position_m = new HtMtEventWrapperPositionChange();
				private final List<PositionChange> changes_m = new ArrayList<PositionChange>();

				public List<PositionChange> getChanges() {
						return changes_m;
				}

			
				public HtMtEventWrapperPositionChange getHtMtEventWrapperPositionChange()
				{
						return position_m;
				};
		};
		
		
		
		public String getCompanyName() {
				return companyName;
		}

		public void setCompanyName(String companyName) {
				this.companyName = companyName;
		}
		
		public String getAccountId() {
				return accountId;
		}

		public void setAccountId(String accountId) {
				this.accountId = accountId;
		}
		
		/*
		public  Map<Long, PositionEntry> getPositions()
		{
						return positionHistory_m;
		};
		*/
		
		public List<PositionEntry> getPositions()
		{
						return positionHistory_m;
		};
		
		public boolean getPositionChangesProvided()
		{
				return positionChnagesProvided_m;
		}
		
		public void setPositionChangesProvided(boolean positionChnagesProvided)
		{
				positionChnagesProvided_m = positionChnagesProvided;
		}
		
		@Override
		public void fromJson(JSONObject upper_level_obj) throws Exception
		{
			positionChnagesProvided_m = HtMtJsonUtils.getBooleanJsonField(upper_level_obj,"CHANGES_INCLUDED");
			companyName = HtMtJsonUtils.<String>getJsonField(upper_level_obj, "COMPANY_NAME");
			accountId = HtMtJsonUtils.<String>getJsonField(upper_level_obj, "ACCOUNT_ID");
			
			JSONArray pos_list_arr = (JSONArray)upper_level_obj.get("POSITION_LIST");
			if (pos_list_arr != null) {
				for(int i=0; i < pos_list_arr.size(); i++) {
					JSONObject obj = (JSONObject)pos_list_arr.get(i);
					
					PositionEntry pe = new PositionEntry();
					
					// parse that
					pe.getHtMtEventWrapperPositionChange().fromJson(obj);
					
					if (positionChnagesProvided_m) {
						pe.getChanges().clear();
						
						JSONArray changes_json = (JSONArray)obj.get("CHANGES");
						for(int j=0; j < changes_json.size(); i++) {
							JSONObject change_o = (JSONObject)changes_json.get(i);
							PositionChange pcc = new PositionChange();
							pcc.fromJson(change_o);
							
							pe.getChanges().add(pcc);
						}
						
					}
					//positionHistory_m.put(pe.getHtMtEventWrapperPositionChange().getPositionUid(), pe);
					positionHistory_m.add(pe);
				}
			}
			
			
		}
		
		@Override
		public void toJson(JSONObject upper_level_obj) throws Exception
		{
				super.toJson(upper_level_obj);
				upper_level_obj.put("CHANGES_INCLUDED", positionChnagesProvided_m);
				
				JSONArray positions_json = new JSONArray();
				
				//for(Iterator<Long> it = positionHistory_m.keySet().iterator(); it.hasNext(); ) {
				for(int it = 0; it < positionHistory_m.size(); it++ ) {
						PositionEntry pe = positionHistory_m.get(it);
						
						JSONObject pe_json = new JSONObject();
						
						pe.getHtMtEventWrapperPositionChange().toJson(pe_json);
						
						if (positionChnagesProvided_m) {
								// history
								JSONArray changes_json = new JSONArray();
								
								List<PositionChange> poschnages = pe.getChanges();
								for(int i = 0; i < poschnages.size(); i++) {
										PositionChange pcc = poschnages.get( i );
										
										JSONObject change_entry_json = new JSONObject();
										pcc.toJson(change_entry_json);
										
										changes_json.add(change_entry_json);
								}
								
								pe_json.put("CHANGES", changes_json);
						}
						
						positions_json.add(pe_json);
						
				}
				
				upper_level_obj.put("POSITION_LIST", positions_json);
				upper_level_obj.put("COMPANY_NAME", companyName);
				upper_level_obj.put("ACCOUNT_ID", accountId);
				
				
		}
		
		@Override
		public boolean routEventFor(String accountId, String companyName) throws Exception
		{
				if (HtUtils.nvl(accountId) || HtUtils.nvl(companyName))
						throw new HtException(getContext(), "routEventFor", "account id or company name invalid");
				
				return this.accountId.equals(accountId) && this.companyName.equals(companyName);
				
		}
		
}
