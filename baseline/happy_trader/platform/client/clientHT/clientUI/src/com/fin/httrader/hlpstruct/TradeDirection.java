/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.hlpstruct;

import com.fin.httrader.utils.HtEnumeratorDescriptor;
import java.util.Set;

/**
 *
 * @author Victor_Zoubok
 */
public class TradeDirection {
	public static final int TD_NONE = 0;
	public static final int TD_LONG = 1;
	public static final int TD_SHORT = 2;
	public static final int TD_BOTH = 3;

	private static HtEnumeratorDescriptor descriptor_m = new HtEnumeratorDescriptor("com.fin.httrader.hlpstruct.TradeDirection");
	
	public static String getTradeDirectionName(int ap) {
		return descriptor_m.getNameById_StrictValue(ap);
	}

	public static Set<Integer> getAllTradeDirectionTypes() {
		return descriptor_m.getIdSet();
	}
}
