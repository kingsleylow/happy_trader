/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.utils.hlpstruct.brokerDialog;

import com.fin.httrader.hlpstruct.OrderType;
import com.fin.httrader.utils.Uid;

/**
 *
 * @author Victor_Zoubok
 */
public abstract class HtBaseEntry 
	{
		public static final int TYPE_ORDER = 1;
		public static final int TYPE_ORDER_RESPONSE = 2;
		
		
		public HtBaseEntry()
		{
		}
		
		public HtBaseEntry(String symbol, String provider, int orderType, Uid id, Uid SessionId, int seqNum)
		{
			this.id_m = id;
			this.sessionId_m = SessionId;
			this.seq_id_m = seqNum;
			this.orderType_m = orderType;
			this.provider_m = provider;
			this.symbol_m = symbol;
			
			
		}
			
		
		@Override
		public String toString()
		{
			StringBuilder r = new StringBuilder();
			r.append("ID: ").append(id_m.toString()).append("\n");
			r.append("Session ID: ").append(sessionId_m).append("\n");
			r.append("Sequence Num: ").append(seq_id_m).append("\n");
			r.append("Order type: ").append(OrderType.getTypeName(orderType_m)).append("\n");
			r.append("Provider: ").append(provider_m).append("\n");
			r.append("Symbol: ").append(symbol_m).append("\n");
		
			return r.toString();
		}
		
		public abstract int getType();
		
		public <T> T getThisEntry()
		{
			return (T)this;
		}
		
		public String symbol_m = null;
		public String provider_m = null;
		public int orderType_m = OrderType.OP_DUMMY;
		
		// main id 
		public Uid id_m = null;
		
		// session id
		public Uid sessionId_m = null;
		
		// sequence - must be ordered using that id
		public int seq_id_m;
		
		
	};
