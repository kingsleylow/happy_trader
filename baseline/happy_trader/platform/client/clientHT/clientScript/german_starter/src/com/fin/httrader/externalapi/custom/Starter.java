/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.externalapi.custom;

import com.fin.httrader.externalapi.utils.HtExtPropertyHolder;
import com.fin.httrader.externalapi.HtExtBaseLogger;
import com.fin.httrader.externalapi.HtExtException;
import com.fin.httrader.externalapi.utils.HtExtIntBuffer;
import com.fin.httrader.externalapi.utils.HtExtBoolBuffer;
import com.fin.httrader.configprops.HtAlgBrkPairProp;
import com.fin.httrader.configprops.HtDataProfileProp;
import com.fin.httrader.configprops.HtEventPluginProp;
import com.fin.httrader.configprops.HtHistoryProviderProp;
import com.fin.httrader.configprops.HtRTProviderProp;
import com.fin.httrader.configprops.HtServerProp;
import com.fin.httrader.externalapi.*;
import com.fin.httrader.hlpstruct.TradeServerLogLevels;
import com.fin.httrader.hlpstruct.externalrunstruct.RunParameter;
import com.fin.httrader.interfaces.HtEventListener;
import com.fin.httrader.managers.RtConfigurationManager;
import com.fin.httrader.managers.RtGlobalEventManager;
import com.fin.httrader.managers.RtTradingServerManager;
import com.fin.httrader.model.HtDatabaseProxy;
import com.fin.httrader.model.HtServerProxy;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtSystem;
import com.fin.httrader.utils.HtTimeCalculator;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.LongParam;
import com.fin.httrader.utils.SingleReaderQueue;
import com.fin.httrader.utils.SingleReaderQueueExceedLimit;
import com.fin.httrader.utils.Uid;
import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.utils.hlpstruct.HtPair;
import com.fin.httrader.utils.hlpstruct.HtRtData;
import com.fin.httrader.utils.hlpstruct.HumanSerializer;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import java.io.File;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TimeZone;

/**
 *
 * @author Victor_Zoubok
 */
public class Starter implements HtExtRunner, HtEventListener {

	private String PAR_SMARTCOM_LOGIN = "";
	private String PAR_SIM_PROFILE = "";
	private String PAR_SMARTCOM_PASSWORD = "";
	private int PAR_SMARTCOM_LOGLEVEL = 2;
	private String PAR_SMARTCOM_HOST = "";
	private String PAR_SMARTCOM_PORT = "";
	private String PAR_SMARTCOM_PORTFOLIO = "";
	private boolean PAR_IS_DEBUG_CONFIGURATION = false;
	private boolean PAR_RECREATE_ENTRIES = false;
	private String PAR_SYMBOL_FILE = "c:\\work\\German_work\\in\\symbols.txt";
	private String PAR_BASE_EXPORT_PATH = "c:\\work\\German_work\\out\\export_smartcom\\";
	private boolean PAR_AUTO_START_TRADE = false;
	private boolean SHORT_SELL = true;
	// -----------------------------
	// constants
	private final String CPP_SERVER_NAME = "server_SMARTCOM";
	private final int CPP_SERVER_PORT = 8001;
	private final String RT_PROVIDER = "smartcom_provider";
	private final String RT_PROVIDER_BAR_POSTFIX = "_bar";
	private final String RT_PROVIDER_BAR = RT_PROVIDER + RT_PROVIDER_BAR_POSTFIX;
	private final String EVENT_PLUGIN_NAME = "init_smartComServer";
	private final String SMARTCOM_ALG_BRK_PAIR = "smartcom_german";
	private final String HISTORY_DATA_PROFILE = "data_prf";
	private final int SMARTCOM_CONNECT_TOUT_SEC = 10;
	private final int SMARTCOM_EXPORTDATA_TOUT_SEC = 100;
	private final int ALG_MSG_TOUT_SEC = 10;
	private final String HST_PROVIDER = "smartcom_history_provider";
	private final int CPP_SERVER_NORMAL_SHUTDOWN_INTERVAL_SEC = 1000;
	// ----------------------------
	private HtExtBaseLogger logger_m = null;
	private HtExtConfiguration configurator_m = new HtExtConfiguration();
	private final SingleReaderQueue eventsArrived_m = new SingleReaderQueue("Wait for SMARTCOM connection event", false, RtConfigurationManager.StartUpConst.MAX_ELEMENTS_IN_QUEUE);
	private int threadId_m = -1;
	private String runName_m = null;
	private String runNameLoad_m = null;
	// --------------------------------

	@Override
	public String getRunName() {
		return "MM_Channel_Starter";
	}
	
	public String getRunNameBase() {
		return getRunName();
	}

	@Override
	public void run(HtExtBaseLogger logger, Map<String, RunParameter> parameters) throws Exception {

		// first create all necessaryy entries and parameters
		logger_m = logger;
		threadId_m = -1;
		runName_m = null;

		try {

			logger.addStringEntry("Subscribing for events from SMARTCOM...");
			RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).subscribeForEvent(XmlEvent.ET_RtProviderSynchronizationEvent, 5001, this);


			initDataProfile();
			readParameters(parameters);

			// create cpp server
			initCppServer();
			initEventPlugin();
			initAlgBrkPair();
			expectSmartComConnected(SMARTCOM_CONNECT_TOUT_SEC);

			initRtProvider();
			Uid fileUid = expectReadHistoryMessage(SMARTCOM_EXPORTDATA_TOUT_SEC);
			performHistoryExport(fileUid);

			sendThreadMessages();
			
		} catch (Throwable e) {
			logger.addErrorEntry("Error occured: " + e.getMessage());
		} finally {
			// unsubscribe
			logger.addStringEntry("Unsubscribing for events form SMARTCOM...");
			RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).unsubscribeForAllEvents(this);

		}


		logger.addStringEntry("Finished");

	}

	/**
	 * Helpers
	 */
	
	void initDataProfile() throws Exception
	{
		logger_m.addStringEntry("Initializing data profile: " + HISTORY_DATA_PROFILE);
		
		// 1-  must exist
		Set<String> existingProfiles = new HashSet<String>();
		HtExtServerCommand.commandRunWrapper(logger_m, configurator_m.getRegisteredDataProfileList(existingProfiles));
		
		HtDataProfileProp prop  = null;
		if (!existingProfiles.contains(HISTORY_DATA_PROFILE)) {
			//
			prop  = new HtDataProfileProp();
			logger_m.addStringEntry("Creating data profile: " + HISTORY_DATA_PROFILE);
			
			LongParam res = new LongParam();
			StringBuilder res_str  = new StringBuilder();
			
			HtExtServerCommand.commandRunWrapper(logger_m, configurator_m.setDataProfileProperty(HISTORY_DATA_PROFILE, prop, res, res_str));
			
		} else {
			
			// need to stop RT provider
			logger_m.addStringEntry("Stopping RT provider because updating the profile: " + RT_PROVIDER);
			HtExtGlobalCommand globalcommand = new HtExtGlobalCommand(logger_m);
			HtExtBoolBuffer isRunningStatus = new HtExtBoolBuffer();
			globalcommand.checkRtProviderRunning(RT_PROVIDER, isRunningStatus);
			if (isRunningStatus.getBoolean()) {
				logger_m.addStringEntry("Stoping RT provider: " + RT_PROVIDER);
				HtExtServerCommand.commandRunWrapper(logger_m, globalcommand.stopRtProvider(RT_PROVIDER));
			}
		
			
			HtExtPropertyHolder<HtDataProfileProp> dataprofHolder = new HtExtPropertyHolder<HtDataProfileProp>();
			logger_m.addStringEntry("Resolving data profile: " + HISTORY_DATA_PROFILE);
			
			HtExtServerCommand.commandRunWrapper(logger_m, configurator_m.getDataProfileProperty(HISTORY_DATA_PROFILE, dataprofHolder));
			prop = dataprofHolder.getProperty();
		}
		
		// 1 minutes for RT_PROVIDER_BAR
		prop.getProvMultFactorMap().put(RT_PROVIDER_BAR, 1);
		prop.getProvTimeScaleMap().put(RT_PROVIDER_BAR, 0);
		prop.getProvIsSubscribedMap().put(RT_PROVIDER_BAR, true);
			
		// subscribe for RT
		prop.getIsSubscribedDrawObjMap().put(RT_PROVIDER_BAR, true );
		prop.getIsSubscribedDrawObjMap().put(RT_PROVIDER, true );
		prop.validateEntry(RT_PROVIDER);
		prop.validateEntry(RT_PROVIDER_BAR);
			
			
		logger_m.addStringEntry("Updating data profile: " + HISTORY_DATA_PROFILE);
		LongParam res = new LongParam();
		StringBuilder res_str  = new StringBuilder();
		
		HtExtServerCommand.commandRunWrapper(logger_m, configurator_m.setDataProfileProperty(HISTORY_DATA_PROFILE, prop, res, res_str));
							
		
		
	}
	
	void readParameters(Map<String, String> parameters) throws Exception {
		logger_m.addStringEntry("These parameters are registered in this script, but actual may differ **********");
		for (Iterator<String> it = parameters.keySet().iterator(); it.hasNext();) {
			String key = it.next();
			String value = parameters.get(key);
			logger_m.addStringEntry("Parameter: " + key + "=[" + value + "]");
		}

		PAR_SIM_PROFILE = HtUtils.getParameterWithException(parameters, "SIM_PROFILE");
		PAR_SMARTCOM_LOGIN = HtUtils.getParameterWithException(parameters, "SMARTCOM_LOGIN");
		PAR_SMARTCOM_PASSWORD = HtUtils.getParameterWithException(parameters, "SMARTCOM_PASSWORD");
		PAR_SMARTCOM_LOGLEVEL = HtUtils.parseInt(HtUtils.getParameterWithException(parameters, "SMARTCOM_LOGLEVEL"));
		PAR_SMARTCOM_HOST = HtUtils.getParameterWithException(parameters, "SMARTCOM_HOST");
		PAR_SMARTCOM_PORT = HtUtils.getParameterWithException(parameters, "SMARTCOM_PORT");
		PAR_SMARTCOM_PORTFOLIO = HtUtils.getParameterWithException(parameters, "SMARTCOM_PORTFOLIO");
		PAR_IS_DEBUG_CONFIGURATION = HtUtils.parseBoolean(HtUtils.getParameterWithException(parameters, "IS_DEBUG_CONFIGURATION"));
		PAR_RECREATE_ENTRIES = HtUtils.parseBoolean(HtUtils.getParameterWithException(parameters, "RECREATE_ENTRIES"));
		PAR_SYMBOL_FILE = HtUtils.getParameterWithException(parameters, "SYMBOL_FILE");
		PAR_BASE_EXPORT_PATH = HtUtils.getParameterWithException(parameters, "BASE_EXPORT_PATH");
		PAR_AUTO_START_TRADE = HtUtils.parseBoolean(HtUtils.getParameterWithException(parameters, "AUTO_START_TRADE"));
		SHORT_SELL =  HtUtils.parseBoolean(HtUtils.getParameterWithException(parameters, "SHORT_SELL"));
		
		logger_m.addStringEntry("****************************************************");

	}

	void initCppServer() throws Exception {
		final String cpp_server_name = (PAR_IS_DEBUG_CONFIGURATION ? CPP_SERVER_NAME + "_d" : CPP_SERVER_NAME);

		// stop if necessary
		final HtExtServerCommand srvCommand = new HtExtServerCommand(logger_m);
		
		HtExtIntBuffer status = new HtExtIntBuffer();
		HtExtServerCommand.commandRunWrapper(logger_m,srvCommand.getTradingServerStatus(cpp_server_name, status));

		if (status.getInt() == RtTradingServerManager.SERVER_RUN_OK) {
			logger_m.addStringEntry("Stoping trading server which is run OK: " + cpp_server_name);
			

			Thread th = new Thread()
			{
				public void run()
				{
					try {
						HtExtServerCommand.commandRunWrapper(logger_m, srvCommand.stopTradingServer(cpp_server_name));
					}
					catch(Throwable e)
					{
					}
				}
			};

			th.start();

			// wait for 7 secs
			logger_m.addStringEntry("Waiting for "+CPP_SERVER_NORMAL_SHUTDOWN_INTERVAL_SEC+" sec(s) to normally shutdown trading server: " + cpp_server_name);
			th.join(CPP_SERVER_NORMAL_SHUTDOWN_INTERVAL_SEC*1000);

			// check status again
			HtExtServerCommand.commandRunWrapper(logger_m,srvCommand.getTradingServerStatus(cpp_server_name, status));
			if (status.getInt() != RtTradingServerManager.SERVER_IDLE) {
				logger_m.addStringEntry("Killing trading server because normal shutdown failed: " + cpp_server_name);
				HtExtServerCommand.commandRunWrapper(logger_m, srvCommand.killTradingServer(cpp_server_name));
			}

			logger_m.addStringEntry("Stopped/Killed OK trading server: " + cpp_server_name);
			
		}
		else if (status.getInt() == RtTradingServerManager.SERVER_RTTHREAD_ERROR) {
			logger_m.addStringEntry("Killing trading server as detected status is errored: " + cpp_server_name);
			HtExtServerCommand.commandRunWrapper(logger_m, srvCommand.killTradingServer(cpp_server_name));

			logger_m.addStringEntry("Killed OK trading server: " + cpp_server_name);
			
		}

		Set<String> existingServers = new HashSet<String>();
		HtExtServerCommand.commandRunWrapper(logger_m, configurator_m.getTradingServerList(existingServers));
		if (!existingServers.contains(cpp_server_name) || PAR_RECREATE_ENTRIES) {


			HtServerProp srvProp = new HtServerProp();

			srvProp.setCommandPort( CPP_SERVER_PORT );
			srvProp.setLogEventLevel(PAR_IS_DEBUG_CONFIGURATION ? CommonLog.LL_DEBUGDETAIL : CommonLog.LL_INFO);
			srvProp.setUseSpawner(false); // direct start

			HashSet<File> execs = new HashSet<File>();
			HtExtServerCommand.commandRunWrapper(logger_m, configurator_m.getAllExecutables(execs));


			String cpp_name = PAR_IS_DEBUG_CONFIGURATION ? "htmain_srv_d.exe" : "htmain_srv.exe";
			String cpp_executable = findExecutable(cpp_name);



			if (cpp_executable == null) {
				throw new HtExtException("Cannot find server executable: " + cpp_name);
			}

			srvProp.setServerExecutable(cpp_executable);
			srvProp.setDebugLevel( PAR_IS_DEBUG_CONFIGURATION ? TradeServerLogLevels.DEBUG_DETAIL : TradeServerLogLevels.INFO ); // thiis is internam LOG(MSG_... level )

			HtExtServerCommand.commandRunWrapper(logger_m, configurator_m.setTradingServerProperty(cpp_server_name, srvProp));
			HtExtServerCommand.commandStringWrapper(logger_m, "Created CPP server entry: " + cpp_server_name);

		} else {
			logger_m.addStringEntry("Use existing Cpp server entry: " + cpp_server_name);
		}

		// ---------------------------
		// start server


		// need to re-read server property
		HtExtPropertyHolder<HtServerProp> srvPropHolder = new HtExtPropertyHolder<HtServerProp>();
		configurator_m.getTradingServerProperty(cpp_server_name, srvPropHolder);

		Map<String, HtServerProxy.HtServerProxy_ProfileEntry> profProvSymbMap = new HashMap<String, HtServerProxy.HtServerProxy_ProfileEntry>();

		HashSet<String> rtProvidersToSubscribe = new HashSet<String>();
		rtProvidersToSubscribe.add(RT_PROVIDER_BAR);
		rtProvidersToSubscribe.add(RT_PROVIDER);


		// start serevr
		HtExtServerCommand.commandRunWrapper(logger_m, srvCommand.startTradingServer(
						cpp_server_name,
						srvPropHolder.getProperty().getServerExecutable(),
						srvPropHolder.getProperty().getDebuglevel(),
						srvPropHolder.getProperty().getServerHost(),
						srvPropHolder.getProperty().getCommandPort(),
						-1,
						false, // not use spawner
						true, // launch process
						-1, // push all data
						-1, // push all data
						profProvSymbMap, // empty
						rtProvidersToSubscribe,
						false, // will not reuse cache
						srvPropHolder.getProperty().getLogEventLevel(), // this is event level
						srvPropHolder.getProperty().getSubscribedEvents(),
						false,
						false));

		HtExtServerCommand.commandStringWrapper(logger_m, "Started CPP server: " + cpp_server_name);


	}

	void initEventPlugin() throws Exception {
		// create event plugin
		HtExtGlobalCommand globalcommand = new HtExtGlobalCommand(logger_m);
		String cpp_server_name = (PAR_IS_DEBUG_CONFIGURATION ? CPP_SERVER_NAME + "_d" : CPP_SERVER_NAME);

		String event_plugin_name = (PAR_IS_DEBUG_CONFIGURATION ? EVENT_PLUGIN_NAME + "_d" : EVENT_PLUGIN_NAME);
		Set<String> existingPlugins = new HashSet<String>();
		HtExtServerCommand.commandRunWrapper(logger_m, configurator_m.getRegisteredEventPluginList(existingPlugins));

		// unload plugin if necessary
		HtExtBoolBuffer isRunningStatus = new HtExtBoolBuffer();
		globalcommand.checkEventPluginRunning(event_plugin_name, isRunningStatus);
		if (isRunningStatus.getBoolean()) {
			logger_m.addStringEntry("Stoping event plugin: " + event_plugin_name);
			HtExtServerCommand.commandRunWrapper(logger_m, globalcommand.stopEventPlugin(event_plugin_name));

		}


		if (!existingPlugins.contains(event_plugin_name) || PAR_RECREATE_ENTRIES) {


			String smartcom_alg_brk_pair = (PAR_IS_DEBUG_CONFIGURATION ? SMARTCOM_ALG_BRK_PAIR + "_d" : SMARTCOM_ALG_BRK_PAIR);

			HtEventPluginProp eventProp = new HtEventPluginProp();
			eventProp.setPluginClass("com.fin.httrader.eventplugins.HtSmartComLoader");

			eventProp.getParameters().put("SMARTCOM_PORT", PAR_SMARTCOM_PORT);
			eventProp.getParameters().put("SMARTCOM_HOST", PAR_SMARTCOM_HOST);
			eventProp.getParameters().put("SMARTCOM_PASSWORD", PAR_SMARTCOM_PASSWORD);
			eventProp.getParameters().put("LOG_LEVEL", String.valueOf(PAR_SMARTCOM_LOGLEVEL));
			eventProp.getParameters().put("SMARTCOM_LOGIN", PAR_SMARTCOM_LOGIN);
			eventProp.getParameters().put("SIM_PROFILE", PAR_SIM_PROFILE ); 
			eventProp.getParameters().put("SMARTCOM_SERVER_ID", cpp_server_name);
			eventProp.getParameters().put("SMARTCOM_ALG_BRK_PAIR", smartcom_alg_brk_pair);
			eventProp.getParameters().put("DO_GET_SYMBOLS", "true");
			eventProp.getParameters().put("RT_PROVIDER", RT_PROVIDER);
			eventProp.getParameters().put("RT_PROVIDER_BAR_POSTFIX", RT_PROVIDER_BAR_POSTFIX);
			eventProp.getParameters().put("ACCOMPANY_TICK_1MIN_BAR", "true");
			eventProp.getParameters().put("PORTFOLIO_NAME", PAR_SMARTCOM_PORTFOLIO);
			eventProp.getParameters().put("AUTOMATICALLY_RECONNECT", "true");
			eventProp.getParameters().put("SHORT_SELL", "true");

			HtExtServerCommand.commandRunWrapper(logger_m, configurator_m.setEventPluginProperty(event_plugin_name, eventProp));
			HtExtServerCommand.commandStringWrapper(logger_m, "Created/recreated event plugin entry: " + event_plugin_name);

		
		} else {
			logger_m.addStringEntry("Use existing event plugin entry: " + event_plugin_name);
			
		}

		// ---------------------- starting
		// read
		HtExtPropertyHolder<HtEventPluginProp> eventPropHolder = new HtExtPropertyHolder<HtEventPluginProp>();
		HtExtServerCommand.commandRunWrapper(logger_m, configurator_m.getEventPluginProperty(event_plugin_name, eventPropHolder));


		// resolve something
		PAR_SIM_PROFILE = eventPropHolder.getProperty().getParameters().get("SIM_PROFILE");
		if (PAR_SIM_PROFILE == null || PAR_SIM_PROFILE.length() <=0)
				throw new HtExtException("Invalid simulation profile name! Must exist 'SIM_PROFILE' in " + event_plugin_name);

		HtExtServerCommand.commandRunWrapper(logger_m, globalcommand.startEventPlugin(event_plugin_name, eventPropHolder.getProperty().getPluginClass(), eventPropHolder.getProperty().getParameters()));
		HtExtServerCommand.commandStringWrapper(logger_m, "Started event plugin entry: " + event_plugin_name);


	}

	void initAlgBrkPair() throws Exception {
		String smartcom_alg_brk_pair = (PAR_IS_DEBUG_CONFIGURATION ? SMARTCOM_ALG_BRK_PAIR + "_d" : SMARTCOM_ALG_BRK_PAIR);
		Set<String> existing = new HashSet<String>();
		HtExtServerCommand.commandRunWrapper(logger_m, configurator_m.getAlgBrkPairList(existing));

		runName_m = getRunNameBase() + "_run_" + HtDateTimeUtils.time2SimpleString_Gmt(HtDateTimeUtils.getCurGmtTime());
		runNameLoad_m = getRunNameBase() + "_init_" + HtDateTimeUtils.time2SimpleString_Gmt(HtDateTimeUtils.getCurGmtTime());
		
		String comment = "";

		String cpp_server_name = (PAR_IS_DEBUG_CONFIGURATION ? CPP_SERVER_NAME + "_d" : CPP_SERVER_NAME);

		// stop thread and unload if necessary


		if (!existing.contains(smartcom_alg_brk_pair) || PAR_RECREATE_ENTRIES) {

			HtAlgBrkPairProp algBrkProp = new HtAlgBrkPairProp();


			algBrkProp.getAlgoritmParams().put("HISTORY_DATA_PROFILE", HISTORY_DATA_PROFILE);
			algBrkProp.getAlgoritmParams().put("RT_PROVIDER", RT_PROVIDER);
			algBrkProp.getAlgoritmParams().put("RT_BAR_PROVIDER", RT_PROVIDER_BAR);
			algBrkProp.getAlgoritmParams().put("PERIOD", "15");
			algBrkProp.getAlgoritmParams().put("PERIOD_STOP", "10");
			algBrkProp.getAlgoritmParams().put("BEGIN_WORK_TIME", "10:00");
			algBrkProp.getAlgoritmParams().put("END_WORK_TIME", "23:45");
			algBrkProp.getAlgoritmParams().put("SHIFT", "0");
			algBrkProp.getAlgoritmParams().put("BASE_EXPORT_PATH", PAR_BASE_EXPORT_PATH);
			algBrkProp.getAlgoritmParams().put("IS_REVERSE", "false");
			algBrkProp.getAlgoritmParams().put("SYMBOL_FILE", PAR_SYMBOL_FILE);
			algBrkProp.getAlgoritmParams().put("KOEFF", "0.02");
			algBrkProp.getAlgoritmParams().put("MIN_LOTS", "1");
			algBrkProp.getAlgoritmParams().put("MAX_LOTS", "5");
			algBrkProp.getAlgoritmParams().put("RISK", "0.02");
			algBrkProp.getAlgoritmParams().put("ACCOUNT_SIZE", "10000");
			algBrkProp.getAlgoritmParams().put("TRADE_PERIOD_MINUTES", "5");

			String alg_dll_base = (PAR_IS_DEBUG_CONFIGURATION ? "htgerm_2channel_mm_bck_d.dll" : "htgerm_2channel_mm_bck.dll");
			String algDllName = findDll(alg_dll_base);
			if (algDllName == null) {
				throw new HtExtException("Cannot find algorithm dll: " + alg_dll_base);
			}

			algBrkProp.setAlgorithmPath(algDllName);

			String brk_dll_base = (PAR_IS_DEBUG_CONFIGURATION ? "htsmartcombrokerprovider_bck_d.dll" : "htsmartcombrokerprovider_bck.dll");
			String brkDllName = findDll(brk_dll_base);
			if (brkDllName == null) {
				throw new HtExtException("Cannot find algorithm dll: " + brk_dll_base);
			}

			algBrkProp.setBrokerPath(brkDllName);

			HtExtServerCommand.commandRunWrapper(logger_m, configurator_m.setAlgBrkPairProperty(smartcom_alg_brk_pair, algBrkProp));
			HtExtServerCommand.commandStringWrapper(logger_m, "Created algorithm/broker entry: " + smartcom_alg_brk_pair);

		} else {
			logger_m.addStringEntry("Use existing algorithm/broker entry: " + smartcom_alg_brk_pair);
		}

		// ------------------
		// load

		HtExtPropertyHolder<HtAlgBrkPairProp> algbrkPropHolder = new HtExtPropertyHolder<HtAlgBrkPairProp>();
		HtExtServerCommand.commandRunWrapper(logger_m, configurator_m.getAlgBrkPairProperty(smartcom_alg_brk_pair, algbrkPropHolder));

		HtExtServerCommand srvCommand = new HtExtServerCommand(logger_m);


		HtExtServerCommand.commandRunWrapper(logger_m, srvCommand.joinTradingServer(cpp_server_name));
		HtExtServerCommand.commandRunWrapper(logger_m,
						srvCommand.loadAlgBrkPair(
						smartcom_alg_brk_pair,
						runNameLoad_m,
						comment,
						1,
						algbrkPropHolder.getProperty().getAlgorithmPath(),
						algbrkPropHolder.getProperty().getBrokerPath(),
						algbrkPropHolder.getProperty().getBrokerPath2(),
						algbrkPropHolder.getProperty().getAlgoritmParams(),
						algbrkPropHolder.getProperty().getBrokerParams(),
						algbrkPropHolder.getProperty().getBrokerParams2()));


		HtExtServerCommand.commandStringWrapper(logger_m, "Started algorithm/broker entry: " + smartcom_alg_brk_pair);

		// start thread


		HtExtIntBuffer threadId = new HtExtIntBuffer();
		HtExtServerCommand.commandRunWrapper(logger_m, srvCommand.startThread(Arrays.asList(smartcom_alg_brk_pair), threadId));
		HtExtServerCommand.commandStringWrapper(logger_m, "Started thread: " + threadId.getInt());

		threadId_m = threadId.getInt();
	}

	public void initRtProvider() throws Exception {
		// check if providers are running and stop them

		HtExtGlobalCommand globalcommand = new HtExtGlobalCommand(logger_m);
		String cpp_server_name = (PAR_IS_DEBUG_CONFIGURATION ? CPP_SERVER_NAME + "_d" : CPP_SERVER_NAME);
		String smartcom_alg_brk_pair = (PAR_IS_DEBUG_CONFIGURATION ? SMARTCOM_ALG_BRK_PAIR + "_d" : SMARTCOM_ALG_BRK_PAIR);

		Set<String> existing = new HashSet<String>();
		HtExtServerCommand.commandRunWrapper(logger_m, configurator_m.getRegisteredRtProviderList(existing));

		HtExtBoolBuffer isRunningStatus = new HtExtBoolBuffer();
		globalcommand.checkRtProviderRunning(RT_PROVIDER, isRunningStatus);
		if (isRunningStatus.getBoolean()) {
			logger_m.addStringEntry("Stoping RT provider: " + RT_PROVIDER);
			HtExtServerCommand.commandRunWrapper(logger_m, globalcommand.stopRtProvider(RT_PROVIDER));

		}

		

		if (!existing.contains(RT_PROVIDER) || PAR_RECREATE_ENTRIES) {
			// create RT provider
			HtRTProviderProp rtProp = new HtRTProviderProp();
			rtProp.setHourShift(0);
			rtProp.setAlien( false );
			rtProp.setSignDigits(-1);
			rtProp.setProviderClass("com.fin.httrader.rtproviders.RtSmartComProvider");

			rtProp.getParameters().put("SYMBOL_FILE", PAR_SYMBOL_FILE);
			rtProp.getParameters().put("SMARTCOM_SERVER_ID", cpp_server_name);
			rtProp.getParameters().put("SMARTCOM_ALG_BRK_PAIR", smartcom_alg_brk_pair);
			rtProp.getParameters().put("REQUEST_TICK_HISTORY_COUNT", "-1");
			rtProp.getParameters().put("REQUEST_TICK_HISTORY_FROM_DATETIME", "");
			rtProp.getParameters().put("REQUEST_BARS_HISTORY_COUNT", "-10000");

			SimpleDateFormat fmt = new SimpleDateFormat("dd-MM-yyyy HH:mm");
			fmt.setTimeZone(TimeZone.getTimeZone("GMT-0"));

		
			rtProp.getParameters().put("REQUEST_BARS_HISTORY_FROM_DATETIME", fmt.format(HtDateTimeUtils.getCurGmtTime_DayBegin() - 1000 * 60 * 60 * 24)); // yesterday date
			rtProp.getParameters().put("REQUEST_BARS_HISTORY_AGGR_PERIOD", "AP_Minute");
			rtProp.getParameters().put("SUBSCRIBE_PRINTS", "true");
			rtProp.getParameters().put("SUBSCRIBE_LEVEL1", "true");
			rtProp.getParameters().put("SUBSCRIBE_LEVEL2", "true");
			rtProp.getParameters().put("REQUEST_HISTORY_TICKS", "false");
			rtProp.getParameters().put("REQUEST_HISTORY_BARS", "true");

			HtExtServerCommand.commandRunWrapper(logger_m, configurator_m.setRtProviderProperty(RT_PROVIDER, rtProp));
			HtExtServerCommand.commandStringWrapper(logger_m, "Created/recreated RT Provider entry: " + RT_PROVIDER);

		} else {
			logger_m.addStringEntry("Use existing RT Provider entry: " + RT_PROVIDER);
		}

		if (!existing.contains(RT_PROVIDER_BAR) || PAR_RECREATE_ENTRIES) {
			HtRTProviderProp rtProp2 = new HtRTProviderProp();
			rtProp2.setHourShift( 0 );
			rtProp2.setAlien(true);
			rtProp2.setSignDigits(-1);
			rtProp2.setProviderClass("dummy");

			HtExtServerCommand.commandRunWrapper(logger_m, configurator_m.setRtProviderProperty(RT_PROVIDER_BAR, rtProp2));
			HtExtServerCommand.commandStringWrapper(logger_m, "Created RT Provider entry: " + RT_PROVIDER_BAR);
		} else {
			logger_m.addStringEntry("Use existing RT Provider entry: " + RT_PROVIDER_BAR);
		}

		// start RT_PROVIDER
		HtExtPropertyHolder<HtRTProviderProp> rtProp3 = new HtExtPropertyHolder<HtRTProviderProp>();
		HtExtServerCommand.commandRunWrapper(logger_m, configurator_m.getRtProviderProperty(RT_PROVIDER, rtProp3));
		HtExtServerCommand.commandRunWrapper(logger_m,
						globalcommand.startRtProvider(
						RT_PROVIDER,
						rtProp3.getProperty().getProviderClass(),
						rtProp3.getProperty().getParameters(),
						rtProp3.getProperty().getHourShift()));

		logger_m.addStringEntry("RT Provider started: " + RT_PROVIDER);
	}

	public void performHistoryExport(Uid fileUid) throws Exception
	{
		// here we shall perform history export of history
		HtExtGlobalCommand globalcommand = new HtExtGlobalCommand(logger_m);
		Set<String> existinghistoryProviders = new HashSet<String>();
		HtExtServerCommand.commandRunWrapper(logger_m, configurator_m.getRegisteredHistoryProviderList(existinghistoryProviders));

		// history provider will be always recreated
		HtHistoryProviderProp hstProp = new HtHistoryProviderProp();
		hstProp.setExportHourShift( 0 );
		hstProp.setImportHourShift(0);
		hstProp.setProviderClass("com.fin.httrader.hstproviders.RtSmartComHistoryProvider");
		hstProp.setPropertyName(HST_PROVIDER);

		String fileName = HtSystem.getSystemTempPath() + File.separator + "smartcom_tmp" + File.separator + fileUid.toString() +".tmpcache";
		File f = new File(fileName);
		if (!f.exists()) {
			throw new HtExtException("Export data file: " + fileName + " not found");
		}
		
		hstProp.getParameters().put("IMPORT_FILE", fileName);

		HtExtServerCommand.commandRunWrapper(logger_m, configurator_m.setHistoryProviderProperty(HST_PROVIDER, hstProp));
		HtExtServerCommand.commandStringWrapper(logger_m, "Forcibly created history provider entry: " + HST_PROVIDER);

		String import_id = "german_import_" + HtDateTimeUtils.getCurGmtTime();
		HtExtServerCommand.commandRunWrapper(logger_m,
						globalcommand.importData(
								import_id,
								HISTORY_DATA_PROFILE,
								HtDatabaseProxy.IMPORT_FLAG_OVERWRITE,
								RT_PROVIDER_BAR,
								hstProp.getPropertyName(),
								hstProp.getProviderClass(),
								hstProp.getParameters(),
								-1,
								-1,
								0,
								false,
								true)
		);
		
		
		logger_m.addStringEntry("Performed history import targeting to RT provider: '" + RT_PROVIDER_BAR +
						"' using history provider: '" + hstProp.getPropertyName() +
						"' wrote to profile: " + HISTORY_DATA_PROFILE + "'");
	}

	public void sendThreadMessages() throws Exception {
		if (threadId_m < 0) {
			throw new HtExtException("Invalid thread ID, probably thread is not started");
		}

		String comment = "";
		if (runName_m == null) {
			throw new HtExtException("Invalid run name, probably algorithm/broker pair was not loaded");
		}

		String cpp_server_name = (PAR_IS_DEBUG_CONFIGURATION ? CPP_SERVER_NAME + "_d" : CPP_SERVER_NAME);
		String smartcom_alg_brk_pair = (PAR_IS_DEBUG_CONFIGURATION ? SMARTCOM_ALG_BRK_PAIR + "_d" : SMARTCOM_ALG_BRK_PAIR);

		HtExtServerCommand srvCommand = new HtExtServerCommand(logger_m);
		HtExtServerCommand.commandRunWrapper(logger_m, srvCommand.joinTradingServer(cpp_server_name));

		StringBuilder out = new StringBuilder();
		HtExtServerCommand.commandRunWrapper(logger_m, srvCommand.sendCustomEventToAlgorithm(
						cpp_server_name,
						smartcom_alg_brk_pair,
						threadId_m,
						"load_data",
						"",
						ALG_MSG_TOUT_SEC,
						out));

		if (!out.toString().equalsIgnoreCase("OK")) {
			throw new HtExtException("Event failed to algorithm: " + smartcom_alg_brk_pair + " returned message: " + out.toString());
		}

		HtExtServerCommand.commandStringWrapper(logger_m, "Loaded data");

		if (PAR_AUTO_START_TRADE) {
			HtExtServerCommand.commandRunWrapper(logger_m, srvCommand.sendCustomEventToAlgorithm(
							cpp_server_name,
							smartcom_alg_brk_pair,
							threadId_m,
							"start_trade",
							"",
							ALG_MSG_TOUT_SEC,
							out));

			if (!out.toString().equalsIgnoreCase("OK")) {
				throw new HtExtException("Event failed to algorithm: " + smartcom_alg_brk_pair + " returned message: " + out.toString());
			}

			HtExtServerCommand.commandStringWrapper(logger_m, "Allowed broker operations");
		} else {
			logger_m.addStringEntry("Please enable trade via 'start_trade' command");
		}

		// SIM_PROFILE_NAME
		if (PAR_SIM_PROFILE.length() <=0) {
			throw new HtExtException("Invalid simulation profile name");
		}
		else {
			logger_m.addStringEntry("Use simulation profile: [" + PAR_SIM_PROFILE + "] for RT activity");
		}
		
		HtExtServerCommand.commandRunWrapper(logger_m, srvCommand.startRtActivity(threadId_m, runName_m, comment, PAR_SIM_PROFILE));
		HtExtServerCommand.commandStringWrapper(logger_m, "Started RT activity");
	}

	/**
	 *
	 * Helpers
	 */
	private String findExecutable(String name) throws Exception {
		String cpp_executable = null;

		HashSet<File> execs = new HashSet<File>();
		HtExtServerCommand.commandRunWrapper(logger_m, configurator_m.getAllExecutables(execs));

		for (Iterator<File> it = execs.iterator(); it.hasNext();) {
			File file_i = it.next();

			if (file_i.getName().toLowerCase().indexOf(name) >= 0) {
				cpp_executable = file_i.getAbsolutePath();
				break;
			}

		}

		return cpp_executable;
	}

	private String findDll(String name) throws Exception {
		String dll_name = null;

		HashSet<File> execs = new HashSet<File>();
		HtExtServerCommand.commandRunWrapper(logger_m, configurator_m.getAllDlls(execs));

		for (Iterator<File> it = execs.iterator(); it.hasNext(); ) {
			File file_i = it.next();

			if (file_i.getName().toLowerCase().indexOf(name) >= 0) {
				dll_name = file_i.getAbsolutePath();
				break;
			}

		}

		return dll_name;
	}

	// this just waits when smartcom is connected
	private void expectSmartComConnected(final int waitsec) throws Exception {
		logger_m.addStringEntry("Waiting SMARTCOM connect status for: " + waitsec + " sec(s)...");


		final String cpp_server_name = (PAR_IS_DEBUG_CONFIGURATION ? CPP_SERVER_NAME + "_d" : CPP_SERVER_NAME);
		final ArrayList list = new ArrayList();


		final LongParam success = new LongParam();
		success.setLong(-1);

		Thread runnable = new Thread() {

			@Override
			public void run() {
				boolean to_stop = false;
				HtTimeCalculator tcalc = new HtTimeCalculator();

				while (true) {

					if (to_stop) {
						break;
					}

					// wait 1 sec
					eventsArrived_m.get(list, 1000);

					for (Iterator it = list.iterator(); it.hasNext();) {
						XmlEvent returned = (XmlEvent) it.next();

						if (returned != null) {
							String server_id = returned.getAsXmlParameter().getString("server_id");
							String rt_provider = returned.getAsXmlParameter().getString("provider_name");
							int msgType = (int) returned.getAsXmlParameter().getInt("type");
							try {
								logger_m.addStringEntry("Event arrrived: " + returned.toString());
							} catch (HtExtException ex) {
								
							}
							
							if (msgType == HtRtData.SYNC_EVENT_PROVIDER_START) {
								if (server_id.equalsIgnoreCase(cpp_server_name) && rt_provider.equalsIgnoreCase(RT_PROVIDER)) {
									to_stop = true;
								}
							}
						}
					}

					if (tcalc.elapsed() > waitsec * 1000) {
						break;
					}


				} // end loop

				if (to_stop) {
					// success
					success.setLong(0);
				}


			}
		};

		runnable.start();
		runnable.join();


		if (success.getLong() == 0) {
			logger_m.addStringEntry("Success - connected!");
		} else {
			throw new HtExtException("Connect confirmation event was not arrived from SMARTCOM layer!");

		}
	}

	// here we will be waiting the confirmation whn history data are ready!!!
	private Uid expectReadHistoryMessage(final int waitsec) throws Exception {
		logger_m.addStringEntry("Waiting SMARTCOM read history  for: " + waitsec + " sec(s)...");
		final Uid uid = new Uid();

		final String cpp_server_name = (PAR_IS_DEBUG_CONFIGURATION ? CPP_SERVER_NAME + "_d" : CPP_SERVER_NAME);
		final ArrayList list = new ArrayList();


		final LongParam success = new LongParam();
		success.setLong(-1);

		Thread runnable = new Thread() {

			@Override
			public void run() {
				boolean to_stop = false;
				HtTimeCalculator tcalc = new HtTimeCalculator();

				while (true) {

					if (to_stop) {
						break;
					}

					// wait 1 sec
					eventsArrived_m.get(list, 1000);

					for (Iterator it = list.iterator(); it.hasNext();) {
						XmlEvent returned = (XmlEvent) it.next();

						if (returned != null) {
							String server_id = returned.getAsXmlParameter().getString("server_id");
							String rt_provider = returned.getAsXmlParameter().getString("provider_name");
							int msgType = (int) returned.getAsXmlParameter().getInt("type");

							if (msgType == HtRtData.SYNC_EVENT_CUSTOM_MESSAGE) {
								if (server_id.equalsIgnoreCase(cpp_server_name) && rt_provider.equalsIgnoreCase(RT_PROVIDER)) {
									String message = returned.getAsXmlParameter().getString("message");

									XmlParameter xmlparam = new XmlParameter();
									HumanSerializer.deserializeXmlParameter(message, xmlparam);

									String msg_type = xmlparam.getString("MSG");
									String file_uid = xmlparam.getString("UID");
									uid.fromString(file_uid);

									if (msg_type.equalsIgnoreCase("HISTORY_SAVED")) {
										to_stop = true;
									}
								}
							}
						}
					}

					if (tcalc.elapsed() > waitsec * 1000) {
						break;
					}


				} // end loop

				if (to_stop) {
					// success
					success.setLong(0);
				}


			}
		};

		runnable.start();
		runnable.join();


		if (success.getLong() == 0) {
			logger_m.addStringEntry("Success - history read! File UID: " + uid.toString());
		} else {
			throw new HtExtException("History read confirmation event was not arrived from SMARTCOM layer!");

		}

		return uid;
	}


	
	// -----------------------

	@Override
	public void onEventArrived(XmlEvent event) throws SingleReaderQueueExceedLimit {
		eventsArrived_m.put(event);
	}

	@Override
	public String getListenerName() {
		return " listener: [ SMARTCOM temporary listener ] ";
	}
}
