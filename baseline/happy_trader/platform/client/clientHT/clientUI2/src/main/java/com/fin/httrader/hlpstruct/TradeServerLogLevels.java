/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.hlpstruct;

import com.fin.httrader.utils.HtEnumeratorDescriptor;
import java.util.Map;
import java.util.TreeMap;

/**
 *
 * @author DanilinS
 */
public class TradeServerLogLevels {
	public static final int FATAL = 0;
	public static final int ERROR = 1;
	public static final int WARN = 2;
	public static final int INFO = 3;
	public static final int DEBUG = 4;
	public static final int DEBUG_DETAIL = 5;

	private static HtEnumeratorDescriptor descriptor_m = new HtEnumeratorDescriptor("com.fin.httrader.hlpstruct.TradeServerLogLevels");



	public static Map<Integer, String> getAllLogLevelsMap()
	{
		return descriptor_m.getNameMap();
		
	}

	public static String getLogLevelName(int loglevel) {

		return descriptor_m.getNameById_StrictValue(loglevel, "");
	}

}
