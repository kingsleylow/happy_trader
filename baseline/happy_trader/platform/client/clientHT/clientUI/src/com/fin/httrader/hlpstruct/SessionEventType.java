/*
 * SessionEventType.java
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
public class SessionEventType {

	public static final int SET_Dummy = 0;
	public static final int SET_Create = 1;
	public static final int SET_Connect = 2;
	public static final int SET_Disconnect = 3;
	public static final int SET_Destroy = 4;

	private static HtEnumeratorDescriptor descriptor_m = new HtEnumeratorDescriptor("com.fin.httrader.hlpstruct.SessionEventType");

	public static String getSessionEventTypeName(int ap) {
		return descriptor_m.getNameById_StrictValue(ap);
	}

}
