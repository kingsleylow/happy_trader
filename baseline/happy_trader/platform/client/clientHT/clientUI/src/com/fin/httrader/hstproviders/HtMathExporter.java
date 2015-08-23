/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.hstproviders;

import au.com.bytecode.opencsv.CSVReader;

import com.fin.httrader.interfaces.HtHistoryProvider;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.LongParam;
import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.utils.hlpstruct.HtLevel1Data;
import com.fin.httrader.utils.hlpstruct.HtRtData;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.nio.LongBuffer;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.ListIterator;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
import java.util.TreeSet;
import java.util.Vector;

/**
 *
 * @author DanilinS
 * This class will initially send
 */
public class HtMathExporter implements HtHistoryProvider {

	private long exportTimeShift_m = 0;
	private StringBuilder providerId_m = new StringBuilder();

	// output file
	private StringBuilder pathToOutput_m = new StringBuilder();

	// our database can contain a lot of symbols so we may filter out what we need
	private StringBuilder inputSymbolFile_m = new StringBuilder();

	// symbols read from the input file
	private Set<String> inputSymbols_m = new TreeSet<String>();

	// list containing correlators
	private List<String> correlList_m = new ArrayList<String>();

	
	// data begin and end
	private long dataBegin_m = -1;
	private long dataEnd_m = -1;
	private int correlationDepth_m = -1;

	// though this is not economical
	// but what we can do here
	private Map<String, List<HtRtData>> allData_m = new TreeMap<String, List<HtRtData>>();

	// ------------------------
	private String getContext() {
		return this.getClass().getSimpleName();
	}

	// -------------------------
	// export
	public void setExportTimeShift(long timeshift) {
		exportTimeShift_m = timeshift;
	}

	public long getExportTimeShift() {
		return exportTimeShift_m;
	}

	public boolean beginDataExport(Map<String, String> initdata, long datebegin, long dateend) throws Exception {

		dataBegin_m = datebegin;
		dataEnd_m = dateend;


		pathToOutput_m.setLength(0);
		pathToOutput_m.append(HtUtils.getParameterWithException(initdata, "OUTPUT_FILE"));

		// if set this passes input symbol list and operates only with that
		inputSymbolFile_m.setLength(0);
		inputSymbolFile_m.append(HtUtils.getParameterWithoutException(initdata, "INPUT_SYMBOLS_FILE"));

		

		inputSymbols_m.clear();
		CSVReader reader = new CSVReader(new FileReader(inputSymbolFile_m.toString()), ',');
		String[] nextLine;

		while ((nextLine = reader.readNext()) != null) {
			inputSymbols_m.add( nextLine[0] );
		}

		reader.close();

		

		// correlation depth
		correlationDepth_m = -1;
		String cdepthS = HtUtils.getParameterWithoutException(initdata, "CORRELATION_DEPTH");
		if (cdepthS.length() > 0) {
			correlationDepth_m = Integer.valueOf(cdepthS);
		}


		//
		HtUtils.<String>addArrayToCollection(correlList_m, HtUtils.getParameterWithException(initdata, "CORRELATORS").split(";"));

		XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_INFO, "Correlation depth: " + correlationDepth_m);

		allData_m.clear();

		return true;
	}

	public void setDataChunk(List<HtRtData> rtdata, String chunksymbol) throws Exception {
		// store data chunks here

		for (int i = 0; i < rtdata.size(); i++) {
			HtRtData data_i = rtdata.get(i);
			String symbol_i = data_i.getSymbol();

			// if there is a file filter
			if (inputSymbolFile_m.length() > 0) {

				// skip that symbol if does not exist
				if (!inputSymbols_m.contains( symbol_i ))
					continue;
			}

			if (dataBegin_m > 0 && data_i.time_m < dataBegin_m) {
				continue;
			}

			if (dataEnd_m > 0 && data_i.time_m > dataEnd_m) {
				continue;
			}

			List<HtRtData> datalist = null;
			if (allData_m.containsKey(symbol_i)) {
				datalist = allData_m.get(symbol_i);
			} else {
				datalist = new ArrayList<HtRtData>();
				allData_m.put(symbol_i, datalist);
			}
			//
			datalist.add(data_i);

		}

	}

	public void endDataExport() {

		// after we populated data perfrom export
		try {
			performActualExport();
		} catch (Throwable e) {
			XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_ERROR, "Exception on exporting data: " + e.getMessage());
		}

		// N/A
		allData_m.clear();
		correlList_m.clear();
		inputSymbolFile_m.setLength(0);
		pathToOutput_m.setLength(0);

		inputSymbols_m.clear();

	}

	

	// ------------------------
	// import - not available
	public void setImportTimeShift(long timeshift) {
		// N/A
	}

	public long getImportTimeShift() {
		return 0;
	}

	public boolean beginDataImport(Map<String, String> initdata, long datebegin, long dateend) throws Exception {
		throw new UnsupportedOperationException("Not supported yet.");
	}

	public void endDataImport() {
		throw new UnsupportedOperationException("Not supported yet.");
	}

	public int getDataChunk(List<HtRtData> rtdata, StringBuilder chunksymbol) throws Exception {
		throw new UnsupportedOperationException("Not supported yet.");
	}

	

	public void getAvailableSymbolsToImport(Vector<String> symbols) {
		throw new UnsupportedOperationException("Not supported yet.");
	}

	public void getSymbolMinMaxDatesToImport(String symbol, LongParam minDate, LongParam maxDate) {
		throw new UnsupportedOperationException("Not supported yet.");
	}

	// --------------------------
	public String getProviderId() {
		return providerId_m.toString();
	}

	public void setProviderId(String providerId) {
		providerId_m.setLength(0);
		providerId_m.append(providerId);
	}

	// -------------------------
	// HELPERS
	void performActualExport() throws Exception {
		BufferedWriter writer = null;
		try {
			File f = new File(pathToOutput_m.toString());

			if (f.exists()) {
				f.delete();
			}

			writer = new BufferedWriter(new FileWriter(f));

			List<double[]> corrlsts = new ArrayList<double[]>();

			// write headers
			writer.write("SYMBOL,");
			for (int i = 0; i < correlList_m.size(); i++) {
				String corrSymbol_i = correlList_m.get(i);
				if (!allData_m.containsKey(corrSymbol_i)) {
					throw new HtException(getContext(), "performActualExport", "Symbol: " + corrSymbol_i + " not found");
				}

				List<HtRtData> correlatorlist_i = allData_m.get(corrSymbol_i);

				double[] arr_i = preparePrices(corrSymbol_i, correlatorlist_i, correlationDepth_m, null, null);
				corrlsts.add(arr_i);

				writer.write(corrSymbol_i);
				writer.write(",");
			}

			writer.write("TIME DATA BEGIN");
			writer.write(",");
			writer.write("TIME DATA END");
			writer.write(",");
			writer.write("RELATIVE CHANGE VALUE");
			writer.write(",");
			writer.write("VOLUME BEGIN");
			writer.write(",");
			writer.write("VOLUME END");

			writer.newLine();



			// claculate
			for (Iterator<String> it = allData_m.keySet().iterator(); it.hasNext();) {
				String symbol_i = it.next();

				// symbol list
				List<HtRtData> datalist_i = allData_m.get(symbol_i);

				// process symbol
				try {
					if (datalist_i.size() < 2) {
						throw new HtException(getContext(), "performActualExport", "Size of price history cannot be less than 2");
					}

					writer.write(symbol_i);
					writer.write(",");

					// our bid prices
					LongParam firsttime = new LongParam();
					LongParam lasttime = new LongParam();

					double[] dataarr = preparePrices(symbol_i, datalist_i, correlationDepth_m, firsttime, lasttime);

					// now proceed through correlators
					for (int i = 0; i < corrlsts.size(); i++) {
						double[] corr_i = corrlsts.get(i);

						if (corr_i.length != dataarr.length) {
							throw new HtException(getContext(), "performActualExport", "Data arrays of different sizes");
						}

						double result = getPearsonCorrelation(dataarr, corr_i);
						writer.write(HtUtils.formatPriceValue(result, 4, false));
						writer.write(",");

					}

					writer.write(HtDateTimeUtils.time2SimpleString_Gmt(firsttime.getLong()));
					writer.write(",");

					writer.write(HtDateTimeUtils.time2SimpleString_Gmt(lasttime.getLong()));
					writer.write(",");

					// now we are going to calculate relative change
					HtRtData lastd = datalist_i.get(datalist_i.size() - 1);
					HtRtData plastd = datalist_i.get(datalist_i.size() - 2);

					double relative_change = (lastd.high2_m - lastd.low2_m) / (plastd.high_m - plastd.low_m);
					writer.write(HtUtils.formatPriceValue(relative_change, 4, false));
					writer.write(",");
					writer.write(HtUtils.formatPriceValue(plastd.volume_m, 4, false));

					writer.write(",");
					writer.write(HtUtils.formatPriceValue(lastd.volume_m, 4, false));
					
					writer.write(",");

					// new line
					writer.newLine();
				} catch (Throwable e) {
					XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_ERROR, "Exception on exporting data: " + e.getMessage() + " , ignoring..." );
				}

			}

		} catch (Throwable e) {
			throw new HtException(getContext(), "performActualExport", "On export exception occured: " + e.getMessage());
		} finally {
			if (writer != null) {
				writer.flush();
				writer.close();
			}
		}
	}

	// if correlationDepth is negative or < 2 - just put all data
	private double[] preparePrices(String symbol, List<HtRtData> rtdatalist, int correlationDepth, LongParam firsttime, LongParam lasttime) throws Exception {
		double[] result = null;

		if (correlationDepth >= 2) {
			if (rtdatalist.size() >= correlationDepth) {
				result = new double[correlationDepth];
			} else {
				throw new HtException(getContext(), "preparePrices",
								"The correlation depth is: " + correlationDepth +
								" which is more than data size: " + rtdatalist.size() +
								" for symbol: " + symbol);
			}
		} else {
			result = new double[rtdatalist.size()];
		}

		int cnt = 0;
		// for the last
		for (ListIterator<HtRtData> p = rtdatalist.listIterator(rtdatalist.size()); p.hasPrevious();) {
			HtRtData htrtdata_i = p.previous();

			if (cnt == 0) {
				if (lasttime != null) {
					lasttime.setLong(htrtdata_i.time_m);
				}
			}

			if (correlationDepth >= 2) {
				if (correlationDepth > cnt) {
					result[cnt] = htrtdata_i.close2_m;
				} else {
					if (firsttime != null) {
						firsttime.setLong(htrtdata_i.time_m);
					}

					break;
				}
			} else {
				result[cnt] = htrtdata_i.close2_m;
				;
			}

			// advance
			cnt++;

		}


		return result;
	}

	private double getPearsonCorrelation(double[] scores1, double[] scores2) throws Exception {

		if (scores1.length < 2 || scores2.length < 2) {
			throw new HtException(getContext(), "getPearsonCorrelation", "The size of data arrays is less than 2");
		}

		double result = 0;
		double sum_sq_x = 0;
		double sum_sq_y = 0;
		double sum_coproduct = 0;
		double mean_x = scores1[0];
		double mean_y = scores2[0];
		for (int i = 2; i < scores1.length + 1; i += 1) {
			double sweep = Double.valueOf(i - 1) / i;
			double delta_x = scores1[i - 1] - mean_x;
			double delta_y = scores2[i - 1] - mean_y;
			sum_sq_x += delta_x * delta_x * sweep;
			sum_sq_y += delta_y * delta_y * sweep;
			sum_coproduct += delta_x * delta_y * sweep;
			mean_x += delta_x / i;
			mean_y += delta_y / i;
		}
		double pop_sd_x = (double) Math.sqrt(sum_sq_x / scores1.length);
		double pop_sd_y = (double) Math.sqrt(sum_sq_y / scores1.length);
		double cov_x_y = sum_coproduct / scores1.length;
		result = cov_x_y / (pop_sd_x * pop_sd_y);
		return result;
	}

  @Override
  public int getLevel1Chunk(List<HtLevel1Data> rtdata, StringBuilder chunksymbol) throws Exception {
	return 0;
  }

	
}
