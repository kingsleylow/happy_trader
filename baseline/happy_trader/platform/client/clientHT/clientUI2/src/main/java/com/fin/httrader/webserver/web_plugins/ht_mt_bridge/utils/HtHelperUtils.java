/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.webserver.web_plugins.ht_mt_bridge.utils;

import com.fin.httrader.configprops.HtAlgBrkPairProp;
import com.fin.httrader.configprops.HtEventPluginProp;
import com.fin.httrader.configprops.HtServerProp;
import com.fin.httrader.configprops.helper.LoadedDllsHelper;
import com.fin.httrader.eventplugins.ht_mt_bridge.HtMtBridgeEventProcessor;
import com.fin.httrader.eventplugins.ht_mt_bridge.HtMtManager;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibresponse.AlgLibResponseBase;
import com.fin.httrader.hlpstruct.ThreadDescriptor;
import com.fin.httrader.managers.RtAlertPluginManager;
import static com.fin.httrader.managers.RtAlertPluginManager.PLUGIN_FOUND_JAR;
import static com.fin.httrader.managers.RtAlertPluginManager.PLUGIN_FOUND_NATIVE;
import com.fin.httrader.managers.RtConfigurationManager;
import com.fin.httrader.managers.RtTradingServerManager;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.model.HtServerProxy;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.IntParam;
import com.fin.httrader.utils.hlpstruct.HtPair;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.HtMtConstants;
import java.io.File;
import java.io.FileInputStream;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.xpath.XPath;
import javax.xml.xpath.XPathFactory;
import org.w3c.dom.Document;

/**
 *
 * @author Victor_Zoubok
 */
public class HtHelperUtils {
		
		private static String getContext()
		{
				return HtHelperUtils.class.getSimpleName();
		}
		
		public static void checkEventPluginStarted(
			String eventPlugin, 
			String srvName_resolved, 
			String algLibPair_resolved, 
			int tid,
			Map<String, String> additionalParameters
		) throws Exception
		{
				Set<String> event_plugins = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRegisteredEventPluginList();
				
				String eventPluginResolved_resolved = HtUtils.searchCollectionIgnoreCase(event_plugins, eventPlugin);
				if (eventPluginResolved_resolved == null)
						throw new HtException(getContext(), "checkServerIsStarted", "Cannot find event plugin entry: " + eventPlugin);
				
				// check if running
				if (!HtCommandProcessor.instance().get_EventPluginProxy().remote_queryPluginRunning(eventPluginResolved_resolved)) {
						// start
						HtEventPluginProp plugProp = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getEventPluginProperty(eventPluginResolved_resolved);
						
						plugProp.getParameters().put(HtMtBridgeEventProcessor.SERVERNAME_PARAM, srvName_resolved);
						plugProp.getParameters().put(HtMtBridgeEventProcessor.ALGLIB_PARAM, algLibPair_resolved);
						plugProp.getParameters().put(HtMtBridgeEventProcessor.TREADID_PARAM, String.valueOf(tid));
						plugProp.getParameters().putAll(additionalParameters);
						
						// save one more time
						HtCommandProcessor.instance().get_HtConfigurationProxy().remote_setEventPluginProperty(eventPluginResolved_resolved, plugProp);
						
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
				
				
		}
		
		public static void checkEventPluginStopped(String pluginName) throws Exception
		{
				//String pluginName = HtMtManager.getInstance().getEventPluginName();
				Set<String> event_plugins = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRegisteredEventPluginList();
				
				String eventPluginResolved_resolved = HtUtils.searchCollectionIgnoreCase(event_plugins, pluginName);
				if (eventPluginResolved_resolved == null)
						throw new HtException(getContext(), "checkServerIsStarted", "Cannot find event plugin entry: " + pluginName);

				if (HtCommandProcessor.instance().get_EventPluginProxy().remote_queryPluginRunning(pluginName))
						// stop plugin
						HtCommandProcessor.instance().get_EventPluginProxy().remote_stopPlugin(pluginName);
		}
		
		
		public static String checkEventPluginCreated(String pluginName, boolean createForce) throws Exception
		{
				// check if even plugin entry is created and creat this if necessary
				Set<String> event_plugins = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRegisteredEventPluginList();
				String eventPluginResolved_resolved = HtUtils.searchCollectionIgnoreCase(event_plugins, pluginName);
				if (eventPluginResolved_resolved == null || createForce) {
						
						// need to create
						HtEventPluginProp prop = new HtEventPluginProp();
						
						String cname = HtMtBridgeEventProcessor.class.getCanonicalName();
						if (HtUtils.nvl(cname))
								throw new HtException(getContext(),"checkEventPluginCreated", "Cannot resolve the name of: HtMtBridgeEventProcessor");
						prop.setPluginClass(cname);
						
						
						// check launch method
						StringBuilder jarPath = new StringBuilder();
						IntParam foundFlag = new IntParam();
						
						List< HtPair<String, File> > providers = HtCommandProcessor.instance().get_EventPluginProxy().remote_queryAvailableProviders();
						HtCommandProcessor.instance().get_EventPluginProxy().remote_checkWherePluginFound(providers, prop.getPluginClass(), jarPath, foundFlag );
						
						// first internal
						if ((foundFlag.getInt() & RtAlertPluginManager.PLUGIN_FOUND_NATIVE) > 0) {
										prop.setLaunchFlag(HtEventPluginProp.LAUNCH_INTERNAL);
						}
						
						// then - jar
						if (prop.getLaunchFlag() == HtEventPluginProp.LAUNCH_DUMMY) {
								// first run from jar
								if ((foundFlag.getInt() & RtAlertPluginManager.PLUGIN_FOUND_JAR) > 0) {
										prop.setLaunchFlag(HtEventPluginProp.LAUNCH_EXT_JAR);
								}
						}
						
						
						
						HtCommandProcessor.instance().get_HtConfigurationProxy().remote_setEventPluginProperty(pluginName, prop);
				}
				
				return pluginName;
				
		}
		
		
		// ------------------------------------------------
		
		
		public static void stopServer(String serverName) throws Exception
		{
				//if (!HtMtManager.getInstance().isInitialized())
				//		throw new HtException(getContext(), "stopServer", "Not initialized");
				
						
				Set<String> servers = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getTradingServerList();
				String serverId_resolved = HtUtils.searchCollectionIgnoreCase(servers, serverName);
				
				if (serverId_resolved == null)
						throw new HtException(getContext(), "stopServer", "Cannot find server entry: " + serverName);
				
				int status = HtCommandProcessor.instance().get_HtServerProxy().remote_returnServerStatus(serverId_resolved);
				if (status == RtTradingServerManager.SERVER_RUN_OK) {
						// stop server
						HtCommandProcessor.instance().get_HtServerProxy().remote_shutDownServerProcess(serverId_resolved);
				}
				else if (status == RtTradingServerManager.SERVER_RTTHREAD_ERROR) {
						// stop server
						HtCommandProcessor.instance().get_HtServerProxy().remote_killServerProcess(serverId_resolved);
				}
				
										
		}
		
		
	
		// retursn resolved
		public static void checkServerIsStarted(String serverId) throws Exception
		{
			
				Set<String> servers = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getTradingServerList();
				
				String serverId_resolved = HtUtils.searchCollectionIgnoreCase(servers, serverId);
				if (serverId_resolved == null)
						throw new HtException(getContext(), "checkServerIsStarted", "Cannot find server entry: " + serverId);
				
				// check if started
				
				int status = HtCommandProcessor.instance().get_HtServerProxy().remote_returnServerStatus(serverId_resolved);
			
				if (status != RtTradingServerManager.SERVER_RUN_OK) {
					//try to start
					startServer(serverId_resolved, null);
					status = HtCommandProcessor.instance().get_HtServerProxy().remote_returnServerStatus(serverId_resolved);
					
					if (status == RtTradingServerManager.SERVER_RUN_OK) 
							return;
								
				}
				
				
				throw new HtException(getContext(), "checkServerIsStarted", "Cannot start server: " + serverId_resolved);
			
		}
		
	
		
		public static String checkSrvCreated(
			String srvName,
			String srvNameBaseExe,
			String settingsFile,
			boolean is_debug,
			boolean createForce
			
		) throws Exception
		{
				String srvName_full = srvName;
				if (is_debug) {
						srvName_full = srvName + RtConfigurationManager.StartUpConst.DEBUG_POSTFIX;
				}
				
				Set<String> servers = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getTradingServerList();
				
				String serverId_resolved = HtUtils.searchCollectionIgnoreCase(servers, srvName_full);
				if (serverId_resolved == null || createForce) {
						
						// create entity
						// resolve executable
						Set<File> executables = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_findAllSrvExecutables();
						File f = resolveLibrary(executables, srvNameBaseExe, is_debug); 
						if (f == null) 
								throw new HtException(getContext(), "checkAlgLibPairCreated", "Server executable: " + srvNameBaseExe+ " not found, is debug: " +is_debug );
								
						HtServerProp prop = new HtServerProp();
						
						prop.setServerExecutable(f.getAbsolutePath());
						prop.setUseSpawner(false);
						
						// read some settings
						parseSettingFile(prop, settingsFile);
						
						if (!prop.isValid())
								throw new HtException(getContext(), "checkAlgLibPairCreated", "Server properties not valid");
						
						
						// create
						HtCommandProcessor.instance().get_HtConfigurationProxy().remote_setTradingServerProperty(srvName_full, prop);
						serverId_resolved = HtUtils.searchCollectionIgnoreCase(HtCommandProcessor.instance().get_HtConfigurationProxy().
								remote_getTradingServerList(), srvName_full);
				}
				
				return serverId_resolved;
				
				
		}
		
		
		// --------------------------------------------
		
		
		// returns thread ID
		public static int loadAndResolveAlgorithBrokerThread(String serverId, String algLibPair) throws Exception
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
		
		
		
		public static String checkAlgLibPairCreated(
			String algLibPair, 
			String algLibPairBaseDll, 
			boolean is_debug,
			String settingsFile,
			boolean createForce
		)throws Exception
		{
				String algLibPair_full = algLibPair;
				if (is_debug) {
						algLibPair_full = algLibPair + RtConfigurationManager.StartUpConst.DEBUG_POSTFIX;
				}
				
				String algLibPair_resolved = HtUtils.searchCollectionIgnoreCase(HtCommandProcessor.instance().get_HtConfigurationProxy().
						remote_getRegisteredAlgBrkPairList(), algLibPair_full);
				
				if (algLibPair_resolved ==null || createForce){
						
						HtAlgBrkPairProp prop = new HtAlgBrkPairProp();
						
						//int loadedLibs = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getDllDescriptor().getDetectedLibraries();
						
						
						// need to resolve paths dll
						Set<File> alglibs = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_findAllAlgorithmDlls();
						File f = resolveLibrary(alglibs, algLibPairBaseDll, is_debug); 
						if (f == null) 
								throw new HtException(getContext(), "checkAlgLibPairCreated", "Library: " + algLibPairBaseDll+ " not found, is debug: " +is_debug );
												
					
						prop.setAlgorithmPath(f.getAbsolutePath());
						
						File sf = new File(settingsFile);
						if (!sf.exists())
								throw new HtException(getContext(), "checkAlgLibPairCreated", "Settings file does not exist: " + sf.getAbsolutePath());
						
						prop.getAlgoritmParams().put(HtMtConstants.ALGLIB_DLL_PARAM_SETTINGS_FILE, sf.getAbsolutePath());
						
						HtCommandProcessor.instance().get_HtConfigurationProxy().remote_setAlgBrkPairProperty(algLibPair_full, prop);
						
							
						algLibPair_resolved = HtUtils.searchCollectionIgnoreCase(HtCommandProcessor.instance().get_HtConfigurationProxy().
								remote_getRegisteredAlgBrkPairList(), algLibPair_full);
				};
		
				return algLibPair_resolved;
		}
		
		// returns actula alg brk pair namee
		public static void loadAndResolveAlgLibPair(String serverId, String algLibPair) throws Exception
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
			
		}
		
		public static void loadAlgBrkPair(String serverId, String algLibPair) throws Exception
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
		
	// --------------------------------
		
		
	public static void checkAlgLibResponse(AlgLibResponseBase base) throws Exception
	{
			
				
				if (base.getMt_result_message() == null || base.getResult_message()==null )
						throw new HtException(getContext(), "checkAlgLibResponse", "Some obigatory fields are missing in the response body");
				
				if (base.getResult_code() !=0)
						throw new HtException(getContext(), "checkAlgLibResponse", "Upper layer returned: [" + base.getResult_message() + "] [" + base.getResult_code() + "]");
				
				if (base.getMt_result_code() !=0)
						throw new HtException(getContext(), "checkAlgLibResponse", "MT layer returned: [" + base.getMt_result_message() + "] [" + base.getMt_result_code() + "]");
				
				
			
		}
	
		/*
		* Helpers
		*/
	
		private static File resolveLibrary(Set<File> source, String baseName, boolean is_debug) throws Exception
		{
				// returns 2 files  - debug and release 
				String baseName_l = baseName.toLowerCase();
				
				
				for (File f_i : source) {
						String lc_i = f_i.getAbsolutePath().toLowerCase();
						if (lc_i.indexOf(baseName_l) != -1) {
								boolean debugPostfixFound = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_isDebugVersion(lc_i);
								if (is_debug) {
										if (debugPostfixFound)
												return f_i;
								}
								else {
										if (!debugPostfixFound)
												return f_i;
								}
								
						}
				}
				
				return null;
		}
		
		// if server not started - starting this
		private static void startServer(String serverId, List<String> rtProvidersToSubscribe) throws Exception {
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
		
	private static void parseSettingFile(HtServerProp prop, String settingFile) throws Exception 
	{
			File f = new File (settingFile);
			if (!f.exists())
					throw new HtException(getContext(), "parseSettingFile", "Settings file not found: " + settingFile);
			
			DocumentBuilderFactory builderFactory =
        DocumentBuilderFactory.newInstance();
				DocumentBuilder builder = null;
				try {
						
						
						builder = builderFactory.newDocumentBuilder();
						Document document = builder.parse( new FileInputStream(f));
						
						XPath xPath =  XPathFactory.newInstance().newXPath();
						
						String port_s = xPath.compile("/settings/generalparams/htserver/port").evaluate(document);
						String internalloglevel_s = xPath.compile("/settings/generalparams/htserver/internalloglevel").evaluate(document);
						String eventlevel_s = xPath.compile("/settings/generalparams/htserver/eventlevel").evaluate(document);
						
						prop.setDebugLevel(HtUtils.parseInt(internalloglevel_s));
						prop.setLogEventLevel(HtUtils.parseInt(	 eventlevel_s ));
						prop.setCommandPort(HtUtils.parseInt(port_s));
						
						
						
				} catch (ParserConfigurationException e) {
						throw new HtException(getContext(), "parseSettingFile", "XML parsing exception: " + e.getMessage());
				}
	}
		
}
