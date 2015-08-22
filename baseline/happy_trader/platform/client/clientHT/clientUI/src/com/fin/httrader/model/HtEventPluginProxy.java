/*
 * HtEventPluginProxy.java
 *
  *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.model;

import com.fin.httrader.hlpstruct.BackgroundJobStatus;
import com.fin.httrader.managers.RtAlertPluginManager;
import com.fin.httrader.utils.IntParam;
import com.fin.httrader.utils.hlpstruct.HtPair;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import java.io.File;
import java.util.List;
import java.util.Map;

/**
 *
 * @author Victor_Zoubok
 */
public class HtEventPluginProxy extends HtProxyBase {

	private String getContext() {
		return this.getClass().getSimpleName();
	}

	/** Creates a new instance of HtRtDsourceProxy */
	public HtEventPluginProxy() {
	}

	// calls that can be be performed remotely
	public void remote_startPlugin(String pluginId, String plugintype, int launch_flag, Map<String, String> initdata) throws Exception {
		RtAlertPluginManager.instance().loadPlugin(pluginId, plugintype, launch_flag, initdata);
	}

	// stop provider
	public void remote_stopPlugin(String pluginId) throws Exception {
		RtAlertPluginManager.instance().unloadPlugin(pluginId);
	}

	public List< HtPair<String, File> > remote_queryAvailableProviders() throws Exception {
		return RtAlertPluginManager.instance().getAvailableProviders(true);
	}
	
	public void remote_checkWherePluginFound(List< HtPair<String, File> > providers, String pluginType, StringBuilder jarPath, IntParam foundFlag) throws Exception {
		RtAlertPluginManager.instance().checkWherePluginFound(providers, pluginType, jarPath, foundFlag);
	}
	
	

	

	public boolean remote_queryPluginRunning(String pluginId) {
		return RtAlertPluginManager.instance().isPluginRunning(pluginId);
	}

	public int remote_getPluginStatusCode(String providerId) throws Exception
	{
		return RtAlertPluginManager.instance().getPluginStatusCode(providerId);
	}

	public String remote_getPluginStatusString(String providerId) throws Exception
	{
		return RtAlertPluginManager.instance().getPluginStatusString(providerId);
	}

	public BackgroundJobStatus remote_getPluginStatus(String providerId) throws Exception
	{
		return RtAlertPluginManager.instance().getPluginStatus(providerId);
	}

	

	public String getProxyName() {
		return "event_plugin_proxy";
	}
}
