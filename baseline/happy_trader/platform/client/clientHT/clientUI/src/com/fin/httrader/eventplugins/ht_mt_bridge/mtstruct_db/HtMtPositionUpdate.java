/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.eventplugins.ht_mt_bridge.mtstruct_db;

import com.fin.httrader.utils.Uid;
import java.math.BigDecimal;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.Table;

@Entity
@Table(name="position_update") 
public class HtMtPositionUpdate  {
		
		private long posUpdateUid = -1; // PK
		private long positionUid = -1; // ref to position
		//private long lastUpdateTime = -1;
		private long stopLoss = -1;
		private long takeProfit = -1;
		private long profit = -1;

		
		@Id
		@GeneratedValue(strategy=GenerationType.IDENTITY)
		@Column(name="id")
		public long getPosUpdateUid() {
				return posUpdateUid;
		}

		
		public void setPosUpdateUid(long posUpdateUid) {
				this.posUpdateUid = posUpdateUid;
		}

		
		@Column(name="position_id")
		public long getPositionUid() {
				return positionUid;
		}

	
		public void setPositionUid(long positionUid) {
				this.positionUid = positionUid;
		}

	
		/*
		public long getLastUpdateTime() {
				return lastUpdateTime;
		}

		
		public void setLastUpdateTime(long lastUpdateTime) {
				this.lastUpdateTime = lastUpdateTime;
		}
		*/
	
		@Column(name="sl")
		public long getStopLoss() {
				return stopLoss;
		}

		public void setStopLoss(long stopLoss) {
				this.stopLoss = stopLoss;
		}

		@Column(name="tp")
		public long getTakeProfit() {
				return takeProfit;
		}

		public void setTakeProfit(long takeProfit) {
				this.takeProfit = takeProfit;
		}

	
		@Column(name="profit")
		public long getProfit() {
				return profit;
		}

		public void setProfit(long profit) {
				this.profit = profit;
		}
		
		public boolean compareTo(HtMtPositionUpdate src)
		{
				return ( 
					this.getProfit() == src.getProfit() &&
					this.getStopLoss() == src.getStopLoss() &&
					this.getTakeProfit() == src.getTakeProfit()
				);
		}
}
