/*
 * TSGenerateTickMethod.java
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
public class TSGenerateTickMethod {

	public static final int TG_Dummy = 0;
	public static final int TG_Uniform = 1;
	public static final int TG_CloseOnly = 2;
	public static final int TG_OHLC = 3;

	private static HtEnumeratorDescriptor descriptor_m = new HtEnumeratorDescriptor("com.fin.httrader.hlpstruct.TSGenerateTickMethod");

	public static String getGenerateTickMethodNameName(int ap)
	{
		return descriptor_m.getNameById_StrictValue(ap);
	}

	public static Set<Integer> getAllGenerateTickMethods()
	{
		return descriptor_m.getIdSet();
	}
}
