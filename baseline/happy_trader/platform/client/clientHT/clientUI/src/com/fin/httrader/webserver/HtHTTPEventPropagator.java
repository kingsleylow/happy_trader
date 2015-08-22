/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.interfaces.HtCommonLogSelection;
import com.fin.httrader.interfaces.HtDataSelection;
import com.fin.httrader.interfaces.HtDrawableObjectSelector;
import com.fin.httrader.interfaces.HtEventListener;
import com.fin.httrader.managers.RtConfigurationManager;
import com.fin.httrader.managers.RtGlobalEventManager;

import com.fin.httrader.model.HtCommandProcessor;

import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;

import com.fin.httrader.utils.SingleReaderQueue;
import com.fin.httrader.utils.SingleReaderQueueExceedLimit;
import com.fin.httrader.utils.Uid;
import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.utils.hlpstruct.HtDrawableObject;
import com.fin.httrader.utils.hlpstruct.HtPair;
import com.fin.httrader.utils.hlpstruct.HtRtData;
import com.fin.httrader.utils.hlpstruct.HtXmlEventExchangePacket;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import com.fin.httrader.utils.hlpstruct.XmlHandler;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.lang.Integer;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
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
 * @author DanilinS
 */
public class HtHTTPEventPropagator extends HtServletsBase implements HtEventListener {

	public static class CommonLogParameters {

		public String contextFilter = null;
		public String contentSmallFilter = null;
		public String threadFilter = null;
		public int level = -1;
		long begin_date_hist = -1;
		long end_date_hist = -1;

		//
		public Pattern p_contextFilter = null;
		public Pattern p_contentSmallFilter = null;
		public Pattern p_threadFilter = null;

		public void initPatterns() {
			p_contextFilter = Pattern.compile(contextFilter.toString().replaceAll("%", "[\\\\w\\\\s]*"));
			p_contentSmallFilter = Pattern.compile(contentSmallFilter.toString().replaceAll("%", "[\\\\w\\\\s]*"));
			p_threadFilter = Pattern.compile(threadFilter.toString().replaceAll("%", "[\\\\w\\\\s]*"));
		}
	};

	public static class RtDataParameters {

		long begin_date_hist = -1;
		long end_date_hist = -1;
		String profile_name_hist = null;
		boolean do_read_history = true;
		public Set<String> allowed_symbols_m = null;
	};

	public static class DrawableObjectParameters {

		public String shortTextFilter = null;
		public String textFilter = null;
		String profile_name_hist = null;
		long begin_date_hist = -1;
		long end_date_hist = -1;
		public Pattern p_shortTextFilter = null;
		public Pattern p_textFilter = null;

		public void initPatterns() {
			p_shortTextFilter = Pattern.compile(shortTextFilter.toString().replaceAll("%", "[\\\\w\\\\s]*"));
			p_textFilter = Pattern.compile(textFilter.toString().replaceAll("%", "[\\\\w\\\\s]*"));
		}
		boolean do_read_history = true;
		public Set<Integer> allowed_type = new HashSet<Integer>();
		public Set<String> allowed_symbols_m = null;
	}

	public static class LengthListenerHelper implements HtXmlEventExchangePacket.LengthListener {

		private ServletOutputStream ostrm_m = null;

		public LengthListenerHelper(ServletOutputStream ostrm) {
			ostrm_m = ostrm;
		}

		public void onContentLength(int contentLength) throws Exception {

			ostrm_m.print(Integer.toHexString(contentLength));
			ostrm_m.print("\r\n");

		}

		public void onChunkFinish() throws Exception {
			ostrm_m.print("\r\n");
			ostrm_m.flush();
		}
	};
	private static final int IDLE_TIME_SEC = 10;
	private static final int WAIT_DB_PEND_EVENTS_SEC = 200;
	private static final int HISTORY_ROWS_CHUNK = 10000;
	private SingleReaderQueue queue_m = new SingleReaderQueue("HTTP Event propagator", true, RtConfigurationManager.StartUpConst.MAX_ELEMENTS_IN_QUEUE);
	
	private String sessionId_m = null;
	private List<Integer> eventTypes_m = null;
	private XmlHandler handler_m = new XmlHandler();
	private Map<Integer, Object> eventFilterParameters_m = new HashMap<Integer, Object>();
	private LengthListenerHelper ll_m = null;
	private FileOutputStream debugstream_m = null;

	public String getContext() {
		return this.getClass().getSimpleName();
	}

	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		//res.setContentType("text/plain; charset=ISO-8859-1");
		//res.setContentType("application/octet-stream");
		forceNoCache(res);
		HtLog.log(Level.INFO, getContext(), "initialize_Get", "Started XML Event propagation servlet");
		ServletOutputStream ostrm = res.getOutputStream();

		res.setHeader("Transfer-Encoding", "chunked");
		res.setContentType("anybinary/type;charset=ISO-8859-1");
		res.flushBuffer();



		try {

			ll_m = new LengthListenerHelper(ostrm);

			// access
			String user = this.getStringParameter(req, "user", false);
			String hashed_password = this.getStringParameter(req, "hashed_password", false);
			String userIp = req.getRemoteHost();

			// common params



			boolean is_debug = this.getBooleanParameter(req, "is_debug", true);


			if (is_debug) {
				debugstream_m = new FileOutputStream(HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getLogDirectory() +
								File.separatorChar + "HtHTTPEventPropagator_"+Uid.generateNewStringUid()+".log", true);

			}

			writeDebugEntry("Request received: "+ req.getQueryString());

			eventTypes_m = getIntListParameter(req, "event_types", false);



			initialize(user, hashed_password, userIp, res);

			// read history and init
			for (int i = 0; i < eventTypes_m.size(); i++) {
				if (eventTypes_m.get(i) == XmlEvent.ET_CommonLog) {
					CommonLogParameters clp = new CommonLogParameters();

					clp.contextFilter = getStringParameter(req, "context_filter", true);
					clp.contentSmallFilter = getStringParameter(req, "content_filter", true);
					clp.threadFilter = getStringParameter(req, "thread_filter", true);
					clp.level = (int) getIntParameter(req, "log_level", true);
					clp.begin_date_hist = getIntParameter(req, "begin_date_hist", false);
					clp.end_date_hist = getIntParameter(req, "end_date_hist", false);

					clp.initPatterns();

					// 
					eventFilterParameters_m.put(XmlEvent.ET_CommonLog, clp);

				} else if (eventTypes_m.get(i) == XmlEvent.ET_RtProviderTicker) {

					RtDataParameters clp = new RtDataParameters();

					clp.do_read_history = this.getBooleanParameter(req, "do_read_history", false);
					if (clp.do_read_history) {
						// read other parameters
						clp.begin_date_hist = getIntParameter(req, "begin_date_hist", false);
						clp.end_date_hist = getIntParameter(req, "end_date_hist", false);
						clp.profile_name_hist = this.getStringParameter(req, "profile_name_hist", false);
					}

					// non obligatory
					List<String> allowed_symbols = getStringListParameter(req, "allowed_symbols", true);
					if (allowed_symbols.size() > 0) {
						clp.allowed_symbols_m = new HashSet<String>();
						clp.allowed_symbols_m.addAll(allowed_symbols);
					}

					eventFilterParameters_m.put(XmlEvent.ET_RtProviderTicker, clp);
				} else if (eventTypes_m.get(i) == XmlEvent.ET_DrawableObject) {
					DrawableObjectParameters clp = new DrawableObjectParameters();

					List<Integer> allowed_do_types = getIntListParameter(req, "allowed_do_types", false);
					clp.allowed_type.addAll(allowed_do_types);
					clp.do_read_history = getBooleanParameter(req, "do_read_history", false);

					// non obligatory
					List<String> allowed_symbols = getStringListParameter(req, "allowed_symbols", true);
					if (allowed_symbols.size() > 0) {
						clp.allowed_symbols_m = new HashSet<String>();
						clp.allowed_symbols_m.addAll(allowed_symbols);
					}

					clp.textFilter = getStringParameter(req, "text_filter", true);
					clp.shortTextFilter = getStringParameter(req, "short_text_filter", true);

					if (clp.do_read_history) {
						clp.begin_date_hist = getIntParameter(req, "begin_date_hist", false);
						clp.end_date_hist = getIntParameter(req, "end_date_hist", false);
						clp.profile_name_hist = this.getStringParameter(req, "profile_name_hist", false);
						clp.initPatterns();
					}



					eventFilterParameters_m.put(XmlEvent.ET_DrawableObject, clp);

				}


				readHistoryFromDBAndSend(res, eventTypes_m.get(i));
			}

			sendFinishReadHistoryFlag(res, ostrm);

			List list = new ArrayList();
			XmlEvent event = null;

			while (true) {

				queue_m.get(list, IDLE_TIME_SEC * 1000);

				for (Iterator it = list.iterator(); it.hasNext();) {

					event = (XmlEvent) it.next();

					if (event != null) {

						sendSingleEvent(res, event, ostrm);

						event = null;

					} else {

						sendHeartBeat(res, ostrm);
					}

				}

				if (list.isEmpty()) {
					sendHeartBeat(res, ostrm);
				}

			}


		} catch (Throwable e) {
			HtLog.log(Level.WARNING, getContext(), "initialize_Get", "Exception in XML Event propagation servlet: \"" + e.getMessage() + "\"");


			sendError("Exception in XML Event propagation servlet: \"" + e.getMessage() + "\"", res);

		}

		deinitialize();

		// close stream
		try {
			// finish chunked encoded
			ostrm.print("0\r\n\r\n");
			ostrm.close();
		} catch (Throwable e) {
		}

		writeDebugEntry("Processing finished");
		try {
			if (debugstream_m != null) {
				debugstream_m.flush();
				debugstream_m.close();
			}
		} catch (Throwable e2) {
		}


		HtLog.log(Level.INFO, getContext(), "initialize_Get", "Finished XML Event propagation servlet");
		return true;
	}

	// ------------------------------
	private void initialize(String user, String hashed_password, String userIp, HttpServletResponse res) throws Exception {


		sessionId_m = HtCommandProcessor.instance().get_HtSecurityProxy().remote_startUserSessionWithHashedPassword(user, hashed_password, userIp);

		if (sessionId_m == null) {
			throw new HtException(getContext(), "initialize", "Invalid user or password");
		}


		for (int i = 0; i < eventTypes_m.size(); i++) {
			RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).
							subscribeForEvent(eventTypes_m.get(i), 30, this);
		}

		// push event
		XmlEvent syncevent = new XmlEvent();
		syncevent.setEventType(XmlEvent.ET_SynchronizationEvent);
		RtGlobalEventManager.instance().pushCommonEvent(syncevent);


		// here we need to wait until the pending event will come through database layer to make sure
		// that we can select all data
		HtCommandProcessor.instance().get_HtDatabaseProxy().remote_waitDatabaseManagerWritePendingEvents(syncevent.getSequenceNumber(), WAIT_DB_PEND_EVENTS_SEC);

	}

	private void readHistoryFromDBAndSend(
					final HttpServletResponse res,
					int eventType) throws Exception {

		HtXmlEventExchangePacket.Packet packet = new HtXmlEventExchangePacket.Packet();
		final HtXmlEventExchangePacket.DataListSerializator serializator = new HtXmlEventExchangePacket.DataListSerializator(packet);
		serializator.initizalize();


		final ServletOutputStream ostrm = res.getOutputStream();
		final HtHTTPEventPropagator pThis = this;



		// so far only XmlEvent.
		if (eventType == XmlEvent.ET_RtProviderTicker) {

			HtDataSelection dataselector = new HtDataSelection() {

				@Override
				public void onSelectionFinish(long cnt) throws Exception {

					if (cnt > 0) {
						serializator.finishSerialization(HtXmlEventExchangePacket.PACKET_LIST_RTDATA);

						HtXmlEventExchangePacket.writePacketToStream(serializator.getPacket(), ostrm, ll_m);


						// initialize again
						serializator.initizalize();

					}

				}

				

				@Override
				public void newRowArrived(long ttime, String tsymbol, String tprovider, int ttype, int aflag, double tbid, double task, double topen, double thigh, double tlow, double tclose, double topen2, double thigh2, double tlow2, double tclose2, double tvolume, long tid, int color, int operation) throws Exception {
					HtRtData rtdata = new HtRtData(ttime, tsymbol, tprovider, ttype, aflag, tbid, task, topen, thigh, tlow, tclose, topen2, thigh2, tlow2, tclose2, tvolume, tid, color, operation);
					serializator.addNewEntry(HtRtData.serializeToBinary(rtdata));

				}
			};

			RtDataParameters clp = (RtDataParameters) eventFilterParameters_m.get(XmlEvent.ET_RtProviderTicker);
			if (clp == null) {
				throw new HtException(getContext(), "readHistoryFromDBAndSend", "Specific parameters for XmlEvent.ET_RtProviderTicker are not registered");
			}

			if (clp.do_read_history) {

				// now ready to read initial history
				HtCommandProcessor.instance().get_HtDatabaseProxy().remote_showDataAll_Callback_NoPaging(
								clp.profile_name_hist,
								new ArrayList<HtPair<String, String>>(),
								clp.begin_date_hist,
								clp.end_date_hist,
								dataselector,
								HISTORY_ROWS_CHUNK,
								true
						);
			} else {
				HtLog.log(Level.INFO, getContext(), "readHistoryFromDBAndSend", "User seleted not to read XmlEvent.ET_RtProviderTicker from DB");
			}

			
		} else if (eventType == XmlEvent.ET_DrawableObject) {
			DrawableObjectParameters clp = (DrawableObjectParameters) eventFilterParameters_m.get(XmlEvent.ET_DrawableObject);
			if (clp == null) {
				throw new HtException(getContext(), "readHistoryFromDBAndSend", "Specific parameters for XmlEvent.ET_DrawableObject are not registered");
			}

			HtDrawableObjectSelector drawableselector = new HtDrawableObjectSelector() {

				long cnt_m = 0;

				@Override
				public void onDrawableObjectArrived(HtDrawableObject dobj) throws Exception {
					XmlEvent event = new XmlEvent();
					HtDrawableObject.createXmlEventDrawableObject(dobj, event);

					if ( pThis.debugstream_m != null ) {
							pThis.writeDebugEntry("Creating history drawable object entry: " + dobj.toSmallDebugString());
						}

					// serialize

					serializator.addNewEntry(eventToBytes(event));

					if ((++cnt_m % HISTORY_ROWS_CHUNK) == 0) {
						// purge

						serializator.finishSerialization(HtXmlEventExchangePacket.PACKET_LIST_EVENTS);

						HtXmlEventExchangePacket.writePacketToStream(serializator.getPacket(), ostrm, ll_m);

						if ( pThis.debugstream_m != null ) {
							pThis.writeDebugEntry("Sending drawable history chunk of the size: " + serializator.getEntryCount());
						}



						// initialize again
						serializator.initizalize();

					}

				}

				@Override
				public void onFinish(long cntr) throws Exception {
					if (cntr > 0) {
						serializator.finishSerialization(HtXmlEventExchangePacket.PACKET_LIST_EVENTS);

						HtXmlEventExchangePacket.writePacketToStream(serializator.getPacket(), ostrm, ll_m);

						if ( pThis.debugstream_m != null ) {
							pThis.writeDebugEntry("Sending drawable history (finish) chunk of the size: " + serializator.getEntryCount());
						}


						// initialize again
						serializator.initizalize();
					}
				}
			};

			if (clp.do_read_history) {

				ArrayList<Integer> allowedTypes = new ArrayList<Integer>(clp.allowed_type);
				if (allowedTypes.size() == 1 && allowedTypes.get(0) == HtDrawableObject.DO_TEXT) {
					// for fast performance this serves text type only
					HtCommandProcessor.instance().get_HtDatabaseProxy().remote_showDrawableObjects_Callback_DO_TEXT(
								clp.profile_name_hist,
								new ArrayList<HtPair<String, String>>(),
								true,
								clp.begin_date_hist,
								clp.end_date_hist,
								clp.shortTextFilter,
								clp.textFilter,
								null,
								null,
								drawableselector,
								-1,
								-1);
				}
				else {

					// these servers all types
					HtCommandProcessor.instance().get_HtDatabaseProxy().remote_showDrawableObjects_Callback(
								clp.profile_name_hist,
								new ArrayList<HtPair<String, String>>(),
								true,
								clp.begin_date_hist,
								clp.end_date_hist,
								allowedTypes,
								clp.shortTextFilter,
								clp.textFilter,
								null,
								null,
								drawableselector,
								-1,
								-1);
				}



			} else {
				HtLog.log(Level.INFO, getContext(), "readHistoryFromDBAndSend", "User seleted not to read XmlEvent.ET_DrawableObject from DB");
			}



		} else if (eventType == XmlEvent.ET_CommonLog) {


			HtCommonLogSelection datalogselector = new HtCommonLogSelection() {

				long cnt_m = 0;

				@Override
				public void newRowArrived(CommonLog commonLogRow) throws Exception {

					//HtLog.log(Level.INFO, this.getClass().getSimpleName(), "newRowArrived", commonLogRow.getContent());

					XmlEvent event = new XmlEvent();
					CommonLog.createXmlEventCommonLog(commonLogRow, event);

					if ( pThis.debugstream_m != null ) {
						pThis.writeDebugEntry("Creating history log entry:" + commonLogRow.toSmallDebugString());
					}

					serializator.addNewEntry(eventToBytes(event));

					if ((++cnt_m % HISTORY_ROWS_CHUNK) == 0) {
						// purge

						serializator.finishSerialization(HtXmlEventExchangePacket.PACKET_LIST_EVENTS);

						HtXmlEventExchangePacket.writePacketToStream(serializator.getPacket(), ostrm, ll_m);


						if ( pThis.debugstream_m != null ) {
							pThis.writeDebugEntry("Sending log history chunk of the size: " + serializator.getEntryCount());
						}

						// initialize again
						serializator.initizalize();

						

					}

				}

				@Override
				public void onFinish(long cntr) throws Exception {
					// purge the remainder
					if (cntr > 0) {
						serializator.finishSerialization(HtXmlEventExchangePacket.PACKET_LIST_EVENTS);

						HtXmlEventExchangePacket.writePacketToStream(serializator.getPacket(), ostrm, ll_m);

						if ( pThis.debugstream_m != null ) {
							pThis.writeDebugEntry("Sending log history (finish) chunk of the size: " + serializator.getEntryCount());
						}


						// initialize again
						serializator.initizalize();

						
					}

				}
			};

			CommonLogParameters clp = (CommonLogParameters) eventFilterParameters_m.get(XmlEvent.ET_CommonLog);
			if (clp == null) {
				throw new HtException(getContext(), "readHistoryFromDBAndSend", "Specific parameters for XmlEvent.ET_CommonLog are not registered");
			}

			// do not filter
			HtCommandProcessor.instance().get_HtDatabaseProxy().remote_getCommonLogDataPage_Callback(
							clp.contextFilter,
							clp.contentSmallFilter,
							clp.threadFilter,
							clp.level,
							clp.begin_date_hist,
							clp.end_date_hist,
							null,
							datalogselector,
							-1,
							-1,
							-1);
		} else {
			HtLog.log(Level.INFO, getContext(), "readHistoryFromDBAndSend", "This type of event cannot be read from DB");
		}

		// clean up everything
		serializator.discardSerialization();

	}

	private void deinitialize() {

		// uninitialize
		try {
			RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).unsubscribeForAllEvents(30, this);
		} catch (Throwable e) {
			HtLog.log(Level.WARNING, getContext(), "deinitialize", "Error on unsubscribing: \"" + e.getMessage() + "\"");
		}

		try {
			if (sessionId_m != null) {
				HtCommandProcessor.instance().get_HtSecurityProxy().remote_stopUserSession(sessionId_m);
			}
		} catch (Throwable e) {
			HtLog.log(Level.WARNING, getContext(), "deinitialize", "Error on stop session: \"" + e.getMessage() + "\"");
		}

		queue_m.releaseMonitoring();

	}

	public void onEventArrived(XmlEvent event) throws SingleReaderQueueExceedLimit {

		queue_m.put(event);

	}

	public String getListenerName() {
		return " listener: [ " + getContext() + " ] ";
	}

	// ---------------------------
	// helpers
	private void sendSingleEvent(HttpServletResponse res, XmlEvent event, final ServletOutputStream ostrm) throws Exception {

		// need to filter out
		int evnetType = event.getEventType();
		if (evnetType == XmlEvent.ET_CommonLog) {
			CommonLogParameters clp = (CommonLogParameters) eventFilterParameters_m.get(XmlEvent.ET_CommonLog);

			CommonLog clog = event.getAsLog();

			// filter

			if (clp.end_date_hist > 0 && clog.getLogDate() > clp.end_date_hist) {
				return;
			}

			if (clp.begin_date_hist > 0 && clog.getLogDate() < clp.begin_date_hist) {
				return;
			}

			if (clog.getLogLevel() > clp.level) {
				return;
			}

			if (clp.threadFilter != null && clp.threadFilter.length() > 0) {
				Matcher mtch = clp.p_threadFilter.matcher(String.valueOf(clog.getLogThread()));
				if (!mtch.find()) {
					return;
				}
			}

			//
			if (clp.contextFilter != null && clp.contextFilter.length() > 0) {
				Matcher mtch = clp.p_contextFilter.matcher(String.valueOf(clog.getContext()));
				if (!mtch.find()) {
					return;
				}
			}

			if (clp.contentSmallFilter != null && clp.contentSmallFilter.length() > 0) {
				Matcher mtch = clp.p_contentSmallFilter.matcher(String.valueOf(clog.getContent()));
				if (!mtch.find()) {
					return;
				}
			}

			if ( debugstream_m != null ) {
						writeDebugEntry("Sending log event: " + clog.toSmallDebugString());
			}

		} else if (evnetType == XmlEvent.ET_DrawableObject) {
			DrawableObjectParameters clp = (DrawableObjectParameters) eventFilterParameters_m.get(XmlEvent.ET_DrawableObject);

			HtDrawableObject dobj = event.getAsDrawableObject();

			if (clp.end_date_hist > 0 && dobj.getTime() > clp.end_date_hist) {
				return;
			}

			if (clp.begin_date_hist > 0 && dobj.getTime() < clp.begin_date_hist) {
				return;
			}

			// filter out by type
			if (clp.allowed_type.size() >0 && !clp.allowed_type.contains(dobj.getType())) {
				return;
			}


			if (clp.allowed_symbols_m != null) {
				if (!clp.allowed_symbols_m.contains(dobj.getSymbol())) {
					return;
				}
			}

			if (dobj.getType() == HtDrawableObject.DO_TEXT) {

				if (clp.shortTextFilter != null && clp.shortTextFilter.length() > 0) {
					Matcher mtch = clp.p_shortTextFilter.matcher(dobj.getAsText().shortText_m.toString());
					if (!mtch.find()) {
						return;
					}
				}

				if (clp.textFilter != null && clp.textFilter.length() > 0) {
					Matcher mtch = clp.p_textFilter.matcher(dobj.getAsText().text_m.toString());
					if (!mtch.find()) {
						return;
					}
				}

			}

		} else if (evnetType == XmlEvent.ET_RtProviderTicker) {
			RtDataParameters clp = (RtDataParameters) eventFilterParameters_m.get(XmlEvent.ET_RtProviderTicker);

			if (clp.allowed_symbols_m != null) {
				if (!clp.allowed_symbols_m.contains(event.getAsRtData().getSymbol())) {
					return;
				}
			}

		}

		HtXmlEventExchangePacket.Packet packet = new HtXmlEventExchangePacket.Packet();
		HtXmlEventExchangePacket.serializeSingleEvent(handler_m, event, packet);

		HtXmlEventExchangePacket.writePacketToStream(packet, ostrm, ll_m);

	}

	private void sendFinishReadHistoryFlag(HttpServletResponse res, final ServletOutputStream ostrm) throws Exception {

		HtXmlEventExchangePacket.Packet packet = new HtXmlEventExchangePacket.Packet(HtXmlEventExchangePacket.PACKET_FINISH_HISTORY);


		HtXmlEventExchangePacket.writePacketToStream(packet, ostrm, ll_m);



	}

	private void sendHeartBeat(HttpServletResponse res, final ServletOutputStream ostrm) throws Exception {

		HtXmlEventExchangePacket.Packet packet = new HtXmlEventExchangePacket.Packet(HtXmlEventExchangePacket.PACKET_HEARTBEAT);
		HtXmlEventExchangePacket.writePacketToStream(packet, ostrm, ll_m);

		//XmlEvent.createSimpleLog("", this.getContext()+" - " + Thread.currentThread().getId(), CommonLog.LL_DEBUGDETAIL, "Heartbeat arrived");

	}

	private void sendError(String message, HttpServletResponse res) {
		try {

			HtXmlEventExchangePacket.Packet packet = new HtXmlEventExchangePacket.Packet();
			HtXmlEventExchangePacket.serializeAsErrorMessage(message, packet);
			final ServletOutputStream ostrm = res.getOutputStream();

			HtXmlEventExchangePacket.writePacketToStream(packet, ostrm, ll_m);


		} catch (Throwable e) {
			HtLog.log(Level.WARNING, getContext(), "sendError", "Exception in sending error: \"" + e.getMessage() + "\"");

		}
	}

	private byte[] eventToBytes(XmlEvent event) throws Exception {
		XmlParameter xmlparameter = new XmlParameter();
		XmlEvent.convertEventToXmlParameter(event, xmlparameter);

		StringBuilder out = new StringBuilder();
		handler_m.serializeParameter(xmlparameter, out);


		return out.toString().getBytes("UTF-8");
	}

	private void writeDebugEntry(String message)
	{
		try {
			if ( debugstream_m != null ) {
				StringBuilder out = new StringBuilder();
				out.append("[ ");
				out.append(HtDateTimeUtils.time2String_Gmt(HtDateTimeUtils.getCurGmtTime()));
				out.append(" ] - ( ");
				out.append(Thread.currentThread().getId());
				out.append(" ) - ");
				out.append(message);
				out.append("\r\n");

				debugstream_m.write(out.toString().getBytes());
				debugstream_m.flush();
			}
		}
		catch(Throwable e)
		{
			HtLog.log(Level.WARNING, getContext(), "writeDebugEntry", "Exception in write debug entry: \"" + e.getMessage() + "\"");
		}
	}
}
