/*
 * HtExtRunRtProvider.java
 *
 * Created on 24 ќкт€брь 2008 г., 22:07
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package com.fin.httrader.externalapi.custom;

import com.fin.httrader.configprops.HtRTProviderProp;
import com.fin.httrader.externalapi.HtExtGlobalCommand;
import com.fin.httrader.externalapi.HtExtServerCommand;
import com.fin.httrader.externalapi.HtExtConfiguration;
import com.fin.httrader.externalapi.HtExtLogger;
import com.fin.httrader.externalapi.HtExtRunner;

/**
 *
 * @author Victor_Zoubok
 */
public class HtExtRunRtProvider implements HtExtRunner {
    
    /** Creates a new instance of HtExtRunRtProvider */
    public HtExtRunRtProvider() {
    }
    
    // ---------------------------
    
    public void run(HtExtLogger logger) throws Exception 
    {
	// ----------------------------------------------------------------------
	// Parameters
	//
	String rtProvider = "MT Provider";
	
	// ----------------------------------------------------------------------
	// 
	//
	HtExtConfiguration configurator = new HtExtConfiguration();
	HtExtGlobalCommand globalcommand = new HtExtGlobalCommand( logger);
		
	logger.addStringEntry("Starting RT provider: " + rtProvider);
	
	HtRTProviderProp rtProp_MTProvider = new HtRTProviderProp();
	HtExtServerCommand.commandRunWrapper(logger, configurator.getRtProviderProperty(rtProvider, rtProp_MTProvider));
	HtExtServerCommand.commandRunWrapper(logger, globalcommand.startRtProvider(rtProvider, rtProp_MTProvider.providerClass_m.toString(), rtProp_MTProvider.initParams_m, 0) );
	//
	logger.addStringEntry("Started RT provider: " + rtProvider);
	
	logger.addStringEntry("Do not forget run HT_Export in MT4!!!!");
    }

    public String getRunName() {
	    return "run_rt_provider";
    }
    
}
