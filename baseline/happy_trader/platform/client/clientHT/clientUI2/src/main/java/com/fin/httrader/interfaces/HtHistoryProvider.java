/*
 * HtHistoryProvider.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.interfaces;

import com.fin.httrader.utils.hlpstruct.HtRtData;
import com.fin.httrader.utils.LongParam;
import com.fin.httrader.utils.hlpstruct.HtLevel1Data;
import java.util.List;

import java.util.Map;
import java.util.Vector;

/**
 *
 * @author victor_zoubok
 * This is interface for implementoing History providers 
 */
public interface HtHistoryProvider {

	// flags shoing the state of the proivider if job is expected to by asynchronious
	public final int STATE_BUSY_IMPORT = 1;
	public final int STATE_BUSY_EXPORT = 1;
	public final int STATE_BUSY_FREE = 1;

	public void setImportTimeShift(long timeshift);

	public void setExportTimeShift(long timeshift);

	public long getImportTimeShift();

	public long getExportTimeShift();

	// import functions
	public boolean beginDataImport(Map<String, String> initdata, long datebegin, long dateend) throws Exception;

	public void endDataImport();

	// if chunksymbol is not null, HtRtData symbols are ignored
	// rtdata and chunksymbol are completed with imported data
	// returns the pct of processed data
	// or 0 if no more chunks is expected the next time
	public int getDataChunk(List<HtRtData> rtdata, StringBuilder chunksymbol) throws Exception;
	
	// this is for level 1 data
	public int getLevel1Chunk(List<HtLevel1Data> rtdata, StringBuilder chunksymbol) throws Exception;

	//
	// return false if export is not supported
	public boolean beginDataExport(Map<String, String> initdata, long datebegin, long dateend) throws Exception;

	// if chunksymbol is not null, HtRtData symbols are ignored
	// rtdata and chunksymbol are passedc be the caller
	public void setDataChunk(List<HtRtData> rtdata, String chunksymbol) throws Exception;

	public void endDataExport();

	public String getProviderId();

	public void setProviderId(String providerId);

	// functiuon returning date-time stamps of data to be imported
	// valid after beginDataImport
	public void getAvailableSymbolsToImport(Vector<String> symbols);

	// function returning minimum and maximum date-time stamps of data to be imported
	public void getSymbolMinMaxDatesToImport(
			String symbol,
			LongParam minDate,
			LongParam maxDate
	);

	
}
