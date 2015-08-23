/*
 * HtExtRunner.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

// main class to implement exteran runs
// the descendent must derive from 
package com.fin.httrader.externalapi;

import com.fin.httrader.hlpstruct.externalrunstruct.RunParameter;
import java.util.Map;

/**
 *
 * @author Victor_Zoubok
 */
public interface HtExtRunner {
    
    public void run(
			HtExtBaseLogger logger, 
			Map<String, RunParameter> parameters, 
			Map<String, String> runKeys,
			StringBuilder customResult
	) throws Exception;
	
    public String getRunName();
    
};
