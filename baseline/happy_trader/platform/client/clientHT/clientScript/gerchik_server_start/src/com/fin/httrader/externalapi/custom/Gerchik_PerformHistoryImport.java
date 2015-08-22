/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.externalapi.custom;

import com.fin.httrader.configprops.HtDataProfileProp;
import com.fin.httrader.configprops.HtHistoryProviderProp;
import com.fin.httrader.configprops.HtRTProviderProp;
import com.fin.httrader.externalapi.HtExtConfiguration;
import com.fin.httrader.externalapi.HtExtGlobalCommand;
import com.fin.httrader.externalapi.HtExtLogger;
import com.fin.httrader.externalapi.HtExtRunner;
import com.fin.httrader.externalapi.HtExtServerCommand;
import com.fin.httrader.model.HtDatabaseProxy;
import com.fin.httrader.utils.HtUtils;
import java.io.File;
import java.lang.String;
import java.util.HashSet;
import java.util.Map;

/**
 *
 * @author DanilinS
 */
public class Gerchik_PerformHistoryImport implements HtExtRunner{

	public void run(HtExtLogger logger, Map<String, String> parameters) throws Exception {
		// make sure yahoo provider is created
		HtExtConfiguration configurator = new HtExtConfiguration();
		HtExtGlobalCommand globalcommand = new HtExtGlobalCommand( logger);

		// perform export
		
		// set provider
		String history_provider="Yahoo Finance (auto)";
		String history_provider_class="com.fin.httrader.hstproviders.HtYahooProvider";
		String in_misha_path = parameters.get("base_path");
		int history_date_load_depth = Integer.valueOf( parameters.get("history_date_load_depth") );
		String history_data_profile = parameters.get("history_data_profile");
		String rt_provider_history = parameters.get("rt_provider_history");
		String allsymbols_file_list_hstprov = parameters.get("allsymbols_file_list_hstprov");

		
		logger.addStringEntry("Base path: " +in_misha_path);
		logger.addStringEntry("Load history depth: " +String.valueOf( history_date_load_depth ));
		logger.addStringEntry("History provider: "+ history_provider+"@"+history_provider_class);
		logger.addStringEntry("History data profile: "+ history_data_profile);
		logger.addStringEntry("RT Provider as the target for history import: "+ rt_provider_history);
		logger.addStringEntry("File containing all symbols to import: "+ allsymbols_file_list_hstprov);

		//
		logger.addStringEntry("Creating history provider: " +history_provider);
		HtHistoryProviderProp hstProp = new HtHistoryProviderProp();
		hstProp.providerClass_m.append("com.fin.httrader.hstproviders.HtYahooProvider");
		hstProp.exportHourShift_m = 0;
		hstProp.importHourShift_m = 0;
		hstProp.isCallExportDialog_m = false;
		hstProp.isCallImportDialog_m = false;
		hstProp.initParams_m.put("YAHOO_SYMBOLS", in_misha_path+File.separatorChar+"in"+File.separatorChar+allsymbols_file_list_hstprov);
		hstProp.initParams_m.put("DEPTH_DAYS", String.valueOf( history_date_load_depth ));
		
		HtExtServerCommand.commandRunWrapper(logger, configurator.setHistoryProviderProperty(history_provider, hstProp));

		//

		logger.addStringEntry("Checking data profile: " +history_data_profile);
		HashSet<String> profileList = new HashSet<String>();
		HtExtServerCommand.commandRunWrapper(logger, configurator.getRegisteredDataProfileList(profileList));

		if (profileList.contains(history_data_profile)) {
			logger.addStringEntry("Data profile exists: " +history_data_profile);
		}
		else {
			// create new history data profile
			logger.addStringEntry("Will create data profile: " +history_data_profile);
			HtDataProfileProp dprop = new HtDataProfileProp();

			HtExtServerCommand.commandRunWrapper(logger, configurator.setDataProfileProperty(history_data_profile, dprop));
		}

		// creating RT provider as a target of history import
		logger.addStringEntry("Checking RT provider as a target of history provider: " +rt_provider_history);
		HashSet<String> rtProviderList = new HashSet<String>();
		HtExtServerCommand.commandRunWrapper(logger, configurator.getRegisteredRtProviderList(rtProviderList));

		if (rtProviderList.contains(rt_provider_history)) {
			logger.addStringEntry("RT provider as a target of history provider exists: " +rt_provider_history);
		}
		else {
			logger.addStringEntry("Will create RT provider as a target of history provider exists: " +rt_provider_history);
			HtRTProviderProp rtProp = new HtRTProviderProp();
			rtProp.isAlien_m = true;

			HtExtServerCommand.commandRunWrapper(logger, configurator.setRtProviderProperty(rt_provider_history, rtProp));
		}
		
		

		// perform export
		String importId = "IMPORT_"+HtUtils.time2SimpleString_Gmt( HtUtils.getCurGmtTime() );
		logger.addStringEntry("Will perfrom import with ID: "+importId);
		
		HtExtServerCommand.commandRunWrapper(logger, globalcommand.importData(
						importId,
						history_data_profile, // data profile
						HtDatabaseProxy.IMPORT_FLAG_OVERWRITE,
						rt_provider_history, //
						history_provider,
						history_provider_class,
						hstProp.initParams_m,
						-1,
						-1,
						0,
						false	));

		logger.addStringEntry("Completed: "+this.getClass().getSimpleName());
		
	}

	public String getRunName() {
		return "Perform History Import";
	}



}
