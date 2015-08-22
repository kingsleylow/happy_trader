/*
 * RtTBRF.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.rtproviders;

import com.fin.httrader.hlpstruct.BackgroundJobStatus;
import com.fin.httrader.interfaces.HtRealTimeProvider;
import com.fin.httrader.managers.RtGlobalEventManager;
import com.fin.httrader.managers.RtRealTimeProviderManager;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.hlpstruct.HtRtData;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import java.io.IOException;
import java.io.InputStream;
import java.text.NumberFormat;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Collection;
import java.util.Date;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.Properties;
import java.util.TimeZone;
import java.util.Timer;
import java.util.TimerTask;
import java.util.TreeSet;
import java.util.Vector;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import javax.mail.BodyPart;
import javax.mail.Message;
import javax.mail.Multipart;
import javax.mail.Session;
import javax.mail.Transport;
import javax.mail.URLName;
import javax.mail.internet.InternetAddress;
import javax.mail.internet.MimeBodyPart;
import javax.mail.internet.MimeMessage;
import javax.mail.internet.MimeMultipart;


import org.apache.http.HttpEntity;
import org.apache.http.HttpHost;
import org.apache.http.HttpResponse;
import org.apache.http.HttpStatus;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.conn.params.ConnRoutePNames;
import org.apache.http.impl.client.DefaultHttpClient;
/*
import org.xbill.DNS.Lookup;
import org.xbill.DNS.MXRecord;
import org.xbill.DNS.Record;
import org.xbill.DNS.Resolver;
import org.xbill.DNS.SimpleResolver;
import org.xbill.DNS.TextParseException;
import org.xbill.DNS.Type;
 */

/**
 *
 * @author Victor_Zoubok
 */
public class RtTBRF implements HtRealTimeProvider {

	private BackgroundJobStatus status_m = new BackgroundJobStatus();

	public BackgroundJobStatus returnProviderStatus() {
		return status_m;
	}

	@Override
	public void returnAvailableSymbolList(List<String> symbols) {
		throw new UnsupportedOperationException("Not supported yet.");
	}

	public class QueryDataTask extends TimerTask {

		private long curScheduledTime_m = -1;
		private Timer timer_m = null;
		private RtTBRF base_m = null;
		private int year_m = -1;
		private int month_m = -1;
		private int day_m = -1;

		QueryDataTask(long curScheduledTime, Timer newTm, RtTBRF base, int year, int month, int day) {
			timer_m = newTm;
			curScheduledTime_m = curScheduledTime;
			base_m = base;

			year_m = year;
			month_m = month;
			day_m = day;
		}

		public void run() {

			try {

				base_m.sendProviderMessage("Timer fired");

				//
				// do timer task
				base_m.doTimerTask(year_m, month_m, day_m);


				long newTime = curScheduledTime_m;
				if (newTime > 0) {
					// will set up new time- one day in advance
					newTime += 24 * 60 * 60 * 1000;
					base_m.sendProviderMessage("Timer will setup at new (GMT) time: " + HtDateTimeUtils.time2SimpleString_Gmt(newTime));

					QueryDataTask newTask = new QueryDataTask(newTime, timer_m, base_m, year_m, month_m, day_m);

					timer_m.schedule(newTask, new Date(newTime));

				} else {
					timer_m.cancel();
					timer_m.purge();
				}



			} catch (Throwable e) {
				base_m.sendProviderError("Error happened on timer fire: " + e.getMessage());

			}
		}
	};
	//
	private long timeShift_m = 0;
	private StringBuilder id_m = new StringBuilder();
	private SimpleDateFormat formatDay_m = new SimpleDateFormat("yyyy.MM.dd HH:mm,z", Locale.US);
	private SimpleDateFormat formatOnlyDay_m = new SimpleDateFormat("yyyy.MM.dd,z", Locale.US);
	private HashSet<Timer> firedTimers_m = new HashSet<Timer>();
	// symbol lsit
	private HashSet<String> symbolList_m = new HashSet<String>();
	// proxy for HTTP quesries
	private StringBuilder proxyHost_m = new StringBuilder();
	private int proxyPort_m = -1;
	// email list
	private HashSet<String> emailList_m = new HashSet<String>();
	private HashSet<String> smsBeelineList_m = new HashSet<String>();
	// smtp host
	private StringBuilder smtpHost_m = new StringBuilder();
	private int smtpPort_m = 25;
	private StringBuilder smtpUserName_m = new StringBuilder();
	private StringBuilder smtpPassword_m = new StringBuilder();

	/** Creates a new instance of RtTBRF */
	public RtTBRF() {
	}

	private String getContext() {
		return this.getClass().getSimpleName();
	}

	public void initialize(Map<String, String> initdata) throws Exception {
		status_m.initializeOk();
		
		for (Iterator<String> it = initdata.keySet().iterator(); it.hasNext();) {
			String key = it.next().toUpperCase();
			if (key.equals("PROXYHOST")) {
				proxyHost_m.setLength(0);
				proxyHost_m.append(initdata.get(key));
			} else if (key.equals("PROXYPORT")) {
				proxyPort_m = Integer.valueOf(initdata.get(key));
			} else if (key.equals("SMTPHOST")) {
				smtpHost_m.setLength(0);
				smtpHost_m.append(initdata.get(key));
			} else if (key.equals("SMTPPORT")) {
				smtpPort_m = Integer.valueOf(initdata.get(key));
			} else if (key.equals("SMTPUSER")) {
				smtpUserName_m.setLength(0);
				smtpUserName_m.append(initdata.get(key));
			} else if (key.equals("SMTPPASSWORD")) {
				smtpPassword_m.setLength(0);
				smtpPassword_m.append(initdata.get(key));
			} else if (key.indexOf("EMAIL_") >= 0) {
				emailList_m.add(new String(initdata.get(key)));
			} else if (key.indexOf("PHONE_") >= 0) {
				smsBeelineList_m.add(new String(initdata.get(key)));
			} else if (key.indexOf("SYMBOL_") >= 0) {
				symbolList_m.add(new String(initdata.get(key)));
			} else if (key.indexOf("TIME_") >= 0) {
				// get the time
				StringBuilder timeStr = new StringBuilder(initdata.get(key));

				long setTime = 0;
				Calendar dCal = Calendar.getInstance();
				TimeZone tzone = TimeZone.getTimeZone("GMT-0");
				dCal.setTimeZone(tzone);

				int year = dCal.get(Calendar.YEAR);
				int month = 1 + dCal.get(Calendar.MONTH);
				int day = dCal.get(Calendar.DAY_OF_MONTH);

				if (timeStr.toString().equalsIgnoreCase("now")) {
					// n/a
				} else {


					StringBuilder dateTimeStr = new StringBuilder();
					dateTimeStr.append(year);
					dateTimeStr.append(".");
					dateTimeStr.append(month);
					dateTimeStr.append(".");
					dateTimeStr.append(day);
					dateTimeStr.append(" ");

					// HH:MM
					dateTimeStr.append(timeStr);
					dateTimeStr.append(",GMT");
					Date dat = formatDay_m.parse(dateTimeStr.toString());
					setTime = dat.getTime();

				}
				//

				Timer newTm = new Timer();


				newTm.schedule(new RtTBRF.QueryDataTask(setTime, newTm, this, year, month, day), new Date(setTime));
				synchronized (firedTimers_m) {
					firedTimers_m.add(newTm);
				}

				if (setTime > 0) {
					sendProviderMessage("Created timer schedule on (GMT): " + HtDateTimeUtils.time2SimpleString_Gmt(setTime));
				} else {
					sendProviderMessage("Created timer one-off schedule now");
				}

			}
		}
	}

	public void doTimerTask(int year, int month, int day) {
		try {


			sendTsBRfPrices(year, month, day);
		} catch (Throwable e) {
			sendProviderError("Error happened on performing timing task: " + e.getMessage());
		}
	}

	public void deinitialize() throws Exception {
		synchronized (firedTimers_m) {
			for (Iterator<Timer> it = firedTimers_m.iterator(); it.hasNext();) {
				Timer tm = it.next();

				tm.cancel();
				tm.purge();
			}
		}
	}

	public String getProviderId() {
		return id_m.toString();
	}

	public void setProviderId(String providerId) {
		id_m.setLength(0);
		id_m.append(providerId);
	}

	public void setTimeShift(long timeshift) {
		timeShift_m = timeshift;
	}

	public long getTimeShift() {
		return timeShift_m;
	}

	/**
	 * Helpers
	 */
	public void sendProviderMessage(String message) {
		XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_INFO, message);
	}

	public void sendProviderError(String errormsg) {
		XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_ERROR, errormsg);


	}

	private void sendTsBRfPrices(int year, int month, int day) throws Exception {
		for (Iterator<String> it = symbolList_m.iterator(); it.hasNext();) {
			String symbol = it.next();
			HtRtData rtdata = new HtRtData();

			rtdata.setSymbol(symbol);
			rtdata.setProvider(getProviderId());

			// divide symbol
			String symbol1 = symbol.substring(0, 3);
			String symbol2 = symbol.substring(3, 6);

			if (symbol1 == null || symbol1.length() != 3) {
				throw new HtException(getContext(), "sendTsBRfPrices", "Invalid symbol 1");
			}

			if (symbol2 == null || symbol2.length() != 3) {
				throw new HtException(getContext(), "sendTsBRfPrices", "Invalid symbol 2");
			}


			double resultTsB = getTsBRfPrice(symbol1, symbol2, year, month, day);
			if (resultTsB > 0) {
				rtdata.type_m = HtRtData.RT_Type;
				rtdata.ask_m = resultTsB;
				rtdata.bid_m = resultTsB;

				// date
				StringBuilder dateTimeStr = new StringBuilder();
				dateTimeStr.append(year);
				dateTimeStr.append(".");
				dateTimeStr.append(month);
				dateTimeStr.append(".");
				dateTimeStr.append(day);
				dateTimeStr.append(",GMT");

				Date dt = formatOnlyDay_m.parse(dateTimeStr.toString());
				rtdata.time_m = dt.getTime();

				// send this
				XmlEvent rtDataEvent = new XmlEvent();
				RtRealTimeProviderManager.createXmlEventRtData(rtdata, rtDataEvent);
				RtGlobalEventManager.instance().pushCommonEvent(rtDataEvent);

			// that's it
			} else {
				throw new HtException(getContext(), "sendTsBRfPrices", "Was not able to read the price from conv.rbc.ru for: " + symbol1 + " - " + symbol2);
			}

			// read current rate
			double resultMarket = getMarketPrice(symbol1, symbol2);
			if (resultMarket <= 0) {
				throw new HtException(getContext(), "sendTsBRfPrices", "Was not able to read the price from wap.alpari.org for: " + symbol1 + " - " + symbol2);
			}


			// create SMS message
			StringBuilder message = new StringBuilder();

			// get GMT time


			message.append(HtDateTimeUtils.time2SimpleString_Gmt(HtDateTimeUtils.getCurGmtTime()));
			message.append("(GMT): ");
			message.append(symbol1 + "/" + symbol2 + " TSB:" + resultTsB + " MRKT:" + resultMarket);

			sendProviderMessage("Created message: " + message);
			//

			// send for emails
			for (Iterator<String> it2 = emailList_m.iterator(); it2.hasNext();) {
				String email = it2.next();
				try {
					sendEmail(message.toString(), email);
					sendProviderMessage("Successfully sent message to: " + email);
				} catch (Throwable e) {
					sendProviderError("On attempting to send email to: " + email + " exception happened: " + e.getMessage());
				}

			}

			sendProviderMessage("All email recepients processed");

			// send sms
			for (Iterator<String> it2 = smsBeelineList_m.iterator(); it2.hasNext();) {
				String phone = it2.next();
				try {
					String ret = sendBeelineSms(message.toString(), phone);
					sendProviderMessage("Successfully sent sms to: " + phone + " with result: " + ret);
				} catch (Throwable e) {
					sendProviderError("On attempting to send sms to: " + phone + " exception happened: " + e.getMessage());
				}

			}


			//
			sendProviderMessage("All SMS recepients processed");

		}
	}

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
		String smessage = message.replaceAll("[\\s]", "+");

		url.append("http://www.beeonline.ru/servlet/send/sms/?phone=");
		url.append(purenumber);
		url.append("&number_sms=number_sms_send");
		url.append("&prf=");
		url.append(prefix);
		url.append("&termtype=G");
		url.append("&message=");
		url.append(smessage);
		url.append("&translit=");

		//
		HttpClient httpclient = new DefaultHttpClient();
		HttpGet httpget = new HttpGet(url.toString());

		int statusCode = -1;
		try {
		

			if (proxyHost_m.length() > 0) {
				HttpHost proxy = new HttpHost(proxyHost_m.toString(), proxyPort_m);
				httpclient.getParams().setParameter(ConnRoutePNames.DEFAULT_PROXY, proxy);
			}

		
			// Provide custom retry handler is necessary
			//method.getParams().setParameter(HttpMethodParams.RETRY_HANDLER, new DefaultHttpMethodRetryHandler(3, false));
			httpget.setHeader("Referer", "http://BeeOnLine-Express");
			httpget.setHeader("Host", "www.beeonline.ru");
			httpget.setHeader("Accept-Language", "ru,ja;q=0.5");
			httpget.setHeader("Accept-Encoding", "deflate");
			httpget.setHeader("User-Agent", "Mozilla/4.7 [en] (Win95; I)");
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


		

		} catch (Throwable e) {
			throw new HtException(getContext(), "sendBeelineSms", e);
		} finally {
			
		}

		if (statusCode != HttpStatus.SC_OK) {
			throw new HtException(getContext(), "sendBeelineSms", "Server responded with code: " + statusCode);
		}

		out.append("Beeline server returned: ");
		out.append(body);
		return out.toString();
	}

	private void sendEmail(String message, String address) throws Exception {


		int port = -1;
		String host = "";

		/*
		int idx = address.indexOf('@');
		if (idx >= 0) {
		String domainName = address.substring(idx + 1);
		Vector<URLName> mxRecords = getMXRecordsForHost(domainName);
		if (mxRecords.size()==0)
		throw new HtException(getContext(), "sendEmail", "Was not able to resolve MX records for: " + domainName );

		for(Iterator<URLName> it = mxRecords.iterator(); it.hasNext(); )   {
		URLName mxHost = it.next();

		host = mxHost.getHost();
		port = mxHost.getPort();

		}
		}
		else
		throw new HtException(getContext(), "sendEmail", "Invalid email address: " + address );

		 */


		String subject = "TSBRF";
		Properties prop = System.getProperties();


		prop.put("mail.smtp.host", smtpHost_m.toString());
		prop.put("mail.smtp.port", String.valueOf(smtpPort_m));

		// authentication
		if (this.smtpUserName_m.length() > 0) {
			prop.put("mail.smtp.auth", "true");

		}

		Session ses1 = Session.getDefaultInstance(prop, null);
		MimeMessage msg = new MimeMessage(ses1);
		msg.setFrom(new InternetAddress(address));


		msg.addRecipient(Message.RecipientType.TO, new InternetAddress(address));
		msg.setSubject(subject);

		BodyPart messageBodyPart = new MimeBodyPart();
		messageBodyPart.setText(message);
		Multipart multipart = new MimeMultipart();
		multipart.addBodyPart(messageBodyPart);

		msg.setContent(multipart);
		msg.setSentDate(new Date());

		if (this.smtpUserName_m.length() > 0) {
			Transport tr = ses1.getTransport("smtp");
			tr.connect(smtpHost_m.toString(), smtpUserName_m.toString(), smtpPassword_m.toString());
			msg.saveChanges();      // don't forget this
			tr.sendMessage(msg, msg.getAllRecipients());
			tr.close();

		} else {
			// no auth
			Transport.send(msg);
		}
	}

	// get current market price
	private double getMarketPrice(String symbol1, String symbol2) throws Exception {
		double result = -1;

		String url = "http://wap.alpari.org/forex.wml";
		StringBuilder body = new StringBuilder();

		HttpClient httpclient = new DefaultHttpClient();
		HttpGet httpget = new HttpGet(url.toString());
		int statusCode = -1;

		try {
			

			if (proxyHost_m.length() > 0) {
				HttpHost proxy = new HttpHost(proxyHost_m.toString(), proxyPort_m);
				httpclient.getParams().setParameter(ConnRoutePNames.DEFAULT_PROXY, proxy);
			}

		

			// Provide custom retry handler is necessary
			//method.getParams().setParameter(HttpMethodParams.RETRY_HANDLER, new DefaultHttpMethodRetryHandler(3, false));

			// Execute the method.
			HttpResponse response = httpclient.execute(httpget);
			statusCode = response.getStatusLine().getStatusCode();

			HttpEntity entity = response.getEntity();
			if (entity != null) {
					InputStream is = entity.getContent();
					body.append(HtUtils.convertInputStreamToString(is, null));
			}

		} catch (Throwable e) {
			throw new HtException(getContext(), "getMarketPrice", e);
		} finally {
			
		}

		if (statusCode != HttpStatus.SC_OK) {
			return result;
		}

		String wholesymbol = symbol1 + symbol2;
		Pattern pattern = Pattern.compile(wholesymbol.toUpperCase() + " ([\\d\\.]+)\\/([\\d\\.]+)");
		Matcher matcher = pattern.matcher(body.toString().toUpperCase());

		while (matcher.find()) {
			String grp = matcher.group(1);
			sendProviderMessage("Read current market price value: \"" + grp + "\" - " + symbol1 + " - " + symbol2);

			//
			result = Double.valueOf(grp);
			break;
		}


		return result;
	}

	private double getTsBRfPrice(String symbol1, String symbol2, int year, int month, int day) throws Exception {
		// http://conv.rbc.ru/convert.shtml?mode=calc&source=cb.0&tid_from=EUR&commission=1&tid_to=USD&summa=100&day=16&mon=05&year=2008
		double result = -1;


		// sielently advance day for tomorrow
		Calendar dCal = Calendar.getInstance();
		TimeZone tzone = TimeZone.getTimeZone("GMT-0");
		dCal.setTimeZone(tzone);


		dCal.set(Calendar.YEAR, year);
		dCal.set(Calendar.MONTH, month - 1);
		dCal.set(Calendar.DAY_OF_MONTH, day);

		dCal.roll(Calendar.DAY_OF_MONTH, 1);
		int nyear = dCal.get(Calendar.YEAR);
		int nmonth = 1 + dCal.get(Calendar.MONTH);
		int nday = dCal.get(Calendar.DAY_OF_MONTH);



		StringBuilder url = new StringBuilder();
		NumberFormat nf = NumberFormat.getInstance();
		nf.setMinimumIntegerDigits(2);


		url.append("http://conv.rbc.ru/convert.shtml?mode=calc&source=cb.0&tid_from=");
		url.append(symbol1);
		url.append("&commission=1&tid_to=");
		url.append(symbol2);
		url.append("&summa=100");
		url.append("&day=");
		url.append(nf.format(nday));
		url.append("&mon=");
		url.append(nf.format(nmonth));
		url.append("&year=");
		url.append(nyear);
		//
		StringBuilder body = new StringBuilder();

		HttpClient httpclient = new DefaultHttpClient();
		HttpGet httpget = new HttpGet(url.toString());
		int statusCode = -1;

		try {
			
			if (proxyHost_m.length() > 0) {
				HttpHost proxy = new HttpHost(proxyHost_m.toString(), proxyPort_m);
				httpclient.getParams().setParameter(ConnRoutePNames.DEFAULT_PROXY, proxy);
			}

			
			// Provide custom retry handler is necessary
			//method.getParams().setParameter(HttpMethodParams.RETRY_HANDLER, new DefaultHttpMethodRetryHandler(3, false));

			// Execute the method.
			HttpResponse response = httpclient.execute(httpget);
			statusCode = response.getStatusLine().getStatusCode();

			HttpEntity entity = response.getEntity();
			if (entity != null) {
					InputStream is = entity.getContent();
					body.append(HtUtils.convertInputStreamToString(is, null));
			}

		} catch (Throwable e) {
			throw new HtException(getContext(), "getTsBRfPrice", e);
		} finally {
			
		}

		if (statusCode != HttpStatus.SC_OK) {
			return result;
		}

		//
		Pattern pattern = Pattern.compile("<TD height=32>:</TD>(\\s+)<TD><B>([\\d\\.]+)");
		Matcher matcher = pattern.matcher(body.toString());

		while (matcher.find()) {
			String grp = matcher.group(2);
			sendProviderMessage("Read raw TSBRF price value: \"" + grp + "\" for: " + nyear + "." + nmonth + "." + nday + " - " + symbol1 + " - " + symbol2);

			//
			result = Double.valueOf(grp);
			break;
		}



		return result;
	}
	//
    /*
	 * Special Thanks to Tim Motika (tmotika at ionami dot com) for
	 * his reworking of this method.
	 */
	/*
	public Vector getMXRecordsForHost(String hostName) throws Exception
	{
	
	Vector recordsColl = null;
	try {
	Lookup lookup = new Lookup(hostName, Type.MX);

	Resolver resolver = new SimpleResolver();
	lookup.setResolver(resolver);
	lookup.setCache(null);


	Record[] records = lookup.run();
	int result = lookup.getResult();

	// Note: alteration here since above may be null
	recordsColl = new Vector();
	for (int i = 0; i < records.length; i++) {
	// if records was null .size() will be zero causing this to just skip
	MXRecord mx = (MXRecord) records[i];

	int prt = mx.getPriority();

	String targetString = mx.getTarget().toString();
	URLName uName = new URLName("smtp://" + targetString.substring(0, targetString.length() - 1));
	recordsColl.add(uName);
	//System.out.println("Host " + uName.getHost() + " has preference " + mx.getPriority());
	}

	// No existing MX records can either mean:
	// 1. the server doesn't do mail because it doesn't exist.
	// 2. the server doesn't need another host to do its mail.
	// If the server resolves, assume it does its own mail.
	if (recordsColl.size() <= 0) {
	Record[] recordsTypeA = new Lookup(hostName, Type.A).run();

	if (recordsTypeA != null && recordsTypeA.length > 0) {
	recordsColl.addElement(new URLName("smtp://" + hostName));
	}
	}

	} catch (TextParseException e) {
	throw new HtException(getContext(), "getMXRecordsForHost", e);
	}
	
	return recordsColl;
	}
	 */
}
