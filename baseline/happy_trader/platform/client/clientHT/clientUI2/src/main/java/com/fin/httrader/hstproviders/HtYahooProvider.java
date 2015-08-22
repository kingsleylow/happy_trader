/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.hstproviders;

import au.com.bytecode.opencsv.CSVReader;

import com.fin.httrader.interfaces.HtHistoryProvider;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtFileUtils;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.HtSystem;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.LongParam;
import com.fin.httrader.utils.URLUTF8Encoder;
import com.fin.httrader.utils.hlpstruct.HtLevel1Data;
import com.fin.httrader.utils.hlpstruct.HtRtData;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.InputStream;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.TimeZone;
import java.util.Vector;
import org.apache.http.HttpEntity;
import org.apache.http.HttpHost;
import org.apache.http.HttpResponse;
import org.apache.http.HttpStatus;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.conn.params.ConnRoutePNames;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.params.HttpConnectionParams;
import org.apache.http.params.HttpParams;

/**
 *
 * @author DanilinS
 */
public class HtYahooProvider implements HtHistoryProvider {

	private StringBuilder providerName_m = new StringBuilder();
	private StringBuilder pathToSymbolFile_m = new StringBuilder();
	private int connectTimeOutSec_m = 10;
	private int socketTimeOutSec_m = 10;
	private int downloadDepthDays_m = -1;
	private long exportHourShift_m;
	private long importHourShift_m;
	private StringBuilder proxyHost_m = new StringBuilder();
	private int proxyPort_m = -1;
	//private Vector<String> symbolList_m = new Vector<String>();
	private HashSet<String> processingList_m = new HashSet<String>();
	private long beg_time_m = -1;
	private long end_time_m = -1;
	private BufferedWriter symbolLog_m = null;

	private StringBuilder pathToCSV_m = new StringBuilder();

	// -------------------------
	private String getContext() {
		return this.getClass().getSimpleName();
	}

	// -------------------------
	// this functions must be called to adjust time FROM HERE!!!
	@Override
	public void setImportTimeShift(long timeshift) {
		importHourShift_m = timeshift;
	}

	@Override
	public void setExportTimeShift(long timeshift) {
		exportHourShift_m = timeshift;
	}

	@Override
	public long getImportTimeShift() {
		return importHourShift_m;
	}

	@Override
	public long getExportTimeShift() {
		return exportHourShift_m;
	}

	// ---------------------------
	// export is not supported
	@Override
	public boolean beginDataExport(Map<String, String> initdata, long datebegin, long dateend) throws Exception {
		return false;
	}

	@Override
	public void setDataChunk(List<HtRtData> rtdata, String chunksymbol) throws Exception {
		throw new UnsupportedOperationException("Not supported yet.");
	}

	@Override
	public void endDataExport() {
		throw new UnsupportedOperationException("Not supported yet.");
	}

	

	// --------------------------
	@Override
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

		pathToCSV_m.setLength(0);
		pathToCSV_m.append(HtUtils.getParameterWithoutException(initdata, "CSV_EXPORT_PATH"));

		if (pathToCSV_m.length() > 0) {
			File f1 = new File(pathToCSV_m.toString());
			f1.mkdirs();
		}


		if (proxyHost_m.length() > 0) {
			proxyPort_m = Integer.valueOf(HtUtils.getParameterWithException(initdata, "PROXY_PORT"));
		}

		// parse symbol list
		processingList_m.clear();
		processingList_m.addAll(  HtFileUtils.readSymbolListFromFile(pathToSymbolFile_m.toString()) );
		


		// time
		end_time_m = HtDateTimeUtils.getCurGmtTime_DayBegin();

		long millis_offset = (long)downloadDepthDays_m * 24 * 60 * 60 * 1000;
		beg_time_m = end_time_m - millis_offset;

		
		this.connectTimeOutSec_m = HtUtils.parseInt(HtUtils.getParameterWithoutException(initdata, "HTTP_TIMEOUT_SEC"));
		this.socketTimeOutSec_m = HtUtils.parseInt(HtUtils.getParameterWithoutException(initdata, "HTTP_TIMEOUT_SEC"));
	
		return true;
	}

	@Override
	public void endDataImport() {
	
		proxyHost_m.setLength(0);
		proxyPort_m = -1;
		downloadDepthDays_m = -1;
		pathToSymbolFile_m.setLength(0);
		end_time_m = -1;
		end_time_m = -1;

		if (symbolLog_m != null) {
			try {
				symbolLog_m.flush();
				symbolLog_m.close();
			}
			catch(Throwable e)
			{
				XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_ERROR, "Cannot write log file: " + e.getMessage() );
			}
		}
	}

	@Override
	public int getDataChunk(List<HtRtData> rtdata, StringBuilder chunksymbol) throws Exception {

		rtdata.clear();

		List<HtRtData> rtdatabuf=new ArrayList<HtRtData>();

		if (processingList_m.size() >0) {

			// get the symbol
			String symbolToProcess = processingList_m.iterator().next();

			// remove to be processed symbol
			processingList_m.remove(symbolToProcess);

			chunksymbol.setLength(0);
			chunksymbol.append(symbolToProcess);


			try {
				// before make sure we don't have temp storage

				String encodedSymbol = URLUTF8Encoder.encode(symbolToProcess);

			  StringBuilder fileName = new StringBuilder(HtSystem.getSystemTempPath());
				fileName.append(File.separatorChar);
				fileName.append( "yahoo_history_provider" );
				fileName.append(File.separatorChar);

				fileName.append(beg_time_m);
				fileName.append("_");
				fileName.append(end_time_m);

				// log
				if (symbolLog_m == null) {
					File f = new File(fileName.toString() + File.separatorChar + "work.log");
					f.getParentFile().mkdirs();
					symbolLog_m = new BufferedWriter(new FileWriter(f));
				}

				fileName.append(File.separatorChar);
				fileName.append( getPathPiece(encodedSymbol) );
				fileName.append(File.separatorChar);
				
				File dir = new File(fileName.toString());
				dir.mkdirs();
				
				fileName.append(encodedSymbol);
				
				fileName.append("_.csv");

				File cacheFile=new File(fileName.toString());
				if (!cacheFile.exists()) {
					// time today
					String body = downloadHistoryPrices(symbolToProcess, proxyHost_m.toString(),proxyPort_m, downloadDepthDays_m, beg_time_m, end_time_m);

					if (body.length()==0) {
							XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_INFO, "For symbol: '" + symbolToProcess + "' is empty content, ignoring..." );

							symbolLog_m.write("Error: " + symbolToProcess+ "\r\n" );
							symbolLog_m.flush();
							// out
							return processingList_m.size();
					}
					else {

						if ( !HtFileUtils.writeContentToFile(cacheFile, body)) {
							//throw new HtException(getContext(), "getDataChunk", "Can't write to cache file: " + fileName);
							XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_ERROR, "Can't write to cache file: " + fileName );
						}

						
					}
					
				} else {
					XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_DEBUGDETAIL, "For symbol: '" + symbolToProcess + "' reusing cache file" );

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
				
				for(int i = 0; i < rtdatabuf.size(); i++)	{
					rtdata.add( rtdatabuf.get(rtdatabuf.size()-i-1) );

				}

				// export

				if (pathToCSV_m.length() > 0 ) {
					StringBuilder exportContent = new StringBuilder();
					
					SimpleDateFormat exportDataGmtFormat = new SimpleDateFormat("yyyyMMdd,HHmmss");
					exportDataGmtFormat.setTimeZone(TimeZone.getTimeZone("GMT"));

					for(int i = 0; i < rtdata.size(); i++) {
						HtRtData rtdata_i = rtdata.get(i);
						exportContent.append(exportDataGmtFormat.format(new Date(rtdata_i.time_m)));
						exportContent.append(",");
						exportContent.append(HtUtils.formatPriceValue(rtdata_i.open_m, 4, true));
						exportContent.append(",");
						exportContent.append(HtUtils.formatPriceValue(rtdata_i.high_m, 4, true));
						exportContent.append(",");
						exportContent.append(HtUtils.formatPriceValue(rtdata_i.low_m, 4, true));
						exportContent.append(",");
						exportContent.append(HtUtils.formatPriceValue(rtdata_i.close_m, 4, true));
						exportContent.append(",");
						exportContent.append(HtUtils.formatPriceValue(rtdata_i.volume_m, 4, true));
						exportContent.append("\n");
					}

					String exportSymbolFile = pathToCSV_m.toString() + File.separatorChar + encodedSymbol + "_.csv";
					
					if ( !HtFileUtils.writeContentToFile(exportSymbolFile, exportContent.toString())) {
						throw new HtException(getContext(), "getDataChunk", "Can't write data to explicitely defined export file: " + exportSymbolFile);
					}
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

	



	@Override
	public String getProviderId() {
		return providerName_m.toString();
	}

	@Override
	public void setProviderId(String providerId) {
		providerName_m.setLength(0);
		providerName_m.append(providerId);
	}

	@Override
	public void getAvailableSymbolsToImport(Vector<String> symbols) {
		symbols.clear();
		symbols.addAll( processingList_m );

	}

	@Override
	public void getSymbolMinMaxDatesToImport(String symbol, LongParam minDate, LongParam maxDate) {
		// current dats

		maxDate.setLong(this.end_time_m);
		minDate.setLong(this.beg_time_m);

	}

	// ------------------------------
	// helpers

	// create a special piece of path based on the symbol
	private String getPathPiece(String encodedSymbol)
	{
		StringBuilder out = new StringBuilder();

		if (encodedSymbol!= null) {
			if (encodedSymbol.length() >=1) {
				out.append(encodedSymbol.charAt(0));
				
				if (encodedSymbol.length() >=2) {
					out.append(encodedSymbol.charAt(1));
					
				}
			}
		}

		return out.toString();
	}

	private String downloadHistoryPrices(String symbol, String proxy_host, int proxy_port, int days_depth, long beg_time, long end_time) throws Exception {

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


		HttpClient httpclient = new DefaultHttpClient();
		HttpParams params = httpclient.getParams();
		
		// set timeout
		if (connectTimeOutSec_m > 0)
			HttpConnectionParams.setConnectionTimeout(params, this.connectTimeOutSec_m * 1000);
		
		if (socketTimeOutSec_m > 0)
			HttpConnectionParams.setSoTimeout(params, this.socketTimeOutSec_m * 1000);


		HttpGet httpget = new HttpGet(url.toString());


		try {



			if (proxy_host.length() > 0) {
				HttpHost proxy = new HttpHost(proxy_host.toString(), proxy_port);
				httpclient.getParams().setParameter(ConnRoutePNames.DEFAULT_PROXY, proxy);
			}


			

			// Provide custom retry handler is necessary

			//method.getParams().setParameter(HttpMethodParams.RETRY_HANDLER, new DefaultHttpMethodRetryHandler(3, false));
			httpget.setHeader("Referer", "http://finance.yahoo.com/q/hp?s="+URLUTF8Encoder.encode(symbol));
			httpget.setHeader("Host", "finance.yahoo.com");
			httpget.setHeader("Accept-Language", "ru,ja;q=0.5");
			httpget.setHeader("Accept-Encoding", "deflate");
			httpget.setHeader("User-Agent", "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 1.1.4322; InfoPath.1; .NET CLR 2.0.50727; .NET CLR 3.0.04506.30; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022)");
			httpget.setHeader("Content-Type", "application/x-www-form-urlencoded");

			// Execute the method.

			HttpResponse response = httpclient.execute(httpget);

			if (response.getStatusLine().getStatusCode() == HttpStatus.SC_OK) {
				HttpEntity entity = response.getEntity();
				if (entity != null) {
					InputStream is = entity.getContent();
					body.append(HtUtils.convertInputStreamToString(is, null));
				}
			}
			else {
				throw new HtException(getContext(), "downloadHistoryPrices", "HTTP response is invalid: " + response.getStatusLine().getStatusCode() + " - " +response.getStatusLine().getReasonPhrase());
			}


		 //HtUtils.log(Level.INFO, getContext(), "sendBeelineSms", "URL: " + method.getURI());


		} catch (Throwable e) {
			// will log only
			body.setLength(0);
			XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_ERROR, "While downloading exception: " + e.getMessage());

			//throw new HtException(getContext(), "downloadHistoryPrices", e);
		}

		XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_DEBUG, "Provider downloaded for symbol " + symbol+ " " + body.toString().length()+" bytes");


		// parse body

		//http://ichart.finance.yahoo.com/table.csv?s=DELL&d=8&e=30&f=2009&g=d&a=7&b=17&c=1988&ignore=.csv

		return body.toString();
	}

  @Override
  public int getLevel1Chunk(List<HtLevel1Data> rtdata, StringBuilder chunksymbol) throws Exception {
	return 0;
  }
}
