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
 * @author DanilinS
 * price constants - they live in Order price field
 */
public class PriceConst {
	public static final int PC_ZERO =			0; // price is zero
	public static final int PC_BEST_BID = 1; // best bid
	public static final int PC_BEST_ASK = 2;  // best ask
	public static final int PC_INPUT =		3;

	private static HtEnumeratorDescriptor descriptor_m = new HtEnumeratorDescriptor("com.fin.httrader.hlpstruct.PriceConst");

	

	
	public static String getPriceConstName(int ap) {
		return descriptor_m.getNameById_StrictValue(ap);
		
	}

	public static int getPriceConstId(String name)
	{
		return descriptor_m.getIdByName_StrictValue(name, PC_ZERO);

		
	}

	public  static Set<Integer> getAllPriceConsts() {
		return descriptor_m.getIdSet();
	}

	public static Map<Integer, String> getPriceConstsNamesMap() {
		return descriptor_m.getNameMap();
	}
}
