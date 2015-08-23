/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.hlpstruct;

import com.fin.httrader.utils.HtEnumeratorDescriptor;

/**
 *
 * @author DanilinS
 */
public class RtDataPrintColor {

		// color type
	static public final int CT_NA = 0;
	static public final int CT_Bid_Deal = 1;
	static public final int CT_Ask_Deal = 100;
	static public final int CT_Neutral = 50;

	private static HtEnumeratorDescriptor descriptor_m = new HtEnumeratorDescriptor("com.fin.httrader.hlpstruct.RtDataPrintColor");
	
	// resolve color constants to name
	public static String getColorTypeName(int color) {
		return descriptor_m.getNameById_StrictValue(color, String.valueOf(color));
		
	}
}
