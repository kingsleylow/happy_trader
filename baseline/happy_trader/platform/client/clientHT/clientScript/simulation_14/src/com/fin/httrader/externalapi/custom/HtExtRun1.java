/*
 * HtExtRun1.java
 *
 * Created on 20 Сентябрь 2008 г., 12:25
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.externalapi.custom;

import com.fin.httrader.configprops.HtAlgBrkPairProp;
import com.fin.httrader.externalapi.*;
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

	private void run_internal(HtExtLogger logger, String comment, String serverId, String algPairId, String symbol, String globalId, String simulationBegin, String simulationEnd) throws Exception {
		String runId = globalId + symbol + "_pvt";
		logger.addStringEntry("Run ID: " + runId);


		HtExtServerCommand command = new HtExtServerCommand(logger);
		command.joinTradingServer(serverId);

		HtExtConfiguration configurator = new HtExtConfiguration();
		HtAlgBrkPairProp algBrkProp = new HtAlgBrkPairProp();


		HtExtServerCommand.commandRunWrapper(logger, configurator.getAlgBrkPairProperty(algPairId, algBrkProp));


		// adjust trading symbol
		algBrkProp.algParams_m.put("trade_symbol", new String(symbol));
		logger.addStringEntry("Path to alg library: " + algBrkProp.algorithmPath_m.toString());
		logger.addStringEntry("Path to brk library: " + algBrkProp.brokerPath_m.toString());



		//
		HtExtServerCommand.commandRunWrapper(logger,
						command.loadAlgBrkPair(
						algPairId,
						runId,
						comment,
						1,
						algBrkProp.algorithmPath_m.toString(),
						algBrkProp.brokerPath_m.toString(),
						algBrkProp.algParams_m,
						algBrkProp.brokerParams_m));

		// start thread
		HtExtIntBuffer threadId = new HtExtIntBuffer();

		Vector<String> pairs = new Vector<String>();
		pairs.add(algPairId);

		HtExtServerCommand.commandRunWrapper(logger, command.startThread(pairs, threadId));

		Vector<HtPair<String, String>> provSymbList = new Vector<HtPair<String, String>>();


		// for now will add all symbols
		provSymbList.add(new HtPair<String, String>("MT Provider", symbol));



		// start simulation
		HtExtServerCommand.commandRunWrapper(logger, command.startSimulation(
						threadId.getInt(),
						runId,
						comment,
						"a2",
						provSymbList,
						HtUtils.parseDateTimeParameter(simulationBegin, false),
						HtUtils.parseDateTimeParameter(simulationEnd, false),
						false, // each tick arrival
						true, // remove cache aftre simulation
						false // do not reuse cache aftre simulation
						));


		// need to wait
		HtExtServerCommand.commandRunWrapper(logger, command.waitThreadInLazyMode(threadId.getInt(), 20));


		// stop thread
		HtExtServerCommand.commandRunWrapper(logger, command.stopThread(threadId.getInt()));


		// unload algorithm
		HtExtServerCommand.commandRunWrapper(logger, command.unloadAlgorithms(serverId, algPairId));
	}

	// ----------------------------
	private void run_internal_2(HtExtLogger logger, String comment, String serverId, String algPairId, String algPairId2, String globalId, String simulationBegin, String simulationEnd) throws Exception {
		String runId = globalId + "_ptv_2";
		logger.addStringEntry("Run ID: " + runId);


		HtExtServerCommand command = new HtExtServerCommand(logger);
		command.joinTradingServer(serverId);

		HtExtConfiguration configurator = new HtExtConfiguration();
		HtAlgBrkPairProp algBrkProp = new HtAlgBrkPairProp();
		HtAlgBrkPairProp algBrkProp2 = new HtAlgBrkPairProp();


		HtExtServerCommand.commandRunWrapper(logger, configurator.getAlgBrkPairProperty(algPairId, algBrkProp));
		HtExtServerCommand.commandRunWrapper(logger, configurator.getAlgBrkPairProperty(algPairId2, algBrkProp2));





		//
		HtExtServerCommand.commandRunWrapper(logger,
						command.loadAlgBrkPair(
						algPairId,
						runId,
						comment,
						1,
						algBrkProp.algorithmPath_m.toString(),
						algBrkProp.brokerPath_m.toString(),
						algBrkProp.algParams_m,
						algBrkProp.brokerParams_m));

		HtExtServerCommand.commandRunWrapper(logger,
						command.loadAlgBrkPair(
						algPairId2,
						runId,
						comment,
						2,
						algBrkProp2.algorithmPath_m.toString(),
						algBrkProp2.brokerPath_m.toString(),
						algBrkProp2.algParams_m,
						algBrkProp2.brokerParams_m));

		// start thread
		HtExtIntBuffer threadId = new HtExtIntBuffer();

		Vector<String> pairs = new Vector<String>();
		pairs.add(algPairId);
		pairs.add(algPairId2);


		HtExtServerCommand.commandRunWrapper(logger, command.startThread(pairs, threadId));

		Vector<HtPair<String, String>> provSymbList = new Vector<HtPair<String, String>>();



		provSymbList.add(new HtPair<String, String>("MT Provider", "USDCHF"));
		provSymbList.add(new HtPair<String, String>("MT Provider", "EURUSD"));

		// start simulation
		HtExtServerCommand.commandRunWrapper(logger, command.startSimulation(
						threadId.getInt(),
						runId,
						comment,
						"a2",
						provSymbList,
						HtUtils.parseDateTimeParameter(simulationBegin, false),
						HtUtils.parseDateTimeParameter(simulationEnd, false),
						false, // each tick arrival
						true, // remove cache aftre simulation
						false // do not reuse cache aftre simulation
						));


		// need to wait
		HtExtServerCommand.commandRunWrapper(logger, command.waitThreadInLazyMode(threadId.getInt(), 20));


		// stop thread
		HtExtServerCommand.commandRunWrapper(logger, command.stopThread(threadId.getInt()));


		// unload algorithm
		HtExtServerCommand.commandRunWrapper(logger, command.unloadAlgorithms(serverId, algPairId));

		// unload algorithm
		HtExtServerCommand.commandRunWrapper(logger, command.unloadAlgorithms(serverId, algPairId2));
	}


	// ----------------------------
	public void run(HtExtLogger logger) throws Exception {
		logger.addStringEntry("Started processing - version 1.0.943");

		String comment = "PTV EURUSD USDCHF";
		String globalId = String.valueOf(Calendar.getInstance().getTimeInMillis()) + "_";
		//String serverId = "server 1_debug";
		String serverId = "server 1";

		//String algPairId = "pvt_alg_debug";
		//String algPairId2 = "ptv_alg_2_debug";

		String algPairId = "ptv_alg";
		String algPairId2 = "ptv_alg_2";
		logger.addStringEntry("Alg Lib Pair: " + algPairId);
		logger.addStringEntry("Server: " + serverId);

		String simulationBegin = "01-01-2006 00:00";
		String simulationEnd = "30-11-2008 23:59";

		logger.addStringEntry("Simulation from: " + simulationBegin + " to " + simulationEnd);


		//run_internal(logger, comment, serverId, algPairId, "EURUSD", globalId, simulationBegin, simulationEnd );
		//run_internal(logger, comment, serverId, algPairId, "EURJPY", globalId, simulationBegin, simulationEnd );
		//run_internal(logger, comment, serverId, algPairId, "GBPUSD", globalId, simulationBegin, simulationEnd );
		//run_internal(logger, comment, serverId, algPairId, "USDCHF", globalId, simulationBegin, simulationEnd );
		//run_internal(logger, comment, serverId, algPairId, "USDJPY", globalId, simulationBegin, simulationEnd );

		run_internal_2(logger, comment, serverId, algPairId, algPairId2, globalId, simulationBegin, simulationEnd);

		logger.addStringEntry("Finished totally");

	}

	public String getRunName() {
		return "run_sim_14";
	}
}
