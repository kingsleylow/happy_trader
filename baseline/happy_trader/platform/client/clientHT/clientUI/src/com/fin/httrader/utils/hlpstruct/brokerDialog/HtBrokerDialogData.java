/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.utils.hlpstruct.brokerDialog;

import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.Uid;
import java.util.ArrayList;
import java.util.List;
import java.util.SortedMap;
import java.util.TreeMap;

/**
 *
 * @author Victor_Zoubok
 * main structure keeping all orders and responses in order
 */
public class HtBrokerDialogData {
	
	private String getContext() {
		return this.getClass().getSimpleName();

	}
	
	public HtBrokerDialogData()
	{
	}
	
	// orders are inserted first
	public void addOrderEntry(
					Uid orderUid, 
					Uid sessionUid, 
					int seqNum, 
					int orderType, 
					String symbol, 
					String provider, 
					long orderIssueTime, 
					long orderOperationTime
	)
	{
		// orders are inserted due 
		HtOrderEntry oe = new HtOrderEntry(symbol, provider, orderType, orderUid, sessionUid,seqNum,orderIssueTime,orderOperationTime);
		fullData_m.put(createKey(seqNum), oe);
	}
	
	// responses are inserted later
	// so it's expected all orders are inserted!
	public void addResponseEntryWithChildRef(
				Uid responseUid, 
				Uid sessionUid, 
				int seqNum, 
				int orderType, 
				String symbol, 
				String provider, 
				long responseIssueTime, 
				long responseOperationTime,
				Uid parentOrderUid, // can be null
				int parentSeqNum, // can be ==-1
				int resultCode,
				int resultDetailCode
	) throws Exception
	{
		HtOrderResponseEntry be = new HtOrderResponseEntry(symbol, provider, orderType, responseUid, 
						sessionUid,seqNum,responseIssueTime,responseOperationTime,parentOrderUid,parentSeqNum, resultCode, resultDetailCode);
		
		
		
		if (parentOrderUid != null && parentOrderUid.isValid() && parentSeqNum >= 0) {
			// need to find parent id
			String parentOrderKey = createKey(parentSeqNum);
			String responseKey = createKey(parentSeqNum, seqNum);
			
			HtBaseEntry po = fullData_m.get(parentOrderKey);
			if (po == null)
				throw new HtException(getContext(), "addResponseEntryWithChildRef", "Cannot find parent order with id: " + parentOrderUid.toString());
			
			HtOrderEntry po_c  = po.<HtOrderEntry>getThisEntry();
			po_c.addChildResponseRef(be);
			
			// add to the base map
			//fullData_m.put(responseKey, be);
			
			// only responses
			responsesOnly_m.put(seqNum, be);
			
			
		}
		else {
			throw new HtException(getContext(), "addResponseEntryWithChildRef", "No parent reference provided by response with id: " + responseUid.toString());
		}
	}
	
	// assume this is the last function - e.i. all responses were inserted
	public void addResponseEntryWithoutChildRef(
			Uid responseUid, 
			Uid sessionUid, 
			int seqNum, 
			int orderType, 
			String symbol, 
			String provider, 
			long responseIssueTime, 
			long responseOperationTime,
			int resultCode,
			int resultDetailCode
	)
		throws Exception
	{
		HtOrderResponseEntry be = new HtOrderResponseEntry(symbol, provider, orderType, responseUid, 
						sessionUid,seqNum,responseIssueTime,responseOperationTime,null,-1, resultCode, resultDetailCode);
		
		// now need to iterate through all list
		// and see the nearest response with seq num which is less
		if (responsesOnly_m.isEmpty()) {
			// no responses - ours will be the first
			// and it's going to be beginning from zeroes... as we assume no other 
			//  parent responses will be here
			fullData_m.put(createKey(0,seqNum ), be);
		} else {
		
			// seqNum is not included
			SortedMap<Integer, HtOrderResponseEntry> smap = responsesOnly_m.subMap(responsesOnly_m.firstKey(), seqNum);
			
			if (smap.isEmpty()) {
				// nothing to see - just add
				fullData_m.put(createKey(0,seqNum ), be);
			} else {
				// look backward
				
				Integer[] smap_arr = smap.keySet().toArray(new Integer[smap.size()]); 
				
				
				for(int j = smap_arr.length - 1; j>=0; j--) {
					HtOrderResponseEntry e_j = smap.get( smap_arr[j] );
					
					// find the first element which can be bound
					if (e_j.parentUid_m != null && e_j.parentUid_m.isValid() && e_j.parentSeqNum_m >=0) {
						String fullKey = createKey(e_j.parentSeqNum_m, seqNum);
						fullData_m.put(fullKey, be);
						
						break;
					}
				}
				
			}
		
		}
		// only responses
		responsesOnly_m.put(seqNum, be);
	}
	
	public void doFinish()
	{
		responsesOnly_m.clear();
	}
	
	public void clear()
	{
		fullData_m.clear();
		responsesOnly_m.clear();
	}
	
	public List<HtBaseEntry> getDataList()
	{
		ArrayList<HtBaseEntry> r = new ArrayList<HtBaseEntry>(fullData_m.values());
		return r;
	}
	
	public String dump()
	{
		StringBuilder r = new StringBuilder();
		List<HtBaseEntry> rl = getDataList();
		
		for(int i = 0; i < rl.size(); i++) {
			HtBaseEntry be = rl.get(i);
			r.append("Entry #: ").append(i).append("->\n");
			r.append(be.toString());
			r.append("-------------------------------\n");
			
		}
		
		return r.toString();
	}
	
	/**
	 * 
	 * Helpers
	 */
	
	// key for response
	private String createKey(int seqNumOrder, int seqNumResp)
	{
		
		StringBuilder r = new StringBuilder(String.format("%016d", seqNumOrder)).append(String.format("%016d", seqNumResp)); 
		return r.toString();
	}
	
	// key for order
	private String createKey(int seqNumOrder)
	{
		StringBuilder r = new StringBuilder(String.format("%016d", seqNumOrder)).append(String.format("%016d", 0)); 
		return r.toString();
	}
					
	// this is a global list which must be ordered
	private TreeMap<String,HtBaseEntry> fullData_m = new TreeMap<String, HtBaseEntry>();
	
	// only responses
	private TreeMap<Integer,HtOrderResponseEntry > responsesOnly_m = new TreeMap<Integer,HtOrderResponseEntry >();
	
}
