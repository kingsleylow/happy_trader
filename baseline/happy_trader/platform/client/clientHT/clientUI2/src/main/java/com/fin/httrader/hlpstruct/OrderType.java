/*
 * OrderType.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.hlpstruct;

import com.fin.httrader.utils.HtEnumeratorDescriptor;
import java.util.Iterator;
import java.util.Map;

/**
 *
 * @author Victor_Zoubok
 */
public class OrderType {

	public static final int OP_BUY = 0x0001;
	public static final int OP_BUY_LIMIT = 0x0004;
	public static final int OP_BUY_STOP = 0x0010;
	public static final int OP_CANCEL = 0x0200;    // cancel still not executed order
	//public static final int OP_CLOSE = 0x0100;    // close executed order
	public static final int OP_DUMMY = 0x0800;
	public static final int OP_HANDSHAKE = 0x0400;    // this is just handshake to get any specific response from broker
	public static final int OP_SELL = 0x0002;
	public static final int OP_SELL_LIMIT = 0x0008;
	public static final int OP_SELL_STOP = 0x0020;
	public static final int OP_STOPLOSS_SET = 0x0080;
	public static final int OP_TAKEPROFIT_SET = 0x0040;
	public static final int OP_RELATIVE_OPEN_PRICE = 0x1000;
	public static final int OP_GET_SYMBOL_METAINFO = 0x2000;
	public static final int OP_EXEC_PRICE_SET = 0x4000;
	public static final int OP_EXPIR_SET = 0x8000; // set expiration

	public static final int OP_BUY_STOP_LIMIT	=	0x10000;// buy stop limit
  public static final int OP_SELL_STOP_LIMIT =	0x20000;  // sell stop limit

	public static final int OP_CLOSE_SHORT	= 0x40000; // this closes short by market
	public static final int OP_CLOSE_LONG		= 0x80000; // this closes long by market

	public static final int OP_CLOSE_SHORT_LIMIT					= 0x100000; // this closes short as limit
	public static final int OP_CLOSE_LONG_LIMIT						= 0x200000; // this closes long as limit

	public static final int OP_CLOSE_SHORT_STOP						= 0x400000; // this closes short as stop
	public static final int OP_CLOSE_LONG_STOP						= 0x800000;  // this closes long as stop

	public static final int OP_CLOSE_SHORT_STOP_LIMIT			= 0x1000000; // this closes short as stop
	public static final int OP_CLOSE_LONG_STOP_LIMIT			= 0x2000000;  // this closes long as stop
	
	public static final int OP_ORDER_MOVE = 0x4000000;
	//
	private static HtEnumeratorDescriptor descriptor_m = new HtEnumeratorDescriptor("com.fin.httrader.hlpstruct.OrderType");

	

	// java specific
	public static String getTypeName(int typeId) {
		return descriptor_m.getNameById_BitWize(typeId);
	}

	public static Map<Integer, String> getNameMap()
	{
		return descriptor_m.getNameMap();
	}
}



