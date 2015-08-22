/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.hlpstruct;

import com.fin.httrader.configprops.HtSaveAnnotation;

/**
 *
 * @author Victor_Zoubok
 */
public class BrokerResponseClosePos extends BrokerResponseBase {
	public BrokerResponseClosePos() {
		type_m = ResponseTypes.RT_BrokerResponseClosePos;
	}
	
	@HtSaveAnnotation
	public StringBuilder provider_m = new StringBuilder();

	@HtSaveAnnotation
	public StringBuilder  symbol_m = new StringBuilder();

	@HtSaveAnnotation
	public int volume_m = -1;

	@HtSaveAnnotation
	public double priceOpen_m = -1;

	@HtSaveAnnotation
	public double priceClose_m = -1;
	
	@HtSaveAnnotation
	public double posOpenTime_m = -1;

	@HtSaveAnnotation
	public double posCloseTime_m = -1;

	@HtSaveAnnotation
	public StringBuilder brokerPositionID_m = new StringBuilder();

	@HtSaveAnnotation
	public int tradeDirection_m = TradeDirection.TD_NONE;
	
	@HtSaveAnnotation
	public double comissionPct_m = 0;

	@HtSaveAnnotation
	public double comissionAbs_m = 0;
	
}
