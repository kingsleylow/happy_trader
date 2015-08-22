/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.plugings;

import au.com.bytecode.opencsv.CSVReader;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtFileUtils;
import com.fin.httrader.utils.HtSystem;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.LongParam;
import com.fin.httrader.utils.URLUTF8Encoder;
import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.utils.hlpstruct.HtPair;
import com.fin.httrader.utils.hlpstruct.HtRtData;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.net.URI;
import java.util.Calendar;
import java.util.List;
import java.util.Vector;
import java.util.zip.GZIPInputStream;
import java.util.zip.InflaterInputStream;
import org.apache.http.Header;
import org.apache.http.HeaderElement;
import org.apache.http.HttpEntity;
import org.apache.http.HttpException;
import org.apache.http.HttpHost;
import org.apache.http.HttpResponse;
import org.apache.http.HttpResponseInterceptor;
import org.apache.http.HttpStatus;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.client.utils.URIUtils;
import org.apache.http.conn.params.ConnRoutePNames;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.protocol.HttpContext;

/**
 *
 * @author DanilinS
 */
public class FinamUtils {

	public static final int FINAM_DOWNLOAD_PERIOD_DUMMY = 0;
	public static final int FINAM_DOWNLOAD_PERIOD_TICK = 1;
	public static final int FINAM_DOWNLOAD_PERIOD_1MIN = 2;
	public static final int FINAM_DOWNLOAD_PERIOD_5MIN = 3;
	public static final int FINAM_DOWNLOAD_PERIOD_10MIN = 4;
	public static final int FINAM_DOWNLOAD_PERIOD_15MIN = 5;
	public static final int FINAM_DOWNLOAD_PERIOD_30MIN = 6;
	public static final int FINAM_DOWNLOAD_PERIOD_1HOUR = 7;
	public static final int FINAM_DOWNLOAD_PERIOD_1HOUR_FROM1030 = 11;
	public static final int FINAM_DOWNLOAD_PERIOD_1DAY = 8;
	public static final int FINAM_DOWNLOAD_PERIOD_1WEEK = 9;
	public static final int FINAM_DOWNLOAD_PERIOD_1MONTH = 10;

	public static String getContext() {
		return FinamUtils.class.getSimpleName();
	}

	public static int getDownloadTimePeriodFromString(String tpers) {

		if (tpers.equalsIgnoreCase("TICK")) {
			return FINAM_DOWNLOAD_PERIOD_TICK;
		} else if (tpers.equalsIgnoreCase("1MIN")) {
			return FINAM_DOWNLOAD_PERIOD_1MIN;
		} else if (tpers.equalsIgnoreCase("5MIN")) {
			return FINAM_DOWNLOAD_PERIOD_5MIN;
		} else if (tpers.equalsIgnoreCase("10MIN")) {
			return FINAM_DOWNLOAD_PERIOD_10MIN;
		} else if (tpers.equalsIgnoreCase("15MIN")) {
			return FINAM_DOWNLOAD_PERIOD_15MIN;
		} else if (tpers.equalsIgnoreCase("30MIN")) {
			return FINAM_DOWNLOAD_PERIOD_30MIN;
		} else if (tpers.equalsIgnoreCase("1HOUR")) {
			return FINAM_DOWNLOAD_PERIOD_1HOUR;
		} else if (tpers.equalsIgnoreCase("1HOUR_FROM1030")) {
			return FINAM_DOWNLOAD_PERIOD_1HOUR_FROM1030;
		} else if (tpers.equalsIgnoreCase("1DAY")) {
			return FINAM_DOWNLOAD_PERIOD_1DAY;
		} else if (tpers.equalsIgnoreCase("1WEEK")) {
			return FINAM_DOWNLOAD_PERIOD_1WEEK;
		} else if (tpers.equalsIgnoreCase("1MONTH")) {
			return FINAM_DOWNLOAD_PERIOD_1MONTH;
		}

		return FINAM_DOWNLOAD_PERIOD_DUMMY;
	}

	public static String getSymbolCodeFromSymbolName(String full_name) {

		if (full_name != null) {
			return full_name.substring(full_name.lastIndexOf("-") + 1);
		}

		return null;
	}

	public static String getMarketNameFromSymbolName(String full_name) {
		if (full_name != null) {
			return full_name.substring(0, full_name.lastIndexOf("-"));
		}

		return null;
	}

	public static String getSymbolName(HtPair<String, String> p) {
		return p.first + "-" + p.second;
	}

	public static String getBaseCacheDir(long begin_time, long end_time) {

		StringBuilder fileName = new StringBuilder(HtSystem.getSystemTempPath());
		fileName.append(File.separatorChar);
		fileName.append("finam_cache_dir");
		fileName.append(File.separatorChar);

		fileName.append(begin_time);
		fileName.append("_");
		fileName.append(end_time);

		fileName.append(File.separatorChar);
		File dir = new File(fileName.toString());
		dir.mkdirs();

		return fileName.toString();
	}

	public static void processTextFile(String provider_id, String pure_symbol_name, List<HtRtData> rtdata, File cacheFile) throws Exception {
		CSVReader reader = new CSVReader(new FileReader(cacheFile), ',');
		String[] nextLine;

		// reuse that instance
		Calendar tm = null;

		int rows = 0;

		while ((nextLine = reader.readNext()) != null) {

			// ignore first line
			if (rows++ <= 0) {
				continue;
			}

			if (nextLine.length != 7) // ignore invalids
			{
				continue;
			}

			// Date,Open,High,Low,Close,Volume,Adj Close
			HtRtData rtdata_i = new HtRtData(HtRtData.HST_Type);
			//rtdata_i.setSymbol(symbol_name);
			rtdata_i.setSymbol(pure_symbol_name);

			rtdata_i.open_m = Double.valueOf(nextLine[2]);
			rtdata_i.high_m = Double.valueOf(nextLine[3]);
			rtdata_i.low_m = Double.valueOf(nextLine[4]);
			rtdata_i.close_m = Double.valueOf(nextLine[5]);

			rtdata_i.open2_m = rtdata_i.open_m;
			rtdata_i.high2_m = rtdata_i.high_m;
			rtdata_i.low2_m = rtdata_i.low_m;
			rtdata_i.close2_m = rtdata_i.close_m;

			rtdata_i.volume_m = Double.valueOf(nextLine[6]);



			int year = Integer.valueOf(nextLine[0].substring(0, 4));
			int month = Integer.valueOf(nextLine[0].substring(4, 6));
			int day = Integer.valueOf(nextLine[0].substring(6, 8));

			int hour = Integer.valueOf(nextLine[1].substring(0, 2));
			int minute = Integer.valueOf(nextLine[1].substring(2, 4));
			int sec = Integer.valueOf(nextLine[1].substring(4, 6));

			LongParam millisec = new LongParam();
			tm = HtDateTimeUtils.yearMonthDayHourMinSec2Time_ReuseCalendar_Gmt(year, month, day, hour, minute, sec, tm, millisec);

			rtdata_i.time_m = millisec.getLong();

			// ignore not valid
			if (!rtdata_i.isValid()) {
				XmlEvent.createSimpleLog("", provider_id, CommonLog.LL_ERROR, "On the iteration #" + rows + " found invalid ticker: " + rtdata_i.toString() + " ignoring...");
			} else {
				rtdata.add(rtdata_i);
			}


		}

		reader.close();

	}

	public static boolean downloadHistoryPrices(
					String cache_file_name,
					String provider_id,
					String symbol, // synthetic symbol name consisting market + contrace
					String pure_symbol_name, // only pure symbol name
					int market_id,
					int emitent_id,
					int finam_time_period,
					String proxy_host,
					int proxy_port,
					int days_depth,
					long beg_time,
					long end_time) throws Exception {

		boolean result = false;
		XmlEvent.createSimpleLog("", provider_id, CommonLog.LL_DEBUG, "Provider will download data for " + symbol +
						" from: " + HtDateTimeUtils.time2String_Gmt(beg_time) + " to: " + HtDateTimeUtils.time2String_Gmt(end_time));


		LongParam end_year = new LongParam(), end_month = new LongParam(), end_day = new LongParam();
		LongParam beg_year = new LongParam(), beg_day = new LongParam(), beg_month = new LongParam();


		HtDateTimeUtils.time2YearMonthDay_Gmt(end_time, end_year, end_month, end_day);
		HtDateTimeUtils.time2YearMonthDay_Gmt(beg_time, beg_year, beg_month, beg_day);

		StringBuilder fileName = new StringBuilder(pure_symbol_name);
		fileName.append("_");
		fileName.append(beg_year.getLong());
		fileName.append(beg_month.getLong());
		fileName.append(beg_day.getLong());
		fileName.append("_");
		fileName.append(end_year.getLong());
		fileName.append(end_month.getLong());
		fileName.append(end_day.getLong());



		StringBuilder url_param = new StringBuilder();




		// market
		url_param.append("&market=");
		url_param.append(market_id);

		url_param.append("&em=");
		url_param.append(emitent_id);

		// period
		url_param.append("&p=");
		url_param.append(finam_time_period);

		// begin date
		url_param.append("&df=");
		url_param.append(beg_day.getLong());
		url_param.append("&mf=");
		url_param.append(beg_month.getLong() - 1);
		url_param.append("&yf=");
		url_param.append(beg_year.getLong());

		// end date
		url_param.append("&dt=");
		url_param.append(end_day.getLong());
		url_param.append("&mt=");
		url_param.append(end_month.getLong() - 1);
		url_param.append("&yt=");
		url_param.append(end_year.getLong());

		// who knows what is this?
		url_param.append("&f=");
		url_param.append(fileName.toString());


		// text file
		url_param.append("&e=.txt");

		// date time formats
		url_param.append("&dtf=1&tmf=1");

		//this is the time of BEGIN of the candle
		url_param.append("&MSOR=0");

		// contract name
		url_param.append("&cn=");
		url_param.append(URLUTF8Encoder.encode(pure_symbol_name));

		// fields delimiter - ','
		url_param.append("&sep=1");

		// category separator
		url_param.append("&sep2=1");

		// include the first line
		url_param.append("&at=1");

		// file format
		// DATE, TIME, OPEN, HIGH, LOW, CLOSE, VOL
		url_param.append("&datf=5");

		// create URI

		StringBuilder base = new StringBuilder("/");
		base.append(fileName.toString());
		base.append(".txt?d=d");

		URI uri = URIUtils.createURI("http", "195.128.78.52", -1, base.toString(), url_param.toString(), null);
		DefaultHttpClient httpclient = new DefaultHttpClient();

		httpclient.addResponseInterceptor(new HttpResponseInterceptor() {

			public void process(
							final HttpResponse response,
							final HttpContext context) throws HttpException, IOException {
				HttpEntity entity = response.getEntity();
				Header ceheader = entity.getContentEncoding();
				if (ceheader != null) {
					HeaderElement[] codecs = ceheader.getElements();
					for (int i = 0; i < codecs.length; i++) {
						if (codecs[i].getName().equalsIgnoreCase("gzip")) {
							response.setEntity(new GzipDecompressingEntity(response.getEntity()));
						} else if (codecs[i].getName().equalsIgnoreCase("deflate")) {
							response.setEntity(new DeflateDecompressingEntity(response.getEntity()));
						}
					}
				}
			}
		});




		try {

			HttpGet httpget = new HttpGet(uri);

			if (proxy_host.length() > 0) {
				HttpHost proxy = new HttpHost(proxy_host.toString(), proxy_port);
				httpclient.getParams().setParameter(ConnRoutePNames.DEFAULT_PROXY, proxy);
			}



			// Provide custom retry handler is necessary

			//method.getParams().setParameter(HttpMethodParams.RETRY_HANDLER, new DefaultHttpMethodRetryHandler(3, false));
			httpget.setHeader("Referer", "http://www.finam.ru/analysis/export/default.asp");
			httpget.setHeader("Host", "195.128.78.52");
			httpget.setHeader("Accept-Language", "ru,ja;q=0.5");
			httpget.setHeader("Accept-Encoding", "gzip, deflate");
			httpget.setHeader("User-Agent", "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 1.1.4322; InfoPath.1; .NET CLR 2.0.50727; .NET CLR 3.0.04506.30; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022)");
			httpget.setHeader("Content-Type", "application/x-www-form-urlencoded");

			// Execute the method.

			HttpResponse response = httpclient.execute(httpget);


			if (response.getStatusLine().getStatusCode() == HttpStatus.SC_OK) {

				HttpEntity entity = response.getEntity();

				if (entity != null) {

					String contentEncoding = entity.getContentEncoding().getValue();

					writeStreamContentToFile(cache_file_name, entity.getContent());

					result = true;
				}

			//InputStream is = method.getResponseBodyAsStream();

			} else {
				throw new HtException(getContext(), "downloadHistoryPrices", "HTTP response is invalid: " + response.getStatusLine().getStatusCode() + " - " + response.getStatusLine().getReasonPhrase());
			}

		} catch (Throwable e) {
			// will log only

			XmlEvent.createSimpleLog("", provider_id, CommonLog.LL_ERROR, "While downloading exception: " + e.getMessage());

		//throw new HtException(getContext(), "downloadHistoryPrices", e);
		} finally {
			httpclient.getConnectionManager().shutdown();
		}

		// parse body

		//http://ichart.finance.yahoo.com/table.csv?s=DELL&d=8&e=30&f=2009&g=d&a=7&b=17&c=1988&ignore=.csv
		return result;
	}

	public static void writeStreamContentToFile(String filename, InputStream is) throws Exception {
		FileOutputStream f = new FileOutputStream(filename);
		HtFileUtils.copyInputStream(is, f);
		f.flush();
		f.close();
	}

	public static int parseStringPer2(String src, int two_num) {
		int result = -1;

		int idx = two_num * 2;
		int idx2 = idx + 2;
		if (idx2 > src.length()) {
			return result;
		}

		return Integer.valueOf(src.substring(idx, idx2));

	}
}
