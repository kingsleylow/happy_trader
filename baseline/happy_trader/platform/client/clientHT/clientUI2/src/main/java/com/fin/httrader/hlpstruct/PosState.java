/*
 * PosState.java
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
public class PosState {

	public static final int STATE_DUMMY = 0;
	public static final int STATE_OPEN = 2;
	public static final int STATE_HISTORY = 3;

	private static HtEnumeratorDescriptor descriptor_m = new HtEnumeratorDescriptor("com.fin.httrader.hlpstruct.PosState");

	public static String getPosStateName(int pt) {
		return descriptor_m.getNameById_StrictValue(pt);
		
	}
}


