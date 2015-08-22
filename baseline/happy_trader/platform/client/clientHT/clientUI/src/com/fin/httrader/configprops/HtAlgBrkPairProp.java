/*
 * HtAlgBrkPairProp.java
 *
 * Created on 5  2008 ., 13:50
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
public class HtAlgBrkPairProp extends HtBaseProp {

	/**
	 * Creates a new instance of HtAlgBrkPairProp
	 */
	public HtAlgBrkPairProp() {
		type_m = HtBaseProp.ID_CONFIGURE_ALG_BROKER;
	}
	
	public String getAlgorithmPath()
	{
		return algorithmPath_m.toString();
	}
	
	public String getBrokerPath()
	{
		return brokerPath_m.toString();
	}
	
	public String getBrokerPath2()
	{
		return brokerPath2_m.toString();
	}
	
	public void setAlgorithmPath(String path)
	{
		algorithmPath_m.setLength(0);
		algorithmPath_m.append(path);
	}
	
	public void setBrokerPath(String path)
	{
		brokerPath_m.setLength(0);
		brokerPath_m.append(path);
	}
	
	public void setBrokerPath2(String path)
	{
		brokerPath2_m.setLength(0);
		brokerPath2_m.append(path);
	}
	
	public Map<String, String> getAlgoritmParams()
	{
		return algParams_m;
	}
	
	public Map<String, String> getBrokerParams()
	{
		return brokerParams_m;
	}
	
	public Map<String, String> getBrokerParams2()
	{
		return brokerParams2_m;
	}
	
	@HtSaveAnnotation
	private StringBuilder algorithmPath_m = new StringBuilder();
	
	@HtSaveAnnotation
	private StringBuilder brokerPath_m = new StringBuilder();
	
	@HtSaveAnnotation
	private StringBuilder brokerPath2_m = new StringBuilder();
	
	@HtSaveAnnotation
	private LinkedHashMap<String, String> algParams_m = new LinkedHashMap<String, String>();
	
	@HtSaveAnnotation
	private LinkedHashMap<String, String> brokerParams_m = new LinkedHashMap<String, String>();
	
	@HtSaveAnnotation
	private LinkedHashMap<String, String> brokerParams2_m = new LinkedHashMap<String, String>();


}
