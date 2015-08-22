/*
 * HtExtRun_importSymbols.java
 *
 * Created on 15 ¿ÔÂÎ¸ 2009 „., 11:20
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.externalapi.custom;

import com.fin.httrader.HtMain;
import com.fin.httrader.configprops.HtAlgBrkPairProp;
import com.fin.httrader.configprops.HtHistoryProviderProp;
import com.fin.httrader.configprops.HtServerProp;
import com.fin.httrader.externalapi.HtExtConfiguration;
import com.fin.httrader.externalapi.HtExtGlobalCommand;
import com.fin.httrader.externalapi.HtExtIntBuffer;
import com.fin.httrader.externalapi.HtExtLogger;
import com.fin.httrader.externalapi.HtExtPropertyHolder;
import com.fin.httrader.externalapi.HtExtRunner;
import com.fin.httrader.externalapi.HtExtServerCommand;
import com.fin.httrader.managers.RtConfigurationManager;
import com.fin.httrader.model.HtDatabaseProxy;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.hlpstruct.HtPair;
import java.util.Calendar;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Vector;

/**
 *
 * @author Victor_Zoubok
 */
public class HtExtRun_importSymbols implements HtExtRunner {

	/** Creates a new instance of HtExtRun_importSymbols */
	public HtExtRun_importSymbols() {
	}

	public void run(HtExtLogger logger) throws Exception {
		logger.addStringEntry("Will start importing 14 symbols");


		long globalId = Calendar.getInstance().getTimeInMillis();
		//String profileName = "dpmt2";

		String profileName = "broko";
		int importFlag = HtDatabaseProxy.IMPORT_FLAG_OVERWRITE;
		String rtProvider = "Broko MT Provider";
		//String rtProvider = "RT Demo Provider 1";

		String serverId = "server 1";
		String algBrokerPair = "t101";

		//String serverId = "server 1_debug";
		//String algBrokerPair = "t101_d";


		//------------------------------------------------

		String runName = "T101_RUN_" + String.valueOf(globalId);

		//long beginExport = -1;
		long beginExport = HtUtils.parseDateTimeParameter("01-03-2009 00:00", false);
		long endExport = -1;

		logger.addStringEntry("RT provider: " + rtProvider);
		logger.addStringEntry("Profile: " + profileName);
		logger.addStringEntry("Begin date: " + HtUtils.time2String_Gmt(beginExport));

		HashSet<String> rtProvidersToSubscribe = new HashSet<String>();
		rtProvidersToSubscribe.add(rtProvider);


		// ----------------------------------------------------------


		HashMap<String, Vector<HtPair<String, String>>> profProvSymbMap = new HashMap<String, Vector<HtPair<String, String>>>();

		HtExtConfiguration configurator = new HtExtConfiguration();
		HtHistoryProviderProp[] props = new HtHistoryProviderProp[14];

		// get properties
		// need to perform a series of exports
		HtExtPropertyHolder<HtHistoryProviderProp> prop_GBPUSD = new HtExtPropertyHolder<HtHistoryProviderProp>();
		HtExtServerCommand.commandRunWrapper(logger, configurator.getHistoryProviderProperty("MT_Broko_Hist_GBPUSD", prop_GBPUSD));
		configurator.addToProfileProviderSymbolList(profProvSymbMap, profileName, rtProvider, "GBPUSD");
		props[0] = prop_GBPUSD.p;

		HtExtPropertyHolder<HtHistoryProviderProp> prop_EURGBP = new HtExtPropertyHolder<HtHistoryProviderProp>();
		HtExtServerCommand.commandRunWrapper(logger, configurator.getHistoryProviderProperty("MT_Broko_Hist_EURGBP", prop_EURGBP));
		configurator.addToProfileProviderSymbolList(profProvSymbMap, profileName, rtProvider, "EURGBP");
		props[1] = prop_EURGBP.p;

		HtExtPropertyHolder<HtHistoryProviderProp> prop_GBPCHF = new HtExtPropertyHolder<HtHistoryProviderProp>();
		HtExtServerCommand.commandRunWrapper(logger, configurator.getHistoryProviderProperty("MT_Broko_Hist_GBPCHF", prop_GBPCHF));
		configurator.addToProfileProviderSymbolList(profProvSymbMap, profileName, rtProvider, "GBPCHF");
		props[2] = prop_GBPCHF.p;

		HtExtPropertyHolder<HtHistoryProviderProp> prop_CHFJPY = new HtExtPropertyHolder<HtHistoryProviderProp>();
		HtExtServerCommand.commandRunWrapper(logger, configurator.getHistoryProviderProperty("MT_Broko_Hist_CHFJPY", prop_CHFJPY));
		configurator.addToProfileProviderSymbolList(profProvSymbMap, profileName, rtProvider, "CHFJPY");
		props[3] = prop_CHFJPY.p;

		HtExtPropertyHolder<HtHistoryProviderProp> prop_AUDJPY = new HtExtPropertyHolder<HtHistoryProviderProp>();
		HtExtServerCommand.commandRunWrapper(logger, configurator.getHistoryProviderProperty("MT_Broko_Hist_AUDJPY", prop_AUDJPY));
		configurator.addToProfileProviderSymbolList(profProvSymbMap, profileName, rtProvider, "AUDJPY");
		props[4] = prop_AUDJPY.p;

		HtExtPropertyHolder<HtHistoryProviderProp> prop_EURJPY = new HtExtPropertyHolder<HtHistoryProviderProp>();
		HtExtServerCommand.commandRunWrapper(logger, configurator.getHistoryProviderProperty("MT_Broko_Hist_EURJPY", prop_EURJPY));
		configurator.addToProfileProviderSymbolList(profProvSymbMap, profileName, rtProvider, "EURJPY");
		props[5] = prop_EURJPY.p;

		HtExtPropertyHolder<HtHistoryProviderProp> prop_USDCHF = new HtExtPropertyHolder<HtHistoryProviderProp>();
		HtExtServerCommand.commandRunWrapper(logger, configurator.getHistoryProviderProperty("MT_Broko_Hist_USDCHF", prop_USDCHF));
		configurator.addToProfileProviderSymbolList(profProvSymbMap, profileName, rtProvider, "USDCHF");
		props[6] = prop_USDCHF.p;

		HtExtPropertyHolder<HtHistoryProviderProp> prop_CADJPY = new HtExtPropertyHolder<HtHistoryProviderProp>();
		HtExtServerCommand.commandRunWrapper(logger, configurator.getHistoryProviderProperty("MT_Broko_Hist_CADJPY", prop_CADJPY));
		configurator.addToProfileProviderSymbolList(profProvSymbMap, profileName, rtProvider, "CADJPY");
		props[7] = prop_CADJPY.p;

		HtExtPropertyHolder<HtHistoryProviderProp> prop_AUDUSD = new HtExtPropertyHolder<HtHistoryProviderProp>();
		HtExtServerCommand.commandRunWrapper(logger, configurator.getHistoryProviderProperty("MT_Broko_Hist_AUDUSD", prop_AUDUSD));
		configurator.addToProfileProviderSymbolList(profProvSymbMap, profileName, rtProvider, "AUDUSD");
		props[8] = prop_AUDUSD.p;

		HtExtPropertyHolder<HtHistoryProviderProp> prop_USDJPY = new HtExtPropertyHolder<HtHistoryProviderProp>();
		HtExtServerCommand.commandRunWrapper(logger, configurator.getHistoryProviderProperty("MT_Broko_Hist_USDJPY", prop_USDJPY));
		configurator.addToProfileProviderSymbolList(profProvSymbMap, profileName, rtProvider, "USDJPY");
		props[9] = prop_USDJPY.p;

		HtExtPropertyHolder<HtHistoryProviderProp> prop_EURUSD = new HtExtPropertyHolder<HtHistoryProviderProp>();
		HtExtServerCommand.commandRunWrapper(logger, configurator.getHistoryProviderProperty("MT_Broko_Hist_EURUSD", prop_EURUSD));
		configurator.addToProfileProviderSymbolList(profProvSymbMap, profileName, rtProvider, "EURUSD");
		props[10] = prop_EURUSD.p;

		HtExtPropertyHolder<HtHistoryProviderProp> prop_EURCHF = new HtExtPropertyHolder<HtHistoryProviderProp>();
		HtExtServerCommand.commandRunWrapper(logger, configurator.getHistoryProviderProperty("MT_Broko_Hist_EURCHF", prop_EURCHF));
		configurator.addToProfileProviderSymbolList(profProvSymbMap, profileName, rtProvider, "EURCHF");
		props[11] = prop_EURCHF.p;

		HtExtPropertyHolder<HtHistoryProviderProp> prop_GBPJPY = new HtExtPropertyHolder<HtHistoryProviderProp>();
		HtExtServerCommand.commandRunWrapper(logger, configurator.getHistoryProviderProperty("MT_Broko_Hist_GBPJPY", prop_GBPJPY));
		configurator.addToProfileProviderSymbolList(profProvSymbMap, profileName, rtProvider, "GBPJPY");
		props[12] = prop_GBPJPY.p;

		HtExtPropertyHolder<HtHistoryProviderProp> prop_USDCAD = new HtExtPropertyHolder<HtHistoryProviderProp>();
		HtExtServerCommand.commandRunWrapper(logger, configurator.getHistoryProviderProperty("MT_Broko_Hist_USDCAD", prop_USDCAD));
		configurator.addToProfileProviderSymbolList(profProvSymbMap, profileName, rtProvider, "USDCAD");
		props[13] = prop_USDCAD.p;

		// ------------------------------------
		//
		//
		HtExtGlobalCommand globalcmd = new HtExtGlobalCommand(logger);
		logger.addStringEntry("Import range: " + HtUtils.time2SimpleString_Gmt(beginExport) + " - " + HtUtils.time2SimpleString_Gmt(endExport));

		for (int i = 0; i < props.length; i++) {
			String importId = String.valueOf(globalId) + props[i].getPropertyName();
			logger.addStringEntry("Will import with id: " + importId);

			HtExtServerCommand.commandRunWrapper(logger, globalcmd.importData(
					importId,
					profileName, // at what profile we are inserting
					importFlag,
					rtProvider,
					props[i].getPropertyName(),
					props[i].providerClass_m.toString(),
					props[i].initParams_m,
					beginExport,
					endExport,
					0,
					false));
		}

		//
		logger.addStringEntry("Finished importing 14 symbols");

		
		// startint server providing some data as input
		logger.addStringEntry("Will start trading server: " + serverId);

		HtExtServerCommand srv—ommand = new HtExtServerCommand(logger);

		HtExtPropertyHolder<HtServerProp> serverProp = new HtExtPropertyHolder<HtServerProp>();
		HtExtServerCommand.commandRunWrapper(logger, configurator.getTradingServerProperty(serverId, serverProp));
		Vector<String> execEnviroment = new Vector<String>();
		HtExtServerCommand.commandRunWrapper(logger, configurator.getTradingServerEnviromentEntries(execEnviroment));

		HtExtServerCommand.commandRunWrapper(logger, srv—ommand.startTradingServer(
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

		HtExtPropertyHolder<HtAlgBrkPairProp> algBrkProp = new HtExtPropertyHolder<HtAlgBrkPairProp>();
		HtExtServerCommand.commandRunWrapper(logger, configurator.getAlgBrkPairProperty(algBrokerPair, algBrkProp));

		HtExtServerCommand.commandRunWrapper(logger,
				srv—ommand.loadAlgBrkPair(
				algBrokerPair,
				runName,
				runName + " - comment",
				1,
				algBrkProp.p.algorithmPath_m.toString(),
				algBrkProp.p.brokerPath_m.toString(),
				algBrkProp.p.algParams_m,
				algBrkProp.p.brokerParams_m));

		logger.addStringEntry("Will start thread for: " + serverId + " - " + algBrokerPair);
		HtExtIntBuffer threadId = new HtExtIntBuffer();

		Vector<String> algpairs = new Vector<String>();
		algpairs.add(algBrokerPair);
		HtExtServerCommand.commandRunWrapper(logger, srv—ommand.startThread(algpairs, threadId));

		logger.addStringEntry("Will start RT activity for thread for thread: " + threadId.getInt());
		HtExtServerCommand.commandRunWrapper(logger, srv—ommand.startRtActivity(threadId.getInt(), runName, runName + " - comment", null));

		logger.addStringEntry("Finished startup completely");


	}

	public String getRunName() {
		return "import_14_symbols";
	}
}
