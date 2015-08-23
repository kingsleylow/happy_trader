/*
 * HtEventPluginProp.java
 *
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.configprops;

import com.fin.httrader.utils.HtEnumeratorDescriptor;
import java.util.LinkedHashMap;
import java.util.Map;

/**
 *
 * @author Victor_Zoubok
 */
public class HtEventPluginProp extends HtBaseProp {
		
	public static final int LAUNCH_DUMMY = 0;
	public static final int LAUNCH_INTERNAL = 1;
	public static final int LAUNCH_EXT_JAR = 2;
	public static final int LAUNCH_EXT_PROVIDER = 3;
	
	private static HtEnumeratorDescriptor descriptor_m = new HtEnumeratorDescriptor("com.fin.httrader.configprops.HtEventPluginProp");

	public static String getLaunchFlagName(int pt) {
		return descriptor_m.getNameById_StrictValue(pt);
	}
	
	public static int getLaunchFlagIdByName(String name) {
		return descriptor_m.getIdByName_StrictValue(name);
	}
		
	public static Map<Integer, String> getLaunchFlagNameMap() {
		return descriptor_m.getNameMap();
	}

	/** Creates a new instance of HtEventPluginProp */
	public HtEventPluginProp() {
		type_m = HtBaseProp.ID_GONFIGURE_EVENT_PLUGIN;
	}
	
	public Map<String, String> getParameters()
	{
		return initParams_m;
	}
	
	public String getPluginClass()
	{
		return pluginClass_m.toString();
	}
	
	public void setPluginClass(String plugClass)
	{
		pluginClass_m.setLength(0);
		pluginClass_m.append(plugClass);
	}
	
	public int getLaunchFlag() {
				return launchFlag_m;
	}

	public void setLaunchFlag(int launchFlag) {
				this.launchFlag_m = launchFlag;
	}
	
	@HtSaveAnnotation
	private final LinkedHashMap<String, String> initParams_m = new LinkedHashMap<String, String>();
	
	@HtSaveAnnotation
	private final StringBuilder pluginClass_m = new StringBuilder();
	
	@HtSaveAnnotation
	private int launchFlag_m = LAUNCH_DUMMY;

		

	
}
