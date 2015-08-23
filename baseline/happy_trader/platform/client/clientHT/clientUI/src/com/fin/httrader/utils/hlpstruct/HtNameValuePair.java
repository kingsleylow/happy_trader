/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.utils.hlpstruct;

import org.apache.http.NameValuePair;

/**
 *
 * @author DanilinS
 */
public class HtNameValuePair implements NameValuePair{

	private String key = null;
	private String value = null;

	public HtNameValuePair()
	{
	}

	public HtNameValuePair(String _name,String _value )
	{
		key = _name;
		value = _value;
	}

	@Override
	public String getName() {
		return key;
	}

	@Override
	public String getValue() {
		return value;
	}

	public void setName(String _name)
	{
		key = _name;
	}

	public void setValue(String _value)
	{
		value = _value;
	}

}
