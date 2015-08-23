/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.eventplugins.ht_mt_bridge.delivery;

import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtUtils;
import org.json.simple.JSONObject;

/**
 *
 * @author Victor_Zoubok
 */
public class HtMtEventWrapperAccountChange extends HtMtEventWrapperBase {

	@Override
	public void fromJson(JSONObject upper_level_obj) throws Exception {
		throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
	}
		public HtMtEventWrapperAccountChange() {
				type_m = HtMtEventWrapperBase.TYPE_ACCOUNT_UPDATE;
		}
		
		private String getContext()
		{
				return this.getClass().getSimpleName();
		}

		private String companyName;
		private String accountId;
		private String server;
		private long equity = -1;
		private long freeMargine = -1;
		private int freeMargineMode = -1;
		private long stopOutLevel = -1;
		private int stopOutMode = -1;

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

		public String getServer() {
				return server;
		}

		public void setServer(String server) {
				this.server = server;
		}

		public long getEquity() {
				return equity;
		}

		public void setEquity(long equity) {
				this.equity = equity;
		}

		public long getFreeMargine() {
				return freeMargine;
		}

		public void setFreeMargine(long freeMargine) {
				this.freeMargine = freeMargine;
		}

		public int getFreeMargineMode() {
				return freeMargineMode;
		}

		public void setFreeMargineMode(int freeMargineMode) {
				this.freeMargineMode = freeMargineMode;
		}

		public long getStopOutLevel() {
				return stopOutLevel;
		}

		public void setStopOutLevel(long stopOutLevel) {
				this.stopOutLevel = stopOutLevel;
		}

		public int getStopOutMode() {
				return stopOutMode;
		}

		public void setStopOutMode(int stopOutMode) {
				this.stopOutMode = stopOutMode;
		}

		@Override
		public void toJson(JSONObject upper_level_obj) throws Exception {
				super.toJson(upper_level_obj);

				upper_level_obj.put("ACCOUNT_NAME", accountId);
				upper_level_obj.put("COMPANY_NAME", companyName);
				upper_level_obj.put("SERVER", server);
				upper_level_obj.put("EQUITY", equity);
				upper_level_obj.put("FREE_MARGIN", freeMargine);
				upper_level_obj.put("FREE_MARGIN_MODE", freeMargineMode);
				upper_level_obj.put("STOP_OUT_LEVEL", stopOutLevel);
				upper_level_obj.put("STOP_OUT_MODE", stopOutMode);

		}

		@Override
		public boolean routEventFor(String accountId, String companyName) throws Exception
		{
				if (HtUtils.nvl(accountId) || HtUtils.nvl(companyName))
						throw new HtException(getContext(), "routEventFor", "account id or company name invalid");
				
				return this.accountId.equals(accountId) && this.companyName.equals(companyName);
				
		}

}
