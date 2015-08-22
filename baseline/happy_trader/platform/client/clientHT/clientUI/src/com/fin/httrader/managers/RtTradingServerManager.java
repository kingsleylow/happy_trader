/*
 * RtTradingServerManager.java
 *
  *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.managers;

import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.model.HtServerProxy;
import com.fin.httrader.utils.*;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import com.fin.httrader.utils.hlpstruct.XmlHandler;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import java.io.File;
import java.util.Calendar;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;
import java.util.Vector;
import java.util.logging.Level;

/**
 *
 * @author victor_zoubok
 * This class is responsible for managig trading servers
 * This is singleton class
 */
public class RtTradingServerManager implements RtManagerBase {
	
	// const about state of the trading server
	public static final int SERVER_IDLE = 1;

	public static final int SERVER_RUN_OK = 2;

	public static final int SERVER_RTTHREAD_ERROR = 3;


	
	// ------------------------------------------
	private final HashMap servers_m = new HashMap();
	static private RtTradingServerManager instance_m = null;

	static public RtTradingServerManager create() throws Exception {
		if (instance_m == null) {
			instance_m = new RtTradingServerManager();
		}

		return instance_m;
	}

	static public RtTradingServerManager instance() {

		return instance_m;
	}

	protected String getContext() {
		return this.getClass().getSimpleName();
	}

	private RtTradingServerManager() {
		HtLog.log(Level.INFO, getContext(), getContext(), "Instantiated");
	}

	// spawns server process and forces the server to listen the passed port
	// then instantiates RtTradingServer class and connects to the server
	public XmlParameter initServerProcess(
					String servername,
					String serverExecutable,
					Map<String, String> newEntries,
					String serverHost,
					int serverPort,
					int servicePort,
					int spawnerPort,
					boolean useSpawner,
					XmlParameter initialrequest,
					boolean do_not_create_process,
					//boolean force_debug_output,
					Set<String> rtProvidersToSubscribe, // rt providers to subscribe
					boolean loadRtThread // whether to load RT thread
	) throws Exception {


		// check whether we have standard classes
		RtTradingServer cur_server = new RtTradingServer(
						servername,
						serverExecutable,
						newEntries,
						serverHost,
						serverPort,
						servicePort,
						spawnerPort,
						useSpawner,
						do_not_create_process,
						//force_debug_output,
						rtProvidersToSubscribe);

		// start service therads
		if (loadRtThread) {
			cur_server.startAndWaitForServiceThreads(5000);
		}


		registerNewServer(servername, cur_server);

		XmlParameter response = cur_server.issueCommand(initialrequest);

		// must have here log file name
		String logFileName = response.getString("log_file_name");
		cur_server.setUpLogFileName(logFileName);

		
		return response;
	}
	
	

	public void normalShutDown(String servername) throws Exception {

		HtLog.log(Level.INFO, getContext(), "normalShutDown", "Server will be normally shutdown");

		// launvh the command
		RtTradingServer cur_server = getTradingServer(servername);

		cur_server.normalShutdown();
		unregisterServer(servername);

		HtLog.log(Level.INFO, getContext(), "normalShutDown", "Server was shut down: " + servername);
	}

	// destroy server process and force to stop it
	public void killServerProcess(String serverName) {

		HtLog.log(Level.INFO, getContext(), "killServerProcess", "Server will be killed: " + serverName);


		// unsubscribe
		try {
			RtTradingServer curServer = unregisterServer(serverName);
			curServer.deinitialize();

		} catch (Throwable e) {
			HtLog.log(Level.WARNING, getContext(), "killServerProcess", "Exception on killing server: " + serverName + " - " + e.getMessage());
		}


		HtLog.log(Level.INFO, getContext(), "killServerProcess", "Server was killed: " + serverName);
	}

	public void shutdown() {

		Vector<String> allServers = getAllRegisteredServerName();
		for (int i = 0; i < allServers.size(); i++) {
			final String servername_i = allServers.get(i);

			try {

				// we have more tricky soultion here
				// as this is server shutdown will wait 15 seconds per server
				// and in case of no response just kill it
				Thread killer = new Thread() {

					public void run() {
						try {
							normalShutDown(servername_i);
						} catch (Throwable e) {
							HtLog.log(Level.WARNING, "killer", "run", "Exception on normal shutdown in killer thread for: " + servername_i + " - " + e.getMessage());
						}
					}
				};

				// wait 15 secs, ignoring all errors
				try {
					killer.start();
					killer.join(15000);
				} catch (InterruptedException eo) {
				} catch (Throwable e) {
				}


				
				if (returnServerStatus(servername_i) != this.SERVER_IDLE) {
					killServerProcess(servername_i);
				};

			} catch (Throwable e) {
				HtLog.log(Level.WARNING, getContext(), "shutdown", "Exception on final shutdown server process identified by: " + servername_i + " - " + e.getMessage());
			}


		} // end loop



		HtLog.log(Level.INFO, getContext(), "shutdown", "Finished");
	}
	
	// cluster functions
	

	// ---------------------------------------------
	//
	public int numberOfRunningServers() {
		synchronized (servers_m) {
			return servers_m.size();
		}
	}

	public int returnServerStatus(String servername) {
		synchronized (servers_m) {
			if ( servers_m.containsKey(servername) ) {
				if ( ((RtTradingServer) servers_m.get(servername)).isRtThreadError())
					return RtTradingServerManager.SERVER_RTTHREAD_ERROR;
				else
					return RtTradingServerManager.SERVER_RUN_OK;
			}
			else {
				return RtTradingServerManager.SERVER_IDLE;
			}
		}
	}

	public RtTradingServer getTradingServer(String serverName) throws Exception {

		synchronized (servers_m) {
			if (!servers_m.containsKey(serverName)) {
				throw new HtException(getContext(), "getTradingServer", "Server process is NOT registered via its name: " + serverName);
			}

			return (RtTradingServer) servers_m.get(serverName);
		}
	}

	public String getLogFileName(String serverName) throws Exception {

		synchronized (servers_m) {
			if (!servers_m.containsKey(serverName)) {
				throw new HtException(getContext(), "getTradingServer", "Server process is NOT registered via its name: " + serverName);
			}

			return ((RtTradingServer) servers_m.get(serverName)).getLogFileName();
		}
	}

	
	
	// -----------------

	// event- related functions
	// server request event
	public void pushLocalServerEvent(
					String serverName,
					int resultCode,
					String message) {
		try {

			int logLevel = CommonLog.LL_INFO;
			XmlEvent event = new XmlEvent();
			event.setEventType(XmlEvent.ET_CommonLog);

			// session
			event.getAsLog(true).setSession(serverName);

			StringBuilder out = new StringBuilder();

			if (resultCode == HtServerProxy.SRV_COMMAND_LOCAL_EXCEPTION) {
				out.append("LOCAL EXCEPTION: ").append((message != null ? message : "").toString());
				logLevel = CommonLog.LL_ERROR;

			} else {
				out.append("OK: ").append((message != null ? message : "").toString());
			}


			event.getAsLog(true).setContext("Trading server manager");
			event.getAsLog().setLogThread(Thread.currentThread().getId());
			//event.getAsLog().getAttributeList().add(String.valueOf(resultCode));
			event.getAsLog().addContentedAttribute( String.valueOf(resultCode) );
			event.getAsLog().setLogLevel(logLevel);
			event.getAsLog().setContent(out.toString());
			event.getAsLog().setLogDate(Calendar.getInstance().getTimeInMillis());


			// push data to event manager
			RtGlobalEventManager.instance().pushCommonEvent(event);

		//pushEventData( event);
		} catch (Throwable e) {
			HtLog.log(Level.WARNING, getContext(), "pushLocalServerEvent", "Exception on pushing event: " + e.getMessage());
		}
	}

	//
	public void pushRemoteServerEvent(
					String serverName,
					XmlParameter request,
					XmlParameter response,
					XmlHandler xmlHandler) {
		try {
			int resultCode = -1;
			int logLevel = CommonLog.LL_INFO;

			XmlEvent event = new XmlEvent();
			event.setEventType(XmlEvent.ET_CommonLog);

			// session
			event.getAsLog(true).setSession(serverName);
			StringBuilder out = new StringBuilder();
			StringBuilder outRequest = new StringBuilder();
			StringBuilder outResponse = new StringBuilder();


			if (request != null) {
				
				xmlHandler.serializeParameter(request, outRequest);

				// we are not crazey to log MBs of large request
				if (outRequest.length() > 10000) {
					outRequest.setLength(10000);
					outRequest.append("...");
				}
	
			} 


			if (response != null) {
				
				xmlHandler.serializeParameter(response, outResponse);
			
				long statusCode = -1;
				String statusMsg = null;


				if (response.getKeys().contains("status_code")) {
					statusCode = response.getInt("status_code");

					if (response.getKeys().contains("status")) {
						statusMsg = response.getString("status");
					}

					if (statusMsg == null) {
						statusMsg = "";
					}

					//
					// exception
					if (statusCode != 0) {

						resultCode = HtServerProxy.SRV_COMMAND_REMOTE_EXCEPTION;
						logLevel = CommonLog.LL_ERROR;
						out.append("REMOTE EXCEPTION: ").append(statusMsg.toString());
					} else {
						// OK

						resultCode = HtServerProxy.SRV_COMMAND_OK;
						out.append("OK: ").append(statusMsg.toString());

					}
				} else {
					// no valid response
					resultCode = HtServerProxy.SRV_COMMAND_NA;
					out.append("N/A");

				}
			} 

			if (resultCode <= 0) {
				resultCode = HtServerProxy.SRV_COMMAND_OK;
			}


			event.getAsLog(true).setContext("Trading server manager");
			event.getAsLog().setLogThread(Thread.currentThread().getId());
			event.getAsLog().setLogLevel(logLevel);
			//event.getAsLog().getAttributeList().add(String.valueOf(resultCode));
			
			event.getAsLog().setContent(out.toString());
			event.getAsLog().addContentedAttribute(outRequest.toString());
			event.getAsLog().addContentedAttribute(outResponse.toString());

			event.getAsLog().addContentedAttribute(String.valueOf(resultCode));

			event.getAsLog().setLogDate(Calendar.getInstance().getTimeInMillis());


			// push data to event manager
			RtGlobalEventManager.instance().pushCommonEvent(event);

		//pushEventData( event);
		} catch (Throwable e) {
			HtLog.log(Level.WARNING, getContext(), "pushRemoteServerEvent", "Exception on pushing event: " + e.getMessage());
		}
	}

	
	/**
	 * 
	 * HELPERS 
	 */
	
	
	private void registerNewServer(String serverName, RtTradingServer curServer) throws Exception {
		synchronized (servers_m) {
			if (servers_m.containsKey(serverName)) {
				throw new HtException(getContext(), "registerNewServer", "Server process is registered via its name: " + serverName);
			}

			servers_m.put(serverName, curServer);
		}
	}

	private RtTradingServer unregisterServer(String serverName) throws Exception {
		synchronized (servers_m) {
			if (!servers_m.containsKey(serverName)) {
				throw new HtException(getContext(), "registerNewServer", "Server process is NOT registered via its name: " + serverName);
			}

			RtTradingServer obj = (RtTradingServer) servers_m.get(serverName);
			servers_m.remove(serverName);

			return obj;
		}


	}

	private Vector<String> getAllRegisteredServerName() {
		Vector<String> result = new Vector<String>();

		synchronized (servers_m) {
			result.addAll(servers_m.keySet());
		}

		return result;
	}

	
}



