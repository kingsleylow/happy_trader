/*
 * HtExtRun1.java
 *
 * Created on 20 Сентябрь 2008 г., 12:25
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.externalapi.custom;

import com.fin.httrader.configprops.HtHistoryProviderProp;
import com.fin.httrader.externalapi.*;
import com.fin.httrader.model.HtDatabaseProxy;
import com.fin.httrader.utils.hlpstruct.HtPair;
import com.fin.httrader.utils.HtUtils;
import java.util.Calendar;
import java.util.Vector;

/**
 *
 * @author Victor_Zoubok
 */
public class HtExtRun1 implements HtExtRunner {

	// ----------------------------
	public void HtExtRun1() {
	}

	// ----------------------------
	public void run(HtExtLogger logger) throws Exception {
		logger.addStringEntry("Will perform import data from MT4");

		long globalId = Calendar.getInstance().getTimeInMillis();
		String profileName = "dpmt";
		int importFlag = HtDatabaseProxy.IMPORT_FLAG_OVERWRITE;
		String rtProvider = "MT Provider";

		long beginExport = HtUtils.parseDateTimeParameter("01-10-2008 00:00", false);
		long endExport = -1;


		HtExtConfiguration configurator = new HtExtConfiguration();

		// need to perform a series of exports
		HtExtPropertyHolder<HtHistoryProviderProp> prop_EURCHF = new HtExtPropertyHolder<HtHistoryProviderProp>();
		HtExtServerCommand.commandRunWrapper(logger, configurator.getHistoryProviderProperty("MT_Custom_Hist_EURCHF", prop_EURCHF));

		HtExtPropertyHolder<HtHistoryProviderProp> prop_EURJPY = new HtExtPropertyHolder<HtHistoryProviderProp>();
		HtExtServerCommand.commandRunWrapper(logger, configurator.getHistoryProviderProperty("MT_Custom_Hist_EURJPY", prop_EURJPY));

		HtExtPropertyHolder<HtHistoryProviderProp> prop_EURUSD = new HtExtPropertyHolder<HtHistoryProviderProp>();
		HtExtServerCommand.commandRunWrapper(logger, configurator.getHistoryProviderProperty("MT_Custom_Hist_EURUSD", prop_EURUSD));


		HtExtPropertyHolder<HtHistoryProviderProp> prop_GBPUSD = new HtExtPropertyHolder<HtHistoryProviderProp>();
		HtExtServerCommand.commandRunWrapper(logger, configurator.getHistoryProviderProperty("MT_Custom_Hist_GBPUSD", prop_GBPUSD));


		HtExtPropertyHolder<HtHistoryProviderProp> prop_USDCHF = new HtExtPropertyHolder<HtHistoryProviderProp>();
		HtExtServerCommand.commandRunWrapper(logger, configurator.getHistoryProviderProperty("MT_Custom_Hist_USDCHF", prop_USDCHF));

		HtExtPropertyHolder<HtHistoryProviderProp> prop_USDJPY = new HtExtPropertyHolder<HtHistoryProviderProp>();
		HtExtServerCommand.commandRunWrapper(logger, configurator.getHistoryProviderProperty("MT_Custom_Hist_USDJPY", prop_USDJPY));

		// ------------------------------------
		HtExtGlobalCommand globalcmd = new HtExtGlobalCommand(logger);

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
						false));


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
						false));

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
						false));

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
						false));


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
						false));


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
						false));

		logger.addStringEntry("Finished import operation");

	}

	public String getRunName() {
		return "run_1";
	}
}
