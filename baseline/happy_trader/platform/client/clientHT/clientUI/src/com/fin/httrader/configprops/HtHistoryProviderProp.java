/*
 * HtHistoryProviderProp.java
 *
 * 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.configprops;


import java.util.LinkedHashMap;
import java.util.Map;

/**
 *
 * @author Victor_Zoubok
 */
public class HtHistoryProviderProp extends HtBaseProp {

	public HtHistoryProviderProp() {
		type_m = HtBaseProp.ID_CONFIGURE_HISTORY_PROV;
	}
	
	public int getExportHourShift()
	{
		return exportHourShift_m;
	}
	
	public int getImportHourShift()
	{
		return importHourShift_m;
	}
	
	public int setExportHourShift(int shft)
	{
		return exportHourShift_m = shft;
	}
	
	public int setImportHourShift(int shft)
	{
		return importHourShift_m = shft;
	}
	
	public Map<String, String> getParameters()
	{
		return initParams_m;
	}

	public String getProviderClass()
	{
		return providerClass_m.toString();
	}
	
	public void setProviderClass(String p_class)
	{
		providerClass_m.setLength(0);
		providerClass_m.append(p_class);
	}
	
	@HtSaveAnnotation
	private int exportHourShift_m = 0;
	
	@HtSaveAnnotation
	private int importHourShift_m = 0;
	
	@HtSaveAnnotation
	private LinkedHashMap<String, String> initParams_m = new LinkedHashMap<String, String>();
	
	
	@HtSaveAnnotation
	private StringBuilder providerClass_m = new StringBuilder();

	
}
