/*
 * LongParam.java
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.utils;

/**
 *
 * @author Victor_Zoubok
 */
public class LongParam {

	private long value_m;

	public LongParam() {
		value_m = 0;
	}

	public LongParam(long val) {
		value_m = val;
	}

	public long getLong() {
		return value_m;
	}

	public void setLong(long val) {
		value_m = val;
	}

	public void increment()
	{
		value_m++;
	}

	public void decrement()
	{
		value_m--;
	}

	@Override
	public String toString()
	{
		return String.valueOf(value_m);
	}
}
