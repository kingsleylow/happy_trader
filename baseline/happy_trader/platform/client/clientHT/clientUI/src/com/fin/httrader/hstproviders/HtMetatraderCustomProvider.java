/*
 * HtMetatraderCustomProvider.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.hstproviders;

import com.fin.httrader.interfaces.HtHistoryProvider;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtMathUtils;
import com.fin.httrader.utils.hlpstruct.HtRtData;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.LongParam;
import com.fin.httrader.utils.hlpstruct.HtLevel1Data;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileReader;
import java.nio.ByteBuffer;
import java.nio.CharBuffer;

import java.nio.charset.Charset;
import java.nio.charset.CharsetDecoder;
import java.util.List;
import java.util.Map;
import java.util.Vector;
import java.util.logging.Level;

/**
 *
 * @author victor_zoubok
 */
public class HtMetatraderCustomProvider implements HtHistoryProvider {

	// the maximum read chunk
	private static final int MAX_READ_CHUNK = 300000;
	private long importTimeShift_m = 0;
	private boolean processAskPrices_m = true;
	private int state_m = STATE_BUSY_FREE;
	private FileInputStream reader_m = null;
	private String cutted_m = null;
	// these are dates containing what provider can privide for server
	private long minDate_m = -1;
	private long maxDate_m = -1;
	// these dates are for filtering purposes only if set
	private long filterMinDate_m = -1;
	private long filterMaxDate_m = -1;
	// import export paths
	private StringBuilder pathToImport_m = new StringBuilder();

	private String getContext() {
		return this.getClass().getSimpleName();
	}

	//
	public void setImportTimeShift(long timeshift) {
		importTimeShift_m = timeshift;
	}

	public void setExportTimeShift(long timeshift) {
		//N/a
	}

	public long getImportTimeShift() {
		return importTimeShift_m;
	}

	public long getExportTimeShift() {
		// N/A
		return 0;
	}



	public boolean beginDataImport(Map<String, String> initdata, long datebegin, long dateend) throws Exception {

		pathToImport_m.setLength(0);
		processAskPrices_m = true;

		cutted_m = null;

		minDate_m = -1;
		maxDate_m = -1;

		// filtering
		if (datebegin > 0) {
			filterMinDate_m = datebegin;
		} else {
			filterMinDate_m = -1;
		}

		if (dateend > 0) {
			filterMaxDate_m = dateend;
		} else {
			filterMaxDate_m = -1;
		}

		// file to import
		String file_to_import = initdata.get("file_to_import");
		if ((file_to_import == null) || (file_to_import.length() <= 0)) {
			throw new HtException(getContext(), "beginDataImport", "Parameter (file_to_import) is invalid");
		}

		reader_m = new FileInputStream(file_to_import);
		pathToImport_m.append(file_to_import);

		// whether ask or bid prices
		String processAskPrices = initdata.get("ask_prices");
		if (processAskPrices == null || processAskPrices.length() <= 0) {
			throw new HtException(getContext(), "beginDataImport", "Parameter (ask_prices) is invalid");
		}

		processAskPrices_m = processAskPrices.equalsIgnoreCase("true") ? true : false;

		// resolve symbol
		byte[] symbolb = new byte[32];
		byte[] strlenb = new byte[4];

		int read = reader_m.read(strlenb);
		if (read <= 0) {
			throw new HtException(getContext(), "beginDataImport", "Cannot read header of the file: " + pathToImport_m);
		}


		read = reader_m.read(symbolb);
		if (read <= 0) {
			throw new HtException(getContext(), "beginDataImport", "Cannot read header of the file: " + pathToImport_m);
		}

		Charset isoCharset = Charset.forName("ISO-8859-1");
		CharsetDecoder decoder = isoCharset.newDecoder();

		ByteBuffer sbuf = ByteBuffer.wrap(symbolb);
		CharBuffer cbuf = decoder.decode(sbuf);
		long strlen = (long) HtMathUtils.convertByteToInt4(strlenb);

		cutted_m = cbuf.toString().substring(0, (int) strlen);

		// max min dates
		byte[] maxdb = new byte[4];
		byte[] mindb = new byte[4];

		read = reader_m.read(maxdb);
		if (read <= 0) {
			throw new HtException(getContext(), "beginDataImport", "Cannot read header of the file: " + pathToImport_m);
		}

		read = reader_m.read(mindb);
		if (read <= 0) {
			throw new HtException(getContext(), "beginDataImport", "Cannot read header of the file: " + pathToImport_m);
		}

		minDate_m = (long) HtMathUtils.convertByteToInt4(mindb) * 1000;
		minDate_m += getImportTimeShift() * 60 * 60 * 1000;

		maxDate_m = (long) HtMathUtils.convertByteToInt4(maxdb) * 1000;
		maxDate_m += getImportTimeShift() * 60 * 60 * 1000;

		//
		byte[] minuteFramb = new byte[4];
		read = reader_m.read(minuteFramb);
		if (read <= 0) {
			throw new HtException(getContext(), "beginDataImport", "Cannot read header of the file: " + pathToImport_m);
		}


		state_m = STATE_BUSY_IMPORT;
		return true;
	}

	public void endDataImport() {

		try {
			if (reader_m != null) {
				reader_m.close();
			}
		} catch (Exception e) {
		}

		state_m = STATE_BUSY_FREE;
	}

	public int getDataChunk(List<HtRtData> rtdata, StringBuilder chunksymbol) throws Exception {

		rtdata.clear();

		byte[] ctmb = new byte[4];
		byte[] openb = new byte[8];
		byte[] highb = new byte[8];
		byte[] lowb = new byte[8];
		byte[] closeb = new byte[8];
		byte[] volub = new byte[8];

		byte[] symbolb = new byte[32];

		chunksymbol.setLength(0);
		chunksymbol.append(cutted_m);

		if (chunksymbol.length() <= 0) {
			throw new HtException(getContext(), "getDataChunk", "Cannot read symbol name from the file: " + pathToImport_m);
		}

		int read = 1;
		int readrows = 0;

		while (read >= 0) {

			read = reader_m.read(ctmb);
			if (read <= 0) {
				break;
			}

			//double openPrice  = HtUtils.readDoubleValueFromStream(reader);
			read = reader_m.read(openb);
			if (read <= 0) {
				break;
			}

			read = reader_m.read(highb);
			if (read <= 0) {
				break;
			}

			read = reader_m.read(lowb);
			if (read <= 0) {
				break;
			}

			read = reader_m.read(closeb);
			if (read <= 0) {
				break;
			}

			read = reader_m.read(volub);
			if (read <= 0) {
				break;
			}

			//
			HtRtData rtdata_i = new HtRtData();
			rtdata_i.setSymbol(chunksymbol.toString());
			rtdata_i.type_m = HtRtData.HST_Type;

			// time in milliseconds
			rtdata_i.time_m = (long) HtMathUtils.convertByteToInt4(ctmb) * 1000;
			rtdata_i.time_m += getImportTimeShift() * 60 * 60 * 1000;



			// filter out time
			if (filterMinDate_m > 0 && filterMinDate_m > rtdata_i.time_m) {
				continue;
			}

			if (filterMaxDate_m > 0 && filterMaxDate_m < rtdata_i.time_m) {
				continue;
			}

			//HtUtils.log(Level.INFO, "RtMetaTaderHistoryProviderImp", "readChunk", "Candle time: " + HtUtils.time2String_Gmt(rtdata_i.time_m));


			//
			rtdata_i.volume_m = HtMathUtils.readDoubleValueFromBytes(volub);

			/////
			//
			/////
			//if (processAskPrices_m) {
			// in MT we work only with BID prices
			/*
				rtdata_i.open_m = HtMathUtils.readDoubleValueFromBytes(openb);
				rtdata_i.close_m = HtMathUtils.readDoubleValueFromBytes(closeb);
				rtdata_i.high_m = HtMathUtils.readDoubleValueFromBytes(highb);
				rtdata_i.low_m = HtMathUtils.readDoubleValueFromBytes(lowb);
			 */
			//} else {
				rtdata_i.open2_m = HtMathUtils.readDoubleValueFromBytes(openb);
				rtdata_i.close2_m = HtMathUtils.readDoubleValueFromBytes(closeb);
				rtdata_i.high2_m = HtMathUtils.readDoubleValueFromBytes(highb);
				rtdata_i.low2_m = HtMathUtils.readDoubleValueFromBytes(lowb);
			//}
			;


			// make dummy arrangement
			//GregorianCalendar time_of_candle = new GregorianCalendar();
			//time_of_candle.setTimeInMillis();

			//
			rtdata.add(rtdata_i);

			if (++readrows > MAX_READ_CHUNK) {
				return rtdata.size();
			}

		}

		// finished
		return rtdata.size();
	}
	;

	public boolean beginDataExport(Map<String, String> initdata, long datebegin, long dateend) throws Exception {
		throw new HtException(getContext(), "beginDataExport", "Export not supported");
	}

	public void setDataChunk(List<HtRtData> rtdata, String chunksymbol) throws Exception {
		// N/A
	}

	public void endDataExport() {
		// N/A
	}

	

	public String getProviderId() {
		return null;
	}

	public void setProviderId(String providerId) {
		// N/A
	}

	public void getAvailableSymbolsToImport(Vector<String> symbols) {
		symbols.clear();
		symbols.add(cutted_m);
	}

	public void getSymbolMinMaxDatesToImport(String symbol, LongParam minDate, LongParam maxDate) {


		if (symbol.equals(cutted_m)) {

			minDate.setLong(minDate_m);
			maxDate.setLong(maxDate_m);

		} else {
			minDate.setLong(-1);
			maxDate.setLong(-1);
		}


	}

  @Override
  public int getLevel1Chunk(List<HtLevel1Data> rtdata, StringBuilder chunksymbol) throws Exception {
	return 0;
  }
}
