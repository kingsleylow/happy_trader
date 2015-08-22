/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.externalapi.utils;

/**
 *
 * @author Victor_Zoubok
 */
public class HtExtBoolBuffer {
	private boolean value_m;

	public HtExtBoolBuffer() {
		value_m = false;
	}

	public HtExtBoolBuffer(boolean val) {
		value_m = val;
	}

	public boolean getBoolean() {
		return value_m;
	}

	public void setBoolean(boolean val) {
		value_m = val;
	}
}
