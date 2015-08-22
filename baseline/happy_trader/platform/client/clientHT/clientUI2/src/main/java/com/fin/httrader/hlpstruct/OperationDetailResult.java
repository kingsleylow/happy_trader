/*
 * OperationDetailResult.java
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
public class OperationDetailResult {

	public static final int ODR_CANNOT_MODIFY_ORDER				= 0x00020000;
	public static final int ODR_DUMMY											= 0x00000000;
	public static final int ODR_ERROR_REQUOTE							= 0x00002000;
	public static final int ODR_EXPIRATION_CHANGED_OK			= 0x00000080;
	public static final int ODR_GENERAL_ERROR							= 0x00000800;
	public static final int ODR_INVALID_ACCOUNT						= 0x00004000;
	public static final int ODR_INVALID_EXPIRATION				= 0x00000010;
	public static final int ODR_INVALID_PRICE							= 0x00000001;
	public static final int ODR_INVALID_STOPLOSS_PRICE		= 0x00000004;
	public static final int ODR_INVALID_TAKEPROFIT_PRICE	= 0x00000008;
	public static final int ODR_INVALID_VOLUME						= 0x00000002;
	public static final int ODR_LONG_ONLY									= 0x00010000;
	public static final int ODR_MARGINE_CALL							= 0x00000100;
	public static final int ODR_MARKET_CLOSED							= 0x00080000;
	public static final int ODR_OK												= 0x00100000;
	public static final int ODR_ORDER_EXECUTED						= 0x00000400;
	public static final int ODR_ORDER_INSTALLED						= 0x00000200;
	public static final int ODR_SERVER_BUSY								= 0x00040000;
	public static final int ODR_SHORT_ONLY								= 0x00008000;
	public static final int ODR_STOPLOSS_CHANGED_OK				= 0x00000020;
	public static final int ODR_TAKEPROFIT_CHANGED_OK			= 0x00000040;
	public static final int ORD_NOT_SUPPORTED							= 0x00200000;
	public static final int ORD_INVALID_ORDER							= 0x00400000;
	public static final int ORD_EXECUTE_PRICE_CHANGED_OK	= 0x00800000;

	public static final int ODR_INVALID_STOP_PRICE				=	0x01000000;  // stop price was not accepted
	public static final int ODR_STOP_PRICE_CHANGED_OK			= 0x02000000;  // when installing stop orders this shows that stop was accepted

	public static final int ODR_ORDER_PENDING							= 0x04000000;
	public static final int ODR_ORDER_SUBMITTED						= 0x08000000;

	private static HtEnumeratorDescriptor descriptor_m = new HtEnumeratorDescriptor("com.fin.httrader.hlpstruct.OperationDetailResult");

	public static String getOperationDetailresultName(int opdid) {
		return descriptor_m.getNameById_BitWize(opdid);
	}
}

