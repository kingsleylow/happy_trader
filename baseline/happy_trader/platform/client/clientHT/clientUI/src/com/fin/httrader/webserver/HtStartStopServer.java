/*
 * HtStartStopServer.java
 *
  *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.configprops.HtServerProp;
import com.fin.httrader.managers.RtConfigurationManager;
import com.fin.httrader.managers.RtTradingServerManager;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.model.HtServerProxy;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.hlpstruct.HtPair;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.hlpstruct.XmlHandler;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import com.fin.httrader.webserver.GridDescriptor.GridDataSet;
import com.fin.httrader.webserver.GridDescriptor.GridFieldDescriptor;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
import java.util.TreeSet;
import java.util.Vector;
import java.util.logging.Level;
import javax.servlet.ServletException;
import javax.servlet.ServletOutputStream;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtStartStopServer extends HtServletsBase {

	// helper class to store the page
	private class ViewStorage {

		public ViewStorage(ViewStorage src) {
			DoReuseCache = src.DoReuseCache;
			DoLaunchProcess = src.DoLaunchProcess;

			SelectedProfiles.clear();
			SelectedProfiles.putAll(src.SelectedProfiles);
			selectedTimePeriod = src.selectedTimePeriod;

			SelectedRtProviders.clear();
			SelectedRtProviders.addAll(src.SelectedRtProviders);
		}

		public ViewStorage() {
		}

		public Map<String, List<HtPair<String, String>>> resolveSelectedProfiles()
		{
			Map<String, List<HtPair<String, String>>> result = new TreeMap<String, List<HtPair<String, String>>>();

			for(Iterator<String> it = SelectedProfiles.keySet().iterator(); it.hasNext(); ) {
				String profile = it.next();
				HtServerProxy.HtServerProxy_ProfileEntry entry_i = SelectedProfiles.get(profile);


				result.put(profile,entry_i.providerSymbolList_m );
			}

			return result;
		}

		boolean DoReuseCache = false;
		boolean DoLaunchProcess = true;
		Map<String, HtServerProxy.HtServerProxy_ProfileEntry> SelectedProfiles = new TreeMap<String, HtServerProxy.HtServerProxy_ProfileEntry>();
		Set<String> SelectedRtProviders = new TreeSet<String>();
		String selectedTimePeriod = "";
	}

	/** Creates a new instance of HtStartStopServer */
	public HtStartStopServer() {
	}

	public String getContext() {
		return this.getClass().getSimpleName();
	}

	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		try {
			// generate new Uid and set initial values
			this.generateUniquePageId();


			//
			readServerData(req);


		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			
			return false;
		}
		
		return true;
	}

	public boolean initialize_Post(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		try {
			this.setUniquePageId(getStringParameter(req, "page_uid", false));



			String operation = getStringParameter(req, "operation", false);

			if (operation.equals("start_server")) {
				startTradingServer(req);
			} else if (operation.equals("shutdown_server")) {
				stopTradingServer(req);
			} else if (operation.equals("kill_server")) {
				killTradingServer(req);
			} else if (operation.equals("refresh")) {
				// no-op
			} else if (operation.equalsIgnoreCase("do_reload_log_events")) {
				// reload default log level
				changeLogLevelAndSubscrEvents(req);


			} else {
				throw new HtException(getContext(), "initialize", "Invalid operation: " + operation);
			}

			readServerData(req);

		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			
			return false;
		}
		
		return true;

	}
	

	// THIS must resolve tmp storage
	private ViewStorage resolveTmpStorage(HttpServletRequest req, boolean doCreate) throws Exception {
		ViewStorage result = null;
		result = (ViewStorage) getObjectSessionValue(getUniquePageId(), req, "tmp_srv_storage");

		if (result == null && doCreate) {
			result = new ViewStorage();
			setObjectSessionValue(getUniquePageId(), req, "tmp_srv_storage", result);
		}

		return result;
	}

	private void startTradingServer(HttpServletRequest req) throws Exception {
		String server_id = getStringParameter(req, "server_id", false);

		ViewStorage tmp_storage = resolveTmpStorage(req, true);
		//
		boolean is_reuse_cache = getBooleanParameter(req, "use_cached_data", false);
		boolean is_lanch_process = getBooleanParameter(req, "do_launch_process", false);
		boolean noSelectAtAll = getBooleanParameter(req, "no_select_at_all", false);

		// do not use localtime adjustment - use GMT
		long load_data_dbeg = HtDateTimeUtils.parseDateTimeParameter(getStringParameter(req, "load_data_dbeg", true), false);
		long load_date_dend = HtDateTimeUtils.parseDateTimeParameter(getStringParameter(req, "load_data_dend", true), false);


		// this is only a list of subscribe providesr
		String history_data_str = getStringParameter(req, "historical_data", true);
		//setStringSessionValue(getUniquePageId(), req, "historical_data", history_data_str);


		XmlParameter history_xmlparameter = XmlHandler.deserializeParameterStatic(history_data_str);

		List<String> providers_subscribe = getStringListParameter(req, "subscribed_providers", true);

		String period_value = getStringParameter(req, "period_value", true);
		tmp_storage.selectedTimePeriod = period_value;

		// store tmp
		tmp_storage.DoReuseCache = is_reuse_cache;
		tmp_storage.DoLaunchProcess = is_lanch_process;

		// this is map
		// this flag to enforce not to generate huge SQL and pass all stuff by default


		HashMap<String, HtServerProxy.HtServerProxy_ProfileEntry> profProvSymbMap = new HashMap<String, HtServerProxy.HtServerProxy_ProfileEntry>();

		List<XmlParameter> historical_xml_list = history_xmlparameter.getXmlParameterList("history_data");

		for (int i = 0; i < historical_xml_list.size(); i++) {

			XmlParameter hist_i = historical_xml_list.get(i);
			String profile_i = hist_i.getCommandName();

			// if it is empty means all
			List<HtPair<String, String>> providerSymbolList = new ArrayList<HtPair<String, String>>();

			// populate
			List<String> data_i = hist_i.getStringList("profile_values");
			int k = 0;
			while (k < data_i.size()) {
				String provider_i = data_i.get(k++);
				String symbol_i = data_i.get(k++);

				providerSymbolList.add(new HtPair<String, String>(provider_i, symbol_i));

			}

			HtServerProxy.HtServerProxy_ProfileEntry profileEntry = new HtServerProxy.HtServerProxy_ProfileEntry();

			profileEntry.defaultSelection_m = false;
			
			// if it is empty get from DB
			if (providerSymbolList.isEmpty()) {
				profileEntry.defaultSelection_m = true;
				HtLog.log(Level.INFO, getContext(), "startTradingServer", "As data was provided by default using DB to retrieve them");
				providerSymbolList.addAll( HtCommandProcessor.instance().get_HtDatabaseProxy().remote_getStoredProvidersSymbols_ForProfile_FromHistory(profile_i) );
			}

			profileEntry.providerSymbolList_m = providerSymbolList;

			profProvSymbMap.put(profile_i, profileEntry);

			// save temporary
			tmp_storage.SelectedProfiles.clear();
			tmp_storage.SelectedProfiles.putAll(profProvSymbMap);
		}

	

		// RT Providers
		Set<String> rtProvidersToSubscribe = new HashSet<String>();
		rtProvidersToSubscribe.addAll(providers_subscribe);

		tmp_storage.SelectedRtProviders.clear();
		tmp_storage.SelectedRtProviders.addAll(providers_subscribe);


		// server property
		HtServerProp prop = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getTradingServerProperty(server_id);

		// add log variable

		// attempt to start server
	

		HtCommandProcessor.instance().get_HtServerProxy().remote_startServerProcessWithHistoryData(
						server_id,
						prop.getServerExecutable(),
						prop.getDebuglevel(),
						prop.getServerHost(),
						prop.getCommandPort(),
						RtConfigurationManager.StartUpConst.COMMON_SERVICE_LISTEN_PORT,
						prop.getSpawnerPort(),
						prop.isUseSpawner(),
						is_lanch_process,
						load_data_dbeg,
						load_date_dend,
						profProvSymbMap, // map profile <-> ( provider - symbol)
						rtProvidersToSubscribe,
						is_reuse_cache,
						prop.getLogEventLevel(),
						prop.getSubscribedEvents(),
						noSelectAtAll
				);

	}

	private void stopTradingServer(HttpServletRequest req) throws Exception {
		String server_id = getStringParameter(req, "server_id", false);
		HtCommandProcessor.instance().get_HtServerProxy().remote_shutDownServerProcess(server_id);
	}

	private void killTradingServer(HttpServletRequest req) throws Exception {
		String server_id = getStringParameter(req, "server_id", false);
		HtCommandProcessor.instance().get_HtServerProxy().remote_killServerProcess(server_id);
	}

	private void readServerData(HttpServletRequest req) throws Exception {

		if (HtCommandProcessor.instance().get_HtServerProxy().isBusyWithLengthOperation()) {
			throw new HtException(getContext(), "readServerData", "Server is busy with lengthy operation: " +
							HtCommandProcessor.instance().get_HtServerProxy().getLengthyOperationDescription());
		}


		ViewStorage tmp_storage = resolveTmpStorage(req, false);

		// profile provider map
		if (tmp_storage != null) {
			setStringSessionValue(getUniquePageId(), req, "symbols_for_each_profile", HtWebUtils.createJScriptForProfileProviderMap(tmp_storage.resolveSelectedProfiles()));
		}
		else {
			setStringSessionValue(getUniquePageId(), req, "symbols_for_each_profile", HtWebUtils.createJScriptForProfileProviderMap(new HashMap<String,  List<HtPair<String, String>>>()));
		}

		StringBuilder out = new StringBuilder();

		// server list
		Set<String> server = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getTradingServerList();

		GridDataSet dataset = new GridDataSet(
						new GridFieldDescriptor[]{
							new GridFieldDescriptor("Server Name"),
							new GridFieldDescriptor("Status"),
							new GridFieldDescriptor("Action"),
							new GridFieldDescriptor("Server Information"),
							new GridFieldDescriptor("Log/Event Levels"),
							new GridFieldDescriptor("Sever Internal log"),
							new GridFieldDescriptor("Memory cache management")});




		int cnt = 0;
		for (Iterator<String> it = server.iterator(); it.hasNext();) {
			String server_i = it.next();
			dataset.addRow();

			int server_status = HtCommandProcessor.instance().get_HtServerProxy().remote_returnServerStatus(server_i);

			dataset.setRowValue(cnt, 0, server_i);


			if (RtTradingServerManager.SERVER_IDLE == server_status) {

				dataset.setRowValue(cnt, 1, "Idle");
			} else if (RtTradingServerManager.SERVER_RUN_OK == server_status) {

				dataset.setRowValue(cnt, 1, "Running");
			} else if (RtTradingServerManager.SERVER_RTTHREAD_ERROR == server_status) {

				dataset.setRowValue(cnt, 1, "Error");
			}

			out.setLength(0);
			if (RtTradingServerManager.SERVER_IDLE == server_status) {
				
				//out.append("<a href='#' onclick='do_start_server(\"");
				//out.append(server_i);
				//out.append("\");'>Start</a>");
				
				out.append( HtWebUtils.createClickHtml("do_start_server(\"" + server_i + "\")" ,"Start") );
				
			} else if (RtTradingServerManager.SERVER_RUN_OK == server_status) {
				// kill or shutdown
				//out.append("<a href='#' onclick='do_shutdown_server(\"");
				//out.append(server_i);
				//out.append("\");'>Shutdown</a>");
				
				out.append( HtWebUtils.createClickHtml("do_shutdown_server(\"" + server_i + "\")" ,"Shutdown") );
				
				out.append("&nbsp;");
				
				//out.append("<a href='#' onclick='do_kill_server(\"");
				//out.append(server_i);
				//out.append("\");'>Kill</a>");
				
				out.append( HtWebUtils.createClickHtml("do_kill_server(\"" + server_i + "\")" ,"Kill") );
				
			} else if (RtTradingServerManager.SERVER_RTTHREAD_ERROR == server_status) {
				// kill only
				//out.append("<a href='#' onclick='do_kill_server(\"");
				//out.append(server_i);
				//out.append("\");'>Kill</a>");
				
				out.append( HtWebUtils.createClickHtml("do_kill_server(\"" + server_i + "\")" ,"Kill") );
			}

			dataset.setRowValue(cnt, 2, out.toString());

			out.setLength(0);
			if (RtTradingServerManager.SERVER_IDLE != server_status) {
				
				//out.append("<a href='#' onclick='do_server_info(\"");
				//out.append(server_i);
				//out.append("\");'>Click</a>");
				
				out.append( HtWebUtils.createClickHtml("do_server_info(\"" + server_i + "\")" ,"Click") );

			} else {
				out.append("Idle");
			}
			dataset.setRowValue(cnt, 3, out.toString());

			//
			out.setLength(0);
			if (RtTradingServerManager.SERVER_IDLE != server_status) {
				//out.append("<a href='#' onclick='do_log_event_level(\"");
				//out.append(server_i);
				//out.append("\")'>Click</a>");
				
				out.append( HtWebUtils.createClickHtml("do_log_event_level(\"" + server_i + "\")", "Click"));
				
			} else {
				out.append("Idle");
			}

			dataset.setRowValue(cnt, 4, out.toString());

			// internal log
			out.setLength(0);
			if (RtTradingServerManager.SERVER_IDLE != server_status) {

				//out.append("<a href='#' onclick='do_show_internal_log(\"");
				//out.append(server_i);
				//out.append("\")'>Click</a>");
				
				out.append( HtWebUtils.createClickHtml("do_show_internal_log(\"" + server_i + "\")", "Click"));

			} else {
				out.append("Idle");
			}
			dataset.setRowValue(cnt, 5, out.toString());

			// memory cache management
			out.setLength(0);
			if (RtTradingServerManager.SERVER_IDLE != server_status) {
				//out.append("<a href='#' onclick='do_memory_cache_management(\"");
				//out.append(server_i);
				//out.append("\")'>Click</a>");
				
				out.append( HtWebUtils.createClickHtml("do_memory_cache_management(\"" + server_i + "\")", "Click"));
				
				
			}else {
				out.append("Idle");
			}
			dataset.setRowValue(cnt, 6, out.toString());



			cnt++;
		}

		setStringSessionValue(getUniquePageId(), req, "server_list", HtWebUtils.createHTMLForDhtmlGrid(dataset));



		// now need to data profiles
		//Map<String, List<ProviderSymbolInfoEntry>> profProvSymbMap = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_queryProfileProviderSymbolFromDb_ForProfile_WithDates();

		Set<String> dataprofiles = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRegisteredDataProfilesList();

		GridDataSet dataset_data = new GridDataSet(
						new GridFieldDescriptor[]{
							new GridFieldDescriptor("Check", "ch", "", "center"),
							new GridFieldDescriptor("Data Profile Name"),
							new GridFieldDescriptor("Select Symbols")
						});




		cnt = 0;
		for (Iterator<String> it = dataprofiles.iterator(); it.hasNext();) {
			String profile_i = it.next();

			dataset_data.addRow();

			boolean found = false;
			if (tmp_storage != null) {
				found = tmp_storage.SelectedProfiles.containsKey(profile_i);
			}

			dataset_data.setRowValue(cnt, 0, found ? "1" : "0");
			dataset_data.setRowValue(cnt, 1, profile_i);
			dataset_data.setRowValue(cnt, 2, "<a href='#' onclick='do_select_symbols(\"" + profile_i + "\")'>Click</a>");
			cnt++;

		}


		setStringSessionValue(getUniquePageId(), req, "server_data_history_table", HtWebUtils.createHTMLForDhtmlGrid(dataset_data));


		// server rt providers!!!
		// to subscribe

		GridDataSet dataset_providers = new GridDataSet(
						new GridFieldDescriptor[]{
							new GridFieldDescriptor("Check", "ch", "", "center"),
							new GridFieldDescriptor("RT Provider"),});



		// still not sure if we need the full list of providers
		Set<String> providers = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRegisteredRTProvidersList();



		cnt = 0;
		for (Iterator<String> prov_it = providers.iterator(); prov_it.hasNext();) {
			String provider_i = prov_it.next();

			dataset_providers.addRow();



			boolean found = false;
			if (tmp_storage != null) {
				if (tmp_storage.SelectedRtProviders.contains(provider_i)) {
					found = true;
				}
			}


			dataset_providers.setRowValue(cnt, 0, found ? "1" : "0");
			dataset_providers.setRowValue(cnt, 1, provider_i);

			cnt++;
		}

		setStringSessionValue(getUniquePageId(), req, "server_rt_providers", HtWebUtils.createHTMLForDhtmlGrid(dataset_providers));

		//
		if (tmp_storage != null) {
			setStringSessionValue(getUniquePageId(), req, "is_reuse_cached_data", String.valueOf(tmp_storage.DoReuseCache));
			setStringSessionValue(getUniquePageId(), req, "is_launch_process", String.valueOf(tmp_storage.DoLaunchProcess));
			setStringSessionValue(getUniquePageId(), req, "period_value_index", tmp_storage.selectedTimePeriod);

		}
	}

	private void changeLogLevelAndSubscrEvents(HttpServletRequest req) throws Exception {
		String cur_server_id = getStringParameter(req, "server_id", false);
		List<String> events = getStringListParameter(req, "new_subscr_events", true);

		HashMap<Integer, Boolean> subscrmap = new HashMap<Integer, Boolean>();
		for (int i = 0; i < events.size(); i++) {
			int event_i = Integer.valueOf(events.get(i));
			subscrmap.put(event_i, true);
		}


		HtCommandProcessor.instance().get_HtServerProxy().remote_setAllowedEvents(cur_server_id, subscrmap);

		int log_level = (int) getIntParameter(req, "new_log_level", false);
		HtCommandProcessor.instance().get_HtServerProxy().remote_setlogLevel(cur_server_id, log_level);
	}
}
