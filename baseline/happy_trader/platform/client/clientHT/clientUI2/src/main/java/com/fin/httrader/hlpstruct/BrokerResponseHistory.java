/*
 * BrokerResponseHistory.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.hlpstruct;

import com.fin.httrader.configprops.HtSaveAnnotation;
import com.fin.httrader.utils.hlpstruct.HtRtData;
import java.util.ArrayList;
import java.util.List;
import java.util.Vector;

/**
 *
 * @author Victor_Zoubok
 */
public class BrokerResponseHistory extends BrokerResponseBase {

	public BrokerResponseHistory() {
		type_m = ResponseTypes.RT_BrokerResponseHistory;
	}
	// symbol that is unique for the defined broker - in numenator
	//public StringBuilder symbolNum_m = new StringBuilder();
	// the symbol in denuminator
	//public StringBuilder symbolDenom_m = new StringBuilder();
	
	@HtSaveAnnotation
	public StringBuilder symbol_m = new StringBuilder();
		
	// actual history
	@HtSaveAnnotation
	public List<HtRtData> history_m = new ArrayList<HtRtData>();
	
	// the significan digits number
	@HtSaveAnnotation
	public int signDigits_m = -1;
}
