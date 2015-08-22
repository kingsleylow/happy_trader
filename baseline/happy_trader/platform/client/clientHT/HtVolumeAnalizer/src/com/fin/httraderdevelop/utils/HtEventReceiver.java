/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httraderdevelop.utils;

import com.fin.httrader.eventplugins.volanalizer.HtVolumeAnalizer;
import com.fin.httrader.managers.RtRealTimeProviderManager;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.hlpstruct.HtRtData;
import com.fin.httrader.utils.hlpstruct.HtXmlEventExchangePacket;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import com.fin.httrader.utils.hlpstruct.XmlHandler;
import com.fin.httraderdevelop.HtVolumeAnalizerView;
import java.io.BufferedInputStream;
import java.io.InputStream;
import java.util.logging.Level;
import jsync.Event;
import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.HttpStatus;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;


/**
 *
 * @author DanilinS
 * This is receiver of events
 */
public class HtEventReceiver extends Thread {

	private String url_m = null;
	private String proxy_m = null;
	private Event shutdown_m = new Event();
	private HtVolumeAnalizer plugin_m = null;
	private String user_m = null;
	private String password_m = null;
	private Integer[] subscribe_event_types_m = null;
	private HtVolumeAnalizerView view_m = null;
	HttpGet httpget_m = null;
	Object httpgetMtx_m = new Object();

	public String getContext() {
		return HtEventReceiver.class.getSimpleName();
	}

	public HtEventReceiver(
					HtVolumeAnalizerView view,
					String url,
					String proxy,
					HtVolumeAnalizer plugin,
					String user,
					String password,
					int history_load_days,
					String profile_name_hist) throws Exception {
		url_m = url + "?user=" + user + "&hashed_password=" + password;
		proxy_m = proxy;
		user_m = user;
		password_m = password;
		plugin_m = plugin;
		view_m = view;

		System.getProperties().put("HTTPClient.dontChunkRequests", "false");

		long begin_date_hist = HtDateTimeUtils.getCurGmtTime_DayBegin() - (long) history_load_days * 24 * 60 * 60 * 1000;
		long end_date_hist = -1;

		url_m += "&do_read_history=true";
		url_m += "&begin_date_hist=" + begin_date_hist + "&end_date_hist=" + end_date_hist;
		url_m += "&profile_name_hist=" + profile_name_hist;
		
		subscribe_event_types_m = plugin_m.returnSubscribtionEventTypes();
		if (subscribe_event_types_m != null) {
			url_m += "&event_types=";
			for (int i = 0; i < subscribe_event_types_m.length; i++) {
				url_m += subscribe_event_types_m[i] + ",";
			}
		}

		start();
	}

	public void run() {
		try {



			view_m.startCommunicationChannel();

			XmlHandler handler = new XmlHandler();

			HttpClient httpclient = new DefaultHttpClient();
			HttpResponse response = null;
			synchronized(httpgetMtx_m) {
				httpget_m = new HttpGet(url_m);

				httpget_m.setHeader("Accept-Language", "ru,ja;q=0.5");
				httpget_m.setHeader("Accept-Encoding", "deflate");
				httpget_m.setHeader("User-Agent", "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 1.1.4322; InfoPath.1; .NET CLR 2.0.50727; .NET CLR 3.0.04506.30; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022)");
		
				response = httpclient.execute(httpget_m);
			}

			int statusCode = response.getStatusLine().getStatusCode();
			if (statusCode != HttpStatus.SC_OK) {
				throw new HtFrameException("Cannot read data from the web service");
			}

			HttpEntity entity = response.getEntity();
			if (entity == null) {
				throw new HtFrameException("Cannot read data from the web service");
			}


			InputStream is = entity.getContent();
			boolean is_chanked = entity.isChunked();


			BufferedInputStream isb = new BufferedInputStream(is);

			int cnt = 0;
			boolean history_processing = true;
			HtXmlEventExchangePacket.Packet packet = new HtXmlEventExchangePacket.Packet();

			HtFrameLogger.log(Level.INFO, getContext(), "run", "History read start");
			plugin_m.disableTickTab();
			view_m.startReadingHistory();

			while (true) {

				if (shutdown_m.waitEvent(0)) {
					break;
				}

				HtXmlEventExchangePacket.readPacketFromStream(packet, isb);
				
				if (packet.packet_type_m == HtXmlEventExchangePacket.PACKET_HEARTBEAT) {
					HtFrameLogger.log(Level.INFO, getContext(), "run", "Heart beat");
				} else if (packet.packet_type_m == HtXmlEventExchangePacket.PACKET_ERROR) {
					String errMessage = HtXmlEventExchangePacket.deserializeAsErrorMessage(packet);
					HtFrameLogger.log(Level.WARNING, getContext(), "run", "Error happened: " + errMessage);
					break;

				} else if (packet.packet_type_m == HtXmlEventExchangePacket.PACKET_LIST_RTDATA) {

					HtXmlEventExchangePacket.ByteChunkListener rtdatalistener = new HtXmlEventExchangePacket.ByteChunkListener()
					{
						
						public void onByteChunkArrived(byte[] data) throws Exception {
							XmlEvent event_i = new XmlEvent();
							
							RtRealTimeProviderManager.createXmlEventRtData(HtRtData.deserializeFromBinary(data), event_i);
							passEvent(event_i);
						}
					};

					HtXmlEventExchangePacket.DataListSerializator.deserializeList(rtdatalistener, packet);
									
				} else if (packet.packet_type_m == HtXmlEventExchangePacket.PACKET_SINGLE_EVENT) {

					XmlEvent alertData = HtXmlEventExchangePacket.deserializeSingleEvent(handler, packet);
					passEvent(alertData);

				} else if (packet.packet_type_m == HtXmlEventExchangePacket.PACKET_FINISH_HISTORY) {

					HtFrameLogger.log(Level.INFO, getContext(), "run", "History read finish");
					history_processing = false;
					plugin_m.enableTickTab();
					view_m.stopReadingHistory();

				}


			}
		} catch (Throwable e) {
			HtFrameLogger.log(Level.WARNING, getContext(), "run", "Exception: " + e.getMessage());
		}

		synchronized(httpgetMtx_m) {
			httpget_m = null;
		}

		view_m.stopCommunicationChannel();
		HtFrameLogger.log(Level.WARNING, getContext(), "run", "Finished communication channel");
	}

	public void shutdown() {

		try {
			synchronized(httpgetMtx_m) {
				httpget_m.abort();
			}
		}
		catch(Throwable e)
		{
		}

		shutdown_m.signal();

		try {
	
			join();
		} catch (InterruptedException e) {
		}
	}

	/*
	 * Helpers
	 */
	private void passEvent(XmlEvent event) {
		//if ( HtUtils.findInArray(subscribe_event_types_m, Integer.valueOf(event.getEventType())) == null )
		//	return;

		try {
			plugin_m.execute(event);
		} catch (Throwable e2) {
			HtFrameLogger.log(Level.WARNING, getContext(), "run", "Exception on processing event: " + e2.getMessage());
		}
	}
}
