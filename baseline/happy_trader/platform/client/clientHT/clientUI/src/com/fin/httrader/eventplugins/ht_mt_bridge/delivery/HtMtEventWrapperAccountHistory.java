/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.eventplugins.ht_mt_bridge.delivery;

import com.fin.httrader.eventplugins.ht_mt_bridge.helper.HtMtJsonUtils;
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

	final private List<AccountChange> accountChange_m = new ArrayList<>();
	final private List<BalanceChange> balanceChange_m = new ArrayList<>();

	public HtMtEventWrapperAccountHistory() {
		type_m = HtMtEventWrapperBase.TYPE_ACCOUNT_HISTORY;
	}

	private String getContext() {
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

		public void fromJson(JSONObject upper_level_obj) throws Exception {
			equity = HtMtJsonUtils.getLongJsonField(upper_level_obj, "EQUITY");
			freeMargine = HtMtJsonUtils.getLongJsonField(upper_level_obj, "FREE_MARGIN");
			freeMargineMode = HtMtJsonUtils.getIntJsonField(upper_level_obj, "FREE_MARGIN_MODE");
			stopOutLevel = HtMtJsonUtils.getLongJsonField(upper_level_obj, "STOP_OUT_LEVEL");
			stopOutMode = HtMtJsonUtils.getIntJsonField(upper_level_obj, "STOP_OUT_MODE");

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

		public void fromJson(JSONObject upper_level_obj) throws Exception {
			balance_m = HtMtJsonUtils.getLongJsonField(upper_level_obj, "BALANCE");
		}
	};

	@Override
	public void fromJson(JSONObject upper_level_obj) throws Exception {
		isInit_m = HtMtJsonUtils.getBooleanJsonField(upper_level_obj, "IS_INITIALIZED");

		if (isInit_m) {
			accountId = HtMtJsonUtils.<String>getJsonField(upper_level_obj, "ACCOUNT_NAME");
			server = HtMtJsonUtils.<String>getJsonField(upper_level_obj, "SERVER");
			accountChangesFlag_m = HtMtJsonUtils.getIntJsonField(upper_level_obj, "CHANGES_FLAG");
			companyName = HtMtJsonUtils.<String>getJsonField(upper_level_obj, "COMPANY_NAME");
			holderName = HtMtJsonUtils.<String>getJsonField(upper_level_obj, "HOLDER_NAME");
			currency = HtMtJsonUtils.<String>getJsonField(upper_level_obj, "CURRENCY");
			leverage = HtMtJsonUtils.getIntJsonField(upper_level_obj, "LEVERAGE");
			lastBalance_m = HtMtJsonUtils.getLongJsonField(upper_level_obj, "LAST_BALANCE");

			if ((accountChangesFlag_m & MainPersistManager.QUERY_ACCOUNT_INCLUDE_ACCOUNT_HISTORY) > 0) {
				accountChange_m.clear();
				JSONArray acc_chnages_arr = (JSONArray) upper_level_obj.get("ACCOUNT_CHANGES");
				if (acc_chnages_arr != null) {
					for (int i = 0; i < acc_chnages_arr.size(); i++) {
						JSONObject obj = (JSONObject) acc_chnages_arr.get(i);

						AccountChange ac = new AccountChange();

						// parse that
						ac.fromJson(obj);
						accountChange_m.add(ac);
					}
				}
			}

			if ((accountChangesFlag_m & MainPersistManager.QUERY_ACCOUNT_INCLUDE_ACCOUNT_HISTORY) > 0) {
				balanceChange_m.clear();

				JSONArray balance_changes_arr = (JSONArray) upper_level_obj.get("BALANCE_CHANGES");
				if (balance_changes_arr != null) {
					for (int i = 0; i < balance_changes_arr.size(); i++) {
						JSONObject obj = (JSONObject) balance_changes_arr.get(i);

						BalanceChange bc = new BalanceChange();

						// parse that
						bc.fromJson(obj);
						balanceChange_m.add(bc);
					}

				}

			}

		}
	}

	@Override
	public void toJson(JSONObject upper_level_obj) throws Exception {
		super.toJson(upper_level_obj);

		// may be not valid
		JSONArray acc_json_arr = new JSONArray();
		JSONArray bcc_json_arr = new JSONArray();

		if (this.isInit_m) {
			upper_level_obj.put("IS_INITIALIZED", true);
			upper_level_obj.put("ACCOUNT_NAME", accountId);
			upper_level_obj.put("SERVER", server);
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
	public boolean routEventFor(String accountId, String companyName) throws Exception {
		if (HtUtils.nvl(accountId) || HtUtils.nvl(companyName)) {
			throw new HtException(getContext(), "routEventFor", "account id or company name invalid");
		}

		return this.accountId.equals(accountId) && this.companyName.equals(companyName);

	}

}
