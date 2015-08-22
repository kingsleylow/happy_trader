package com.fin.httrader.plugings;


import au.com.bytecode.opencsv.CSVReader;
import com.fin.httrader.interfaces.HtHistoryProvider;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtFileUtils;
import com.fin.httrader.utils.HtSystem;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.LongParam;
import com.fin.httrader.utils.URLUTF8Encoder;
import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.utils.hlpstruct.HtRtData;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import java.io.File;
import java.io.FileReader;
import java.io.InputStream;
import java.util.Calendar;
import java.util.HashSet;
import java.util.Map;
import java.util.Vector;
import org.apache.commons.httpclient.DefaultHttpMethodRetryHandler;
import org.apache.commons.httpclient.HttpClient;
import org.apache.commons.httpclient.HttpMethod;
import org.apache.commons.httpclient.HttpStatus;
import org.apache.commons.httpclient.methods.GetMethod;
import org.apache.commons.httpclient.params.HttpMethodParams;

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author DanilinS
 */
public class YahooFinanceProvider  implements HtHistoryProvider {
	private StringBuilder providerName_m = new StringBuilder();
	private StringBuilder pathToSymbolFile_m = new StringBuilder();
	private int downloadDepthDays_m = -1;
	private long exportHourShift_m;
	private long importHourShift_m;
	private StringBuilder proxyHost_m = new StringBuilder();
	private int proxyPort_m = -1;
	//private Vector<String> symbolList_m = new Vector<String>();
	private HashSet<String> processingList_m = new HashSet<String>();
	private long beg_time_m = -1;
	private long end_time_m = -1;

	// -------------------------
	private String getContext() {
		return this.getClass().getSimpleName();
	}

	// -------------------------
	// this functions must be called to adjust time FROM HERE!!!
	public void setImportTimeShift(long timeshift) {
		importHourShift_m = timeshift;
	}

	public void setExportTimeShift(long timeshift) {
		exportHourShift_m = timeshift;
	}

	public long getImportTimeShift() {
		return importHourShift_m;
	}

	public long getExportTimeShift() {
		return exportHourShift_m;
	}

	// ---------------------------
	// export is not supported
	public boolean beginDataExport(Map<String, String> initdata, long datebegin, long dateend) throws Exception {
		return false;
	}

	public void setDataChunk(Vector<HtRtData> rtdata, String chunksymbol) throws Exception {
		throw new UnsupportedOperationException("Not supported yet.");
	}

	public void endDataExport() {
		throw new UnsupportedOperationException("Not supported yet.");
	}

	public boolean waitForExportDialog(Map<String, String> context, LongParam datebegin, LongParam dateend) {
		throw new UnsupportedOperationException("Not supported yet.");
	}

	// --------------------------
	public boolean beginDataImport(Map<String, String> initdata, long datebegin, long dateend) throws Exception {

		// init the file with the list of symbols
		pathToSymbolFile_m.setLength(0);
		pathToSymbolFile_m.append(HtUtils.getParameterWithException(initdata, "YAHOO_SYMBOLS"));

		downloadDepthDays_m = Integer.valueOf(HtUtils.getParameterWithException(initdata, "DEPTH_DAYS"));

		if (downloadDepthDays_m <= 0) {
			throw new HtException(getContext(), "beginDataImport", "Download depth is not valid");
		}

		proxyHost_m.setLength(0);
		proxyHost_m.append(HtUtils.getParameterWithoutException(initdata, "PROXY_HOST"));

		if (proxyHost_m.length() > 0) {
			proxyPort_m = Integer.valueOf(HtUtils.getParameterWithException(initdata, "PROXY_PORT"));
		}

		// parse symbol list
		processingList_m.clear();
		CSVReader reader = new CSVReader(new FileReader(pathToSymbolFile_m.toString()), ',');
		String[] nextLine;

		while ((nextLine = reader.readNext()) != null) {
			processingList_m.add( nextLine[0] );
		}

		reader.close();


		// time
		end_time_m = HtDateTimeUtils.getCurGmtTime_DayBegin();
		beg_time_m = end_time_m - downloadDepthDays_m * 24 * 60 * 60 * 1000;

		return true;
	}

	public void endDataImport() {

		proxyHost_m.setLength(0);
		proxyPort_m = -1;
		downloadDepthDays_m = -1;
		pathToSymbolFile_m.setLength(0);
		end_time_m = -1;
		end_time_m = -1;
	}

	public int getDataChunk(Vector<HtRtData> rtdata, StringBuilder chunksymbol) throws Exception {

		rtdata.clear();

		Vector<HtRtData> rtdatabuf=new Vector<HtRtData>();

		if (processingList_m.size() >0) {

			// get the symbol
			String symbolToProcess = processingList_m.iterator().next();

			// remove to be processed symbol
			processingList_m.remove(symbolToProcess);

			chunksymbol.setLength(0);
			chunksymbol.append(symbolToProcess);


			try {
				// before make sure we don't have temp storage
			  StringBuilder fileName = new StringBuilder(HtSystem.getSystemTempPath());
				fileName.append(File.separatorChar);
				fileName.append("yahoo_cache_");
				fileName.append(URLUTF8Encoder.encode(symbolToProcess));
				fileName.append("_");
				fileName.append(beg_time_m);
				fileName.append("_");
				fileName.append(end_time_m);
				fileName.append(".CSV");

				File cacheFile=new File(fileName.toString());
				if (!cacheFile.exists()) {
					// time today
					String body = downloadHistoryPrices(symbolToProcess, proxyHost_m.toString(),proxyPort_m, downloadDepthDays_m, beg_time_m, end_time_m);

					if (body.length()==0) {
							XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_INFO, "For symbol: '" + symbolToProcess + "' is empty content, ignoring..." );
							// out
							return processingList_m.size();
					}
					else {

						if ( !HtFileUtils.writeContentToFile(cacheFile, body)) {
							throw new HtException(getContext(), "getDataChunk", "Can't create cache file: " + fileName);
						}
					}

				} else {
					XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_INFO, "For symbol: '" + symbolToProcess + "' reusing cache file" );

				}


				// now cache file must exist - must read it
				CSVReader reader = new CSVReader(new FileReader(cacheFile), ',');
				String[] nextLine;

				// reuse that instance
				Calendar tm = null;

				int rows = 0;
				while ((nextLine = reader.readNext()) != null) {

					// ignore first line
					if (rows++<=0)
						continue;

					// Date,Open,High,Low,Close,Volume,Adj Close
					HtRtData rtdata_i = new HtRtData(HtRtData.HST_Type);
					rtdata_i.setSymbol(symbolToProcess);

					rtdata_i.open_m = Double.valueOf( nextLine[1] );
					rtdata_i.high_m = Double.valueOf( nextLine[2] );
					rtdata_i.low_m = Double.valueOf( nextLine[3] );
					rtdata_i.close_m = Double.valueOf( nextLine[4] );

					rtdata_i.open2_m = rtdata_i.open_m;
					rtdata_i.high2_m = rtdata_i.high_m;
					rtdata_i.low2_m = rtdata_i.low_m;
					rtdata_i.close2_m = rtdata_i.close_m;

					rtdata_i.volume_m = Double.valueOf( nextLine[5] );

					// parse date time
					// 2009-09-28
					//HtUtils.yearMonthDay2Time_ReuseCalendar_Gmt(int year, int month, int day, Calendar tmc, LongParam result)

					String[] tokens_date = nextLine[0].split("-");

					int year = Integer.valueOf(tokens_date[0]);
					int month =  Integer.valueOf(tokens_date[1]);
					int day=  Integer.valueOf(tokens_date[2]);

					LongParam millisec = new LongParam();
					tm = HtDateTimeUtils.yearMonthDay2Time_ReuseCalendar_Gmt(year, month, day, tm, millisec);

					rtdata_i.time_m = millisec.getLong();

					rtdatabuf.add(rtdata_i);

				}

				reader.close();

				// visa versa
				rtdata.setSize(rtdatabuf.size());
				for(int i = 0; i < rtdatabuf.size(); i++)	{
					rtdata.set(rtdatabuf.size()-i-1, rtdatabuf.get(i));
				}




			}
			catch(Throwable e)
			{
				// delegate further
				XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_ERROR, "Exception on processing the symbol: '" + symbolToProcess +
								"' - " + e.getMessage() );

				throw new HtException(getContext(), "getDataChunk", e);
			}


			// need to process the last
			return (processingList_m.size() + 1);
		}
		else
			return 0;

	}



	public boolean waitForImportDialog(Map<String, String> context, LongParam datebegin, LongParam dateend) {
		throw new UnsupportedOperationException("Not supported yet.");
	}

	public String getProviderId() {
		return providerName_m.toString();
	}

	public void setProviderId(String providerId) {
		providerName_m.setLength(0);
		providerName_m.append(providerId);
	}

	public void getAvailableSymbolsToImport(Vector<String> symbols) {
		symbols.clear();
		symbols.addAll( processingList_m );

	}

	public void getSymbolMinMaxDatesToImport(String symbol, LongParam minDate, LongParam maxDate) {
		// current dats

		maxDate.setLong(HtDateTimeUtils.getCurGmtTime_DayBegin());
		minDate.setLong((long) (maxDate.getLong() - downloadDepthDays_m * 24 * 60 * 60 * 1000));

	}

	// ------------------------------
	// helpers
	String downloadHistoryPrices(String symbol, String proxy_host, int proxy_port, int days_depth, long beg_time, long end_time) throws Exception {

		XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_DEBUG, "Provider will download data for " + symbol+
		" from: " + HtDateTimeUtils.time2String_Gmt(beg_time) + " to: " + HtDateTimeUtils.time2String_Gmt(end_time));


		LongParam end_year = new LongParam(), end_month = new LongParam(), end_day = new LongParam();
		LongParam beg_year = new LongParam(), beg_day = new LongParam(), beg_month = new LongParam();

		HtDateTimeUtils.time2YearMonthDay_Gmt(end_time, end_year, end_month, end_day);
		HtDateTimeUtils.time2YearMonthDay_Gmt(beg_time, beg_year, beg_month, beg_day);

		StringBuilder body = new StringBuilder();
		StringBuilder url = new StringBuilder();


		url.append("http://ichart.finance.yahoo.com/table.csv?s=");
		url.append(URLUTF8Encoder.encode(symbol));

		url.append("&d="); // month last
		url.append(end_month.getLong() - 1);

		url.append("&e="); //day last
		url.append(end_day.getLong());

		url.append("&f="); // year last
		url.append(end_year.getLong());

		url.append("&g=d"); // dayly data

		url.append("&a="); // month frts
		url.append(beg_month.getLong() - 1);

		url.append("&b="); //day frst
		url.append(beg_day.getLong());

		url.append("&c="); // year frst
		url.append(beg_year.getLong());

		url.append("&ignore=.csv");

		HttpMethod method = null;
		int statusCode = -1;
		try {
			HttpClient client = new HttpClient();



			if (proxyHost_m.length() > 0) {
				client.getHostConfiguration().setProxy(proxyHost_m.toString(), proxyPort_m);
			}


			method = new GetMethod(url.toString());

			// Provide custom retry handler is necessary

			method.getParams().setParameter(HttpMethodParams.RETRY_HANDLER, new DefaultHttpMethodRetryHandler(3, false));
			method.setRequestHeader("Referer", "http://finance.yahoo.com/q/hp?s="+URLUTF8Encoder.encode(symbol));
			method.setRequestHeader("Host", "finance.yahoo.com");
			method.setRequestHeader("Accept-Language", "ru,ja;q=0.5");
			method.setRequestHeader("Accept-Encoding", "deflate");
			method.setRequestHeader("User-Agent", "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 1.1.4322; InfoPath.1; .NET CLR 2.0.50727; .NET CLR 3.0.04506.30; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022)");
			method.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");

			// Execute the method.
			statusCode = client.executeMethod(method);
			if (statusCode == HttpStatus.SC_OK) {
				InputStream is = method.getResponseBodyAsStream();
				body.append(HtUtils.convertInputStreamToString(is, null));
			}
			else {
				throw new HtException(getContext(), "downloadHistoryPrices", "HTTP response is invalid: " + statusCode);
			}





		 //HtUtils.log(Level.INFO, getContext(), "sendBeelineSms", "URL: " + method.getURI());


		} catch (Throwable e) {
			// will log only
			body.setLength(0);
			XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_ERROR, "While downloading exception: " + e.getMessage());

			//throw new HtException(getContext(), "downloadHistoryPrices", e);
		} finally {
			if (method != null) {
				method.releaseConnection();
			}
		}

		// parse body

		//http://ichart.finance.yahoo.com/table.csv?s=DELL&d=8&e=30&f=2009&g=d&a=7&b=17&c=1988&ignore=.csv

		return body.toString();
	}

	
}
