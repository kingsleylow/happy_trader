/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.request;

import com.fin.httrader.eventplugins.ht_mt_bridge.HtMtManager;
import com.fin.httrader.eventplugins.ht_mt_bridge.delivery.HtMtEventWrapperAccountHistory;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.HtMtJsonUtils;
import com.fin.httrader.eventplugins.ht_mt_bridge.persistent.MainPersistManager;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response.HtBaseIntResp;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response.HtGetAccountHistoryResp;
import java.util.logging.Level;
import org.json.simple.JSONObject;

/**
 *
 * @author Victor_Zoubok
 */
public class HtGetAccountHistoryReq extends HtBaseIntReq {
		private String companyName_m = null;
		private String accountName_m = null;
		//private boolean isIncludeBalanceChangeHistory_m = false;
		//private boolean isIncludeAccountChangeHistory_m = false;
		
		public static final String COMMAND_NAME = "GET_ACCOUNT_HISTORY_REQ";
		
		private String getContext()
		{
				return this.getClass().getSimpleName();
		}
			
		@Override
		public String getCommandName()
		{
				return COMMAND_NAME;
		}
		
		@Override
		public HtBaseIntResp processRequest() throws Exception
		{
				HtGetAccountHistoryResp resp = new HtGetAccountHistoryResp(getCommandName());
				
				if (!HtMtManager.getInstance().isInitialized())
						throw new HtException(getContext(), "processRequest", "System not initialized");
				
				int flag = MainPersistManager.QUERY_ACCOUNT_DEAFULT;
				if (HtMtManager.getInstance().getRunningParams().getProcessBalanceUpdate()) {
						flag  = MainPersistManager.QUERY_ACCOUNT_INCLUDE_BALANCE_HISTORY;
				}
				else if (HtMtManager.getInstance().getRunningParams().getProcessAccountUpdate()) {
						flag  = MainPersistManager.QUERY_ACCOUNT_INCLUDE_ACCOUNT_HISTORY;
				}
				
				
				HtMtEventWrapperAccountHistory r = HtMtManager.getInstance().queryAccountDetails(accountName_m, companyName_m, flag);
				resp.setAccHistoryObject(r);
				
					HtLog.log(Level.INFO, getContext(), "processRequest", "Requested account history for company: ["+ getCompanyName()+
								"], account: [" + getAccountName() + "]");
				
					
					
				return resp;
		}
		
		@Override
		public void initialize(JSONObject root_json) throws Exception
		{
				this.setCompanyName(HtMtJsonUtils.<String>getJsonField(root_json, "COMPANY_NAME"));
				this.setAccountName(HtMtJsonUtils.<String>getJsonField(root_json, "ACCOUNT_NAME"));
				
				//setIsIncludeBalanceChangeHistory( HtMtJsonUtils.<Boolean>getJsonField(root_json, "INCLUDE_BALANCE_DETAILS") );
				//setIsIncludeBalanceChangeHistory( HtMtJsonUtils.<Boolean>getJsonField(root_json, "INCLUDE_ACCOUNT_DETAILS") );
		}

		public String getCompanyName() {
				return companyName_m;
		}

		public void setCompanyName(String companyName) {
				this.companyName_m = companyName;
		}

		public String getAccountName() {
				return accountName_m;
		}

		public void setAccountName(String accountName) {
				this.accountName_m = accountName;
		}

		/*
		public boolean getIsIncludeBalanceChangeHistory() {
				return isIncludeBalanceChangeHistory_m;
		}

		public void setIsIncludeBalanceChangeHistory(boolean isIncludeBalanceChangeHistory) {
				this.isIncludeBalanceChangeHistory_m = isIncludeBalanceChangeHistory;
		}
		
		public boolean getIsIncludeAccountChangeHistory() {
				return isIncludeAccountChangeHistory_m;
		}

		public void setIsIncludeAccountChangeHistory(boolean isIncludeAccountChangeHistory) {
				this.isIncludeAccountChangeHistory_m = isIncludeAccountChangeHistory;
		}
		*/
		
	
}
