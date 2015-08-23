/*
 * HtCommonFieldFormatter_Boolean.java
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
 */
public class HtCommonFieldFormatter_Boolean implements HtCommonRowSelector.HtCommonFieldFormatter {

	private static HtCommonFieldFormatter_Boolean instance_m = null;
	private static Object mtx_m = new Object();

	// no public ctor
	private HtCommonFieldFormatter_Boolean() {
	}

	public static HtCommonFieldFormatter_Boolean getInstance() {
		synchronized (mtx_m) {
			if (instance_m == null) {
				instance_m = new HtCommonFieldFormatter_Boolean();
			}

			return instance_m;
		}
	}

	public String format(HtCommonRowSelector.CommonRow row, String sval) {
		return sval.equals("0") ? "false" : "true";
	}
}
