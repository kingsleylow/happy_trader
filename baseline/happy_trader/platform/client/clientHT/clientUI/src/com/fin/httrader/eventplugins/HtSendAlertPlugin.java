/*
 * HtSendAlertPlugin.java
 *
 * 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.eventplugins;

import com.fin.httrader.hlpstruct.BackgroundJobStatus;
import com.fin.httrader.interfaces.HtEventPlugin;
import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.URLUTF8Encoder;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import java.io.InputStream;
import java.util.HashMap;
import java.util.Map;
import org.apache.http.HttpEntity;
import org.apache.http.HttpHost;
import org.apache.http.HttpResponse;
import org.apache.http.HttpStatus;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.conn.params.ConnRoutePNames;
import org.apache.http.impl.client.DefaultHttpClient;




/**
 *
 * @author Victor_Zoubok
 */
public class HtSendAlertPlugin implements HtEventPlugin {

	
	private static final int MAX_SMS_LENGTH = 65;
	private static final String SENT_SMS_OK_MSG = "Ok";
	private static final int MAX_SMS_RETRIES = 10;
	private StringBuilder name_m = new StringBuilder();
	private HashMap<String, String> parameters_m = new HashMap<String, String>();

	private String getContext() {
		return getClass().getSimpleName();
	}

	// ------------------------------------------
	/** Creates a new instance of HtSendAlertPlugin */
	public HtSendAlertPlugin() {
		
	}

	public String getEventPluginId() {
		return name_m.toString();
	}

	public void setEventPluginId(String pluginId) {
		name_m.setLength(0);
		name_m.append(pluginId);
	}

	public void execute(XmlEvent alertData) throws Exception {
		String number = parameters_m.get("NUMBER");
		String data = alertData.getAsXmlParameter().getString("data");

		sendBeelineSmsLarge(data, number, SENT_SMS_OK_MSG, MAX_SMS_RETRIES);
	}

	public void initialize(Map<String, String> initdata) throws Exception {
		parameters_m.putAll(initdata);

		String number = parameters_m.get("NUMBER");
		sendBeelineSmsLarge("Plugin initialized", number, SENT_SMS_OK_MSG, MAX_SMS_RETRIES);

		XmlEvent.createSimpleLog("", getEventPluginId(), CommonLog.LL_INFO, "Plugin initiaized");
	}

	public void deinitialize() throws Exception {
		String number = parameters_m.get("NUMBER");

		sendBeelineSmsLarge("Plugin deinitialized", number, SENT_SMS_OK_MSG, MAX_SMS_RETRIES);

		XmlEvent.createSimpleLog("", getEventPluginId(), CommonLog.LL_INFO, "Plugin deinitialized");
	}

	// ----------------------------------------
	// this sends large message splitting it and log results
	private void sendBeelineSmsLarge(String message, String number, String sentOk, int maxRetries) throws Exception {
		StringBuilder mchunk = new StringBuilder();
		int bind = 0;
		int eind = 0;
		boolean exit = false;

		while (true) {
			mchunk.setLength(0);

			eind = bind + MAX_SMS_LENGTH - 3;
			if (eind > message.length()) {
				eind = message.length();
				exit = true;
			}


			mchunk.append(message.substring(bind, eind));
			bind = eind;

			String toSend = mchunk.toString().trim();

			if (!exit) {
				mchunk.append("...");
			}

			// loop to repeat sents
			int cnt = 1;
			while (true) {
				String result = sendBeelineSms(toSend, number);
				XmlEvent.createSimpleLog("", getEventPluginId(), CommonLog.LL_INFO, "Beeline sender returned: " + result +
								"\nphone number: \"" + number + "\"\ndata: \"" + mchunk + "\", retry #" + cnt);

				// will need to retry
				if (sentOk != null && result.indexOf(sentOk) >= 0) {
					// ok
					break;
				} else {

					// wait 3 secs
					HtUtils.Sleep(3.0);
				}

				if (++cnt > maxRetries) {
					break;
				}
			}

			if (exit) {
				break;
			}

			HtUtils.Sleep(2.0);
		}
	}

	// ---------------------------------------
	private String sendBeelineSms(String message, String number) throws Exception {
		StringBuilder out = new StringBuilder();

		StringBuilder url = new StringBuilder();
		//"/servlet/send/sms/?phone=[NUMBER]&number_sms=number_sms_send&prf=[NETWORK]&termtype=[DORG]&message=[MESSAGE]&translit=[USEUCS2]";
		StringBuilder body = new StringBuilder();

		if (number.length() != 11) {
			throw new HtException(getContext(), "sendBeelineSms", "Invalid number: " + number);
		}

		String purenumber = number.substring(4);
		String prefix = number.substring(0, 4);
		//String smessage = message.replaceAll("[\\s]", "+");
		String smessage = message;


		url.append("http://www.beeonline.ru/servlet/send/sms/?phone=");
		url.append(purenumber);
		url.append("&number_sms=number_sms_send");
		url.append("&prf=");
		url.append(prefix);
		url.append("&termtype=G");
		url.append("&message=");
		url.append(URLUTF8Encoder.encode(smessage));
		url.append("&translit=");

		//
		HttpClient httpclient = new DefaultHttpClient();
		HttpGet httpget = new HttpGet(url.toString());
		int statusCode = -1;

		try {
			


			String proxyHost = parameters_m.get("PROXY_HOST");

			if (proxyHost != null && proxyHost.length() > 0) {
				int proxyport = HtUtils.parseInt(parameters_m.get("PROXY_PORT"));

				HttpHost proxy = new HttpHost(proxyHost.toString(), proxyport);
				httpclient.getParams().setParameter(ConnRoutePNames.DEFAULT_PROXY, proxy);

			}


		

			// Provide custom retry handler is necessary
			//method.getParams().setParameter(HttpMethodParams.RETRY_HANDLER, new DefaultHttpMethodRetryHandler(3, false));
			httpget.setHeader("Referer", "http://BeeOnLine-Express");
			httpget.setHeader("Host", "www.beeonline.ru");
			httpget.setHeader("Accept-Language", "ru,ja;q=0.5");
			httpget.setHeader("Accept-Encoding", "deflate");
			httpget.setHeader("User-Agent", "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 1.1.4322; InfoPath.1; .NET CLR 2.0.50727; .NET CLR 3.0.04506.30; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022)");
			httpget.setHeader("Content-Type", "application/x-www-form-urlencoded");

			// Execute the method.
			HttpResponse response = httpclient.execute(httpget);
			statusCode = response.getStatusLine().getStatusCode();
			if (statusCode == HttpStatus.SC_OK) {
				HttpEntity entity = response.getEntity();
				if (entity != null) {
					InputStream is = entity.getContent();
					body.append(HtUtils.convertInputStreamToString(is, null));
				}
			}

		//HtUtils.log(Level.INFO, getContext(), "sendBeelineSms", "URL: " + method.getURI());


		} catch (Throwable e) {
			throw new HtException(getContext(), "sendBeelineSms", e);
		} finally {
		
		}

		//if (statusCode != HttpStatus.SC_OK)
		//    throw new HtException(getContext(), "sendBeelineSms", "Server responded with code: " + statusCode);

		out.append(body);
		return out.toString();
	}

	public Integer[] returnSubscribtionEventTypes() {
		return new Integer[] {XmlEvent.ET_AlgorithmAlert};
	}


}
