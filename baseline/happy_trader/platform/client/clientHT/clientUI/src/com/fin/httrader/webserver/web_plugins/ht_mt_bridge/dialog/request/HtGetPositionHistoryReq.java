/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.request;

import com.fin.httrader.eventplugins.ht_mt_bridge.HtMtManager;
import com.fin.httrader.eventplugins.ht_mt_bridge.delivery.HtMtEventWrapperPositionsHistory;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.HtMtJsonUtils;
import com.fin.httrader.eventplugins.ht_mt_bridge.persistent.MainPersistManager;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response.HtBaseIntResp;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response.HtGetPositionHistoryResp;
import java.util.logging.Level;
import org.json.simple.JSONObject;


/**
 *
 * @author Victor_Zoubok
 */
public class HtGetPositionHistoryReq extends HtBaseIntReq {
		public static final String COMMAND_NAME = "GET_POSITION_HISTORY_REQ";
		
		private String companyName_m = null;
		private String accountName_m = null;
		private String serverName_m = null;
		private boolean isIncludeChangeHistory_m = false;
		private int typeOfPosToSelect_m = -1;
		private long openTimeFilter_m = -1;
		private long closeTimeFilter_m = -1;
		
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
				HtGetPositionHistoryResp resp = new HtGetPositionHistoryResp(getCommandName());
				
				// go to manager
				if (!HtMtManager.getInstance().isInitialized())
						throw new HtException(getContext(), "processRequest", "System not initialized");
				
				int flag_state = getTypeOfPosToSelect();
				
				if (!(flag_state == MainPersistManager.QUERY_POSITION_ONLY_OPEN || 
					flag_state == MainPersistManager.QUERY_POSITION_ONLY_HISTORY ||
					flag_state == MainPersistManager.QUERY_POSITION_ONLY_BOTH
				)) {
						throw new HtException(getContext(), "processRequest", "Invalid <TYPE_OF_POS_TO_SELECT> value, must be valid falg");
				}
				
				// do not include change history
				HtMtEventWrapperPositionsHistory r = HtMtManager.getInstance().queryPositions(
					getAccountName(), 
					getCompanyName(), 
					getServerName(),
					getTypeOfPosToSelect(), 
					getOpenTimeFilter(), 
					getCloseTimeFilter(), 
					isIsIncludeChangeHistory() ? MainPersistManager.QUERY_POSITION_INCLUDE_CHANGE_HISTORY : MainPersistManager.QUERY_POSITION_DO_NOT_INCLUDE_CHANGE_HISTORY
				);
				
				resp.setPosHistoryObject(r);
				
				//HtLog.log(Level.FINE, getContext(), "processRequest", "Requested position history for company: ["+ getCompanyName()+
				//				"], account: [" + getAccountName()+ "], server: ["+ getServerName() +"] number of positions returned: " + r.getPositions().size());
				
				return resp;
		}
		
		@Override
		public void initialize(JSONObject root_json) throws Exception
		{
				
				this.setCompanyName(HtMtJsonUtils.<String>getJsonField(root_json, "COMPANY_NAME"));
				this.setAccountName(HtMtJsonUtils.<String>getJsonField(root_json, "ACCOUNT_NAME"));
				this.setServerName(HtMtJsonUtils.<String>getJsonField(root_json, "SERVER_NAME"));
				
				setOpenTimeFilter(HtMtJsonUtils.<Number>getJsonField(root_json, "OPEN_POS_FILTER_TIME").longValue());
				setCloseTimeFilter(HtMtJsonUtils.<Number>getJsonField(root_json, "CLOSE_POS_FILTER_TIME").longValue());
				
				setIsIncludeChangeHistory( HtMtJsonUtils.<Boolean>getJsonField(root_json, "INCLUDE_CHANGE_DETAILS") );
				setTypeOfPosToSelect(HtMtJsonUtils.<Number>getJsonField(root_json, "TYPE_OF_POS_TO_SELECT").intValue() );
				
				
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
		
		public String getServerName() {
				return this.serverName_m;
		}

		public void setServerName(String serverName) {
				this.serverName_m = serverName;
		}

		public boolean isIsIncludeChangeHistory() {
				return isIncludeChangeHistory_m;
		}

		public void setIsIncludeChangeHistory(boolean isIncludeChangeHistory) {
				this.isIncludeChangeHistory_m = isIncludeChangeHistory;
		}

		public int getTypeOfPosToSelect() {
				return typeOfPosToSelect_m;
		}

		public void setTypeOfPosToSelect(int typeOfPosToSelect) {
				this.typeOfPosToSelect_m = typeOfPosToSelect;
		}

		public long getOpenTimeFilter() {
				return openTimeFilter_m;
		}

		public void setOpenTimeFilter(long openTimeFilter) {
				this.openTimeFilter_m = openTimeFilter;
		}

		public long getCloseTimeFilter() {
				return closeTimeFilter_m;
		}

		public void setCloseTimeFilter(long closeTimeFilter) {
				this.closeTimeFilter_m = closeTimeFilter;
		}
}
