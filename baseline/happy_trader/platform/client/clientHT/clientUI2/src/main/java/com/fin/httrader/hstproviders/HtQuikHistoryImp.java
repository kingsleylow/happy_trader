
				/*
 * HtQuikHistoryImp.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.hstproviders;

import com.fin.httrader.interfaces.HtHistoryProvider;
import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.LongParam;
import com.fin.httrader.utils.hlpstruct.HtLevel1Data;
import com.fin.httrader.utils.hlpstruct.HtRtData;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import java.io.File;
import java.io.FileReader;
import java.io.StreamTokenizer;
import java.text.SimpleDateFormat;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Vector;

/**
 *
 * @author Victor_Zoubok
 */
public class HtQuikHistoryImp implements HtHistoryProvider {

	// the maximum read chunk
	private static final int MAX_READ_CHUNK = 100000;
	private StringBuilder providerName_m = new StringBuilder();
	private FileReader reader_m = null;
	private File workingFile_m = null;
	private StreamTokenizer tokenizer_m = null;

  @Override
  public int getLevel1Chunk(List<HtLevel1Data> rtdata, StringBuilder chunksymbol) throws Exception {
	return 0;
  }

	// structure for fast access to tokens
	private class Entry {

		public int date_idx_m = -1;
		public int time_idx_m = -1;
		public int open_idx_m = -1;
		public int high_idx_m = -1;
		public int low_idx_m = -1;
		public int close_idx_m = -1;
		public int volume_idx_m = -1;
		public int symbol_idx_m = -1;
		// names via which we will resolve other things
		// hard coding this for now
		public StringBuilder date_col_name_m = new StringBuilder("<DATE>");
		public StringBuilder time_col_name_m = new StringBuilder("<TIME>");
		public StringBuilder open_col_name_m = new StringBuilder("<OPEN>");
		public StringBuilder high_col_name_m = new StringBuilder("<HIGH>");
		public StringBuilder low_col_name_m = new StringBuilder("<LOW>");
		public StringBuilder close_col_name_m = new StringBuilder("<CLOSE>");
		public StringBuilder volume_col_name_m = new StringBuilder("<VOL>");
		public StringBuilder symbol_col_name_m = new StringBuilder("<TICKER>");
		// date and time formats
		public SimpleDateFormat dateF_m = new SimpleDateFormat("yyyyMMdd");
		public SimpleDateFormat timeF_m = new SimpleDateFormat("HHmmss");

		public boolean isValid() {
			return (symbol_idx_m >= 0 && date_idx_m >= 0 && time_idx_m >= 0 && open_idx_m >= 0 && high_idx_m >= 0 && low_idx_m >= 0 && close_idx_m >= 0 && volume_idx_m >= 0);
		}
	}
	Entry curEntry_m = null;

	// ---------------------------------------
	/** Creates a new instance of HtQuikHistoryImp */
	public HtQuikHistoryImp() {
	}

	private String getContext() {
		return this.getClass().getSimpleName();
	}

	// ------------------------------------
	public void setImportTimeShift(long timeshift) {
		// n/a
	}

	public void setExportTimeShift(long timeshift) {
		// n/a
	}

	public long getImportTimeShift() {
		return 0;
	}

	public long getExportTimeShift() {
		return 0;
	}

	public boolean beginDataImport(Map<String, String> initdata, long datebegin, long dateend) throws Exception {
		String path_to_import = initdata.get("path_to_load");

		if ((path_to_import == null) || (path_to_import.length() <= 0)) {
			throw new HtException(getContext(), "beginDataImport", "Parameter <path_to_load> is invalid");
		}

		workingFile_m = new File(path_to_import);

		if (!workingFile_m.exists()) {
			throw new HtException(getContext(), "beginDataImport", "The path :\"" + path_to_import + "\" points to non-existing file");
		}

		reader_m = new FileReader(workingFile_m);
		tokenizer_m = new StreamTokenizer(reader_m);

		tokenizer_m.resetSyntax();

		tokenizer_m.whitespaceChars(',', ',');
		tokenizer_m.whitespaceChars('\r', '\r');
		tokenizer_m.whitespaceChars('\n', '\n');
		tokenizer_m.whitespaceChars('\t', '\t');

		tokenizer_m.eolIsSignificant(true);
		tokenizer_m.wordChars('<', '<');
		tokenizer_m.wordChars('>', '>');
		tokenizer_m.wordChars('[', '[');
		tokenizer_m.wordChars(']', ']');
		tokenizer_m.wordChars(' ', ' ');
		tokenizer_m.wordChars('.', '.');
		tokenizer_m.wordChars('a', 'z');
		tokenizer_m.wordChars('A', 'Z');
		tokenizer_m.wordChars('0', '9');

		// fill the Entry structure
		curEntry_m = new Entry();

		// read the first line
		HashMap<String, Integer> columnMap = new HashMap<String, Integer>();

		int idx = 0;
		while (true) {
			int token = tokenizer_m.nextToken();
			if (token == StreamTokenizer.TT_EOL) {
				break;
			}
			if (tokenizer_m.ttype == StreamTokenizer.TT_WORD) {

				if (tokenizer_m.sval != null) {
					columnMap.put(tokenizer_m.sval, idx);

					// advance
					idx++;
				}
			}


		}
		;

		if (columnMap.containsKey(curEntry_m.open_col_name_m.toString())) {
			curEntry_m.open_idx_m = columnMap.get(curEntry_m.open_col_name_m.toString());
		}

		if (columnMap.containsKey(curEntry_m.high_col_name_m.toString())) {
			curEntry_m.high_idx_m = columnMap.get(curEntry_m.high_col_name_m.toString());
		}

		if (columnMap.containsKey(curEntry_m.low_col_name_m.toString())) {
			curEntry_m.low_idx_m = columnMap.get(curEntry_m.low_col_name_m.toString());
		}

		if (columnMap.containsKey(curEntry_m.close_col_name_m.toString())) {
			curEntry_m.close_idx_m = columnMap.get(curEntry_m.close_col_name_m.toString());
		}

		if (columnMap.containsKey(curEntry_m.volume_col_name_m.toString())) {
			curEntry_m.volume_idx_m = columnMap.get(curEntry_m.volume_col_name_m.toString());
		}

		if (columnMap.containsKey(curEntry_m.date_col_name_m.toString())) {
			curEntry_m.date_idx_m = columnMap.get(curEntry_m.date_col_name_m.toString());
		}

		if (columnMap.containsKey(curEntry_m.time_col_name_m.toString())) {
			curEntry_m.time_idx_m = columnMap.get(curEntry_m.time_col_name_m.toString());
		}

		if (columnMap.containsKey(curEntry_m.symbol_col_name_m.toString())) {
			curEntry_m.symbol_idx_m = columnMap.get(curEntry_m.symbol_col_name_m.toString());
		}

		if (!curEntry_m.isValid()) {
			throw new HtException(getContext(), "beginDataImport", "Can't get all parameters from: " + path_to_import);
		}

		XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_INFO, "History provider started import from file: " + path_to_import);


		return true;

	}

	public void endDataImport() {
		try {
			workingFile_m = null;
			if (reader_m != null) {
				reader_m.close();
			}
		} catch (Throwable e) {
		}
		;

		tokenizer_m = null;
		reader_m = null;
		workingFile_m = null;
		curEntry_m = null;

		XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_INFO, "History provider finished import");
	}

	public int getDataChunk(List<HtRtData> rtdata, StringBuilder chunksymbol) throws Exception {
		// the first line muts be the symbol
		rtdata.clear();
		chunksymbol.setLength(0);

		//
		int ntok;
		int rowcnt = 0;
		int column_idx = 0;

		// ----- //

		// initial data
		HtRtData data = new HtRtData(HtRtData.HST_Type);


		long timestamp = 0;
		while (true) {
			ntok = tokenizer_m.nextToken();

			if (ntok == StreamTokenizer.TT_EOL) {

				data.time_m = timestamp;


				// row finished
				rowcnt++;
				column_idx = 0;
				timestamp = 0;


				rtdata.add(data);

				// new ticker
				data = new HtRtData(HtRtData.HST_Type);
			}

			if (rowcnt >= MAX_READ_CHUNK) {
				break;
			}

			if (ntok == StreamTokenizer.TT_EOF) {
				break;
			}

			//


			String sval = tokenizer_m.sval;

			//
			if (tokenizer_m.ttype == StreamTokenizer.TT_WORD || tokenizer_m.ttype == StreamTokenizer.TT_NUMBER) {

				// -------------- // ------------ //

				if (column_idx == curEntry_m.close_idx_m) {
					data.close_m = Double.valueOf(sval);
					data.close2_m = data.close_m;
				} else if (column_idx == curEntry_m.date_idx_m) {
					timestamp += curEntry_m.dateF_m.parse(sval).getTime();
				} else if (column_idx == curEntry_m.high_idx_m) {
					data.high_m = Double.valueOf(sval);
					data.high2_m = data.high_m;
				} else if (column_idx == curEntry_m.low_idx_m) {
					data.low_m = Double.valueOf(sval);
					data.low2_m = data.low_m;
				} else if (column_idx == curEntry_m.open_idx_m) {
					data.open_m = Double.valueOf(sval);
					data.open2_m = data.open_m;
				} else if (column_idx == curEntry_m.volume_idx_m) {
					data.volume_m = Double.valueOf(sval);
				} else if (column_idx == curEntry_m.symbol_idx_m) {
					if (chunksymbol.length() == 0) {
						chunksymbol.append(sval);
					}

					data.setSymbol(sval);
				} else if (column_idx == curEntry_m.time_idx_m) {
					timestamp += curEntry_m.timeF_m.parse(sval).getTime();
				}

				// advance column
				column_idx++;

			}
			;





		}
		; // end of main loop

		return rowcnt;

	}

	public boolean beginDataExport(Map<String, String> initdata, long datebegin, long dateend) throws Exception {
		throw new HtException(getContext(), "beginDataExport", "Export not supported");
	}

	public void setDataChunk(List<HtRtData> rtdata, String chunksymbol) throws Exception {
	}

	public void endDataExport() {
	}

	

	public String getProviderId() {
		return providerName_m.toString();
	}
	;

	public void setProviderId(String providerId) {
		providerName_m.setLength(0);
		providerName_m.append(providerId);
	}

	public void getAvailableSymbolsToImport(Vector<String> symbols) {
	}

	public void getSymbolMinMaxDatesToImport(String symbol, LongParam minDate, LongParam maxDate) {
	}
}
