/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.hlpstruct.externalrunstruct;

import java.util.ArrayList;
import java.util.List;

/**
 *
 * @author Victor_Zoubok
 */
public class RunParameterStringList extends RunParameter {

	public RunParameterStringList()
	{
		type_m = RunParameter.STRING_LIST_TYPE;
	}
	
	public RunParameterStringList(List<String> data)
	{
		this();
		data_m.addAll( data );
	}
	
	public List<String> getStringList()
	{
		return data_m;
	}
	
	

	
	
	
	private List<String> data_m = new ArrayList<String>();
}
