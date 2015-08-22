/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.model;

import com.fin.httrader.managers.RtBootManager;
import com.fin.httrader.utils.hlpstruct.XmlParameter;

/**
 *
 * @author DanilinS
 */
public class HtBootProxy extends HtProxyBase {


	public HtBootProxy() {
	}

	

	@Override
	public String getProxyName() {
		return "boot_proxy";
	}

	public void remote_startServer(String[] args)
	{
		RtBootManager.instance().startServer(args);
	}

	public void remote_shutdown()
	{
		RtBootManager.instance().shutdown();
	}
}
