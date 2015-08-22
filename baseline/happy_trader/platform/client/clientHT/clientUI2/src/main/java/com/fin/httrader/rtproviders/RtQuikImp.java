/*
 * RtQuikImp.java
 *
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.rtproviders;


import com.fin.httrader.hlpstruct.BackgroundJobStatus;
import com.fin.httrader.interfaces.HtRealTimeProvider;
import com.fin.httrader.utils.win32.HtSysPowerEventListener;
import com.fin.httrader.managers.RtGlobalEventManager;
import com.fin.httrader.managers.RtRealTimeProviderManager;
import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.hlpstruct.HtLevel2Data;
import com.fin.httrader.utils.hlpstruct.HtRtData;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import com.fin.httrader.utils.hlpstruct.XmlHandler;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import com.fin.httrader.utils.win32.DDEItem;
import com.fin.httrader.utils.win32.HtDDEServer;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
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
 * @author Victor_Zoubok
 */
public class RtQuikImp implements HtRealTimeProvider, HtSysPowerEventListener {

	private static final String PARAM_FILE_NAME = "PARAM_FILE_NAME_PATH";
	private BackgroundJobStatus status_m = new BackgroundJobStatus();

	public BackgroundJobStatus returnProviderStatus() {
		return status_m;
	}

	@Override
	public void returnAvailableSymbolList(List<String> symbols) {
		throw new UnsupportedOperationException("Not supported yet.");
	}

	// helper class
	// arrived level 2data to have indexed access to symbol entry
	private class Level2SymbolEntry {

		public int bids_prices_x_m = -1;
		public int asks_prices_x_m = -1;
		public int bids_volumes_x_m = -1;
		public int asks_volumes_x_m = -1;
		public int self_bid_x_m = -1;
		public int self_ask_x_m = -1;
		// --------------------------------
		public StringBuilder symbol_m = new StringBuilder();
		public StringBuilder provider_m = new StringBuilder();
		public StringBuilder name_bids_prices_x_m = new StringBuilder();
		public StringBuilder name_asks_prices_x_m = new StringBuilder();
		public StringBuilder name_bids_volumes_x_m = new StringBuilder();
		public StringBuilder name_asks_volumes_x_m = new StringBuilder();
		public StringBuilder name_self_bid_x_m = new StringBuilder();
		public StringBuilder name_self_ask_x_m = new StringBuilder();

		// -------------------------------
		// -------------------------------
		public boolean isValid() {
			return (bids_prices_x_m >= 0 && asks_prices_x_m >= 0 && bids_volumes_x_m >= 0 && asks_volumes_x_m >= 0);
		}
	};

	// ---------------------------------------------
	// helper class
	// arrived data to have indexed access to symbol entry
	private class SymbolEntry {

		// x coordinates
		public int bid_x_m = -1;
		public int ask_x_m = -1;
		public int volume_x_m = -1;
		public int date_x_m = -1;
		public int time_x_m = -1;
		public int symbol_id_x_m = -1;
		public boolean time_only_m = false;
		// -------------
		private long msecCounter_m = 1;
		// string values to initially resolve this stuff
		public StringBuilder name_bid_x_m = new StringBuilder();
		public StringBuilder name_ask_x_m = new StringBuilder();
		public StringBuilder name_volume_x_m = new StringBuilder();
		public StringBuilder name_date_x_m = new StringBuilder();
		public StringBuilder name_time_x_m = new StringBuilder();
		public StringBuilder name_symbol_id_x_m = new StringBuilder();
		public StringBuilder name_symbol_row_y_m = new StringBuilder();
		public StringBuilder symbol_m = new StringBuilder();
		public StringBuilder provider_m = new StringBuilder();
		public SimpleDateFormat dateF_m = new SimpleDateFormat();
		public SimpleDateFormat timeF_m = new SimpleDateFormat();

		public boolean isValid() {
			if (time_only_m) {
				return (bid_x_m >= 0 && ask_x_m >= 0 && volume_x_m >= 0 && time_x_m >= 0 && symbol_id_x_m >= 0 /*&& total_y_m > 0*/);
			} else {
				return (bid_x_m >= 0 && ask_x_m >= 0 && volume_x_m >= 0 && time_x_m >= 0 && symbol_id_x_m >= 0 && date_x_m >= 0 /*&& total_y_m > 0*/);
			}
		}

		public long getDummyMsec() {
			long result = msecCounter_m++;

			if (msecCounter_m >= 800) {
				msecCounter_m = 1;
			}

			return result;
		}
	};
	// ---------------------------------------
	// timeshift
	private long timeShift_m = 0;
	private StringBuilder name_m = new StringBuilder();
	private HashMap<String, String> initData_m = new HashMap<String, String>();
	// instance of DDE listener
	private HtDDEServer ddeServer_m = null;
	private StringBuilder serviceName_m = new StringBuilder();
	private Vector<DDEItem> itemList_m = new Vector<DDEItem>();
	// entry which resolves SymbolEntry based upon arrived topic
	// when we resolve SymbolEntry structure we can find  SymbolEntry
	// and when we get index structures there and directly access our arrived XLTable structure
	// we will get our data
	// that's it!!!
	private HashMap<String, Vector<SymbolEntry>> dataMap_m = new HashMap<String, Vector<SymbolEntry>>();
	// this is level2 data map
	// each topic to be mapped to the single Level2SymbolEntry entry!!!
	private HashMap<String, Level2SymbolEntry> dataLevel2Map_m = new HashMap<String, Level2SymbolEntry>();
	//private XmlEvent rtDataEvent_m = new XmlEvent();
	//private XmlEvent level2DataEvent_m = new XmlEvent();

	// ---------------------------------------------------
	/**
	 * Creates a new instance of RtQuikImp
	 */
	public RtQuikImp() {
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

		String paramFileName = initdata.get(PARAM_FILE_NAME);
		if (paramFileName == null) {
			throw new HtException(getContext(), "initialize", PARAM_FILE_NAME + " is invalid");
		}

		status_m.initializeOk();
		readParametersFile(paramFileName);
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

	// -----------------------------------
	// DDE data arrived
	public void DDEDataArrived(DDEItem item, String value) {
		//System.out.println("DDE arrived: "+item.getItem() + " - " + item.getTopic() + " : " + value );
	}

	/*
	public String getLibPath() {
	String libPathName = HtMain.getBaseDirectory() + "/bin/htjava_dde_server_bck.dll";
	return libPathName;
	}
	 */
	public void onDDEXLTableDataArrived(DDEItem item, Vector<Vector<String>> data) {

		String topic = item.getTopic();
		String sitem = item.getItem();

		//System.out.println("DDE XLTABLE arrived: "+item.getItem() + " - " + item.getTopic() );

		if (dataMap_m.containsKey(topic)) {
			// we have our data
			Vector<SymbolEntry> entryList = dataMap_m.get(topic);

			for (int m = 0; m < entryList.size(); m++) {
				SymbolEntry entry = entryList.get(m);

				if (!entry.isValid()) {
					// try to initialize this entry
					if (data.size() > 0) {
						// 1st row - check if it contains header
						if (sitem.toUpperCase().startsWith("R1C1:")) {
							// this is ours
							Vector<String> row_1 = data.get(0);
							HashMap<String, Integer> columnMap = new HashMap<String, Integer>();

							for (int i = 0; i < row_1.size(); i++) {
								columnMap.put(row_1.get(i), i);
							}
							//

							if (columnMap.containsKey(entry.name_ask_x_m.toString())) {
								entry.ask_x_m = columnMap.get(entry.name_ask_x_m.toString());
							}

							if (columnMap.containsKey(entry.name_bid_x_m.toString())) {
								entry.bid_x_m = columnMap.get(entry.name_bid_x_m.toString());
							}

							if (columnMap.containsKey(entry.name_date_x_m.toString())) {
								entry.date_x_m = columnMap.get(entry.name_date_x_m.toString());
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


						}
					}
					; // data

					//
					if (!entry.isValid()) {
						return;
					}

				}

				// try to reap our values
				// for now we query the necessary row as I think the table is not resended completely
				int total_y = -1;
				for (int i = 0; i < data.size(); i++) {
					Vector<String> data_i = data.get(i);
					String symbolValue = data_i.get(entry.symbol_id_x_m);

					if (symbolValue.equalsIgnoreCase(entry.symbol_m.toString())) {
						// that's ours
						total_y = i;

						try {


							parseAndSendRTData(entry,
											entry.date_x_m >= 0 ? data.get(total_y).get(entry.date_x_m) : "",
											data.get(total_y).get(entry.time_x_m),
											data.get(total_y).get(entry.bid_x_m),
											data.get(total_y).get(entry.ask_x_m),
											data.get(total_y).get(entry.volume_x_m));
						} catch (Throwable e) {
							//HtLog.log(Level.WARNING, getContext(), "onDDEXLTableDataArrived", "Exception on delegating RT data: " + e.getMessage());
							XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_ERROR, "Exception on delegating RT data: " + e.getMessage());
						}

						break;
					}

				}
			//



			}
			;
		} // entry is alive

		// check for level2 entry
		if (dataLevel2Map_m.containsKey(topic)) {
			Level2SymbolEntry level2entry_i = dataLevel2Map_m.get(topic);

			if (!level2entry_i.isValid()) {
				// not valid - initialize
				if (sitem.toUpperCase().startsWith("R1C1:")) {
					// expected to be header
					Vector<String> row_1 = data.get(0);
					HashMap<String, Integer> columnMap = new HashMap<String, Integer>();
					for (int i = 0; i < row_1.size(); i++) {
						columnMap.put(row_1.get(i), i);
					}

					// init entries
					if (columnMap.containsKey(level2entry_i.name_bids_prices_x_m.toString())) {
						level2entry_i.bids_prices_x_m = columnMap.get(level2entry_i.name_bids_prices_x_m.toString());
					}

					if (columnMap.containsKey(level2entry_i.name_asks_prices_x_m.toString())) {
						level2entry_i.asks_prices_x_m = columnMap.get(level2entry_i.name_asks_prices_x_m.toString());
					}

					if (columnMap.containsKey(level2entry_i.name_bids_volumes_x_m.toString())) {
						level2entry_i.bids_volumes_x_m = columnMap.get(level2entry_i.name_bids_volumes_x_m.toString());
					}

					if (columnMap.containsKey(level2entry_i.name_asks_volumes_x_m.toString())) {
						level2entry_i.asks_volumes_x_m = columnMap.get(level2entry_i.name_asks_volumes_x_m.toString());
					}

					if (columnMap.containsKey(level2entry_i.name_self_ask_x_m.toString())) {
						level2entry_i.self_ask_x_m = columnMap.get(level2entry_i.name_self_ask_x_m.toString());
					}

					if (columnMap.containsKey(level2entry_i.name_self_bid_x_m.toString())) {
						level2entry_i.self_bid_x_m = columnMap.get(level2entry_i.name_self_bid_x_m.toString());
					}

				}


				// check again
				if (!level2entry_i.isValid()) {
					return;
				}


			}

			// try to get our values
			try {
				parseAndSendLevel2Data(level2entry_i, data);
			} catch (Throwable e) {
				//HtLog.log(Level.WARNING, getContext(), "onDDEXLTableDataArrived", "Exception on delegating Level2 data: " + e.getMessage());
				XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_ERROR, "Exception on delegating Level2 data: " + e.getMessage());

				status_m.setUp(BackgroundJobStatus.STATUS_ERRORED, "Exception on delegating Level2 data: " + e.getMessage());
			}

		}


	}

	// -----------------------------------
	// helpers
	//
	// -----------------------------------
	private void readParametersFile(String paramFile) throws Exception {
		// clear some vars
		itemList_m.clear();
		serviceName_m.setLength(0);

		XmlHandler handler = new XmlHandler();
		File f = new File(paramFile);

		XmlParameter xmlparam = new XmlParameter();

		if (!f.exists()) {
			// create new file
			// and add some dummy info to be sure later what we developed now :)

			xmlparam.setCommandName("quik_dde_service_descriptor");

			xmlparam.setString("dde_service_name", "(happy_trader)");


			Vector<XmlParameter> symbols_list = new Vector<XmlParameter>();

			XmlParameter dummy_symbol = new XmlParameter();
			dummy_symbol.setCommandName("symbol_descriptor");
			dummy_symbol.setString("symbol", "(dummy_symbol_name)");
			dummy_symbol.setString("provider", "(dummy_provider_name)");

			XmlParameter dummy_symbol_rt_section = new XmlParameter();
			dummy_symbol_rt_section.setCommandName("rt_section");

			XmlParameter dummy_symbol_level2_section = new XmlParameter();
			dummy_symbol_level2_section.setCommandName("level2_section");
			//

			// init DDE item & topic (will crate automatically excel like)
			dummy_symbol_rt_section.setString("dde_table_topic", "(dummy_rt_dde_topic)");


			// column ids containing our info
			dummy_symbol_rt_section.setString("cur_time_column_name", "(time_name_column)");
			dummy_symbol_rt_section.setString("cur_date_column_name", "(date_name_column)");
			dummy_symbol_rt_section.setString("bid_price_column_name", "(bid_price_column)");
			dummy_symbol_rt_section.setString("ask_price_column_name", "(ask_price_column)");
			dummy_symbol_rt_section.setString("volume_column_name", "(volume_column)");
			dummy_symbol_rt_section.setString("symbol_column_name", "(symbol_column)");

			// whether we have a single time field - cur_time_column_name
			dummy_symbol_rt_section.setInt("time_only", 0);

			// row id - from which we are going to get our data
			//dummy_symbol_rt_section.setString("symbol_row_name", "(value_of_symbol_column)");

			// time and dates formats
			dummy_symbol_rt_section.setString("time_format", "(time_format)");
			dummy_symbol_rt_section.setString("date_format", "(date_format)");


			// LEVEL 2 SECTION
			dummy_symbol_level2_section.setString("dde_table_topic", "(dummy_level2_dde_topic)");
			dummy_symbol_level2_section.setString("bids_prices_column_name", "(bids_prices_column)");
			dummy_symbol_level2_section.setString("asks_prices_column_name", "(asks_prices_column)");
			dummy_symbol_level2_section.setString("bids_volumes_column_name", "(bids_volumes_column)");
			dummy_symbol_level2_section.setString("asks_volumes_column_name", "(asks_volumes_column)");

			dummy_symbol_level2_section.setString("asks_self_column_name", "(asks_self_column_name)");
			dummy_symbol_level2_section.setString("bids_self_column_name", "(bids_self_column_name)");
		
			//
			dummy_symbol.setXmlParameter("rt_section", dummy_symbol_rt_section);
			dummy_symbol.setXmlParameter("level2_section", dummy_symbol_level2_section);

			//
			symbols_list.add(dummy_symbol);
			symbols_list.add(dummy_symbol);

			xmlparam.setXmlParameterList("symbols_list", symbols_list);
			//
			StringBuilder out = new StringBuilder();
			handler.serializeParameter(xmlparam, out);

			FileOutputStream fout = new FileOutputStream(f);
			fout.write(out.toString().getBytes());
			fout.flush();
			fout.close();

		} else {
			// parse actually
			FileInputStream fi = new FileInputStream(f);
			String indata = HtUtils.convertInputStreamToString(fi, null);


			// todo!
			// need to replace later
			indata = indata.replaceAll("[\t\r\n]", "");

			handler.deserializeParameter(indata, xmlparam);

			// create RT structure
			if (!xmlparam.getCommandName().equalsIgnoreCase("quik_dde_service_descriptor")) {
				throw new HtException(getContext(), "readParametersFile", "Invalid parameter XML content");
			}


			// first of all iterate through all parameters and see the maximum of item names
			serviceName_m.append(xmlparam.getString("dde_service_name"));



			List<XmlParameter> symbols_list = xmlparam.getXmlParameterList("symbols_list");
			for (int i = 0; i < symbols_list.size(); i++) {
				XmlParameter symbol_param_i = symbols_list.get(i);

				// rt section
				XmlParameter rt_section_param_i = symbol_param_i.getXmlParameter("rt_section");


				String topicName = rt_section_param_i.getString("dde_table_topic");
				if (topicName == null || topicName.length() <= 0) {
					throw new HtException(getContext(), "readParametersFile", "Invalid DDE topic for RT");
				}


				// add to item list
				Vector<DDEItem> itemListLocal = new Vector<DDEItem>();

				// add to global list
				itemListLocal.add(new DDEItem(topicName, "R1C1:R1C1"));
				itemList_m.addAll(itemListLocal);

				// init structures
				SymbolEntry entry_i = new SymbolEntry();
				Level2SymbolEntry level2entry_i = new Level2SymbolEntry();

				String provider = symbol_param_i.getString("provider");
				String symbol = symbol_param_i.getString("symbol");

				if (provider == null || provider.length() == 0) {
					throw new HtException(getContext(), "readParametersFile", "Invalid provider");
				}

				if (symbol == null || symbol.length() == 0) {
					throw new HtException(getContext(), "readParametersFile", "Invalid symbol");
				}



				// rt section
				String cur_time_column_name = rt_section_param_i.getString("cur_time_column_name");
				String cur_date_column_name = rt_section_param_i.getString("cur_date_column_name");
				String bid_price_column_name = rt_section_param_i.getString("bid_price_column_name");
				String ask_price_column_name = rt_section_param_i.getString("ask_price_column_name");
				String volume_column_name = rt_section_param_i.getString("volume_column_name");
				String symbol_column_name = rt_section_param_i.getString("symbol_column_name");

				//String symbol_row_name = rt_section_param_i.getString("symbol_row_name");

				String date_format = rt_section_param_i.getString("date_format");
				String time_format = rt_section_param_i.getString("time_format");
				boolean time_only = (rt_section_param_i.getInt("time_only") == 0 ? false : true);

				if (time_format == null || time_format.length() == 0) {
					throw new HtException(getContext(), "readParametersFile", "Invalid time format");
				}

				if (time_only) {
					
					entry_i.timeF_m.applyPattern(time_format);
					entry_i.timeF_m.setTimeZone(TimeZone.getTimeZone("GMT"));

				} else {
					if (date_format == null || date_format.length() == 0) {
						throw new HtException(getContext(), "readParametersFile", "Invalid date format");
					}

					entry_i.dateF_m.applyPattern(date_format);
					entry_i.timeF_m.applyPattern(time_format);

					entry_i.dateF_m.setTimeZone(TimeZone.getTimeZone("GMT"));
					entry_i.timeF_m.setTimeZone(TimeZone.getTimeZone("GMT"));
				}

				if (cur_time_column_name.length() == 0 ||
								cur_date_column_name.length() == 0 ||
								bid_price_column_name.length() == 0 ||
								ask_price_column_name.length() == 0 ||
								volume_column_name.length() == 0 /*||
								symbol_column_name.length()==0  */) {
					throw new HtException(getContext(), "readParametersFile", "Some important data are empty for RT");
				}


				entry_i.provider_m.append(provider);
				entry_i.symbol_m.append(symbol);
				entry_i.name_time_x_m.append(cur_time_column_name);
				entry_i.name_date_x_m.append(cur_date_column_name);
				entry_i.name_bid_x_m.append(bid_price_column_name);
				entry_i.name_ask_x_m.append(ask_price_column_name);
				entry_i.name_volume_x_m.append(volume_column_name);
				entry_i.name_symbol_id_x_m.append(symbol_column_name);
				entry_i.time_only_m = time_only;


				// other fields to be insert later when real data arrives
				if (dataMap_m.containsKey(topicName)) {
					Vector<SymbolEntry> symbolList = dataMap_m.get(topicName);
					symbolList.add(entry_i);
				} else {
					Vector<SymbolEntry> symbolList = new Vector<SymbolEntry>();
					symbolList.add(entry_i);

					dataMap_m.put(topicName, symbolList);
				}

				// level 2 section
				// this is a separate story
				XmlParameter level2_section_param_i = symbol_param_i.getXmlParameter("level2_section");

				String topicNameLevel2 = level2_section_param_i.getString("dde_table_topic");
				if (topicNameLevel2 == null || topicNameLevel2.length() <= 0) {
					throw new HtException(getContext(), "readParametersFile", "Invalid DDE topic for Level2");
				}

				itemListLocal.add(new DDEItem(topicNameLevel2, "R1C1:R1C1"));
				itemList_m.addAll(itemListLocal);

				//
				String bids_prices_column_name = level2_section_param_i.getString("bids_prices_column_name");
				String asks_prices_column_name = level2_section_param_i.getString("asks_prices_column_name");
				String bids_volumes_column_name = level2_section_param_i.getString("bids_volumes_column_name");
				String asks_volumes_column_name = level2_section_param_i.getString("asks_volumes_column_name");

				String ask_self_column_name = level2_section_param_i.getString("asks_self_column_name");
				String bid_self_column_name = level2_section_param_i.getString("bids_self_column_name");


				// check if empty
				if (bids_prices_column_name.length() == 0 ||
								asks_prices_column_name.length() == 0 ||
								bids_volumes_column_name.length() == 0 ||
								asks_volumes_column_name.length() == 0) {
					throw new HtException(getContext(), "readParametersFile", "Some important parameters for Level2 are empty");
				}

				// init level2 struct
				level2entry_i.provider_m.append(provider);
				level2entry_i.symbol_m.append(symbol);

				level2entry_i.name_bids_prices_x_m.append(bids_prices_column_name);
				level2entry_i.name_asks_prices_x_m.append(asks_prices_column_name);
				level2entry_i.name_bids_volumes_x_m.append(bids_volumes_column_name);
				level2entry_i.name_asks_volumes_x_m.append(asks_volumes_column_name);

				level2entry_i.name_self_ask_x_m.append(ask_self_column_name);
				level2entry_i.name_self_bid_x_m.append(bid_self_column_name);

				// insert entry
				dataLevel2Map_m.put(topicNameLevel2, level2entry_i);

			} // end of symbols list iteration



		}
	}

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

	private void parseAndSendLevel2Data(Level2SymbolEntry entryLevel2, Vector<Vector<String>> data) throws Exception {
		if (data.size() <= 0) // must be at least 1 row
		{
			return;
		}

		HtLevel2Data rtlevel2data = new HtLevel2Data();
		rtlevel2data.setProvider(getProviderId());
		rtlevel2data.setSymbol(entryLevel2.symbol_m.toString());

		// the first row is symbol names
		rtlevel2data.init(data.size());
		int k = 0;
		for (int i = 1; i < data.size(); i++) {
			Vector<String> data_i = data.get(i);

			double ask_volume = stringToDouble(data_i.get(entryLevel2.asks_volumes_x_m));
			double bid_volume = stringToDouble(data_i.get(entryLevel2.bids_volumes_x_m));
			double bid_price = stringToDouble(data_i.get(entryLevel2.bids_prices_x_m));
			double ask_price = stringToDouble(data_i.get(entryLevel2.asks_prices_x_m));

			String ask_self = data_i.get(entryLevel2.self_ask_x_m);
			String bid_self = data_i.get(entryLevel2.self_bid_x_m);

			boolean is_ask_self = (stringToDouble(ask_self) == 0.0 ? false : true);
			boolean is_bid_self = (stringToDouble(bid_self) == 0.0 ? false : true);


			
			if (ask_volume == 0) {
				// these are bids
				HtLevel2Data.Htlevel2Entry e = new HtLevel2Data.Htlevel2Entry(bid_volume, bid_price, is_bid_self, HtLevel2Data.Htlevel2Entry.IS_BID);
				rtlevel2data.getData()[k++] = e;
				
			} else if (bid_volume == 0) {
				// these are offers
				HtLevel2Data.Htlevel2Entry e = new HtLevel2Data.Htlevel2Entry(ask_volume, ask_price, is_ask_self, HtLevel2Data.Htlevel2Entry.IS_ASK);
				rtlevel2data.getData()[k++] = e;
			}

		}

		HtLog.log(Level.INFO, getContext(), "parseAndSendRTData", "Level2 data arrived: " + rtlevel2data.toString());

		//
		XmlEvent level2DataEvent_m = new XmlEvent();
		RtRealTimeProviderManager.createXmlEventLevel2(rtlevel2data, level2DataEvent_m);
		RtGlobalEventManager.instance().pushCommonEvent(level2DataEvent_m);


	}

	private void parseAndSendRTData(SymbolEntry entry, String sDate, String sTime, String sBid, String sAsk, String sVolume) throws Exception {
		HtRtData rtdata = new HtRtData();
		Date timeParsed = entry.timeF_m.parse(sTime);

		rtdata.setProvider(getProviderId());
		rtdata.setSymbol(entry.symbol_m.toString());
		rtdata.type_m = HtRtData.RT_Type;

		if (!entry.time_only_m) {
			Date dateParsed = entry.dateF_m.parse(sDate);
			rtdata.time_m = dateParsed.getTime() + timeParsed.getTime();
		} else {

			// GMT time zone - will adjust manually
			Calendar calendar = Calendar.getInstance(TimeZone.getTimeZone("GMT"));

			//set up only as dat
			calendar.set(Calendar.HOUR, 0);
			calendar.set(Calendar.MILLISECOND, 0);
			calendar.set(Calendar.SECOND, 0);
			calendar.set(Calendar.MILLISECOND, 0);

			rtdata.time_m = calendar.getTimeInMillis() + timeParsed.getTime();
		}

		rtdata.time_m += entry.getDummyMsec();

		rtdata.ask_m = Double.valueOf(sAsk);
		rtdata.bid_m = Double.valueOf(sBid);
		rtdata.volume_m = Double.valueOf(sVolume);
		//

		//HtLog.log(Level.INFO, getContext(), "parseAndSendRTData", "RT data arrived: " + rtdata.toString());

		XmlEvent rtDataEvent_m = new XmlEvent();
		RtRealTimeProviderManager.createXmlEventRtData(rtdata, rtDataEvent_m);
		RtGlobalEventManager.instance().pushCommonEvent(rtDataEvent_m);


	}

	// convert empty value to 0
	private double stringToDouble(String val) throws Exception {
		if (val == null || val.length() == 0) {
			return 0;
		}

		return Double.valueOf(val);
	}

	private int stringToInt(String val) throws Exception {
		if (val == null || val.length() == 0) {
			return 0;
		}

		return Integer.valueOf(val);
	}
}
