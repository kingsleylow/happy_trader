/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.hlpstruct.externalrunstruct;

/**
 *
 * @author Victor_Zoubok
 */
public class RunParameterString extends RunParameter{

	public RunParameterString()
	{
		type_m = RunParameter.STRING_TYPE;
	}
	
	public RunParameterString(String data)
	{
		this();
		data_m = data;
	}
					
	public String getString()
	{
		return data_m;
	}
	
	private String data_m = null;


	
}
