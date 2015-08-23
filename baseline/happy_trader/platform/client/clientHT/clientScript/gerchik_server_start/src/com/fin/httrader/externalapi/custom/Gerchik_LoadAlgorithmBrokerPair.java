/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.externalapi.custom;

import com.fin.httrader.configprops.HtAlgBrkPairProp;
import com.fin.httrader.configprops.HtRTProviderProp;
import com.fin.httrader.externalapi.HtExtConfiguration;
import com.fin.httrader.externalapi.HtExtGlobalCommand;
import com.fin.httrader.externalapi.HtExtIntBuffer;
import com.fin.httrader.externalapi.HtExtLogger;
import com.fin.httrader.externalapi.HtExtRunner;
import com.fin.httrader.externalapi.HtExtServerCommand;
import com.fin.httrader.utils.HtUtils;
import java.io.File;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Map;
import java.util.Vector;

/**
 *
 * @author DanilinS
 */
public class Gerchik_LoadAlgorithmBrokerPair implements HtExtRunner {

	public void run(HtExtLogger logger, Map<String, String> parameters) throws Exception {

		HtExtConfiguration configurator = new HtExtConfiguration();
		HtExtGlobalCommand globalcommand = new HtExtGlobalCommand( logger);

		// fisrt of all create this pair
		String cpp_server_name="Server 1 (auto)";
		String algorithm_broker_pair_name = "gerchik trade (auto)";
		String algorithm_broker_pair_name_dataprovider_only = "gerchik trade provider (auto)";

		String run_name="Gerchik_Moysha_"+HtUtils.time2SimpleString_Gmt( HtUtils.getCurGmtTime() );

		// broker parameters
		String brk_in_misha_path = parameters.get("brk_base_path");
		String brk_laser_user_name = parameters.get("brk_laser_user_name");
		String brk_laser_user_password = parameters.get("brk_laser_user_password");
		String brk_laser_connect_executor = parameters.get("brk_laser_connect_executor");
		String brk_laser_connect_level1 = parameters.get("brk_laser_connect_level1");
		String brk_laser_connect_level2 = parameters.get("brk_laser_connect_level2");
		String brk_level1_list_file = parameters.get("brk_level1_list_file");
		String brk_level1_list_file_full = brk_in_misha_path + File.separatorChar+ "in"+ File.separatorChar+ brk_level1_list_file;
		String brk_provider_for_rt = parameters.get("brk_provider_for_rt");
		int brk_login_tout_sec = Integer.valueOf( parameters.get("brk_login_tout_sec") );


		logger.addStringEntry("Found broker user name : " +brk_laser_user_name);
		logger.addStringEntry("Found broker password : " +brk_laser_user_password);
		logger.addStringEntry("Found broker connect executor : " +brk_laser_connect_executor);
		logger.addStringEntry("Found broker connect level1 : " +brk_laser_connect_level1);
		logger.addStringEntry("Found broker connect level2 : " +brk_laser_connect_level2);

		// algoritm parameters
		String alg_file_for_precalc_indexes = parameters.get("alg_file_for_precalc_indexes");
		String alg_file_for_precalc_symbols = parameters.get("alg_file_for_precalc_symbols");
		String alg_file_for_precalc_indexes_full =  brk_in_misha_path + File.separatorChar+ "in"+ File.separatorChar+alg_file_for_precalc_indexes;
		String alg_file_for_precalc_symbols_full =  brk_in_misha_path + File.separatorChar+ "in"+ File.separatorChar+alg_file_for_precalc_symbols;

		int alg_correlation_depth = Integer.valueOf(parameters.get("alg_correlation_depth"));
		String alg_provider_for_precalc = parameters.get("alg_provider_for_precalc");
		String alg_is_precalc_only = parameters.get("alg_is_precalc_only");
		String alg_provider_for_rt_symbols = parameters.get("alg_provider_for_rt_symbols");

		String alg_file_data_precalc_out = parameters.get("alg_file_data_precalc_out");
		String alg_file_for_rt_indexes = parameters.get("alg_file_for_rt_indexes");
		String alg_file_for_rt_symbols = parameters.get("alg_file_for_rt_symbols");

		String alg_file_data_precalc_out_full = brk_in_misha_path + File.separatorChar+ "out"+ File.separatorChar+ alg_file_data_precalc_out;
		String alg_file_for_rt_indexes_full = brk_in_misha_path + File.separatorChar+ "in"+ File.separatorChar+ alg_file_for_rt_indexes;
		String alg_file_for_rt_symbols_full = brk_in_misha_path + File.separatorChar+ "in"+ File.separatorChar+ alg_file_for_rt_symbols;


		String alg_provider_for_rt_indexes = parameters.get("alg_provider_for_rt_indexes");
		String alg_log_base_path = brk_in_misha_path + File.separatorChar+ "out"+ File.separatorChar+ "log";
		

		double alg_sba = Double.valueOf( parameters.get("alg_sba") );
		double alg_pr = Double.valueOf( parameters.get("alg_pr") );
		double alg_risk_min = Double.valueOf( parameters.get("alg_risk_min") );
		double alg_shares = Double.valueOf( parameters.get("alg_shares") );
		boolean alg_singleton = Boolean.valueOf( parameters.get("alg_singleton") );
		boolean alg_long_only = Boolean.valueOf( parameters.get("alg_long_only") );

		String alg_order_method = parameters.get("alg_order_method");
		String alg_order_place = parameters.get("alg_order_place");
		int alg_timeout_after_open = Integer.valueOf( parameters.get("alg_timeout_after_open") );
		boolean alg_open_ioc = Boolean.valueOf( parameters.get("alg_open_ioc") );
		// if order is IOC - this is timeout
		int alg_open_ioc_tforce_sec = Integer.valueOf( parameters.get("alg_open_ioc_tforce_sec") );
		double alg_slide= Double.valueOf( parameters.get("alg_slide") );
		

		File f = new File(brk_level1_list_file_full);
		if (!f.exists()) {
			logger.addErrorEntry("File with all symbols for laser RT provider must exists: " + brk_level1_list_file_full);
			return;
		}
		



		//
		logger.addStringEntry("Base path: " +brk_in_misha_path);

		
		HtAlgBrkPairProp algBrkProp = new HtAlgBrkPairProp();

		logger.addStringEntry("Creating algorithm-broker entry: " +algorithm_broker_pair_name);
		HashSet<File> execs = new HashSet<File>();
		HtExtServerCommand.commandRunWrapper(logger, configurator.getAllDlls(execs) );

		String alg_executable = null;
		String brk_executable = null;

		for(Iterator<File> it = execs.iterator(); it.hasNext(); ) {
			File file_i = it.next();
			

			if (file_i.getName().toLowerCase().indexOf("htmgerchik_bck.dll") >=0) {
				alg_executable = file_i.getAbsolutePath();
				
			}
		};

		for(Iterator<File> it = execs.iterator(); it.hasNext(); ) {
			File file_i = it.next();

			if (file_i.getName().toLowerCase().indexOf("htbrkliblaser_bck.dll") >=0) {
				brk_executable = file_i.getAbsolutePath();
				break;
			}

		}

		if (alg_executable == null || brk_executable==null) {
			logger.addErrorEntry("Cannot find algorithm or broker dlls");
			return;
		}


		algBrkProp.brokerPath_m.append(brk_executable);
		algBrkProp.algorithmPath_m.append(alg_executable);

		

		algBrkProp.algParams_m.put("LOG_BASE_PATH", alg_log_base_path);
		algBrkProp.algParams_m.put("FILE_FOR_PRECALC_INDEXES", alg_file_for_precalc_indexes_full);
		algBrkProp.algParams_m.put("FILE_FOR_PRECALC_SYMBOLS", alg_file_for_precalc_symbols_full);
		algBrkProp.algParams_m.put("PROVIDER_FOR_PRECALC", alg_provider_for_precalc);
		algBrkProp.algParams_m.put("IS_PRECALC_ONLY", alg_is_precalc_only.equalsIgnoreCase("true") ? "true" : "false");
		algBrkProp.algParams_m.put("PROVIDER_FOR_RT_SYMBOLS", alg_provider_for_rt_symbols );
		algBrkProp.algParams_m.put("PROVIDER_FOR_RT_INDEXES", alg_provider_for_rt_indexes);
		algBrkProp.algParams_m.put("FILE_DATA_PRECALC_OUT", alg_file_data_precalc_out_full);
		algBrkProp.algParams_m.put("FILE_FOR_RT_SYMBOLS", alg_file_for_rt_symbols_full);
		algBrkProp.algParams_m.put("FILE_FOR_RT_INDEXES", alg_file_for_rt_indexes_full);
		algBrkProp.algParams_m.put("CORRELATION_DEPTH", String.valueOf(alg_correlation_depth));
		algBrkProp.algParams_m.put("SBA", String.valueOf(alg_sba));
		algBrkProp.algParams_m.put("PR", String.valueOf(alg_pr ));
		algBrkProp.algParams_m.put("RISK_MIN", String.valueOf(alg_risk_min));
		algBrkProp.algParams_m.put("SHARES", String.valueOf(alg_shares));
		algBrkProp.algParams_m.put("SINGLETON", String.valueOf(alg_singleton));
		algBrkProp.algParams_m.put("LONG_ONLY", String.valueOf(alg_long_only));

		algBrkProp.algParams_m.put("ORDER_METHOD", alg_order_method);
		algBrkProp.algParams_m.put("ORDER_PLACE", alg_order_place);
		algBrkProp.algParams_m.put("TIMEOUT_AFTER_OPEN", String.valueOf(alg_timeout_after_open));
		algBrkProp.algParams_m.put("OPEN_IOC", String.valueOf(alg_open_ioc));
		algBrkProp.algParams_m.put("OPEN_IOC_TIMEOUT_SEC", String.valueOf(alg_open_ioc_tforce_sec));
		algBrkProp.algParams_m.put("SLIDE", String.valueOf(alg_slide));

		// broker params
		algBrkProp.brokerParams_m.put("MANAGE_POS_ONLY", "true" );
		algBrkProp.brokerParams_m.put("LEVEL1_LIST_FILE", brk_level1_list_file_full);


		logger.addStringEntry("Algorithm DLL: " +alg_executable);
		logger.addStringEntry("Broker DLL: " +brk_executable);

		// set property
		HtExtServerCommand.commandRunWrapper(logger,configurator.setAlgBrkPairProperty(algorithm_broker_pair_name, algBrkProp));
		logger.addStringEntry("Created algorithm-broker entry: " +algorithm_broker_pair_name);


		// remove initially

		// create another algorithm - broker entry - only for prices
		HtAlgBrkPairProp algBrkProp_Provider = new HtAlgBrkPairProp();

		logger.addStringEntry("Creating algorithm-broker entry - only provider: " +algorithm_broker_pair_name_dataprovider_only);
		HashSet<File> execs2 = new HashSet<File>();
		HtExtServerCommand.commandRunWrapper(logger, configurator.getAllDlls(execs2) );

		String alg_executable2 = null;
		String brk_executable2 = null;

		for(Iterator<File> it = execs2.iterator(); it.hasNext(); ) {
			File file_i = it.next();
			

			if (file_i.getName().toLowerCase().indexOf("htalglibdummy_bck.dll") >=0) {
				alg_executable2 = file_i.getAbsolutePath();
				
			}
		};

		for(Iterator<File> it = execs.iterator(); it.hasNext(); ) {
			File file_i = it.next();

			if (file_i.getName().toLowerCase().indexOf("htbrkliblaser_bck.dll") >=0) {
				brk_executable2 = file_i.getAbsolutePath();
				break;
			}

		}

		if (alg_executable2 == null || brk_executable2==null) {
			logger.addErrorEntry("Cannot find algorithm or broker dlls");
			return;
		}

		// ---------------
		// parameters

		algBrkProp_Provider.brokerPath_m.append(brk_executable2);
		algBrkProp_Provider.algorithmPath_m.append(alg_executable2);

		algBrkProp_Provider.brokerParams_m.put("LASER_USER", brk_laser_user_name);
		algBrkProp_Provider.brokerParams_m.put("LASER_PASSWORD", brk_laser_user_password);
		algBrkProp_Provider.brokerParams_m.put("LASER_CONNECT_EXECUTOR", brk_laser_connect_executor);
		algBrkProp_Provider.brokerParams_m.put("LASER_CONNECT_LEVEL1", brk_laser_connect_level1);
		algBrkProp_Provider.brokerParams_m.put("LASER_CONNECT_LEVEL2", brk_laser_connect_level2);
		algBrkProp_Provider.brokerParams_m.put("LEVEL1_LIST_FILE", brk_level1_list_file_full);
		algBrkProp_Provider.brokerParams_m.put("PROVIDER_FOR_RT", brk_provider_for_rt);
		algBrkProp_Provider.brokerParams_m.put("LOGIN_TOUT_SEC", String.valueOf(brk_login_tout_sec));



		logger.addStringEntry("Algorithm DLL - data provider: " +alg_executable2);
		logger.addStringEntry("Broker DLL - data provider: " +brk_executable2);

		HtExtServerCommand.commandRunWrapper(logger,configurator.setAlgBrkPairProperty(algorithm_broker_pair_name_dataprovider_only, algBrkProp_Provider));
		logger.addStringEntry("Created algorithm-broker entry: " +algorithm_broker_pair_name_dataprovider_only);

		

		// need to create an alien provider brk_provider_for_rt - this is the RT provider of broker symbols
		logger.addStringEntry("Creating the alien RT provider: " +brk_provider_for_rt);
		HtRTProviderProp rtProp = new HtRTProviderProp();
		rtProp.isAlien_m = true;
		HtExtServerCommand.commandRunWrapper(logger, configurator.setRtProviderProperty(brk_provider_for_rt, rtProp));
		

		logger.addStringEntry("Loading algorithm-broker entry: " +algorithm_broker_pair_name);

		HtExtServerCommand srvCommand = new HtExtServerCommand( logger);
		HtExtServerCommand.commandRunWrapper(logger, srvCommand.joinTradingServer(cpp_server_name));


		// the first
		HtExtServerCommand.commandRunWrapper(logger,
	    srvCommand.loadAlgBrkPair(
	    algorithm_broker_pair_name_dataprovider_only,
	    run_name,
	    "",
	    1, // priority
	    algBrkProp_Provider.algorithmPath_m.toString(),
			algBrkProp_Provider.brokerPath_m.toString(),
	    algBrkProp_Provider.algParams_m,
	    algBrkProp_Provider.brokerParams_m)
	   );

		logger.addStringEntry("Loaded algorithm-broker entry - data provider: " +algorithm_broker_pair_name_dataprovider_only);

		//
		// the second
		HtExtServerCommand.commandRunWrapper(logger,
	    srvCommand.loadAlgBrkPair(
	    algorithm_broker_pair_name,
	    run_name,
	    "",
	    2, // priority
	    algBrkProp.algorithmPath_m.toString(),
			algBrkProp.brokerPath_m.toString(),
	    algBrkProp.algParams_m,
	    algBrkProp.brokerParams_m)
	   );

		logger.addStringEntry("Loaded algorithm-broker entry: " +algorithm_broker_pair_name);


		//

		HtExtIntBuffer threadId = new HtExtIntBuffer();

		Vector<String> algpairs = new Vector<String>();
		algpairs.add(algorithm_broker_pair_name_dataprovider_only);
		algpairs.add(algorithm_broker_pair_name);
		
		HtExtServerCommand.commandRunWrapper(logger, srvCommand.startThread(algpairs, threadId));

		logger.addStringEntry("Loaded thread: " +threadId.getInt());

		// start RT activity
		String runName = "run_name_" + HtUtils.getCurGmtTime();
		HtExtServerCommand.commandRunWrapper(logger, srvCommand.startRtActivity(threadId.getInt(), runName, "", ""));
		logger.addStringEntry("Started RT activity: " +threadId.getInt() + " - " + runName);
		

	}

	public String getRunName() {
		return "load gerchik algorithm broker pair";
	}

}
