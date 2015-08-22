/*
 * HtExtRun2.java
 *
 * Created on 19 ќкт€брь 2008 г., 0:43
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package com.fin.httrader.externalapi.custom;

import com.fin.httrader.externalapi.HtExtLogger;
import com.fin.httrader.externalapi.HtExtRunner;

/**
 *
 * @author Victor_Zoubok
 */
public class HtExtRun2 implements HtExtRunner {
    
    /** Creates a new instance of HtExtRun2 */
    public HtExtRun2() {
    }

	public void run(HtExtLogger logger) throws Exception {
	    	logger.addStringEntry("Started processing - version 2.0.6");
		
		Thread.sleep(15000);
		logger.addStringEntry("Finished");

	}

	public String getRunName() {
	    return "HtExtRunner_2";
	}
    
}
