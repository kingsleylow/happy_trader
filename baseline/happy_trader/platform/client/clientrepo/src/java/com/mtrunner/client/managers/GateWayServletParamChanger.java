/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.mtrunner.client.managers;

import com.mtrunner.client.servlets.HappyTraderProxyGateServlet;

/**
 *
 * @author Administrator
 */
public class GateWayServletParamChanger implements HappyTraderProxyGateServlet.ParamSubstitutor{

	private static volatile GateWayServletParamChanger instance_m = null;
	
	public static GateWayServletParamChanger getInstance()
	{
		if (instance_m == null) {
			synchronized(GateWayServletParamChanger.class) {
				instance_m = new GateWayServletParamChanger();
			}
		}
		return instance_m;
	}
	
	
	@Override
	public String changeStringParameter(String commandName, String paramName) {
		
		if (commandName == null)
			return null;
		
		if (commandName.equalsIgnoreCase("RESTART_INSTANCE_REQ")) {
			if (paramName.equalsIgnoreCase("SETTINGS_FILE")) {
				return SettingsManager.getInstance().getMtSetupXml();
			}
		}
		else if (commandName.equalsIgnoreCase("START_INSTANCE_REQ")) {
			if (paramName.equalsIgnoreCase("SETTINGS_FILE")) {
				return SettingsManager.getInstance().getMtSetupXml();
			}
		}
		
		return null;
	}

	
}
