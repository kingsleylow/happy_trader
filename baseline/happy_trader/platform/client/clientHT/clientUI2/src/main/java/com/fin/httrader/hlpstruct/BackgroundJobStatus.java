/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.hlpstruct;

import com.fin.httrader.utils.HtEnumeratorDescriptor;

/**
 *
 * @author DanilinS
 * THREAD SAFE !!!
 */
public class BackgroundJobStatus {

	public static final int STATUS_DUMMY = -1;
	public static final int STATUS_OK = 0;
	public static final int STATUS_STILL_NOT_READY = 1;
	public static final int STATUS_WARNING = 2;
	public static final int STATUS_ERRORED = 3;
	
	private int status_m = STATUS_DUMMY;
	final private StringBuilder statusString_m = new StringBuilder();

	private static HtEnumeratorDescriptor descriptor_m = new HtEnumeratorDescriptor("com.fin.httrader.hlpstruct.BackgroundJobStatus");

	public BackgroundJobStatus() {
	}

	public BackgroundJobStatus(int code, String reason) {
		setUp(code, reason);
	}

	
	
	public static String getStatusString(int status) {

		switch (status) {
			case STATUS_DUMMY: return "DUMMY";
			case STATUS_OK: return "OK";
			case STATUS_STILL_NOT_READY : return "NOT READY";
			case STATUS_ERRORED: return "ERROR";
			case STATUS_WARNING: return "WARNING";
		}
		return "N/A";
	}

	public int getStatusCode() {
		synchronized (statusString_m) {
			return status_m;
		}
	}

	public String getStatusReason() {
		synchronized (statusString_m) {
			return statusString_m.toString();
		}
	}

	public void setUp(int code, String reason) {
		synchronized (statusString_m) {
			statusString_m.setLength(0);
			statusString_m.append(reason);

			status_m = code;
		}
	}

	public void initializeOk() {
		synchronized (statusString_m) {
			statusString_m.setLength(0);
			status_m = STATUS_OK;
		}
	}
}


