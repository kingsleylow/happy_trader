/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.eventplugins.ht_mt_bridge.mtstruct_db;

import com.fin.httrader.utils.Uid;
import java.util.ArrayList;
import java.util.List;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.Table;

@Entity
@Table(name="account_metainfo") 

public class HtMtAccountMetainfo {
		private long accountMetanfoUid = -1; // PK
		private long companyMetainfoUid = -1; // ref to company

		
		private String accountId;
		private String holderName;
		// base currenncy
		private String currency;
		private int leverage = -1;
		private String server;
		
		//private long createTime = -1;
		//private long lastUpdateTime = -1;
		
		
		@Column(name="leverage")
		public int getLeverage() {
				return leverage;
		}

		
		public void setLeverage(int leverage) {
				this.leverage = leverage;
		}
		
		@Id
		@GeneratedValue(strategy=GenerationType.IDENTITY)
		@Column(name="id")
		public long getAccountMetanfoUid() {
				return accountMetanfoUid;
		}

		
		public void setAccountMetanfoUid(long accountMetanfoUid) {
				this.accountMetanfoUid = accountMetanfoUid;
		}
		
		@Column(name="company_minfo_id")
		public long getCompanyMetanfoUid()
		{
				return companyMetainfoUid;
		}
		
		public void setCompanyMetanfoUid(long companyMetainfoUid) {
				this.companyMetainfoUid = companyMetainfoUid;
		}
		

		@Column(name="server")
		public String getServer() {
				return server;
		}

	
		public void setServer(String server) {
				this.server = server;
		}
	
		@Column(name="account_name")
		public String getAccountId() {
				return accountId;
		}

	
		public void setAccountId(String accountId) {
				this.accountId = accountId;
		}

		@Column(name="holder_name")
		public String getHolderName() {
				return holderName;
		}

		public void setHolderName(String holderName) {
				this.holderName = holderName;
		}

	
		@Column(name="currency")
		public String getCurrency() {
				return currency;
		}

		
		public void setCurrency(String currency) {
				this.currency = currency;
		}

	
		/*
		public long getCreateTime() {
				return createTime;
		}

		
		public void setCreateTime(long createTime) {
				this.createTime = createTime;
		}

		public long getLastUpdateTime() {
				return lastUpdateTime;
		}

		
		public void setLastUpdateTime(long lastUpdateTime) {
				this.lastUpdateTime = lastUpdateTime;
		}
		*/
		
}
