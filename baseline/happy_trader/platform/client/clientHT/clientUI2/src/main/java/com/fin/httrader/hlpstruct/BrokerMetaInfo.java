/*
 * BrokerMetaInfo.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.hlpstruct;

import com.fin.httrader.configprops.HtSaveAnnotation;
import java.io.Serializable;

/**
 *
 * @author Victor_Zoubok
 */
public class BrokerMetaInfo implements Serializable, Comparable<BrokerMetaInfo> {

	@Override
	public int compareTo(BrokerMetaInfo o) {
		StringBuilder result = new StringBuilder();
		result.append(symbol_m);

		result.append(pointValue_m);
		result.append(spread_m);
		result.append(signDigits_m);

		//
		StringBuilder oresult = new StringBuilder();
		oresult.append(o.symbol_m);
		oresult.append(o.pointValue_m);
		oresult.append(o.spread_m);
		oresult.append(o.signDigits_m);

		return result.toString().compareTo(oresult.toString());

	}
	// symbol that is unique for the defined broker - in numenator
	//public StringBuilder  symbolNum_m = new StringBuilder();
	// the symbol in denuminator
	//ublic StringBuilder  symbolDenom_m = new StringBuilder();
	
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
