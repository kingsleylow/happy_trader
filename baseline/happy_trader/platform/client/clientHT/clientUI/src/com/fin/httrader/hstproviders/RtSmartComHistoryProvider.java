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
import com.fin.httrader.utils.hlpstruct.HtLevel1Data;
import com.fin.httrader.utils.hlpstruct.HtRtData;
import java.io.FileReader;
import java.util.*;

/**
 *
 * @author Victor_Zoubok
 */
public class RtSmartComHistoryProvider implements HtHistoryProvider {

  private long exportHourShift_m = -1;
  private long importHourShift_m = -1;
  private StringBuilder providerName_m = new StringBuilder();
  private StringBuilder pathToCSV_m = new StringBuilder();
  private boolean isDoneLevel1_m = false;
  private boolean isDoneHist_m = false;
  private String symbol_m = "";
 
  
  private long btime_m = -1;
  private long etime_m = -1;

  // ------------------------------
  public RtSmartComHistoryProvider() {
  }
  
  private String getContext()
  {
	return RtSmartComHistoryProvider.class.getSimpleName();
  }

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

  // ------------------------
  // import
  @Override
  public boolean beginDataImport(Map<String, String> initdata, long datebegin, long dateend) throws Exception {
	isDoneHist_m = false;
	isDoneLevel1_m = false;
	pathToCSV_m.setLength(0);
	pathToCSV_m.append(HtUtils.getParameterWithoutException(initdata, "IMPORT_FILE"));
	symbol_m = HtUtils.getParameterWithoutException(initdata, "SYMBOL");
	
	
	initialize();

	return true;
  }

  @Override
  public void endDataImport() {
  }

  @Override
  public int getLevel1Chunk(List<HtLevel1Data> rtdata, StringBuilder chunksymbol) throws Exception {
	if (isDoneLevel1_m) {
	  return 0;
	}

	rtdata.clear();

	chunksymbol.setLength(0);
	chunksymbol.append(symbol_m);

	CSVReader reader = new CSVReader(new FileReader(pathToCSV_m.toString()), ',');
	String[] nextLine;
	
	
			

	// assume here we have only one symbol !!!
	// as the nature of export file
	while ((nextLine = reader.readNext()) != null) {
	  String datetime_s = nextLine[0];
	  HtDateTimeUtils.parseDateTimeParameter_ToGmt(datetime_s, "yyyy.MM.dd HH:mm");
	  String rt_provider = nextLine[1].trim();
	  String symbol = nextLine[2].trim();
	


	  if (nextLine.length >= 9) {
		// must be open interest - nextLine[8]
		String oint_s = nextLine[8];
		if (oint_s != null && oint_s.length() > 0) {
		  double oint = Double.valueOf(oint_s);

		  HtLevel1Data rtdata_i = new HtLevel1Data();
		  rtdata_i.setSymbol(symbol);
		  
		  rtdata_i.time_m = HtDateTimeUtils.parseDateTimeParameter_ToGmt(datetime_s, "yyyy.MM.dd HH:mm");


		  rtdata_i.open_interest_m = oint;

		  rtdata.add(rtdata_i);

		}
	  }

	 
	}

	reader.close();

	isDoneLevel1_m = true;
	return rtdata.size();


  }

  @Override
  public int getDataChunk(List<HtRtData> rtdata, StringBuilder chunksymbol) throws Exception {
	if (isDoneHist_m) {
	  return 0;
	}

	rtdata.clear();

	chunksymbol.setLength(0);
	chunksymbol.append(symbol_m);

	CSVReader reader = new CSVReader(new FileReader(pathToCSV_m.toString()), ',');
	String[] nextLine;

	

	while ((nextLine = reader.readNext()) != null) {
	  String datetime_s = nextLine[0];
	  HtDateTimeUtils.parseDateTimeParameter_ToGmt(datetime_s, "yyyy.MM.dd HH:mm");
	  String rt_provider = nextLine[1].trim();
	  String symbol = nextLine[2].trim();

	

	  HtRtData rtdata_i = new HtRtData(HtRtData.HST_Type);
	  rtdata_i.setSymbol(symbol);
	 

	  rtdata_i.open_m = Double.valueOf(nextLine[3]);
	  rtdata_i.high_m = Double.valueOf(nextLine[4]);
	  rtdata_i.low_m = Double.valueOf(nextLine[5]);
	  rtdata_i.close_m = Double.valueOf(nextLine[6]);

	  rtdata_i.open2_m = rtdata_i.open_m;
	  rtdata_i.high2_m = rtdata_i.high_m;
	  rtdata_i.low2_m = rtdata_i.low_m;
	  rtdata_i.close2_m = rtdata_i.close_m;

	  rtdata_i.volume_m = Double.valueOf(nextLine[7]);
	  rtdata_i.time_m = HtDateTimeUtils.parseDateTimeParameter_ToGmt(datetime_s, "yyyy.MM.dd HH:mm");



	  rtdata.add(rtdata_i);
	  
	 

	}

	reader.close();

	isDoneHist_m = true;
	return rtdata.size();

  }

  // ---------------------
  // export
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

  // -------------------
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
	symbols.add(symbol_m);

  }

  @Override
  public void getSymbolMinMaxDatesToImport(String symbol, LongParam minDate, LongParam maxDate) {
	minDate.setLong(this.btime_m);
	maxDate.setLong(this.etime_m);
  }
  // --------------------
  // helpers

  private void initialize() throws Exception {

	
  }
}
