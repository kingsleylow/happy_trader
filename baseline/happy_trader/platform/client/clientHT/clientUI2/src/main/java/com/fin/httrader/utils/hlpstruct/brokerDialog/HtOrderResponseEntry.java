/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.utils.hlpstruct.brokerDialog;

import com.fin.httrader.hlpstruct.OperationDetailResult;
import com.fin.httrader.hlpstruct.OperationResult;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.Uid;
import java.util.Iterator;

/**
 *
 * @author Victor_Zoubok
 */
public class HtOrderResponseEntry extends HtBaseEntry{
	@Override
		public int getType() {
			return TYPE_ORDER_RESPONSE;
		}
	
	public HtOrderResponseEntry()
	{
	}
	
	public HtOrderResponseEntry(
					String symbol, 
					String provider, 
					int orderType, 
					Uid id, 
					Uid SessionId, 
					int seqNum, 
					long responseIssueTime, 
					long responseOperationTime, 
					Uid parentUid, 
					int parentSeqNum,
					int resultCode,
					int resultDetailCode
	)
	{
		super(symbol, provider, orderType, id, SessionId, seqNum);
		responseOperationTime_m = responseOperationTime;
		responseIssueTime_m = responseIssueTime;
		parentUid_m = parentUid;
		parentSeqNum_m = parentSeqNum;
		resultCode_m = resultCode;
		resultDetailCode_m = resultDetailCode;
		
	}
	
	@Override
	public String toString() {
		StringBuilder r = new StringBuilder(super.toString());
		r.append("Response Issue Time: ").append(HtDateTimeUtils.time2String_Gmt(responseIssueTime_m)).append("\n");
		r.append("Response Operation Time: ").append(HtDateTimeUtils.time2String_Gmt(responseOperationTime_m)).append("\n");
		r.append("Parent UID: ").append(parentUid_m != null ? parentUid_m.toString():"N/A").append("\n");
		r.append("Parent Sequence Num: ").append(parentSeqNum_m).append("\n");
		r.append("Result Code: ").append(OperationResult.getOperationResultName(resultCode_m)).append("\n");
		r.append("Result Detail Code: ").append(OperationDetailResult.getOperationDetailresultName(resultDetailCode_m)).append("\n");

		return r.toString();
	}
	
	// reference to parent object
	public Uid parentUid_m = null;
	
	public int parentSeqNum_m = -1;
	
	// operation time returned by broker
	public long responseIssueTime_m = -1;
	
	public long responseOperationTime_m = -1;
	
	public int resultCode_m = OperationResult.PT_DUMMY;
	
	public int resultDetailCode_m = OperationDetailResult.ODR_DUMMY;
}
