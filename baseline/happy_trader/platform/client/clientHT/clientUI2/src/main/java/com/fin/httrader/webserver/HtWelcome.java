/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.HtMain;
import com.fin.httrader.configprops.HtStartupConstants;

/**
 *
 * @author Victor_Zoubok
 */
public class HtWelcome extends HtServletsBase {

	

	public String getVersionInfo()
	{
		return HtStartupConstants.VERSION;
	}

	
}
