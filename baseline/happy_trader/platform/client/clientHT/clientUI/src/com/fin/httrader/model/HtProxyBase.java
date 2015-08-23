/*
 * HtProxyBase.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.model;

import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import jsync.Event;

/**
 *
 * @author Administrator
 */
public abstract class HtProxyBase {

	private final Event lengthyOperEvent_m = new Event();
	private final StringBuilder lengthyOperDescr_m = new StringBuilder();

	// attempts ty apply lengthy-oper lock on success returns true
	public String getLengthyOperationDescription() {
		synchronized (lengthyOperEvent_m) {
			return lengthyOperDescr_m.toString();
		}
	}

	public boolean startLengthyOperation(String description) {
		synchronized (lengthyOperEvent_m) {
			if (lengthyOperEvent_m.waitEvent(0)) {
				// if signalled return false
				return false;
			}

			lengthyOperEvent_m.signal();
			lengthyOperDescr_m.setLength(0);
			lengthyOperDescr_m.append(description);
			return true;
		}
	}

	public void finishLengthyOperation() {
		synchronized (lengthyOperEvent_m) {

			// reset if signalled
			if (lengthyOperEvent_m.waitEvent(0)) {
				lengthyOperEvent_m.reset();
				lengthyOperDescr_m.setLength(0);
			}
		}
	}

	public void resetLengthyOperationFlag() {
		synchronized (lengthyOperEvent_m) {
			if (lengthyOperEvent_m.waitEvent(0)) {
				lengthyOperEvent_m.reset();
			}
		}
	}

	public boolean isBusyWithLengthOperation() {
		synchronized (lengthyOperEvent_m) {
			if (lengthyOperEvent_m.waitEvent(0)) {
				// if signalled return false
				return true;
			}

			return false;
		}
	}

	

	
	public abstract String getProxyName();

	private String getContext() {
		return getClass().getSimpleName();
	}
}
