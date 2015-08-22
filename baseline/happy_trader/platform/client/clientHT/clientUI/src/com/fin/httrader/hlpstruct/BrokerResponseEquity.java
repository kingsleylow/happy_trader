/*
 * BrokerResponseEquity.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.hlpstruct;

/**
 *
 * @author Victor_Zoubok
 */
public class BrokerResponseEquity extends BrokerResponseBase {

	public BrokerResponseEquity() {
		type_m = ResponseTypes.RT_BrokerResponseEquity;
	}
	// position broker ID
	public StringBuilder brokerPositionID_m = new StringBuilder();
	// this is exchange rate to calculate profit in report currency
	public double curSmbNumSmbRepExchangeRate_m = -1;
	// current profit (equity) in denominator currency
	public double curPositionDenomProfit_m = -1;
	// current position profit (equity) in report currency
	public double curPositionReportProfit_m = -1;
	// comission / charges in Denominator currency
	public double opChargeDenomCurrency_m = -1;
	// commisions charges in % from position
	public double opChargeOperationPct_m = -1;
	// number of significant digits when perfroming calculations
	public int signDigits_m = -1;
}
