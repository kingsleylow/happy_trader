/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.model;

import com.fin.httrader.managers.RtConsoleManager;
import com.fin.httrader.utils.Uid;
import com.fin.httrader.utils.hlpstruct.XmlParameter;

/**
 *
 * @author DanilinS
 */
public class HtConsoleProxy extends HtProxyBase{

	public HtConsoleProxy() {
	}

	private String getContext() {
		return this.getClass().getSimpleName();
	}

	@Override
	public String getProxyName() {
		return "console_proxy";
	}

	public void remote_consoleLoop()
	{
		

		RtConsoleManager.instance().consoleLoop();
	}



}
