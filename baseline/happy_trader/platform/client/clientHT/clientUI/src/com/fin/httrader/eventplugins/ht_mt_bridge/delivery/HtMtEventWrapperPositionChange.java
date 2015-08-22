/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.eventplugins.ht_mt_bridge.delivery;

import com.fin.httrader.eventplugins.ht_mt_bridge.helper.HtMtJsonUtils;
import com.fin.httrader.eventplugins.ht_mt_bridge.mtstruct_db.HtMtPosition;
import com.fin.httrader.eventplugins.ht_mt_bridge.mtstruct_db.HtMtPositionUpdate;

import com.fin.httrader.hlpstruct.PosState;
import com.fin.httrader.hlpstruct.PosType;
import com.fin.httrader.hlpstruct.TradeDirection;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtUtils;
import java.util.ArrayList;
import java.util.List;
import org.json.simple.JSONObject;

/**
 *
 * @author Victor_Zoubok
 * This event encapsulate position chnage
 */
public class HtMtEventWrapperPositionChange extends HtMtEventWrapperBase{
		
		private long positionUid;
		private String companyName;
		private String accountId;
		private String tradeSymbol;
		private String ticket;
		private String comment;
		private long openTime = -1;
		private long closeTime = -1;
		private long openPrice  =-1;
		private long closePrice  =-1;
		private long volume = -1;
		
		private long stopLoss = -1;
		private long takeProfit = -1;
		private long profit= -1;
		private long expiration = -1; // date time expiration
		
		// make sence for open  orders
		
		private int direction = TradeDirection.TD_NONE;
		private int posState = PosState.STATE_DUMMY;
		private int posType = PosType.POS_DUMMY; // if position is pending or not

		
		
		
		// ---------------------------------
		public HtMtEventWrapperPositionChange()
		{
				type_m = HtMtEventWrapperBase.TYPE_POSITION_UPDATE;
		}
		
		private String getContext()
		{
				return this.getClass().getSimpleName();
		}
		
		public void setPositionUid(long posUid) {
				this.positionUid = posUid;
		}

		public long getPositionUid() {
				return this.positionUid;
		}
		
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
		
	
		
		public String getTradeSymbol() {
				return tradeSymbol;
		}

		public void setTradeSymbol(String tradeSymbol) {
				this.tradeSymbol = tradeSymbol;
		}

		public String getTicket() {
				return ticket;
		}

		public void setTicket(String ticket) {
				this.ticket = ticket;
		}

		public String getComment() {
				return comment;
		}

		public void setComment(String comment) {
				this.comment = comment;
		}

		public long getOpenTime() {
				return openTime;
		}

		public void setOpenTime(long openTime) {
				this.openTime = openTime;
		}

		public long getCloseTime() {
				return closeTime;
		}

		public void setCloseTime(long closeTime) {
				this.closeTime = closeTime;
		}

		public long getOpenPrice() {
				return openPrice;
		}

		public void setOpenPrice(long openPrice) {
				this.openPrice = openPrice;
		}

		public long getClosePrice() {
				return closePrice;
		}

		public void setClosePrice(long closePrice) {
				this.closePrice = closePrice;
		}
		
		
		public long getLots() {
				return this.volume;
		}

		public void setLots(long lots) {
				this.volume = lots;
		}

		public long getStopLoss() {
				return stopLoss;
		}

		public void setStopLoss(long initialStopLoss) {
				this.stopLoss = initialStopLoss;
		}

		public long getTakeProfit() {
				return takeProfit;
		}

		public void setTakeProfit(long initialTakeProfit) {
				this.takeProfit = initialTakeProfit;
		}

		public long getProfit() {
				return profit;
		}

		public void setProfit(long profit) {
				this.profit = profit;
		}

		public long getExpiration() {
				return expiration;
		}

		public void setExpiration(long expiration) {
				this.expiration = expiration;
		}

		public int getDirection() {
				return direction;
		}

		public void setDirection(int direction) {
				this.direction = direction;
		}

		public int getPosState() {
				return posState;
		}

		public void setPosState(int posState) {
				this.posState = posState;
		}

		public int getPosType() {
				return posType;
		}

		public void setPosType(int posType) {
				this.posType = posType;
		}
			
		public void initializeFromPos(HtMtPosition pos)
		{
				this.setPositionUid(pos.getPositionUid());
				this.setClosePrice(pos.getClosePrice());
				this.setCloseTime(pos.getCloseTime());
				this.setComment(pos.getComment());
				this.setDirection(pos.getDirection());
				this.setExpiration(pos.getExpiration());
				this.setStopLoss(pos.getInitialStopLoss());
				this.setTakeProfit(pos.getInitialTakeProfit());
				this.setOpenPrice(pos.getOpenPrice());
				this.setOpenTime(pos.getOpenTime());
				this.setPosState(pos.getPosState());
				this.setPosType(pos.getPosType());
				this.setProfit(pos.getProfit());
				this.setTicket(pos.getTicket());
				this.setTradeSymbol(pos.getTradeSymbol());
				this.setLots(pos.getLots());
				
		}
		
		@Override
		public void fromJson(JSONObject upper_level_obj) throws Exception {
			
			positionUid = HtMtJsonUtils.getLongJsonField(upper_level_obj, "POSITION_UID");
			companyName = HtMtJsonUtils.<String>getJsonField(upper_level_obj, "COMPANY_NAME");
			accountId = HtMtJsonUtils.<String>getJsonField(upper_level_obj, "ACCOUNT_ID");
			tradeSymbol = HtMtJsonUtils.<String>getJsonField(upper_level_obj, "SYMBOL");
			ticket = HtMtJsonUtils.<String>getJsonField(upper_level_obj, "TICKET");
			comment = HtMtJsonUtils.<String>getJsonField(upper_level_obj, "COMMENT");
			
			openTime = HtMtJsonUtils.getLongJsonField(upper_level_obj, "OPEN_TIME");
			closeTime = HtMtJsonUtils.getLongJsonField(upper_level_obj, "CLOSE_TIME");
			openPrice = HtMtJsonUtils.getLongJsonField(upper_level_obj, "OPEN_PRICE");
			closePrice = HtMtJsonUtils.getLongJsonField(upper_level_obj, "CLOSE_PRICE");
			
			stopLoss = HtMtJsonUtils.getLongJsonField(upper_level_obj, "SL");
			takeProfit = HtMtJsonUtils.getLongJsonField(upper_level_obj, "TP");
			profit =  HtMtJsonUtils.getLongJsonField(upper_level_obj, "PROFIT");
			expiration = HtMtJsonUtils.getLongJsonField(upper_level_obj, "EXPIRATION_TIME");
			direction = HtMtJsonUtils.getIntJsonField(upper_level_obj, "DIRECTION");
			posState = HtMtJsonUtils.getIntJsonField(upper_level_obj, "STATE");
			posType = HtMtJsonUtils.getIntJsonField(upper_level_obj, "TYPE");
			
			volume = HtMtJsonUtils.getLongJsonField(upper_level_obj, "LOTS");
		}
		
		@Override
		public void toJson(JSONObject upper_level_obj) throws Exception
		{
			
				super.toJson(upper_level_obj);
					
				upper_level_obj.put("POSITION_UID", positionUid);
				upper_level_obj.put("COMPANY_NAME", companyName);
				upper_level_obj.put("ACCOUNT_ID", accountId);
				upper_level_obj.put("SYMBOL", tradeSymbol);
				upper_level_obj.put("TICKET", ticket);
				upper_level_obj.put("COMMENT", comment);
				upper_level_obj.put("OPEN_TIME",openTime);
				upper_level_obj.put("CLOSE_TIME", closeTime);
				upper_level_obj.put("OPEN_PRICE", openPrice);
				upper_level_obj.put("CLOSE_PRICE", closePrice);
				upper_level_obj.put("SL", stopLoss);
				upper_level_obj.put("TP", takeProfit);
				upper_level_obj.put("PROFIT", profit);
				upper_level_obj.put("EXPIRATION_TIME", expiration);
				upper_level_obj.put("DIRECTION", direction);
				upper_level_obj.put("STATE", posState);
				upper_level_obj.put("TYPE", posType);
				upper_level_obj.put("LOTS", volume);
		
		}
		
		@Override
		public boolean routEventFor(String accountId, String companyName) throws Exception
		{
				if (HtUtils.nvl(accountId) || HtUtils.nvl(companyName))
						throw new HtException(getContext(), "routEventFor", "account id or company name invalid");
				
				return this.accountId.equals(accountId) && this.companyName.equals(companyName);
				
		}
		
}
