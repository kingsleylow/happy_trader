/*
 * HtExtIntBuffer.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.externalapi.utils;

/**
 *
 * @author Victor_Zoubok
 */
public class HtExtIntBuffer {

	private int value_m;

	public HtExtIntBuffer() {
		value_m = 0;
	}

	public HtExtIntBuffer(int val) {
		value_m = val;
	}

	public int getInt() {
		return value_m;
	}

	public void setInt(int val) {
		value_m = val;
	}
}
