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
import java.util.Vector;

/**
 *
 * @author Victor_Zoubok
 */
public class HtExtRun1 implements HtExtRunner{ 

    private void run_internal( HtExtLogger logger, String serverId, String algPairId, String symbol, int globalId) throws Exception
    {
	String runId = globalId+ "_" + symbol + "_2001_01_2008_08";
	logger.addStringEntry("Run ID: " + runId);
	
	
	HtExtServerCommand command = new HtExtServerCommand(logger);
	command.joinTradingServer(serverId);
	
	HtExtConfiguration configurator = new HtExtConfiguration();
	HtAlgBrkPairProp algBrkProp = new  HtAlgBrkPairProp();
	HtExtServerCommand.commandRunWrapper(logger, configurator.getAlgBrkPairProperty("dummy_day_trade", algBrkProp ));

	
	HtExtServerCommand.commandRunWrapper(logger, 
	    command.loadAlgBrkPair(
	    "dummy_day_trade", 
	    runId, 
	    "comment 1", 
	    1, 
	    algBrkProp.algorithmPath_m.toString(),
	    algBrkProp.brokerPath_m.toString(),
	    algBrkProp.brokerParams_m,
	    algBrkProp.brokerParams_m)
	   );
	
	
	// start thread
	HtExtIntBuffer threadId = new HtExtIntBuffer();
	
	Vector<String> pairs = new Vector<String>();
	pairs.add(algPairId);
	
	HtExtServerCommand.commandRunWrapper(logger, command.startThread(pairs, threadId));
	
	Vector<HtPair<String, String> > provSymbList = new Vector<HtPair<String, String> >();
	provSymbList.add(new HtPair<String, String>("MT Provider",symbol));
	
	// start simulation
	HtExtServerCommand.commandRunWrapper(logger, command.startSimulation(
	    threadId.getInt(),
	    runId,
	    "comment 2",
	    "a2",
	    provSymbList,
	    HtUtils.parseDateTimeParameter("01-01-2001 00:00", false),
	    HtUtils.parseDateTimeParameter("01-08-2008 23:59", false),
	    false,
	    true,
	    false ) );
	
	
	// need to wait
	HtExtServerCommand.commandRunWrapper(logger, command.waitThreadInLazyMode(threadId.getInt(), 12) );
	
	
	// stop thread
	HtExtServerCommand.commandRunWrapper(logger, command.stopThread(threadId.getInt()) );
	
	
	// unload algorithm
	HtExtServerCommand.commandRunWrapper(logger, command.unloadAlgorithms(serverId,algPairId ) );
    }
    
    // ----------------------------
   
   
    
    // ----------------------------
    public void HtExtRun1()
    {
    }
    
    // ----------------------------
    public void run(HtExtLogger logger) throws Exception {
	logger.addStringEntry("Started processing - version 1.0.6");
	
	int globalId = 101;
	String serverId = "server 1";
	String algPairId = "dummy_day_trade"; 
	logger.addStringEntry("Alg Lib Pair: " + algPairId);
	logger.addStringEntry("Server: " + serverId);
	
	
	run_internal(logger, serverId, algPairId, "EURUSD", globalId );
	run_internal(logger, serverId, algPairId, "EURJPY", globalId );
	run_internal(logger, serverId, algPairId, "GBPUSD", globalId );
	run_internal(logger, serverId, algPairId, "USDCHF", globalId );
	run_internal(logger, serverId, algPairId, "USDJPY", globalId );
	
	
	
	
	logger.addStringEntry("Finished totally");
	
    }
    
    public String getRunName() {
	return "run_1";
    }
    
    
    
    
    
}
