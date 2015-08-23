/*
 * HtExtServerCommand.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.externalapi;

import com.fin.httrader.externalapi.utils.HtExtIntBuffer;
import com.fin.httrader.configprops.HtAlgBrkPairProp;
import com.fin.httrader.configprops.HtSimulationProfileProp;
import com.fin.httrader.externalapi.HtExtCommandResult;
import com.fin.httrader.hlpstruct.ServerThreadStatus;
import com.fin.httrader.managers.RtConfigurationManager;
import com.fin.httrader.managers.RtTradingServerManager;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.model.HtServerProxy;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.hlpstruct.HtPair;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.LongParam;
import com.fin.httrader.utils.Uid;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.Vector;
import java.util.logging.Level;

/**
 *
 * @author Victor_Zoubok
 */
public class HtExtServerCommand extends HtExtCommandBase {

	private StringBuilder server_m = new StringBuilder();

	// ------------------------------
	protected String getContext() {
		return getClass().getSimpleName();
	}

	private String getServerName() throws Exception {
		if (server_m == null || server_m.length() <= 0) {
			throw new HtException(getContext(), "getServerName", "Invalid trading server name");
		}

		return server_m.toString();
	}

	private void setServerName(String serverId) {
		server_m.setLength(0);
		server_m.append(serverId);
	}

	// -----------------------------------------
	public HtExtServerCommand(HtExtBaseLogger logger) {
		super(logger);
	}

	// ------------------------------------
	// start trading srever
	public HtExtCommandResult startTradingServer(
					String serverName,
					String serverExecutable,
					int server_log_level,
					String serverHost,
					int serverPort,
					int spawnPort,
					boolean useSpawner,
					boolean isLaunchProcess,
					long begdate,
					long enddate,
					Map<String, HtServerProxy.HtServerProxy_ProfileEntry> profProvSymbMap, // map profile <-> ( provider - symbol )
					Set<String> rtProvidersToSubscribe,
					boolean reuseCache,
					int defaultLogLevel,
					Map<Integer, Boolean> subsribedEvents,
					boolean noSelectAtAll,
					boolean allIfMapEmpty // if provSymMap and allIfMapEmpty==true - select all rows
	) {
		HtExtCommandResult result = new HtExtCommandResult("startTradingServer");
		try {

		

			HtCommandProcessor.instance().get_HtServerProxy().remote_startServerProcessWithHistoryData(
							serverName,
							serverExecutable,
							server_log_level,
							serverHost,
							serverPort,
							RtConfigurationManager.StartUpConst.COMMON_SERVICE_LISTEN_PORT,
							spawnPort,
							useSpawner,
							isLaunchProcess,
							begdate,
							enddate,
							profProvSymbMap, // map profile <-> ( provider - symbol)
							rtProvidersToSubscribe,
							reuseCache,
							defaultLogLevel,
							subsribedEvents,
							noSelectAtAll
				);

			// set the name
			setServerName(serverName);

		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	// ------------------------------------

	public HtExtCommandResult getTradingServerStatus(String serverId, HtExtIntBuffer srvStatus) {
		HtExtCommandResult result = new HtExtCommandResult("joinTradingServer");
		try {
			// just set up the server name
			srvStatus.setInt(HtCommandProcessor.instance().get_HtServerProxy().remote_returnServerStatus(serverId));
			

		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	// ------------------------------------
	// stop trading server
	public HtExtCommandResult stopTradingServer() {
		HtExtCommandResult result = new HtExtCommandResult("stopTradingServer");
		try {
			HtCommandProcessor.instance().get_HtServerProxy().remote_shutDownServerProcess(getServerName());
		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	// ------------------------------------
	public HtExtCommandResult stopTradingServer(String serverId) {
		HtExtCommandResult result = new HtExtCommandResult("stopTradingServer");
		try {
			HtCommandProcessor.instance().get_HtServerProxy().remote_shutDownServerProcess(serverId);
		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	// ------------------------------------
	// stop trading server
	public HtExtCommandResult killTradingServer(String serverId) {
		HtExtCommandResult result = new HtExtCommandResult("killTradingServer");
		try {
			HtCommandProcessor.instance().get_HtServerProxy().remote_killServerProcess(serverId);

		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	// ----------------------------------
	// kill
		public HtExtCommandResult killTradingServer() {
		HtExtCommandResult result = new HtExtCommandResult("killTradingServer");
		try {
			HtCommandProcessor.instance().get_HtServerProxy().remote_killServerProcess(getServerName());

		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}


	// ------------------------------------
	// join already running trading server
	public HtExtCommandResult joinTradingServer(String serverId) {
		HtExtCommandResult result = new HtExtCommandResult("joinTradingServer");
		try {
			// just set up the server name
			int status = HtCommandProcessor.instance().get_HtServerProxy().remote_returnServerStatus(serverId);
			if (status==RtTradingServerManager.SERVER_IDLE) {
				throw new HtException(getContext(), "joinTradingServer", "Trading server \"" + serverId + "\" is not running");
			}

			if (status==RtTradingServerManager.SERVER_RTTHREAD_ERROR) {
				throw new HtException(getContext(), "joinTradingServer", "Trading server \"" + serverId + "\" had RT thread errors");
			}

			setServerName(serverId);


		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	// ------------------------------------
	//
	public HtExtCommandResult loadAlgBrkPair(
					String algPair,
					String runName,
					String runComment,
					int priority,
					String algPath,
					String brokerPath,
					String brokerPath2,
					Map<String, String> algParams,
					Map<String, String> brkParams,
					Map<String, String> brkParams2
		) {
		HtExtCommandResult result = new HtExtCommandResult("loadAlgBrkPair");

		try {


			HtCommandProcessor.instance().get_HtServerProxy().remote_loadAlgorithmBrokerPair(
							getServerName(),
							runName,
							runComment,
							algPair,
							algPath,
							brokerPath,
							brokerPath2,
							algParams,
							brkParams,
							brkParams2,
							priority);
		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	//
	// ------------------------------------
	public HtExtCommandResult unloadAlgorithms(String algPair) {
		HtExtCommandResult result = new HtExtCommandResult("unloadAlgBrkPair");

		try {
			HtCommandProcessor.instance().get_HtServerProxy().remote_unloadAlgorithmBrokerPair(getServerName(), algPair);
		} catch (Throwable e) {
			result.initialize(e);
		}
		return result;
	}
	
	public HtExtCommandResult requestLoadedAlgLibPairs(Map<String, Integer> dataList, int tId) {
		HtExtCommandResult result = new HtExtCommandResult("requestLoadedAlgLibPairs");

		try {
			dataList.clear();
			dataList.putAll( HtCommandProcessor.instance().get_HtServerProxy().remote_requestLoadedAlgorithmBrokerPairs(getServerName(), tId) );
		} catch (Throwable e) {
			result.initialize(e);
		}
		return result;
	}

	// ------------------------------------
	// run simulation until the finish
	public HtExtCommandResult startSimulation(
					int threadId,
					String runName,
					String runComment,
					boolean singleRun,
					Uid simulationHistoryUid) {
		HtExtCommandResult result = new HtExtCommandResult("startSimulation");

		try {
			
			HtCommandProcessor.instance().get_HtServerProxy().remote_startHistorySimulation(
							runName,
							runComment,
							getServerName(),
							threadId,
							singleRun,
							simulationHistoryUid);


		} catch (Throwable e) {
			result.initialize(e);
		}
		return result;
	}

	// ------------------------------------
	public HtExtCommandResult getThreadStatus(int threadId, HtExtIntBuffer threadStatus) {
		HtExtCommandResult result = new HtExtCommandResult("getThreadStatus");

		try {
			int status = HtCommandProcessor.instance().get_HtServerProxy().remote_requestThreadStatus(getServerName(), threadId);
			threadStatus.setInt(status);
		} catch (Throwable e) {
			result.initialize(e);
		}
		return result;
	}

	// ------------------------------------
	public HtExtCommandResult stopSimulation(int threadId) {
		HtExtCommandResult result = new HtExtCommandResult("stopSimulation");

		try {
			HtCommandProcessor.instance().get_HtServerProxy().remote_stopHistorySimulation(getServerName(), threadId);


		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	// ------------------------------------
	public HtExtCommandResult startRtActivity(int threadId, String runName, String runComment, String simProfileName) {
		HtExtCommandResult result = new HtExtCommandResult("startRtActivity");

		try {
			if (simProfileName != null && simProfileName.length() > 0) {
				HtSimulationProfileProp prop =
								HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getSimulationProfileProperty(simProfileName);

				HtCommandProcessor.instance().get_HtServerProxy().remote_startRtActivity(getServerName(), threadId, runName, runComment, prop);

			} else {
				// dummy
				

				HtCommandProcessor.instance().get_HtServerProxy().remote_startRtActivity(getServerName(), threadId, runName, runComment, null);

			}



		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	// ------------------------------------
	public HtExtCommandResult stopRtActivity(int threadId) {
		HtExtCommandResult result = new HtExtCommandResult("stopRtActivity");

		try {
			HtCommandProcessor.instance().get_HtServerProxy().remote_stopRtActivity(getServerName(), threadId);


		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	// ------------------------------------
	public HtExtCommandResult startThread(List<String> pairs, HtExtIntBuffer threadId) {
		HtExtCommandResult result = new HtExtCommandResult("startThread");

		try {
			threadId.setInt(-1);
			int tId = HtCommandProcessor.instance().get_HtServerProxy().remote_startAlgorithmThread(getServerName(), pairs);
			threadId.setInt(tId);
		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	// ------------------------------------
	public HtExtCommandResult stopThread(int threadId) {
		HtExtCommandResult result = new HtExtCommandResult("stopThread");

		try {
			HtCommandProcessor.instance().get_HtServerProxy().remote_stopAlgorithmThread(getServerName(), threadId);
		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	// -------------------------------------
	// periodycally asks a working thread its status and returns upon completion
	public HtExtCommandResult waitThreadInLazyMode(int threadId, double pollsec) {
		HtExtCommandResult result = new HtExtCommandResult("waitThreadInLazyMode");
		try {
			HtExtIntBuffer threadStatus = new HtExtIntBuffer();

			int totalSecs = 0;
			while (true) {

				threadStatus.setInt(-1);
				HtExtCommandResult res = getThreadStatus(threadId, threadStatus);
				if (res.getCode() != 0) {
					getLogger().addStringEntry("getThreadStatus(...) returned error: \"" + res.getReason() + "\"");
					break;
				}


				if (threadStatus.getInt() == ServerThreadStatus.TH_Simulation) {
					// sleeping 5 secs
					HtUtils.Sleep(pollsec);

					totalSecs += pollsec;
				} else {
					getLogger().addStringEntry("Thread: " + threadId + " now in " + ServerThreadStatus.getThreadStatus(threadStatus.getInt()) + " mode, returning...");
					break;
				}

				getLogger().addStringEntry("Thread: " + threadId + " is waiting for simulation finish for: " + totalSecs + " sec...");
			}
		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}
	// -------------------------------------
	public HtExtCommandResult sendCustomEventToAlgorithm(
					String serverId,
					String algBrkPair,
					int threadId,
					String customName,
					String customData,
					long waitSec,
					StringBuilder out
	)
	{
		HtExtCommandResult result = new HtExtCommandResult("sendCustomEventToAlgorithm");

		try {
			out.setLength(0);
			out.append( HtCommandProcessor.instance().get_HtServerProxy().remote_SendCustomEvent_toAlgorithm(serverId, algBrkPair, threadId, customName, customData, waitSec));
		}
		catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}
	
	// -------------------------------------
}
