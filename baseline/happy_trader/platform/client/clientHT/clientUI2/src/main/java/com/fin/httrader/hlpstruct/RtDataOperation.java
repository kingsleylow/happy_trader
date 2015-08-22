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
public class RtDataOperation {
	static public final int OT_NA			= 0;
	static public final int OT_Buy		= 1;
	static public final int OT_Sell		= 2;
	static public final int OT_Short	= 3;
	static public final int OT_Cover	= 4;

	private static HtEnumeratorDescriptor descriptor_m = new HtEnumeratorDescriptor("com.fin.httrader.hlpstruct.RtDataOperation");

	// resolve operation constants to name
	public static String getOperationTypeName(int operation) {
		return descriptor_m.getNameById_StrictValue(operation, String.valueOf(operation));
		
	}
}
