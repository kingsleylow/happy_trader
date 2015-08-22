/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.externalapi.custom;

import com.fin.httrader.configprops.HtServerProp;
import com.fin.httrader.externalapi.HtExtConfiguration;
import com.fin.httrader.externalapi.HtExtGlobalCommand;
import com.fin.httrader.externalapi.HtExtLogger;
import com.fin.httrader.externalapi.HtExtPropertyHolder;
import com.fin.httrader.externalapi.HtExtRunner;
import com.fin.httrader.externalapi.HtExtServerCommand;
import com.fin.httrader.hlpstruct.TradeServerLogLevels;

import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.utils.hlpstruct.HtPair;
import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.Set;
import java.util.Vector;

/**
 *
 * @author DanilinS
 * This starts XPP trading server onlly
 */
public class Gerchik_StartCppServer implements HtExtRunner {
	// make sure yahoo provider is created
	

	public void run(HtExtLogger logger, Map<String, String> parameters) throws Exception
	{
		HtExtConfiguration configurator = new HtExtConfiguration();
		HtExtGlobalCommand globalcommand = new HtExtGlobalCommand( logger);

		String cpp_server_name="Server 1 (auto)";
		String data_profile_to_push = parameters.get("data_profile_to_push");
		String rt_providers_to_subscribe_s = parameters.get("rt_providers_to_subscribe");
		int history_date_load_depth = Integer.valueOf( parameters.get("history_date_load_depth") );

		String [] rt_providers_to_subscribe_arr = rt_providers_to_subscribe_s.trim().split("[,]");
	
		logger.addStringEntry("Cpp server name: " +cpp_server_name);
		logger.addStringEntry("Data profile to push with the server: " +data_profile_to_push);
		logger.addStringEntry("History data load depth: " +history_date_load_depth);
		
		logger.addStringEntry("Creating CPP server: " +cpp_server_name);
		HtServerProp srvProp = new HtServerProp();
		srvProp.cmdPort_m = 5695;
		srvProp.logLevel_m = CommonLog.LL_INFO;

		HashSet<File> execs = new HashSet<File>();
		HtExtServerCommand.commandRunWrapper(logger, configurator.getAllExecutables(execs) );

		String cpp_executable = null;
		for(Iterator<File> it = execs.iterator(); it.hasNext(); ) {
			File file_i = it.next();

			if (file_i.getName().toLowerCase().indexOf("htmain_srv.exe") >=0) {
				cpp_executable = file_i.getAbsolutePath();
				break;
			}
			
		}

		if (cpp_executable == null) {
			logger.addErrorEntry("Cannot fins server executable");
			return;
		}

		srvProp.serverExecutable_m.append(cpp_executable);
		srvProp.htDebugLevel_m = TradeServerLogLevels.INFO;

		HtExtServerCommand.commandRunWrapper(logger, configurator.setTradingServerProperty(cpp_server_name, srvProp));
		logger.addStringEntry("Created CPP server: " +cpp_server_name +" OK");

		// need to re-read server property
		HtExtPropertyHolder<HtServerProp> srvPropHolder = new HtExtPropertyHolder<HtServerProp>();
		configurator.getTradingServerProperty(cpp_server_name, srvPropHolder);

		logger.addStringEntry("Starting CPP server: " +cpp_server_name +" OK");

		// will have to start it
		HtExtServerCommand srvCommand = new HtExtServerCommand( logger);

		// map
		HashMap< String, List<HtPair<String, String> > > profProvSymbMap = new HashMap< String, List<HtPair<String, String> > >();
		HashSet<String> rtProvidersToSubscribe = new HashSet<String>();

		// need to get all history symbols our profile

		List<HtPair<String, String>> provSymbolList = new ArrayList<HtPair<String, String>>();
		HtExtServerCommand.commandRunWrapper(logger,
						configurator.getStoredProviderSymbolsForProfile(provSymbolList, data_profile_to_push) );

		for(int i = 0; i < provSymbolList.size(); i++) {
			// too many data
			// logger.addStringEntry("Will be pushed to: " + provSymbolList.get(i).first + " - " + provSymbolList.get(i).second);
		}
		
		profProvSymbMap.put(data_profile_to_push, provSymbolList);

		// RT Providers
		for(int i = 0; i < rt_providers_to_subscribe_arr.length; i++) {
			String rt_prov_i = rt_providers_to_subscribe_arr[i].trim();

			logger.addStringEntry("Will be subscribed to RT provider: " + rt_prov_i);
			
			rtProvidersToSubscribe.add(rt_prov_i);
			
		}


		// only 10 days history
		long begdate = HtUtils.getCurGmtTime_DayBegin() - history_date_load_depth*24*60*60*1000;
		HtExtServerCommand.commandRunWrapper(logger, srvCommand.startTradingServer(
					cpp_server_name,
					srvPropHolder.p.serverExecutable_m.toString(),
					srvPropHolder.p.htDebugLevel_m,
					srvPropHolder.p.cmdPort_m,
					true, // launch process
					begdate, // push all data
					-1, // push all data
					profProvSymbMap, // map profile <-> ( provider - symbol )
					rtProvidersToSubscribe,
					false, // will not reuse cache
					srvPropHolder.p.logLevel_m, // this is event level
					srvPropHolder.p.subscribedEvents_m,
					false)
			);
		logger.addStringEntry("Starting CPP server: " +cpp_server_name +" was started OK. Congratulations! Data loaded from: " + HtUtils.time2String_Gmt(begdate));

		

		
	}

	public String getRunName() {
		return "start CPP servers";
	}


}
