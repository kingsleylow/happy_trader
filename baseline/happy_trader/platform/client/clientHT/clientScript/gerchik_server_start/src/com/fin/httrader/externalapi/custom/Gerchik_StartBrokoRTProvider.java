/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.externalapi.custom;

import com.fin.httrader.configprops.HtRTProviderProp;
import com.fin.httrader.externalapi.HtExtConfiguration;
import com.fin.httrader.externalapi.HtExtGlobalCommand;
import com.fin.httrader.externalapi.HtExtLogger;
import com.fin.httrader.externalapi.HtExtRunner;
import com.fin.httrader.externalapi.HtExtServerCommand;
import com.fin.httrader.hlpstruct.ReatTimeProviderStatus;
import java.io.File;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Map;

/**
 *
 * @author DanilinS
 */
public class Gerchik_StartBrokoRTProvider implements HtExtRunner {

	public void run(HtExtLogger logger, Map<String, String> parameters) throws Exception {
		HtExtConfiguration configurator = new HtExtConfiguration();
		HtExtGlobalCommand globalcommand = new HtExtGlobalCommand( logger);

		String rt_provider_type = "com.fin.httrader.rtproviders.RtMetaTrader";
		String provider_name = parameters.get("provider_name");
		String base_path = parameters.get("base_path");
		String alg_file_for_rt_indexes = parameters.get("alg_file_for_rt_indexes");
		String alg_file_for_rt_indexes_full = base_path+ File.separatorChar + "in" + File.separatorChar + alg_file_for_rt_indexes;

		File f = new File(alg_file_for_rt_indexes_full);
		if (!f.exists()) {
			logger.addErrorEntry("File with all indexes for broko RT provider must exists: " + alg_file_for_rt_indexes_full);
			return;
		}

		HashSet<File> execs = new HashSet<File>();
		HtExtServerCommand.commandRunWrapper(logger, configurator.getAllExecutables(execs) );

		String ddeclient_executable = null;
		for(Iterator<File> it = execs.iterator(); it.hasNext(); ) {
			File file_i = it.next();

			if (file_i.getName().toLowerCase().indexOf("ddeclient.exe") >=0) {
				ddeclient_executable = file_i.getAbsolutePath();
				break;
			}

		}

		logger.addStringEntry("Creating RT provider: " +provider_name);

		HtRTProviderProp rtProp = new HtRTProviderProp();

		rtProp.providerClass_m.append(rt_provider_type);
		rtProp.initParams_m.put("file_data_list", alg_file_for_rt_indexes_full);
		rtProp.initParams_m.put("ddeclient_path", ddeclient_executable);
		HtExtServerCommand.commandRunWrapper(logger,  configurator.setRtProviderProperty(provider_name, rtProp));

		HtExtServerCommand.commandRunWrapper(logger,  
						globalcommand.startRtProvider(provider_name, rtProp.providerClass_m.toString(), rtProp.initParams_m, 0));

		// will wait and check in 10 seconds
		logger.addStringEntry("Started RT provider: " +provider_name+",  will wait for 10 seconds and checks status");

		ReatTimeProviderStatus status = new ReatTimeProviderStatus();
		Thread.currentThread().sleep(10 * 1000);
		HtExtServerCommand.commandRunWrapper(logger,  globalcommand.checkRTProviderStatus(provider_name, status));

		if (!status.isOK()) {
			logger.addErrorEntry("Invalid provider status: " + status.getStatusCode() + " -" + status.getStatusReason());
			return;
		}

	}

	public String getRunName() {
		return "start broko RT provider";
	}

}
