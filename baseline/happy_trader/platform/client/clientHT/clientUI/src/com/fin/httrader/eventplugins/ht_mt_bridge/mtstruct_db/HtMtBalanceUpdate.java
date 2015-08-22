/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.eventplugins.ht_mt_bridge.mtstruct_db;

import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.Table;

@Entity
@Table(name="balance_update") 
public class HtMtBalanceUpdate {
		
		
		private long balanceUpdateUid = -1; // PK
		private long accountMetainfoUid = -1; // reference to account metainfo
		
		private long balance = -1;
		
		@Id
		@GeneratedValue(strategy=GenerationType.IDENTITY)
		@Column(name="id")
		public long getBalanceUpdateUid() {
				return balanceUpdateUid;
		}
		
	
		public void setBalanceUpdateUid(long accountUpdateUid) {
				this.balanceUpdateUid = accountUpdateUid;
		}
		
		@Column(name="balance")
		public long getBalance() {
				return balance;
		}
		
		public void setBalance(long balance) {
				this.balance = balance;
		}
		
		
		@Column(name="account_minfo_id")
		public long getAccountMetainfoUid() {
				return accountMetainfoUid;
		}

		
		public void setAccountMetainfoUid(long accountMetainfoUid) {
				this.accountMetainfoUid = accountMetainfoUid;
		}
		
		
		
		public boolean equalTo(HtMtBalanceUpdate src)
		{
				return (this.balance == src.balance );
		};
		

}
