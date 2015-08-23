/*
 * HtExtCommandResult.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.externalapi;

/**
 *
 * @author Victor_Zoubok
 */
public class HtExtCommandResult {

	public HtExtCommandResult(String caller) {
		caller_m.append(caller);
	}

	public void initialize(Throwable e) {
		result_m = -1;
		strResult_m.setLength(0);
		strResult_m.append(e.getMessage());
	}

	public int getCode() {
		return result_m;
	}

	public String getReason() {
		return strResult_m.toString();
	}

	public String getCaller() {
		return caller_m.toString();
	}

	public String getStringResult() {
		StringBuilder out = new StringBuilder();

		out.append("( ");
		out.append(getCaller());
		out.append(" ) - ");

		if (getCode() == 0) {
			out.append("OK");
		} else {
			out.append("Error: ");
			out.append(strResult_m);
		}

		return out.toString();
	}

	public boolean isOk()
	{
		return result_m == 0;
	}
	//
	private int result_m = 0;
	private StringBuilder caller_m = new StringBuilder();
	private StringBuilder strResult_m = new StringBuilder();
}
