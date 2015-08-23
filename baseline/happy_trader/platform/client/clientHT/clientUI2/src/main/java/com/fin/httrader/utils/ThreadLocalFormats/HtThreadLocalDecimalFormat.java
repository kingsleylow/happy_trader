/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.utils.ThreadLocalFormats;

import java.text.DecimalFormat;
import java.text.DecimalFormatSymbols;
import java.text.NumberFormat;

/**
 *
 * @author DanilinS
 */
public class HtThreadLocalDecimalFormat {

	private char decSep_m = '.';
	private int groupingSize_m = 20;

	public HtThreadLocalDecimalFormat(char decSep, int groupingSize) {
		decSep_m = decSep;
		groupingSize_m = groupingSize;

	}
	private final ThreadLocal local_thread_m = new ThreadLocal() {

		@Override
		protected Object initialValue() {
			DecimalFormat dc = (DecimalFormat) NumberFormat.getInstance();

			DecimalFormatSymbols dsymb = new DecimalFormatSymbols();
			dsymb.setDecimalSeparator(decSep_m);
			//dsymb.setGroupingSeparator('');
			dc.setDecimalFormatSymbols(dsymb);
			dc.setGroupingSize(20);

			return dc;
		}
	};

	public NumberFormat getNumberFormatThreadCopy() {
		return (NumberFormat) local_thread_m.get();
	}
}
