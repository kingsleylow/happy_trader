/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.hlpstruct;

/**
 *
 * @author DanilinS
 */
public class BrokerResponseDelayedOrder  extends BrokerResponseBase {
	public BrokerResponseDelayedOrder()
	{
		type_m = ResponseTypes.RT_BrokerResponseDelayedOrder;
	}

	int delayedOrderType_m = DelayedOrderType.DO_DUMMY; 
	double execPrice_m = -1;
	Order order_m;
	int doType_m = DelayedOrderResponseType.DORT_DUMMY; 
}
