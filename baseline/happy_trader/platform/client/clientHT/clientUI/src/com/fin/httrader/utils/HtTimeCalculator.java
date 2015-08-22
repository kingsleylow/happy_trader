/*
 * HtTimeCalculator.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.utils;



/**
 *
 * @author Victor_Zoubok
 */
public class HtTimeCalculator {

	private long startTime_m = -1;

	/** Creates a new instance of HtTimeCalculator */
	public HtTimeCalculator() {
		startTime_m = System.currentTimeMillis();
	}

	public long elapsed() {
		return System.currentTimeMillis() - startTime_m;
	}
}
