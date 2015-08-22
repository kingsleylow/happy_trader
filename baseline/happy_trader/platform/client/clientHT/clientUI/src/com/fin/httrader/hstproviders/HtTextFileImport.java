/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.hstproviders;

import au.com.bytecode.opencsv.CSVReader;
import com.fin.httrader.interfaces.HtHistoryProvider;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtFileUtils;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.LongParam;
import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.utils.hlpstruct.HtLevel1Data;
import com.fin.httrader.utils.hlpstruct.HtRtData;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import java.io.File;
import java.io.FileReader;
import java.util.Calendar;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Vector;
import java.util.logging.Level;

/**
 *
 * @author DanilinS
 * Privides import from a text files
 * from a list of directory
 */
public class HtTextFileImport implements HtHistoryProvider {

	private StringBuilder providerName_m = new StringBuilder();

	private long exportHourShift_m = -1;

	private long importHourShift_m = -1;

	private boolean skipFirstLine_m = true;

	// directory for import
	private StringBuilder exportFileDir_m = new StringBuilder();

	
	

	// list of files to be processed
	private HashMap<String, File> filesToProcess_m = new HashMap<String, File>();



	// --------------------

	private String getContext() {
		return this.getClass().getSimpleName();
	}


	// --------------------
	

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

	


	public boolean beginDataImport(Map<String, String> initdata, long datebegin, long dateend) throws Exception {

		// init the file with the list of symbols
		exportFileDir_m.setLength(0);
		exportFileDir_m.append(HtUtils.getParameterWithException(initdata, "IMPORT_PATH"));
		skipFirstLine_m = HtUtils.getParameterWithoutException(initdata, "SKIP_FIRST_LINE").equalsIgnoreCase("true");

	
		filesToProcess_m.clear();

		// go through directory and determine symbols
		File f = new File(exportFileDir_m.toString());

		if (!f.isDirectory())
			throw new HtException(getContext(), "beginDataImport", exportFileDir_m + " is not a directory");

		HashSet<File> files = new HashSet<File>();
		HtFileUtils.readAllfilesRecursively(null, exportFileDir_m.toString(), files );

		// create
		for(Iterator<File> it = files.iterator(); it.hasNext(); ) {
			File fi = it.next();

			filesToProcess_m.put( HtFileUtils.removeExtention(fi.getName()).toUpperCase(), fi );
		}
		
		
		return true;

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

		symbols.addAll( filesToProcess_m.keySet());
	}

	public void getSymbolMinMaxDatesToImport(String symbol, LongParam minDate, LongParam maxDate) {
		maxDate.setLong(-1);
		minDate.setLong(-1);
	}

	public void endDataImport() {
		
		filesToProcess_m.clear();
		exportFileDir_m.setLength( 0 );

	}

	public int getDataChunk(List<HtRtData> rtdata, StringBuilder chunksymbol) throws Exception {
		if (filesToProcess_m.size() >0) {

			String symbolToProcess = filesToProcess_m.keySet().iterator().next();
			File fi = filesToProcess_m.get(symbolToProcess);
			filesToProcess_m.remove(symbolToProcess);

			chunksymbol.setLength(0);
			chunksymbol.append(symbolToProcess);

			parseFile(rtdata, fi, symbolToProcess );
			HtLog.log(Level.INFO, getContext(), "getDataChunk", "Parsed file - symbol: " + symbolToProcess + " number of rows: " +rtdata.size() + " left symbols: " + filesToProcess_m.size());
			
			
			return (filesToProcess_m.size() + 1);
		}
		else
			// finish
			return 0;
	}

	

	//
	// ---------------------------
	// export is not supported
	public boolean beginDataExport(Map<String, String> initdata, long datebegin, long dateend) throws Exception {
		return false;
	}

	public void setDataChunk(List<HtRtData> rtdata, String chunksymbol) throws Exception {
		throw new UnsupportedOperationException("Not supported yet.");
	}

	public void endDataExport() {
		throw new UnsupportedOperationException("Not supported yet.");
	}

	

	/*
	 * Helpers
	 */

	// ignore exceptions just log
	private void parseFile(List<HtRtData> rtdata, File f, String symbol)
	{
		CSVReader reader = null;
		
		try {
			reader = new CSVReader(new FileReader(f), ',');

			String[] nextLine = null;
			int rows = 0;
			Calendar tm = null;

			while ((nextLine = reader.readNext()) != null) {
				// ignore first line
				if (skipFirstLine_m && rows++<=0)
						continue;

				// 11112009,4:16:00,0.8000,0.8000,0.7500,0.8000,1500.0000
				// MMDDYYYY

				
				HtRtData rtdata_i = new HtRtData(HtRtData.HST_Type);
				rtdata_i.setSymbol(symbol);

				rtdata_i.open_m = Double.valueOf( nextLine[2] );
				rtdata_i.high_m = Double.valueOf( nextLine[3] );
				rtdata_i.low_m = Double.valueOf( nextLine[4] );
				rtdata_i.close_m = Double.valueOf( nextLine[5] );

				rtdata_i.open2_m = rtdata_i.open_m;
				rtdata_i.high2_m = rtdata_i.high_m;
				rtdata_i.low2_m = rtdata_i.low_m;
				rtdata_i.close2_m = rtdata_i.close_m;

				rtdata_i.volume_m = Double.valueOf( nextLine[6] );

				// parse date time
				LongParam millisec = new LongParam();
				int year, month, day, hour, minute, sec;
				String dateString =  nextLine[0];
				String timeString = nextLine[1];

				month =		Integer.valueOf( dateString.substring(0, 2) );
				day =			Integer.valueOf( dateString.substring(2, 4) );
				year =		Integer.valueOf( dateString.substring(4, 8) );

				String[] tokens_time = timeString.split(":");
				hour =		Integer.valueOf( tokens_time[0] );
				minute =	Integer.valueOf( tokens_time[1] );
				sec =			Integer.valueOf( tokens_time[2] );
				
				tm = HtDateTimeUtils.yearMonthDayHourMinSec2Time_ReuseCalendar_Gmt(year, month, day, hour, minute, sec, tm, millisec);

				rtdata_i.time_m = millisec.getLong();
				
				rtdata.add( rtdata_i );
				
			}
		}
		catch(Throwable e)
		{
			XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_ERROR, "While processing file: " + f.getName()+ " exception occured: " +e.getMessage());
		}
		finally
		{
			try {
				if (reader != null)
					reader.close();
			}
			catch(Throwable e)
			{
			}
		}
	}

  @Override
  public int getLevel1Chunk(List<HtLevel1Data> rtdata, StringBuilder chunksymbol) throws Exception {
	return 0;
  }
}