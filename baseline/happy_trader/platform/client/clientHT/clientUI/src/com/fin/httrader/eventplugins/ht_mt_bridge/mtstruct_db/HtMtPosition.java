/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.eventplugins.ht_mt_bridge.mtstruct_db;

import com.fin.httrader.hlpstruct.OrderType;
import com.fin.httrader.hlpstruct.PosState;
import com.fin.httrader.hlpstruct.PosType;
import com.fin.httrader.hlpstruct.TradeDirection;
import com.fin.httrader.utils.Uid;
import java.math.BigDecimal;
import java.util.List;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.Table;

@Entity
@Table(name = "position")
public class HtMtPosition {

		private long positionUid = -1; // PK
		private long accountMetainfoUid = -1; // ref to acc metainfo

		//private long lastUpdateTime = -1;
		//private long lastCreateTime = -1;
		private String tradeSymbol;
		private String ticket;
		private String comment;
		private long openTime = -1;
		private long closeTime = -1;
		private long openPrice = -1;
		private long closePrice = -1;

		private long initialStopLoss = -1;
		private long initialTakeProfit = -1;
		private long profit = -1;
		private long expiration = -1; // date time expiration
		
		private long volume = -1; // lots 

		// make sence for open  orders
		private int direction = TradeDirection.TD_NONE;
		private int posState = PosState.STATE_DUMMY;
		private int posType = PosType.POS_DUMMY; // if position is pending or not
		

		@Id
		@GeneratedValue(strategy = GenerationType.IDENTITY)
		@Column(name = "id")
		public long getPositionUid() {
				return positionUid;
		}

		public void setPositionUid(long positionUid) {
				this.positionUid = positionUid;
		}

		@Column(name = "pos_type", columnDefinition = "SMALLINT(6)")
		public int getPosType() {
				return this.posType;
		}

		public void setPosType(int posType) {
				this.posType = posType;
		}

		@Column(name = "account_minfo_id")
		public long getAccountMetainfoUid() {
				return accountMetainfoUid;
		}

		/**
		 * @param accountMetainfoUid the accountMetainfoUid to set
		 */
		public void setAccountMetainfoUid(long accountMetainfoUid) {
				this.accountMetainfoUid = accountMetainfoUid;
		}


		/*
		 public long getLastUpdateTime() {
		 return lastUpdateTime;
		 }

		
		 public void setLastUpdateTime(long lastUpdateTime) {
		 this.lastUpdateTime = lastUpdateTime;
		 }
		 */
		@Column(name = "trade_symbol")
		public String getTradeSymbol() {
				return tradeSymbol;
		}

		public void setTradeSymbol(String tradeSymbol) {
				this.tradeSymbol = tradeSymbol;
		}

		@Column(name = "ticket")
		public String getTicket() {
				return ticket;
		}

		public void setTicket(String ticket) {
				this.ticket = ticket;
		}

		@Column(name = "comment")
		public String getComment() {
				return comment;
		}

		public void setComment(String comment) {
				this.comment = comment;
		}

		@Column(name = "open_time")
		public long getOpenTime() {
				return openTime;
		}

		public void setOpenTime(long openTime) {
				this.openTime = openTime;
		}

		@Column(name = "close_time")
		public long getCloseTime() {
				return closeTime;
		}

		public void setCloseTime(long closeTime) {
				this.closeTime = closeTime;
		}

		@Column(name = "open_price")
		public long getOpenPrice() {
				return openPrice;
		}

		public void setOpenPrice(long openPrice) {
				this.openPrice = openPrice;
		}

		@Column(name = "close_price")
		public long getClosePrice() {
				return closePrice;
		}

		public void setClosePrice(long closePrice) {
				this.closePrice = closePrice;
		}

		@Column(name = "direction", columnDefinition = "SMALLINT(6)")
		public int getDirection() {
				return direction;
		}

		public void setDirection(int direction) {
				this.direction = direction;
		}

		@Column(name = "pos_state", columnDefinition = "SMALLINT(6)")
		public int getPosState() {
				return posState;
		}

		public void setPosState(int posState) {
				this.posState = posState;
		}

		@Column(name = "init_sl")
		public long getInitialStopLoss() {
				return initialStopLoss;
		}

		public void setInitialStopLoss(long initialStopLoss) {
				this.initialStopLoss = initialStopLoss;
		}

		@Column(name = "init_tp")
		public long getInitialTakeProfit() {
				return initialTakeProfit;
		}

		public void setInitialTakeProfit(long initialTakeProfit) {
				this.initialTakeProfit = initialTakeProfit;
		}

		@Column(name = "profit")
		public long getProfit() {
				return profit;
		}

		public void setProfit(long profit) {
				this.profit = profit;
		}

		@Column(name = "expiration")
		public long getExpiration() {
				return expiration;
		}

		public void setExpiration(long expiration) {
				this.expiration = expiration;
		}
		
		@Column(name = "volume")
		public long getLots() {
				return volume;
		}

		public void setLots(long lots) {
				this.volume = lots;
		}

		//just generates HtMtPositionUpdate
		public HtMtPositionUpdate createUpdateObject() {
				HtMtPositionUpdate update = new HtMtPositionUpdate();

				update.setPositionUid(this.getPositionUid()); // init FK

				update.setProfit(this.getProfit());
				update.setStopLoss(this.getInitialStopLoss());
				update.setTakeProfit(this.getInitialTakeProfit());

				return update;
		}

		public HtMtPositionUpdate doWeHaveUpdateObject(HtMtPosition posupdate) {
				// compares 2 objects and returns update object if exists
				if (this.getInitialStopLoss() != posupdate.getInitialStopLoss()
					|| this.getInitialTakeProfit() != posupdate.getInitialTakeProfit()
					|| this.getProfit() != posupdate.getProfit()) {
						return createUpdateObject();
				}

				return null;

		}

		public void assign(HtMtPosition src) {
				accountMetainfoUid = src.accountMetainfoUid; // ref to acc metainfo

				tradeSymbol = src.tradeSymbol;
				ticket = src.ticket;
				comment = src.comment;
				openTime = src.openTime;
				closeTime = src.closeTime;
				openPrice = src.openPrice;
				closePrice = src.closePrice;
				volume = src.volume;

				initialStopLoss = src.initialStopLoss;
				initialTakeProfit = src.initialTakeProfit;
				profit = src.profit;
				expiration = src.expiration; // date time expiration

		// make sence for open  orders
				direction = src.direction;
				posState = src.posState;
				posType = src.posType;
		}

		public boolean equalTo(HtMtPosition src) {
				// compares to existing object

				return (ticket.equals(src.ticket)
					&& comment.equals(src.comment)
					&& openTime == src.openTime
					&& openPrice == src.openPrice
					&& closeTime == src.closeTime
					&& closePrice == src.closePrice
					&& initialStopLoss == src.initialStopLoss
					&& initialTakeProfit == src.initialTakeProfit
					&& profit == src.profit
					&& expiration == src.expiration
					&& direction == src.direction
					&& posState == src.posState
					&& posType == src.posType
					&& volume == src.volume
				);

		}

}
