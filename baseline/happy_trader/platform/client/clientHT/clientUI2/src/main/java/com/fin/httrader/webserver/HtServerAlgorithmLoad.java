/*
 * HtServerAlgorithmLoad.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.configprops.HtAlgBrkPairProp;
import com.fin.httrader.configprops.HtDataProfileProp;
import com.fin.httrader.configprops.HtSimulationProfileProp;
import com.fin.httrader.configprops.HtSimulationProfileProp.SymbolRelatedProperty;
import com.fin.httrader.hlpstruct.ServerThreadStatus;
import com.fin.httrader.hlpstruct.TSAggregationPeriod;
import com.fin.httrader.hlpstruct.TSGenerateTickMethod;
import com.fin.httrader.hlpstruct.TickUsage;
import com.fin.httrader.managers.RtTradingServerManager;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.hlpstruct.HtPair;
import com.fin.httrader.utils.HtTimeCalculator;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.LongParam;
import com.fin.httrader.utils.Uid;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import java.io.IOException;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
import java.util.Vector;
import java.util.logging.Level;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtServerAlgorithmLoad extends HtServletsBase {

	public String getContext() {
		return this.getClass().getSimpleName();
	}

	/** Creates a new instance of HtServerAlgorithmLoad */
	public HtServerAlgorithmLoad() {
	}

	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		try {
			// generate new Uid and set initial values
			this.generateUniquePageId();

			// empty everything

			setStringSessionValue(getUniquePageId(), req, "running_server_list", "");
			setStringSessionValue(getUniquePageId(), req, "sim_profile_list", "");

			//
			readAlgorithmData(req);


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

			// empty everything

			setStringSessionValue(getUniquePageId(), req, "running_server_list", "");
			setStringSessionValue(getUniquePageId(), req, "sim_profile_list", "");

			String operation = getStringParameter(req, "operation", false);
			if (operation.equalsIgnoreCase("load_algorithms")) {
				loadAlgorithms(req);

			} else if (operation.equalsIgnoreCase("unload_algorithms")) {
				unloadAlgorithms(req);

			} else if (operation.equalsIgnoreCase("reload_algorithms")) {
				reloadAlgorithms(req);

			} else if (operation.equalsIgnoreCase("do_refresh")) {
			} else if (operation.equalsIgnoreCase("do_start_thread")) {
				startThread(req);
			} else if (operation.equalsIgnoreCase("do_stop_thread")) {
				stopThread(req);
			} //
			else if (operation.equalsIgnoreCase("do_start_rt")) {
				startRt(req);
			} else if (operation.equalsIgnoreCase("do_stop_rt")) {
				stopRt(req);
			} else if (operation.equalsIgnoreCase("do_start_simulation")) {
				startSimulation(req);
			} else if (operation.equalsIgnoreCase("do_stop_simulation")) {
				stopSimulation(req);
			} else {
				throw new HtException(getContext(), "initialize", "Invalid operation: " + operation);
			}

			readAlgorithmData(req);


		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			return false;
		}
		
		return true;

	}
	

	private void readAlgorithmData(HttpServletRequest req) throws Exception {


		if (HtCommandProcessor.instance().get_HtServerProxy().isBusyWithLengthOperation()) {
			throw new HtException(getContext(), "readAlgorithmData", "Server is busy with lengthy operation: " +
							HtCommandProcessor.instance().get_HtServerProxy().getLengthyOperationDescription());
		}

		StringBuilder out = new StringBuilder();
		Set<String> servers = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getTradingServerList();


		String cur_server_id = getStringParameter(req, "server_id", true);
		boolean atLeastOne = false;
		for (Iterator<String> it = servers.iterator(); it.hasNext();) {
			String server_id = it.next();
			int server_status = HtCommandProcessor.instance().get_HtServerProxy().remote_returnServerStatus(server_id);


			if (server_status != RtTradingServerManager.SERVER_IDLE) {
				// the first running server
				if (cur_server_id.length() <= 0) {
					cur_server_id = server_id;
				}

				if (cur_server_id.equals(server_id)) {
					out.append("<option selected >").append(server_id).append("</option>");
				} else {
					out.append("<option>").append(server_id).append("</option>");
				}

				atLeastOne = true;
			}
		}

		setStringSessionValue(getUniquePageId(), req, "running_server_list", out.toString());

		// no need to go on
		if (!atLeastOne) {
			return;
		}

		//
		out.setLength(0);
		Set<String> algs = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRegisteredAlgBrkPairList();

		Map<String, Integer> loaded = HtCommandProcessor.instance().get_HtServerProxy().remote_requestLoadedAlgorithmBrokerPairs(cur_server_id, -1);

		out.append("<tr NOWRAP=true>");

		out.append("<td type='ro'>Select</td>");
		out.append("<td type='ro'>Algorithm Broker Pair</td>");
		out.append("<td type='ro'>Status</td>");
		out.append("<td type='ro'>Priority</td>");
		out.append("<td type='ro'>Load/Unload</td>");
		out.append("<td type='ro'>Update Pair Parameters</td>");

		out.append("</tr>");

		int cnt = 1;

		for (Iterator<String> it = algs.iterator(); it.hasNext();) {

			out.append("<tr NOWRAP=true id='");
			out.append(cnt);
			out.append("'>");

			String algPairName = it.next();

			int priority = cnt;
			boolean isLoaded = loaded.containsKey(algPairName);
			if (isLoaded) {
				priority = loaded.get(algPairName);
			}

			out.append("<td>");

			if (isLoaded) {
				out.append("<input type=checkbox></>");
			} else {
				out.append("<input type=checkbox disabled=true></>");
			}


			out.append("</td>");

			out.append("<td>");
			out.append(algPairName);
			out.append("</td>");

			out.append("<td>");
			out.append(isLoaded ? "Loaded" : "Idle");
			out.append("</td>");

			// priority
			out.append("<td>");
			out.append(priority);
			out.append("</td>");

			out.append("<td>");
			if (!isLoaded) {
				out.append("<a href='#' onclick='do_load_algorithm(\"");
				out.append(algPairName);
				out.append("\",");
				out.append(cnt);
				out.append(");'>Load</a>");
			} else {
				out.append("<a href='#' onclick='do_unload_algorithm(\"");
				out.append(algPairName);
				out.append("\",");
				out.append(cnt);
				out.append(");'>Unload</a>");
			}
			out.append("</td>");

			out.append("<td>");
			if (isLoaded) {
				out.append("<a href='#' onclick='do_reload_algorithm_parameters(\"");
				out.append(algPairName);
				out.append("\");'>Click</a>");
			}
			else {
				out.append("Not loaded");
			}
			out.append("</td>");

			cnt++;

			out.append("</tr>");
		}


		setStringSessionValue(getUniquePageId(), req, "algorithm_list", out.toString());

		//
		out.setLength(0);
		List<Integer> threads = HtCommandProcessor.instance().get_HtServerProxy().remote_requestStartedThreads(cur_server_id);

		out.append("<tr NOWRAP=true>");

		out.append("<td type='ro'>Thread ID</td>");
		out.append("<td type='ro'>Status</td>");
		out.append("<td type='ro'>Stop Thread</td>");
		out.append("<td type='ro'>Start/Stop RT</td>");
		out.append("<td type='ro'>Start/Stop Simulation</td>");
		out.append("<td type='ro' width='0px'>LOADED_ALGORITHMS</td>");


		out.append("</tr>");



		int rcnt = -1;
		for(int t = 0; t < threads.size(); t++) {
			int tId = threads.get(t);
			int threadStatus = HtCommandProcessor.instance().get_HtServerProxy().remote_requestThreadStatus(cur_server_id, tId);

			int rowid = ++rcnt;
			out.append("<tr NOWRAP=true id='");
			out.append(rowid);
			out.append("'>");


			out.append("<td>");
			out.append(tId);
			out.append("</td>");

			out.append("<td>");
			out.append(ServerThreadStatus.getThreadStatus(threadStatus));
			out.append("</td>");



			out.append("<td>");
			out.append("<a href='#' onclick='do_stop_thread(");
			out.append(tId);
			out.append(");'>Click</a>");
			out.append("</td>");


			out.append("<td>");
			out.append("<a href='#' onclick='do_start_rt(");
			out.append(tId);
			out.append(");'>Start RT</a>");
			out.append("&nbsp;");
			out.append("<a href='#' onclick='do_stop_rt(");
			out.append(tId);
			out.append(");'>Stop RT</a>");
			out.append("</td>");


			out.append("<td>");
			out.append("<a href='#' onclick='do_start_simulation(");
			out.append(tId);
			out.append(");'>Start Simulation</a>");
			out.append("&nbsp;");
			out.append("<a href='#' onclick='do_stop_simulation(");
			out.append(tId);
			out.append(");'>Stop Simulation</a>");
			out.append("</td>");


			// list of loaded algorithm broker pairs
			Map<String, Integer> loadedpairs = HtCommandProcessor.instance().get_HtServerProxy().remote_requestLoadedAlgorithmBrokerPairs(cur_server_id, tId);
			out.append("<td>");
			out.append("new Array(");
			for (Iterator<String> it2 = loadedpairs.keySet().iterator(); it2.hasNext();) {
				out.append("'");
				out.append(it2.next());
				out.append("',");
			}

			out.setLength(out.length() - 1);
			out.append(")");
			out.append("</td>");


		


			out.append("</tr>");
		}


		setStringSessionValue(getUniquePageId(), req, "thread_list", out.toString());


	// prepair map for thread <-> loaded algorithm broker pairs
	}

	private void loadAlgorithms(HttpServletRequest req) throws Exception {
		String cur_server_id = getStringParameter(req, "server_id", false);
		String alg_id = getStringParameter(req, "alg_pair", false);
		String run_name = getStringParameter(req, "run_name", false);
		String run_comment = getStringParameter(req, "run_comment", true);

		HtAlgBrkPairProp prop = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getAlgBrkPairProperty(alg_id);

		int priority = (int) getIntParameter(req, "priority", false);

		HtCommandProcessor.instance().get_HtServerProxy().remote_loadAlgorithmBrokerPair(
						cur_server_id,
						run_name,
						run_comment,
						alg_id,
						prop.getAlgorithmPath(),
						prop.getBrokerPath(),
						prop.getBrokerPath2(),
						prop.getAlgoritmParams(),
						prop.getBrokerParams(),
						prop.getBrokerParams2(),
						priority);

	}

	private void reloadAlgorithms(HttpServletRequest req) throws Exception {
		String cur_server_id = getStringParameter(req, "server_id", false);
		String alg_id = getStringParameter(req, "alg_pair", false);


		int tid = (int) getIntParameter(req, "thread_id", false);

		HashMap<String, String> newAlgParams = new HashMap<String, String>();
		HashMap<String, String> newBrkParams = new HashMap<String, String>();


		List<HtPair<String, String>> algp = getStringPairListParameter(req, "alg_params", true);
		List<HtPair<String, String>> brkp = getStringPairListParameter(req, "brk_params", true);

		for (int i = 0; i < algp.size(); i++) {
			HtPair<String, String> a_i = algp.get(i);
			newAlgParams.put(a_i.first, a_i.second);
		}

		for (int i = 0; i < brkp.size(); i++) {
			HtPair<String, String> b_i = brkp.get(i);
			newBrkParams.put(b_i.first, b_i.second);
		}


		HtCommandProcessor.instance().get_HtServerProxy().remote_reloadAlgorithmBrokerPair(
						cur_server_id,
						alg_id,
						tid,
						newAlgParams,
						newBrkParams);
	}

	private void unloadAlgorithms(HttpServletRequest req) throws Exception {
		String cur_server_id = getStringParameter(req, "server_id", false);
		String alg_id = getStringParameter(req, "alg_pair", false);

		HtCommandProcessor.instance().get_HtServerProxy().remote_unloadAlgorithmBrokerPair(cur_server_id, alg_id);
	}

	private void startThread(HttpServletRequest req) throws Exception {
		String cur_server_id = getStringParameter(req, "server_id", false);
		List<String> pairs = getStringListParameter(req, "selected_pairs", false);

		HtCommandProcessor.instance().get_HtServerProxy().remote_startAlgorithmThread(cur_server_id, pairs);
	}

	private void stopThread(HttpServletRequest req) throws Exception {
		String cur_server_id = getStringParameter(req, "server_id", false);
		int thread_id = (int) getIntParameter(req, "thread_id", false);

		HtCommandProcessor.instance().get_HtServerProxy().remote_stopAlgorithmThread(cur_server_id, thread_id);

	}

	/**

	 */
	private void startRt(HttpServletRequest req) throws Exception {
		String cur_server_id = getStringParameter(req, "server_id", false);
		String run_name = getStringParameter(req, "run_name", false);
		String run_comment = getStringParameter(req, "run_comment", true);
		int thread_id = (int) getIntParameter(req, "thread_id", false);
		int sim_prof_pass = (int) getIntParameter(req, "sim_prof_pass", false);

		// optional parameter
		if (sim_prof_pass > 0) {
			String sim_profile_name = getStringParameter(req, "sim_profile_name", false);
			// validate this profile
			HtSimulationProfileProp prop = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getSimulationProfileProperty(sim_profile_name);

			HtCommandProcessor.instance().get_HtServerProxy().remote_startRtActivity(cur_server_id, thread_id, run_name, run_comment, prop);
		} else {
		
			HtCommandProcessor.instance().get_HtServerProxy().remote_startRtActivity(cur_server_id, thread_id, run_name, run_comment, null);

		}

	}

	private void stopRt(HttpServletRequest req) throws Exception {
		String cur_server_id = getStringParameter(req, "server_id", false);
		int thread_id = (int) getIntParameter(req, "thread_id", false);

		HtCommandProcessor.instance().get_HtServerProxy().remote_stopRtActivity(cur_server_id, thread_id);
	}

	private void startSimulation(HttpServletRequest req) throws Exception {

		String cur_server_id = getStringParameter(req, "server_id", false);
		int thread_id = (int) getIntParameter(req, "thread_id", false);
		String run_name = getStringParameter(req, "run_name", false);
		String run_comment = getStringParameter(req, "run_comment", true);

		Uid hist_context_uid = getUidParameter(req, "hist_context_uid", false);


		// NO Local time - use GMT
	
		boolean singleSimRun = getBooleanParameter(req, "single_sim_run", false);
		

		
		
		HtCommandProcessor.instance().get_HtServerProxy().remote_startHistorySimulation(
						run_name,
						run_comment,
						cur_server_id,
						thread_id,
						singleSimRun,
						hist_context_uid);

		HtLog.log(Level.INFO, getContext(), "startSimulation", "Started simulation with history context UID: " + hist_context_uid.toString() );


	}

	private void stopSimulation(HttpServletRequest req) throws Exception {
		String cur_server_id = getStringParameter(req, "server_id", false);
		int thread_id = (int) getIntParameter(req, "thread_id", false);

		HtCommandProcessor.instance().get_HtServerProxy().remote_stopHistorySimulation(cur_server_id, thread_id);
	}
}
