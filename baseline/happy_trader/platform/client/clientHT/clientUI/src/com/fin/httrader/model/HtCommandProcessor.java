/*
 * HtCommandProcessor.java
 
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.model;

import com.fin.httrader.HtMain;
import com.fin.httrader.hlpstruct.broker.BrokerConnect;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import java.util.Calendar;
import java.util.List;
import java.util.Vector;
import java.util.logging.Level;

/**
 *
 * @author victor_zoubok
 * The class is responsible for the processing of this application command
 * It may use helper classes if necessary
 * Generally it gets command from UI execute it and retreives the result back to UI
 */
public class HtCommandProcessor {

	static private HtCommandProcessor instance_m = null;

	private HtProxyBase[] proxies_m = null;

	static public HtCommandProcessor create() {
		if (instance_m == null) {
			instance_m = new HtCommandProcessor();
		}

		return instance_m;
	}

	static public HtCommandProcessor instance() {
		if (instance_m == null) {
			instance_m = new HtCommandProcessor();
		}

		return instance_m;
	}

	private String getContext() {
		return this.getClass().getSimpleName();
	}

	private HtCommandProcessor() {
		proxies_m = new HtProxyBase[]{
							new HtRtDsourceProxy(),
							new HtServerProxy(),
							new HtDatabaseProxy(),
							new HtConfigurationProxy(),
							new HtHistoryDsourceProxy(),
							new HtExternalApiProxy(),
							new HtEventPluginProxy(),
							new HtSecurityProxy(),
							new HtConsoleProxy(),
							new HtBootProxy(),
							new HtBackgroundJobProxy(),
							new HtLongPollProxy()
						};
	}

	// direct accessors (for local client)
	public HtRtDsourceProxy get_HtRtDsourceProxy() {
		return (HtRtDsourceProxy) proxies_m[0];
	}

	public HtServerProxy get_HtServerProxy() {
		return (HtServerProxy) proxies_m[1];
	}

	public HtDatabaseProxy get_HtDatabaseProxy() {
		return (HtDatabaseProxy) proxies_m[2];
	}

	public HtConfigurationProxy get_HtConfigurationProxy() {
		return (HtConfigurationProxy) proxies_m[3];
	}

	public HtHistoryDsourceProxy get_HtHistoryDsourceProxy() {
		return (HtHistoryDsourceProxy) proxies_m[4];
	}

	
	public HtExternalApiProxy get_HtExternalApiProxy() {
		return (HtExternalApiProxy) proxies_m[5];
	}
	 

	public HtEventPluginProxy get_EventPluginProxy() {
		return (HtEventPluginProxy) proxies_m[6];
	}

	public HtSecurityProxy get_HtSecurityProxy() {
		return (HtSecurityProxy) proxies_m[7];
	}

	public HtConsoleProxy get_HtConsoleProxy() {
		return (HtConsoleProxy) proxies_m[8];
	}
	public HtBootProxy get_HtBootProxy() {
		return (HtBootProxy) proxies_m[9];
	}
	
	public HtBackgroundJobProxy get_HtBackgroundJobProxy() {
		return (HtBackgroundJobProxy) proxies_m[10];
	}
	
	public HtLongPollProxy get_HtLongPollProxy() {
		return (HtLongPollProxy) proxies_m[11];
	}



	// deinitialize command processor
	public void shutdown()
	{
	}

	
	

}
