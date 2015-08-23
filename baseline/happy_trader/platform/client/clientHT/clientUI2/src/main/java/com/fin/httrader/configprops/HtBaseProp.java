/*
 * HtBaseProp.java
 *
 * Created on 24 2008 �., 15:42
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.configprops;

import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.hlpstruct.XmlParameter;

/**
 *
 * @author Victor_Zoubok
 * this is the base class for all the derived properties
 */
public abstract class HtBaseProp {

	/**
	Identifiers of base entry
	 */
	public static final String ID_CONFIGURE_ALG_BROKER = "algbrk";
	public static final String ID_CONFIGURE_DATA_PROFILE = "dataprof";
	public static final String ID_CONFIGURE_GLOBAL = "global";
	public static final String ID_CONFIGURE_HISTORY_PROV = "hstprov";
	public static final String ID_CONFIGURE_RT_PROV = "rtprov";
	public static final String ID_CONFIGURE_SRV = "srv";
	public static final String ID_CONFIGURE_SIMUL_PROF = "simprof";
	public static final String ID_GONFIGURE_EVENT_PLUGIN = "eventplugin";
	public static final String ID_GONFIGURE_STARTUP_CONST = "startup";
	// -------------------------------------
	private StringBuilder propertyName_m = new StringBuilder();

	// -------------------------------------
	public String getPropertyName() {
		return propertyName_m.toString();
	}

	public void setPropertyName(String name) {
		propertyName_m.setLength(0);
		propertyName_m.append(name);
	}

	public static HtBaseProp instantiate(XmlParameter xmlparameter) throws Exception {
		String type = xmlparameter.getCommandName();
		if (type.equals(ID_CONFIGURE_HISTORY_PROV)) {
			return new HtHistoryProviderProp();
		} else if (type.equals(ID_CONFIGURE_RT_PROV)) {
			return new HtRTProviderProp();
		} else if (type.equals(ID_CONFIGURE_DATA_PROFILE)) {
			return new HtDataProfileProp();
		} else if (type.equals(ID_CONFIGURE_ALG_BROKER)) {
			return new HtAlgBrkPairProp();
		} else if (type.equals(ID_CONFIGURE_SRV)) {
			return new HtServerProp();
		} else if (type.equals(ID_CONFIGURE_SIMUL_PROF)) {
			return new HtSimulationProfileProp();
		} else if (type.equals(ID_GONFIGURE_EVENT_PLUGIN)) {
			return new HtEventPluginProp();
		} else if (type.equals(ID_GONFIGURE_STARTUP_CONST)) {
			return new HtStartupConstants();
		}


		throw new HtException("HtBaseProp", "instantiate", "Invalid parameter type: " + type);

	}

	public String getType() {
		return type_m;
	}

	
	// type of property
	protected String type_m = null;
}
