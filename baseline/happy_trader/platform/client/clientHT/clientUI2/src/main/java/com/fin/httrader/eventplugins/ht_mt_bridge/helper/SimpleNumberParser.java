/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.eventplugins.ht_mt_bridge.helper;

import com.fin.httrader.utils.HtUtils;

/**
 *
 * @author Victor_Zoubok
 */
public class SimpleNumberParser {
		
	public SimpleNumberParser()
	{
	}
	
	public double parseDouble(String value)
	{
			return HtUtils.parseDouble(value);
	}
	
	public int parseInt(String value)
	{
			return HtUtils.parseInt(value);
	}
	
	public long parseLong(String value)
	{
			return HtUtils.parseLong(value);
	}
}
