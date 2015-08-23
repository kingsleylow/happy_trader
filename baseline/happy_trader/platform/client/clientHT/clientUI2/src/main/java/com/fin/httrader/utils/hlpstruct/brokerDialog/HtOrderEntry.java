/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.utils.hlpstruct.brokerDialog;

import com.fin.httrader.hlpstruct.OrderType;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.Uid;
import java.util.*;

/**
 *
 * @author Victor_Zoubok
 */
public class HtOrderEntry extends HtBaseEntry {

	@Override
	public int getType() {
		return TYPE_ORDER;
	}

	public HtOrderEntry() {
	}

	public HtOrderEntry(
					String symbol, 
					String provider, 
					int orderType, 
					Uid id, 
					Uid SessionId, 
					int seqNum, 
					long orderIssueTime, 
					long orderOperationTime
			) {
		super(symbol, provider, orderType, id, SessionId, seqNum);
		orderIssueTime_m = orderIssueTime;
		orderOperationTime_m = orderOperationTime;

	}

	public void addChildResponseRef(HtOrderResponseEntry re) {
		responses_m.put(re.seq_id_m, re);
	}
	
	public List<HtOrderResponseEntry> getChildDataList()
	{
		ArrayList<HtOrderResponseEntry> r = new ArrayList<HtOrderResponseEntry>(responses_m.values());
		return r;
	}

	@Override
	public String toString() {
		StringBuilder r = new StringBuilder(super.toString());
		r.append("Order Issue Time: ").append(HtDateTimeUtils.time2String_Gmt(orderIssueTime_m)).append("\n");
		r.append("Order Operation Time: ").append(HtDateTimeUtils.time2String_Gmt(orderOperationTime_m)).append("\n");
		List<HtOrderResponseEntry> children = getChildDataList();
						
		for (int i = 0; i < children.size(); i++) {
			r.append("  --- Dependent response ID: ").append(children.get(i)).append("------\n");
		}

		return r.toString();
	}
	// 
	// these are child responses 
	// note that not all responses have parents
	private TreeMap<Integer, HtOrderResponseEntry> responses_m = new TreeMap<Integer, HtOrderResponseEntry>();
	
	public long orderIssueTime_m = -1;
	public long orderOperationTime_m = -1;
}
