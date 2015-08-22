/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.eventplugins.ht_mt_bridge.delivery;

import com.fin.httrader.eventplugins.ht_mt_bridge.mtstruct_db.HtMtAccountMetainfo;
import com.fin.httrader.eventplugins.ht_mt_bridge.persistent.MainPersistManager;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtUtils;
import java.util.ArrayList;
import java.util.List;
import org.json.simple.JSONArray;
import org.json.simple.JSONObject;

/**
 *
 * @author Victor_Zoubok
 */
public class HtMtEventWrapperAccountHistory extends HtMtEventWrapperBase {

		

		private String companyName;
		private String accountId;
		private String server;
		private String holderName;
		// base currenncy
		private String currency;
		private int leverage = -1;
		private int accountChangesFlag_m = MainPersistManager.QUERY_ACCOUNT_DEAFULT;
		private long lastBalance_m = -1; // last balance store here
		private boolean isInit_m = false;

		final private List<AccountChange> accountChange_m = new ArrayList<AccountChange>();
		final private List<BalanceChange> balanceChange_m = new ArrayList<BalanceChange>();

		public HtMtEventWrapperAccountHistory() {
				type_m = HtMtEventWrapperBase.TYPE_ACCOUNT_HISTORY;
		}
		
		private String getContext()
		{
				return this.getClass().getSimpleName();
		}
		
		public boolean isInit() {
				return this.isInit_m;
		}

		public void setInit() {
				this.isInit_m = true;
		}

		public String getServer() {
				return this.server;
		}

		public void setServer(String server) {
				this.server = server;
		}
		
		public String getCompanyName() {
				return this.companyName;
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

		public List<AccountChange> getAccountChange() {
				return accountChange_m;
		}

		public List<BalanceChange> getBalanceChange() {
				return balanceChange_m;
		}

		public int getAccountChangesFlag() {
				return this.accountChangesFlag_m;
		}

		public void setAccountChangesFlag(int accChangesFlag) {
				this.accountChangesFlag_m = accChangesFlag;
		}

		public long getLastBalance() {
				return lastBalance_m;
		}

		public void setLastBalance(long lastBalance) {
				lastBalance_m = lastBalance;
		}

		public static class AccountChange {

				private long equity = -1;
				private long freeMargine = -1;
				private int freeMargineMode = -1;
				private long stopOutLevel = -1;
				private int stopOutMode = -1;

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

				public void toJson(JSONObject upper_level_obj) throws Exception {
						upper_level_obj.put("EQUITY", equity);
						upper_level_obj.put("FREE_MARGIN", freeMargine);
						upper_level_obj.put("FREE_MARGIN_MODE", freeMargineMode);
						upper_level_obj.put("STOP_OUT_LEVEL", stopOutLevel);
						upper_level_obj.put("STOP_OUT_MODE", stopOutMode);
				}
		};

		public static class BalanceChange {

				private long balance_m = -1;

				public long getBalance() {
						return balance_m;
				}

				public void setBalance(long balance) {
						this.balance_m = balance;
				}

				public void toJson(JSONObject upper_level_obj) throws Exception {
						upper_level_obj.put("BALANCE", balance_m);
				}
		};

		@Override
		public void toJson(JSONObject upper_level_obj) throws Exception {
				super.toJson(upper_level_obj);

				// may be not valid
				JSONArray acc_json_arr = new JSONArray();
				JSONArray bcc_json_arr = new JSONArray();
						
				if (this.isInit_m) {
						upper_level_obj.put("IS_INITIALIZED", true);
						upper_level_obj.put("ACCOUNT_NAME", accountId);
						upper_level_obj.put("SERVER", this.server);
						upper_level_obj.put("CHANGES_FLAG", accountChangesFlag_m);
						upper_level_obj.put("COMPANY_NAME", companyName);
						upper_level_obj.put("HOLDER_NAME", holderName);
						upper_level_obj.put("CURRENCY", currency);
						upper_level_obj.put("LEVERAGE", leverage);
						upper_level_obj.put("LAST_BALANCE", lastBalance_m);

						
						if ((accountChangesFlag_m & MainPersistManager.QUERY_ACCOUNT_INCLUDE_ACCOUNT_HISTORY) > 0) {

								for (int i = 0; i < accountChange_m.size(); i++) {
										AccountChange ac = accountChange_m.get(i);

										JSONObject upper_lev_json = new JSONObject();
										ac.toJson(upper_lev_json);

										acc_json_arr.add(upper_lev_json);
								}
						}

						if ((accountChangesFlag_m & MainPersistManager.QUERY_ACCOUNT_INCLUDE_BALANCE_HISTORY) > 0) {
								for (int i = 0; i < balanceChange_m.size(); i++) {
										BalanceChange bc = balanceChange_m.get(i);

										JSONObject upper_lev_json = new JSONObject();
										bc.toJson(upper_lev_json);

										bcc_json_arr.add(upper_lev_json);
								}
						}

						upper_level_obj.put("ACCOUNT_CHANGES", acc_json_arr);
						upper_level_obj.put("BALANCE_CHANGES", bcc_json_arr);
				} else {
						// not valid
						upper_level_obj.put("IS_INITIALIZED", false);
						upper_level_obj.put("ACCOUNT_CHANGES", acc_json_arr);
						upper_level_obj.put("BALANCE_CHANGES", bcc_json_arr);
				}

		}
		
		@Override
		public boolean routEventFor(String accountId, String companyName) throws Exception
		{
				if (HtUtils.nvl(accountId) || HtUtils.nvl(companyName))
						throw new HtException(getContext(), "routEventFor", "account id or company name invalid");
				
				return this.accountId.equals(accountId) && this.companyName.equals(companyName);
				
		}

}
