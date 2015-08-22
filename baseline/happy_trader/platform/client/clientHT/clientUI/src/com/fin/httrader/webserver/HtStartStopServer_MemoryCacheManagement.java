/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.configprops.HtSimulationProfileProp;
import com.fin.httrader.hlpstruct.TSAggregationPeriod;
import com.fin.httrader.hlpstruct.TSGenerateTickMethod;
import com.fin.httrader.hlpstruct.TickUsage;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.LongParam;
import com.fin.httrader.utils.Uid;
import com.fin.httrader.utils.hlpstruct.HtPair;
import com.fin.httrader.webserver.GridDescriptor.GridDataSet;
import com.fin.httrader.webserver.GridDescriptor.GridFieldDescriptor;
import com.google.gson.Gson;
import com.google.gson.reflect.TypeToken;
import java.io.IOException;
import java.io.OutputStream;
import java.lang.reflect.Type;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
import java.util.logging.Level;
import javax.servlet.ServletException;
import javax.servlet.ServletOutputStream;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import jsync.Event;

/**
 *
 * @author DanilinS
 */
public class HtStartStopServer_MemoryCacheManagement extends HtServletsBase {

	public String getContext() {
		return this.getClass().getSimpleName();
	}

	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);


		try {
			// new UID always
			generateUniquePageId();

			String server_id = getStringParameter(req, "server_id", false);
			setStringSessionValue(getUniquePageId(), req, "trading_server_name", server_id);

			readMemoryCachedSymbols(req, server_id);

			readSimulationProfiles(req);


		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			
			return false;
		}
		
		return true;

	}

	public boolean initialize_Post(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {

		// all this works as a servlet - just return what it has here
		HtLog.log(Level.WARNING, getContext(), "initialize_Post", "Starting...");

		res.setContentType("text/xml; charset=UTF-8");
		forceNoCache(res);

		HtAjaxCaller ajaxCaller = new HtAjaxCaller();

		// all these can be lengthy start another thread
		final ServletOutputStream ou = res.getOutputStream();
		final Event shEvent = new Event();
		Thread th = new Thread()
		{
			public void run()
			{
				while(true){
					try{
						ou.print(" ");
						//HtLog.log(Level.INFO, "Helper Thread", "run", "pushed whitespace to the stream");

						if (shEvent.waitEvent(1000)){
							break;
						}
					}
					catch(Throwable e)
					{
						HtLog.log(Level.WARNING, "Helper Thread", "run", "Exception: " + e.getMessage());
						break;
					}
				}

				HtLog.log(Level.INFO, "Helper Thread", "run", "Finishing helping thread");
			}
		};



		try {

			th.start();

			setUniquePageId(getStringParameter(req, "page_uid", false));
			String sim_profile = getStringParameter(req, "sim_profile_name", false);
			String operation = getStringParameter(req, "operation", false);

			if (operation.equalsIgnoreCase("read_sim_profile_data")) {
				readSimulationProfileData(ajaxCaller, sim_profile);
			} else if (operation.equalsIgnoreCase("create_sim_ticks")) {
				String server_id = getStringParameter(req, "server_id", false);

				String begDateStr = getStringParameter(req, "bdate", true);
				String endDateStr = getStringParameter(req, "edate", true);
				String idString = getStringParameter(req, "id_string", true);

				long begDate = HtDateTimeUtils.parseDateTimeParameter(begDateStr, false);
				long endDate = HtDateTimeUtils.parseDateTimeParameter(endDateStr, false);


				List<HtPair<String, String>> provSymbListParticipate = new ArrayList<HtPair<String, String>>();

				Gson gson = new Gson();
				String plist = getStringParameter(req, "plist", false);
				Type collectionType = new TypeToken<List<List<String>>>(){}.getType();
				List<List<String>> extracted = gson.fromJson(plist, collectionType);
				for (int i = 0; i < extracted.size(); i++) {
					List<String> e_i = extracted.get(i);
					HtPair<String, String> pair_i = new HtPair<String, String>(e_i.get(0), e_i.get(1));
					provSymbListParticipate.add(pair_i);
				}

				createSimulationTicks(ajaxCaller, server_id, sim_profile, provSymbListParticipate, begDate, endDate, idString);
			} else {
				throw new HtException(getContext(), "initialize_Post", "Invalid operation: " + operation);
			}


		} catch (Throwable e) {
			ajaxCaller.setError(-1, e.getMessage());
			HtLog.log(Level.WARNING, getContext(), "initialize_Post", "Exception in processing: " + e.getMessage());
		} finally {
			shEvent.signal();
			try {
				th.join();
			}
			catch(InterruptedException e)
			{
			}
		};
		
		ajaxCaller.serializeToXml(res);
		
		HtLog.log(Level.INFO, getContext(), "initialize_Post", "Finished");
		
		return true;
	}

	/**
	 * Helpers
	 */
	private void readSimulationProfiles(HttpServletRequest req) throws Exception {
		// next all profiles
		Set<String> simprofiles = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getSimulationProfileList();
		setStringSessionValue(getUniquePageId(), req, "sim_profile_list", HtWebUtils.createOptionList(null, simprofiles, false));
	}

	private void readMemoryCachedSymbols(HttpServletRequest req, String server_id) throws Exception {
		// first reap all cached data from the main cache
		LongParam min_total_time = new LongParam();
		LongParam max_total_time = new LongParam();

		Map<String, Map<String, HtPair<Long, Long>>> cachedSymbols = new TreeMap<String, Map<String, HtPair<Long, Long>>>();
		HtCommandProcessor.instance().get_HtServerProxy().remote_getHistoryContextCachedSymbols(
						server_id, Uid.generateInvalidUid(), cachedSymbols, min_total_time, max_total_time);

		GridDataSet dataset = new GridDataSet(
						new GridFieldDescriptor[]{
							new GridFieldDescriptor("Provider"),
							new GridFieldDescriptor("Symbol"),
							new GridFieldDescriptor("Time Begin"),
							new GridFieldDescriptor("Time End")
						});

		int cnt = 0;
		for (Iterator<String> it = cachedSymbols.keySet().iterator(); it.hasNext();) {
			String provider = it.next();
			Map<String, HtPair<Long, Long>> timeSpanMap = cachedSymbols.get(provider);

			for (Iterator<String> it2 = timeSpanMap.keySet().iterator(); it2.hasNext();) {
				String symbol = it2.next();

				HtPair<Long, Long> timeSpan = timeSpanMap.get(symbol);
				dataset.addRow();

				dataset.setRowValue(cnt, 0, provider);
				dataset.setRowValue(cnt, 1, symbol);
				dataset.setRowValue(cnt, 2, HtDateTimeUtils.time2SimpleString_Gmt(timeSpan.first));
				dataset.setRowValue(cnt, 3, HtDateTimeUtils.time2SimpleString_Gmt(timeSpan.second));

				cnt++;
			}
		}

		setStringSessionValue(getUniquePageId(), req, "symbol_data", HtWebUtils.createXMLForDhtmlGrid(dataset));
		setStringSessionValue(getUniquePageId(), req, "symbol_list_size", String.valueOf(cnt));
		setStringSessionValue(getUniquePageId(), req, "min_total_time", HtDateTimeUtils.time2SimpleString_Gmt(min_total_time.getLong()));
		setStringSessionValue(getUniquePageId(), req, "max_total_time", HtDateTimeUtils.time2SimpleString_Gmt(max_total_time.getLong()));


	}

	private void readSimulationProfileData(HtAjaxCaller ajaxCaller, String sim_profile) throws Exception {
		HtSimulationProfileProp prop = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getSimulationProfileProperty(sim_profile);

		Map<String, String> resmap = new HashMap<String, String>();

		// wrap to special structure
		resmap.put("sprop_aggr_per", TSAggregationPeriod.getAggregationPeriodName(prop.aggrPeriod_m));
		resmap.put("sprop_gen_tick_method", TSGenerateTickMethod.getGenerateTickMethodNameName(prop.genTickMethod_m));
		resmap.put("sprop_mult_factor", String.valueOf(prop.multFactor_m));
		resmap.put("sprop_ticks_split_number", String.valueOf(prop.tickSplitNumber_m));
		resmap.put("sprop_ticks_usage", TickUsage.getTickUsageName(prop.tickUsage_m));

		// integer
		resmap.put("sprop_aggr_per_int", String.valueOf(prop.aggrPeriod_m));
		resmap.put("sprop_gen_tick_method_int", String.valueOf(prop.genTickMethod_m));
		resmap.put("sprop_ticks_usage_int", String.valueOf(prop.tickUsage_m));

		// list of all registered symbols
		List<HtPair<String, String>> provSymbList = prop.getRegisteredProviderSymbols();

		List<String> providers = new ArrayList<String>();
		List<String> symbols = new ArrayList<String>();

		for (int i = 0; i < provSymbList.size(); i++) {
			String provider = provSymbList.get(i).first;
			String symbol = provSymbList.get(i).second;

			providers.add(provider);
			symbols.add(symbol);
		}



		Gson gson = new Gson();
		String providers_result = gson.toJson(providers);
		String symbols_result = gson.toJson(symbols);

		resmap.put("providers_result", providers_result);
		resmap.put("symbols_result", symbols_result);


		String data = gson.toJson(resmap);

		ajaxCaller.setOk();
		ajaxCaller.setData(data);

	}

	private void createSimulationTicks(
					HtAjaxCaller ajaxCaller,
					String server_name,
					String sim_profile,
					List<HtPair<String, String>> provSymbListParticipate,
					long begdate,
					long enddate,
					String idString
	) throws Exception {
		// resolve simulation property
		HtSimulationProfileProp prop = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getSimulationProfileProperty(sim_profile);
		Uid simUid = HtCommandProcessor.instance().get_HtServerProxy().remote_generateSimulationTicks(server_name, begdate, enddate, idString, prop, provSymbListParticipate);
	
		ajaxCaller.setOk();
		ajaxCaller.setData(simUid.toString());

	}
}
