/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package htrangebreaker.utils;


import com.fin.httrader.managers.RtRealTimeProviderManager;
import com.fin.httrader.utils.HtMathUtils;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.LongParam;
import com.fin.httrader.utils.hlpstruct.HtRtData;
import com.fin.httrader.utils.hlpstruct.HtXmlEventExchangePacket;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import com.fin.httrader.utils.hlpstruct.XmlHandler;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import com.fin.httrader.webserver.HtHTTPEventPropagator;

import com.fin.httrader.webserver.HtWebUtils;
import java.io.BufferedInputStream;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.logging.Level;
import jsync.Event;
import org.apache.commons.codec.net.URLCodec;
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
public class HtCommonEventReceiver extends Thread {

	private StringBuilder url_m = new StringBuilder();
	private String proxy_m = null;
	private Event shutdown_m = new Event();
	
	private CommunicationInterface commInterface_m = null;
	private XmlHandler handler_m = new XmlHandler();

	private HttpGet httpget_m = null;
	private Object httpgetMtx_m = new Object();
	

	static {
		System.getProperties().put("HTTPClient.dontChunkRequests", "false");
	}

	public String getContext() {
		return HtCommonEventReceiver.class.getSimpleName();
	}

	public HtCommonEventReceiver(
					String host,
					int port,
					String user,
					String hashed_password,
					long begin_date,
					long end_date,
					String contextFilter,
					String contentSmallFilter,
					String threadFilter,
					int level,
					boolean isServletDebugMode,
					CommunicationInterface commInterface
	) throws Exception {
	
		commInterface_m = commInterface;
	
		url_m.append("http://").append(host).append(":").append(port).append( "/htHTTPEventPropagator.jsp");

		url_m.append("?user=").append(user);
		url_m.append("&hashed_password=").append(hashed_password);

		if (isServletDebugMode) {
			url_m.append("&is_debug=true" );
		}
		
		url_m.append("&begin_date_hist=").append(begin_date);
		url_m.append("&end_date_hist=").append(end_date);

		url_m.append("&event_types=").append(HtWebUtils.<Integer>createURLListParameterHelper( new Integer[]{XmlEvent.ET_CommonLog}));

		
		url_m.append("&context_filter=").append(HtWebUtils.encodeURIComponent( contextFilter ));
		url_m.append("&content_filter=").append(HtWebUtils.encodeURIComponent(contentSmallFilter));
		url_m.append("&thread_filter=").append(HtWebUtils.encodeURIComponent(threadFilter));
		url_m.append("&log_level=").append(level);

		start();
	}

	@Override
	public void run() {

	
		
		try {

			XmlHandler handler = new XmlHandler();

			HttpClient httpclient = new DefaultHttpClient();
			HttpResponse response = null;

			synchronized(httpgetMtx_m) {
				httpget_m = new HttpGet(url_m.toString());
				Utils.initHeaders(httpget_m);
				response = httpclient.execute(httpget_m);

			}

			HtFrameLogger.log("Sending log request: " + url_m.toString());

			int statusCode = response.getStatusLine().getStatusCode();
			if (statusCode != HttpStatus.SC_OK) {
				throw new HtFrameException("Cannot read data from the web service");
			}

			if (commInterface_m != null) {
				commInterface_m.onStartCommunicationChannel();
			}

			HttpEntity entity = response.getEntity();
			if (entity == null) {
				throw new HtFrameException("Cannot read data from the web service");
			}


			InputStream is = entity.getContent();
			BufferedInputStream isb = new BufferedInputStream(is);
			HtXmlEventExchangePacket.Packet packet = new HtXmlEventExchangePacket.Packet();
			

			if (commInterface_m != null) {
				commInterface_m.onStartLengthyOperation("Reading log history...");
			}

			boolean isHistory = true;

			while (true) {

				if (shutdown_m.waitEvent(0)) {
					break;
				}

				HtXmlEventExchangePacket.readPacketFromStream(packet, isb);
				
				if (packet.packet_type_m == HtXmlEventExchangePacket.PACKET_HEARTBEAT) {
					if (commInterface_m != null) {
						commInterface_m.onHeartBeat();
					}


				} else if (packet.packet_type_m == HtXmlEventExchangePacket.PACKET_ERROR) {
					String errMessage = HtXmlEventExchangePacket.deserializeAsErrorMessage(packet);

					if (commInterface_m != null) {
						commInterface_m.onErrorOccured("Error happened on receiving data: " + errMessage);
					}
				
					break;

				} else if (packet.packet_type_m == HtXmlEventExchangePacket.PACKET_LIST_EVENTS) {

					final LongParam cnt = new LongParam();
					final boolean isHistoryf = isHistory;
					HtXmlEventExchangePacket.ByteChunkListener rtdatalistener = new HtXmlEventExchangePacket.ByteChunkListener()
					{
						
						@Override
						public void onByteChunkArrived(byte[] data) throws Exception {

							if (shutdown_m.waitEvent(0)) {
								throw new HtFrameException("Log transfer was interrupted");
							}
							
							XmlEvent event_i = new XmlEvent();

							XmlParameter xmlparameter = new XmlParameter();
							String datas = new String(data, "UTF-8");
							try {
								handler_m.deserializeParameter(datas, xmlparameter);
							}
							catch(Exception e)
							{
								HtFrameLogger.log("Exception happened on deserializing xml data: " + e.getMessage());
								throw e;
							}

							XmlEvent.convertXmlParameterToEvent(event_i, xmlparameter);
						
							cnt.increment();

							if (commInterface_m != null) {
								commInterface_m.onLogEntryReceived(isHistoryf, event_i.getAsLog());
							}
							
						}
					};


					HtXmlEventExchangePacket.DataListSerializator.deserializeList(rtdatalistener, packet);
					HtFrameLogger.log("Got the list of log events of size: " + cnt.getLong() +" from raw packet of size: " + (packet.data_m != null ? packet.data_m.length:0));
		
				} else if (packet.packet_type_m == HtXmlEventExchangePacket.PACKET_SINGLE_EVENT) {

					XmlEvent event_i = HtXmlEventExchangePacket.deserializeSingleEvent(handler, packet);
					if (commInterface_m != null) {
							commInterface_m.onLogEntryReceived(isHistory,event_i.getAsLog());
					}

				} else if (packet.packet_type_m == HtXmlEventExchangePacket.PACKET_FINISH_HISTORY) {


					isHistory  = false;
					HtFrameLogger.log("Finished log history");
					if (commInterface_m != null) {
						commInterface_m.onStopLengthyOperation();
					}

				}


			}
		} catch (Throwable e) {

			
			if (commInterface_m != null) {
				commInterface_m.onErrorOccured(e.getMessage());
			}
		}

		synchronized(httpgetMtx_m) {
			httpget_m = null;
		}

		if (commInterface_m != null) {
				commInterface_m.onStopCommunicationChannel();
				commInterface_m.onStopLengthyOperation();
		};
		
		
	}

	public void shutdown()
	{
		HtFrameLogger.log("Shutting down communication log channel");

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
	
}
