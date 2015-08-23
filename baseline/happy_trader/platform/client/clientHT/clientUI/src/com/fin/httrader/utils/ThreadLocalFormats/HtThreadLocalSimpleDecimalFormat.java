/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.utils.ThreadLocalFormats;

import java.text.DecimalFormat;

/**
 *
 * @author DanilinS
 */
public class HtThreadLocalSimpleDecimalFormat {

	private String format_m = null;

	public HtThreadLocalSimpleDecimalFormat(String format) {
		format_m = format;
	}
	private final ThreadLocal local_thread_m = new ThreadLocal() {

		@Override
		protected Object initialValue() {
			return new DecimalFormat(format_m);
		}
	};

	public DecimalFormat getSimpleDecimalFormatThreadCopy() {
		return (DecimalFormat) local_thread_m.get();
	}
}
