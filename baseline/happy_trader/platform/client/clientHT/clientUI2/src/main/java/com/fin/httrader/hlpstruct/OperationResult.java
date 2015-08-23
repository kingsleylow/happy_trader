/*
 * OperationResult.java
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
public class OperationResult {

	// whethere order was installed or executed
	public static final int PT_DUMMY = 0;
	public static final int PT_ORDER_ESTABLISHED = 1;
	public static final int PT_CANNOT_ESTABLISH_ORDER = 2;
	// whether order was modifies
	public static final int PT_ORDER_MODIFIED = 3;
	public static final int PT_CANNOT_MODIFY_ORDER = 4;

	// whether position was closed
	//public static final int PT_ORDER_CLOSED = 5;
	//public static final int PT_CANNOT_CLOSE_ORDER = 6;

	// whether position was cancelled
	public static final int PT_ORDER_CANCELLED = 7;

	public static final int PT_CANNOT_CANCEL_ORDER = 8;

	// this happens on overnight and other situations when position is extra-charged
	// or commission is charged - whatever when some money are added or substructed to/from your account
	public static final int PT_POSITION_CHARGED = 9;

	public static final int PT_GENERAL_INFO = 10;    // general info
	
	public static final int PT_CURRENT_QUOTES = 11;    // current bid/ask price
	public static final int PT_DISCONNECTED = 12;    // broker layer disconnected the client
	
	public static final int PT_GENERAL_ERROR_HAPPENED = 13;    // some general error happened

	// any kind of information message from the broker not changing current position

	public static final int PT_NEWS_ARRIVED = 14;    // news arrived
	public static final int PT_HANDSHAKEARRIVED = 15;
	
	public static final int PT_REGISTERED_DELAYED_ORDER		= 16;
	public static final int PT_UNREGISTERED_DELAYED_ORDER	= 17;
	public static final int PT_DELAYED_ORDER_GOING_TO_EXECUTE	= 18;

	public static final int PT_POSITION_CLOSED								= 19;
	public static final int PT_CANNOT_CLOSE_POSITION					= 20;

	public static final int PT_POSITION_MODIFIED							= 21;
	public static final int PT_CANNOT_MODIFY_POSITION					= 22;

	// ------------------------
	
	private static HtEnumeratorDescriptor descriptor_m = new HtEnumeratorDescriptor("com.fin.httrader.hlpstruct.OperationResult");

	
	// java specific
	public static String getOperationResultName(int opId) {
		return descriptor_m.getNameById_StrictValue(opId);
	}
	
}



