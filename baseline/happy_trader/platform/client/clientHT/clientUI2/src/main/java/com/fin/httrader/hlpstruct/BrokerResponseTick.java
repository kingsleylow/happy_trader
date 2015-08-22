/*
 * BrokerResponseTick.java
 *
*
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.hlpstruct;

import com.fin.httrader.hlpstruct.BrokerResponseBase;
import com.fin.httrader.configprops.HtSaveAnnotation;

/**
 *
 * @author Victor_Zoubok
 */
public class BrokerResponseTick extends BrokerResponseBase {

	public BrokerResponseTick() {
		type_m = ResponseTypes.RT_BrokerResponseTick;
	}
	// symbol that is unique for the defined broker - in numenator
	//public StringBuilder symbolNum_m = new StringBuilder();
	// the symbol in denuminator
	//public StringBuilder symbolDenom_m = new StringBuilder();
	
	@HtSaveAnnotation
	public StringBuilder symbol_m = new StringBuilder();
	
	// current prices & open interest
	@HtSaveAnnotation
	public double opBid_m = -1;
	
	@HtSaveAnnotation
	public double opAsk_m = -1;
	
	@HtSaveAnnotation
	public double opOpInt_m = -1;
	
	
	// the significan digits number
	@HtSaveAnnotation
	public int signDigits_m = -1;
}
