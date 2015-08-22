/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.utils;

/**
 *
 * @author DanilinS
 */
public class DoubleParam {
	private double value_m;

	public DoubleParam() {
		value_m = 0;
	}

	public DoubleParam(double val) {
		value_m = val;
	}

	public double getDouble() {
		return value_m;
	}

	public void setDouble(double val) {
		value_m = val;
	}

	@Override
	public String toString()
	{
		return String.valueOf(value_m);
	}
}
