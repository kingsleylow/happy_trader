/*
 * RtWebServletManager.java
 *
  *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.managers;



import com.fin.httrader.configprops.HtStartupConstants;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;

import com.fin.httrader.utils.hlpstruct.XmlHandler;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import com.fin.httrader.utils.HtSystem;
import java.io.BufferedReader;
import java.io.File;
import winstone.Launcher;


import java.io.FileReader;
import java.util.Calendar;
import java.util.HashMap;
import java.util.Map;

import java.util.logging.Level;

/**
 *
 * @author victor_zoubok
 * This class is responsible for processing input commands
 * And make some output as a reaction on these commands
 */
public class RtWebServletManager implements RtManagerBase {

	private Launcher winstone_m = null;
	private XmlHandler xmlEventHandler_m = new XmlHandler();
	private StringBuilder webRootPath_m = new StringBuilder();
	
	static private RtWebServletManager instance_m = null;
	
	static public RtWebServletManager create(int httpport, String connectUrl,  String winstoneLogLevel) throws Exception {
		if (instance_m == null) {
			instance_m = new RtWebServletManager(httpport, connectUrl, winstoneLogLevel);
		}

		return instance_m;
	}

	static public RtWebServletManager instance() {

		return instance_m;
	}

	protected String getContext() {
		return this.getClass().getSimpleName();
	}

	/*
	public String getWebRoot() {
		return webRootPath_m.toString();
	}
	 */

	public String getImgTempStorageUrl() {
		return "/report/imgs/";
	}

	public String resolveImgTempStorage() {
		String path = webRootPath_m.toString() + "/report/imgs/";
		File f = new File(path);
		f.mkdirs();
		return f.getAbsolutePath();
	}

	private RtWebServletManager(int httpport, String connectUrl, String winstoneLogLevel) throws Exception {

		// start server
		Map args = new HashMap();

		webRootPath_m.append(HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getWebRootDir());
		args.put("webroot", webRootPath_m.toString());    // or any other command line args, eg port
		args.put("httpPort", String.valueOf(httpport));
		args.put("ajp13Port", -1);

		// no directory listing
		args.put("directoryListings", "false");

		// args.put("webappsDir", HtMain.getBaseDirectory() + "/console");
		// args.put("hostsDir", HtMain.getBaseDirectory() + "/console");
		args.put("useJasper", "true");

		// JNDI usage
		// java:/comp/env/jdbc/connPool
		
		args.put("useJNDI", "true");
		args.put("jndi.resource." + HtStartupConstants.JNDI_CONNECT_POOL_ENTRY_NAME, "javax.sql.DataSource"); // name of class
		args.put("jndi.param."+HtStartupConstants.JNDI_CONNECT_POOL_ENTRY_NAME+".url", connectUrl);
		args.put("jndi.param."+HtStartupConstants.JNDI_CONNECT_POOL_ENTRY_NAME+".driverClassName", HtStartupConstants.MYSQL_DB_DRIVER);
		args.put("jndi.param."+HtStartupConstants.JNDI_CONNECT_POOL_ENTRY_NAME+".keepAliveSQL", "select 1");
		args.put("jndi.param."+HtStartupConstants.JNDI_CONNECT_POOL_ENTRY_NAME+".keepAlivePeriod", String.valueOf(  RtConfigurationManager.StartUpConst.WINSTONE_EXECUTE_ALIVE_CONNECTION_MINS )); // 1 minute
		
		
		// check if this class can be loaded
		
		//args.put("jndi.param.jdbc/connPool.username","");
		//args.put("jndi.param.jdbc/connPool.password","");


		StringBuilder logname = new StringBuilder();

		logname.append(HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getLogDirectory());
		logname.append("/htwebsrv_");

		Calendar cld = Calendar.getInstance();

		logname.append(String.format("%1$td-%1$tm-%1$tY_%1$tH-%1$tM-%1$tS", cld.getTimeInMillis()));
		logname.append(".log");
		args.put("logfile", logname.toString());

		//
		
		String javaHomeDir = HtSystem.getEnvironmentValue("JAVA_HOME");
		if (javaHomeDir == null || javaHomeDir.length() <= 0) {
			//throw new HtException(getContext(), "RtWebServletManager", sysJavaHome);
			HtLog.log(Level.WARNING, getContext(), getContext(),"JAVA_HOME is not set, thus trying to guess from sys property 'java.home'");


			// we have sys java home so will try to get smth from here
			javaHomeDir = HtSystem.resolveJavaHome();
		}
		
		String toolsFile = javaHomeDir + File.separator + "lib" + File.separator +"tools.jar";

		// if we have the correct path -  just check it
		File f_toolsFile = new File(toolsFile);
		if (!f_toolsFile.exists())
			throw new HtException(getContext(), "RtCommandManager", "Path to tools.jar does is not valid: " + toolsFile);

		// need to set up java home

		
		HtLog.log(Level.INFO, getContext(), getContext(),"JAVA_HOME variable set to: \"" + javaHomeDir + "\"");
		HtLog.log(Level.INFO, getContext(), getContext(), "tools.jar is located: \"" + toolsFile + "\"");

		

		// System.setProperty("java.home", javaHomeDir);
		args.put("javaHome", javaHomeDir);
		args.put("toolsJar", toolsFile);

		// String dummyjavaHome = WebAppConfiguration.stringArg(args, "javaHome", sysJavaHome);
		// String toolsJarLocation = WebAppConfiguration.stringArg(args, "toolsJar", null);
		Launcher.initLogger(args);
		winstone_m = new Launcher(args);

		// HTTP Listener started
		// or
		// Error during HTTP listener init
		checkForStartupExceptions(logname.toString());
		HtLog.log(Level.INFO, getContext(), getContext(),"Instantiated, HTTP server listens on: " + httpport);

		// 1...9
		//

		
		if (winstoneLogLevel.equalsIgnoreCase("MIN")) {
			winstone.Logger.setCurrentDebugLevel(winstone.Logger.MIN);
		} else if (winstoneLogLevel.equalsIgnoreCase("ERROR")) {
			winstone.Logger.setCurrentDebugLevel(winstone.Logger.ERROR);
		} else if (winstoneLogLevel.equalsIgnoreCase("WARNING")) {
			winstone.Logger.setCurrentDebugLevel(winstone.Logger.WARNING);
		} else if (winstoneLogLevel.equalsIgnoreCase("INFO")) {
			winstone.Logger.setCurrentDebugLevel(winstone.Logger.INFO);
		} else if (winstoneLogLevel.equalsIgnoreCase("SPEED")) {
			winstone.Logger.setCurrentDebugLevel(winstone.Logger.SPEED);
		} else if (winstoneLogLevel.equalsIgnoreCase("DEBUG")) {
			winstone.Logger.setCurrentDebugLevel(winstone.Logger.DEBUG);
		} else if (winstoneLogLevel.equalsIgnoreCase("FULL_DEBUG")) {
			winstone.Logger.setCurrentDebugLevel(winstone.Logger.FULL_DEBUG);
		} else if (winstoneLogLevel.equalsIgnoreCase("MAX")) {
			winstone.Logger.setCurrentDebugLevel(winstone.Logger.MAX);
		} else {
			throw new HtException(getContext(), "RtCommandManager", "Invalid Winstone log level: " + winstoneLogLevel);
		}

	}

	public void shutdown() {
		try {

			// TODO need to enforce shutdown even connections are alive


			winstone_m.shutdown();

			
			// remove tmp storage
			clearTmpImgDirectory(resolveImgTempStorage());

		} catch (Throwable e) {
			HtLog.log(Level.WARNING, getContext(), "shutdown",
					"Exception on shut down command manager: " + e.getMessage(), null);
		}

		HtLog.log(Level.INFO, getContext(), "shutdown", "Command HTTP server finished");
	}

	
	// ---------------------
	// helpers

	//
	void checkForStartupExceptions(String logfile) throws Exception
	{
		// HTTP Listener started
		// or
		// Error during HTTP listener init
		boolean except = false;
		BufferedReader br = null;
		try {
			File file = new File(logfile);
			br = new BufferedReader(new FileReader(logfile));

			String line = null;
			while((line=br.readLine())!=null){
				if (line.indexOf("Error during HTTP listener init") >=0) {
					except = true;
					break;
				}

			}

		}
		catch(Throwable e)
		{
			HtLog.log(Level.WARNING, getContext(), "checkForStartupExceptions", e.getMessage());
		}
		finally
		{
			if (br != null)
				br.close();

			
		}

		if (except)
			throw new HtException(getContext(), "checkForStartupExceptions", "The log file says that HTTP listener was not started, probably another instance is running");

	}

	private String createErrorXml(int errCode, String errMessage) {
		StringBuilder out = new StringBuilder();

		try {
			XmlHandler hndlr = new XmlHandler();
			XmlParameter param = new XmlParameter();

			param.setCommandName("global_response");
			param.setInt("global_status_code", errCode);
			param.setString("global_status_message", errMessage);
			hndlr.serializeParameter(param, out);
		} catch (Throwable e) {
			// ignore
		}

		return out.toString();
	}

	private void clearTmpImgDirectory(String root) {
		try {
			File f = new File(root);
			if (f != null) {


				if (f.isDirectory()) {
					File[] inside = f.listFiles();
					if (inside != null) {
						for (int i = 0; i < inside.length; i++) {
							if (inside[i].isDirectory()) {
								clearTmpImgDirectory(inside[i].getAbsolutePath());
							} else {
								inside[i].delete();
							}
						}
					}
				}

				// last delete
				f.delete();

			}
		} catch (Throwable e) {
			HtLog.log(Level.WARNING, getContext(), "clearTmpImgDirectory", "Exception on deleting temporary img directory: " + e.getMessage());
		}
	}
}


//~ Formatted by Jindent --- http://www.jindent.com
