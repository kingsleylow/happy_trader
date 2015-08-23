/*
 * Order.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.hlpstruct;

import com.fin.httrader.configprops.HtSaveAnnotation;
import com.fin.httrader.utils.Uid;

/**
 *
 * @author Victor_Zoubok
 */
public class Order {

	public Order() {
		// generate ID
		ID_m.generate();
	}

	// native ID
	@HtSaveAnnotation
	public Uid ID_m = new Uid();

	// broker order ID
	@HtSaveAnnotation
	public StringBuilder brokerPositionID_m = new StringBuilder();

	@HtSaveAnnotation
	public StringBuilder brokerOrderID_m = new StringBuilder();

	// RT provider
	@HtSaveAnnotation
	public StringBuilder provider_m = new StringBuilder();
	
	@HtSaveAnnotation
	public StringBuilder symbol_m = new StringBuilder();
	
	// comment
	@HtSaveAnnotation
	public StringBuilder comment_m = new StringBuilder();

	@HtSaveAnnotation
	public int orderValidity_m = OrderValidity.OV_DUMMY;

	// expiration time for non-executed orders
	@HtSaveAnnotation
	public double orExpirationTime_m = -1;

	// price for market orders
	@HtSaveAnnotation
	public double orPrice_m = -1;

	// for STOP or STOP lIMKT orders this is STOP price
	@HtSaveAnnotation
	public double orStopPrice_m = -1;

	// initial stop loss price
	@HtSaveAnnotation
	public double orSLprice_m = -1;

	// initial take profit price
	@HtSaveAnnotation
	public double orTPprice_m = -1;

	// order issue time
	@HtSaveAnnotation
	public double orderIssueTime_m = -1;
	
	// order time
	@HtSaveAnnotation
	public double orTime_m = -1;

	@HtSaveAnnotation
	public int orVolume_m = -1;

	// type of order
	@HtSaveAnnotation
	public int orderType_m = OrderType.OP_DUMMY;

	@HtSaveAnnotation
	public int orPriceType_m = PriceConst.PC_ZERO;

	@HtSaveAnnotation
	public int orStopPriceType_m = PriceConst.PC_ZERO;

	// for stock orders only
	@HtSaveAnnotation
	public StringBuilder place_m = new StringBuilder();

	@HtSaveAnnotation
	public StringBuilder method_m = new StringBuilder();

	@HtSaveAnnotation
	public StringBuilder additionalInfo_m = new StringBuilder();
	
	// context in Java layer is only a string
	@HtSaveAnnotation
	public StringBuilder context_m = new StringBuilder();
}



