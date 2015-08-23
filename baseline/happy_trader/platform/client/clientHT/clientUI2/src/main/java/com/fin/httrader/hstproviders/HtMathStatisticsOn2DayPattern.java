/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.hstproviders;

import com.fin.httrader.interfaces.HtHistoryProvider;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtMathUtils;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.LongParam;
import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.utils.hlpstruct.HtLevel1Data;
import com.fin.httrader.utils.hlpstruct.HtRtData;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.ListIterator;
import java.util.Map;
import java.util.Map;
import java.util.TreeMap;
import java.util.Vector;

/**
 *
 * @author DanilinS
 *
 */
public class HtMathStatisticsOn2DayPattern implements HtHistoryProvider {

  @Override
  public int getLevel1Chunk(List<HtLevel1Data> rtdata, StringBuilder chunksymbol) throws Exception {
	return 0;
  }

	private class SymbolStatistics {

		public List<Double> result_day_changes = new ArrayList<Double>();
		public List<Double> result_day_go_ups = new ArrayList<Double>();
		public List<Double> result_day_go_downs = new ArrayList<Double>();

		public double  result_day_changes_avr = -1;
		public double  result_day_go_ups_avr = -1;
		public double  result_day_go_downs_avr = -1;

		// total days reviewed
		public int total_days_m = -1;

	};

	// ----------------
	private long exportTimeShift_m = 0;
	private StringBuilder providerId_m = new StringBuilder();

	// output file
	private StringBuilder pathToOutput_m = new StringBuilder();
	private double thresholdRatio_m = -1;

	// data begin and end
	private long dataBegin_m = -1;
	private long dataEnd_m = -1;

	// though this is not economical
	// but what we can do here
	private Map<String, List<HtRtData>> allData_m = new TreeMap<String, List<HtRtData>>();
	private Map<String, SymbolStatistics> statistics_m = new TreeMap<String, SymbolStatistics>();

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

		// correlation depth

		// correlation depth
		thresholdRatio_m = -1;
		String cdepthS = HtUtils.getParameterWithoutException(initdata, "THRESHOLD_RATIO");
		if (cdepthS.length() > 0) {
			thresholdRatio_m = Double.valueOf(cdepthS);
		}



		allData_m.clear();
		statistics_m.clear();

		return true;
	}

	public void setDataChunk(List<HtRtData> rtdata, String chunksymbol) throws Exception {
		// store data chunks here

		for (int i = 0; i < rtdata.size(); i++) {
			HtRtData data_i = rtdata.get(i);
			String symbol_i = data_i.getSymbol();

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
			performActualCalculation();
		} catch (Throwable e) {
			XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_ERROR, "Exception on exporting data: " + e.getMessage());
		}

		// N/A
		allData_m.clear();
		statistics_m.clear();



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
	void performActualCalculation() throws Exception {
		BufferedWriter writer = null;
		try {
			File f = new File(pathToOutput_m.toString());

			if (f.exists()) {
				f.delete();
			}

			writer = new BufferedWriter(new FileWriter(f));
			writer.write("symbol,total days,total condition matches, third average day change,third average up move,third average down move");
			writer.newLine();


			// claculate
			for (Iterator<String> it = allData_m.keySet().iterator(); it.hasNext();) {
				String symbol_i = it.next();

				// symbol list
				List<HtRtData> datalist_i = allData_m.get(symbol_i);

				// process symbol
				try {
					if (datalist_i.size() < 3) {
						throw new HtException(getContext(), "performActualCalculation", "Size of price history cannot be less than 2");
					}

					makeStatisticsOnSymbol(symbol_i, datalist_i);
					reportSymbol(symbol_i, writer);


				} catch (Throwable e) {
					XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_ERROR, "Exception on exporting data: " + e.getMessage() + " , ignoring...");
				}

			}

		} catch (Throwable e) {
			throw new HtException(getContext(), "performActualCalculation", "On calculations exception occured: " + e.getMessage());
		} finally {
			if (writer != null) {
				writer.flush();
				writer.close();
			}
		}
	}

	// ---------------
	// helpers
	private void makeStatisticsOnSymbol(String symbol, List<HtRtData> datalist) throws Exception {
		HtRtData day0 = null;
		HtRtData day1 = null;
		HtRtData day2 = null;

		for (int i = 2; i < datalist.size(); i++) {
			HtRtData data_i = datalist.get(i);

			// 0 and first days - solution days
			day2 = datalist.get(i);
			day1 = datalist.get(i - 1);
			day0 = datalist.get(i - 2);

			double day0_range = day0.high2_m - day0.low2_m;
			double day1_range = day1.high2_m - day1.low2_m;
			double day2_range = day2.high2_m - day2.low2_m;

			// resulting relative change compare to the first day
			double relative_change1 = day2_range / day0_range;
			double relative_change0 = day1_range / day0_range;

			// result
			// here we measure relatively to the first day
			double relative_up_go2 = Math.abs(day2.close2_m - day1.high2_m) / day0_range;
			double relative_down_go2 = Math.abs(day2.close2_m - day1.low2_m) / day0_range;

			if (relative_change0 < thresholdRatio_m) {
				// our condition
				
				SymbolStatistics symb_stat = null;
				if (statistics_m.containsKey(symbol)) {
					symb_stat =  statistics_m.get(symbol);
				} else {
					symb_stat = new SymbolStatistics();
					statistics_m.put(symbol, symb_stat);
				}

				symb_stat.total_days_m = datalist.size();
				//
				symb_stat.result_day_changes.add(relative_change1);
				symb_stat.result_day_go_downs.add(relative_down_go2);
				symb_stat.result_day_go_ups.add(relative_up_go2);

				// claculate averages
				symb_stat.result_day_changes_avr = HtMathUtils.calculateAverage(symb_stat.result_day_changes);
				symb_stat.result_day_go_downs_avr = HtMathUtils.calculateAverage(symb_stat.result_day_go_downs);
				symb_stat.result_day_go_ups_avr = HtMathUtils.calculateAverage(symb_stat.result_day_go_ups);


			}
		}
	}

	private void reportSymbol(String symbol, BufferedWriter writer) throws Exception
	{
		SymbolStatistics symb_stat = statistics_m.get(symbol);
		if (symb_stat==null)
			// no resulst
			return;

		writer.write(symbol);
		writer.write(",");
		writer.write( String.valueOf( symb_stat.total_days_m) );
		writer.write(",");
		writer.write(String.valueOf( symb_stat.result_day_changes.size() ));
		writer.write(",");
		writer.write(HtUtils.formatPriceValue(symb_stat.result_day_changes_avr,4,false));
		writer.write(",");
		writer.write(HtUtils.formatPriceValue(symb_stat.result_day_go_downs_avr,4,false));
		writer.write(",");
		writer.write(HtUtils.formatPriceValue(symb_stat.result_day_go_ups_avr,4,false));

		writer.newLine();
	}
	
	
}
