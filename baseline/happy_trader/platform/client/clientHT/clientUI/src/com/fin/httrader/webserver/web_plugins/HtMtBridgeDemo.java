/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.webserver.web_plugins;

import com.fin.httrader.configprops.HtAlgBrkPairProp;
import com.fin.httrader.configprops.HtEventPluginProp;
import com.fin.httrader.configprops.HtServerProp;
import com.fin.httrader.eventplugins.ht_mt_bridge.HtMtBridgeEventProcessor;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibrequest.Req_G_GET_ALL_METADATA;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibresponse.AlgLibResponseBase;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibresponse.Resp_G_GET_ALL_METADATA;
import com.fin.httrader.hlpstruct.ThreadDescriptor;
import com.fin.httrader.managers.RtConfigurationManager;
import com.fin.httrader.managers.RtSecurityManager;
import com.fin.httrader.managers.RtTradingServerManager;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.model.HtServerProxy;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtMathUtils;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.IntParam;
import com.fin.httrader.utils.LongParam;
import com.fin.httrader.utils.hlpstruct.HtPair;
import com.fin.httrader.webserver.HtServletsBase;
import com.fin.httrader.webserver.HtWebUtils;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import javax.servlet.ServletException;
import javax.servlet.http.Cookie;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.JSONValue;

/**
 *
 * @author Victor_Zoubok
 */
public class HtMtBridgeDemo extends HtServletsBase {

		private final static String SRV_NAME_M = "srv_mt_manager";
		private final static String ALGLIB_NAME_M = "a_mt_controller";
		private final static String EVENT_PLUGIN_M = "ht_mt_bridge";
		

		protected String getContext() {
				return HtMtBridgeDemo.class.getName();
		}
		
		@Override
		protected boolean initialize_Get(HttpServletRequest req,
			HttpServletResponse res) throws ServletException, IOException {
				res.setDateHeader("Expires", 1);
				res.setContentType("text/html; charset=UTF-8");

				try {
						this.generateUniquePageId();
						StringBuilder eventPlugin_o = new StringBuilder();
						StringBuilder srvName_o = new StringBuilder();
						StringBuilder algLibPair_o = new StringBuilder();
						IntParam tid_o = new IntParam();
						
						boolean is_debug_ver = this.getBooleanParameter(req, "debug", true);
						
						getInitialData(eventPlugin_o, srvName_o,  algLibPair_o, tid_o, is_debug_ver);
						
						// save
						setStringSessionValue(getUniquePageId(), req, "event_plugin", srvName_o.toString());
						setStringSessionValue(getUniquePageId(), req, "server_name", srvName_o.toString());
						setStringSessionValue(getUniquePageId(), req, "alg_lib_pair", algLibPair_o.toString());
						setIntegerSessionValue(getUniquePageId(), req, "tid", tid_o.getInt());
						
						
						List<HtPair<String, Boolean>> instances = getAllMetadata(srvName_o.toString(), algLibPair_o.toString(), tid_o.getInt());
						
						StringBuilder selectText = new StringBuilder();
						for(int i = 0; i < instances.size(); i++) {
								HtPair<String, Boolean> p  =instances.get(i);
								
								selectText.append("<option value='").append(p.first).append("' >");
								if (p.second) 
										selectText.append(p.first).append(" - CONNECTED");
								else
										selectText.append(p.first).append(" - NOT CONNECTED");
								
								selectText.append("</option>");
								
						}
						
						setStringSessionValue(getUniquePageId(), req, "mt_instances", selectText.toString());

				} catch (Throwable e) {
						writeHtmlErrorToOutput(res, e);
						return false;
				}

				return true;
		}

		@Override
		protected boolean initialize_Post(HttpServletRequest req,
			HttpServletResponse res) throws ServletException, IOException
		{
				res.setDateHeader("Expires", 1);
				res.setContentType("text/html; charset=UTF-8");

				try {
						this.generateUniquePageId();
						
						

				} catch (Throwable e) {
						writeHtmlErrorToOutput(res, e);
						return false;
				}

				return true;
		}
		
		/**
		 * Helpers
		 */
		
		void getInitialData(StringBuilder eventPlugin_o, StringBuilder srvName_o,  StringBuilder algLibPair_o, IntParam tid_o, boolean is_debug) throws Exception
		{
				// server
				String srv_actual_name = SRV_NAME_M;
				String alg_lib_actual_name = ALGLIB_NAME_M;
				
				if (is_debug) {
						srv_actual_name = srv_actual_name + "_d";
						alg_lib_actual_name = alg_lib_actual_name + "_d";
				}
				
				
				String srvName_resolved = checkServerIsStarted(srv_actual_name);
				String algLibPair_resolved = loadAndResolveAlgLibPair(srvName_resolved, alg_lib_actual_name);
				int tid = loadAndResolveAlgorithBrokerThread	(srvName_resolved, algLibPair_resolved);
				String eventPlugin_resolved = checkEventPluginStarted(EVENT_PLUGIN_M, srvName_resolved, algLibPair_resolved, tid);
				
					
				tid_o.setInt(tid);
				
				srvName_o.setLength(0);
				srvName_o.append(srvName_resolved);
						
				algLibPair_o.setLength(0);
				algLibPair_o.append(algLibPair_resolved);
				
				eventPlugin_o.setLength(0);
				eventPlugin_o.append( eventPlugin_resolved );
				
		}
		
		private String checkEventPluginStarted(String eventPlugin, String srvName_resolved, String algLibPair_resolved, int tid) throws Exception
		{
				Set<String> events = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRegisteredEventPluginList();
				
				String eventPluginResolved_resolved = HtUtils.searchCollectionIgnoreCase(events, eventPlugin);
				if (eventPluginResolved_resolved == null)
						throw new HtException(getContext(), "checkServerIsStarted", "Cannot find server entry: " + eventPlugin);
				
				// check if running
				if (!HtCommandProcessor.instance().get_EventPluginProxy().remote_queryPluginRunning(eventPluginResolved_resolved)) {
						// start
						HtEventPluginProp plugProp = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getEventPluginProperty(eventPluginResolved_resolved);
						
						plugProp.getParameters().put(HtMtBridgeEventProcessor.SERVERNAME_PARAM, srvName_resolved);
						plugProp.getParameters().put(HtMtBridgeEventProcessor.ALGLIB_PARAM, algLibPair_resolved);
						plugProp.getParameters().put(HtMtBridgeEventProcessor.TREADID_PARAM, String.valueOf(tid));
						
						
						HtCommandProcessor.instance().get_EventPluginProxy().remote_startPlugin(
									eventPluginResolved_resolved,
									plugProp.getPluginClass(),
									plugProp.getLaunchFlag(),
									plugProp.getParameters()
						);
						
						
						// check again
						if (!HtCommandProcessor.instance().get_EventPluginProxy().remote_queryPluginRunning(eventPluginResolved_resolved)) 
								throw new HtException(getContext(), "checkEventPluginStarted", "Cannot start plugin: " + eventPluginResolved_resolved);
						
				
				}
				
				return eventPluginResolved_resolved;
				
		}
	
		// retursn resolved
		private String checkServerIsStarted(String serverId) throws Exception
		{
			
				Set<String> servers = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getTradingServerList();
				
				String serverId_resolved = HtUtils.searchCollectionIgnoreCase(servers, serverId);
				if (serverId_resolved == null)
						throw new HtException(getContext(), "checkServerIsStarted", "Cannot find server entry: " + serverId);
				
				// check if started
				
				int status = HtCommandProcessor.instance().get_HtServerProxy().remote_returnServerStatus(serverId_resolved);
				if (status == RtTradingServerManager.SERVER_RUN_OK) 
						return serverId_resolved;
				else {
					//try to start
					startServer(serverId_resolved, null);
					status = HtCommandProcessor.instance().get_HtServerProxy().remote_returnServerStatus(serverId_resolved);
					
					if (status == RtTradingServerManager.SERVER_RUN_OK) 
							return serverId_resolved;
								
				}
				
				
				throw new HtException(getContext(), "checkServerIsStarted", "Cannot start server: " + serverId_resolved);
			
		}
		
		// returns thread ID
		private int loadAndResolveAlgorithBrokerThread(String serverId, String algLibPair) throws Exception
		{
				
				List<ThreadDescriptor> loadedThreads = HtCommandProcessor.instance().get_HtServerProxy().remote_requestStartedThreads2(serverId);
					
				int tid = -1;
				for(int i = 0; i < loadedThreads.size(); i++) {
							ThreadDescriptor th = loadedThreads.get(i);
														
							Set<String> algLibpairs = th.loadedAlgBrkPairs_m;
							String algLibPair_resolved = HtUtils.searchCollectionIgnoreCase(algLibpairs, algLibPair);
							
							if (algLibPair_resolved != null) {
									tid=th.tid_m;
									break;
							}
							
				}
				
				if (tid ==-1) {
						List<String> algLibPairs = new ArrayList<String>();
						algLibPairs.add(algLibPair);
						
						tid = HtCommandProcessor.instance().get_HtServerProxy().remote_startAlgorithmThread(serverId, algLibPairs);
						if (tid==-1)
								throw new HtException(getContext(), "loadAndResolveAlgorithBrokerThread", "Cannot start thread for alg lib pair: " + algLibPair);
				}
					
				return tid;
		}
		
		// returns actula alg brk pair namee
		private String loadAndResolveAlgLibPair(String serverId, String algLibPair) throws Exception
		{
				
				String algBrkPair_resolved = HtUtils.searchCollectionIgnoreCase(HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRegisteredAlgBrkPairList(), algLibPair);
				if (algBrkPair_resolved ==null)
						throw new HtException(getContext(), "loadAndResolveAlgLibPair", "Alg Lib Pair cannot be resolbed: " + algLibPair);
				
				// cehck if loaded
				
				boolean loaded =  HtCommandProcessor.instance().get_HtServerProxy().remote_isAlgorithmLoaded(serverId, algBrkPair_resolved);
				if (!loaded) {
						// try to load
						loadAlgBrkPair(serverId, algBrkPair_resolved);
						
						loaded =  HtCommandProcessor.instance().get_HtServerProxy().remote_isAlgorithmLoaded(serverId, algBrkPair_resolved);
						if (!loaded)
								throw new HtException(getContext(), "loadAndResolveAlgLibPair", "Alg Lib Pair cannot be resolved: " + algBrkPair_resolved);
						
				}
				
				return algBrkPair_resolved;
		}
		
		private void loadAlgBrkPair(String serverId, String algLibPair) throws Exception
		{
				
				HtAlgBrkPairProp algBrkProp = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getAlgBrkPairProperty(algLibPair);


				// check if alg brk pairs are not loaded
				// check if it is loaded
				Map<String, Integer> loadedAlgLibPairs = HtCommandProcessor.instance().get_HtServerProxy().remote_requestLoadedAlgorithmBrokerPairs(serverId, -1);
				if (!loadedAlgLibPairs.containsKey(algLibPair)) {
					// it is not loaded - load it

					HtCommandProcessor.instance().get_HtServerProxy().remote_loadAlgorithmBrokerPair(
									serverId,
									"[auto load]",
									"",
									algLibPair,
									algBrkProp.getAlgorithmPath(),
									algBrkProp.getBrokerPath(),
									algBrkProp.getBrokerPath2(),
									algBrkProp.getAlgoritmParams(),
									algBrkProp.getBrokerParams(),
									algBrkProp.getBrokerParams2(),
									1);
				}
				
				// expected loaded

		}
		
		// if server not started - starting this
		private void startServer(String serverId, List<String> rtProvidersToSubscribe) throws Exception {
				HtServerProp srvProp_i = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getTradingServerProperty(serverId);


				Set<String> rtProvidersToSubscribeSet = new HashSet<String>();
				if (rtProvidersToSubscribe != null)
						rtProvidersToSubscribeSet.addAll(rtProvidersToSubscribe);

				Map<String, HtServerProxy.HtServerProxy_ProfileEntry> profProvSymbMap = new HashMap<String, HtServerProxy.HtServerProxy_ProfileEntry>();


				HtCommandProcessor.instance().get_HtServerProxy().remote_startServerProcessWithHistoryData(
								serverId,
								srvProp_i.getServerExecutable(),
								srvProp_i.getDebuglevel(),
								srvProp_i.getServerHost(),
								srvProp_i.getCommandPort(),
								RtConfigurationManager.StartUpConst.COMMON_SERVICE_LISTEN_PORT,
								srvProp_i.getSpawnerPort(),
								srvProp_i.isUseSpawner(),
								true,
								-1,
								-1,
								profProvSymbMap,
								rtProvidersToSubscribeSet,
								false,
								srvProp_i.getLogEventLevel(),
								srvProp_i.getSubscribedEvents(),
								false);
	}
		
		List<HtPair<String, Boolean>> getAllMetadata(String serverName, String algLibPair, int tid) throws Exception
		{
			
				List<HtPair<String, Boolean>>  result = new ArrayList<HtPair<String, Boolean>> ();
				
				Req_G_GET_ALL_METADATA req = new Req_G_GET_ALL_METADATA();
				
				JSONObject json_obj = new JSONObject();
				req.toJson(json_obj);
				
				String custom_data = JSONValue.toJSONString(json_obj);
				
				
				// issue get metadata command
				String result_data = HtCommandProcessor.instance().get_HtServerProxy().remote_SendCustomEvent_toAlgorithm(
								serverName,
								algLibPair,
								tid,
								"G_GET_ALL_METADATA",
								custom_data,
								RtConfigurationManager.StartUpConst.WAIT_RESPONSE_FROM_TRADING_SERVER_SEC
				);
				
				Resp_G_GET_ALL_METADATA resp = new Resp_G_GET_ALL_METADATA();
				JSONObject root_json = (JSONObject)JSONValue.parseWithException(result_data);
				
				resp.fromJson(root_json);
				
				
				// parse result data
				
				checkAlgLibResponse(resp);
				
				
				for(int i = 0; i < resp.getData().size(); i++) {
						Resp_G_GET_ALL_METADATA.MtEntry e = resp.getData().get(i);
						
						Boolean is_connected = e.isConnected();
						String name = e.getName();
						
						HtPair<String, Boolean> p = new HtPair(name,is_connected );
						result.add(p);
						
				}
				
				return result;
				
		}
		
		//parse HT return status and return DATA object
		void checkAlgLibResponse(AlgLibResponseBase base) throws Exception
		{
			
				
				if (base.getMt_result_message() == null || base.getResult_message()==null )
						throw new HtException(getContext(), "initialize_Get", "Some obigatory fields are missing in the response");
				
				if (base.getResult_code() !=0)
						throw new HtException(getContext(), "initialize_Get", "Upper layer returned: [" + base.getResult_message() + "] [" + base.getResult_code() + "]");
				
				if (base.getMt_result_code() !=0)
						throw new HtException(getContext(), "initialize_Get", "MT layer returned: [" + base.getMt_result_message() + "] [" + base.getMt_result_code() + "]");
				
				
			
		}
}
