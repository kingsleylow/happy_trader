/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.hlpstruct;

import com.fin.httrader.utils.HtEnumeratorDescriptor;
import java.util.Arrays;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
import java.util.TreeSet;

/**
 *
 * @author Victor_Zoubok
 */
public class OrderValidity {
	public static final int OV_DUMMY = 0;
	public static final int OV_GTC = 1;
	public static final int OV_DAY = 2;

	private static HtEnumeratorDescriptor descriptor_m = new HtEnumeratorDescriptor("com.fin.httrader.hlpstruct.OrderValidity");

	
	

	public static String getOrderValidityName(int ov_id) {
		return descriptor_m.getNameById_StrictValue(ov_id);
	}

	public static int getOrderValidityId(String posTypeName) {
		return descriptor_m.getIdByName_StrictValue(posTypeName, OV_DUMMY);
	}

	public static Set<Integer> getAllOrderValidities()
	{
		return descriptor_m.getIdSet();
	}

	public static Map<Integer, String> getAllValiditiesMap() {
		return descriptor_m.getNameMap();
	}
	
}
