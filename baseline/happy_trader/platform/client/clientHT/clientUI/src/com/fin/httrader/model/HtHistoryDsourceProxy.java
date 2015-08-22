/*
 * HtHistoryDsourceProxy.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.model;

import com.fin.httrader.managers.RtHistoryProviderManager;
import com.fin.httrader.utils.hlpstruct.HtPair;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import java.io.File;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
import java.util.TreeSet;

/**
 *
 * @author Victor_Zoubok
 */
public class HtHistoryDsourceProxy extends HtProxyBase {

	private String getContext() {
		return this.getClass().getSimpleName();
	}



	public String getProxyName() {
		return "hist_dsource_proxy";
	}

	/** Creates a new instance of HtHistoryDsourceProxy */
	public HtHistoryDsourceProxy() {
	}

	//
	public Map< String, File > remote_queryAvailableProviders() throws Exception {
		return RtHistoryProviderManager.instance().getAvailableProviders(true);
	}
	
	
}
