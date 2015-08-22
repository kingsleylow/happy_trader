/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.htraderhelper.utils;

/**
 *
 * @author DanilinS
 */
public class AlertPriorities {
	public static final int AP_LOW =			3;
	public static final int AP_MEDIUM =		2;
	public static final int AP_HIGH =			1;
	public static final int AP_CRITICAL =	0;

	public static String getAlertpriorityByName(int pt) {
		switch (pt) {
			case AP_LOW:
				return "LOW";
			case AP_MEDIUM:
				return "MEDIUM";
			case AP_HIGH:
				return "HIGH";
			case AP_CRITICAL:
				return "CRITICAL";
		}
		return "";
	}

	public static Integer[] getAllAlertPriorities() {
		return new Integer[]{AP_CRITICAL, AP_HIGH, AP_MEDIUM, AP_LOW};
	}
}
