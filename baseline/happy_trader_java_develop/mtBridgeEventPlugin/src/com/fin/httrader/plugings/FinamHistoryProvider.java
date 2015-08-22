/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.plugings;

import au.com.bytecode.opencsv.CSVReader;
import com.fin.httrader.interfaces.HtHistoryProvider;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtFileUtils;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.HtSystem;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.LongParam;
import com.fin.httrader.utils.URLUTF8Encoder;
import com.fin.httrader.utils.hlpstruct.*;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Vector;
import java.util.logging.Level;

/**
 *
 * @author DanilinS
 */
public class FinamHistoryProvider implements HtHistoryProvider {

	private StringBuilder providerName_m = new StringBuilder();
	private StringBuilder pathToSymbolFile_m = new StringBuilder();
	private StringBuilder marketName_m = new StringBuilder();
	private int downloadDepthDays_m = -1;
	private long exportHourShift_m;
	private long importHourShift_m;
	private StringBuilder proxyHost_m = new StringBuilder();
	private StringBuilder wlExportPath_m = new StringBuilder();
	private int proxyPort_m = -1;
	// list of symbols
	// секция - контракт
	private HashSet<HtPair<String, String>> processingList_m = new HashSet<HtPair<String, String>>();
	private long beg_time_m = -1;
	private long end_time_m = -1;
	private int download_time_period_m = -1;

	// -----------------------------
	private String getContext() {
		return FinamHistoryProvider.class.getSimpleName();
	}

	// this functions must be called to adjust time FROM HERE!!!
	public void setImportTimeShift(long timeshift) {
		importHourShift_m = timeshift;
	}

	public void setExportTimeShift(long timeshift) {
		exportHourShift_m = timeshift;
	};

	public long getImportTimeShift() {
		return importHourShift_m;
	}

	public long getExportTimeShift() {
		return exportHourShift_m;
	}
	// -----------------------------

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
		pathToSymbolFile_m.append(HtUtils.getParameterWithException(initdata, "FINAM_SYMBOLS_FILE"));

		wlExportPath_m.setLength(0);
		wlExportPath_m.append(HtUtils.getParameterWithoutException(initdata, "WL_EXPORT_PATH"));

		downloadDepthDays_m = Integer.valueOf(HtUtils.getParameterWithException(initdata, "DEPTH_DAYS"));

		if (downloadDepthDays_m <= 0) {
			throw new HtException(getContext(), "beginDataImport", "Download depth is not valid");
		}

		proxyHost_m.setLength(0);
		proxyHost_m.append(HtUtils.getParameterWithoutException(initdata, "PROXY_HOST"));

		if (proxyHost_m.length() > 0) {
			proxyPort_m = Integer.valueOf(HtUtils.getParameterWithException(initdata, "PROXY_PORT"));
		}


		// init const
		FinamDownloadedConstants.initialize();
		
		File symbolsFile = new File(pathToSymbolFile_m.toString());

		if (symbolsFile.exists()) {

		
			// parse symbol list
			BufferedReader br = new BufferedReader(     new InputStreamReader(new FileInputStream(symbolsFile), "Cp1251"));
			
			processingList_m.clear();
			CSVReader reader = new CSVReader(br, ',');
			String[] nextLine;

			while ((nextLine = reader.readNext()) != null) {
				if (nextLine.length < 3) {
					throw new HtException(getContext(), "beginDataImport", "A symbol file row format must be: <MARKET>,<SYMBOL CODE>,<PSEUDONYM>");
				}

				HtPair<String, String> p = new HtPair<String, String>(nextLine[0], nextLine[1]);
				processingList_m.add(p);
				
				// pseudonym
				String pseudonym = nextLine[2];
				
			}

			reader.close();
		} else {

			marketName_m.setLength(0);
			marketName_m.append("Акции США");
			//marketName_m.append( HtUtils.getParameterWithException(initdata, "FINAM_MARKET_NAME") );


			
			// file with symbols does not exist
			// defaults to USA stocks
			for(Iterator<String> it = FinamDownloadedConstants.r.keySet().iterator(); it.hasNext(); ) {
				String key = it.next();

				int idx = key.indexOf('-');
				if (idx >=0) {
					String market = key.substring(0, idx );
					String symbol = key.substring(idx + 1);

					if (marketName_m.toString().equalsIgnoreCase(market)) {
						HtPair<String, String> p = new HtPair<String, String>(market, symbol);
						processingList_m.add(p);
					}

				}
			}
		}


		// time

		// end time is the maximum last time
		end_time_m = HtDateTimeUtils.getCurGmtTime_DayBegin();

		long millis_offset = (long) downloadDepthDays_m * 24 * 60 * 60 * 1000;
		beg_time_m = end_time_m - millis_offset;


		

		download_time_period_m = FinamUtils.getDownloadTimePeriodFromString(HtUtils.getParameterWithException(initdata, "TIME_PERIOD"));

		if (download_time_period_m <= 0) {
			throw new HtException(getContext(), "beginDataImport", "Invalid download time period");
		}

		return true;
	}

  @Override
	public void endDataImport() {

		FinamDownloadedConstants.erase();
		proxyHost_m.setLength(0);
		proxyPort_m = -1;
		downloadDepthDays_m = -1;
		pathToSymbolFile_m.setLength(0);
		end_time_m = -1;
		end_time_m = -1;
		download_time_period_m = -1;
	}

  @Override
	public int getDataChunk(List<HtRtData> rtdata, StringBuilder chunksymbol) throws Exception {

		rtdata.clear();


		if (processingList_m.size() > 0) {

			// get the symbol
			HtPair<String, String> symbolToProcess = processingList_m.iterator().next();

			// remove to be processed symbol
			processingList_m.remove(symbolToProcess);

			String symbol_name = FinamUtils.getSymbolName(symbolToProcess);
			String simple_symbol_name = symbolToProcess.second;
			String market_name = symbolToProcess.first;

			chunksymbol.setLength(0);
			//chunksymbol.append(symbol_name);
			chunksymbol.append(simple_symbol_name);


			try {


				// get market and emitent id

				HtPair<Integer, Integer> p = FinamDownloadedConstants.r.get(symbol_name);
				if (p == null) {
					XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_INFO, "For symbol: '" + symbol_name + "' cannot find market and emitent IDs, ignoring...");

					// out
					return processingList_m.size();

				}

				int market_id = p.first, emitent_id = p.second;

				// before make sure we don't have temp storage
				// as end_time_m - this is always the current data but we need updated caches always
				// end_time - must be current time here
				String baseDir = FinamUtils.getBaseCacheDir(beg_time_m, HtDateTimeUtils.getCurGmtTime());


				StringBuilder cacheFileName = new StringBuilder(baseDir);
				cacheFileName.append(File.separatorChar);
				cacheFileName.append(URLUTF8Encoder.encode(market_name));
				cacheFileName.append(File.separatorChar);

				cacheFileName.append(URLUTF8Encoder.encode(simple_symbol_name));
				cacheFileName.append(".txt");

				File cacheFile = new File(cacheFileName.toString());
				if (!cacheFile.exists()) {

					// create dir
					cacheFile.getParentFile().mkdirs();

					boolean result = FinamUtils.downloadHistoryPrices(
									cacheFileName.toString(),
									getProviderId(),
									symbol_name,
									simple_symbol_name,
									market_id,
									emitent_id,
									download_time_period_m,
									proxyHost_m.toString(),
									proxyPort_m,
									downloadDepthDays_m,
									beg_time_m,
									end_time_m);

					if (!result) {
						XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_INFO, "For symbol: '" + symbol_name + "' is bad content, ignoring...");
						// out
						return processingList_m.size();
					}


				} else {
					XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_INFO, "For symbol: '" + symbol_name + "' reusing cache file");

				}

				// now cache file must exist - must read it
				FinamUtils.processTextFile(getProviderId(), simple_symbol_name, rtdata, cacheFile);

				// if file was ok - need to copy
				if (wlExportPath_m.length() > 0) {
					HtFileUtils.copyFile(cacheFileName.toString(), wlExportPath_m.toString() + File.separatorChar + simple_symbol_name + ".txt");
				}


			} catch (Throwable e) {
				// delegate further
				XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_ERROR, "Exception on processing the symbol: '" + symbol_name +
								"' - " + e.getMessage());

				throw new HtException(getContext(), "getDataChunk", e);
			}


			// need to process the last
			return (processingList_m.size() + 1);
		} else {
			return 0;
		}

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
		for (Iterator<HtPair<String, String>> it = processingList_m.iterator(); it.hasNext();) {
			HtPair<String, String> p = it.next();

			// секция - контракт

			symbols.add(FinamUtils.getSymbolName(p));
		}


	}

  @Override
	public void getSymbolMinMaxDatesToImport(String symbol, LongParam minDate, LongParam maxDate) {
		// current dats

		maxDate.setLong(end_time_m);
		minDate.setLong(beg_time_m);


	}



  @Override
  public int getLevel1Chunk(List<HtLevel1Data> rtdata, StringBuilder chunksymbol) throws Exception {
	return 0;
  }

	
}
