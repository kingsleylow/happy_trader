/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.hlpstruct.externalrunstruct;

import java.util.LinkedHashMap;
import java.util.Map;

/**
 *
 * @author Victor_Zoubok
 */
public class RunParameterStringMap extends RunParameter {

	
	public RunParameterStringMap()
	{
		type_m = RunParameter.STRING_MAP_TYPE;
	}
	
	public RunParameterStringMap(Map<String, String> data)
	{
		this();
		data_m.putAll( data );
	}
	
	public Map<String,String> getStringMap()
	{
		return data_m;
	}
	
		
	
	private Map<String, String> data_m = new LinkedHashMap<String,String>();
}
