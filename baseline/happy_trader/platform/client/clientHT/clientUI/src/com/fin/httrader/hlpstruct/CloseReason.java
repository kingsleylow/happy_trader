/*
 * CloseReason.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.hlpstruct;

import com.fin.httrader.utils.HtEnumeratorDescriptor;

/**
 *
 * @author Victor_Zoubok
 */
public class CloseReason {

	public static final int REASON_DUMMY = 0;
	public static final int REASON_NORMAL = 1; // normal close of position because of command
	public static final int REASON_SL = 2;
	public static final int REASON_TP = 3;
	public static final int REASON_CANCEL = 4;
	public static final int REASON_MARGINE_CALL = 5;
	public static final int REASON_ERROR = 6;
	public static final int REASON_EXPIRATION = 7;

	private static HtEnumeratorDescriptor descriptor_m = new HtEnumeratorDescriptor("com.fin.httrader.hlpstruct.CloseReason");

	public static String getCloseReasonName(int pt) {
		return descriptor_m.getNameById_StrictValue(pt);
	}
}
