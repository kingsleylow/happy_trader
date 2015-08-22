/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.externalapi.custom;

import com.fin.httrader.externalapi.utils.HtExtPropertyHolder;
import com.fin.httrader.externalapi.HtExtBaseLogger;
import com.fin.httrader.externalapi.HtExtException;
import com.fin.httrader.externalapi.utils.HtExtBoolBuffer;
import com.fin.httrader.configprops.HtAlgBrkPairProp;
import com.fin.httrader.configprops.HtDataProfileProp;
import com.fin.httrader.configprops.HtEventPluginProp;
import com.fin.httrader.configprops.HtHistoryProviderProp;
import com.fin.httrader.configprops.HtRTProviderProp;
import com.fin.httrader.configprops.HtServerProp;
import com.fin.httrader.externalapi.*;
import com.fin.httrader.externalapi.utils.HtExtIntBuffer;
import com.fin.httrader.hlpstruct.TradeServerLogLevels;
import com.fin.httrader.hlpstruct.externalrunstruct.RunParameter;
import com.fin.httrader.hlpstruct.externalrunstruct.RunParameterUtils;
import com.fin.httrader.interfaces.HtEventListener;
import com.fin.httrader.managers.RtConfigurationManager;
import com.fin.httrader.managers.RtGlobalEventManager;
import com.fin.httrader.managers.RtTradingServerManager;
import com.fin.httrader.model.HtDatabaseProxy;
import com.fin.httrader.model.HtServerProxy;
import com.fin.httrader.utils.*;
import com.fin.httrader.utils.hlpstruct.*;
import java.io.File;
import java.io.FileReader;
import java.text.SimpleDateFormat;
import java.util.*;

/**
 *
 * @author Victor_Zoubok
 */
public class Main implements HtExtRunner, HtEventListener {

  private static final String START_MODE_CLEAR = "CLEAR";
  private static final String START_MODE_CREATE = "CREATE";
  private static final String START_MODE_RUN = "RUN";
  // -----------------------------
  private String PAR_SMARTCOM_LOGIN = "";
  private String PAR_SIM_PROFILE = "";
  private String PAR_SMARTCOM_PASSWORD = "";
  private int PAR_SMARTCOM_LOGLEVEL = 2;
  private String PAR_SMARTCOM_HOST = "";
  private String PAR_SMARTCOM_PORT = "";
  private int PAR_RECONNECT_TIMEOUT = 30;
  private String PAR_SMARTCOM_PORTFOLIO = "";
  private int PAR_REQUEST_MY_TRADES_POLL_SEC = -1;
  private boolean PAR_IS_DEBUG_CONFIGURATION = false;
  private String PAR_SYMBOL_FILE = "";
  private String PAR_BASE_EXPORT_PATH = "";
  private String PAR_CPP_SERVER_NAME = "";
  private int PAR_CPP_SERVER_PORT = -1;
  private String PAR_RT_PROVIDER = "";
  private String PAR_HST_PROVIDER = "";
  private String PAR_RT_PROVIDER_BAR_POSTFIX = "";
  private String PAR_RT_PROVIDER_BAR = "";
  private String PAR_EVENT_PLUGIN_NAME = "";
  private String PAR_SMARTCOM_ALG_BRK_PAIR = "";
  private String PAR_HISTORY_DATA_PROFILE = "";
  private String PAR_HST_PROVIDER_CLASS = "";
  private String PAR_EVENT_PLUGIN_CLASS = "";
  private String PAR_RT_PROVIDER_CLASS = "";
  private String PAR_ALG_DLL = "";
  private String PAR_BRK1_DLL = "";
  private String PAR_BRK2_DLL = "";
  private int PAR_BROKER_SEQ_NUM = -1;
  
 
  
  // RT provider
  private String PAR_REQUEST_BARS_HISTORY_AGGR_PERIOD = "";
  private int PAR_REQUEST_BARS_HISTORY_MULTFACTOR = -1;
  
  // 
  private final int SMARTCOM_CONNECT_TOUT_SEC = 200;
  private final int SMARTCOM_EXPORTDATA_TOUT_SEC = 2000;
  private final int ALG_MSG_TOUT_SEC = 10;
  // ----------------------------
  private HtExtBaseLogger logger_m = null;
  private HtExtConfiguration configurator_m = new HtExtConfiguration();
  private final SingleReaderQueue eventsArrived_m = new SingleReaderQueue("Wait for SMARTCOM connection event", false, RtConfigurationManager.StartUpConst.MAX_ELEMENTS_IN_QUEUE);
  private int threadId_m = -1;
  private String runName_m = null;
  private String runNameLoad_m = null;
  // list of symbols
  private Set<String> symbols_m = new HashSet<String>();

  // --------------------------------
  @Override
  public String getRunName() {
	return "manage_smartcom_lib";
  }

  public String getRunNameBase() {
	return getRunName();
  }
  
  
 
  @Override
  public void run(HtExtBaseLogger logger, Map<String, RunParameter> parameters, Map<String, String> runKeys, StringBuilder customResult) throws Exception {

	// first create all necessaryy entries and parameters
	logger_m = logger;


	try {

	  
	  readParameters(parameters);
	  String start_mode = runKeys.get("START_MODE");
	  if (start_mode == null)
		throw new HtExtException("Invalid 'START_MODE' parameter");

	  if (start_mode.equalsIgnoreCase(START_MODE_CLEAR)) {
		logger.addStringEntry("Removing entries...");

		this.deleteDataProfile();
		this.deleteRtprovider();
		this.deleteHistoryProvider();
		this.deleteCppServer();
		this.deleteEventPligin();
		this.deleteAlgLibPair();
	  } else if (start_mode.equalsIgnoreCase(START_MODE_CREATE)) {
		logger.addStringEntry("Creating entries...");

		initDataProfile();
		initRtProvider();
		initHistoryProvider();

		// create cpp server
		initCppServer();
		initEventPlugin();
		//initSimProfile();
		initAlgBrkPair(parameters);
	  } else if (start_mode.equalsIgnoreCase(START_MODE_RUN)) {
		RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).subscribeForEvent(XmlEvent.ET_RtProviderSynchronizationEvent, 5001, this);


		startCppServer();
		startEventPlugin();
		startAlgBrkPair();
		expectSmartComConnected();

		startRtProvider();
		expectReadHistoryMessage();

		sendThreadMessages();

	  } else {
		throw new HtExtException("Invalid 'START_MODE' parameter");
	  }


	} catch (Throwable e) {
	  logger.addErrorEntry("Error occured: " + e.getMessage());
	  String strace = HtException.getFullErrorString(e);
	  logger.addErrorEntry(strace);
	  
	} finally {
	  // unsubscribe
	  RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).unsubscribeForAllEvents(this);
	}


	logger.addStringEntry("Finished");
	customResult.append("Finished Ok");

  }

  /**
   * Helpers
   */
  void initDataProfile() throws Exception {
	logger_m.addStringEntry("Initializing data profile: " + PAR_HISTORY_DATA_PROFILE);

	// 1-  must exist

	HtDataProfileProp prop = new HtDataProfileProp();

	// 1 minutes for RT_PROVIDER_BAR
	prop.getProvMultFactorMap().put(PAR_RT_PROVIDER_BAR, 1);
	prop.getProvTimeScaleMap().put(PAR_RT_PROVIDER_BAR, 0);
	prop.getProvIsSubscribedMap().put(PAR_RT_PROVIDER_BAR, true);

	// subscribe for RT
	prop.getIsSubscribedDrawObjMap().put(PAR_RT_PROVIDER_BAR, true);
	prop.getIsSubscribedDrawObjMap().put(PAR_RT_PROVIDER, true);
	prop.validateEntry(PAR_RT_PROVIDER);
	prop.validateEntry(PAR_RT_PROVIDER_BAR);



	StringBuilder res_str = new StringBuilder();
	LongParam resultVar = new LongParam();

	HtExtServerCommand.commandRunWrapper(logger_m, configurator_m.setDataProfileProperty(PAR_HISTORY_DATA_PROFILE, prop, resultVar, res_str));
	logger_m.addStringEntry("When creatined data profile: [" + PAR_HISTORY_DATA_PROFILE + "] mesage returned: " + res_str);

	if (resultVar.getLong() != 0) {
	  throw new HtExtException("Exception on creating data profile!");
	}



  }

  void readParameters(Map<String, RunParameter> parameters) throws Exception {




	PAR_SIM_PROFILE = RunParameterUtils.getSimleStringParameter(parameters, "SIM_PROFILE", true);

	PAR_SMARTCOM_LOGIN = RunParameterUtils.getSimleStringParameter(parameters, "SMARTCOM_LOGIN", true);
	PAR_SMARTCOM_PASSWORD = RunParameterUtils.getSimleStringParameter(parameters, "SMARTCOM_PASSWORD", true);
	PAR_SMARTCOM_LOGLEVEL = HtUtils.parseInt(RunParameterUtils.getSimleStringParameter(parameters, "SMARTCOM_LOGLEVEL", true));
	PAR_SMARTCOM_HOST = RunParameterUtils.getSimleStringParameter(parameters, "SMARTCOM_HOST", true);
	PAR_SMARTCOM_PORT = RunParameterUtils.getSimleStringParameter(parameters, "SMARTCOM_PORT", true);
	PAR_RECONNECT_TIMEOUT = HtUtils.parseInt(RunParameterUtils.getSimleStringParameter(parameters, "RECONNECT_TIMEOUT", true));
	PAR_SMARTCOM_PORTFOLIO = RunParameterUtils.getSimleStringParameter(parameters, "SMARTCOM_PORTFOLIO", true);
	PAR_REQUEST_MY_TRADES_POLL_SEC = HtUtils.parseInt(RunParameterUtils.getSimleStringParameter(parameters, "REQUEST_MY_TRADES_POLL_SEC", true));
	PAR_IS_DEBUG_CONFIGURATION = HtUtils.parseBoolean(RunParameterUtils.getSimleStringParameter(parameters, "IS_DEBUG_CONFIGURATION", true));
	PAR_SYMBOL_FILE = RunParameterUtils.getSimleStringParameter(parameters, "SYMBOL_FILE", true);
	PAR_BASE_EXPORT_PATH = RunParameterUtils.getSimleStringParameter(parameters, "BASE_EXPORT_PATH", true);
	PAR_CPP_SERVER_NAME = RunParameterUtils.getSimleStringParameter(parameters, "CPP_SERVER_NAME", true);
	PAR_CPP_SERVER_PORT = HtUtils.parseInt(RunParameterUtils.getSimleStringParameter(parameters, "CPP_SERVER_PORT", true));
	PAR_RT_PROVIDER = RunParameterUtils.getSimleStringParameter(parameters, "RT_PROVIDER", true);
	PAR_HST_PROVIDER = RunParameterUtils.getSimleStringParameter(parameters, "HST_PROVIDER", true);
	PAR_RT_PROVIDER_BAR_POSTFIX = RunParameterUtils.getSimleStringParameter(parameters, "RT_PROVIDER_BAR_POSTFIX", true);
	PAR_RT_PROVIDER_BAR = PAR_RT_PROVIDER + PAR_RT_PROVIDER_BAR_POSTFIX;
	PAR_EVENT_PLUGIN_NAME = RunParameterUtils.getSimleStringParameter(parameters, "EVENT_PLUGIN_NAME", true);
	PAR_SMARTCOM_ALG_BRK_PAIR = RunParameterUtils.getSimleStringParameter(parameters, "SMARTCOM_ALG_BRK_PAIR", true);
	PAR_HISTORY_DATA_PROFILE = RunParameterUtils.getSimleStringParameter(parameters, "HISTORY_DATA_PROFILE", true);
	PAR_HST_PROVIDER_CLASS = RunParameterUtils.getSimleStringParameter(parameters, "HST_PROVIDER_CLASS", true);
	PAR_EVENT_PLUGIN_CLASS = RunParameterUtils.getSimleStringParameter(parameters, "EVENT_PLUGIN_CLASS", true);
	PAR_RT_PROVIDER_CLASS = RunParameterUtils.getSimleStringParameter(parameters, "RT_PROVIDER_CLASS", true);
	PAR_BROKER_SEQ_NUM = HtUtils.parseInt(RunParameterUtils.getSimleStringParameter(parameters, "BROKER_SEQ_NUM", true));
	
	PAR_REQUEST_BARS_HISTORY_AGGR_PERIOD = RunParameterUtils.getSimleStringParameter(parameters, "REQUEST_BARS_HISTORY_AGGR_PERIOD", true);
	PAR_REQUEST_BARS_HISTORY_MULTFACTOR = HtUtils.parseInt(RunParameterUtils.getSimleStringParameter(parameters, "REQUEST_BARS_HISTORY_MULTFACTOR", true));

	PAR_ALG_DLL = RunParameterUtils.getSimleStringParameter(parameters, "ALG_DLL", true);
	PAR_BRK1_DLL = RunParameterUtils.getSimleStringParameter(parameters, "BRK1_DLL", true);
	PAR_BRK2_DLL = RunParameterUtils.getSimleStringParameter(parameters, "BRK2_DLL", true);

	
	
	symbols_m.clear();
	symbols_m.addAll( HtFileUtils.readSymbolListFromFile(PAR_SYMBOL_FILE) );
	HtExtServerCommand.commandStringWrapper(logger_m, "Read total symbols: " + symbols_m.size());
	
	if (symbols_m.isEmpty())
	  throw new HtExtException("Symbol list cannot be empty!");

  }

  void initCppServer() throws Exception {
	String cpp_server_name = (PAR_IS_DEBUG_CONFIGURATION ? PAR_CPP_SERVER_NAME + "_d" : PAR_CPP_SERVER_NAME);



	HtServerProp srvProp = new HtServerProp();

	srvProp.setCommandPort(PAR_CPP_SERVER_PORT);
	srvProp.setLogEventLevel(PAR_IS_DEBUG_CONFIGURATION ? CommonLog.LL_DEBUGDETAIL : CommonLog.LL_INFO);
	srvProp.setUseSpawner(false); // direct start

	HashSet<File> execs = new HashSet<File>();
	HtExtServerCommand.commandRunWrapper(logger_m, configurator_m.getAllExecutables(execs));



	String cpp_executable = findExecutable(PAR_IS_DEBUG_CONFIGURATION);

	if (cpp_executable == null) {
	  throw new HtExtException("Cannot find server executable");
	}

	srvProp.setServerExecutable(cpp_executable);
	srvProp.setDebugLevel(PAR_IS_DEBUG_CONFIGURATION ? TradeServerLogLevels.DEBUG_DETAIL : TradeServerLogLevels.INFO); // thiis is internam LOG(MSG_... level )

	HtExtServerCommand.commandRunWrapper(logger_m, configurator_m.setTradingServerProperty(cpp_server_name, srvProp));
	HtExtServerCommand.commandStringWrapper(logger_m, "Created CPP server entry: " + cpp_server_name);



	// ---------------------------
	// start server


	// need to re-read server property
	HtExtPropertyHolder<HtServerProp> srvPropHolder = new HtExtPropertyHolder<HtServerProp>();
	configurator_m.getTradingServerProperty(cpp_server_name, srvPropHolder);


	HashSet<String> rtProvidersToSubscribe = new HashSet<String>();
	rtProvidersToSubscribe.add(PAR_RT_PROVIDER_BAR);
	rtProvidersToSubscribe.add(PAR_RT_PROVIDER);


	// start serevr
	HtExtServerCommand.commandStringWrapper(logger_m, "Started CPP server: " + cpp_server_name);


  }

  void initEventPlugin() throws Exception {
	// create event plugin

	String cpp_server_name = (PAR_IS_DEBUG_CONFIGURATION ? PAR_CPP_SERVER_NAME + "_d" : PAR_CPP_SERVER_NAME);
	String event_plugin_name = PAR_EVENT_PLUGIN_NAME;


	String smartcom_alg_brk_pair = (PAR_IS_DEBUG_CONFIGURATION ? PAR_SMARTCOM_ALG_BRK_PAIR + "_d" : PAR_SMARTCOM_ALG_BRK_PAIR);

	HtEventPluginProp eventProp = new HtEventPluginProp();
	eventProp.setPluginClass(PAR_EVENT_PLUGIN_CLASS);

	eventProp.getParameters().put("SMARTCOM_PORT", PAR_SMARTCOM_PORT);
	eventProp.getParameters().put("SMARTCOM_HOST", PAR_SMARTCOM_HOST);
	eventProp.getParameters().put("RECONNECT_TIMEOUT", String.valueOf(PAR_RECONNECT_TIMEOUT));
	eventProp.getParameters().put("SMARTCOM_PASSWORD", PAR_SMARTCOM_PASSWORD);
	eventProp.getParameters().put("LOG_LEVEL", String.valueOf(PAR_SMARTCOM_LOGLEVEL));
	eventProp.getParameters().put("SMARTCOM_LOGIN", PAR_SMARTCOM_LOGIN);
	eventProp.getParameters().put("SIM_PROFILE", PAR_SIM_PROFILE);
	eventProp.getParameters().put("SMARTCOM_SERVER_ID", cpp_server_name);
	eventProp.getParameters().put("SMARTCOM_ALG_BRK_PAIR", smartcom_alg_brk_pair);
	eventProp.getParameters().put("RT_PROVIDER", PAR_RT_PROVIDER);
	eventProp.getParameters().put("RT_PROVIDER_BAR_POSTFIX", PAR_RT_PROVIDER_BAR_POSTFIX);
	eventProp.getParameters().put("ACCOMPANY_TICK_1MIN_BAR", "true");
	eventProp.getParameters().put("PORTFOLIO_NAME", PAR_SMARTCOM_PORTFOLIO);
	eventProp.getParameters().put("AUTOMATICALLY_RECONNECT", "true");
	eventProp.getParameters().put("BROKER_SEQ_NUM", String.valueOf(PAR_BROKER_SEQ_NUM));
	


	HtExtServerCommand.commandRunWrapper(logger_m, configurator_m.setEventPluginProperty(event_plugin_name, eventProp));
	HtExtServerCommand.commandStringWrapper(logger_m, "Created event plugin entry: " + event_plugin_name);

  }

  void initSimProfile() throws Exception {
  }

  void initAlgBrkPair(Map<String, RunParameter> parameters) throws Exception {
	String smartcom_alg_brk_pair = (PAR_IS_DEBUG_CONFIGURATION ? PAR_SMARTCOM_ALG_BRK_PAIR + "_d" : PAR_SMARTCOM_ALG_BRK_PAIR);



	HtAlgBrkPairProp algBrkProp = new HtAlgBrkPairProp();

	String algDllName = findDll(PAR_ALG_DLL, PAR_IS_DEBUG_CONFIGURATION);
	if (algDllName == null) {
	  throw new HtExtException("Cannot find algorithm dll: " + PAR_ALG_DLL);
	}

	algBrkProp.setAlgorithmPath(algDllName);

	String brkDllName = findDll(PAR_BRK1_DLL, PAR_IS_DEBUG_CONFIGURATION);
	if (brkDllName == null) {
	  throw new HtExtException("Cannot find broker dll: " + PAR_BRK1_DLL);
	}

	algBrkProp.setBrokerPath(brkDllName);

	String brkDll2Name = findDll(PAR_BRK2_DLL, PAR_IS_DEBUG_CONFIGURATION);
	if (brkDll2Name == null) {
	  throw new HtExtException("Cannot find broker dll: " + PAR_BRK2_DLL);
	}

	algBrkProp.setBrokerPath2(brkDll2Name);

	// now need to collect broker parameters
	Map<String, String> ap = RunParameterUtils.getMapStringParameter(parameters, "ALG_LIB_PARAMS", true);
	algBrkProp.getAlgoritmParams().putAll(ap);


	HtExtServerCommand.commandRunWrapper(logger_m, configurator_m.setAlgBrkPairProperty(smartcom_alg_brk_pair, algBrkProp));
	HtExtServerCommand.commandStringWrapper(logger_m, "Created algorithm/broker entry: " + smartcom_alg_brk_pair);



  }

  public void initRtProvider() throws Exception {
	// check if providers are running and stop them


	String cpp_server_name = (PAR_IS_DEBUG_CONFIGURATION ? PAR_CPP_SERVER_NAME + "_d" : PAR_CPP_SERVER_NAME);
	String smartcom_alg_brk_pair = (PAR_IS_DEBUG_CONFIGURATION ? PAR_SMARTCOM_ALG_BRK_PAIR + "_d" : PAR_SMARTCOM_ALG_BRK_PAIR);


	// create RT provider
	HtRTProviderProp rtProp = new HtRTProviderProp();
	rtProp.setHourShift(0);
	rtProp.setAlien(false);
	rtProp.setSignDigits(-1);
	rtProp.setProviderClass(PAR_RT_PROVIDER_CLASS);

	rtProp.getParameters().put("SYMBOL_FILE", PAR_SYMBOL_FILE);
	rtProp.getParameters().put("SMARTCOM_SERVER_ID", cpp_server_name);
	rtProp.getParameters().put("SMARTCOM_ALG_BRK_PAIR", smartcom_alg_brk_pair);
	rtProp.getParameters().put("REQUEST_TICK_HISTORY_COUNT", "-1");
	rtProp.getParameters().put("REQUEST_TICK_HISTORY_FROM_DATETIME", "");
	rtProp.getParameters().put("REQUEST_BARS_HISTORY_COUNT", "-10000");

	SimpleDateFormat fmt = new SimpleDateFormat("dd-MM-yyyy HH:mm");
	fmt.setTimeZone(TimeZone.getTimeZone("GMT-0"));

	
	rtProp.getParameters().put("REQUEST_BARS_HISTORY_FROM_DATETIME", fmt.format(HtDateTimeUtils.getCurGmtTime_DayBegin() - 1000 * 60 * 60 * 24)); // yesterday date
	rtProp.getParameters().put("REQUEST_BARS_HISTORY_AGGR_PERIOD", PAR_REQUEST_BARS_HISTORY_AGGR_PERIOD);
	rtProp.getParameters().put("REQUEST_BARS_HISTORY_MULT_FACTOR", String.valueOf( PAR_REQUEST_BARS_HISTORY_MULTFACTOR ));
	rtProp.getParameters().put("SUBSCRIBE_PRINTS", "true");
	rtProp.getParameters().put("SUBSCRIBE_LEVEL1", "true");
	rtProp.getParameters().put("SUBSCRIBE_LEVEL2", "true");
	rtProp.getParameters().put("REQUEST_HISTORY_TICKS", "false");
	rtProp.getParameters().put("REQUEST_HISTORY_BARS", "true");
	rtProp.getParameters().put("BROKER_SEQ_NUM", String.valueOf( PAR_BROKER_SEQ_NUM ));
	rtProp.getParameters().put("REQUEST_MY_TRADES_POLL_SEC", String.valueOf(PAR_REQUEST_MY_TRADES_POLL_SEC));


	HtExtServerCommand.commandRunWrapper(logger_m, configurator_m.setRtProviderProperty(PAR_RT_PROVIDER, rtProp));
	HtExtServerCommand.commandStringWrapper(logger_m, "Created RT Provider entry: " + PAR_RT_PROVIDER);



	// create alien
	HtRTProviderProp rtProp_bar = new HtRTProviderProp();
	rtProp_bar.setAlien(true);
	HtExtServerCommand.commandRunWrapper(logger_m, configurator_m.setRtProviderProperty(PAR_RT_PROVIDER_BAR, rtProp_bar));
	HtExtServerCommand.commandStringWrapper(logger_m, "Created RT Provider entry: " + PAR_RT_PROVIDER_BAR);

  }

  public void initHistoryProvider() throws Exception {
	HtHistoryProviderProp hstProp = new HtHistoryProviderProp();
	hstProp.setExportHourShift(0);
	hstProp.setImportHourShift(0);
	hstProp.setProviderClass(PAR_HST_PROVIDER_CLASS);
	hstProp.setPropertyName(PAR_HST_PROVIDER);


	hstProp.getParameters().put("IMPORT_FILE", "DUMMY");

	HtExtServerCommand.commandRunWrapper(logger_m, configurator_m.setHistoryProviderProperty(PAR_HST_PROVIDER, hstProp));
	HtExtServerCommand.commandStringWrapper(logger_m, "Created history provider entry: " + PAR_HST_PROVIDER);
  }

  /**
   *
   * Helpers
   */
  private String findExecutable(boolean isDebug) throws Exception {


	HashSet<File> execs = new HashSet<File>();
	HtExtServerCommand.commandRunWrapper(logger_m, configurator_m.getAllSrvExecutables(execs));


	for (Iterator<File> it = execs.iterator(); it.hasNext();) {
	  File file_i = it.next();
	  String file_pi = file_i.getAbsolutePath();

	  HtExtBoolBuffer res = new HtExtBoolBuffer();
	  configurator_m.isDebugVesrion(file_pi, res);

	  if (isDebug) {
		if (res.getBoolean()) {
		  return file_pi;
		}
	  } else {
		if (!res.getBoolean()) {
		  return file_pi;
		}
	  }

	}

	return null;
  }

  private String findDll(String name, boolean isDebug) throws Exception {
	String dll_name = null;

	HashSet<File> execs = new HashSet<File>();
	HtExtServerCommand.commandRunWrapper(logger_m, configurator_m.getAllDlls(execs));

	String name_l = name.toLowerCase();
	for (Iterator<File> it = execs.iterator(); it.hasNext();) {
	  File file_i = it.next();

	  if (file_i.getName().toLowerCase().indexOf(name_l) >= 0) {

		dll_name = file_i.getAbsolutePath();
		HtExtBoolBuffer res = new HtExtBoolBuffer();
		HtExtServerCommand.commandRunWrapper(logger_m, configurator_m.isDebugVesrion(dll_name, res));
		if (isDebug) {
		  if (res.getBoolean()) {
			return dll_name;
		  }
		} else {
		  if (!res.getBoolean()) {
			return dll_name;
		  }
		}



	  }

	}

	return null;
  }

  private void deleteDataProfile() throws Exception {
	StringBuilder res_str = new StringBuilder();
	LongParam resultVar = new LongParam();
	HtExtServerCommand.commandRunWrapperNoError(logger_m, configurator_m.removeDataProfileProperty(PAR_HISTORY_DATA_PROFILE, resultVar, res_str));
	logger_m.addStringEntry("Removed data profile: " + PAR_HISTORY_DATA_PROFILE);

  }

  void deleteCppServer() throws Exception {
	String cpp_server_name = (PAR_IS_DEBUG_CONFIGURATION ? PAR_CPP_SERVER_NAME + "_d" : PAR_CPP_SERVER_NAME);
	HtExtServerCommand.commandRunWrapperNoError(logger_m, configurator_m.removeTradingServerProperty(cpp_server_name));

	HtExtServerCommand.commandStringWrapper(logger_m, "Removed CPP server entry: " + cpp_server_name);

  }

  void deleteEventPligin() throws Exception {
	String event_plugin_name = PAR_EVENT_PLUGIN_NAME;

	HtExtServerCommand.commandRunWrapperNoError(logger_m, configurator_m.removeEventPluginProperty(event_plugin_name));
	HtExtServerCommand.commandStringWrapper(logger_m, "Removed event plugin entry: " + event_plugin_name);
  }

  public void deleteAlgLibPair() throws Exception {
	String smartcom_alg_brk_pair = (PAR_IS_DEBUG_CONFIGURATION ? PAR_SMARTCOM_ALG_BRK_PAIR + "_d" : PAR_SMARTCOM_ALG_BRK_PAIR);


	HtExtServerCommand.commandRunWrapperNoError(logger_m, configurator_m.removeAlgBrkPairProperty(smartcom_alg_brk_pair));
	HtExtServerCommand.commandStringWrapper(logger_m, "Removed alglib entry: " + smartcom_alg_brk_pair);
  }

  public void deleteRtprovider() throws Exception {

	HtExtServerCommand.commandRunWrapperNoError(logger_m, configurator_m.removeRtProviderProperty(PAR_RT_PROVIDER));
	HtExtServerCommand.commandStringWrapper(logger_m, "Removed RT Provider entry: " + PAR_RT_PROVIDER);

	HtExtServerCommand.commandRunWrapperNoError(logger_m, configurator_m.removeRtProviderProperty(PAR_RT_PROVIDER_BAR));
	HtExtServerCommand.commandStringWrapper(logger_m, "Removed RT Provider entry: " + PAR_RT_PROVIDER_BAR);



  }

  public void deleteHistoryProvider() throws Exception {
	HtExtServerCommand.commandRunWrapperNoError(logger_m, configurator_m.removeHistoryProviderProperty(PAR_HST_PROVIDER));
	HtExtServerCommand.commandStringWrapper(logger_m, "Removed history provider entry: " + PAR_HST_PROVIDER);
  }

  void startCppServer() throws Exception {
	final String cpp_server_name = (PAR_IS_DEBUG_CONFIGURATION ? PAR_CPP_SERVER_NAME + "_d" : PAR_CPP_SERVER_NAME);

	// stop if necessary
	final HtExtServerCommand srvCommand = new HtExtServerCommand(logger_m);


	// ---------------------------
	// start server


	// need to re-read server property
	HtExtPropertyHolder<HtServerProp> srvPropHolder = new HtExtPropertyHolder<HtServerProp>();
	HtExtServerCommand.commandRunWrapper(logger_m,configurator_m.getTradingServerProperty(cpp_server_name, srvPropHolder));

	Map<String, HtServerProxy.HtServerProxy_ProfileEntry> profProvSymbMap = new HashMap<String, HtServerProxy.HtServerProxy_ProfileEntry>();

	HashSet<String> rtProvidersToSubscribe = new HashSet<String>();
	rtProvidersToSubscribe.add(PAR_RT_PROVIDER_BAR);
	rtProvidersToSubscribe.add(PAR_RT_PROVIDER);


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

  void startEventPlugin() throws Exception {
	HtExtGlobalCommand globalcommand = new HtExtGlobalCommand(logger_m);

	HtExtPropertyHolder<HtEventPluginProp> eventPropHolder = new HtExtPropertyHolder<HtEventPluginProp>();
	HtExtServerCommand.commandRunWrapper(logger_m, configurator_m.getEventPluginProperty(PAR_EVENT_PLUGIN_NAME, eventPropHolder));


	// resolve something
	PAR_SIM_PROFILE = eventPropHolder.getProperty().getParameters().get("SIM_PROFILE");
	if (PAR_SIM_PROFILE == null || PAR_SIM_PROFILE.length() <= 0) {
	  throw new HtExtException("Invalid simulation profile name! Must exist 'SIM_PROFILE' in " + PAR_EVENT_PLUGIN_NAME);
	}

	HtExtServerCommand.commandRunWrapper(logger_m, globalcommand.startEventPlugin(PAR_EVENT_PLUGIN_NAME, eventPropHolder.getProperty().getPluginClass(), eventPropHolder.getProperty().getParameters()));
	HtExtServerCommand.commandStringWrapper(logger_m, "Started event plugin entry: " + PAR_EVENT_PLUGIN_NAME);
  }

  void startAlgBrkPair() throws Exception {
	Set<String> existing = new HashSet<String>();
	HtExtServerCommand.commandRunWrapper(logger_m, configurator_m.getAlgBrkPairList(existing));

	runName_m = getRunNameBase() + "_run_" + HtDateTimeUtils.time2SimpleString_Gmt(HtDateTimeUtils.getCurGmtTime());
	runNameLoad_m = getRunNameBase() + "_init_" + HtDateTimeUtils.time2SimpleString_Gmt(HtDateTimeUtils.getCurGmtTime());

	String comment = "starting from script: " + this.getRunName();



	// ------------------
	// load

	HtExtPropertyHolder<HtAlgBrkPairProp> algbrkPropHolder = new HtExtPropertyHolder<HtAlgBrkPairProp>();
	HtExtServerCommand.commandRunWrapper(logger_m, configurator_m.getAlgBrkPairProperty(PAR_SMARTCOM_ALG_BRK_PAIR, algbrkPropHolder));

	HtExtServerCommand srvCommand = new HtExtServerCommand(logger_m);


	HtExtServerCommand.commandRunWrapper(logger_m, srvCommand.joinTradingServer(PAR_CPP_SERVER_NAME));
	HtExtServerCommand.commandRunWrapper(logger_m,
			srvCommand.loadAlgBrkPair(
			PAR_SMARTCOM_ALG_BRK_PAIR,
			runNameLoad_m,
			comment,
			1,
			algbrkPropHolder.getProperty().getAlgorithmPath(),
			algbrkPropHolder.getProperty().getBrokerPath(),
			algbrkPropHolder.getProperty().getBrokerPath2(),
			algbrkPropHolder.getProperty().getAlgoritmParams(),
			algbrkPropHolder.getProperty().getBrokerParams(),
			algbrkPropHolder.getProperty().getBrokerParams2()));


	HtExtServerCommand.commandStringWrapper(logger_m, "Started algorithm/broker entry: " + PAR_SMARTCOM_ALG_BRK_PAIR);

	// start thread


	HtExtIntBuffer threadId = new HtExtIntBuffer();
	HtExtServerCommand.commandRunWrapper(logger_m, srvCommand.startThread(Arrays.asList(PAR_SMARTCOM_ALG_BRK_PAIR), threadId));
	HtExtServerCommand.commandStringWrapper(logger_m, "Started thread: " + threadId.getInt());

	threadId_m = threadId.getInt();
  }

  // this just waits when smartcom is connected
  private void expectSmartComConnected() throws Exception {
	logger_m.addStringEntry("Waiting SMARTCOM connect status for: " + SMARTCOM_CONNECT_TOUT_SEC + " sec(s)...");



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
				if (server_id.equalsIgnoreCase(PAR_CPP_SERVER_NAME) && rt_provider.equalsIgnoreCase(PAR_RT_PROVIDER)) {
				  to_stop = true;
				}
			  }
			}
		  }

		  if (tcalc.elapsed() > SMARTCOM_CONNECT_TOUT_SEC * 1000) {
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

  public void startRtProvider() throws Exception {
	// check if providers are running and stop them

	HtExtGlobalCommand globalcommand = new HtExtGlobalCommand(logger_m);


	// start RT_PROVIDER
	HtExtPropertyHolder<HtRTProviderProp> rtProp3 = new HtExtPropertyHolder<HtRTProviderProp>();
	HtExtServerCommand.commandRunWrapper(logger_m, configurator_m.getRtProviderProperty(PAR_RT_PROVIDER, rtProp3));
	HtExtServerCommand.commandRunWrapper(logger_m,
			globalcommand.startRtProvider(
			PAR_RT_PROVIDER,
			rtProp3.getProperty().getProviderClass(),
			rtProp3.getProperty().getParameters(),
			rtProp3.getProperty().getHourShift()));

	logger_m.addStringEntry("RT Provider started: " + PAR_RT_PROVIDER);
  }

  // here we will be waiting the confirmation whn history data are ready!!!
  private void expectReadHistoryMessage() throws Exception {
	logger_m.addStringEntry("Waiting SMARTCOM read history  for: " + SMARTCOM_EXPORTDATA_TOUT_SEC + " sec(s)...");
	
	final ArrayList list = new ArrayList();


	final LongParam exp_count = new LongParam();
	exp_count.setLong(0);
	

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
				if (server_id.equalsIgnoreCase(PAR_CPP_SERVER_NAME) && rt_provider.equalsIgnoreCase(PAR_RT_PROVIDER)) {
				  String message = returned.getAsXmlParameter().getString("message");

				  XmlParameter xmlparam = new XmlParameter();
				  HumanSerializer.deserializeXmlParameter(message, xmlparam);

				  String msg_type = xmlparam.getString("MSG");
				  String symbol = xmlparam.getString("SYMBOL");
				  

				  if (msg_type.equalsIgnoreCase("HISTORY_SAVED")) {
					String file_uid = xmlparam.getString("UID");
					Uid uid = new Uid(file_uid);
					exp_count.setLong( exp_count.getLong() + 1);
					try {
					  performHistoryExport(uid, symbol);
					}
					catch(Throwable e)
					{
					  try {
						logger_m.addErrorEntry("When importing data for symbol: "+symbol+" using uid: "+ uid + ", exception happend: " + e.getMessage() );
					  }
					  catch(Throwable e2)
					  {
					  }
					  
					  return;
					}
					
					if (exp_count.getLong() == symbols_m.size())
					  return;
				  }
				  else if (msg_type.equalsIgnoreCase("HISTORY_NOT_SAVED")) {
					try {
					  logger_m.addErrorEntry("When importing data for symbol: " + symbol +" SMARTCOM returned failed status"); 
					}
					catch(Throwable e2)
					{  
					}
					
					return;
				  }
				}
			  }
			}
		  }

		  if (tcalc.elapsed() > SMARTCOM_EXPORTDATA_TOUT_SEC * 1000) {
			break;
		  }


		} // end loop

		

	  }
	};

	runnable.start();
	runnable.join();


	if (exp_count.getLong() == symbols_m.size()) {
	  logger_m.addStringEntry("Success - history read OK!");
	} else {
	  throw new HtExtException("History read confirmations events was not fully arrived from SMARTCOM layer!");

	}

	
  }

  public void performHistoryExport(Uid fileUid, String symbol) throws Exception {
	// here we shall perform history export of history
	HtExtGlobalCommand globalcommand = new HtExtGlobalCommand(logger_m);
	Set<String> existinghistoryProviders = new HashSet<String>();
	HtExtServerCommand.commandRunWrapper(logger_m, configurator_m.getRegisteredHistoryProviderList(existinghistoryProviders));

	// history provider will be always recreated
	HtHistoryProviderProp hstProp = new HtHistoryProviderProp();
	hstProp.setExportHourShift(0);
	hstProp.setImportHourShift(0);
	hstProp.setProviderClass(PAR_HST_PROVIDER_CLASS);
	hstProp.setPropertyName(PAR_HST_PROVIDER);
	

	String fileName = HtSystem.getSystemTempPath() + File.separator + "smartcom_tmp" + File.separator + fileUid.toString() + ".tmpcache";
	File f = new File(fileName);
	if (!f.exists()) 
	  throw new HtExtException("Export data file: " + fileName + " not found");
	
	
	if (symbol.length() == 0)
	   throw new HtExtException("Invalid symbol!");

	hstProp.getParameters().put("IMPORT_FILE", fileName);
	hstProp.getParameters().put("SYMBOL", symbol);

	HtExtServerCommand.commandRunWrapper(logger_m, configurator_m.setHistoryProviderProperty(PAR_HST_PROVIDER, hstProp));
	HtExtServerCommand.commandStringWrapper(logger_m, "Forcibly created history provider entry: " + PAR_HST_PROVIDER);

	String import_id = this.getRunNameBase() + HtDateTimeUtils.getCurGmtTime();
	HtExtServerCommand.commandRunWrapper(logger_m,
			globalcommand.importData(
			import_id,
			PAR_HISTORY_DATA_PROFILE,
			HtDatabaseProxy.IMPORT_FLAG_OVERWRITE,
			PAR_RT_PROVIDER_BAR,
			hstProp.getPropertyName(),
			hstProp.getProviderClass(),
			hstProp.getParameters(),
			-1,
			-1,
			0,
			false,
			true));


	logger_m.addStringEntry("Performed history import targeting to RT provider: '" + PAR_RT_PROVIDER_BAR
			+ "' using history provider: '" + hstProp.getPropertyName()
			+ "' wrote to profile: " + PAR_HISTORY_DATA_PROFILE + "'" +
			", symbol: " + symbol +
			", file uid: " + fileUid);
  }

  public void sendThreadMessages() throws Exception {
	if (threadId_m < 0) {
	  throw new HtExtException("Invalid thread ID, probably thread is not started");
	}

	String comment = "";
	if (runName_m == null) {
	  throw new HtExtException("Invalid run name, probably algorithm/broker pair was not loaded");
	}


	HtExtServerCommand srvCommand = new HtExtServerCommand(logger_m);
	HtExtServerCommand.commandRunWrapper(logger_m, srvCommand.joinTradingServer(PAR_CPP_SERVER_NAME));

	StringBuilder out = new StringBuilder();
	HtExtServerCommand.commandRunWrapper(logger_m, srvCommand.sendCustomEventToAlgorithm(
			PAR_CPP_SERVER_NAME,
			PAR_SMARTCOM_ALG_BRK_PAIR,
			threadId_m,
			"load_data",
			"",
			ALG_MSG_TOUT_SEC,
			out));

	if (out.toString().indexOf("OK") < 0) {
	  throw new HtExtException("Event failed to algorithm: " + PAR_SMARTCOM_ALG_BRK_PAIR + " returned message: " + out.toString());
	}

	HtExtServerCommand.commandStringWrapper(logger_m, "Loaded data");



	// SIM_PROFILE_NAME
	if (PAR_SIM_PROFILE.length() <= 0) {
	  throw new HtExtException("Invalid simulation profile name");
	} else {
	  logger_m.addStringEntry("Use simulation profile: [" + PAR_SIM_PROFILE + "] for RT activity");
	}

	HtExtServerCommand.commandRunWrapper(logger_m, srvCommand.startRtActivity(threadId_m, runName_m, comment, PAR_SIM_PROFILE));
	HtExtServerCommand.commandStringWrapper(logger_m, "Started RT activity");
  }

  @Override
  public void onEventArrived(XmlEvent event) throws SingleReaderQueueExceedLimit {
	eventsArrived_m.put(event);
  }

  @Override
  public String getListenerName() {
	return " listener: [ SMARTCOM temporary listener ] ";
  }
}
