/*
 * TSAggregationPeriod.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.hlpstruct;

import com.fin.httrader.utils.HtEnumeratorDescriptor;
import java.util.Set;

/**
 *
 * @author Victor_Zoubok
 * Trading server aggregation period
 */
public class TSAggregationPeriod {

	public static final int AP_Dummy = 0;
	public static final int AP_NoAggr = 1;
	public static final int AP_Minute = 2;
	public static final int AP_Hour = 3;
	public static final int AP_Day = 4;
	public static final int AP_Week = 5;
	public static final int AP_Month = 6;
	public static final int AP_Year = 7;

	private static HtEnumeratorDescriptor descriptor_m = new HtEnumeratorDescriptor("com.fin.httrader.hlpstruct.TSAggregationPeriod");

	
	public static String getAggregationPeriodName(int ap) {
		return descriptor_m.getNameById_StrictValue(ap);
	}

	public static int getAggregationPeriodFromName(String aggrPeriod)
	{
		return descriptor_m.getIdByName_StrictValue(aggrPeriod, AP_Dummy);
	}

	
	public static Set<Integer> getAllAggregationPeriods() {
		return descriptor_m.getIdSet(AP_Dummy);
	}

	public static boolean isValid(int aggrPeriod) {
		return descriptor_m.getIdSet().contains(aggrPeriod);
	}
}
