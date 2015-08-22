/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package htrangebreaker.utils;

import com.fin.httrader.utils.LongParam;
import com.fin.httrader.utils.hlpstruct.HtDrawableObject;
import com.fin.httrader.utils.hlpstruct.HtXmlEventExchangePacket;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import com.fin.httrader.utils.hlpstruct.XmlHandler;

import com.fin.httrader.utils.hlpstruct.XmlParameter;
import com.fin.httrader.webserver.HtWebUtils;
import java.io.BufferedInputStream;
import java.io.InputStream;
import java.util.List;
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
 * this class receives drawable_objects events - related to trading
 */
public class TradeEventReceiver extends Thread {

	public static final String ALL_SYMBOL_ID = "-ALL-";
	private StringBuilder url_m = new StringBuilder();
	private Event shutdown_m = new Event();
	private CommunicationInterface commInterface_m = null;
	private HttpGet httpget_m = null;
	private Object httpgetMtx_m = new Object();
	// -------------


	static {
		System.getProperties().put("HTTPClient.dontChunkRequests", "false");
	}

	public String getContext() {
		return TradeEventReceiver.class.getSimpleName();
	}



	public TradeEventReceiver(
					HtSettingsStruct ss,
					CommunicationInterface commInterface,
					long begin_date,
					long end_date,
					String profile_name_hist,
					String textFilter,
					String shortTextFilter,
					Integer[] eventTypes,
					String[] allowed_symbols,
					boolean isServletDebugMode
	) {
		 
		commInterface_m = commInterface;

		url_m.append("http://").append(ss.host).append(":").append(ss.port).append("/htHTTPEventPropagator.jsp");

		url_m.append("?user=").append(ss.user);
		url_m.append("&hashed_password=").append(ss.hashed_password);
		if (isServletDebugMode) {
			url_m.append("&is_debug=true" );
		}

		url_m.append("&begin_date_hist=").append(begin_date);
		url_m.append("&end_date_hist=").append(end_date);


		url_m.append("&event_types=").append(HtWebUtils.<Integer>createURLListParameterHelper( eventTypes ));

		url_m.append("&allowed_symbols=").append(HtWebUtils.<String>createURLListParameterHelper(allowed_symbols));
		url_m.append("&allowed_do_types=").append(HtWebUtils.<Integer>createURLListParameterHelper(	new Integer[]{HtDrawableObject.DO_TEXT}));

		
		url_m.append("&short_text_filter=").append(HtWebUtils.encodeURIComponent(shortTextFilter != null ? shortTextFilter: "" ));
		url_m.append("&text_filter=").append(HtWebUtils.encodeURIComponent(textFilter != null ? textFilter: ""));

		if (profile_name_hist != null) {
			url_m.append("&profile_name_hist=").append(profile_name_hist);
			url_m.append("&do_read_history=true");
		}
		else {

			url_m.append("&do_read_history=false");
		}

		start();
	}

	@Override
	public void run() {

		try {

			final XmlHandler handler = new XmlHandler();

			HttpClient httpclient = new DefaultHttpClient();
			HttpResponse response = null;
			synchronized (httpgetMtx_m) {
				httpget_m = new HttpGet(url_m.toString());

				Utils.initHeaders(httpget_m);

				response = httpclient.execute(httpget_m);
			}

			HtFrameLogger.log("Sending trade event request: " + url_m.toString());

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
				commInterface_m.onStartLengthyOperation("Reading trade events history...");
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
						
						public void onByteChunkArrived(byte[] data) throws Exception {

							if (shutdown_m.waitEvent(0)) {
								throw new HtFrameException("Trade events transfer was interrupted");
							}

							XmlEvent event_i = new XmlEvent();

							XmlParameter xmlparameter = new XmlParameter();
							handler.deserializeParameter(new String(data, "UTF-8"), xmlparameter);

							XmlEvent.convertXmlParameterToEvent(event_i, xmlparameter);

							cnt.increment();

							if (commInterface_m != null) {
								if (event_i.getEventType() == XmlEvent.ET_DrawableObject) {
									commInterface_m.onDrawableObjectReceived(isHistoryf, event_i.getAsDrawableObject());
								} else if (event_i.getEventType() == XmlEvent.ET_RtProviderTicker) {
									commInterface_m.onHtRtDataReceived(isHistoryf, event_i.getAsRtData());
								}
							}
							
						}
					};


					HtXmlEventExchangePacket.DataListSerializator.deserializeList(rtdatalistener, packet);
					HtFrameLogger.log("Got the list of trade events of size: " + cnt.getLong() +" from raw packet of size: " + (packet.data_m != null ? packet.data_m.length:0));

		
				}	else if (packet.packet_type_m == HtXmlEventExchangePacket.PACKET_SINGLE_EVENT) {

					XmlEvent event_i = HtXmlEventExchangePacket.deserializeSingleEvent(handler, packet);
					if (commInterface_m != null) {
						if (event_i.getEventType() == XmlEvent.ET_DrawableObject) {
							commInterface_m.onDrawableObjectReceived(isHistory, event_i.getAsDrawableObject());
						} else if (event_i.getEventType() == XmlEvent.ET_RtProviderTicker) {
							commInterface_m.onHtRtDataReceived(isHistory, event_i.getAsRtData());
						}
					}

				} else if (packet.packet_type_m == HtXmlEventExchangePacket.PACKET_FINISH_HISTORY) {

					isHistory = false;
					HtFrameLogger.log("Finsihed events history");
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

		synchronized (httpgetMtx_m) {
			httpget_m = null;
		}

		if (commInterface_m != null) {
			commInterface_m.onStopCommunicationChannel();
			commInterface_m.onStopLengthyOperation();
		}




	}

	public void shutdown() {
		HtFrameLogger.log("Shutting down trade events channel");
		
		try {
			synchronized (httpgetMtx_m) {
				httpget_m.abort();
			}
		} catch (Throwable e) {
		}

		shutdown_m.signal();


		try {


			join();
		} catch (InterruptedException e) {
		}

		
	}
}
