/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.utils.hlpstruct;

import java.util.ArrayList;
import java.util.List;

/**
 *
 * @author Victor_Zoubok
 */
public class HtRunNameEntry {

	public static class HtSessionEntry {
		
		public HtSessionEntry()
		{
		}
		
		public HtSessionEntry(String sessionId, int opCount, int respCount, int closePosCount, long id)
		{
			sessionId_m = sessionId;
			opCount_m = opCount;
			respCount_m = respCount;
			closePosCount_m = closePosCount;
			id_m = id;
		}
		
		public String sessionId_m;
		
		public int opCount_m = -1;
		
		public int respCount_m = -1;
		
		public int closePosCount_m = -1;
		
		public long id_m = -1;
		
	};
	
	public HtRunNameEntry(String runName, long opFirstTime)
	{
		runName_m = runName;
		opFirstTime_m = opFirstTime;
		
	}
	
	public String runName_m = null; 
	public long opFirstTime_m = -1;
	
	public List<HtSessionEntry> sessions_m = new ArrayList<HtSessionEntry>();
	
	
}
