/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.eventplugins;

import com.fin.httrader.hlpstruct.ThreadDescriptor;
import com.fin.httrader.interfaces.HtEventListener;
import com.fin.httrader.interfaces.HtEventPlugin;
import com.fin.httrader.managers.RtGlobalEventManager;
import com.fin.httrader.managers.RtTradingServerManager;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.IntParam;
import com.fin.httrader.utils.LongParam;
import com.fin.httrader.utils.SingleReaderQueueExceedLimit;
import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;

/**
 *
 * @author Victor_Zoubok
 * This is a specific layer running specified algorithm and server and the number of threads depending
 * on how busy is the specified thread
 */
public class HtAlgorithmRequestBroker implements HtEventPlugin, HtEventListener {

	private String serverName_m = null;
	private String alorithmName_m = null;
	private int maxThreadCount_m = 50;
	private int minThreadCount_m = 5;
	private int threadChunkSize_m = 5;
	private StringBuilder name_m = new StringBuilder();
	private final Object mtx_m = new Object();

	public static String getContext() {
		return HtAlgorithmRequestBroker.class.getSimpleName();
	}

	// --------------------------------
	@Override
	public Integer[] returnSubscribtionEventTypes() {

		// just custom event which is terned into alg event
		return new Integer[]{XmlEvent.ET_CustomEvent};

	}

	@Override
	public void initialize(Map<String, String> initdata) throws Exception {
		serverName_m = HtUtils.getParameterWithException(initdata, "SERVER_NAME");
		alorithmName_m = HtUtils.getParameterWithException(initdata, "ALGORITHM_NAME");

		RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).subscribeForEvent(XmlEvent.ET_CustomEvent, 5007, this);


	}

	@Override
	public void deinitialize() throws Exception {
		RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).unsubscribeForAllEvents(this);
	}

	@Override
	public String getEventPluginId() {
		return name_m.toString();
	}

	@Override
	public void setEventPluginId(String pluginId) {
		name_m.setLength(0);
		name_m.append(pluginId);
	}

	@Override
	public void execute(XmlEvent alertData) throws Exception {
		synchronized (mtx_m) {
			if (alertData.getEventType() == XmlEvent.ET_CustomEvent) {
				// process this
				if (alertData.getType() == XmlEvent.DT_CommonXml) {
					XmlParameter xml_param = alertData.getAsXmlParameter();
					String custom_event_name = xml_param.getCommandName();

					if (custom_event_name != null && custom_event_name.equals("custom_alg_event_delegator")) {

						String command = xml_param.getString("command");
						String command_data = xml_param.getString("command_data");
						boolean non_blocking = (xml_param.getInt("non_blocking") == 1);
						int tout_sec_blocking = (int) xml_param.getInt("tout_sec");

						// check if the server is started
						int status = HtCommandProcessor.instance().get_HtServerProxy().remote_returnServerStatus(serverName_m);
						if (status != RtTradingServerManager.SERVER_RUN_OK) {
							throw new HtException(getContext(), "execute", "Invalid trading server status: " + serverName_m + " - " + status);
						}

						// check if our algorithm is loaded
						boolean isLoaded = HtCommandProcessor.instance().get_HtServerProxy().remote_isAlgorithmLoaded(serverName_m, alorithmName_m);
						if (!isLoaded) {
							throw new HtException(getContext(), "execute", "Algorithm is not loaded: " + serverName_m + " - " + alorithmName_m);
						}

						// start new thread
						int last_thread = startThread();
						XmlEvent.createSimpleLog("", getEventPluginId(), CommonLog.LL_INFO, "Started thread for the server: " + serverName_m + " thread ID: " + last_thread);

						sendCustomMessage(command, command_data, last_thread, non_blocking, tout_sec_blocking);


						//
					}


				}


			}
		}
	}

	@Override
	public String getListenerName() {
		return "listener: [algorithm request broker listener]";
	}

	@Override
	public void onEventArrived(XmlEvent event) throws SingleReaderQueueExceedLimit {

		// chaise for a specific event here and issue stop thread

		if (event.getEventType() == XmlEvent.ET_CustomEvent) {
			if (event.getType() == XmlEvent.DT_CommonXml) {
				XmlParameter xml_param = event.getAsXmlParameter();
				String custom_event_name = xml_param.getCommandName();

				// process event how to stop working thread here
				if (custom_event_name != null) {
					if (custom_event_name.equals("custom_alg_event_stop_thread")) {
						try {

							int tid = (int) xml_param.getInt("tid_to_stop");

							stopThread(tid);
							XmlEvent.createSimpleLog("", getEventPluginId(), CommonLog.LL_INFO, "Stopped the thread for the server: " + serverName_m + " thread ID: " + tid);


						} catch (Throwable e) {
							XmlEvent.createSimpleLog("", getEventPluginId(), CommonLog.LL_ERROR, "Exception when stopping thread for the server: " + serverName_m + " - " + e.getMessage());

						}
					}
				}
			}
		}

	}

	int startThread() throws Exception {
		List<String> algList = new ArrayList<String>();
		algList.add(alorithmName_m);

		return HtCommandProcessor.instance().get_HtServerProxy().remote_startAlgorithmThread(serverName_m, algList);

	}

	void stopThread(int tid) throws Exception {

		HtCommandProcessor.instance().get_HtServerProxy().remote_stopAlgorithmThread(serverName_m, tid);

	}

	void sendCustomMessage(String command, String command_data, int tid, boolean non_blocking, int tout_sec_blocking) throws Exception {
		if (tid < 0) {
			throw new HtException(getContext(), "sendCustomMessage", "Thread must be valid for the server: " + serverName_m);
		}

		if (non_blocking) {

			XmlEvent.createSimpleLog("", getEventPluginId(), CommonLog.LL_INFO, "Sending non-blocking custom event to the server: " + serverName_m + ", thread ID: " + tid);


			// non-blocking data sending
			HtCommandProcessor.instance().get_HtServerProxy().remote_SendCustomEvent_toAlgorithm_NoWait(
							serverName_m,
							alorithmName_m,
							tid, // delegate to all threads
							command,
							HtUtils.hexlifyString(command_data, "ISO-8859-1")
			);

			sendResponseMessage(HtUtils.hexlifyString("NON-BLOCKING", "ISO-8859-1"));
		} else {

			XmlEvent.createSimpleLog("", getEventPluginId(), CommonLog.LL_INFO, "Sending blocking custom event to the server: " + serverName_m + ", thread ID: " + tid);


			// blocking data sending
			String out = HtCommandProcessor.instance().get_HtServerProxy().remote_SendCustomEvent_toAlgorithm(
							serverName_m,
							alorithmName_m,
							tid, // delegate to all threads
							command,
							HtUtils.hexlifyString(command_data, "ISO-8859-1"),
							tout_sec_blocking);

			sendResponseMessage(out);

		}



	}

	private void sendResponseMessage(String out) throws Exception
	{
			// need to delegate it to the servlet
			XmlEvent xmlevent = new XmlEvent();
			xmlevent.setEventType(XmlEvent.ET_CustomEvent);
			xmlevent.setType(XmlEvent.DT_CommonXml);

			xmlevent.getAsXmlParameter().setCommandName("custom_alg_event_return_result");
			xmlevent.getAsXmlParameter().setString("out", HtUtils.unhexlifyString(out, "ISO-8859-1"));


			RtGlobalEventManager.instance().pushCommonEvent(xmlevent);
	}

}
