/*
 * RtTradingServer.java
 *
  *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.managers;

import com.fin.httrader.model.HtServerProxy;

import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.*;
import com.fin.httrader.utils.transport.HtInquotePacket;
import com.fin.httrader.utils.hlpstruct.HtLevel2Data;
import com.fin.httrader.utils.hlpstruct.HtRtData;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import com.fin.httrader.utils.hlpstruct.XmlHandler;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import com.fin.httrader.utils.win32.*;

import com.fin.httrader.interfaces.*;

import com.fin.httrader.utils.hlpstruct.HtLevel1Data;
import com.fin.httrader.utils.transport.HtCommonSocket;
import com.fin.httrader.utils.transport.HtTCPIPSocket;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetSocketAddress;

import java.net.Socket;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;
import java.util.TreeSet;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;
import java.util.logging.Level;
import jsync.Event;
import org.xml.sax.SAXException;



// ---------------------------------------------
//
/**
 *
 * @author victor_zoubok
 * This class incapsulate communication to trading server.
 * For each trading server an instance if this class is instantiated
 */
public class RtTradingServer implements HtEventListener {

	static class HtRtSendingThread extends RtTradingServerBaseThread<RtTradingServer> {

		private XmlHandler xmlHandler_m = new XmlHandler();

		@Override
		protected String getClassContext() {
			return HtRtSendingThread.class.getSimpleName();
		}

		@Override
		public void doRun() throws Exception {
					// started


			// connect to the server
			HtCommonSocket socket = RtTradingServer.connectToServerUsingTCPIP(getBaseReference().getServerHost(), getBaseReference().getServerPort());

			performDataStreamInitializations( socket,
							new DataInputStream(socket.getInputStream()),
							new DataOutputStream(socket.getOutputStream())
			);



			// send command for socket selector
			// //////

			RtTradingServer.sendServerAcceptCommand(getOutputStream(), "quote_processor");

			XmlParameter xml_parameter = new XmlParameter();
			StringBuilder xml_parameter_buf = new StringBuilder();

			//HtInquotePacket packetToSend = new HtInquotePacket();
			int event_type = -1;


			// go into sending loop

			ArrayList list = new ArrayList();
			byte[] htrtdata_buffer = new byte[QuoteSerializer.getHtRtDataSize()];
			byte[] level1data_buffer = new byte[QuoteSerializer.getLevel1DataSize()];
			

			while (true) {

				try {

					// wait infinetely
					getBaseReference().getRtQueue().get(list);

					for (Iterator it = list.iterator(); it.hasNext();) {

						XmlEvent event_data = (XmlEvent) it.next();

						event_type = event_data.getEventType();
						if (event_type == XmlEvent.ET_RtProviderSynchronizationEvent) {
							XmlEvent.convertEventToXmlParameter(event_data, xml_parameter);
							xmlHandler_m.serializeParameter(xml_parameter, xml_parameter_buf);
							byte[] data_to_send = HtStringByteConverter.encodeUTF8(xml_parameter_buf.toString());
							HtInquotePacket.<DataOutputStream>send(HtInquotePacket.FLAG_XMLEVENT_PACKET, data_to_send, getOutputStream());
							HtLog.log(Level.INFO, getClassContext(), "run", "ET_RtProviderSynchronizationEvent arrived !!!!");
							
						} else if (event_type == XmlEvent.ET_RtProviderTicker) {
							HtRtData rt_data = event_data.getAsRtData();
							QuoteSerializer.serializeRtDataToBinary(rt_data, htrtdata_buffer);
							HtInquotePacket.<DataOutputStream>send(HtInquotePacket.FLAG_RT_PACKET, htrtdata_buffer, getOutputStream());

						} else if (event_type == XmlEvent.ET_Level2Data) {

							HtLevel2Data level2_data = event_data.getAsLevel2Data();
							//byte[] level2data_buffer = new byte[QuoteSerializer.getLevel2DataSize(level2_data)];
							
							byte[] level2data_buffer = QuoteSerializer.serializeLevel2ToBinary(level2_data);
							HtInquotePacket.<DataOutputStream>send(HtInquotePacket.FLAG_LEVEL2_PACKET, level2data_buffer, getOutputStream());

						} else if (event_type == XmlEvent.ET_Level1Data) {

							HtLevel1Data level1_data = event_data.getAsLevel1Data();

							QuoteSerializer.serializeLevel1ToBinary(level1_data, level1data_buffer);
							HtInquotePacket.<DataOutputStream>send(HtInquotePacket.FLAG_LEVEL1_PACKET, level1data_buffer, getOutputStream());

						}

					} // end event loop

				} catch (Throwable e) {

					// wait a bit
					//Thread.currentThread().sleep(500);

					// in case of exception kill resources
					HtLog.log(Level.WARNING, getClassContext(), "run", "Exception happened inside processing loop: " + e.getMessage());

					// check for shtdown out of loop
					if (waitShutdownEvent(0)) {
						break;
					}

					// logging
					RtTradingServerManager.instance().pushLocalServerEvent(getBaseReference().getServerName(), HtServerProxy.SRV_COMMAND_LOCAL_EXCEPTION, "RT Sender exception: " + e.getMessage());

					//RtIdleManager.instance().signalCriticalError(getClassContext(), server_m.getServerName(), "RT Sender exception: " + e.getMessage());


					// signal if not signalled
					setErroredStatus();


				}

				// seems the time of shutdown
				if (waitShutdownEvent(0)) {
					break;
				}



			} // end loop
		}
	};


	static class HtEventReceivingThread extends RtTradingServerBaseThread<RtTradingServer> {

		@Override
		protected String getClassContext() {
			return HtEventReceivingThread.class.getSimpleName();
		}

		private void dumpEventToFile(HtInquotePacket in_packet, Throwable cause)
		{
			try {
				StringBuilder baseFile = new StringBuilder(HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getLogDirectory());
				baseFile.append(File.separatorChar).append("event_receive_error_").append(HtDateTimeUtils.getCurGmtTime());
				
				
				String filename = baseFile + ".dump";
				String filname_acc = baseFile + "_reason.txt";
				
				StringBuilder content_acc = new StringBuilder("Reason:\n");
				content_acc.append(HtException.getFullErrorString(cause)).append("\nType:\n").append(in_packet.getIntType()).append("\n----------------------");
								
				HtFileUtils.writeContentToFile(filename, in_packet.getData());
				HtFileUtils.writeContentToFile(filname_acc, content_acc.toString());
				
			}
			catch(Throwable e)
			{
				HtLog.log(Level.WARNING, getClassContext(), "dumpEventToFile", "Exception: " + e.getMessage());
			}
		}




		@Override
		public void doRun() throws Exception {

			// connect to the server

			HtCommonSocket socket = RtTradingServer.connectToServerUsingTCPIP(getBaseReference().getServerHost(), getBaseReference().getServerPort());

			performDataStreamInitializations( socket,
							new DataInputStream(socket.getInputStream()),
							new DataOutputStream(socket.getOutputStream())
			);


			// send command for socket selector
			// ////// ////

			RtTradingServer.sendServerAcceptCommand(getOutputStream(), "out_processor");

			// Xml Handler
			XmlHandler xmlEventHandler = new XmlHandler();

			
			String xmlcontent = "";

			HtInquotePacket in_packet = new HtInquotePacket();
			XmlParameter in_xmlparam = new XmlParameter();
			HtRtData rt_data = new HtRtData();
			HtLevel1Data level1_data = new HtLevel1Data();
			HtLevel2Data level2_data = new HtLevel2Data();


			// go into sending loop
			while (true) {

				try {


					in_packet.read(getInputStream());

					XmlEvent event = new XmlEvent();


					if (in_packet.getFlag() == HtInquotePacket.FLAG_RT_PACKET) {
						QuoteSerializer.deserializeRtDataFromBinary(rt_data, in_packet.getData());
						RtRealTimeProviderManager.createXmlEventRtData(rt_data, event);
					} else if (in_packet.getFlag() == HtInquotePacket.FLAG_LEVEL1_PACKET) {
						QuoteSerializer.deserializeLevel1FromBinary(level1_data, in_packet.getData());
						RtRealTimeProviderManager.createXmlEventLevel1(level1_data, event);
					} else if (in_packet.getFlag() == HtInquotePacket.FLAG_LEVEL2_PACKET) {
						QuoteSerializer.deserializeLevel2FromBinary(level2_data, in_packet.getData());
						RtRealTimeProviderManager.createXmlEventLevel2(level2_data, event);
					}
					else if (in_packet.getFlag() == HtInquotePacket.FLAG_XMLEVENT_PACKET) {

						// must be correct encoding :)
						
						xmlcontent = HtStringByteConverter.decodeWin1251(in_packet.getData());
						xmlEventHandler.deserializeParameter(xmlcontent, in_xmlparam);
						XmlEvent.convertXmlParameterToEvent(event, in_xmlparam);
					} else {
						throw new HtException(getClassContext(), "run", "Invalid packet type: " + in_packet.getFlag());
					}

				
					// push data to event manager
					RtGlobalEventManager.instance().pushCommonEvent(event);

					if (waitShutdownEvent(0)) {
						break;
					}

				} catch (SAXException sa) {
					// wait a bit
					Thread.sleep(200);

				
					// in case of exception kill resources
					HtLog.log(Level.WARNING, getClassContext(), "run", "SAX Exception happened inside processing loop: '" + sa.getMessage() );
					
					dumpEventToFile(in_packet, sa);

					// logging
					RtTradingServerManager.instance().pushLocalServerEvent(getBaseReference().getServerName(), HtServerProxy.SRV_COMMAND_LOCAL_EXCEPTION, "SAX event receiver exception: " + sa.getMessage());

				// here not fatal - ony parser
				// signal if not signalled
				//if (!errorInThread_m.waitEvent(0)) {
				//	errorInThread_m.signal();
				//}

				} catch (Throwable e) {

					// wait a bit
					Thread.sleep(200);
					
					// in case of exception kill resources
					HtLog.log(Level.WARNING, getClassContext(), "run", "Exception happened inside processing loop: " + e.getMessage());

					dumpEventToFile(in_packet, e);

					// check for shtdown out of loop
					if (waitShutdownEvent(0)) {
						break;
					}

					// logging
					RtTradingServerManager.instance().pushLocalServerEvent(getBaseReference().getServerName(), HtServerProxy.SRV_COMMAND_LOCAL_EXCEPTION, "Event Receiver exception: " + e.getMessage());

					//RtIdleManager.instance().signalCriticalError(getClassContext(), server_m.getServerName(), "Event Receiver exception: " + e.getMessage());

					// signal if not signalled
					setErroredStatus();

				}

				// seems the time of shutdown
				if (waitShutdownEvent(0)) {
					break;
				}


			} // end loop

		}
	};

	// -------------------------------------------------

	private static final int WAIT_MSEC_ISSUE_COMMAND = 20 * 1000; //max seconds to wait until CPP server returns from the previous command
	private static final int WAIT_SEC_DB_PENDING_FINISH = 200;
	// command sockets
	private HtCommonSocket cmdSock_m = null;
	private InputStream cmdIn_m = null;
	private OutputStream cmdOut_m = null;
	// process
	private RtTradingServerProcess pr_m = null;
	// communication port
	private int port_m = -1;

	private StringBuilder serverHost_m = new StringBuilder("");
	// id of the server object
	private StringBuilder id_m = new StringBuilder();
	// thread responsible for sending Rt data the connected server
	private HtRtSendingThread rtSender_m = new HtRtSendingThread();
	// event receiving thread
	private HtEventReceivingThread eventReceiver_m = new HtEventReceivingThread();
	// that means that current command is executed
	//private Event processingCppServerCommand_m = new Event();

	//private Event idleCppServerCommand_m = new Event(true); // signalled
	
	private  Lock idleCppServerCommand_m = new ReentrantLock();

	// that means that the command to deinitialize was done
	private Event processingDeinitialization_m = new Event();
	// real-time data
	private SingleReaderQueue rtData_m = new SingleReaderQueue("CPP Trading Server Queue", true, RtConfigurationManager.StartUpConst.MAX_ELEMENTS_IN_QUEUE);
	// to filter out what RT providers we are going to allow for the server
	HashSet<String> rtProvidersToSubscribe_m = new HashSet<String>();
	// this is last sequence number of being propagated events
	private long lastSequenceNumber_m = -1;

	// log file name returned by cpp server
	private StringBuilder logFileName_m = new StringBuilder();

	// -----------------------------------------------
	protected String getContext() {
		return getClass().getSimpleName();
	}

	public SingleReaderQueue getRtQueue() {
		return rtData_m;
	}

	public String getLogFileName() {
		return logFileName_m.toString();
	}

	public void setUpLogFileName(String logFileName) {

		logFileName_m.setLength(0);
		logFileName_m.append(logFileName);

	}

	// finish and wait RT sender and event receiver threads
	public void finishAndWaitForServiceThreads(int msec) {

		// stop the RT thread
		rtSender_m.signalToShutdown();

		rtData_m.shutdown();
		/*
		try {
			rtData_m.put(new XmlEvent());
		} catch (Throwable e) {
		}
		*/

		try {
			rtSender_m.join(msec);
		} catch (Throwable e) {
		}


		// stop event threads
		eventReceiver_m.signalToShutdown();
		eventReceiver_m.closeComChannel();

		try {
			eventReceiver_m.join(msec);
		} catch (Throwable e) {
		}

		// release that queue from monitoring
		rtData_m.releaseMonitoring();


	}

	public void startAndWaitForServiceThreads(int mscec) throws Exception {
		if (isServiceThreadsStarted()) {
			throw new HtException(getContext(), "startAndWaitForServiceThreads", "RT service thread already started");
		}

		rtSender_m.resetStarted();
		rtSender_m.resetToShutdown();

		// rt data sender
		rtSender_m.setBaseReference(this);
		rtSender_m.start();


		if (!rtSender_m.waitForThreadInit(mscec)) {
			throw new HtException(getContext(), "startAndWaitForServiceThreads", "Cannot start RT sending thread");
		}

		// event receiver
		eventReceiver_m.resetStarted();
		eventReceiver_m.resetToShutdown();

		eventReceiver_m.setBaseReference(this);
		eventReceiver_m.start();

		if (!eventReceiver_m.waitForThreadInit(mscec)) {
			throw new HtException(getContext(), "startAndWaitForServiceThreads", "Cannot start Event receiver thread");
		}


	}

	public boolean isRtThreadError() {
		return (rtSender_m.isErrorOccured() || eventReceiver_m.isErrorOccured());
	}

	/** Creates a new instance of RtTradingServer */
	public RtTradingServer(
					String serverid,
					String serverExecutable,
					Map<String, String> newEntries,
					String serverHost,
					int serverPort,
					int servicePort,
					int spawnerPort,
					boolean useSpawner,
					boolean do_not_create_process,
					//boolean force_debug_output,
					Set<String> rtProvidersToSubscribe // rt providers to subscribe
					) throws Exception {
		try {

		
			// server id
			id_m.append(serverid);
			port_m = serverPort;

			// resolve server host
			if (serverHost != null)
				serverHost_m.append(serverHost.toLowerCase());
			else
				serverHost_m.append("127.0.0.1");

			if (rtProvidersToSubscribe != null)
				rtProvidersToSubscribe_m.addAll(rtProvidersToSubscribe);

			

			// ctor must launch the process
			if (!do_not_create_process) {

				// spawner port means we use spawn process
				if (!useSpawner) {
						HtLog.log(Level.INFO, getContext(), getContext(), "Locally spawn server executable");
						pr_m = new RtTradingServerLocalProcess();
				}
				else {
						HtLog.log(Level.INFO, getContext(), getContext(), "Use remote service to spawn executable at: " + getServerHost()+":"+spawnerPort);
						pr_m = new RtTradingServerRemoteProcess(getServerHost(), spawnerPort );
				}
				
				pr_m.startProcess(serverExecutable, serverPort, servicePort, serverid, newEntries);

			}

			// creates socket object and conect to server

			cmdSock_m = RtTradingServer.connectToServerUsingTCPIP(getServerHost(), getServerPort());

			//cmdIn_m = new DataInputStream(cmdSock_m.getInputStream());
			//cmdOut_m = new DataOutputStream(cmdSock_m.getOutputStream());

			cmdIn_m = cmdSock_m.getInputStream();
			cmdOut_m = cmdSock_m.getOutputStream();

			
			// subscribe to all RT data, will filter out later
			RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).subscribeForEvent(XmlEvent.ET_RtProviderTicker, 1, this);
			RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).subscribeForEvent(XmlEvent.ET_Level2Data, 1, this);
			RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).subscribeForEvent(XmlEvent.ET_Level1Data, 1, this);
			RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).subscribeForEvent(XmlEvent.ET_SynchronizationEvent, 1, this);
			RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).subscribeForEvent(XmlEvent.ET_RtProviderSynchronizationEvent, 1, this);


			// send selector command to select the successfull client socket thread
			// //////
			sendServerAcceptCommand(cmdOut_m, "inqueue_controller");

			//HtLog.log(Level.INFO, getContext(), getContext(), "Trading server init selector command length="+data_buf.length);
			// //////


			HtLog.log(Level.INFO, getContext(), getContext(), "Instantiated");

		}
		catch (Throwable e) {

			// timeout = 5 secs
			finishAndWaitForServiceThreads(5000);

			cleanResources();

			// propagate
			throw new HtException(getContext(), getContext(), "Error on initialization: " + e.getMessage());
		}


	}

	public void normalShutdown() throws Exception {
		XmlParameter currentXmlCommand = null;
		XmlParameter response = null;
		try {
			if (!isServiceThreadsStarted()) {
				HtLog.log(Level.WARNING, getContext(), "normalShutdown", "The thread was not actually started");
			}

			if (processingDeinitialization_m.waitEvent(0)) {
				throw new HtException(getContext(), "normalShutdown", "Shutdown in process for server: " + getServerName());
			}
			processingDeinitialization_m.signal();

			currentXmlCommand = new XmlParameter();
			currentXmlCommand.setCommandName("shutdown_server");

			//as this should kill socket connection ...
			response = issueCommand(currentXmlCommand);

			// -
			// here we have infinite timeouts because in case of hangs we will kill that from concurrent thread
			// infinite timeout
			finishAndWaitForServiceThreads(0);

			// wait inifinitely until process finishes,
			waitForProcessFinishes();

			// clean resources afterwords
			cleanResources();


			// OK
			XmlHandler mainCommandThreadHandler = new XmlHandler();
			RtTradingServerManager.instance().pushRemoteServerEvent(getServerName(), currentXmlCommand, response, mainCommandThreadHandler);

			RtTradingServerManager.instance().pushLocalServerEvent(getServerName(), HtServerProxy.SRV_COMMAND_OK, "Shutdown performed ok");

		} catch (Throwable e) {
			// ERROR
			RtTradingServerManager.instance().pushLocalServerEvent(getServerName(), HtServerProxy.SRV_COMMAND_LOCAL_EXCEPTION, e.getMessage());

			// propagate
			throw new HtException(getContext(), getContext(), "Error on normal shutdown: " + e.getMessage());
		}

	}

	// forcibly closes the connection and kill the process
	// as for normal shutdown there is a special command
	public void deinitialize() {
		// do only killing - kill everything
		try {

			// timeout=5 secs
			finishAndWaitForServiceThreads(5000);

		} catch (Throwable e) {
			RtTradingServerManager.instance().pushLocalServerEvent(getServerName(), HtServerProxy.SRV_COMMAND_LOCAL_EXCEPTION, e.getMessage());
			HtLog.log(Level.WARNING, getContext(), "deinitialize", "Error on killing the server: " + getServerName() + " - " + e.getMessage());
		} finally {
			// clean resourecs in either case
			cleanResources();
		}

		RtTradingServerManager.instance().pushLocalServerEvent(getServerName(), HtServerProxy.SRV_COMMAND_OK, "Killed");
	}

	public void suspendRtPropagation() throws Exception {
		// before send last synchronization event
		XmlEvent synch = new XmlEvent();
		synch.setEventType(XmlEvent.ET_SynchronizationEvent);
		RtGlobalEventManager.instance().pushCommonEvent(synch);


		// then suspend it thus making sure at
		RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).forceDelayForSubscriber(this);

	}

	public void resumeRtPropagation() throws Exception {
		RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).switchOffDelayForSubscriber(this);
	}

	public void waitDatabaseManagerWritePendingEvents() throws Exception {
		RtDatabaseManager.instance().waitDatabaseManagerWritePendingEvents(this.lastSequenceNumber_m, WAIT_SEC_DB_PENDING_FINISH);

	}

	// this function sends the command to the server and wait the response
	// this is blocking functions as command exchange occures in the main queue
	public XmlParameter issueCommand(XmlParameter request) throws Exception {


		try {
			if (!idleCppServerCommand_m.tryLock(WAIT_MSEC_ISSUE_COMMAND, TimeUnit.MILLISECONDS)) {
						throw new HtException(getContext(), "issueCommand", "The system still has not received the previous responce from the running CPP server: " + 
							getServerName() + " at least for " + WAIT_MSEC_ISSUE_COMMAND + " msec(s)");
			}
				
			StringBuilder out = new StringBuilder();
			XmlHandler mainCommandThreadHandler = new XmlHandler();
			mainCommandThreadHandler.serializeParameter(request, out);

			XmlParameter result = issueCommand(out.toString(), mainCommandThreadHandler);
			
			return result;
		} catch (Throwable e) {
			throw new HtException(getContext(), "issueCommand", e);
		} finally {
			// reset this
			//processingCppServerCommand_m.reset();

			// signal if not signalled
			idleCppServerCommand_m.unlock();
		}
	}

	public int getServerPort() {
		return port_m;
	}

	public String getServerHost()
	{
		return serverHost_m.toString();
	}

	// subscribes for retreival
	// there can be more that one source of data
	// this is called in the context of RT provider thread
	// which is definitely different from main UI thrad
	@Override
	public void onEventArrived(XmlEvent event) throws SingleReaderQueueExceedLimit {

		int etype = event.getEventType();

		if (etype == XmlEvent.ET_RtProviderSynchronizationEvent) {
			if (rtProvidersToSubscribe_m.contains(event.getAsXmlParameter().getString("provider_name"))) {
				rtData_m.put(event);
			}
		} else if (etype == XmlEvent.ET_Level2Data) {
			HtLevel2Data data = event.getAsLevel2Data();
			if (rtProvidersToSubscribe_m.contains(data.getProvider())) {
				rtData_m.put(event);
			}
		} else if (etype == XmlEvent.ET_Level1Data) {
			HtLevel1Data data = event.getAsLevel1Data();
			if (rtProvidersToSubscribe_m.contains(data.getProvider())) {
				rtData_m.put(event);
			}
		} else if (etype == XmlEvent.ET_RtProviderTicker) {

			// level 2 data - only to CPP server
			HtRtData data = event.getAsRtData();
			if (rtProvidersToSubscribe_m.contains(data.getProvider())) {
				rtData_m.put(event);
			}

		}

		// store last sequence number
		lastSequenceNumber_m = event.getSequenceNumber();
	}

	public Set<String> getSubscribedRTProviders() {
		return new TreeSet(rtProvidersToSubscribe_m);
	}

	public String getServerName() {
		return id_m.toString();
	}

	// ----------------------------------------------
	// helpers
	private boolean isServiceThreadsStarted() {
		return (rtSender_m.isStarted() && eventReceiver_m.isStarted());
	}

	private void waitForProcessFinishes() {
		if (pr_m != null)
				pr_m.waitForFinish();
	

	}

	private void cleanResources() {

		if (pr_m != null)
				pr_m.destroyProcess();
		
		try {
			if (cmdSock_m != null) {
				cmdSock_m.close();
			}
			if (cmdIn_m != null) {
				cmdIn_m.close();
			}
			if (cmdOut_m != null) {
				cmdOut_m.close();
			}
		} catch (Throwable e2) {
		}


		// unsubscribe for all RT events
		try {
			RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).unsubscribeForEvent(XmlEvent.ET_RtProviderTicker, 1, this);
			RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).unsubscribeForEvent(XmlEvent.ET_Level2Data, 1, this);
			RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).unsubscribeForEvent(XmlEvent.ET_Level1Data, 1, this);
			RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).unsubscribeForEvent(XmlEvent.ET_SynchronizationEvent, 1, this);
			RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).unsubscribeForEvent(XmlEvent.ET_RtProviderSynchronizationEvent, 1, this);

		} catch (Throwable e2) {
		}


	}

	private XmlParameter issueCommand(String request, XmlHandler mainCommandThreadHandler) throws Exception {
		XmlParameter response = new XmlParameter();
		//HtLog.log(Level.FINE, getContext(), "issueCommand", "Server \""+getServerName()+"\" going to issue the command:\n"+request, null);

		byte[] data_buf = request.getBytes();
		byte[] data_buf_length = HtMathUtils.convertLongToByte(data_buf.length);

		cmdOut_m.write(data_buf_length, 0, data_buf_length.length);
		cmdOut_m.write(data_buf, 0, data_buf.length);

		// read the response
		HtSocketUtils.readFullyFromStream(data_buf_length, 0, HtUtils.SOCKET_LENGTH_HEADER_BYTES, cmdIn_m);
		
		//cmdIn_m.readFully(data_buf_length, 0, HtUtils.SOCKET_LENGTH_HEADER_BYTES);
		long in_to_read = HtMathUtils.convertByteToLong(data_buf_length);
		if (in_to_read <= 0) {
			throw new HtException(getContext(), "issueCommand", "Zero response length");
		}

		byte[] response_bytes = new byte[(int) in_to_read];
		
		HtSocketUtils.readFullyFromStream(response_bytes, 0, (int)in_to_read, cmdIn_m);
		//cmdIn_m.readFully(response_bytes, 0, (int) in_to_read);
		
		//HtLog.log(Level.INFO, "foo!", "foo!", "Read bytes: " + in_to_read + ", content: [" +  new String(response_bytes) + "]" );

		// as we have in win-1251 encoding from CPP server
		String xmlcontent = HtStringByteConverter.decodeWin1251(response_bytes);
		mainCommandThreadHandler.deserializeParameter(xmlcontent, response);

		return response;
	}

	// -------------------------------------------------
	//
	//
	public static HtCommonSocket connectToServerUsingTCPIP(String host, int port) throws Exception {
		// timout 10 seconds
		return HtSocketUtils.connectToServerUsingTCPIP(host, port, 10*000, RtConfigurationManager.StartUpConst.MAX_CPP_SERVER_CONNECT_ATTEMPTS);
	}

	public static void sendServerAcceptCommand(OutputStream ostream, String command) throws Exception {
		XmlParameter selectorCommand = new XmlParameter();
		selectorCommand.setCommandName(command);
		StringBuilder selectorCommand_buf = new StringBuilder();
		(new XmlHandler()).serializeParameter(selectorCommand, selectorCommand_buf);

		//as this should kill socket connection ...
		byte[] data_buf = selectorCommand_buf.toString().getBytes();
		byte[] data_buf_length = HtMathUtils.convertLongToByte(data_buf.length);

		ostream.write(data_buf_length, 0, data_buf_length.length);
		ostream.write(data_buf, 0, data_buf.length);
	}

	@Override
	public String getListenerName() {
		return " listener: [ " + getContext() + " ] ";
	}
}
