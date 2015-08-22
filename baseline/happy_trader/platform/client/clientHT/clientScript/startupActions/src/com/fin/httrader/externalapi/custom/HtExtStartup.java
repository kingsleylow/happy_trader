/*
 * HtExtStartup.java
 *
 * Created on 20 Ñåíòÿáðü 2008 ã., 12:25
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package com.fin.httrader.externalapi.custom;


import com.fin.httrader.HtMain;
import com.fin.httrader.configprops.HtAlgBrkPairProp;
import com.fin.httrader.configprops.HtEventPluginProp;
import com.fin.httrader.configprops.HtHistoryProviderProp;
import com.fin.httrader.configprops.HtRTProviderProp;
import com.fin.httrader.configprops.HtServerProp;
import com.fin.httrader.externalapi.*;
import com.fin.httrader.managers.RtConfigurationManager;
import com.fin.httrader.model.HtDatabaseProxy;
import com.fin.httrader.utils.hlpstruct.HtPair;
import com.fin.httrader.utils.HtUtils;
import java.util.Calendar;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Vector;

/**
 *
 * @author Victor_Zoubok
 */
public class HtExtStartup implements HtExtRunner{
    
    // ----------------------------
    public void HtExtStartup() {
    }
    
    // ----------------------------
    
    public void run(HtExtLogger logger) throws Exception {
	// ----------------------------------------------------------------------
	// Parameters
	//
	long globalId = Calendar.getInstance().getTimeInMillis();
	String profileName = "dpmt";
	int importFlag = HtDatabaseProxy.IMPORT_FLAG_OVERWRITE;
	String rtProvider = "MT Provider";
	String algBrokerPair = "dummy_day_trade";
	String runName = String.valueOf(globalId) + "_RUN";
	
	long beginExport = HtUtils.parseDateTimeParameter("10-10-2008 00:00", false);
	long endExport = -1;
	
	String pluginId = "plugin 1";
	
	////
	String serverId = "server 1";
	
	// symbols for server
	HashMap< String, Vector<HtPair<String, String> > > profProvSymbMap = new HashMap< String, Vector<HtPair<String, String> > >();
	HashSet<String> rtProvidersToSubscribe = new HashSet<String>();
	rtProvidersToSubscribe.add(rtProvider);
	
	
	// ----------------------------------------------------------------------
	//
	//
	logger.addStringEntry("Will perform import data from MT4");
	
	
	HtExtConfiguration configurator = new HtExtConfiguration();
	
	// need to perform a series of exports
	HtExtPropertyHolder<HtHistoryProviderProp> prop_EURCHF = new HtExtPropertyHolder<HtHistoryProviderProp>();
	HtExtServerCommand.commandRunWrapper(logger, configurator.getHistoryProviderProperty("MT_Custom_Hist_EURCHF", prop_EURCHF ));
	configurator.addToProfileProviderSymbolList(profProvSymbMap, profileName, rtProvider, "EURCHF");
	
	HtExtPropertyHolder<HtHistoryProviderProp> prop_EURJPY = new HtExtPropertyHolder<HtHistoryProviderProp>();
	HtExtServerCommand.commandRunWrapper(logger, configurator.getHistoryProviderProperty("MT_Custom_Hist_EURJPY", prop_EURJPY ));
	configurator.addToProfileProviderSymbolList(profProvSymbMap, profileName, rtProvider, "EURJPY");
	
		HtExtPropertyHolder<HtHistoryProviderProp> prop_EURUSD = new HtExtPropertyHolder<HtHistoryProviderProp>();
	HtExtServerCommand.commandRunWrapper(logger, configurator.getHistoryProviderProperty("MT_Custom_Hist_EURUSD", prop_EURUSD ));
	configurator.addToProfileProviderSymbolList(profProvSymbMap, profileName, rtProvider, "EURUSD");
	
	
		HtExtPropertyHolder<HtHistoryProviderProp> prop_GBPUSD = new HtExtPropertyHolder<HtHistoryProviderProp>();
	HtExtServerCommand.commandRunWrapper(logger, configurator.getHistoryProviderProperty("MT_Custom_Hist_GBPUSD", prop_GBPUSD ));
	configurator.addToProfileProviderSymbolList(profProvSymbMap, profileName, rtProvider, "GBPUSD");
	
	
		HtExtPropertyHolder<HtHistoryProviderProp> prop_USDCHF = new HtExtPropertyHolder<HtHistoryProviderProp>();
	HtExtServerCommand.commandRunWrapper(logger, configurator.getHistoryProviderProperty("MT_Custom_Hist_USDCHF", prop_USDCHF ));
	configurator.addToProfileProviderSymbolList(profProvSymbMap, profileName, rtProvider, "USDCHF");
	
		HtExtPropertyHolder<HtHistoryProviderProp> prop_USDJPY = new HtExtPropertyHolder<HtHistoryProviderProp>();
	HtExtServerCommand.commandRunWrapper(logger, configurator.getHistoryProviderProperty("MT_Custom_Hist_USDJPY", prop_USDJPY ));
	configurator.addToProfileProviderSymbolList(profProvSymbMap, profileName, rtProvider, "USDJPY");
	
	
	// ------------------------------------
	HtExtGlobalCommand  globalcmd = new HtExtGlobalCommand(logger);
	
	logger.addStringEntry("Import range: " + HtUtils.time2SimpleString_Gmt(beginExport) + " - " + HtUtils.time2SimpleString_Gmt(endExport));
	
	String importId = globalId + "EURCHF";
	logger.addStringEntry("Will import with id: " + importId);
	
	HtExtServerCommand.commandRunWrapper(logger, globalcmd.importData(
	    importId,
	    profileName, // at what profile we are inserting
	    importFlag,
	    rtProvider,
	    prop_EURCHF.p.getPropertyName(),
	    prop_EURCHF.p.providerClass_m.toString(),
	    prop_EURCHF.p.initParams_m,
	    beginExport,
	    endExport,
	    0,
	    false )
	    );
	
	
	importId = globalId + "EURJPY";
	logger.addStringEntry("Will import with id: " + importId);
	HtExtServerCommand.commandRunWrapper(logger, globalcmd.importData(
	    importId,
	    profileName, // at what profile we are inserting
	    importFlag,
	    rtProvider,
	    prop_EURJPY.p.getPropertyName(),
	    prop_EURJPY.p.providerClass_m.toString(),
	    prop_EURJPY.p.initParams_m,
	    beginExport,
	    endExport,
	    0,
	    false )
	    );
	
	importId = globalId + "EURUSD";
	logger.addStringEntry("Will import with id: " + importId);
	HtExtServerCommand.commandRunWrapper(logger, globalcmd.importData(
	    importId,
	    profileName, // at what profile we are inserting
	    importFlag,
	    rtProvider,
	    prop_EURUSD.p.getPropertyName(),
	    prop_EURUSD.p.providerClass_m.toString(),
	    prop_EURUSD.p.initParams_m,
	    beginExport,
	    endExport,
	    0,
	    false )
	    );
	
	importId = globalId + "GBPUSD";
	logger.addStringEntry("Will import with id: " + importId);
	HtExtServerCommand.commandRunWrapper(logger, globalcmd.importData(
	    importId,
	    profileName, // at what profile we are inserting
	    importFlag,
	    rtProvider,
	    prop_GBPUSD.p.getPropertyName(),
	    prop_GBPUSD.p.providerClass_m.toString(),
	    prop_GBPUSD.p.initParams_m,
	    beginExport,
	    endExport,
	    0,
	    false )
	    );
	
	
	importId = globalId + "USDCHF";
	logger.addStringEntry("Will import with id: " + importId);
	HtExtServerCommand.commandRunWrapper(logger, globalcmd.importData(
	    importId,
	    profileName, // at what profile we are inserting
	    importFlag,
	    rtProvider,
	    prop_USDCHF.p.getPropertyName(),
	    prop_USDCHF.p.providerClass_m.toString(),
	    prop_USDCHF.p.initParams_m,
	    beginExport,
	    endExport,
	    0,
	    false )
	    );
	
	
	importId = globalId + "USDJPY";
	logger.addStringEntry("Will import with id: " + importId);
	HtExtServerCommand.commandRunWrapper(logger, globalcmd.importData(
	    importId,
	    profileName, // at what profile we are inserting
	    importFlag,
	    rtProvider,
	    prop_USDJPY.p.getPropertyName(),
	    prop_USDJPY.p.providerClass_m.toString(),
	    prop_USDJPY.p.initParams_m,
	    beginExport,
	    endExport,
	    0,
	    false )
	    );
	
	logger.addStringEntry("Will run Beeline event plugin");
	
	
	HtExtPropertyHolder<HtEventPluginProp> pluginProp = new HtExtPropertyHolder<HtEventPluginProp>();
	HtExtServerCommand.commandRunWrapper(logger, configurator.getEventPluginProperty(pluginId, pluginProp));
	HtExtServerCommand.commandRunWrapper(logger,globalcmd.startEventPlugin(pluginId, pluginProp.p.pluginClass_m.toString(), pluginProp.p.initParams_m));
	
	// startint server providing some data as input
	logger.addStringEntry("Will start trading server: " + serverId);
	HtExtServerCommand srvÑommand = new HtExtServerCommand( logger);
	
	HtExtPropertyHolder<HtServerProp> serverProp = new HtExtPropertyHolder<HtServerProp>();
	HtExtServerCommand.commandRunWrapper(logger,configurator.getTradingServerProperty(serverId, serverProp));
	Vector<String> execEnviroment = new Vector<String>();
	HtExtServerCommand.commandRunWrapper(logger, configurator.getTradingServerEnviromentEntries(execEnviroment));
	
	
	HtExtServerCommand.commandRunWrapper(logger, srvÑommand.startTradingServer(
	    serverId,
	    serverProp.p.serverExecutable_m.toString(),
	    execEnviroment,
	    serverProp.p.cmdPort_m,
	    true,
	    beginExport,
	    endExport,
	    profProvSymbMap, // map profile <-> ( provider - symbol
			rtProvidersToSubscribe,
	    RtConfigurationManager.StartUpConst.SELECT_PAGE_DEFAULT_SIZE, // make selections via this chunk
	    true,
	    serverProp.p.logLevel_m,
	    serverProp.p.subscribedEvents_m,
	    false));
	
	
	logger.addStringEntry("Will load alg-broker pair for: " + serverId + " - " + algBrokerPair);
	
	HtExtPropertyHolder<HtAlgBrkPairProp> algBrkProp = new  HtExtPropertyHolder<HtAlgBrkPairProp>();
	HtExtServerCommand.commandRunWrapper(logger, configurator.getAlgBrkPairProperty(algBrokerPair, algBrkProp ));

	
	HtExtServerCommand.commandRunWrapper(logger, 
	    srvÑommand.loadAlgBrkPair(
	    "dummy_day_trade", 
	    runName, 
	    "comment 6",
	    1, 
	    algBrkProp.p.algorithmPath_m.toString(),
	    algBrkProp.p.brokerPath_m.toString(),
	    algBrkProp.p.brokerParams_m,
	    algBrkProp.p.brokerParams_m)
	   );
	
	//
	logger.addStringEntry("Will start thread for: " + serverId + " - " + algBrokerPair);
	HtExtIntBuffer threadId = new HtExtIntBuffer();
	
	Vector<String> algpairs = new Vector<String>();
	algpairs.add(algBrokerPair);
	HtExtServerCommand.commandRunWrapper(logger,srvÑommand.startThread(algpairs, threadId));
	
	logger.addStringEntry("Will start RT activity for thread for thread: "+threadId.getInt() );
	HtExtServerCommand.commandRunWrapper(logger,srvÑommand.startRtActivity(threadId.getInt(), runName, "comment 7", null));
	
	
	logger.addStringEntry("Finished startup, second part");
	
    }
    
    public String getRunName() {
	return "startup";
    }
    
    // ----------------------------
    
    
    
    
    
}
