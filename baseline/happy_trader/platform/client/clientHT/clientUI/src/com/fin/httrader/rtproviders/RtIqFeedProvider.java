/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.rtproviders;

import com.fin.httrader.hlpstruct.BackgroundJobStatus;
import com.fin.httrader.interfaces.HtRealTimeProvider;
import com.fin.httrader.managers.RtGlobalEventManager;
import com.fin.httrader.managers.RtRealTimeProviderManager;
import com.fin.httrader.managers.RtTradingServer;
import com.fin.httrader.managers.RtTradingServerBaseThread;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.*;
import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.utils.hlpstruct.HtRtData;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import com.fin.httrader.utils.transport.HtCommonSocket;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.text.SimpleDateFormat;

import java.util.*;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;
import java.util.logging.Level;
import jsync.Event;

/**
 *
 * @author Victor_Zoubok
 */
public class RtIqFeedProvider implements HtRealTimeProvider {

	private static final String SYM_HID_PRFX = "H_";
	private static final String IQFEED_END_CHUNK = "!ENDMSG!";
	private static final byte CR = 13;
	private static final byte LF = 10;
	private static final byte[] ENDLINE = {CR, LF};
	private BackgroundJobStatus status_m = new BackgroundJobStatus();
	private String name_m = null;
	private long timeShift_m = 0;
	private HashSet<String> symbols_m = new HashSet<String>();
	// parameters
	private int iqFeedPort_Level1_m = 5009;
	private int iqFeedPort_Admin_m = 9300;
	private int iqFeedPort_Lookup_m = 9100;
	private String appId_m = "IQFEED_DEMO";
	private String appVersion_m = "1.0";
	private String iqConnectExecutable_m = "";
	private int timeFrameMin_m = 1;
	private int historyDays_m = 1;
	private String login_m = "";
	private String password_m = "";
	private Process pr_m = null;
	private IqAdminThread adminTalker_m = null;
	private IqLevel1Thread level1Talker_m = null;
	private IqLookupThread lookupTalker_m = null;
	private Thread t_m = null;

	// ----------------------------------------------------
	// base class 
	static abstract class IqFeedThread extends RtTradingServerBaseThread<RtIqFeedProvider> {

		private Event connected_m = new Event();
		private Event lazy_m = new Event();
		private String connectHost_m = null;
		private int connectPort_m = -1;
		private Object context_m;
		private int commandTimeOut_m = -1;

		public IqFeedThread(int toutMsec, String connectHost, int connectPort) {
			super();
			commandTimeOut_m = toutMsec;
			connectHost_m = connectHost;
			connectPort_m = connectPort;

			lazy_m.signal();
		}

		@Override
		public void doRun() throws Exception {
			XmlEvent.createSimpleLog("", getClassContext(), CommonLog.LL_INFO, "Thread started");

			try {
				HtCommonSocket socket = HtSocketUtils.connectToServerUsingTCPIP(connectHost_m, connectPort_m, 0, 2);

				performDataStreamInitializations(socket,
								new DataInputStream(socket.getInputStream()),
								new DataOutputStream(socket.getOutputStream()));
			} catch (Throwable e) {
				XmlEvent.createSimpleLog("", getClassContext(), CommonLog.LL_ERROR, "Cannot connect to iqfeed client because of: " + e.getMessage());
				this.getBaseReference().returnProviderStatus().setUp(-2, "Cannot connect to iqfeed client because of: " + e.getMessage());
				return;
			}


			// on that stage we exepect are connected

			connected_m.signal();
			XmlEvent.createSimpleLog("", getClassContext(), CommonLog.LL_INFO, "Connected to: " + connectHost_m + ":" + connectPort_m);

			doMainLoop();

			XmlEvent.createSimpleLog("", getClassContext(), CommonLog.LL_INFO, "Finishing thread...");
		}

		protected boolean isInNotLazyModeNoWait() {
			return !lazy_m.waitEvent(0);
		}

		protected void signalLazyMode() {
			lazy_m.signal();
		}

		protected int getTimoutMsec() {
			return commandTimeOut_m;
		}

		protected boolean waitForConnection(int toutMsec) {
			return connected_m.waitEvent(toutMsec);
		}

		protected Object getCommandContext() {
			return context_m;
		}

		// non blocking version - do not use any of events as no need
		protected void issueNonBlockingCommand(String command) throws Exception {
			getOutputStream().write(HtStringByteConverter.encodeUTF8(command));
			getOutputStream().write(ENDLINE);
			getOutputStream().flush();
		}

		// blocking command - use of lazy event
		protected void issueCommand(String command, Object context) throws Exception {

			// if not in signalled state - e.i. working
			if (!lazy_m.waitEvent(0)) {
				throw new HtException(getClassContext(), "issueCommand", "Operation in progress");
			}


			context_m = context;

			// reset this
			lazy_m.reset();

			getOutputStream().write(HtStringByteConverter.encodeUTF8(command));
			getOutputStream().write(ENDLINE);
			getOutputStream().flush();

			XmlEvent.createSimpleLog("", getClassContext(), CommonLog.LL_INFO, "Command issued: [" + command + "]");

			// wait for completion
			if (!lazy_m.waitEvent(commandTimeOut_m)) {


				context_m = null;
				lazy_m.signal();

				throw new HtException(getClassContext(), "issueCommand", "While waiting for command completion: [" + command + "] timeout exceeded (msec): " + commandTimeOut_m);
			}

			context_m = null;

			XmlEvent.createSimpleLog("", getClassContext(), CommonLog.LL_INFO, "Completed command: [" + command + "]");
		}

		@Override
		protected String getClassContext() {
			return IqFeedThread.class.getSimpleName();
		}

		protected abstract void doMainLoop();
	};

	static class IqLookupThread extends IqFeedThread {

		private Map<String, Long> lastData_m = new HashMap<String, Long>();
		private Event readHistoryData_m = new Event();
		private int pendingHistoryData_m = 0;

		@Override
		protected String getClassContext() {
			return IqLookupThread.class.getSimpleName();
		}

		public IqLookupThread(int toutMsec, String connectHost, int connectPort) {
			super(toutMsec, connectHost, connectPort);

		}

		@Override
		protected void doMainLoop() {
			// now...



			while (true) {
				try {
					// infinite timeout
					byte[] data = HtUtils.readFromStreamUntilBytesAreReached(-1, getInputStream(), ENDLINE);
					String line = HtStringByteConverter.decodeWin1251(data);

					HtLog.log(Level.INFO, getClassContext(), "doMainLoop", line);


					parseCandleLine(line);

					// here we chase only historical data and delegate the as RT only


				} catch (Throwable e) {
					XmlEvent.createSimpleLog("", getBaseReference().getProviderId(), CommonLog.LL_ERROR, "Error in lookup thread: " + e.getMessage());
					getBaseReference().returnProviderStatus().setUp(-1, "Error in lookup thread: " + e.getMessage());
					break;
				}

				//getInputStream().read(b);
				if (waitShutdownEvent(0)) {
					break;
				}
			} // end of loop

			XmlEvent.createSimpleLog("", getBaseReference().getProviderId(), CommonLog.LL_INFO, "Finishing lookup thread...");
		}

		public void issueGetHistorycalLastData(String symbol, int minutesPeriod) throws Exception {

			//HtLog.log(Level.INFO, getClassContext(), "doMainLoop", "Poll fresh data");

			int interval = 60 * minutesPeriod;
			if (interval >= 86400 || interval <= 0) {
				throw new HtException(getClassContext(), "getHistorycalData", "Invalid period in minutes: " + minutesPeriod);
			}

			int maxDatapoints = 2;

			StringBuilder context = new StringBuilder("RT:").append(symbol);
			StringBuilder r = new StringBuilder("HIX,").append(symbol).append(",").append(interval).append(",").
							append(maxDatapoints).append(",1,").append(context);


			// can be spawned from different threads
			synchronized (this) {
				issueNonBlockingCommand(r.toString());
			}

		}

		public void setUpPendingHistoryData(int cnt) {
			pendingHistoryData_m = cnt;
		}

		public void resetHistoryDataEvent() {
			readHistoryData_m.reset();
		}
		
		
		
		public boolean ifAwaitingHistoryData()
		{
			return !readHistoryData_m.waitEvent(0);
		}

		public boolean waitUntilHistoryDataCompletes(int toutMsec) {
			if (toutMsec < 0) {
				readHistoryData_m.waitEvent();
				return true;
			} else {
				return readHistoryData_m.waitEvent(toutMsec);
			}
		}

		public void issueGetHistorycalData(String symbol, int minutesPeriod, int daysHistory) throws Exception {

			//HtLog.log(Level.INFO, getClassContext(), "doMainLoop", "Issuing history data...");

			int interval = 60 * minutesPeriod;
			if (interval >= 86400 || interval <= 0) {
				throw new HtException(getClassContext(), "getHistorycalData", "Invalid period in minutes: " + minutesPeriod);
			}

			// claculate the number if datapoints to retreive
			// return 
			// HIX,[Symbol],[Interval],[MaxDatapoints],[DataDirection],[RequestID],[DatapointsPerSend]<CR><LF> 


			//int maxDatapoints = 2 * 24 * 60 / minutesPeriod;

			//StringBuilder r = new StringBuilder("HIX,").append(symbol).append(",").append(interval).append(",").
			//				append(maxDatapoints).append(",1,").append(symbol);

			//HtLog.log(Level.INFO, getClassContext(), "issueGetHistorycalData", "Issue: " + r.toString());


			long date_begin = HtDateTimeUtils.getCurGmtTime_DayBegin();
			date_begin -= daysHistory * 24 * 60 * 60 * 1000;

			SimpleDateFormat fmt = new SimpleDateFormat("yyyyMMdd");
			fmt.setTimeZone(TimeZone.getTimeZone("GMT-0"));
			fmt.format(new Date(date_begin));

			String date_begin_s = fmt.format(new Date(date_begin));

			XmlEvent.createSimpleLog("", getBaseReference().getProviderId(), CommonLog.LL_INFO,
							"Issue history data for symbol: [" + symbol + "] requesting from [" + date_begin_s + "]");


			StringBuilder context = new StringBuilder("HIST:").append(symbol);
			StringBuilder r = new StringBuilder("HIT,").append(symbol).append(",").append(interval).append(",").
							append(date_begin_s).append(",,,,,1,").append(context);


			// can be spawned from different threads
			synchronized (this) {
				issueNonBlockingCommand(r.toString());
			}

		}

		/*
		 * Helpers
		 */
		private void parseCandleLine(String line) {
			if (line == null) {
				return;
			}
			
			String symbol = null;
			String mode = null;

			try {
				String[] tokens = line.split(",");
				
				if (tokens.length>=2) {
					String prefix = tokens[0];
					String[] prarr = prefix.split(":");
					
					symbol = prarr[1];
					mode = prarr[0];
				}
				
				// 
				if (mode.equalsIgnoreCase("HIST")) {
					if (ifAwaitingHistoryData()) {
						if (pendingHistoryData_m > 0) {
							if (tokens.length == 2) {
								// HIST:DELL,!ENDMSG!,
								String t2 = tokens[1];
								if (t2.equalsIgnoreCase("!ENDMSG!")) {
									if (--pendingHistoryData_m <=0) {
										readHistoryData_m.signal();
									}
								}
								
							}
						}
					}
				}
				else if (mode.equalsIgnoreCase("RT")) {
					
				}
				else
					throw new HtException(getClassContext(), "parseCandleLine", "Invalid mode");
				
				
				if (tokens.length == 8 && symbol != null) {
						
						String time_s = tokens[1];
						String high_s = tokens[2];
						String low_s = tokens[3];
						String open_s = tokens[4];
						String close_s = tokens[5];
						String volume_total_s = tokens[6];
						String volume_s = tokens[7];

						HtRtData rtdata = new HtRtData();
						rtdata.type_m = HtRtData.HST_Type;

						//CCYY-MM-DD HH:MM:SS
						rtdata.time_m = HtDateTimeUtils.parseDateTimeParameter_ToGmt(time_s, "yyyy-MM-dd HH:mm:ss");
						rtdata.open_m = HtUtils.parseDouble(open_s);
						rtdata.high_m = HtUtils.parseDouble(high_s);
						rtdata.low_m = HtUtils.parseDouble(low_s);
						rtdata.close_m = HtUtils.parseDouble(close_s);

						rtdata.open2_m = rtdata.open_m;
						rtdata.high2_m = rtdata.high_m;
						rtdata.low2_m = rtdata.low_m;
						rtdata.close2_m = rtdata.close_m;

						rtdata.volume_m = HtUtils.parseDouble(volume_s);
						rtdata.setSymbol(symbol);
						rtdata.setProvider(getBaseReference().getProviderId());

						Long ltm = lastData_m.get(symbol);
						if (ltm != null) {
							if (rtdata.time_m > ltm.longValue()) {
								lastData_m.put(symbol, rtdata.time_m);

								// send 
								sendRtData(rtdata);
							}
						} else {
							lastData_m.put(symbol, rtdata.time_m);
							sendRtData(rtdata);
						}


						// need to store it to avoid dublicates
						//HtLog.log(Level.INFO, getClassContext(), "parseCandleLine", "RtData: " + rtdata.toString());
					}
				

			} catch (Throwable e) {
				XmlEvent.createSimpleLog("", getBaseReference().getProviderId(), CommonLog.LL_ERROR, "Error when parsing candle line: [" + line
								+ "] - " + e.getMessage());
			}



		}

		private void sendRtData(HtRtData rtdata) {
			XmlEvent rtDataEvent = new XmlEvent();
			RtRealTimeProviderManager.createXmlEventRtData(rtdata, rtDataEvent);
			RtGlobalEventManager.instance().pushCommonEvent(rtDataEvent);
		}
	};

	static class IqLevel1Thread extends IqFeedThread {

		public IqLevel1Thread(int toutMsec, String connectHost, int connectPort) {
			super(toutMsec, connectHost, connectPort);

		}

		@Override
		protected String getClassContext() {
			return IqLevel1Thread.class.getSimpleName();
		}

		@Override
		protected void doMainLoop() {
			// now...
			while (true) {
				try {
					// infinite timeout
					byte[] data = HtUtils.readFromStreamUntilByteIsReached(getTimoutMsec(), getInputStream(), LF);
					//String line = HtStringByteConverter.decodeWin1251(data);

					//HtLog.log(Level.INFO, getClassContext(), "doMainLoop", line);


					//if (isInNotLazyModeNoWait()) {
					// parse wehn we are in cleared state 
					//	parseAdminResponse(line);
					//}



				} catch (Throwable e) {
					XmlEvent.createSimpleLog("", getBaseReference().getProviderId(), CommonLog.LL_ERROR, "Error in level 1 thread: " + e.getMessage());
					getBaseReference().returnProviderStatus().setUp(-1, "Error in level 1 thread: " + e.getMessage());
					break;
				}

				//getInputStream().read(b);
				if (waitShutdownEvent(0)) {
					break;
				}
			} // end of loop

			XmlEvent.createSimpleLog("", getBaseReference().getProviderId(), CommonLog.LL_INFO, "Finishing level 1 thread...");
		}
	};

	static class IqAdminThread extends IqFeedThread {

		public IqAdminThread(int toutMsec, String connectHost, int connectPort) {
			super(toutMsec, connectHost, connectPort);

		}

		@Override
		protected String getClassContext() {
			return IqAdminThread.class.getSimpleName();
		}

		@Override
		public void doMainLoop() {
			// init sockets



			// now...
			while (true) {
				try {
					// infinite timeout
					byte[] data = HtUtils.readFromStreamUntilByteIsReached(getTimoutMsec(), getInputStream(), LF);
					String line = HtStringByteConverter.decodeWin1251(data);

					//HtLog.log(Level.INFO, getClassContext(), "doMainLoop", line);

					if (isInNotLazyModeNoWait()) {
						// parse wehn we are in cleared state 
						parseAdminResponse(line);
					}



				} catch (Throwable e) {
					XmlEvent.createSimpleLog("", getBaseReference().getProviderId(), CommonLog.LL_ERROR, "Error in admin thread: " + e.getMessage());
					getBaseReference().returnProviderStatus().setUp(-1, "Error in admin thread: " + e.getMessage());
					break;
				}

				//getInputStream().read(b);
				if (waitShutdownEvent(0)) {
					break;
				}
			} // end of loop

			XmlEvent.createSimpleLog("", getBaseReference().getProviderId(), CommonLog.LL_INFO, "Finishing admin thread...");
		}

		public void sendRegisterCommand() throws Exception {
			StringBuilder r = new StringBuilder("S,REGISTER CLIENT APP,");
			r.append(getBaseReference().getAppId()).append(",").append(getBaseReference().getVer());
			issueCommand(r.toString(), "REGISTER CLIENT APP COMPLETED");
		}

		public void sendDeregisterCommand() throws Exception {
			StringBuilder r = new StringBuilder("S,REMOVE CLIENT APP,");
			r.append(getBaseReference().getAppId()).append(",").append(getBaseReference().getVer());
			issueCommand(r.toString(), "REMOVE CLIENT APP COMPLETED");

		}

		/**
		 * Helpers
		 */
		private void parseAdminResponse(String source) {


			if (source == null) {
				return;
			}

			// just speed up
			if (source.charAt(0) != 'S') {
				return;
			}

			//
			String[] tokens = source.split(",");
			if (tokens.length >= 2) {

				if (tokens[1].equalsIgnoreCase((String) getCommandContext())) {

					// if lazy is in cleared state - signal
					if (isInNotLazyModeNoWait()) {
						signalLazyMode();
					}
				}

			}


		}
	}

	// -------------------------
	private String getContext() {
		return getClass().getSimpleName();
	}

	@Override
	public void initialize(Map<String, String> initdata) throws Exception {
		String symbolFile = HtUtils.getParameterWithException(initdata, "SYMBOL_FILE");
		symbols_m.addAll(HtFileUtils.readSymbolListFromFile(symbolFile));

		if (symbols_m.isEmpty()) {
			throw new HtException(getContext(), "initialize", "Number of symbols is empty");
		}

		XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_INFO, "Found symbols, size: " + symbols_m.size());

		//

		iqFeedPort_Level1_m = HtUtils.parseInt(HtUtils.getParameterWithException(initdata, "IQFEED_PORT_LEVEL1"));
		iqFeedPort_Admin_m = HtUtils.parseInt(HtUtils.getParameterWithException(initdata, "IQFEED_PORT_ADMIN"));
		iqFeedPort_Lookup_m = HtUtils.parseInt(HtUtils.getParameterWithException(initdata, "IQFEED_PORT_LOOKUP"));
		appId_m = HtUtils.getParameterWithException(initdata, "IQFEED_APP_ID");
		iqConnectExecutable_m = HtUtils.getParameterWithException(initdata, "IQCONNECT_EXECUTABLE");
		login_m = HtUtils.getParameterWithException(initdata, "IQCONNECT_LOGIN");
		password_m = HtUtils.getParameterWithException(initdata, "IQCONNECT_PASSWORD");
		timeFrameMin_m = HtUtils.parseInt(HtUtils.getParameterWithException(initdata, "TIMEFRAME_MIN"));
		historyDays_m = HtUtils.parseInt(HtUtils.getParameterWithException(initdata, "HISTORY_DAYS"));

		try {
			initAdminTalker();
		} catch (Throwable e) {
			shutdownAdminTalker();
			throw new HtException(getContext(), "initialize", "Error on initialization admin talker: " + e.getMessage());
		}

		try {
			initLevel1Talker();
		} catch (Throwable e) {
			shutdownLevel1Talker();
			throw new HtException(getContext(), "initialize", "Error on initialization level 1 talker: " + e.getMessage());
		}


		// below must be done in a separate thread

		try {
			initLoolupTalker();
		} catch (Throwable e) {
			shutdownLookupTalker();
			throw new HtException(getContext(), "initialize", "Error on initialization lookup talker: " + e.getMessage());
		}


		t_m = new Thread() {

			@Override
			public void run() {
				try {
					lookupTalker_m.setUpPendingHistoryData(getSymbols().size());

					// need to wait here until all is ok
					lookupTalker_m.resetHistoryDataEvent();

					// now need to get all history data
					for (Iterator<String> it = getSymbols().iterator(); it.hasNext();) {
						lookupTalker_m.issueGetHistorycalData(it.next(), getTimeFrameMinutes(), getDaysBackFill());
					}

					//wait until it completes
					lookupTalker_m.waitUntilHistoryDataCompletes(-1);

					try {
						initPollingTask();

						// only at this point status is ok
						status_m.setUp(BackgroundJobStatus.STATUS_OK, "Operating normally");

					} catch (Throwable e) {
						shutdownPollingTask();
						throw new HtException(getContext(), "initialize", "Error on initialization polling task: " + e.getMessage());
					}
				} catch (Throwable e2) {

					XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_ERROR, "Exception on initializing: " + e2.getMessage() + ", please shutdown");
				}

			}
		};

		// just skip the join
		t_m.start();

		// 
		status_m.setUp(BackgroundJobStatus.STATUS_STILL_NOT_READY, "Initializing");

	}

	@Override
	public void deinitialize() throws Exception {
		shutdownPollingTask();

		shutdownLookupTalker();
		shutdownLevel1Talker();
		shutdownAdminTalker();

		symbols_m.clear();
	}

	@Override
	public String getProviderId() {
		return name_m;
	}

	@Override
	public void setProviderId(String providerId) {
		name_m = providerId;
	}

	@Override
	public void setTimeShift(long timeshift) {
		timeShift_m = timeshift;
	}

	@Override
	public long getTimeShift() {
		return timeShift_m;
	}

	@Override
	public void returnAvailableSymbolList(List<String> symbols) {
		symbols.clear();
		symbols.addAll(symbols_m);
	}

	@Override
	public BackgroundJobStatus returnProviderStatus() {
		return status_m;
	}

	public int getAdminPort() {
		return this.iqFeedPort_Admin_m;
	}

	public int getLevel1Port() {
		return this.iqFeedPort_Level1_m;
	}

	public int getLookupPort() {
		return this.iqFeedPort_Lookup_m;
	}

	public String getAppId() {
		return this.appId_m;
	}

	public String getVer() {
		return "1.0";
	}

	public Set<String> getSymbols() {
		return symbols_m;
	}

	public int getTimeFrameMinutes() {
		return this.timeFrameMin_m;
	}

	public int getDaysBackFill() {
		return this.historyDays_m;
	}

	/*
	 * Helpers
	 *
	 */
	private void launchIqFeedConnect() throws Exception {
		if (pr_m == null) {
			String[] cmdarr = new String[]{
				iqConnectExecutable_m,
				"-product", appId_m,
				"-version", appVersion_m,
				"-login", login_m,
				"-password", password_m,
				"-autoconnect", ""
			};

			File f_file = new File(iqConnectExecutable_m);
			if (!f_file.exists()) {
				throw new HtException(getContext(), "startProcess", "Server executable does not exist: \"" + iqConnectExecutable_m + "\"");
			}

			String parent = f_file.getParent();
			File f_dir = new File(parent);

			// create execution enviroment
			String[] execEnviroment = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getEnviromentEntries(null);

			pr_m = Runtime.getRuntime().exec(cmdarr, execEnviroment, f_dir);
		}
	}

	private void killIqFeedConnect() {
		try {
			if (pr_m != null) {
				pr_m.destroy();
				pr_m = null;
			}

		} catch (Throwable e2) {
		}
	}
	final static int NUM_SCHEDULER_THREADS = 3; // this is to avoid delayes
	private ScheduledExecutorService poll_shecduler_m = null;

	private void initPollingTask() throws Exception {
		poll_shecduler_m = Executors.newScheduledThreadPool(NUM_SCHEDULER_THREADS);
		final RtIqFeedProvider thisPtr = this;
		final Set<String> symbols = symbols_m;
		Runnable command = new Runnable() {

			@Override
			public void run() {
				//HtLog.log(Level.INFO, this.getClass().getSimpleName(), "initPollingTask", "Issue polling command...");
				for (Iterator<String> it = symbols.iterator(); it.hasNext();) {
					try {
						thisPtr.lookupTalker_m.issueGetHistorycalLastData(it.next(), thisPtr.timeFrameMin_m);
					} catch (Throwable e) {
						//HtLog.log(Level.WARNING, this.getClass().getSimpleName(), "initPollingTask", "Error in polling: " + e.getMessage());
						XmlEvent.createSimpleLog("", thisPtr.getProviderId(), CommonLog.LL_ERROR, "Error in polling: " + e.getMessage());
					}
				}
			}
		};


		int trm_sec = timeFrameMin_m * 60;
		poll_shecduler_m.scheduleAtFixedRate(command, trm_sec, trm_sec, TimeUnit.SECONDS);
		XmlEvent.createSimpleLog("", thisPtr.getProviderId(), CommonLog.LL_INFO, "Initialized polling each " + trm_sec + " sec(s)");

	}

	private void shutdownPollingTask() {
		poll_shecduler_m.shutdown();
		XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_INFO, "Stopped polling");

	}

	private void initAdminTalker() throws Exception {
		if (adminTalker_m != null) {
			throw new HtException(getContext(), "initAdminTalker", "Already initialized");
		}

		adminTalker_m = new IqAdminThread(10 * 1000, "127.0.0.1", getAdminPort());


		// rt data sender
		adminTalker_m.setBaseReference(this);
		adminTalker_m.start();

		if (!adminTalker_m.waitForThreadInit(10 * 1000)) {
			throw new HtException(getContext(), "initAdminTalker", "Cannot start thread");
		}


		if (!adminTalker_m.waitForConnection(10 * 1000)) {
			throw new HtException(getContext(), "initAdminTalker", "Cannot complete connection");
		}

		// registering
		adminTalker_m.sendRegisterCommand();


		// need to wait until register is completed
	}

	private void initLevel1Talker() throws Exception {
		if (level1Talker_m != null) {
			throw new HtException(getContext(), "initLevel1", "Level1 already initialized");
		}

		level1Talker_m = new IqLevel1Thread(10 * 1000, "127.0.0.1", getLevel1Port());


		// rt data sender
		level1Talker_m.setBaseReference(this);
		level1Talker_m.start();

		if (!level1Talker_m.waitForThreadInit(10 * 1000)) {
			throw new HtException(getContext(), "initLevel1", "Cannot start thread");
		}


		if (!level1Talker_m.waitForConnection(10 * 1000)) {
			throw new HtException(getContext(), "initLevel1", "Cannot complete connection");
		}

	}

	private void initLoolupTalker() throws Exception {
		if (lookupTalker_m != null) {
			throw new HtException(getContext(), "initLoolupTalker", "Lookup already initialized");
		}

		lookupTalker_m = new IqLookupThread(10 * 1000, "127.0.0.1", getLookupPort());


		// rt data sender
		lookupTalker_m.setBaseReference(this);
		lookupTalker_m.start();

		if (!lookupTalker_m.waitForThreadInit(10 * 1000)) {
			throw new HtException(getContext(), "initLoolupTalker", "Cannot start thread");
		}


		if (!lookupTalker_m.waitForConnection(10 * 1000)) {
			throw new HtException(getContext(), "initLevel1", "Cannot complete connection");
		}


	}

	private void shutdownAdminTalker() throws Exception {
		if (adminTalker_m != null) {

			// may be tineout exception
			try {
				adminTalker_m.sendDeregisterCommand();
			} catch (Throwable e) {
			}


			// stop event threads
			adminTalker_m.signalToShutdown();
			adminTalker_m.closeComChannel();

			try {
				adminTalker_m.join(20 * 1000);
			} catch (Throwable e) {
			}

			adminTalker_m = null;
		}

	}

	private void shutdownLookupTalker() throws Exception {
		if (lookupTalker_m != null) {




			// stop event threads
			lookupTalker_m.signalToShutdown();
			lookupTalker_m.closeComChannel();

			try {
				lookupTalker_m.join(20 * 1000);
			} catch (Throwable e) {
			}

			lookupTalker_m = null;
		}

	}

	private void shutdownLevel1Talker() throws Exception {
		if (level1Talker_m != null) {

			// stop event threads
			level1Talker_m.signalToShutdown();
			level1Talker_m.closeComChannel();

			try {
				level1Talker_m.join(20 * 1000);
			} catch (Throwable e) {
			}

			level1Talker_m = null;
		}

	}
}
