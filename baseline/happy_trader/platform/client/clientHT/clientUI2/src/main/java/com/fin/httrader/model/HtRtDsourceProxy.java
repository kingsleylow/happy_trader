/*
 * HtRtDsourceProxy.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.model;


import com.fin.httrader.hlpstruct.BackgroundJobStatus;
import com.fin.httrader.managers.RtRealTimeProviderManager;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import java.io.File;
import java.util.List;
import java.util.Map;

/**
 *
 * @author Administrator
 */
public class HtRtDsourceProxy extends HtProxyBase {

	private String getContext() {
		return this.getClass().getSimpleName();
	}

	/** Creates a new instance of HtRtDsourceProxy */
	public HtRtDsourceProxy() {
	}

	// calls that can be be performed remotely
	public void remote_startProvider(String providerId, String providerclass, Map<String, String> initdata, int hourshift) throws Exception {
		RtRealTimeProviderManager.instance().loadProvider(providerId, providerclass, initdata, hourshift);
	}

	// stop provider
	public void remote_stopProvider(String providerId) throws Exception {
		RtRealTimeProviderManager.instance().unloadProvider(providerId);
	}

	public Map< String, File > remote_queryAvailableProviders() throws Exception {
		return RtRealTimeProviderManager.instance().getAvailableProviders(true);
	}

	

	public boolean remote_queryProviderRunning(String providerId) {

		return RtRealTimeProviderManager.instance().isProviderRunning(providerId);
	}

	public int remote_getProviderStatusCode(String providerId) throws Exception
	{
		return RtRealTimeProviderManager.instance().getProviderStatusCode(providerId);
	}

	public String remote_getProviderStatusString(String providerId) throws Exception
	{
		return RtRealTimeProviderManager.instance().getProviderStatusString(providerId);
	}

	public BackgroundJobStatus remote_getProviderStatus(String providerId) throws Exception
	{
		return RtRealTimeProviderManager.instance().getProviderStatus(providerId);
	}
	
	public void remote_returnAvailableSymbols(String providerId, List<String> symbols)
	{
		RtRealTimeProviderManager.instance().returnAvailableSymbolList(providerId, symbols);
	}

	

	public String getProxyName() {
		return "rt_dsource_proxy";
	}
}
