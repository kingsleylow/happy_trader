/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.rtproviders;

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
public class RtQuikTicks implements HtRealTimeProvider, HtSysPowerEventListener {

	@Override
	public void returnAvailableSymbolList(List<String> symbols) {
		throw new UnsupportedOperationException("Not supported yet.");
	}

	private class SymbolEntry {

		// x coordinates
		public int price_x_m = -1;
		public int volume_x_m = -1;
		public int time_x_m = -1;
		public int symbol_id_x_m = -1;
		public int color_id_x_m = -1;
		public int deal_count_x_m = -1;
		public int order_id_m = -1;
		// -------------
		private long msecCounter_m = 1;
		// string values to initially resolve this stuff
		public StringBuilder name_price_x_m = new StringBuilder();
		public StringBuilder name_volume_x_m = new StringBuilder();
		public StringBuilder name_time_x_m = new StringBuilder();
		public StringBuilder name_color_id_x_m = new StringBuilder();
		public StringBuilder name_symbol_id_x_m = new StringBuilder();
		public StringBuilder name_deal_count_id_x_m = new StringBuilder();
		public StringBuilder order_id_x_m = new StringBuilder();
		
		public SimpleDateFormat timeF_m = new SimpleDateFormat();


		// ----------
		public void initialize(
						String orderNumFormat,
						String timeFormat,
						String name_price_x,
						String name_volume_x,
						String name_time_x,
						String name_color_id_x,
						String name_symbol_id_x,
						String deal_count_id_x) {

			name_price_x_m.append(name_price_x);
			name_volume_x_m.append(name_volume_x);
			name_time_x_m.append(name_time_x);
			name_color_id_x_m.append(name_color_id_x);
			name_symbol_id_x_m.append(name_symbol_id_x);
			name_deal_count_id_x_m.append(deal_count_id_x);
			order_id_x_m.append( orderNumFormat);

			timeF_m.applyPattern(timeFormat);
			timeF_m.setTimeZone(TimeZone.getTimeZone("GMT"));
		}

		public boolean isValid() {

			return (order_id_m>=0 && color_id_x_m >= 0 && price_x_m >= 0 && volume_x_m >= 0 && time_x_m >= 0 && symbol_id_x_m >= 0 /*&& total_y_m > 0*/);

		}

		public long getDummyMsec() {
			long result = msecCounter_m++;

			if (msecCounter_m >= 800) {
				msecCounter_m = 1;
			}

			return result;
		}
	};
	private BackgroundJobStatus status_m = new BackgroundJobStatus();
	private long timeShift_m = 0;
	private StringBuilder name_m = new StringBuilder();
	private HashMap<String, String> initData_m = new HashMap<String, String>();
	private HashMap<String, Vector<SymbolEntry>> dataMap_m = new HashMap<String, Vector<SymbolEntry>>();
	// DDE server instance
	private HtDDEServer ddeServer_m = null;
	private StringBuilder serviceName_m = new StringBuilder();
	private Vector<DDEItem> itemList_m = new Vector<DDEItem>();
	private int curDay_m = -1;
	private int curMonth_m = -1;
	private int curYear_m = -1;
	private HashMap<String, Long> lastTimes_m = new HashMap<String, Long>();
	private Calendar tm_m = null;
	private int first_order_id_m = -1;

	// ----------------
	public RtQuikTicks() {
	}

	private String getContext() {
		return getClass().getSimpleName();
	}

	public void initialize(Map<String, String> initdata) throws Exception {
		if (ddeServer_m != null) {
			throw new HtException(getContext(), "initialize", "Already initialized");
		}

		initData_m.clear();
		initData_m.putAll(initdata);


		status_m.initializeOk();

		// read parameters

		// service name
		serviceName_m.setLength(0);
		serviceName_m.append(HtUtils.getParameterWithException(initdata, "SERVICE_NAME"));


		first_order_id_m = HtUtils.parseInt(HtUtils.getParameterWithoutException(initdata, "FIRST_ROW"));

		// topic name
		// add to item list
		Vector<DDEItem> itemListLocal = new Vector<DDEItem>();

		// topic must be
		String topicName = HtUtils.getParameterWithException(initdata, "TOPIC_NAME");

		// add to global list
		itemListLocal.add(new DDEItem(topicName, "R1C1:R1C1"));
		itemList_m.addAll(itemListLocal);

		// register topic
		SymbolEntry entry_i = new SymbolEntry();


		entry_i.initialize(
						"",
						"HH:mm:ss",
						"PRICE",
						"VALUE",
						"TRADETIME",
						"BUYSELL",
						"SECCODE",
						"QTY");

		Vector<SymbolEntry> symbolList = new Vector<SymbolEntry>();
		symbolList.add(entry_i);

		dataMap_m.put(topicName, symbolList);

		// today
		// GMT time zone - will adjust manually
		Calendar calendar = Calendar.getInstance(TimeZone.getTimeZone("GMT"));
		curDay_m = calendar.get(Calendar.DAY_OF_MONTH);
		curMonth_m = calendar.get(Calendar.MONTH) + 1;
		curYear_m = calendar.get(Calendar.YEAR);


		initDDEServer();

		XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_INFO, "Provider started");
	}

	public void deinitialize() throws Exception {
		deinitDDEServer();
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

	public void onDDEXLTableDataArrived(DDEItem item, Vector<Vector<String>> data) {
		String topic = item.getTopic();
		String sitem = item.getItem();

		

		//System.out.println("DDE XLTABLE arrived: "+item.getItem() + " - " + item.getTopic() );

		if (dataMap_m.containsKey(topic)) {
			// we have our data
			Vector<SymbolEntry> entryList = dataMap_m.get(topic);

			for (int m = 0; m < entryList.size(); m++) {
				SymbolEntry entry = entryList.get(m);

				//if (!entry.isValid()) {
					// try to initialize this entry
					if (data.size() > 0) {
						// 1st row - check if it contains header
						if (sitem.toUpperCase().startsWith("R1C1:")) {
							// this is ours

							lastTimes_m.clear();

							// start

							XmlEvent syncEvent = new XmlEvent();
							RtRealTimeProviderManager.createXmlEventSynchronization(
											syncEvent,
											HtDateTimeUtils.getCurGmtTime(),
											getProviderId(), 
											HtRtData.SYNC_EVENT_PROVIDER_START,
											null
							);
							
							RtGlobalEventManager.instance().pushCommonEvent(syncEvent);

							Vector<String> row_1 = data.get(0);
							HashMap<String, Integer> columnMap = new HashMap<String, Integer>();

							for (int i = 0; i < row_1.size(); i++) {
								columnMap.put(row_1.get(i), i);
							}
							//

							if (columnMap.containsKey(entry.name_price_x_m.toString())) {
								entry.price_x_m = columnMap.get(entry.name_price_x_m.toString());
							}

							if (columnMap.containsKey(entry.name_deal_count_id_x_m.toString())) {
								entry.deal_count_x_m = columnMap.get(entry.name_deal_count_id_x_m.toString());
							}


							if (columnMap.containsKey(entry.name_color_id_x_m.toString())) {
								entry.color_id_x_m = columnMap.get(entry.name_color_id_x_m.toString());
							}

							if (columnMap.containsKey(entry.name_time_x_m.toString())) {
								entry.time_x_m = columnMap.get(entry.name_time_x_m.toString());
							}

							if (columnMap.containsKey(entry.name_symbol_id_x_m.toString())) {
								entry.symbol_id_x_m = columnMap.get(entry.name_symbol_id_x_m.toString());
							}

							if (columnMap.containsKey(entry.name_volume_x_m.toString())) {
								entry.volume_x_m = columnMap.get(entry.name_volume_x_m.toString());
							}

							// empty
							if (columnMap.containsKey(entry.order_id_x_m.toString())) {
								entry.order_id_m = columnMap.get(entry.order_id_x_m.toString());
							}



						}
					}
					;


					//
					if (!entry.isValid()) {
						return;
					}

				//}

				// try to reap our values
				// for now we query the necessary row as I think the table is not resended completely
				int total_y = -1;
				//List<XmlEvent> events_to_send = new ArrayList<XmlEvent>();
				//List<XmlEvent> events_to_send_filtered = new ArrayList<XmlEvent>();
				for (int i = 0; i < data.size(); i++) {
					Vector<String> data_i = data.get(i);
					String symbolValue = data_i.get(entry.symbol_id_x_m);

					// that's ours
					total_y = i;

					String order_id_s = data.get(total_y).get(entry.order_id_m);
					int order_num = (int)HtUtils.parseDouble(order_id_s);

					// skip rows
					if (first_order_id_m >=0 && order_num < first_order_id_m)
						continue;

					

					try {


						HtRtData rtdata = parseAndSendRTData(entry,
										symbolValue,
										data.get(total_y).get(entry.time_x_m),
										data.get(total_y).get(entry.price_x_m),
										data.get(total_y).get(entry.volume_x_m),
										data.get(total_y).get(entry.color_id_x_m),
										data.get(total_y).get(entry.deal_count_x_m));

						long ttime = rtdata.time_m;
						String symbol = rtdata.getSymbol();

						
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

						
						//HtLog.log(Level.INFO, getContext(), "pass RT tick: ", order_num + " - " + symbol + " - " + rtdata.toString());

					} catch (Throwable e) {

						// reset last times
						lastTimes_m.clear();
						//HtLog.log(Level.WARNING, getContext(), "onDDEXLTableDataArrived", "Exception on delegating RT data: " + e.getMessage());
						XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_ERROR, "Exception on delegating RT data: " + e.getMessage());
					}

				}


			
			}


		} // entry is alive


	}

	// helpers
	private void initDDEServer() throws Exception {
		if (ddeServer_m != null) {
			throw new HtException(getContext(), "initDDEServer", "Already initialized");
		}

		ddeServer_m = new HtDDEServer(this);

		Vector<DDEItem> newList = new Vector<DDEItem>();

		// normalize itemlist
		HashMap<String, Set<DDEItem>> normalized = new HashMap<String, Set<DDEItem>>();

		for (int i = 0; i < itemList_m.size(); i++) {
			DDEItem item_i = itemList_m.get(i);

			if (normalized.containsKey(item_i.topic_m)) {
				Set<DDEItem> itemSet = normalized.get(item_i.getTopic());
				itemSet.add(item_i);



			} else {
				HashSet<DDEItem> itemSet = new HashSet<DDEItem>();
				itemSet.add(item_i);

				normalized.put(item_i.getTopic(), itemSet);

			}

		}

		//
		for (Iterator<String> it = normalized.keySet().iterator(); it.hasNext();) {
			String topic_i = it.next();
			Set<DDEItem> itemSet = normalized.get(topic_i);

			for (Iterator<DDEItem> it2 = itemSet.iterator(); it2.hasNext();) {
				newList.add(new DDEItem(topic_i, it2.next().getItem()));
			}

		}
		//
		if (serviceName_m.length() <= 0) {
			throw new HtException(getContext(), "initDDEServer", "Invalid service name");
		}

		boolean result = ddeServer_m.initialize(serviceName_m.toString(), newList);

		if (result == false) {
			throw new HtException(getContext(), "initDDEServer", "Cannot perform DDE operations");
		}



	}

	private void deinitDDEServer() throws Exception {
		if (ddeServer_m == null) {
			throw new HtException(getContext(), "deinitDDEServer", "Not initialized");
		}

		// stop that
		ddeServer_m.uninitialize2();

		ddeServer_m.destroy();
		ddeServer_m = null;


	}

	private HtRtData parseAndSendRTData(
					SymbolEntry entry,
					String symbol,
					String sTime,
					String sPrice,
					String sVolume,
					String sOperation,
					String sQty
		) throws Exception {
		// custome event


		// parse date time
		LongParam millisec = new LongParam();
		int hour, minute, sec;


		String[] tokens_time = sTime.split(":");
		hour = Integer.valueOf(tokens_time[0]);
		minute = Integer.valueOf(tokens_time[1]);
		sec = Integer.valueOf(tokens_time[2]);

		tm_m = HtDateTimeUtils.yearMonthDayHourMinSec2Time_ReuseCalendar_Gmt(curYear_m, curMonth_m, curDay_m, hour, minute, sec, tm_m, millisec);


		double price = Double.valueOf(sPrice);

		return new HtRtData(
						getProviderId(),
						symbol,
						millisec.getLong(),
						price,
						price,
						Double.valueOf(sQty),
						RtDataPrintColor.CT_NA,
						sOperation.equalsIgnoreCase("BUY") ? RtDataOperation.OT_Buy : RtDataOperation.OT_Sell
		);

	}
}
