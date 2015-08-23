/*
 * HtServerProp.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.configprops;

import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.Map;

/**
 *
 * @author Victor_Zoubok
 */
public class HtServerProp extends HtBaseProp {

		public static final int	 MSG_FATAL = 0;
		public static final int  MSG_ERROR  = 1;
		public static final int  MSG_WARN  = 2;
		public static final int  MSG_INFO  = 3;
		public static final int  MSG_DEBUG  = 4;
		public static final int  MSG_DEBUG_DETAIL = 5;

	/** Creates a new instance of HtServerProp */
	public HtServerProp() {
		type_m = HtBaseProp.ID_CONFIGURE_SRV;

		// be default we subscribe for all events
		for (Iterator<Integer> it = XmlEvent.getAllAvalableEventsForServer().iterator(); it.hasNext();) {
			subscribedEvents_m.put(it.next(), true);
		}
	}
	
	public boolean isValid()
	{
			if (cmdPort_m <=0)
					return false;
			
			if (logLevel_m < CommonLog.LL_FATAL || logLevel_m > CommonLog.LL_DEBUGDETAIL)
					return false;
			
			if (htDebugLevel_m < MSG_FATAL || htDebugLevel_m > MSG_DEBUG_DETAIL)
					return false;
			
			return true;
	}
	
	public int getCommandPort()
	{
		return cmdPort_m;
	}
	
	public void setCommandPort(int cmdPort)
	{
		cmdPort_m = cmdPort;
	}
	
	public int getSpawnerPort()
	{
		return spawnerPort_m;
	}
	
	public void setSpawnerPort(int spawnPort)
	{
		spawnerPort_m = spawnPort;
	}
	
	public String getServerExecutable()
	{
		return serverExecutable_m.toString();
	}
	
	public void setServerExecutable(String srvExecutable)
	{
		serverExecutable_m.setLength(0);
		serverExecutable_m.append(srvExecutable);
	}
	
	public Map<Integer, Boolean> getSubscribedEvents()
	{
		return subscribedEvents_m;
	}
	
	public int getLogEventLevel()
	{
		return logLevel_m;
	}
	
	public void setLogEventLevel(int level)
	{
		logLevel_m = level;
	}
	
	public int getDebuglevel()
	{
		return htDebugLevel_m;
	}
	
	public void setDebugLevel(int level)
	{
		htDebugLevel_m = level;
	}
	
	public String getServerHost()
	{
		return serverHost_m.toString();
	}
	
	public void setServerHost(String srvExecutable)
	{
		serverHost_m.setLength(0);
		serverHost_m.append(srvExecutable);
	}
	
	public boolean isUseSpawner()
	{
		return useSpawner_m;
	}
	
	public void setUseSpawner(boolean isUseSpawner)
	{
		useSpawner_m = isUseSpawner;
	}
	
	// command port
	@HtSaveAnnotation
	private int cmdPort_m = -1;

	// remote spawner port
	@HtSaveAnnotation
	private int spawnerPort_m = -1;

	@HtSaveAnnotation
	private final StringBuilder serverExecutable_m = new StringBuilder();
	
	// subscribed events
	@HtSaveAnnotation
	private final LinkedHashMap<Integer, Boolean> subscribedEvents_m = new LinkedHashMap<Integer, Boolean>();
	
	// log level
	@HtSaveAnnotation
	private int logLevel_m = CommonLog.LL_DEBUGDETAIL;

	// internal server log level
	@HtSaveAnnotation
	private int htDebugLevel_m=3;

	// server host
	@HtSaveAnnotation
	private final StringBuilder serverHost_m = new StringBuilder("127.0.0.1");

	@HtSaveAnnotation
	private boolean useSpawner_m = false;


}
