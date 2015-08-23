/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.webserver;

import com.fin.httrader.interfaces.HtCommonRowSelector;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtUtils;

/**
 *
 * @author DanilinS
 */
public class HtCommonFieldFormatter_GmtTime_FullFormat implements HtCommonRowSelector.HtCommonFieldFormatter {
	private static HtCommonFieldFormatter_GmtTime_FullFormat instance_m = null;
	private static Object mtx_m = new Object();

	private HtCommonFieldFormatter_GmtTime_FullFormat()
	{
	}

	public static HtCommonFieldFormatter_GmtTime_FullFormat getInstance() {
		synchronized (mtx_m) {
			if (instance_m == null) {
				instance_m = new HtCommonFieldFormatter_GmtTime_FullFormat();
			}

			return instance_m;
		}
	}

	public String format(HtCommonRowSelector.CommonRow row, String sval) {

		double lval = Double.valueOf(sval);
		return HtDateTimeUtils.time2String_Gmt((long) lval);

	}
}
