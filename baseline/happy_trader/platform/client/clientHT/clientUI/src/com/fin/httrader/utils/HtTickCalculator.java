/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.utils;

/**
 *
 * @author DanilinS
 */
public class HtTickCalculator {
	private long startTime_m = -1;

	/** Creates a new instance of HtTimeCalculator */
	public HtTickCalculator() {
		startTime_m = System.nanoTime();
	}

	public long elapsed() {
		return System.nanoTime() - startTime_m;
	}
}
