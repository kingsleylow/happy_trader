/*
 * BrokerResponseMetaInfo.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.hlpstruct;

import com.fin.httrader.configprops.HtSaveAnnotation;

/**
 *
 * @author Victor_Zoubok
 */
public class BrokerResponseMetaInfo extends BrokerResponseBase {

	/** Creates a new instance of BrokerResponseMetaInfo */
	public BrokerResponseMetaInfo() {
		type_m = ResponseTypes.RT_BrokerResponseMetaInfo;
	}
	
	// symbol that is unique for the defined broker - in numenator
	//@HtSaveAnnotation
	//public StringBuilder symbolNum_m = new StringBuilder();
	
	// the symbol in denuminator
	//@HtSaveAnnotation
	//public StringBuilder symbolDenom_m = new StringBuilder();
	
	@HtSaveAnnotation
	public StringBuilder symbol_m = new StringBuilder();
	
	// the value of one point for the pair
	@HtSaveAnnotation
	public double pointValue_m = -1;
	
	// spread
	@HtSaveAnnotation
	public double spread_m = -1;
	
	// the number of significant dogits
	@HtSaveAnnotation
	public int signDigits_m = -1;
}
