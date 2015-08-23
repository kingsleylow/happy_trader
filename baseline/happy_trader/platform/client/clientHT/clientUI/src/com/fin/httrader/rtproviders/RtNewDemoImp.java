/*
 * RtNewDemoImp.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.rtproviders;

import com.fin.httrader.HtMain;
import com.fin.httrader.hlpstruct.BackgroundJobStatus;
import com.fin.httrader.interfaces.HtRealTimeProvider;
import com.fin.httrader.managers.RtGlobalEventManager;
import com.fin.httrader.managers.RtRealTimeProviderManager;
import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.hlpstruct.HtRtData;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import java.util.Calendar;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Random;
import java.util.logging.Level;
import jsync.Event;

/**
 *
 * @author Victor_Zoubok
 */
public class RtNewDemoImp extends Thread implements HtRealTimeProvider {

	private BackgroundJobStatus status_m = new BackgroundJobStatus();
	boolean initialized_m = false;
	// initial data
	HashMap<String, String> initData_m = new HashMap<String, String>();
	private String name_m = null;
	// event to shutdown
	Event toShutdown_m = new Event();
	Event started_m = new Event();
	private long timeShift_m = 0;

	/** Creates a new instance of RtDemoImp */
	public RtNewDemoImp() {
	}

	public String getProviderId() {
		return name_m;
	}

	public void setProviderId(String providerId) {
		name_m = providerId;
	}

	public void setTimeShift(long timeshift) {
		timeShift_m = timeshift;
	}

	public long getTimeShift() {
		return timeShift_m;
	}

	public void run() {
		// thread run finction

		//XmlEvent rtDataEvent = new XmlEvent();
		//XmlEvent messageEvent = new XmlEvent();

		StringBuilder symbol_orig = new StringBuilder("symbol");
		StringBuilder symbol_i = new StringBuilder();

		try {

			// theread is really started
			started_m.signal();


			HtLog.log(Level.INFO, "RtDemoImp", "run", "Working thread is initialized", null);
			//callback_m.onSubscriptionBegin(this, RtProviderCallback.OK, null);

			XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_INFO, "Provider started");


			Random rnd = new Random();

			int msec_to_wait = 1000;
			if (initData_m.containsKey("wait_time")) {
				msec_to_wait = HtUtils.parseInt(initData_m.get("wait_time"));
				if (msec_to_wait <=0)
					msec_to_wait = 1000;
			}


			while (true) {


				if (toShutdown_m.waitEvent(msec_to_wait)) {
					break;
				}
				

				for (int i = 1; i <= 14; i++) {

					symbol_i.setLength(0);

					symbol_i.append(symbol_orig);
					symbol_i.append(i);

					String actual = initData_m.get(symbol_i.toString());

					if (actual != null && actual.length() > 0) {
						generateData(actual, rnd);
					}

				}



			}
		} catch (Throwable e) {
			//callback_m.onErrorHappened(this, RtProviderCallback.FAILURE, e.getMessage());

			XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_ERROR, "Provider exception: " + e.getMessage());
			status_m.setUp(BackgroundJobStatus.STATUS_ERRORED, e.getMessage());

			return;

		}

		XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_INFO, "Provider finished");


		//callback_m.onSubscriptionEnd(this, RtProviderCallback.OK, null);
		HtLog.log(Level.INFO, "RtNewDemoImp", "run", "Working thread finishing...", null);
	}

	public synchronized void initialize(Map<String, String> initdata) throws Exception {
		if (initialized_m) {
			throw new HtException("RtNewDemoImp", "initialize", "Already initialized");
		}

		initData_m.clear();
		initData_m.putAll(initdata);
		initialized_m = true;

		// clear events
		toShutdown_m.reset();
		started_m.reset();

		status_m.initializeOk();

		// start the thread
		start();

		// wait until it is started
		started_m.waitEvent();
	}

	public synchronized void deinitialize() throws Exception {
		if (!initialized_m) {
			throw new HtException("RtNewDemoImp", "deinitialize", "Not initialized");
		}


		initialized_m = false;
		toShutdown_m.signal();


		// wait until thread finishes
		join();
		HtLog.log(Level.INFO, "RtNewDemoImp", "deinitialize", "Working thread finished", null);

	}

	// ----------------------------
	private void generateData(String symbol, Random rnd) throws Exception {

		HtRtData rtdata = new HtRtData();
		rtdata.type_m = HtRtData.RT_Type;
		rtdata.ask_m = rnd.nextDouble() * 10;
		rtdata.bid_m = rtdata.ask_m + rtdata.ask_m / 10;
		rtdata.volume_m = 100.0;

		rtdata.time_m = Calendar.getInstance().getTimeInMillis() + this.getTimeShift()*60*60*1000;
		rtdata.setSymbol(symbol);
		rtdata.setProvider(getProviderId());


		XmlEvent rtDataEvent=new XmlEvent();

		//callback_m.onNewDataArrived(this, rtdata, "");
		RtRealTimeProviderManager.createXmlEventRtData(rtdata, rtDataEvent);

		RtGlobalEventManager.instance().pushCommonEvent(rtDataEvent);
	}

	public BackgroundJobStatus returnProviderStatus() {
		return status_m;
	}

	@Override
	public void returnAvailableSymbolList(List<String> symbols) {
		throw new UnsupportedOperationException("Not supported yet.");
	}
}