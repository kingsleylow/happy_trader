/*
 * TickUsage.java
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
 */
public class TickUsage {

	public static final int PC_USE_DUMMY = 0;
	public static final int PC_USE_BID = 1;
	public static final int PC_USE_ASK = 2;
	public static final int PC_USE_BOTH = 3;

	private static HtEnumeratorDescriptor descriptor_m = new HtEnumeratorDescriptor("com.fin.httrader.hlpstruct.TickUsage");

	
	public static String getTickUsageName(int ap) {
		return descriptor_m.getNameById_StrictValue(ap);
	}

	public static Set<Integer> getAllTickUsageTypes() {
		return descriptor_m.getIdSet();
	}
}
