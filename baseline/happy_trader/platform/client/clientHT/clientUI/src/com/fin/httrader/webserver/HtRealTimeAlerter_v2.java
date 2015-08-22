/*
 * HtRealTimeAlerter.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;


import com.fin.httrader.hlpstruct.AlertPriorities;

import com.fin.httrader.interfaces.HtEventListener;
import com.fin.httrader.managers.RtConfigurationManager;
import com.fin.httrader.managers.RtDatabaseManager;
import com.fin.httrader.managers.RtGlobalEventManager;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.HtUtils;

import com.fin.httrader.utils.SingleReaderQueue;
import com.fin.httrader.utils.SingleReaderQueueExceedLimit;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import java.io.IOException;
import java.util.Iterator;


import java.util.Vector;
import java.util.logging.Level;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import javax.servlet.ServletException;
import javax.servlet.ServletOutputStream;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;


/**
 *
 * @author Victor_Zoubok
 * this serves as a servlets providing real-time alert propagation
 * multiple instances - one per request
 */
public class HtRealTimeAlerter_v2 extends HtServletsBase implements HtEventListener {

	// we will not deliver larger rowsets
	private static final long MAX_ROWS_QUERY = 1000;
	// queue for holding event data
	//private Queue queue_m = new Queue();
	//private LinkedBlockingQueue queue_m = new LinkedBlockingQueue();
	private SingleReaderQueue queue_m = new SingleReaderQueue("Real Time Alerter Queue", true, RtConfigurationManager.StartUpConst.MAX_ELEMENTS_IN_QUEUE);
	// event sequence number
	private long lastSequenceNumber_m = -1;
	StringBuilder runNameFilter_m = new StringBuilder();
	StringBuilder sessionFilter_m = new StringBuilder();
	StringBuilder contentFilter_m = new StringBuilder();
	Pattern p_runNameFilter_m = null;
	Pattern p_sessionFilter_m = null;
	Pattern p_contentFilter_m = null;
	long beginDate_m = -1;
	int priorityFilter_m = -1;
	// if we still in delayed state
	boolean delayedForSubscriber_m = false;

	// -----------------------------
	/** Creates a new instance of HtRealTimeAlerter */
	public HtRealTimeAlerter_v2() {
	}

	public String getContext() {
		return this.getClass().getSimpleName();
	}

	// this is expected to be jsp
	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {

		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		HtLog.log(Level.INFO, getContext(), "doGet", "Started alert propagation, instance: " + this.hashCode());

		try {

			// set this page UID
			setUniquePageId(getStringParameter(req, "page_uid", false));

			// initial filter parameters
			contentFilter_m.append(getStringParameter(req, "content_filter", true));
			runNameFilter_m.append(getStringParameter(req, "run_name_filter", true));
			sessionFilter_m.append(getStringParameter(req, "session_filter", true));
			beginDate_m = getIntParameter(req, "bdate_l", true);
			priorityFilter_m = (int) getIntParameter(req, "priority_filter", true);




			String contentFilterPtrn = contentFilter_m.toString().replaceAll("%", "[\\\\w\\\\s]*");
			String runNameFilterPtrn = runNameFilter_m.toString().replaceAll("%", "[\\\\w\\\\s]*");
			String sessionFilterPtrn = sessionFilter_m.toString().replaceAll("%", "[\\\\w\\\\s]*");

			p_runNameFilter_m = Pattern.compile(runNameFilterPtrn);
			p_sessionFilter_m = Pattern.compile(sessionFilterPtrn);
			p_contentFilter_m = Pattern.compile(contentFilterPtrn);




			//
			sendFirstDummyBytes(res);
			sendHeartBeat(res, HtDateTimeUtils.time2SimpleString_Gmt(HtDateTimeUtils.getCurGmtTime()));
			initialize(res);


			Vector list = new Vector();
			XmlEvent event = null;
			while (true) {


				//event = (XmlEvent) queue_m.get(RtConfigurationManager.StartUpConst.HEARTBEAT_TIMEOUT_SEC * 1000);
				//event = (XmlEvent) queue_m.poll(RtConfigurationManager.StartUpConst.HEARTBEAT_TIMEOUT_SEC * 1000,  TimeUnit.MILLISECONDS);

				queue_m.get(list, RtConfigurationManager.StartUpConst.RTPRICE_MAN_HEARTBEAT_TIMEOUT_SEC * 1000);
				for (Iterator it = list.iterator(); it.hasNext();) {

					event = (XmlEvent) it.next();

					if (event != null) {
						if (event.getType() == XmlEvent.DT_CommonXml && event.getEventType() == XmlEvent.ET_AlgorithmAlert) {


							// crack alert data and send RT data
							crackXmlEvent(res, event);

						} else {
							throw new HtException(getContext(), "doGet", "Wrong alert type");
						}


						event = null;

					} else {
						HtLog.log(Level.FINE, getContext(), "doGet", "Sending heartbeat...");
						sendHeartBeat(res, HtDateTimeUtils.time2SimpleString_Gmt(HtDateTimeUtils.getCurGmtTime()));

					}

				}

								
			} // end loop



		} catch (Throwable e) {
			HtLog.log(Level.WARNING, getContext(), "doGet", "Exception in alert propagation servlet: \"" + e.getMessage() + "\"");

			try {
				sendErrorEntry(res, HtDateTimeUtils.time2SimpleString_Gmt(HtDateTimeUtils.getCurGmtTime()), e.getMessage());

			} catch (Throwable e2) {
				HtLog.log(Level.WARNING, getContext(), "doGet", "Exception in sending error message: \"" + e2.getMessage() + "\"");
			}
		}


		deinitialize();
		HtLog.log(Level.INFO, getContext(), "doGet", "Finished alert propagation, instance: " + this.hashCode());
		
		return true;
	}

	public void onEventArrived(XmlEvent event) throws SingleReaderQueueExceedLimit{
		
		queue_m.put(event);
		lastSequenceNumber_m = event.getSequenceNumber();
		

	}

	// --------------------------------------
	// Helpers
	private String prepareRealData(String realdata) {

		int dsize = realdata.length();

		StringBuilder cut_data = new StringBuilder();
		if (dsize > 20) {

			cut_data.append(realdata.substring(0, 20).replaceAll("[']", "\\\\'"));
			cut_data.append("...");
		} else {
			cut_data.append(realdata.substring(0, dsize).replaceAll("[']", "\\\\'"));
		}


		return cut_data.toString();
	}

	private void sendDataChunk(HttpServletResponse res, String data) throws Exception {
		res.getOutputStream().print(data);
		res.flushBuffer();
	}

	private void sendJScriptDataChunk(HttpServletResponse res, String data) throws Exception {
		// data is the content of JScript
		res.getOutputStream().print("<script type=\"text/javascript\">");
		res.getOutputStream().print(data);
		res.getOutputStream().print("</script>");
		res.flushBuffer();
	}

	private void sendErrorEntry(HttpServletResponse res, String dateStr, String errorMsg) throws Exception {
		res.getOutputStream().print("<script type=\"text/javascript\">");
		res.getOutputStream().print("window.parent.em('");
		res.getOutputStream().print(dateStr);
		res.getOutputStream().print("','");
		res.getOutputStream().print(errorMsg);
		res.getOutputStream().print("');");
		res.getOutputStream().print("</script>");
		res.flushBuffer();
	}

	private void sendHeartBeat(HttpServletResponse res, String dateStr) throws Exception {
		res.getOutputStream().print("<script type=\"text/javascript\">");
		res.getOutputStream().print("window.parent.hb('");
		res.getOutputStream().print(dateStr);
		res.getOutputStream().print("');");
		res.getOutputStream().print("</script>");
		res.flushBuffer();
	}

	private void sendCutoffTime(HttpServletResponse res, long cutofftime, long beginTimeStamp) throws Exception {
		res.getOutputStream().print("<script type=\"text/javascript\">");
		res.getOutputStream().print("window.parent.ct(");
		res.getOutputStream().print(cutofftime);
		res.getOutputStream().print(",");
		res.getOutputStream().print(beginTimeStamp);
		res.getOutputStream().print(");");
		res.getOutputStream().print("</script>");
		res.flushBuffer();
	}

	private void sendRtEntry(HttpServletResponse res, String dateStr, String priorityStr, String runName, String session, String id, String realData, int priority) throws Exception {
		res.getOutputStream().print("<script type=\"text/javascript\">");
		res.getOutputStream().print("window.parent.re('");
		res.getOutputStream().print(dateStr);
		res.getOutputStream().print("','");
		res.getOutputStream().print(priorityStr);
		res.getOutputStream().print("',");
		res.getOutputStream().print(priority);
		res.getOutputStream().print(",'");
		res.getOutputStream().print(runName);
		res.getOutputStream().print("','");
		res.getOutputStream().print(session);
		res.getOutputStream().print("','");
		res.getOutputStream().print(id);
		res.getOutputStream().print("','");


		res.getOutputStream().print(realData.replaceAll("[']", "\\\\'"));
		res.getOutputStream().print("');");
		res.getOutputStream().print("</script>");
		res.flushBuffer();
	}

	private void sendFirstDummyBytes(HttpServletResponse res) throws Exception {
		// send this to force IE to parse
		StringBuilder dummydata = new StringBuilder();
		dummydata.setLength(256);
		sendDataChunk(res, dummydata.toString());
	}

	private void initialize(HttpServletResponse res) throws Exception {
		// subscribe for alerts
		RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).subscribeForEvent(XmlEvent.ET_AlgorithmAlert, 15, this);

		// then we need to suspend events propagation
		RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).forceDelayForSubscriber(this);
		delayedForSubscriber_m = true;


		// wait until DB is consistent
		try {
			HtCommandProcessor.instance().get_HtDatabaseProxy().remote_waitDatabaseManagerWritePendingEvents(this.lastSequenceNumber_m, 100);
			
		} catch (Throwable e) {
			// to avoid dead locking
			RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).switchOffDelayForSubscriber(this);
			delayedForSubscriber_m = false;

			throw new HtException(getContext(), "initialize", e.getMessage());
		}


		// TODO!!!
		// here we need to read all from DB
		// if we need to resend that

		readCutoffTime(res);


		// at this moment we are resuming operations
		RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).switchOffDelayForSubscriber(this);
		delayedForSubscriber_m = false;

	}

	private void deinitialize() {


		// if still is in delayed state
		if (delayedForSubscriber_m) {
		
			RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).switchOffDelayForSubscriber(this);
			
			
		}


		// uninitialize
		try {
			RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).unsubscribeForEvent(XmlEvent.ET_AlgorithmAlert, 15, this);
		} catch (Throwable e) {
			HtLog.log(Level.WARNING, getContext(), "deinitialize", "Error in unsubscribing from alerts: \"" + e.getMessage() + "\"");
		}

		queue_m.releaseMonitoring();

	}



	private void readCutoffTime(HttpServletResponse res) throws Exception {

		long cutofftime = HtCommandProcessor.instance().get_HtDatabaseProxy().remote_getAlertLastDateTime(
						contentFilter_m.toString(),
						runNameFilter_m.toString(),
						sessionFilter_m.toString(),
						priorityFilter_m,
						beginDate_m,
						-1);

		sendCutoffTime(res, cutofftime, beginDate_m);
	}

	private void crackXmlEvent(HttpServletResponse res, XmlEvent event) throws Exception {
		XmlParameter xmlparameter = event.getAsXmlParameter();

		// check according filter criteria
		String run_name_event = xmlparameter.getString("run_name");
		String content_event = xmlparameter.getString("data");
		String small_content_event = xmlparameter.getString("small_data");
		String session_event = event.getSource();
		int priority_event_i = (int) xmlparameter.getInt("priority");

		String priority_event = AlertPriorities.getAlertpriorityByName(priority_event_i);


		if (priorityFilter_m >= 0) {
			if (priority_event_i > priorityFilter_m) {
				return;
			}
		}

		if (beginDate_m > 0) {
			if (event.getEventDate() < beginDate_m) {
				return;
			}
		}


		if (contentFilter_m.length() > 0) {
			Matcher matcher_content = this.p_contentFilter_m.matcher(content_event);
			if (!matcher_content.find()) {
				return;
			}
		}

		if (runNameFilter_m.length() > 0) {
			Matcher matcher_run_name = this.p_runNameFilter_m.matcher(run_name_event);
			if (!matcher_run_name.find()) {
				return;
			}
		}

		if (sessionFilter_m.length() > 0) {
			Matcher matcher_session = this.p_sessionFilter_m.matcher(session_event);
			if (!matcher_session.find()) {
				return;
			}
		}

		sendRtEntry(res,
						HtDateTimeUtils.time2SimpleString_Gmt(event.getEventDate()),
						priority_event,
						run_name_event,
						session_event,
						xmlparameter.getUid("id").toString(),
						small_content_event,
						priority_event_i);



	}

	public String getListenerName() {
		return " listener: [ " + getContext() + " ] ";
	}
}
