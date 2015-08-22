package com.fin.httrader.rtplugins;


import com.fin.httrader.hlpstruct.ReatTimeProviderStatus;

import com.fin.httrader.interfaces.HtRealTimeProvider;
import com.fin.httrader.managers.RtGlobalEventManager;
import com.fin.httrader.managers.RtRealTimeProviderManager;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.utils.hlpstruct.HtRtData;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import java.util.Calendar;
import java.util.HashMap;
import java.util.Map;
import java.util.Random;
import java.util.logging.Level;
import jsync.Event;

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author DanilinS
 */
public class DemoProvider extends Thread implements HtRealTimeProvider {

	private ReatTimeProviderStatus status_m = new ReatTimeProviderStatus();
	boolean initialized_m = false;
	Foo foo = new Foo();

	// initial data
	HashMap<String, String> initData_m = new HashMap<String, String>();
	private String name_m = null;
	
	// event to shutdown
	Event toShutdown_m = new Event();
	Event started_m = new Event();
	private long timeShift_m = 0;

	/** Creates a new instance of RtDemoImp */
	public DemoProvider() {
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

			//if (callback_m==null)
			//throw new HtException("RtDemoImp", "run", "The context is not initialized");

			HtLog.log(Level.INFO, "RtDemoImp", "run", "Working thread is initialized", null);

			//callback_m.onSubscriptionBegin(this, RtProviderCallback.OK, null);
			XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_INFO, "Provider started");

			Random rnd = new Random();

			int msec_to_wait = 1000;
			

			while (true) {


				if (toShutdown_m.waitEvent(msec_to_wait)) {
					break;
				}


			
				generateData("SYM1", rnd);
				generateData("SYM2", rnd);
			



			}
		} catch (Throwable e) {
			//callback_m.onErrorHappened(this, RtProviderCallback.FAILURE, e.getMessage());
			XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_ERROR, "Provider exception: " + e.getMessage());
			status_m.setUp(ReatTimeProviderStatus.STATUS_ERRORED, e.getMessage());

			return;

		}

		//callback_m.onSubscriptionEnd(this, RtProviderCallback.OK, null);
		XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_INFO, "Provider finished");



		HtLog.log(Level.INFO, "RtDemoImp", "run", "Working thread finishing...", null);
	}

	public synchronized void initialize(Map<String, String> initdata) throws Exception {
		if (initialized_m) {
			throw new HtException("RtDemoImp", "initialize", "Already initialized");
		}

		initData_m.clear();
		initData_m.putAll(initdata);
		//callback_m = callback;
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
			throw new HtException("RtDemoImp", "deinitialize", "Not initialized");
		}


		initialized_m = false;
		toShutdown_m.signal();


		// wait until thread finishes
		join();
		HtLog.log(Level.INFO, "RtDemoImp", "deinitialize", "Working thread finished", null);

	}

	// ----------------------------
	private void generateData(String symbol, Random rnd) throws Exception {

		HtRtData rtdata = new HtRtData();
		rtdata.type_m = HtRtData.RT_Type;
		rtdata.ask_m = rnd.nextDouble() * 10;
		rtdata.bid_m = rtdata.ask_m + rtdata.ask_m / 10;

		rtdata.time_m = Calendar.getInstance().getTimeInMillis() + this.getTimeShift()*60*60*1000;
		rtdata.setSymbol(symbol);
		rtdata.setProvider(getProviderId());

		XmlEvent rtDataEvent= new XmlEvent();
		RtRealTimeProviderManager.createXmlEvent(rtdata, rtDataEvent);

		RtGlobalEventManager.instance().pushCommonEvent(rtDataEvent);
	}

	public ReatTimeProviderStatus returnProviderStatus() {
		return status_m;
	}

}
