/*
 * HtServerProxy.java
 *
 *er
 * and open the template in the editor.
 */
package com.fin.httrader.model;

import com.fin.httrader.configprops.HtDataProfileProp;
import com.fin.httrader.configprops.HtSimulationProfileProp;
import com.fin.httrader.hlpstruct.Order;
import com.fin.httrader.hlpstruct.Position;
import com.fin.httrader.hlpstruct.TSAggregationPeriod;
import com.fin.httrader.hlpstruct.TSGenerateTickMethod;
import com.fin.httrader.hlpstruct.broker.BrokerConnect;
import com.fin.httrader.hlpstruct.remotecall.BoolList_Proxy;
import com.fin.httrader.hlpstruct.remotecall.Bool_Proxy;
import com.fin.httrader.hlpstruct.remotecall.CallingContext;
import com.fin.httrader.hlpstruct.remotecall.IntList_Proxy;
import com.fin.httrader.hlpstruct.remotecall.Int_Proxy;
import com.fin.httrader.hlpstruct.remotecall.Order_Proxy;
import com.fin.httrader.hlpstruct.remotecall.ParamBase;
import com.fin.httrader.hlpstruct.remotecall.PositionList_Proxy;
import com.fin.httrader.hlpstruct.remotecall.StringList_Proxy;
import com.fin.httrader.hlpstruct.remotecall.String_Proxy;
import com.fin.httrader.hlpstruct.remotecall.UidList_Proxy;
import com.fin.httrader.hlpstruct.remotecall.Uid_Proxy;
import com.fin.httrader.interfaces.HtDataSelection;
import com.fin.httrader.interfaces.HtEventListener;
import com.fin.httrader.managers.RtDatabaseManager;
import com.fin.httrader.managers.RtDatabaseSelectionManager;
import com.fin.httrader.managers.RtGlobalEventManager;
import com.fin.httrader.managers.RtTradingServer;
import com.fin.httrader.managers.RtTradingServerManager;
import com.fin.httrader.HtMain;
import com.fin.httrader.hlpstruct.ThreadDescriptor;
import com.fin.httrader.managers.RtConfigurationManager;
import com.fin.httrader.utils.BinConverter;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.HtTimeCalculator;
import com.fin.httrader.utils.hlpstruct.HtPair;
import com.fin.httrader.utils.hlpstruct.HtRtData;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.LongParam;
import com.fin.httrader.utils.SingleReaderQueue;
import com.fin.httrader.utils.SingleReaderQueueExceedLimit;
import com.fin.httrader.utils.Uid;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import com.fin.httrader.utils.hlpstruct.XmlHandler;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
import java.util.TreeSet;
import java.util.Vector;

import java.util.logging.Level;

/**
 *
 * @author Administrator Class representing managements of server page // should
 * launch all the commands from GUI layer
 */
public class HtServerProxy extends HtProxyBase {
		// server event flags

		public static final int SRV_COMMAND_OK = 0;
		public static final int SRV_COMMAND_LOCAL_EXCEPTION = 1;
		public static final int SRV_COMMAND_REMOTE_EXCEPTION = 2;
		public static final int SRV_COMMAND_NA = 3;
		//
		private XmlHandler xmlCommandThreadHandler_m = new XmlHandler();

		public static class HtServerProxy_ProfileEntry {

				public List<HtPair<String, String>> providerSymbolList_m = null;
				public boolean defaultSelection_m = false;
		};

		private String getContext() {
				return this.getClass().getSimpleName();

		}

		public HtServerProxy() {
		}

		// remote calls
		public String remote_getLogFileName(String serverName) throws Exception {
				return RtTradingServerManager.instance().getLogFileName(serverName);
		}

		public int remote_queryRunningServersNumber() {
				return RtTradingServerManager.instance().numberOfRunningServers();
		}

		public int remote_returnServerStatus(String serverName) {
				return RtTradingServerManager.instance().returnServerStatus(serverName);
		}

		public Set<String> remote_getSubscribedRTProviders(String serverName) throws Exception {
				return RtTradingServerManager.instance().getTradingServer(serverName).getSubscribedRTProviders();
		}

		// this starts server and pushes history data, then begins RT propagation
		public void remote_startServerProcessWithHistoryData(
			String serverName,
			String serverExecutable,
			int server_log_level, // internal servel log level
			String serverHost,
			int serverPort,
			int servicePort,
			int spawnerPort,
			boolean useSpawner,
			boolean isLaunchProcess,
			long begdate,
			long enddate,
			Map<String, HtServerProxy_ProfileEntry> profProvSymbMap, // map profile <-> ( provider - symbol ) - this is only history data
			Set<String> rtProvidersToSubscribe, // subscribe only that providers
			boolean reuseCache,
			int defaultLogLevel,
			Map<Integer, Boolean> subsribedEvents,
			boolean noSelectAtAll) throws Exception {

				HtLog.log(Level.INFO, getContext(), "remote_startServerProcessWithHistoryData",
					"Will start server process and push history from: " + HtDateTimeUtils.time2String_Gmt(begdate) + " to: " + HtDateTimeUtils.time2String_Gmt(enddate));

				// create exec enviroment
				HashMap<String, String> newEntries = new HashMap<String, String>();

				// this is internal LOG(MSG_... level inn CPP code
				if (server_log_level > 0) {
						newEntries.put(RtConfigurationManager.StartUpConst.TRADE_SERVER_LOGVAR_NAME, String.valueOf(server_log_level));
				}

				// logging
				for (Iterator<String> prov_it = rtProvidersToSubscribe.iterator(); prov_it.hasNext();) {
						HtLog.log(Level.INFO, getContext(), "remote_startServerProcessWithHistoryData", "Subscribed with provider: " + prov_it.next());
				}

				XmlParameter request = new XmlParameter();
				request.setCommandName("load_server");

				// this is to say that we need to delete old cache left 
				request.setInt("no_data_to_load", noSelectAtAll ? 1 : 0);
				request.setInt("keep_cache_after_shutdown", reuseCache ? 1 : 0);

				XmlParameter response = null;

				try {

						if (!startLengthyOperation("starting server")) {
								throw new HtException(getContext(), "remote_startServerProcessWithHistoryData", "Server is busy performing lengthy operation: " + getLengthyOperationDescription());
						}

						// start server without RT thread
						response = RtTradingServerManager.instance().initServerProcess(
							serverName,
							serverExecutable,
							newEntries,
							serverHost,
							serverPort,
							servicePort,
							spawnerPort,
							useSpawner,
							request,
							!isLaunchProcess,
							//false,
							rtProvidersToSubscribe,
							false // sdo not tart RT thread
						);

						// push history data
						remote_pushHistoryData(
							serverName,
							begdate,
							enddate,
							profProvSymbMap, // map profile <-> ( provider - symbol
							RtConfigurationManager.StartUpConst.SELECT_PAGE_DEFAULT_SIZE, // make selections via this chunk
							reuseCache, // flag showing that we may reuse cache
							noSelectAtAll);

						// start service thread(s)
						RtTradingServerManager.instance().getTradingServer(serverName).startAndWaitForServiceThreads(5000);

				} catch (Throwable e) {
						// logging
						RtTradingServerManager.instance().pushLocalServerEvent(serverName, HtServerProxy.SRV_COMMAND_LOCAL_EXCEPTION, e.getMessage());

						HtLog.log(Level.WARNING, getContext(), "remote_startServerProcessWithHistoryData", "Failure on start: " + e.getMessage() + " will kill server process");

						//clean up - kill this as too few chances to recover
						RtTradingServerManager.instance().killServerProcess(serverName);

						// propagate
						throw new HtException(getContext(), "remote_startServerProcessWithHistoryData", "Exception happend on initialization of server process " + serverName + " - " + e.getMessage());

				} finally {
						finishLengthyOperation();
				}

				// OK
				finalizeCommandProcessing(serverName, request, response);

		// set default log level
				// this is event level
				remote_setlogLevel(serverName, defaultLogLevel);

				//set up allowed events
				remote_setAllowedEvents(serverName, subsribedEvents);

		}

		// starts server
		public void remote_startServerProcess(
			String serverName,
			String serverExecutable,
			int server_log_level, // internal servel log level
			String serverHost,
			int serverPort,
			int servicePort,
			int spawnerPort,
			boolean useSpawner,
			boolean isLaunchProcess,
			Set<String> rtProvidersToSubscribe,
			int defaultLogLevel,
			Map<Integer, Boolean> subsribedEvents) throws Exception {
				XmlParameter request = new XmlParameter();
				request.setCommandName("load_server");

				XmlParameter response = null;
				try {

						if (!startLengthyOperation("starting server")) {
								throw new HtException(getContext(), "remote_startServerProcess", "Server is busy performing lengthy operation: " + getLengthyOperationDescription());
						}

						// create exec enviroment
						HashMap<String, String> newEntries = new HashMap<String, String>();

						if (server_log_level > 0) {
								newEntries.put(RtConfigurationManager.StartUpConst.TRADE_SERVER_LOGVAR_NAME, String.valueOf(server_log_level));
						}

						response = RtTradingServerManager.instance().initServerProcess(
							serverName,
							serverExecutable,
							newEntries,
							serverHost,
							serverPort,
							servicePort,
							spawnerPort,
							useSpawner,
							request,
							!isLaunchProcess,
							//false,
							rtProvidersToSubscribe,
							true // start RT thread
						);

				} catch (Throwable e) {

						RtTradingServerManager.instance().pushLocalServerEvent(serverName, HtServerProxy.SRV_COMMAND_LOCAL_EXCEPTION, e.getMessage());

						// free on failure
						RtTradingServerManager.instance().killServerProcess(serverName);

						throw new HtException(getContext(), "remote_startServerProcess", "Exception happend on initialization of server process " + serverName + " - " + e.getMessage());
				} finally {
						finishLengthyOperation();
				}

				// OK
				finalizeCommandProcessing(serverName, request, response);

				// now need to adopt default log level
				remote_setlogLevel(serverName, defaultLogLevel);

				//set up allowed events
				remote_setAllowedEvents(serverName, subsribedEvents);

		}

		public void remote_shutDownServerProcess(
			String serverName) throws Exception {

				RtTradingServerManager.instance().normalShutDown(serverName);

		}

		public void remote_killServerProcess(
			String serverName) throws Exception {
				RtTradingServerManager.instance().killServerProcess(serverName);
		}

		public int remote_startAlgorithmThread(String serverName, List<String> algList) throws Exception {
				XmlParameter request = null;
				XmlParameter response = null;
				try {
						request = new XmlParameter();

						request.setCommandName("start_algorithm_thread");
						request.setStringList("algorithm_list", algList);

						response = RtTradingServerManager.instance().getTradingServer(serverName).issueCommand(request);

				} catch (Throwable e) {
						localExceptionCommandProcessing(serverName, e);
				}

		// at this point algorithm thread is started !!!
				// OK
				finalizeCommandProcessing(serverName, request, response);

				//
				int tId = (int) response.getInt("algorithm_thread_id");

				return tId;

		}

		public void remote_stopAlgorithmThread(String serverName, int tId) throws Exception {
				XmlParameter request = null;
				XmlParameter response = null;
				try {
						request = new XmlParameter();

						request.setCommandName("stop_algorithm_thread");
						request.setInt("algorithm_thread_id", tId);

						response = RtTradingServerManager.instance().getTradingServer(serverName).issueCommand(request);

				} catch (Throwable e) {
						localExceptionCommandProcessing(serverName, e);
				}

				// OK
				finalizeCommandProcessing(serverName, request, response);
		}

		//
		public void remote_startRtActivity(
			String serverName,
			int tId,
			String runName,
			String runComment,
			HtSimulationProfileProp prop // optional parameter
		) throws Exception {
				XmlParameter request = null;
				XmlParameter response = null;

				try {
						request = new XmlParameter();
						request.setCommandName("start_rt");
						request.setInt("algorithm_thread_id", tId);
						request.setString("run_name", runName);
						request.setString("run_comment", runComment);

						// so far no additional info
						request.setInt("pass_additional_info", 0);

						if (prop != null) {
								List<HtPair<String, String>> provSymbList = prop.getRegisteredProviderSymbols();

								if (provSymbList.size() > 0) {

										// pass all these additional info
										request.setInt("pass_additional_info", 1);

										// init with all data
										List<String> symbols = new ArrayList<String>();
										List<String> providers = new ArrayList<String>();

										List<XmlParameter> sxmlList = new ArrayList<XmlParameter>();

										for (int i = 0; i < provSymbList.size(); i++) {
												String provider = provSymbList.get(i).first;
												String symbol = provSymbList.get(i).second;

												providers.add(provider);
												symbols.add(symbol);

												Uid uid = prop.resolveProviderSymbol(provider, symbol);
												HtSimulationProfileProp.SymbolRelatedProperty s = prop.getSymbolProperty(uid);

												XmlParameter sxml = new XmlParameter();
												sxml.setDouble("comission", s.comission_m);
												sxml.setDouble("comissionPct", s.comissionPct_m);
												sxml.setInt("execute_pos_exact_price", s.executePosExactPrices_m ? 1 : 0);
												sxml.setDouble("min_limit_order_distance", s.minLimitOrderDistance_m);
												sxml.setDouble("min_stop_loss_distance", s.minStopLossDistance_m);
												sxml.setDouble("min_stop_order_distance", s.minStopOrderDistance_m);
												sxml.setDouble("min_take_profit_distance", s.minTakeProfitDistance_m);
												sxml.setDouble("point_value", s.pointValue_m);
												sxml.setInt("sign_digits", s.signDigits_m);
												sxml.setDouble("simulation_slippage", s.simulationSlippage_m);
												sxml.setDouble("simulation_spread", s.simulationSpread_m);

												sxmlList.add(sxml);
										}

										//
										request.setXmlParameterList("symbol_trade_info", sxmlList);
										request.setStringList("provider_list", providers);
										request.setStringList("symbol_list", symbols);

								}
						}

						response = RtTradingServerManager.instance().getTradingServer(serverName).issueCommand(request);
				} catch (Throwable e) {
						localExceptionCommandProcessing(serverName, e);
				}

				// OK
				finalizeCommandProcessing(serverName, request, response);

		}

		public void remote_stopRtActivity(String serverName, int tId) throws Exception {
				XmlParameter request = null;
				XmlParameter response = null;

				try {
						request = new XmlParameter();
						request.setCommandName("stop_rt");
						request.setInt("algorithm_thread_id", tId);

						response = RtTradingServerManager.instance().getTradingServer(serverName).issueCommand(request);

				} catch (Throwable e) {
						localExceptionCommandProcessing(serverName, e);
				}

				// OK
				finalizeCommandProcessing(serverName, request, response);
		}

		public boolean remote_isInputQueueEmpty(String serverName, int tId) throws Exception {
				XmlParameter request = null;
				XmlParameter response = null;

				boolean result = false;

				try {
						request = new XmlParameter();
						request.setCommandName("is_input_queue_empty");
						request.setInt("algorithm_thread_id", tId);

						response = RtTradingServerManager.instance().getTradingServer(serverName).issueCommand(request);

				} catch (Throwable e) {
						localExceptionCommandProcessing(serverName, e);
				}

				// OK
				finalizeCommandProcessing(serverName, request, response);

				result = (response.getInt("is_input_queue_empty") == 1);

				return result;
		}

		public void remote_queryAvailableSimulationEntries(
			String serverName,
			Map<Uid, HtPair<String, Long>> uidIdCreatedMap) throws Exception {
				XmlParameter request = null;
				XmlParameter response = null;
				request = new XmlParameter();
				request.setCommandName("query_available_simulation_caches");

				try {
						response = RtTradingServerManager.instance().getTradingServer(serverName).issueCommand(request);
				} catch (Throwable e) {
						localExceptionCommandProcessing(serverName, e);
				}

				// OK
				finalizeCommandProcessing(serverName, request, response);

				uidIdCreatedMap.clear();
				List<XmlParameter> result = response.getXmlParameterList("result");
				if (result != null) {
						for (int i = 0; i < result.size(); i++) {
								XmlParameter p = result.get(i);

								HtPair<String, Long> pair_i = new HtPair<String, Long>(p.getString("id"), p.getDate("created_on"));
								uidIdCreatedMap.put(p.getUid("uid"), pair_i);

						}
				}

		}

		// this only generate simulation tciks
		public Uid remote_generateSimulationTicks(
			String serverName,
			long begsimtime,
			long endsimtime,
			String idString,
			HtSimulationProfileProp prop,
			List<HtPair<String, String>> provSymbListParticipate) throws Exception {
				XmlParameter request = null;
				XmlParameter response = null;

				try {
						request = new XmlParameter();
						request.setCommandName("prepare_simulation_ticks");

						request.setDate("begin_simulation_time", begsimtime);
						request.setDate("end_simulation_time", endsimtime);

						request.setString("id_string", idString);

						request.setInt("aggr_period", prop.aggrPeriod_m);
						request.setInt("mult_factor", prop.multFactor_m);
						request.setInt("gen_tick_method", prop.genTickMethod_m);
						request.setInt("ticks_split_number", prop.tickSplitNumber_m);
						request.setInt("tick_usage", prop.tickUsage_m);

						if (provSymbListParticipate.isEmpty()) {
								throw new HtException(getContext(), "remote_startHistorySimulation", "No symbols selected");
						}

						List<String> symbols = new ArrayList<String>();
						List<String> providers = new ArrayList<String>();

						List<XmlParameter> sxmlList = new ArrayList<XmlParameter>();
						for (int i = 0; i < provSymbListParticipate.size(); i++) {
								String provider = provSymbListParticipate.get(i).first;
								String symbol = provSymbListParticipate.get(i).second;

								providers.add(provider);
								symbols.add(symbol);

								if (!prop.isExistProviderSymbol(provider, symbol)) {
										throw new HtException(getContext(), "remote_generateSimulationTicks",
											"For provider: " + provider + ", symbol: " + symbol + " relevant simulation entry does not exist");
								}

								Uid uid = prop.resolveProviderSymbol(provider, symbol);

								HtSimulationProfileProp.SymbolRelatedProperty s = prop.getSymbolProperty(uid);

								XmlParameter sxml = new XmlParameter();
								sxml.setDouble("comission", s.comission_m);
								sxml.setDouble("comissionPct", s.comissionPct_m);
								sxml.setInt("execute_pos_exact_price", s.executePosExactPrices_m ? 1 : 0);
								sxml.setDouble("min_limit_order_distance", s.minLimitOrderDistance_m);
								sxml.setDouble("min_stop_loss_distance", s.minStopLossDistance_m);
								sxml.setDouble("min_stop_order_distance", s.minStopOrderDistance_m);
								sxml.setDouble("min_take_profit_distance", s.minTakeProfitDistance_m);
								sxml.setDouble("point_value", s.pointValue_m);
								sxml.setInt("sign_digits", s.signDigits_m);
								sxml.setDouble("simulation_slippage", s.simulationSlippage_m);
								sxml.setDouble("simulation_spread", s.simulationSpread_m);

								sxmlList.add(sxml);

						}

						request.setXmlParameterList("symbol_trade_info", sxmlList);
						request.setStringList("provider_list", providers);
						request.setStringList("symbol_list", symbols);

						response = RtTradingServerManager.instance().getTradingServer(serverName).issueCommand(request);

				} catch (Throwable e) {
						localExceptionCommandProcessing(serverName, e);
				}

				// OK
				finalizeCommandProcessing(serverName, request, response);

				// return new simulation UID
				return response.getUid("ticks_uid");
		}

		public void remote_startHistorySimulation(
			String runName,
			String runComment,
			String serverName,
			int tId,
			boolean singleSimRun,
			Uid simulationHistoryUid) throws Exception {
				XmlParameter request = null;
				XmlParameter response = null;

				try {
						request = new XmlParameter();
						request.setCommandName("start_history_simulation");

						request.setInt("algorithm_thread_id", tId);
						request.setString("run_name", runName);
						request.setString("run_comment", runComment);
						request.setInt("single_sim_run", singleSimRun ? 1 : 0);
						request.setUid("simulation_history_uid", simulationHistoryUid);

						response = RtTradingServerManager.instance().getTradingServer(serverName).issueCommand(request);

				} catch (Throwable e) {
						localExceptionCommandProcessing(serverName, e);
				}

				// OK
				finalizeCommandProcessing(serverName, request, response);

				// return new simulation UID
				simulationHistoryUid.fromUid(response.getUid("simulation_history_uid"));

		}

		public void remote_stopHistorySimulation(
			String serverName,
			int tId) throws Exception {
				XmlParameter request = null;
				XmlParameter response = null;

				try {
						request = new XmlParameter();
						request.setCommandName("stop_history_simulation");
						request.setInt("algorithm_thread_id", tId);

						response = RtTradingServerManager.instance().getTradingServer(serverName).issueCommand(request);

				} catch (Throwable e) {
						localExceptionCommandProcessing(serverName, e);
				}

				// OK
				finalizeCommandProcessing(serverName, request, response);
		}

		public void remote_getLoadedAlgorithmBrokerPairParameters(
			String serverName,
			String algBrokerItem,
			Map<String, String> algParams,
			Map<String, String> brkParams) throws Exception {
				XmlParameter request = null;
				XmlParameter response = null;

				try {
						request = new XmlParameter();
						request.setCommandName("get_loaded_algorithm_params");
						request.setString("name", algBrokerItem);

						// issue
						response = RtTradingServerManager.instance().getTradingServer(serverName).issueCommand(request);
				} catch (Throwable e) {
						localExceptionCommandProcessing(serverName, e);
				}

				// OK
				finalizeCommandProcessing(serverName, request, response);
		//

				XmlParameter alg_response = response.getXmlParameter("algorithm_params");
				XmlParameter brk_response = response.getXmlParameter("broker_params");

				HtUtils.parameterToMap(algParams, alg_response);
				HtUtils.parameterToMap(brkParams, brk_response);

		}

		public void remote_getLoadedAlgorithmBrokerPairParametersForTheThread(
			String serverName,
			String algBrokerItem,
			int tId,
			Map<String, String> algParams,
			Map<String, String> brkParams) throws Exception {
				XmlParameter request = null;
				XmlParameter response = null;

				try {
						request = new XmlParameter();
						request.setCommandName("reload_algorithm_parameters_for_thread");
						request.setString("name", algBrokerItem);
						request.setInt("thread_id", tId);

						// issue
						response = RtTradingServerManager.instance().getTradingServer(serverName).issueCommand(request);
				} catch (Throwable e) {
						localExceptionCommandProcessing(serverName, e);
				}

				// OK
				finalizeCommandProcessing(serverName, request, response);
		//

				XmlParameter alg_response = response.getXmlParameter("algorithm_params");
				XmlParameter brk_response = response.getXmlParameter("broker_params");

				HtUtils.parameterToMap(algParams, alg_response);
				HtUtils.parameterToMap(brkParams, brk_response);

		}

		public void remote_reloadAlgorithmBrokerPair(
			String serverName,
			String algBrokerItem,
			int tid,
			Map<String, String> algParams,
			Map<String, String> brokParams) throws Exception {
				XmlParameter request = null;
				XmlParameter response = null;

				try {
						request = new XmlParameter();
						request.setCommandName("reload_algorithm_parameters");

						request.setString("name", algBrokerItem);
						request.setInt("algorithm_thread_id", tid);

						XmlParameter xmlparameter = new XmlParameter();
						HtUtils.mapToParameter("algorithm_params", algParams, xmlparameter);

						StringBuilder out = new StringBuilder();
						xmlCommandThreadHandler_m.serializeParameter(xmlparameter, out);
						request.setString("algorithm_params", BinConverter.bytesToBinHex(out.toString().getBytes()));

						HtUtils.mapToParameter("broker_params", brokParams, xmlparameter);
						xmlCommandThreadHandler_m.serializeParameter(xmlparameter, out);
						request.setString("broker_params", BinConverter.bytesToBinHex(out.toString().getBytes()));

						// issue
						response = RtTradingServerManager.instance().getTradingServer(serverName).issueCommand(request);
				} catch (Throwable e) {
						localExceptionCommandProcessing(serverName, e);
				}

				// OK
				finalizeCommandProcessing(serverName, request, response);
		}

		public void remote_loadAlgorithmBrokerPair(
			String serverName,
			String run_name,
			String runComment,
			String algBrokerItem,
			String algPath,
			String brokerPath,
			String brokerPath2,
			Map<String, String> algParams,
			Map<String, String> brokParams,
			Map<String, String> brokParams2,
			int priorityId) throws Exception {
				XmlParameter request = null;
				XmlParameter response = null;

				try {
						request = new XmlParameter();
						request.setCommandName("load_algorithm");

						request.setString("name", algBrokerItem);
						request.setString("run_name", run_name);
						request.setString("run_comment", runComment);
						request.setString("algorithm_path", algPath);
						request.setString("broker_path", brokerPath);
						request.setString("broker_path2", brokerPath2);

						XmlParameter xmlparameter = new XmlParameter();
						HtUtils.mapToParameter("algorithm_params", algParams, xmlparameter);

						StringBuilder out = new StringBuilder();
						xmlCommandThreadHandler_m.serializeParameter(xmlparameter, out);
						request.setString("algorithm_params", BinConverter.bytesToBinHex(out.toString().getBytes()));

						HtUtils.mapToParameter("broker_params", brokParams, xmlparameter);
						xmlCommandThreadHandler_m.serializeParameter(xmlparameter, out);
						request.setString("broker_params", BinConverter.bytesToBinHex(out.toString().getBytes()));

						HtUtils.mapToParameter("broker_params2", brokParams2, xmlparameter);
						xmlCommandThreadHandler_m.serializeParameter(xmlparameter, out);
						request.setString("broker_params2", BinConverter.bytesToBinHex(out.toString().getBytes()));

						request.setInt("priority_id", priorityId);

						// issue
						response = RtTradingServerManager.instance().getTradingServer(serverName).issueCommand(request);
				} catch (Throwable e) {
						localExceptionCommandProcessing(serverName, e);
				}

				// OK
				finalizeCommandProcessing(serverName, request, response);
		}

		public void remote_unloadAlgorithmBrokerPair(String serverName, String algBrokerItem) throws Exception {
				XmlParameter request = null;
				XmlParameter response = null;

				try {
						request = new XmlParameter();
						request.setCommandName("unload_algorithm");
						request.setString("name", algBrokerItem);

						response = RtTradingServerManager.instance().getTradingServer(serverName).issueCommand(request);

				} catch (Throwable e) {
						localExceptionCommandProcessing(serverName, e);
				}

				// OK
				finalizeCommandProcessing(serverName, request, response);
		}

		public XmlParameter remote_requestServerState(String serverName) throws Exception {
				XmlParameter request = null;
				XmlParameter response = null;

				try {
						request = new XmlParameter();
						request.setCommandName("get_ping");

						response = RtTradingServerManager.instance().getTradingServer(serverName).issueCommand(request);
				} catch (Throwable e) {
						localExceptionCommandProcessing(serverName, e);
				}

				// OK
				finalizeCommandProcessing(serverName, request, response);

				return response;
		}

		public void remote_clearHistoryData(
			final String serverName,
			Map<String, Vector<HtPair<String, String>>> profProvSymbMap) throws Exception {
				XmlParameter request_clear = new XmlParameter();
				request_clear.setCommandName("clear_history_cache");

				RtTradingServer srv = RtTradingServerManager.instance().getTradingServer(serverName);

				try {
			// temporarly suspend RT provision
						// srv.suspendRtSending();

						srv.suspendRtPropagation();

						// implicitely commits RT thread!
						srv.waitDatabaseManagerWritePendingEvents();
						RtDatabaseManager.instance().commitRtThread();

						for (Iterator<String> it = profProvSymbMap.keySet().iterator(); it.hasNext();) {
								String profile_i = it.next();
								Vector<HtPair<String, String>> provSymbMap = profProvSymbMap.get(profile_i);

								for (int i = 0; i < provSymbMap.size(); i++) {

										String provider_i = provSymbMap.get(i).first;
										String symbol_i = provSymbMap.get(i).second;

										request_clear.setString("provider", provider_i);
										request_clear.setString("symbol", symbol_i);

										XmlParameter response_clear = RtTradingServerManager.instance().getTradingServer(serverName).issueCommand(request_clear);

										// OK - logging
										finalizeCommandProcessing(serverName, request_clear, response_clear);

								}

						}// end loop

				} catch (Throwable e) {
						localExceptionCommandProcessing(serverName, e);
				} finally {
			// resule RT provision
						//srv.resumeRtSending();
						srv.resumeRtPropagation();

				}
		}

		// pushes history data
		public void remote_pushHistoryData(
			final String serverName,
			long begdate,
			long enddate,
			Map<String, HtServerProxy_ProfileEntry> profProvSymbMap, // map profile <-> ( provider - symbol
			long rowsChunkCount, // make selections via this chunk
			boolean reuseCache, // flag showing that we may reuse cache
			boolean noSelectAtAll) throws Exception {
				final XmlParameter request = new XmlParameter();
				request.setCommandName("push_history_data");

		//XmlParameter request_clear = new XmlParameter();
				//request_clear.setCommandName("clear_history_cache");
				RtTradingServerManager trmgr = RtTradingServerManager.instance();
				RtDatabaseManager dbman = RtDatabaseManager.instance();
				RtTradingServer srv = trmgr.getTradingServer(serverName);

				try {
			// temporarly suspend RT provision
						// srv.suspendRtSending();
						srv.suspendRtPropagation();

						// implicitely commits RT thread!
						srv.waitDatabaseManagerWritePendingEvents();
						dbman.commitRtThread();

						//
						List<XmlParameter> symbols_metadata = new ArrayList<XmlParameter>();

			//LongParam minDate = new LongParam(-1);
						//LongParam maxDate = new LongParam(-1);
						// claculate time stamps of data we will send
						for (Iterator<String> it = profProvSymbMap.keySet().iterator(); it.hasNext();) {
								String profile_i = it.next();
								HtServerProxy_ProfileEntry entryi_i = profProvSymbMap.get(profile_i);
								List<HtPair<String, String>> provSymbMap = entryi_i.providerSymbolList_m;

								for (int i = 0; i < provSymbMap.size(); i++) {
										String provider_i = provSymbMap.get(i).first;
										String symbol_i = provSymbMap.get(i).second;

					//RtDatabaseSelectionManager.instance().getLoadedMinMaxDates(profile_i, symbol_i, provider_i, minDate, maxDate);
										XmlParameter dataPair = new XmlParameter();
					//dataPair.setInt("date_begin_msec", minDate.getLong());
										//dataPair.setInt("date_end_msec", maxDate.getLong());
										dataPair.setString("provider", provider_i);
										dataPair.setString("symbol", symbol_i);
										dataPair.setString("profile", profile_i);

										symbols_metadata.add(dataPair);

								}
						}

						// store everything
						request.setDate("begin_query_date", begdate);
						request.setDate("end_query_date", enddate);
						request.setXmlParameterList("symbol_metadata", symbols_metadata);
						request.setInt("keep_cache_after_shutdown", reuseCache ? 1 : 0);
						request.setInt("no_data_to_load", noSelectAtAll ? 1 : 0);

						XmlParameter response = RtTradingServerManager.instance().getTradingServer(serverName).issueCommand(request);
						finalizeCommandProcessing(serverName, null, response);

				} catch (Throwable e) {
						localExceptionCommandProcessing(serverName, e);
				} finally {
			// resule RT provision
						//srv.resumeRtSending();
						srv.resumeRtPropagation();

				}

		}

		// returns algorithm name and priority
		public Map<String, Integer> remote_requestLoadedAlgorithmBrokerPairs(String serverName, int tId) throws Exception {
				XmlParameter request = null;
				XmlParameter response = null;

				try {
						request = new XmlParameter();

						request.setCommandName("get_loaded_algorithms");
						request.setInt("thread_id", tId);

						response = RtTradingServerManager.instance().getTradingServer(serverName).issueCommand(request);

				} catch (Throwable e) {
						localExceptionCommandProcessing(serverName, e);
				}

				// OK
				finalizeCommandProcessing(serverName, request, response);

				HashMap<String, Integer> aids = new HashMap<String, Integer>();
				List<String> algs = response.getStringList("loaded_algorithms");
				List<Long> priors = response.getIntList("priorities");

				for (int i = 0; i < algs.size(); i++) {
						long priority = priors.get(i);
						aids.put(algs.get(i), (int) priority);
				}

				return aids;
		}

		public boolean remote_isAlgorithmLoaded(String serverName, String algBrkPair) throws Exception {
				XmlParameter request = null;
				XmlParameter response = null;

				try {
						request = new XmlParameter();

						request.setCommandName("is_algorithm_loaded");
						request.setString("alg_lib_pair", algBrkPair);

						response = RtTradingServerManager.instance().getTradingServer(serverName).issueCommand(request);

				} catch (Throwable e) {
						localExceptionCommandProcessing(serverName, e);
				}

		// OK
				finalizeCommandProcessing(serverName, request, response);

				return (response.getInt("is_algorithm_loaded") == 1);
		}

		public List<Integer> remote_requestStartedThreads(String serverName) throws Exception {
				XmlParameter request = null;
				XmlParameter response = null;

				try {
						request = new XmlParameter();

						request.setCommandName("get_started_threads");

						response = RtTradingServerManager.instance().getTradingServer(serverName).issueCommand(request);

				} catch (Throwable e) {
						localExceptionCommandProcessing(serverName, e);
				}

				// OK
				finalizeCommandProcessing(serverName, request, response);

				List<Integer> result = new ArrayList<Integer>();
				List<Long> r = response.getIntList("started_threads");

				for (int i = 0; i < r.size(); i++) {
						long rtid = r.get(i);
						result.add((int) rtid);
				}

				return result;

		}

		public List<ThreadDescriptor> remote_requestStartedThreads2(String serverName) throws Exception {
				XmlParameter request = null;
				XmlParameter response = null;

				try {
						request = new XmlParameter();

						request.setCommandName("get_started_threads2");

						response = RtTradingServerManager.instance().getTradingServer(serverName).issueCommand(request);

				} catch (Throwable e) {
						localExceptionCommandProcessing(serverName, e);
				}

				// OK
				finalizeCommandProcessing(serverName, request, response);

				List<ThreadDescriptor> threads = new ArrayList<ThreadDescriptor>();

				List<XmlParameter> threads_xmlparam = response.getXmlParameterList("threads");
				for (int i = 0; i < threads_xmlparam.size(); i++) {

						ThreadDescriptor tdescr_i = new ThreadDescriptor();
						XmlParameter param_i = threads_xmlparam.get(i);

						tdescr_i.tid_m = (int) param_i.getInt("tid");
						tdescr_i.inputQueuePendingSize_m = (int) param_i.getInt("input_queue_pending_size");
						tdescr_i.tstatus_m = (int) param_i.getInt("thread_status_int");
						tdescr_i.loadedAlgBrkPairs_m.addAll(param_i.getStringList("loaded_alg_brk_pairs"));

						threads.add(tdescr_i);
				}

				return threads;

		}

	// for clusterization
		// query all servers and determine the number of active threads
		// on failure 
		public String remote_returnNextAvailableServer(List<String> serverList, int maxThreadsPerServer) throws Exception {
				String result = null;
				for (int i = 0; i < serverList.size(); i++) {
						String server_i = serverList.get(i);

						List<Integer> sthreads = remote_requestStartedThreads(server_i);
						if (maxThreadsPerServer > sthreads.size()) {
								return server_i;
						}

				}

				return result;
		}

		public int remote_requestThreadStatus(String serverName, int tId) throws Exception {
				XmlParameter request = null;
				XmlParameter response = null;

				try {
						request = new XmlParameter();

						request.setCommandName("get_thread_status");
						request.setInt("thread_id", tId);

						response = RtTradingServerManager.instance().getTradingServer(serverName).issueCommand(request);

				} catch (Throwable e) {
						localExceptionCommandProcessing(serverName, e);
				}

				// OK
				finalizeCommandProcessing(serverName, request, response);

				return (int) response.getInt("thread_status_int");

		}

		/*
		 */
		/**
		 */
	// this returns proxy connection object which created only in the context of thread and
		// only in the context of loaded alg brk pair
		// this proxy object is intended to perform operations only
		// you cannot create this manually
		public BrokerConnect remote_createBrokerConnectionObject(
			String serverName,
			String algLibPair,
			int tId,
			int brokerSeqNum,
			String runName,
			String runComment,
			String connectString,
			final long waitsec) throws Exception {
				// create connection object
				CallingContext callContext = new CallingContext("remote_createBrokerConnectionObject");

				callContext.<String_Proxy>setParameter("connect_string", ParamBase.instantiate("String_Proxy")).set(new String(connectString));
				callContext.<String_Proxy>setParameter("run_name", ParamBase.instantiate("String_Proxy")).set(new String(runName));
				callContext.<String_Proxy>setParameter("run_comment", ParamBase.instantiate("String_Proxy")).set(new String(runComment));
				callContext.<String_Proxy>setParameter("alg_lib_pair", ParamBase.instantiate("String_Proxy")).set(new String(algLibPair));

				// out parameters
				callContext.<Uid_Proxy>setParameter("uid", ParamBase.instantiate("Uid_Proxy"));
				callContext.<Bool_Proxy>setParameter("is_connected", ParamBase.instantiate("Bool_Proxy"));
				callContext.<Int_Proxy>setParameter("state", ParamBase.instantiate("Int_Proxy"));

				XmlEvent commandEvent = prepareRemoteEvent(callContext, true);

				XmlEvent brkRespdata = new XmlEvent();

				remote_pushEventToServer(serverName, tId, brokerSeqNum, commandEvent, waitsec, null, brkRespdata, false, true);
				if (brkRespdata.getType() == XmlEvent.DT_Dummy) {
						throw new HtException(getContext(), "remote_createBrokerConnectionObject", "Wait timeout expired(sec): \"" + waitsec + "\"");
				}

				CallingContext.convertCallingContextFromXmlParameter(callContext, brkRespdata.getAsXmlParameter());
				if (callContext.getResultCode() != 0) {
						throw new HtException(getContext(), "remote_createBrokerConnectionObject", "Remote call failed: \"" + callContext.getResultReason() + "\"");
				}

				return new BrokerConnect(this,
					callContext.<Uid_Proxy>getParameter("uid").get(),
					connectString,
					callContext.<Bool_Proxy>getParameter("is_connected").get(),
					callContext.<Int_Proxy>getParameter("state").get(),
					callContext.<String_Proxy>getParameter("run_name").get(),
					callContext.<String_Proxy>getParameter("run_comment").get());

		}

		// this deletes broker connection object
		public void remote_deleteBrokerConnectionObject(
			String serverName,
			String algLibPair,
			int tId,
			int brokerSeqNum,
			Uid connectionUid,
			final long waitsec) throws Exception {
				// create connection object
				CallingContext callContext = new CallingContext("remote_deleteBrokerConnectionObject");

				callContext.<Uid_Proxy>setParameter("uid", ParamBase.instantiate("Uid_Proxy")).set(new Uid(connectionUid));
				callContext.<String_Proxy>setParameter("alg_lib_pair", ParamBase.instantiate("String_Proxy")).set(algLibPair);

				XmlEvent commandEvent = prepareRemoteEvent(callContext, true);

				XmlEvent brkRespdata = new XmlEvent();

				remote_pushEventToServer(serverName, tId, brokerSeqNum, commandEvent, waitsec, null, brkRespdata, false, true);
				if (brkRespdata.getType() == XmlEvent.DT_Dummy) {
						throw new HtException(getContext(), "remote_deleteBrokerConnectionObject", "Wait timeout expired(sec): \"" + waitsec + "\"");
				}

				CallingContext.convertCallingContextFromXmlParameter(callContext, brkRespdata.getAsXmlParameter());
				if (callContext.getResultCode() != 0) {
						throw new HtException(getContext(), "remote_deleteBrokerConnectionObject", "Remote call failed: \"" + callContext.getResultReason() + "\"");
				}

		}

		// this connect BrokerConnect object
		public BrokerConnect remote_connectBrokerConnectionObject(
			String serverName,
			String algLibPair,
			int tId,
			int brokerSeqNum,
			Uid connectionUid, // use existing connection
			final long waitsec) throws Exception {

				CallingContext callContext = new CallingContext("remote_connectBrokerConnectionObject");
				callContext.<Uid_Proxy>setParameter("uid", ParamBase.instantiate("Uid_Proxy")).set(new Uid(connectionUid));
				callContext.<String_Proxy>setParameter("alg_lib_pair", ParamBase.instantiate("String_Proxy")).set(new String(algLibPair));

				// out parameters
				callContext.<Bool_Proxy>setParameter("is_connected", ParamBase.instantiate("Bool_Proxy"));
				callContext.<Int_Proxy>setParameter("state", ParamBase.instantiate("Int_Proxy"));
				callContext.<String_Proxy>setParameter("run_name", ParamBase.instantiate("String_Proxy"));
				callContext.<String_Proxy>setParameter("run_comment", ParamBase.instantiate("String_Proxy"));
				callContext.<String_Proxy>setParameter("connect_string", ParamBase.instantiate("String_Proxy"));

				XmlEvent commandEvent = prepareRemoteEvent(callContext, true);

				XmlEvent brkRespdata = new XmlEvent();

				remote_pushEventToServer(serverName, tId, brokerSeqNum, commandEvent, waitsec, null, brkRespdata, false, true);
				if (brkRespdata.getType() == XmlEvent.DT_Dummy) {
						throw new HtException(getContext(), "remote_connectBrokerConnectionObject", "Wait timeout expired(sec): \"" + waitsec + "\"");
				}

				CallingContext.convertCallingContextFromXmlParameter(callContext, brkRespdata.getAsXmlParameter());
				if (callContext.getResultCode() != 0) {
						throw new HtException(getContext(), "remote_connectBrokerConnectionObject", "Remote call failed: \"" + callContext.getResultReason() + "\"");
				}

				// substitute the connection object
				BrokerConnect connection = new BrokerConnect(this,
					callContext.<Uid_Proxy>getParameter("uid").get(),
					callContext.<String_Proxy>getParameter("connect_string").get(),
					callContext.<Bool_Proxy>getParameter("is_connected").get(),
					callContext.<Int_Proxy>getParameter("state").get(),
					callContext.<String_Proxy>getParameter("run_name").get(),
					callContext.<String_Proxy>getParameter("run_comment").get());

				return connection;

		}

		// this disconnects broker connection object
		public BrokerConnect remote_disconnectBrokerConnectionObject(
			String serverName,
			String algLibPair,
			int tId,
			int brokerSeqNum,
			Uid connectionUid, // use existing connection
			final long waitsec) throws Exception {

				CallingContext callContext = new CallingContext("remote_disconnectBrokerConnectionObject");
				callContext.<Uid_Proxy>setParameter("uid", ParamBase.instantiate("Uid_Proxy")).set(new Uid(connectionUid));
				callContext.<String_Proxy>setParameter("alg_lib_pair", ParamBase.instantiate("String_Proxy")).set(new String(algLibPair));

				// out parameters
				callContext.<Bool_Proxy>setParameter("is_connected", ParamBase.instantiate("Bool_Proxy"));
				callContext.<Int_Proxy>setParameter("state", ParamBase.instantiate("Int_Proxy"));
				callContext.<String_Proxy>setParameter("run_name", ParamBase.instantiate("String_Proxy"));
				callContext.<String_Proxy>setParameter("run_comment", ParamBase.instantiate("String_Proxy"));
				callContext.<String_Proxy>setParameter("connect_string", ParamBase.instantiate("String_Proxy"));

				XmlEvent commandEvent = prepareRemoteEvent(callContext, true);
				XmlEvent brkRespdata = new XmlEvent();

				remote_pushEventToServer(serverName, tId, brokerSeqNum, commandEvent, waitsec, null, brkRespdata, false, true);
				if (brkRespdata.getType() == XmlEvent.DT_Dummy) {
						throw new HtException(getContext(), "remote_disconnectBrokerConnectionObject", "Wait timeout expired(sec): \"" + waitsec + "\"");
				}

				CallingContext.convertCallingContextFromXmlParameter(callContext, brkRespdata.getAsXmlParameter());
				if (callContext.getResultCode() != 0) {
						throw new HtException(getContext(), "remote_disconnectBrokerConnectionObject", "Remote call failed: \"" + callContext.getResultReason() + "\"");
				}

				// substitute the connection object
				BrokerConnect connection = new BrokerConnect(this,
					callContext.<Uid_Proxy>getParameter("uid").get(),
					callContext.<String_Proxy>getParameter("connect_string").get(),
					callContext.<Bool_Proxy>getParameter("is_connected").get(),
					callContext.<Int_Proxy>getParameter("state").get(),
					callContext.<String_Proxy>getParameter("run_name").get(),
					callContext.<String_Proxy>getParameter("run_comment").get());

				return connection;
		}

		public void remote_rebounceBrokerConnection(
			String serverName,
			String algLibPair,
			int tId,
			int brokerSeqNum,
			Uid connectionUid,
			final long waitsec) throws Exception {
				CallingContext callContext = new CallingContext("remote_rebounceBrokerConnection");

				callContext.<Uid_Proxy>setParameter("uid", ParamBase.instantiate("Uid_Proxy")).set(new Uid(connectionUid));
				callContext.<String_Proxy>setParameter("alg_lib_pair", ParamBase.instantiate("String_Proxy")).set(new String(algLibPair));

				XmlEvent commandEvent = prepareRemoteEvent(callContext, true);
				XmlEvent brkRespdata = new XmlEvent();

				remote_pushEventToServer(serverName, tId, brokerSeqNum, commandEvent, waitsec, null, brkRespdata, false, true);
				if (brkRespdata.getType() == XmlEvent.DT_Dummy) {
						throw new HtException(getContext(), "remote_rebounceBrokerConnection", "Wait timeout expired(sec): \"" + waitsec + "\"");
				}

				CallingContext.convertCallingContextFromXmlParameter(callContext, brkRespdata.getAsXmlParameter());
				if (callContext.getResultCode() != 0) {
						throw new HtException(getContext(), "remote_rebounceBrokerConnection", "Remote call failed: \"" + callContext.getResultReason() + "\"");
				}

		}

		public List<BrokerConnect> remote_returnAvailableBrokerConnections(
			String serverName,
			String algLibPair,
			int tId,
			int brokerSeqNum,
			final long waitsec) throws Exception {
				List<BrokerConnect> result = new ArrayList<BrokerConnect>();
				CallingContext callContext = new CallingContext("remote_returnAvailableBrokerConnections");

				callContext.<String_Proxy>setParameter("alg_lib_pair", ParamBase.instantiate("String_Proxy")).set(algLibPair);

				// out parameters
				callContext.<UidList_Proxy>setParameter("uid_sessions", ParamBase.instantiate("UidList_Proxy"));
				callContext.<StringList_Proxy>setParameter("connect_string_sessions", ParamBase.instantiate("StringList_Proxy"));
				callContext.<BoolList_Proxy>setParameter("connect_statuses_sessions", ParamBase.instantiate("BoolList_Proxy"));
				callContext.<IntList_Proxy>setParameter("states_sessions", ParamBase.instantiate("IntList_Proxy"));
				callContext.<StringList_Proxy>setParameter("run_names_sessions", ParamBase.instantiate("StringList_Proxy"));
				callContext.<StringList_Proxy>setParameter("run_comments_sessions", ParamBase.instantiate("StringList_Proxy"));

				XmlEvent commandEvent = prepareRemoteEvent(callContext, true);

				XmlEvent brkRespdata = new XmlEvent();

				remote_pushEventToServer(serverName, tId, brokerSeqNum, commandEvent, waitsec, null, brkRespdata, false, true);
				if (brkRespdata.getType() == XmlEvent.DT_Dummy) {
						throw new HtException(getContext(), "remote_returnAvailableBrokerConnections", "Wait timeout expired(sec): \"" + waitsec + "\"");
				}

				CallingContext.convertCallingContextFromXmlParameter(callContext, brkRespdata.getAsXmlParameter());
				if (callContext.getResultCode() != 0) {
						throw new HtException(getContext(), "remote_returnAvailableBrokerConnections", "Remote call failed: \"" + callContext.getResultReason() + "\"");
				}

				List<Uid> connectUids = callContext.<UidList_Proxy>getParameter("uid_sessions").get();
				List<String> connectStrings = callContext.<StringList_Proxy>getParameter("connect_string_sessions").get();
				List<Boolean> connectStatuses = callContext.<BoolList_Proxy>getParameter("connect_statuses_sessions").get();
				List<Long> states = callContext.<IntList_Proxy>getParameter("states_sessions").get();
				List<String> runNames = callContext.<StringList_Proxy>getParameter("run_names_sessions").get();
				List<String> runComments = callContext.<StringList_Proxy>getParameter("run_comments_sessions").get();

				for (int i = 0; i < connectUids.size(); i++) {
						BrokerConnect connect = new BrokerConnect(this, connectUids.get(i), connectStrings.get(i), connectStatuses.get(i), states.get(i), runNames.get(i), runComments.get(i));
						result.add(connect);
				}

				return result;
		}

		// this makes unblocking call
		public boolean remote_op_unblockingIssueOrder(
			String serverName,
			String algLibPair,
			int tId,
			int brokerSeqNum,
			BrokerConnect connect,
			Order order,
			final long waitsec) throws Exception {
				CallingContext callContext = new CallingContext("remote_op_unblockingIssueOrder");

				callContext.<Uid_Proxy>setParameter("uid", ParamBase.instantiate("Uid_Proxy")).set(new Uid(connect.getUid()));
				callContext.<String_Proxy>setParameter("alg_lib_pair", ParamBase.instantiate("String_Proxy")).set(new String(algLibPair));
				callContext.<Order_Proxy>setParameter("order", ParamBase.instantiate("Order_Proxy")).set(order);

				// out parameter
				callContext.<PositionList_Proxy>setParameter("order_issue_result", ParamBase.instantiate("Bool_Proxy"));

				XmlEvent commandEvent = prepareRemoteEvent(callContext, true);

				XmlEvent brkRespdata = new XmlEvent();
				remote_pushEventToServer(serverName, tId, brokerSeqNum, commandEvent, waitsec, null, brkRespdata, false, true);

				if (brkRespdata.getType() == XmlEvent.DT_Dummy) {
						throw new HtException(getContext(), "remote_op_unblockingIssueOrder", "Wait timeout expired(sec): \"" + waitsec + "\"");
				}

				CallingContext.convertCallingContextFromXmlParameter(callContext, brkRespdata.getAsXmlParameter());
				if (callContext.getResultCode() != 0) {
						throw new HtException(getContext(), "remote_op_unblockingIssueOrder", "Remote call failed: \"" + callContext.getResultReason() + "\"");
				}

				boolean result = callContext.<Bool_Proxy>getParameter("order_issue_result").get();
				return result;
		}

		public List<Position> remote_op_getBrokerPositionList(
			String serverName,
			String algLibPair,
			int tId,
			int brokerSeqNum,
			int posState, // PosState
			BrokerConnect connect,
			final long waitsec) throws Exception {
				List<Position> positionList = new ArrayList<Position>();

				CallingContext callContext = new CallingContext("remote_op_getBrokerPositionList");

				callContext.<Uid_Proxy>setParameter("uid", ParamBase.instantiate("Uid_Proxy")).set(new Uid(connect.getUid()));
				callContext.<Int_Proxy>setParameter("pos_state", ParamBase.instantiate("Int_Proxy")).set(new Long(posState));
				callContext.<String_Proxy>setParameter("alg_lib_pair", ParamBase.instantiate("String_Proxy")).set(new String(algLibPair));

				// out parameters
				callContext.<PositionList_Proxy>setParameter("position_list", ParamBase.instantiate("PositionList_Proxy"));

				XmlEvent commandEvent = prepareRemoteEvent(callContext, true);

				XmlEvent brkRespdata = new XmlEvent();
				remote_pushEventToServer(serverName, tId, brokerSeqNum, commandEvent, waitsec, null, brkRespdata, false, true);
				if (brkRespdata.getType() == XmlEvent.DT_Dummy) {
						throw new HtException(getContext(), "remote_op_getBrokerPositionList", "Wait timeout expired(sec): \"" + waitsec + "\"");
				}

				CallingContext.convertCallingContextFromXmlParameter(callContext, brkRespdata.getAsXmlParameter());
				if (callContext.getResultCode() != 0) {
						throw new HtException(getContext(), "remote_op_getBrokerPositionList", "Remote call failed: \"" + callContext.getResultReason() + "\"");
				}

				positionList.addAll(callContext.<PositionList_Proxy>getParameter("position_list").get());

				return positionList;
		}

		// this is another version
		// it does not wait results in another queue, instead ше акууяуы рееквку ьфшт йгугу
		public String remote_SendCustomEvent_toAlgorithm_direct(
			String serverName,
			String algLibPair,
			int tId,
			String customName,
			String customData,
			long waitsec
		) throws Exception {

				CallingContext callContext = new CallingContext("remote_SendCustomEvent", customName);
				callContext.<String_Proxy>setParameter("alg_lib_pair", ParamBase.instantiate("String_Proxy")).set(algLibPair);
				callContext.<String_Proxy>setParameter("custom_data", ParamBase.instantiate("String_Proxy")).set(customData);

				if (customName.length() <= 0) {
						throw new HtException(getContext(), "remote_SendCustomEvent_toAlgorithm_direct", "Invalid custom event name!");
				}

				// out
				XmlEvent commandEvent = prepareRemoteEvent(callContext, false);
				XmlParameter brkAlgdata = new XmlParameter();

				// sent to algorithm brokerSeqNum==-1
				remote_pushEventToServer_direct_Algorithm(serverName, tId, commandEvent, brkAlgdata, waitsec);
				
				XmlEvent brkAlgdata_e = new XmlEvent();
				XmlEvent.convertXmlParameterToEvent(brkAlgdata_e, brkAlgdata);
				if (brkAlgdata_e.getEventType() != XmlEvent.ET_AlgorithmBrokerEventResp_FromAlgorithm)
						throw new HtException(getContext(), "remote_SendCustomEvent_toAlgorithm_direct", "Invalid event type returned");
				
				CallingContext.convertCallingContextFromXmlParameter(callContext, brkAlgdata_e.getAsXmlParameter());
				if (callContext.getResultCode() != 0) {
						throw new HtException(getContext(), "remote_SendCustomEvent_toAlgorithm_direct", "Remote call failed: \"" + callContext.getResultReason() + "\"");
				}

				// get result
				return callContext.<String_Proxy>getParameter("out_return_data").get();
		}

		// sends arbitrary event to algoritm layer
		public String remote_SendCustomEvent_toAlgorithm(
			String serverName,
			String algLibPair,
			int tId,
			String customName,
			String customData,
			final long waitsec) throws Exception {
				CallingContext callContext = new CallingContext("remote_SendCustomEvent", customName);
				callContext.<String_Proxy>setParameter("alg_lib_pair", ParamBase.instantiate("String_Proxy")).set(algLibPair);
				callContext.<String_Proxy>setParameter("custom_data", ParamBase.instantiate("String_Proxy")).set(customData);

				if (customName.length() <= 0) {
						throw new HtException(getContext(), "remote_SendCustomEvent_toAlgorithm", "Invalid custom event name!");
				}

				// out
				XmlEvent commandEvent = prepareRemoteEvent(callContext, false);
				XmlEvent brkAlgdata = new XmlEvent();

				// sent to algorithm brokerSeqNum==-1
				remote_pushEventToServer(serverName, tId, -1, commandEvent, waitsec, brkAlgdata, null, true, false);
				if (brkAlgdata.getType() == XmlEvent.DT_Dummy) {
						throw new HtException(getContext(), "remote_SendCustomEvent_toAlgorithm", "Wait timeout expired(sec): \"" + waitsec + "\"");
				}

				CallingContext.convertCallingContextFromXmlParameter(callContext, brkAlgdata.getAsXmlParameter());
				if (callContext.getResultCode() != 0) {
						throw new HtException(getContext(), "remote_SendCustomEvent_toAlgorithm", "Remote call failed: \"" + callContext.getResultReason() + "\"");
				}

				// get result
				return callContext.<String_Proxy>getParameter("out_return_data").get();
		}

		// nowait version
		public void remote_SendCustomEvent_toAlgorithm_NoWait(
			String serverName,
			String algLibPair,
			int tId,
			String customName,
			String customData) throws Exception {
				CallingContext callContext = new CallingContext("remote_SendCustomEvent", customName);
				callContext.<String_Proxy>setParameter("alg_lib_pair", ParamBase.instantiate("String_Proxy")).set(algLibPair);
				callContext.<String_Proxy>setParameter("custom_data", ParamBase.instantiate("String_Proxy")).set(customData);

		// out
				XmlEvent commandEvent = prepareRemoteEvent(callContext, false);
				remote_pushEventToServer(serverName, tId, -1, commandEvent, -1, null, null, false, false);

		}

		// sends arbitrary event to broker! layer
		public String remote_SendCustomEvent_toBroker(
			String serverName,
			String algLibPair,
			int tId,
			int brokerSeqNum,
			String customName,
			String customData,
			final long waitsec) throws Exception {
				CallingContext callContext = new CallingContext("remote_BrokerCustomEvent", customName);
				callContext.<String_Proxy>setParameter("alg_lib_pair", ParamBase.instantiate("String_Proxy")).set(algLibPair);
				callContext.<String_Proxy>setParameter("custom_data", ParamBase.instantiate("String_Proxy")).set(customData);

		// out
				// true - to broker
				XmlEvent commandEvent = prepareRemoteEvent(callContext, true);
				XmlEvent brkRespdata = new XmlEvent();

				remote_pushEventToServer(serverName, tId, brokerSeqNum, commandEvent, waitsec, null, brkRespdata, false, true);

				if (brkRespdata.getType() == XmlEvent.DT_Dummy) {
						throw new HtException(getContext(), "remote_SendCustomEvent_toBroker", "Wait timeout expired(sec): \"" + waitsec + "\"");
				}

				CallingContext.convertCallingContextFromXmlParameter(callContext, brkRespdata.getAsXmlParameter());
				if (callContext.getResultCode() != 0) {
						throw new HtException(getContext(), "remote_SendCustomEvent_toBroker", "Remote call failed: \"" + callContext.getResultReason() + "\"");
				}

				// get result
				return callContext.<String_Proxy>getParameter("out_return_data").get();
		}

	// ------------------------
		// helpers
		public XmlEvent prepareRemoteEvent(CallingContext callContext, boolean to_broker) throws Exception {
				XmlEvent commandEvent = new XmlEvent();

				commandEvent.setEventType(to_broker ? XmlEvent.ET_BrokerEvent : XmlEvent.ET_AlgorithmEvent);
				commandEvent.setType(XmlEvent.DT_CommonXml);
				commandEvent.generateUid();

				commandEvent.setEventDate(HtDateTimeUtils.getCurGmtTime());

				// arbitrary out data
				callContext.<String_Proxy>setParameter("out_return_data", ParamBase.instantiate("String_Proxy")).set(new String());

				CallingContext.convertCallingContextToXmlParameter(callContext, commandEvent.getAsXmlParameter(true));

				return commandEvent;
		}

		public void remote_pushEventToServer_direct_Algorithm(
			String serverName,
			int tId, // thread ID
			XmlEvent data, // data to send
			XmlParameter resp_data, // response from algorithm
			long waitsec
		) throws Exception {

				XmlParameter request = null;
				XmlParameter response = null;

			
			

				if (!data.getUid().isValid()) {
						throw new HtException(getContext(), "remote_pushEventToServer_direct", "Invalid event UID");
				}

				request = new XmlParameter();
				request.setCommandName("push_thread_event_blocking");
				request.setInt("algorithm_thread_id", tId);
				

				// to alg level
				request.setInt("broker_seq_num", -1);
				request.setInt("wait_sec", waitsec);
				XmlParameter eventdata = new XmlParameter();
				XmlEvent.convertEventToXmlParameter(data, eventdata);
				request.setXmlParameter("event", eventdata);

				try {
						// do not wait anything
						response = RtTradingServerManager.instance().getTradingServer(serverName).issueCommand(request);
						resp_data.create( response.getXmlParameter("response_event") );
						
					
				} catch (Throwable e) {
						localExceptionCommandProcessing(serverName, e);
				}

				// OK
				finalizeCommandProcessing(serverName, request, response);

		}
	// pushes event to the server
		// thats the base functions for working with broker operations and sessions
		public void remote_pushEventToServer(
			String serverName,
			int tId, // thread ID
			int brokerSeqNum, // broker number to be used when sending request
			XmlEvent data, // data to send
			final long waitsec,
			final XmlEvent algrespdata, // response from algorithm
			final XmlEvent brkrespdata, // response from broker layer
			final boolean waitalg,
			final boolean waitbrk
		) throws Exception {
				XmlParameter request = null;
				XmlParameter response = null;

				try {

						if (algrespdata != null) {
								algrespdata.clear();
						}

						if (brkrespdata != null) {
								brkrespdata.clear();
						}

						if (!data.getUid().isValid()) {
								throw new HtException(getContext(), "remote_pushEventToServer", "Invalid event UID");
						}

						request = new XmlParameter();
						request.setCommandName("push_thread_event");
						request.setInt("algorithm_thread_id", tId);
						if (!(brokerSeqNum == 1 || brokerSeqNum == 2 || brokerSeqNum == -1)) {
								throw new HtException(getContext(), "remote_pushEventToServer", "Broker sequence number can be only 1 or 2");
						}

						request.setInt("broker_seq_num", brokerSeqNum);
						XmlParameter eventdata = new XmlParameter();
						XmlEvent.convertEventToXmlParameter(data, eventdata);
						request.setXmlParameter("event", eventdata);

						if (waitbrk || waitalg) {

								//final Queue arrived = new Queue();
								final SingleReaderQueue arrived = new SingleReaderQueue("Server Event Queue", false, RtConfigurationManager.StartUpConst.MAX_ELEMENTS_IN_QUEUE);

								final Uid srcUid = new Uid(data.getUid());

								final ArrayList list = new ArrayList();

								// listening thread
								Thread runnable = new Thread() {

										@Override
										public void run() {

												boolean to_stop = false;
												HtTimeCalculator tcalc = new HtTimeCalculator();

												while (true) {

														if (to_stop) {
																break;
														}

							// wait infinetely
														//XmlEvent returned = (XmlEvent) arrived.get(1000);
														// wait 1 sec
														arrived.get(list, 1000);

														for (Iterator it = list.iterator(); it.hasNext();) {
																XmlEvent returned = (XmlEvent) it.next();

																if (returned != null) {

																		// this is our type
																		if (returned.getEventType() == XmlEvent.ET_AlgorithmBrokerEventResp_FromAlgorithm && algrespdata != null) {
																				algrespdata.create(returned);
																		}

																		if (returned.getEventType() == XmlEvent.ET_AlgorithmBrokerEventResp_FromBroker && brkrespdata != null) {
																				brkrespdata.create(returned);
																		}

																		// check
																		if (waitalg && !waitbrk) {
																				if (algrespdata != null && algrespdata.getEventType() == XmlEvent.ET_AlgorithmBrokerEventResp_FromAlgorithm) {
																						to_stop = true;
																						break;
																				}
																		} else if (!waitalg && waitbrk) {
																				if (brkrespdata != null && brkrespdata.getEventType() == XmlEvent.ET_AlgorithmBrokerEventResp_FromBroker) {
																						to_stop = true;
																						break;
																				}
																		} else if (waitalg && waitbrk) {
																				if ((brkrespdata != null && brkrespdata.getEventType() == XmlEvent.ET_AlgorithmBrokerEventResp_FromBroker)
																					&& (algrespdata != null && algrespdata.getEventType() == XmlEvent.ET_AlgorithmBrokerEventResp_FromAlgorithm)) {
																						to_stop = true;
																						break;
																				}
																		}

																}

														} // end loop for events

														if (tcalc.elapsed() > waitsec * 1000) {
																break;
														}

												}// end main loop
										}
								};

								// start listening thread
								runnable.start();

								// now need to wait the response
								HtEventListener listener = new HtEventListener() {

										@Override
										public void onEventArrived(XmlEvent event) throws SingleReaderQueueExceedLimit {
												// only events with

												if (event.getParentUid().equals(srcUid)) {
														arrived.put(event);
												}
										}

										@Override
										public String getListenerName() {
												return " listener: [ trading server command listener ] ";
										}
								};

								try {
										// subscribe
										RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).subscribeForEvent(XmlEvent.ET_AlgorithmBrokerEventResp_FromAlgorithm, 2, listener);
										RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).subscribeForEvent(XmlEvent.ET_AlgorithmBrokerEventResp_FromBroker, 2, listener);

										// send the command
										response = RtTradingServerManager.instance().getTradingServer(serverName).issueCommand(request);

										// wait inifinitely as here we have no problema, look at issue command
										runnable.join();

										// OK
										finalizeCommandProcessing(serverName, request, response);

								} catch (Throwable e) {
										throw new HtException(getContext(), "remote_pushEventToServer", "Exception happened on trading server event processing: " + e.getMessage());
								} finally {
										RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).unsubscribeForAllEvents(listener);
								}
						} else {
								// do not wait anything
								response = RtTradingServerManager.instance().getTradingServer(serverName).issueCommand(request);

								// OK
								finalizeCommandProcessing(serverName, request, response);

						}

				} catch (Throwable e) {
						localExceptionCommandProcessing(serverName, e);
				}
		}

		public int remote_getLogLevel(String serverName) throws Exception {
				XmlParameter request = null;
				XmlParameter response = null;

				try {
						request = new XmlParameter();
						request.setCommandName("get_log_level");

						response = RtTradingServerManager.instance().getTradingServer(serverName).issueCommand(request);

				} catch (Throwable e) {
						localExceptionCommandProcessing(serverName, e);
				}

				// OK
				finalizeCommandProcessing(serverName, request, response);

				int loglevel = (int) response.getInt("log_level");
				if (!CommonLog.getAvailableLogLevels().contains(loglevel)) {
						throw new HtException(getContext(), "remote_getLogLevel", "Invalid log level");
				}

				return loglevel;
		}

		// set up log level
		public void remote_setlogLevel(String serverName, int loglevel) throws Exception {
				XmlParameter request = null;
				XmlParameter response = null;

				try {
						request = new XmlParameter();
						request.setCommandName("set_log_level");
						if (!CommonLog.getAvailableLogLevels().contains(loglevel)) {
								throw new HtException(getContext(), "remote_setlogLevel", "Invalid log level");
						}

						request.setInt("log_level", loglevel);
						response = RtTradingServerManager.instance().getTradingServer(serverName).issueCommand(request);

				} catch (Throwable e) {
						localExceptionCommandProcessing(serverName, e);
				}

				// OK
				finalizeCommandProcessing(serverName, request, response);
		}

		// set evenst types that server is going to send
		public Map<Integer, Boolean> remote_getAllowedEvents(String serverName) throws Exception {
				TreeMap<Integer, Boolean> eventMap = new TreeMap<Integer, Boolean>();

				XmlParameter request = null;
				XmlParameter response = null;

				try {
						request = new XmlParameter();
						request.setCommandName("get_allowed_events");

						response = RtTradingServerManager.instance().getTradingServer(serverName).issueCommand(request);

				} catch (Throwable e) {
						localExceptionCommandProcessing(serverName, e);
				}

				// OK
				finalizeCommandProcessing(serverName, request, response);

				List<XmlParameter> eventslist = response.getXmlParameterList("allowed_events");
				Set<Integer> available = XmlEvent.getAllAvalableEventsForServer();

				// initialize with false allowence
				for (Iterator<Integer> it = available.iterator(); it.hasNext();) {
						eventMap.put(it.next(), false);
				}

				for (int i = 0; i < eventslist.size(); i++) {
						XmlParameter par_i = eventslist.get(i);

			//boolean isAllowed = false;
						int event_type = (int) par_i.getInt("event_type");
						boolean event_allowed = (par_i.getInt("is_event_allowed") == 0 ? false : true);

						if (available.contains(event_type)) {
								eventMap.put(event_type, event_allowed);
						}
				}

				return eventMap;
		}

		public void remote_setAllowedEvents(String serverName, Map<Integer, Boolean> eventMap) throws Exception {
				XmlParameter request = null;
				XmlParameter response = null;

				try {
						request = new XmlParameter();
						request.setCommandName("set_allowed_events");

						if (eventMap == null) {
								throw new HtException(getContext(), "remote_setAllowedEvents", "Invalid events map");
						}

						Vector<XmlParameter> eventslist = new Vector<XmlParameter>();
						Set<Integer> available = XmlEvent.getAllAvalableEventsForServer();

						for (Iterator<Integer> it = available.iterator(); it.hasNext();) {
								int event_type = it.next().intValue();
								boolean isAllowed = false;

								// allowed only waht we set
								if (eventMap.containsKey(event_type)) {
										if (eventMap.get(event_type).booleanValue()) {
												isAllowed = true;
										}
								}

								XmlParameter event_i = new XmlParameter();
								event_i.setInt("event_type", event_type);
								event_i.setInt("is_event_allowed", isAllowed ? 1 : 0);

								eventslist.add(event_i);

						}

						request.setXmlParameterList("allowed_events", eventslist);

						response = RtTradingServerManager.instance().getTradingServer(serverName).issueCommand(request);
				} catch (Throwable e) {
						localExceptionCommandProcessing(serverName, e);
				}

				// OK
				finalizeCommandProcessing(serverName, request, response);
		}

		public void remote_getMemoryCachedSymbolDetails(
			int tId,
			String serverName,
			String symbol,
			String provider,
			int aggrPeriod,
			int multFactor,
			LongParam cookie,
			LongParam beginDate,
			LongParam endDate) throws Exception {
				XmlParameter request = null;
				XmlParameter response = null;

				try {
						request = new XmlParameter();
						request.setCommandName("get_cached_symbol_detail_in_memory");

						if (cookie == null) {
								throw new HtException(getContext(), "remote_getMemoryCachedSymbolDetails", "Invalid cookie parameter");
						}

						if (beginDate == null) {
								throw new HtException(getContext(), "remote_getMemoryCachedSymbolDetails", "Invalid begin date parameter");
						}

						if (endDate == null) {
								throw new HtException(getContext(), "remote_getMemoryCachedSymbolDetails", "Invalid end date parameter");
						}

						if (!TSAggregationPeriod.isValid(aggrPeriod)) {
								throw new HtException(getContext(), "remote_getMemoryCachedSymbolDetails", "aggregation period is invalid");
						}

						request.setInt("algorithm_thread_id", tId);
						request.setString("provider", provider);
						request.setString("symbol", symbol);
						request.setInt("aggr_period", aggrPeriod);
						request.setInt("mult_factor", multFactor);

						response = RtTradingServerManager.instance().getTradingServer(serverName).issueCommand(request);

				} catch (Throwable e) {
						localExceptionCommandProcessing(serverName, e);
				}

				// OK
				finalizeCommandProcessing(serverName, request, response);

		//
				//parse
				cookie.setLong(response.getInt("cookie"));
				beginDate.setLong(response.getDate("begin_date"));
				endDate.setLong(response.getDate("end_date"));

		}

		public void remote_getSimulationTicksGeneratedSymbols(
			String serverName,
			Uid ticks_uid,
			Map<String, Map<String, HtPair<Long, Long>>> cachedSymbols,
			LongParam min_total_time,
			LongParam max_total_time,
			LongParam create_time,
			HtSimulationProfileProp.HtSimulationProfileBaseParams baseSimParams) throws Exception {
				XmlParameter request = null;
				XmlParameter response = null;

				try {
						request = new XmlParameter();
						request.setCommandName("get_generated_simulation_ticks_details");

						// if not valid expected RT otherwise - simulation
						request.setUid("ticks_uid", ticks_uid);

						cachedSymbols.clear();
						response = RtTradingServerManager.instance().getTradingServer(serverName).issueCommand(request);

				} catch (Throwable e) {
						localExceptionCommandProcessing(serverName, e);
				}

				// OK
				finalizeCommandProcessing(serverName, request, response);

				// parse request - all is contained here
				XmlParameter slist_param = response.getXmlParameter("result");
				long d = slist_param.getDate("low_time");
				d = (d < 0 ? -1 : d * 1000);
				min_total_time.setLong(d);

				d = slist_param.getDate("high_time");
				d = (d < 0 ? -1 : d * 1000);
				max_total_time.setLong(d);

				d = slist_param.getDate("created_on");
				d = (d < 0 ? -1 : d * 1000);
				create_time.setLong(d);

				baseSimParams.aggrPeriod_m = (int) slist_param.getInt("aggr_period");
				baseSimParams.multFactor_m = (int) slist_param.getInt("mult_factor");
				baseSimParams.genTickMethod_m = (int) slist_param.getInt("gen_tick_method");
				baseSimParams.tickSplitNumber_m = (int) slist_param.getInt("ticks_split_number");
				baseSimParams.tickUsage_m = (int) slist_param.getInt("tick_usage");

				// list
				List<XmlParameter> symbols = slist_param.getXmlParameterList("symbols_list");

				for (int i = 0; i < symbols.size(); i++) {
						XmlParameter par_i = symbols.get(i);

						String provider = par_i.getString("provider");
						String symbol = par_i.getString("symbol");

						long begdate = par_i.getDate("begin_time");
						begdate = (begdate < 0 ? -1 : begdate * 1000);

						long enddate = par_i.getDate("end_time");
						enddate = (enddate < 0 ? -1 : enddate * 1000);

						HtPair<Long, Long> begEndTime = new HtPair<Long, Long>(new Long(begdate), new Long(enddate));

						Map<String, HtPair<Long, Long>> onlySymbols = null;
						if (cachedSymbols.containsKey(provider)) {
								onlySymbols = cachedSymbols.get(provider);
						} else {
								onlySymbols = new TreeMap<String, HtPair<Long, Long>>();
								cachedSymbols.put(provider, onlySymbols);
						}

						onlySymbols.put(symbol, begEndTime);

				}

		// some profile values
		}

		public void remote_getHistoryContextCachedSymbols(
			String serverName,
			Uid simulationUid,
			Map<String, Map<String, HtPair<Long, Long>>> cachedSymbols,
			LongParam min_total_time,
			LongParam max_total_time) throws Exception {
				XmlParameter request = null;
				XmlParameter response = null;

				try {
						request = new XmlParameter();
						request.setCommandName("get_cached_symbols");

						// if not valid expected RT otherwise - simulation
						request.setUid("simulation_uid", simulationUid);

						if (cachedSymbols == null) {
								throw new HtException(getContext(), "remote_getCachedSymbols", "Invalid cached symbols map");
						}

						cachedSymbols.clear();
						response = RtTradingServerManager.instance().getTradingServer(serverName).issueCommand(request);

				} catch (Throwable e) {
						localExceptionCommandProcessing(serverName, e);
				}

				// OK
				finalizeCommandProcessing(serverName, request, response);

		// parse request
				List<XmlParameter> symbols = response.getXmlParameterList("symbols");
				min_total_time.setLong(response.getDate("min_total_time"));
				max_total_time.setLong(response.getDate("max_total_time"));

				if (symbols == null) {
						return;
				}

				for (int i = 0; i < symbols.size(); i++) {
						XmlParameter par_i = symbols.get(i);

						String provider = par_i.getString("provider");
						String symbol = par_i.getString("symbol");

						long begdate = par_i.getDate("begin_time");
						long enddate = par_i.getDate("end_time");

						HtPair<Long, Long> begEndTime = new HtPair<Long, Long>(new Long(begdate), new Long(enddate));

						Map<String, HtPair<Long, Long>> onlySymbols = null;
						if (cachedSymbols.containsKey(provider)) {
								onlySymbols = cachedSymbols.get(provider);
						} else {
								onlySymbols = new TreeMap<String, HtPair<Long, Long>>();
								cachedSymbols.put(provider, onlySymbols);
						}

						onlySymbols.put(symbol, begEndTime);

				}
		}

		@Override
		public String getProxyName() {
				return "server_proxy";
		}

		/**
		 * helpers
		 */
		private void localExceptionCommandProcessing(String serverName, Throwable e) throws Exception {
				String msg = e.toString();
				if (HtUtils.nvl(msg))
						msg = "N/A";
				RtTradingServerManager.instance().pushLocalServerEvent(serverName, HtServerProxy.SRV_COMMAND_LOCAL_EXCEPTION, msg);
				throw new HtException(getContext(), "localExceptionCommandProcessing", e);
		}

		private void finalizeCommandProcessing(String serverName, XmlParameter request, XmlParameter response) throws Exception {
				if (response == null) {
						throw new HtException(getContext(), "finalizeCommandProcessing", "No valid response returned from the server: " + serverName);
				}

				RtTradingServerManager.instance().pushRemoteServerEvent(serverName, request, response, xmlCommandThreadHandler_m);

				if (response.getInt("status_code") != 0) {
						throw new HtException(getContext(), "finalizeCommandProcessing", "Server: \"" + serverName + "\" responded with exception: \"" + HtUtils.getResponseError(response) + "\", status message: \"" + response.getString("status") + "\"");
				}

		}
}
