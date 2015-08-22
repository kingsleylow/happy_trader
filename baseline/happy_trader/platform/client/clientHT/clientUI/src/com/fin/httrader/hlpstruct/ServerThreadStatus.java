/*
 * ServerThreadStatus.java
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
public class ServerThreadStatus {

	public static final int TH_Dummy				= 0;
	public static final int TH_Lazy					= 1;
	public static final int TH_RT						= 2;
	public static final int TH_Simulation		= 3;
	public static final int TH_Failure			= 4;
	public static final int TH_Initializing = 5;

	private static HtEnumeratorDescriptor descriptor_m = new HtEnumeratorDescriptor("com.fin.httrader.hlpstruct.ServerThreadStatus");

	public static String getThreadStatus(int status_id) {
		return descriptor_m.getNameById_StrictValue(status_id);
	}
}
