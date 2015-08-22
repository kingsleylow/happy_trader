/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.rtproviders;

import java.text.SimpleDateFormat;
import com.fin.httrader.hlpstruct.BackgroundJobStatus;
import com.fin.httrader.hlpstruct.RtDataOperation;
import com.fin.httrader.hlpstruct.RtDataPrintColor;
import com.fin.httrader.interfaces.HtRealTimeProvider;
import com.fin.httrader.utils.win32.HtSysPowerEventListener;
import com.fin.httrader.managers.RtGlobalEventManager;
import com.fin.httrader.managers.RtRealTimeProviderManager;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.LongParam;
import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.utils.hlpstruct.HtRtData;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import com.fin.httrader.utils.win32.DDEItem;
import com.fin.httrader.utils.win32.HtDDEServer;
import com.fin.httrader.utils.win32.HtQuikListener;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.GregorianCalendar;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TimeZone;
import java.util.Vector;
import java.util.logging.Level;

/**
 *
 * @author DanilinS
 */
public class RtQuikTicksViaODBC implements HtRealTimeProvider, HtQuikListener.ListenerInterface {

	private BackgroundJobStatus status_m = new BackgroundJobStatus();
	private long timeShift_m = 0;
	private StringBuilder name_m = new StringBuilder();
	private HashMap<String, String> initData_m = new HashMap<String, String>();
	// DDE server instance
	private HtQuikListener quikListener_m = null;
	private int curDay_m = -1;
	private int curMonth_m = -1;
	private int curYear_m = -1;
	private HashMap<String, Long> lastTimes_m = new HashMap<String, Long>();
	private Calendar tm_m = null;
	private int first_order_id_m = -1;

	// ----------------
	public RtQuikTicksViaODBC() {
	}

	private String getContext() {
		return getClass().getSimpleName();
	}

	public void initialize(Map<String, String> initdata) throws Exception {
		if (quikListener_m != null) {
			throw new HtException(getContext(), "initialize", "Already initialized");
		}

		initData_m.clear();
		initData_m.putAll(initdata);


		status_m.initializeOk();



		first_order_id_m = HtUtils.parseInt(HtUtils.getParameterWithoutException(initdata, "FIRST_ROW"));



		// today
		// GMT time zone - will adjust manually
		Calendar calendar = Calendar.getInstance(TimeZone.getTimeZone("GMT"));
		curDay_m = calendar.get(Calendar.DAY_OF_MONTH);
		curMonth_m = calendar.get(Calendar.MONTH) + 1;
		curYear_m = calendar.get(Calendar.YEAR);


		this.initServer();

		XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_INFO, "Provider started");
	}

	public void deinitialize() throws Exception {
		this.deinitServer();
		XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_INFO, "Provider stopped");
	}

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
		return status_m;
	}

	public void DDEDataArrived(DDEItem item, String value) {
		// n/a
	}

	
	// helpers
	private void initServer() throws Exception {
		if (quikListener_m != null) {
			throw new HtException(getContext(), "initServer", "Already initialized");
		}

		quikListener_m = new HtQuikListener(this);




	}

	private void deinitServer() throws Exception {
		if (quikListener_m == null) {
			throw new HtException(getContext(), "deinitDDEServer", "Not initialized");
		}


		quikListener_m.destroy();
		quikListener_m = null;


	}

	private HtRtData parseAndSendRTData(
					int hour, int minute, int second, String symbol, String operation, double price, double volume
		) throws Exception {
		// custome event


		// parse date time
		LongParam millisec = new LongParam();
		

		tm_m = HtDateTimeUtils.yearMonthDayHourMinSec2Time_ReuseCalendar_Gmt(curYear_m, curMonth_m, curDay_m, hour, minute, second, tm_m, millisec);


	
		return new HtRtData(
						getProviderId(),
						symbol,
						millisec.getLong(),
						price,
						price,
						volume,
						RtDataPrintColor.CT_NA,
						operation.equalsIgnoreCase("BUY") ? RtDataOperation.OT_Buy : RtDataOperation.OT_Sell);

	}

	@Override
	public void onQuikTickArrived(int hour, int minute, int second, String symbol, String operation, double price, double volume) {
		try {


			HtRtData rtdata = parseAndSendRTData(hour, minute, second, symbol, operation, price, volume );
				

			long ttime = rtdata.time_m;
			


			Long lastTime = lastTimes_m.get(symbol);
			if (lastTime != null) {
				if (ttime <= lastTime) // plus 2 milliseconds
				{
					ttime = lastTime + 2;
				}
			}

			lastTimes_m.put(symbol, ttime);
			rtdata.time_m = ttime;


			XmlEvent rtDataEvent = new XmlEvent();
			RtRealTimeProviderManager.createXmlEventRtData(rtdata, rtDataEvent);
			RtGlobalEventManager.instance().pushCommonEvent(rtDataEvent);

		} catch (Throwable e) {

			// reset last times
			lastTimes_m.clear();
			
			XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_ERROR, "Exception on delegating RT data: " + e.getMessage());
		}

	}

	@Override
	public void onErrorArrived(String error) {
		status_m.setUp(-1, error);
		XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_ERROR, "Error arrived in the process of work: " + error);
	}

	@Override
	public void returnAvailableSymbolList(List<String> symbols) {
		throw new UnsupportedOperationException("Not supported yet.");
	}
}
