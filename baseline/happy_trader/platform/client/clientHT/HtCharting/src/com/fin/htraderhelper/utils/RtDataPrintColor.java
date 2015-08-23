/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.htraderhelper.utils;

/**
 *
 * @author DanilinS
 */
public class RtDataPrintColor {
	// color type

	static public final int CT_NA = 0;
	static public final int CT_Bid_Deal = 1;
	static public final int CT_Ask_Deal = 100;
	static public final int CT_Neutral = 50;

	// resolve color constants to name
	public static String getColorTypeName(int color) {
		switch (color) {
			case CT_NA:
				return "N/A";
			case CT_Bid_Deal:
				return "Bid Deal";
			case CT_Ask_Deal:
				return "Ask Deal";
			case CT_Neutral:
				return "Neutral";
		}
		return String.valueOf(color);
	}
}
