/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.mtrunner.client.managers;

import java.io.File;
import java.util.Properties;
import javax.annotation.PostConstruct;
import javax.annotation.Resource;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.ApplicationContext;
import org.springframework.context.annotation.Configuration;
import org.springframework.context.annotation.PropertySource;
import org.springframework.context.support.ClassPathXmlApplicationContext;
import org.springframework.core.env.Environment;
import org.springframework.stereotype.Component;


/**
 *
 * @author Administrator
 */

public class SettingsManager {
	
	public static String CHANGE_CONSTANT="9F94C4610E4B4F13BAC477888B2D3744";
	
	private String happyTraderUrl;
	private String happyTraderUser;
	private String happyTraderPassword;
	
	private String HappyTraderBridgeLog;
	private String ForwardIp;
	private int HistoryPositionPageSize;
	private String baseDir;

	
	private static volatile SettingsManager instance_m = null;
	
	public static SettingsManager getInstance()
	{
		if (instance_m == null) {
			synchronized(SettingsManager.class) {
				instance_m = new SettingsManager();
			}
		}
		return instance_m;
	}
	
	
	private SettingsManager()
	{
		
	}
	
	public int getHistoryPositionPageSize()
	{
		return HistoryPositionPageSize;
	}

	public void setHistoryPositionPageSize(int ps) {
		this.HistoryPositionPageSize = ps;
	}
	
	public String getForwardIp() {
		return ForwardIp;
	}

	public void setForwardIp(String ForwardIp) {
		this.ForwardIp = happyTraderUrl;
	}
	
	
	public String getHappyTraderBridgeLog() {
		return HappyTraderBridgeLog;
	}

	public void setHappyTraderBridgeLog(String HappyTraderBridgeLog) {
		this.HappyTraderBridgeLog = HappyTraderBridgeLog;
	}
	
	public String getHappyTraderUrl() {
		return happyTraderUrl;
	}

	public void setHappyTraderUrl(String happyTraderUrl) {
		this.happyTraderUrl = happyTraderUrl;
	}

	public String getHappyTraderUser() {
		return happyTraderUser;
	}

	public void setHappyTraderUser(String happyTraderUser) {
		this.happyTraderUser = happyTraderUser;
	}

	public String getHappyTraderPassword() {
		return happyTraderPassword;
	}

	public void setHappyTraderPassword(String happyTraderPassword) {
		this.happyTraderPassword = happyTraderPassword;
	}

	public String getBaseDir() {
		return baseDir;
	}

	public void setBaseDir(String baseDir) {
		this.baseDir = baseDir;
	}
	

	/**
	 * 
	 */
	
	public String getMtDistrDir()
	{
		String base = baseDir + File.separatorChar + "mtdistr";
		return base;
	}
	
	public String getMtTragetBaseDir()
	{
		String base = baseDir + File.separatorChar + "installation";
		return base;
	}
	
	public String getMtDataBaseDir()
	{
		String base = baseDir + File.separatorChar + "data";
		return base;
	}
	
	public String getMtSetupXml()
	{
		String base = baseDir + File.separatorChar + "setup.xml";
		return base;
	}
	
}
