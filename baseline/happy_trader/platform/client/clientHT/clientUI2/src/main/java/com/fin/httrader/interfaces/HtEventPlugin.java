/*
 * HtEventPlugin.java
 *
  *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package com.fin.httrader.interfaces;


import com.fin.httrader.hlpstruct.BackgroundJobStatus;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import java.util.Map;

/**
 *
 * @author Victor_Zoubok
 */
public interface HtEventPlugin {
	
		// --------------------------
		// to override

		public abstract Integer[] returnSubscribtionEventTypes();

    public abstract void initialize(Map<String, String> initdata) throws Exception;
    
    public abstract void deinitialize() throws Exception;
    
    public abstract String getEventPluginId();
    
    public abstract void setEventPluginId(String pluginId);
        
    public abstract void execute(XmlEvent alertData) throws Exception;

	

}
