/*
 * HtCommonFieldFormatter_GmtTime.java
 *

 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.interfaces.HtCommonRowSelector;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtUtils;

/**
 *
 * @author Victor_Zoubok
 */
public class HtCommonFieldFormatter_GmtTime implements HtCommonRowSelector.HtCommonFieldFormatter {

	private static HtCommonFieldFormatter_GmtTime instance_m = null;
	private static final Object mtx_m = new Object();

	// no public ctor
	private HtCommonFieldFormatter_GmtTime() {
	}

	public static HtCommonFieldFormatter_GmtTime getInstance() {
		synchronized (mtx_m) {
			if (instance_m == null) {
				instance_m = new HtCommonFieldFormatter_GmtTime();
			}

			return instance_m;
		}
	}

	@Override
	public String format(HtCommonRowSelector.CommonRow row, String sval) {

		if (sval == null || sval.length() <=0)
			return "";
		
		double lval = Double.valueOf(sval);
		return HtDateTimeUtils.time2SimpleString_Gmt((long) lval);

	}
}
