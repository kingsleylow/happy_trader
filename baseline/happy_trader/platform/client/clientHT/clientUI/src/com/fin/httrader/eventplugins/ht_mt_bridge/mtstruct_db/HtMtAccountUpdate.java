/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.eventplugins.ht_mt_bridge.mtstruct_db;


import java.util.ArrayList;
import java.util.List;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.Table;


@Entity
@Table(name="account_update") 
public class HtMtAccountUpdate  {
		
		private long accountUpdateUid = -1; // PK
		private long accountMetainfoUid = -1; // reference to account metainfo
	
		
				
		
		private long equity = -1;
		private long freeMargine = -1;
		private int freeMargineMode = -1;
		private long stopOutLevel = -1;
		private int stopOutMode= -1;
		
		
		public boolean equalTo(HtMtAccountUpdate src)
		{
				// do not 
				return (equity == src.equity && 
					freeMargine == src.freeMargine && 
					freeMargineMode == src.freeMargineMode && 
					stopOutLevel == src.stopOutLevel && 
					stopOutMode == src.stopOutMode
				);
		};

		@Id
		@GeneratedValue(strategy=GenerationType.IDENTITY)
		@Column(name="id")
		public long getAccountUpdateUid() {
				return accountUpdateUid;
		}
		
		public void setAccountUpdateUid(long accountUpdateUid) {
				this.accountUpdateUid = accountUpdateUid;
		}

		
	
	

		@Column(name="account_minfo_id")
		public long getAccountMetainfoUid() {
				return accountMetainfoUid;
		}

		
		public void setAccountMetainfoUid(long accountMetainfoUid) {
				this.accountMetainfoUid = accountMetainfoUid;
		}

	
		

		
		
		@Column(name="equity")
		public long getEquity() {
				return equity;
		}

		
		public void setEquity(long equity) {
				this.equity = equity;
		}

		
		@Column(name="free_margin")
		public long getFreeMargine() {
				return freeMargine;
		}

	
		public void setFreeMargine(long freeMargine) {
				this.freeMargine = freeMargine;
		}

		
		@Column(name="free_margin_mode", columnDefinition ="SMALLINT(6)" )
		public int getFreeMargineMode() {
				return freeMargineMode;
		}

		
		public void setFreeMargineMode(int freeMargineMode) {
				this.freeMargineMode = freeMargineMode;
		}

		
		@Column(name="stop_out", columnDefinition ="SMALLINT(6)")
		public long getStopOutLevel() {
				return stopOutLevel;
		}

		
		public void setStopOutLevel(long stopOutLevel) {
				this.stopOutLevel = stopOutLevel;
		}

		@Column(name="stop_out_mode", columnDefinition ="SMALLINT(6)")
		public int getStopOutMode() {
				return stopOutMode;
		}

		
		public void setStopOutMode(int stopOutMode) {
				this.stopOutMode = stopOutMode;
		}
		
		
		
		/*
		public long getLastUpdateTime() {
				return lastUpdateTime;
		}

		
		public void setLastUpdateTime(long lastUpdateTime) {
				this.lastUpdateTime = lastUpdateTime;
		}
		
		public long getCreateTime() {
				return createTime;
		}
		

		
		public void setCreateTime(long createTime) {
				this.createTime = createTime;
		}
		
		*/
		
	
		
		
}
