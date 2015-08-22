/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.utils;

/**
 *
 * @author Victor_Zoubok
 */
public class IntParam {
	private int value_m;

	public IntParam() {
		value_m = 0;
	}

	public IntParam(int val) {
		value_m = val;
	}

	public int getInt() {
		return value_m;
	}

	public void setInt(int val) {
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
