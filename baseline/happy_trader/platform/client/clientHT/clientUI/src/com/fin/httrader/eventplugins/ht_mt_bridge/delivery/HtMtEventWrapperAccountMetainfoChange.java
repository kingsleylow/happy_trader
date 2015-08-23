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
public class HtMtEventWrapperAccountMetainfoChange extends HtMtEventWrapperBase{
		
	
		
		private String accountId;
		private String server;
		private String holderName;
		// base currenncy
		private String currency;
		private int leverage = -1;
		private String companyName;

		@Override
	public void fromJson(JSONObject upper_level_obj) throws Exception {
		throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
	}
	
		public HtMtEventWrapperAccountMetainfoChange()
		{
				type_m = HtMtEventWrapperBase.TYPE_ACCOUNT_MINFO_UPDATE;
		}
			
		private String getContext()
		{
				return this.getClass().getSimpleName();
		}
		
		public String getServer() {
				return this.server;
		}

		public void setServer(String server) {
				this.server = server;
		}
		
		public String getAccountId() {
				return accountId;
		}

		public void setAccountId(String accountId) {
				this.accountId = accountId;
		}

		public String getHolderName() {
				return holderName;
		}

		public void setHolderName(String holderName) {
				this.holderName = holderName;
		}

		public String getCurrency() {
				return currency;
		}

		public void setCurrency(String currency) {
				this.currency = currency;
		}

		public int getLeverage() {
				return leverage;
		}

		public void setLeverage(int leverage) {
				this.leverage = leverage;
		}
		
		public String getCompanyName() {
				return companyName;
		}

		public void setCompanyName(String companyName) {
				this.companyName = companyName;
		}

		@Override
		public void toJson(JSONObject upper_level_obj) throws Exception {
				super.toJson(upper_level_obj);
			
				upper_level_obj.put("ACCOUNT_NAME", accountId);
				upper_level_obj.put("SERVER", server);
				upper_level_obj.put("HOLDER_NAME", holderName);
				upper_level_obj.put("CURRENCY", currency);
				upper_level_obj.put("LEVERAGE", leverage);
				upper_level_obj.put("COMPANY_NAME", companyName);
				
		}
		
		@Override
		public boolean routEventFor(String accountId, String companyName) throws Exception
		{
				if (HtUtils.nvl(accountId) || HtUtils.nvl(companyName))
						throw new HtException(getContext(), "routEventFor", "account id or company name invalid");
				
				return this.accountId.equals(accountId) && this.companyName.equals(companyName);
				
		}
}
