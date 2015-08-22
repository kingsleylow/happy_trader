/*
 * AlertPriorities.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.hlpstruct;

import com.fin.httrader.utils.HtEnumeratorDescriptor;
import java.util.Map;
import java.util.Set;

/**
 *
 * @author Victor_Zoubok
 */
public class AlertPriorities {

	public static final int AP_LOW =			3;
	public static final int AP_MEDIUM =		2;
	public static final int AP_HIGH =			1;
	public static final int AP_CRITICAL =	0;

	private static HtEnumeratorDescriptor descriptor_m = new HtEnumeratorDescriptor("com.fin.httrader.hlpstruct.AlertPriorities");


	public static String getAlertpriorityByName(int pt) {
		return descriptor_m.getNameById_StrictValue(pt);
	}

	public static Set<Integer> getAllAlertPriorities() {
		return descriptor_m.getIdSet();
	}

	public static Map<Integer, String> getAllAlertPrioritiesMap() {
		return descriptor_m.getNameMap();
	}
}
