/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.managers;

import com.fin.httrader.externalapi.HtExtScriptLogger;
import com.fin.httrader.hlpstruct.ExternalScriptStructure;
import com.fin.httrader.interfaces.HtDataSelection;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.HtStringByteConverter;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.Uid;
import com.fin.httrader.utils.hlpstruct.HtRtData;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import com.fin.httrader.utils.hlpstruct.XmlHandler;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import com.fin.httrader.utils.transport.HtInquoteCommonServiceRequestHistory;
import com.fin.httrader.utils.transport.HtInquoteCommonServiceRequestRunExtPackage;
import com.fin.httrader.utils.transport.HtInquotePacket;
import com.fin.httrader.utils.win32.QuoteSerializer;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketAddress;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Set;
import java.util.logging.Level;
import jsync.Event;

/**
 *
 * @author DanilinS
 * This class is a common service manager, for example from CPP service
 */
public class RtCommonServiceManager extends Thread implements RtManagerBase {

	private static class WorkingThread extends Thread {
		//private Event threadStarted_m = new Event();

		private Event toShutdown_m = new Event();
		private Socket socket_m = null;
		private RtCommonServiceManager base_m = null;

		protected String getContext() {
			return this.getClass().getSimpleName();
		}

		public WorkingThread(Socket socket, RtCommonServiceManager base) {
			socket_m = socket;
			base_m = base;

			start();
		}

		@Override
		public void run() {
			try {

				HtLog.log(Level.INFO, getContext(), "run", "Working processing thread started");

				// now do all our job
				final DataInputStream in = new DataInputStream(socket_m.getInputStream());
				final DataOutputStream out = new DataOutputStream(socket_m.getOutputStream());




				// read packet and wait
				HtInquotePacket in_packet = new HtInquotePacket();
				in_packet.read(in);

				if (in_packet.getFlag() == HtInquotePacket.FLAG_COMMON_SERVICE_REQUEST_EXT_PACKAGE_RUN) {
					executeExternalJarPackage(in_packet, out);

				} else if (in_packet.getFlag() == HtInquotePacket.FLAG_COMMON_SERVICE_REQUEST_HISTORY) {
					// we are reading history assuming we can break in the middle of reading
					returnHistoryData(in_packet, out);
				}



			} catch (Throwable e) {
				HtLog.log(Level.WARNING, getContext(), "run", "Exception on processing: " + e.getMessage());
			}

			closeSocket();
			base_m.removeWorkingThread(this);

			HtLog.log(Level.INFO, getContext(), "run", "Working processing thread finishing...");

		}

		public void closeSocket() {
			try {
				if (socket_m != null) {
					socket_m.close();
					socket_m = null;
				}
			} catch (Throwable e) {
			}
		}

		public void shutdown() {
			toShutdown_m.signal();
			closeSocket();

			try {
				join();
			} catch (InterruptedException e) {
			}

		}

		/**
		 * Helpers
		 */
		// this is just create a message and sends this to output
		private void createMessageToOutput(DataOutputStream out, String data) {
			try {
				XmlEvent event_data = new XmlEvent();

				event_data.setEventType(XmlEvent.ET_CustomEvent);
				event_data.getAsStringBuffer(true).append(HtUtils.wrapToCDataTags(data));

				XmlParameter xml_parameter = new XmlParameter();
				XmlEvent.convertEventToXmlParameter(event_data, xml_parameter);
				String xml_parameter_buf = XmlHandler.serializeParameterStatic(xml_parameter);

				byte[] data_to_send = HtStringByteConverter.encodeUTF8(xml_parameter_buf);
				HtInquotePacket.<DataOutputStream>send(HtInquotePacket.FLAG_XMLEVENT_PACKET, data_to_send, out);
			} catch (Throwable e) {
				HtLog.log(Level.WARNING, getContext(), "createMessageToOutput", "Cannot write to output: " + e.getMessage());
			}
		}

		private HtExtScriptLogger createLogger(Uid runUid, final DataOutputStream out) throws Exception {
			HtExtScriptLogger r = new HtExtScriptLogger(runUid) {

				@Override
				public void addStringEntry_Derived(String data) throws Exception {
					createMessageToOutput(out, data);
				}

				@Override
				public void addErrorEntry_Derived(String data) throws Exception {
					createMessageToOutput(out, data);
				}
			};

			return r;
		}

		private void executeExternalJarPackage(HtInquotePacket in_packet, DataOutputStream out) throws Exception
		{
			try {

				HtInquoteCommonServiceRequestRunExtPackage.RequestParameters rp = HtInquoteCommonServiceRequestRunExtPackage.parsePacket(in_packet);

				// just let recipents side taht transmission is finished
				if (rp.returnImmedaitely_m) {
					HtLog.log(Level.INFO, getContext(), "executeExternalJarPackage", "We are finishing the data transfer as was demanded");
					HtInquotePacket.<DataOutputStream>sendTransmisionFinish(out);
				}

				// just copy
				Uid runUid = HtCommandProcessor.instance().get_HtExternalApiProxy().remote_copyTemplateToWorkingDirectory(null, rp.packageName_m);

				
				// update
				ExternalScriptStructure estruct = new ExternalScriptStructure(runUid, ExternalScriptStructure.STATUS_PENDING, HtDateTimeUtils.getCurGmtTime(), -1, rp.packageName_m, "");
				HtCommandProcessor.instance().get_HtExternalApiProxy().remote_insertNewEntry(estruct);


				// create logger class
				HtExtScriptLogger logger = createLogger(runUid, out);

				HtCommandProcessor.instance().get_HtExternalApiProxy().remote_executeWholeJarInCallingThread(logger, runUid);

			} catch (Throwable e)
			{
				String msg = "Exception on processing request of type: FLAG_COMMON_SERVICE_REQUEST_EXT_PACKAGE_RUN: " + e.getMessage();
				HtLog.log(Level.WARNING, getContext(), "executeExternalJarPackage", msg);
				createMessageToOutput(out, msg);
			}
			finally 
			{
				// sned finish flag
				HtInquotePacket.<DataOutputStream>sendTransmisionFinish(out);
			}
		}

		private void returnHistoryData(HtInquotePacket in_packet, final DataOutputStream out) throws Exception {
			try {

				final byte[] htrtdata_buffer = new byte[QuoteSerializer.getHtRtDataSize()];

				HtInquoteCommonServiceRequestHistory.RequestParameters rp = HtInquoteCommonServiceRequestHistory.parsePacket(in_packet);

				HtDataSelection dataselector = new HtDataSelection() {

					@Override
					public void onSelectionFinish(long cnt) throws Exception {
						// need to notify we finish
						HtInquotePacket.<DataOutputStream>sendTransmisionFinish(out);
					}

					@Override
					public void newRowArrived(long ttime, String tsymbol, String tprovider, int ttype, int aflag, double tbid, double task, double topen, double thigh, double tlow, double tclose, double topen2, double thigh2, double tlow2, double tclose2, double tvolume, long tid, int color, int operation) throws Exception {
						// serialize as binary
						HtRtData rtdata = new HtRtData(ttime, tsymbol, tprovider, ttype, aflag, tbid, task, topen, thigh, tlow, tclose, topen2, thigh2, tlow2, tclose2, tvolume, tid, color, operation);
						QuoteSerializer.serializeRtDataToBinary(rtdata, htrtdata_buffer);
						HtInquotePacket.<DataOutputStream>send(HtInquotePacket.FLAG_RT_PACKET, htrtdata_buffer, out);

						// if shotdown
						if (toShutdown_m.waitEvent(0)) {
							throw new HtException(getContext(), "HtDataSelection", "Operation was forcibly interrupted");
						}
					}
				};

				// make a request

				HtCommandProcessor.instance().get_HtDatabaseProxy().remote_showDataAll_Callback_NoPaging(
								rp.profile_name_m,
								rp.provSymMap,
								rp.begin_date_m,
								rp.end_date_m,
								dataselector,
								-1,
								true // if no data is passed resilve by default
								);

			} catch (Throwable e) {
				HtLog.log(Level.WARNING, getContext(), "run", "Exception on processing request of type: FLAG_COMMON_SERVICE_REQUEST_HISTORY: " + e.getMessage());
			}
		}
	};
	// --------------------------------
	static private RtCommonServiceManager instance_m = null;
	private static final int MAX_WORK_THREADS = 4;
	private ServerSocket listener_m = null;
	private Event toShutdown_m = new Event();
	private Event started_m = new Event();
	private final Set<WorkingThread> workingThreads_m = new HashSet<WorkingThread>();

	static public RtCommonServiceManager create() throws Exception {
		if (instance_m == null) {
			instance_m = new RtCommonServiceManager();
		}

		return instance_m;
	}

	static public RtCommonServiceManager instance() {
		return instance_m;
	}

	protected final String getContext() {
		return this.getClass().getSimpleName();
	}

	// --------------------------
	public RtCommonServiceManager() throws Exception {
		HtLog.log(Level.INFO, getContext(), getContext(), "Instantiated");

		// start listening
		listener_m = new ServerSocket();
		listener_m.bind(new InetSocketAddress("127.0.0.1", RtConfigurationManager.StartUpConst.COMMON_SERVICE_LISTEN_PORT));

		HtLog.log(Level.INFO, getContext(), getContext(), "Started listening on port: " + RtConfigurationManager.StartUpConst.COMMON_SERVICE_LISTEN_PORT);

		start();

		if (!started_m.waitEvent(5000)) {
			throw new HtException(getContext(), getContext(), "Cannot start common service manager thread");
		}
	}

	public void removeWorkingThread(WorkingThread wthread) {
		synchronized (workingThreads_m) {
			workingThreads_m.remove(wthread);
		}
	}

	public void run() {
		try {

			started_m.signal();

			while (true) {

				HtLog.log(Level.INFO, getContext(), "run", "Waiting for incoming connection...");
				Socket fromclient = listener_m.accept();

				synchronized (workingThreads_m) {
					if (workingThreads_m.size() < MAX_WORK_THREADS) {
						workingThreads_m.add(new WorkingThread(fromclient, this));
					} else {
						HtLog.log(Level.WARNING, getContext(), "run", "Client request rejected as exceed the limit of: " + MAX_WORK_THREADS);

						try {
							fromclient.close();
						} catch (Throwable e) {
						}
					}

				}

				HtLog.log(Level.INFO, getContext(), "run", "Proceed client request");



				// seems the time of shutdown
				if (toShutdown_m.waitEvent(0)) {
					break;
				}
			}

		} catch (Throwable e) {
			HtLog.log(Level.WARNING, getContext(), "run", "Exception on processing: " + e.getMessage());
		}
	}

	@Override
	public void shutdown() {
		closeServerSocket();

		toShutdown_m.signal();


		// remove clients threads if existted
		Set<WorkingThread> wthSet = new HashSet<WorkingThread>();
		synchronized (workingThreads_m) {
			wthSet.addAll(workingThreads_m);
		}

		for (Iterator<WorkingThread> it = wthSet.iterator(); it.hasNext();) {
			WorkingThread wit = it.next();
			// forcibly shutdown
			wit.shutdown();
		}


		try {
			join();
		} catch (Throwable e) {
		}

		HtLog.log(Level.INFO, getContext(), "shutdown", "Finished");
	}

	/*
	 * Helpers
	 */
	private void closeServerSocket() {
		try {
			if (listener_m != null) {
				listener_m.close();
				listener_m = null;
			}
		} catch (Throwable e) {
		}
	}
}
