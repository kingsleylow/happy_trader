/*
 * RtDemoImp.java
 *
 * Created on December 3, 2006, 5:56 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.rtproviders;

import com.fin.httrader.HtMain;
import com.fin.httrader.hlpstruct.AlertPriorities;
import com.fin.httrader.hlpstruct.BackgroundJobStatus;
import com.fin.httrader.hlpstruct.RtDataPrintColor;
import com.fin.httrader.managers.RtGlobalEventManager;
import com.fin.httrader.managers.RtRealTimeProviderManager;
import com.fin.httrader.rtproviders.*;
import com.fin.httrader.interfaces.*;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.hlpstruct.CommonLog;

import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.hlpstruct.HtRtData;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.hlpstruct.HtDrawableObject;
import com.fin.httrader.utils.hlpstruct.HtLevel1Data;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import java.util.Calendar;
import java.util.List;
import java.util.Map;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Random;
import java.util.logging.Level;
import jsync.Event;

/**
 *
 * @author Administrator
 */
public class RtDemoImp extends Thread implements HtRealTimeProvider {

	private BackgroundJobStatus status_m = new BackgroundJobStatus();
	boolean initialized_m = false;
	// initial data
	HashMap<String, String> initData_m = new HashMap<String, String>();
	private String name_m = null;
	//RtProviderCallback callback_m = null;
	// event to shutdown
	Event toShutdown_m = new Event();
	Event started_m = new Event();
	private long timeShift_m = 0;
	HashSet<String> symbols_m=new HashSet<String>();

	/** Creates a new instance of RtDemoImp */
	public RtDemoImp() {
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
			if (initData_m.containsKey("wait_time")) {
				msec_to_wait = Integer.valueOf(initData_m.get("wait_time"));
			}

			long globalidx = 0;
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
						symbols_m.add(actual);
						generateData(actual, rnd, globalidx);

						globalidx++;
					}

				}




			}
		} catch (Throwable e) {
			//callback_m.onErrorHappened(this, RtProviderCallback.FAILURE, e.getMessage());
			XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_ERROR, "Provider exception: " + e.getMessage());
			status_m.setUp(BackgroundJobStatus.STATUS_ERRORED, e.getMessage());

			return;

		}

		//callback_m.onSubscriptionEnd(this, RtProviderCallback.OK, null);
		XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_INFO, "Provider finished");



		HtLog.log(Level.INFO, "RtDemoImp", "run", "Working thread finishing...", null);
	}

	@Override
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

		XmlEvent synch= new XmlEvent();
		RtRealTimeProviderManager.createXmlEventSynchronization(synch, HtDateTimeUtils.getCurGmtTime(), getProviderId(), HtRtData.SYNC_EVENT_PROVIDER_START, "demo p started" );
		RtGlobalEventManager.instance().pushCommonEvent(synch);
	}

	@Override
	public synchronized void deinitialize() throws Exception {
		if (!initialized_m) {
			throw new HtException("RtDemoImp", "deinitialize", "Not initialized");
		}


		initialized_m = false;
		toShutdown_m.signal();


		// wait until thread finishes
		join();

		XmlEvent synch= new XmlEvent();
		RtRealTimeProviderManager.createXmlEventSynchronization(synch, HtDateTimeUtils.getCurGmtTime(), getProviderId(), HtRtData.SYNC_EVENT_PROVIDER_FINISH, "demo p finished" );
		RtGlobalEventManager.instance().pushCommonEvent(synch);

		HtLog.log(Level.INFO, "RtDemoImp", "deinitialize", "Working thread finished", null);

	}

	// ----------------------------
	private void generateData(String symbol, Random rnd, long globalidx) throws Exception {

		
		HtRtData rtdata = new HtRtData();
		rtdata.type_m = HtRtData.RT_Type;
		rtdata.color_m = RtDataPrintColor.CT_Ask_Deal;
		rtdata.ask_m = rnd.nextDouble() * 10;
		rtdata.bid_m = rtdata.ask_m + rtdata.ask_m / 2;

		/*
		rtdata.open2_m = 2.0;
		rtdata.high2_m = 3.0;
		rtdata.low2_m = 4.0;
		rtdata.close2_m = 5.0;

		rtdata.open_m = 20.0;
		rtdata.high_m = 30.0;
		rtdata.low_m = 6.0;
		rtdata.close_m = 7.0;
		*/

		rtdata.volume_m = 100;

		rtdata.time_m = HtDateTimeUtils.getCurGmtTime() + this.getTimeShift()*60*60*1000;
		/*
		if (0!=(globalidx% 1000))
			rtdata.time_m = HtDateTimeUtils.getCurGmtTime_DayBegin() - globalidx*1000;
		else
			rtdata.time_m = HtDateTimeUtils.getCurGmtTime();
		*/
		rtdata.setSymbol(symbol);
		rtdata.setProvider(getProviderId());

		//callback_m.onNewDataArrived(this, rtdata, "");
		XmlEvent rtDataEvent= new XmlEvent();
		RtRealTimeProviderManager.createXmlEventRtData(rtdata, rtDataEvent);
		RtGlobalEventManager.instance().pushCommonEvent(rtDataEvent);
		
		XmlEvent rtDataEvent2= new XmlEvent();
		HtLevel1Data rtdata2 = new HtLevel1Data();
		rtdata2.best_ask_price_m = 1;
		rtdata2.best_bid_price_m  =2;
		rtdata2.best_ask_volume_m = 3;
		rtdata2.best_bid_volume_m = 4;
		rtdata2.open_interest_m  =6;
		rtdata2.setSymbol(symbol);
		rtdata2.setProvider(getProviderId());

		
		RtRealTimeProviderManager.createXmlEventLevel1(rtdata2, rtDataEvent2);
		RtGlobalEventManager.instance().pushCommonEvent(rtDataEvent2);
		
		 

		// generate text drawable
		/*
		HtDrawableObject dobj = new HtDrawableObject();
		dobj.setTime(HtDateTimeUtils.getCurGmtTime());
		dobj.setSymbol("-ALL-");
		dobj.setProvider("laser_provider");
		dobj.setRunName("run_name_dummy");

		
		dobj.getAsText(true).priority_m = AlertPriorities.AP_MEDIUM;
		dobj.getAsText().shortText_m.append( "short_text" );
		dobj.getAsText().text_m.append("long text  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");

		XmlEvent event_dr = new XmlEvent();
		HtDrawableObject.createXmlEventDrawableObject(dobj, event_dr);
		RtGlobalEventManager.instance().pushCommonEvent(event_dr);
		 */



	}

	public BackgroundJobStatus returnProviderStatus() {
		return status_m;
	}

	@Override
	public void returnAvailableSymbolList(List<String> symbols) {
		symbols.clear();
		symbols.addAll(symbols_m);
	}
}
