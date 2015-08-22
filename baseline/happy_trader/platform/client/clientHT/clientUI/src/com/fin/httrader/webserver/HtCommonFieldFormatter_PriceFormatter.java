/*
 * HtCommonFieldFormatter_PriceFormatter.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.interfaces.HtCommonRowSelector;
import com.fin.httrader.utils.HtUtils;

/**
 *
 * @author Victor_Zoubok
 * The class intended to perfrom double formatting according another field sign digits value
 */
public class HtCommonFieldFormatter_PriceFormatter implements HtCommonRowSelector.HtCommonFieldFormatter {

	private static final String format_m = "%.4f";
	private static HtCommonFieldFormatter_PriceFormatter instance_m = null;
	private static Object mtx_m = new Object();
	
	private HtCommonFieldFormatter_PriceFormatter() {
		
	}

	public static HtCommonFieldFormatter_PriceFormatter getInstance() {
		synchronized (mtx_m) {
			if (instance_m == null) {
				instance_m = new HtCommonFieldFormatter_PriceFormatter();
			}

			return instance_m;
		}
	}

	public String format(HtCommonRowSelector.CommonRow row, String sval) {
	
		return String.format(format_m, HtUtils.parseDouble(sval));
	}
}
