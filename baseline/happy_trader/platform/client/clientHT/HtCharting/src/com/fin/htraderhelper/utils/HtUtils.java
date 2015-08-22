/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.htraderhelper.utils;

import java.io.InputStream;
import java.text.DecimalFormat;
import java.text.DecimalFormatSymbols;
import java.text.NumberFormat;

/**
 *
 * @author DanilinS
 */
public class HtUtils {

	public static String formatDouble(double val, int signdig) {

		DecimalFormat dfrmt = (DecimalFormat) NumberFormat.getInstance();

		if (signdig > 0) {
			dfrmt.setMinimumFractionDigits(signdig);
			dfrmt.setMaximumFractionDigits(signdig);
		} else {
			dfrmt.setMinimumFractionDigits(4);
			dfrmt.setMaximumFractionDigits(4);

		}

		DecimalFormatSymbols dsymb = new DecimalFormatSymbols();
		dsymb.setDecimalSeparator('.');
		dfrmt.setDecimalFormatSymbols(dsymb);
		dfrmt.setGroupingSize(20);


		return dfrmt.format(val);
	}

	public static String convertInputStreamToString(InputStream is, String encoding) throws Exception {

		int BUFFER_SIZE = 2048;

		StringBuilder out = new StringBuilder();
		byte[] buffer = new byte[BUFFER_SIZE];
		int off = 0;
		while (true) {

			int read = is.read(buffer, 0, BUFFER_SIZE);
			if (read <= 0) {
				break;
			}

			byte[] sbuffer = new byte[read];
			System.arraycopy(buffer, 0, sbuffer, 0, read);

			if (encoding != null) {
				out.append(new String(sbuffer, encoding));
			} else {
				out.append(new String(sbuffer));
			}

		}

		return out.toString();
	}
}
