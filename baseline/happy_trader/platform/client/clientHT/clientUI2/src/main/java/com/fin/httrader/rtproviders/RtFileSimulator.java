/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.rtproviders;

import com.fin.httrader.hlpstruct.BackgroundJobStatus;
import com.fin.httrader.interfaces.HtRealTimeProvider;
import com.fin.httrader.utils.HtUtils;
import java.util.List;
import java.util.Map;
import jsync.Event;

/**
 *
 * @author DanilinS
 * It gets data from the file
 */
public class RtFileSimulator extends Thread implements HtRealTimeProvider {
	private BackgroundJobStatus status_m = new BackgroundJobStatus();

	private Event toShutdown_m = new Event();

	private StringBuilder dataDirectory_m = new StringBuilder();

	private long timeShift_m = 0;

	private StringBuilder name_m = new StringBuilder();


	private String getContext()
	{
		return getClass().getSimpleName();
	}

	// -------------------------

	public void initialize(Map<String, String> initdata) throws Exception {
		dataDirectory_m.setLength(0);
		dataDirectory_m.append( HtUtils.getParameterWithException(initdata, "DATA_FILE_PATH") );
	  //

		
		
	}

	public void deinitialize() throws Exception {
		
	}

	// -----------
	
	public String getProviderId() {
		return name_m.toString();
	}

	public void setProviderId(String providerId) {
		name_m.setLength(0);
		name_m.append(providerId);
	}

	public void setTimeShift(long timeshift) {
		timeShift_m = timeshift;
	}

	public long getTimeShift() {
		return timeShift_m;
	}

	public BackgroundJobStatus returnProviderStatus() {
		// return error state
		return status_m;

	}

	// -----------------------------
	// helpers

	@Override
	public void returnAvailableSymbolList(List<String> symbols) {
		throw new UnsupportedOperationException("Not supported yet.");
	}
	
}
