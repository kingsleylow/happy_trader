/*
 * HtServerAlgorithmLoad_viewMemoryCacheDetails.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
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
import com.fin.httrader.utils.hlpstruct.HtPair;
import com.fin.httrader.utils.HtTimeCalculator;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.LongParam;
import com.fin.httrader.utils.Uid;
import com.fin.httrader.webserver.GridDescriptor.GridDataSet;
import com.fin.httrader.webserver.GridDescriptor.GridFieldDescriptor;
import java.io.IOException;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;
import java.util.Vector;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtServerAlgorithmLoad_viewMemoryCacheDetails extends HtServletsBase {

	@Override
	public String getContext() {
		return this.getClass().getSimpleName();
	}

	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);


		try {
			// new UID always

			generateUniquePageId();
			extractMemoryCacheDetails(req);

		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			return false;
		}
		
		return true;

	} //

	// this function resolves what we have in memory cache according our previous tests
	private void extractMemoryCacheDetails(HttpServletRequest req) throws Exception {

		Uid hist_context_uid = getUidParameter(req, "hist_context_uid", false);
		String server_id = getStringParameter(req, "server_id", false);
		
	
		if (!hist_context_uid.isValid()) {
			throw new HtException(getContext(), "extractMemoryCacheDetails", "Invalid history context UID");
		}

		LongParam min_total_time = new LongParam();
		LongParam max_total_time =  new LongParam();
		LongParam create_time = new LongParam();
		HtSimulationProfileProp.HtSimulationProfileBaseParams baseSimParams = new HtSimulationProfileProp.HtSimulationProfileBaseParams();

		Map<String, Map<String, HtPair<Long, Long>>> cachedSymbols = new TreeMap<String, Map<String, HtPair<Long, Long>>>();

		HtCommandProcessor.instance().get_HtServerProxy().remote_getSimulationTicksGeneratedSymbols(
					server_id,
					hist_context_uid,
					cachedSymbols,
					min_total_time,
					max_total_time,
					create_time,
					baseSimParams
		);

			
		GridDataSet dataset = new GridDataSet(
						new GridFieldDescriptor[]{
							new GridFieldDescriptor("RT Provider"),
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

		// store symbol list
		setStringSessionValue(getUniquePageId(), req, "symbol_data", HtWebUtils.createXMLForDhtmlGrid(dataset));

		// some simulation parameters
		setStringSessionValue(getUniquePageId(), req, "sprop_aggr_per", TSAggregationPeriod.getAggregationPeriodName(baseSimParams.aggrPeriod_m));
		setStringSessionValue(getUniquePageId(), req, "sprop_gen_tick_method", TSGenerateTickMethod.getGenerateTickMethodNameName(baseSimParams.genTickMethod_m));
		setStringSessionValue(getUniquePageId(), req, "sprop_mult_factor", String.valueOf(baseSimParams.multFactor_m));
		setStringSessionValue(getUniquePageId(), req, "sprop_ticks_split_number", String.valueOf(baseSimParams.tickSplitNumber_m));
		setStringSessionValue(getUniquePageId(), req, "sprop_ticks_usage", TickUsage.getTickUsageName(baseSimParams.tickUsage_m));

		// dates

		setStringSessionValue(getUniquePageId(), req, "min_total_time", HtDateTimeUtils.time2SimpleString_Gmt(min_total_time.getLong()));
		setStringSessionValue(getUniquePageId(), req, "max_total_time", HtDateTimeUtils.time2SimpleString_Gmt(max_total_time.getLong()));

		// count
		setStringSessionValue(getUniquePageId(), req, "symbol_list_size", String.valueOf(cnt));

		setStringSessionValue(getUniquePageId(), req, "description", server_id + " has cached ticks identified by UID: " + hist_context_uid.toString() +
			" created on " + HtDateTimeUtils.time2SimpleString_Gmt(create_time.getLong()));

	}
}
