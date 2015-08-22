/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.htraderhelper;

import com.fin.htraderhelper.utils.*;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.StreamTokenizer;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
import java.util.TreeSet;
import org.jfree.ui.RefineryUtilities;

/**
 *
 * @author DanilinS
 * This will parse input data
 */
public class HtDataProvider {

	public static final String BEGIN_SEP_TAG = "<B";
	public static final String END_SEP_TAG = "E>";

	// the toal numbers of objects
	public class SymbolProperty {

		public SymbolProperty() {
		}
		public int num_Print_m = 0;
		public int num_Leve11_m = 0;
		public int num_OHCL_m = 0;
		public int num_Text_m = 0;
		public int num_LineIndic_m = 0;
		public Map<Long, HtDrawableObject> dobjects_m = new TreeMap<Long, HtDrawableObject>();

		// these are short texts of available text objects
		//public Set<String> shortTexts_m = new HashSet<String>();
	}
	StringBuilder path_m = new StringBuilder();

	// set
	//Map<String, JFreeChart> chartSet_m = new HashMap<String, JFreeChart>();
	Map<SymbolKey, SymbolProperty> dobjectsProps_m = new TreeMap<SymbolKey, SymbolProperty>();




	// ----------------------------
	public HtDataProvider(String dataFilePath, HtWorkProgressor progressor) throws Exception {
		path_m.append(dataFilePath);
	

		// parse

		//List<XmlParameter> drowableObjList = new ArrayList<XmlParameter>();
		XmlHandler hn = new XmlHandler();


		BufferedReader in = null;

		File finp = new File(path_m.toString());

		long total = finp.length();


		if (progressor != null) {
			progressor.setPctValue(0);
		}

		long current = 0;
		int repeat = 0;
		try {
			in = new BufferedReader(new FileReader(finp));

			// 0000000000

			while (true) {
				//

				char[] len_buf = new char[10];

				int res = in.read(len_buf);
				if (res == -1) {
					break;
				}
				current += 10;

				int len = Integer.valueOf(new String(len_buf));

				char[] data_buf = new char[len];

				res = in.read(data_buf);
				if (res == -1) {
					break;
				}
				current += len;

				// do not update it constantly
				if (repeat++ >= 100) {
					if (progressor != null) {
						int pct = (int)(((double)current/(double)total)*100.0);
						progressor.setPctValue(pct);
					
					}
					
					repeat = 0;
				}


				// here we have current

				String chunk = new String(data_buf);
				XmlParameter chunk_param = new XmlParameter();
				hn.deserializeParameter(chunk.toString(), chunk_param);
				//drowableObjList.add(chunk_param);

				HtDrawableObject dobj_i = new HtDrawableObject();
				HtDrawableObject.drawableObjectFromParameter(chunk_param, dobj_i);

				
				SymbolKey key_i = new SymbolKey(dobj_i.getRunName(), dobj_i.getName(), dobj_i.getProvider(), dobj_i.getSymbol());

				SymbolProperty prop_i = null;
				if (dobjectsProps_m.containsKey(key_i)) {
					prop_i = dobjectsProps_m.get(key_i);
				} else {
					prop_i = new SymbolProperty();
					dobjectsProps_m.put(key_i, prop_i);
				}

				prop_i.dobjects_m.put(dobj_i.getTime(), dobj_i);
				if (dobj_i.getType() == HtDrawableObject.DO_LEVEL1) {
					prop_i.num_Leve11_m++;
				} else if (dobj_i.getType() == HtDrawableObject.DO_LINE_INDIC) {
					prop_i.num_LineIndic_m++;
				} else if (dobj_i.getType() == HtDrawableObject.DO_OHLC) {
					prop_i.num_OHCL_m++;
				} else if (dobj_i.getType() == HtDrawableObject.DO_PRINT) {
					prop_i.num_Print_m++;
				} else if (dobj_i.getType() == HtDrawableObject.DO_TEXT) {
					prop_i.num_Text_m++;

				}

			}

		} catch (Throwable e) {

			throw new Exception("On processing exception: " + e.getMessage());
		} finally {
			try {
				if (in != null) {
					in.close();
				}
			} catch (Throwable ign) {
			}
		}

		if (progressor != null) {
			progressor.setPctValue(100);
		}
	}

	public Set<SymbolKey> getSymbolList() {
		return dobjectsProps_m.keySet();
	}

	public void createChart(SymbolKey symbol) throws Exception {
		DataChart chart = new DataChart(symbol, this);
		chart.pack();
		RefineryUtilities.centerFrameOnScreen(chart);
		chart.setVisible(true);
	}

	public void exportData(File baseDir, HtWorkProgressor progressor) throws Exception {

		if (progressor != null) {
			progressor.setPctValue(0);
		}

		if (!baseDir.isDirectory()) {
			throw new Exception("Not a directory: " + baseDir.getAbsolutePath());
		}

		if (!baseDir.exists()) {
			throw new Exception("A target dir does not exist: " + baseDir.getAbsolutePath());
		}

		HtFileUtils.deleteDirectory(baseDir);

		if (dobjectsProps_m.isEmpty()) {
			throw new Exception("No available data to export");
		}

		String base_dir = baseDir.getAbsolutePath();


		StringBuilder path = new StringBuilder();
		int cnt = 0;
		for (Iterator<SymbolKey> it = dobjectsProps_m.keySet().iterator(); it.hasNext();) {

			SymbolKey key_i = it.next();


			if (progressor != null) {
				int pct = (int)(((double)cnt/(double)dobjectsProps_m.size())*100);
				progressor.setPctValue(pct);
			}

			cnt++;

			// create dir structure

			path.setLength(0);
			path.append(base_dir);
			path.append(File.separatorChar);
			path.append(URLUTF8Encoder.encode(key_i.getRunName()));
			path.append(File.separatorChar);
			path.append(URLUTF8Encoder.encode(key_i.getName()));
			path.append(File.separatorChar);
			path.append(URLUTF8Encoder.encode(key_i.getProvider()));
			path.append(File.separatorChar);
			path.append(URLUTF8Encoder.encode(key_i.getSymbol()));
			path.append(File.separatorChar);

			File fdir = new File(path.toString());
			fdir.mkdirs();



			BufferedWriter bf_level1 = new BufferedWriter( new FileWriter(path.toString() + URLUTF8Encoder.encode(key_i.getSymbol()) + "_level1.txt"));
			bf_level1.write("<DATE TIME>;<OBJECT NAME>-><BEST ASK PRICE>;<BEST BID PRICE>;<BEST ASK VOLUME>;<BEST BID VOLUME>;\r\n");

			BufferedWriter bf_print = new BufferedWriter( new FileWriter(path.toString() + URLUTF8Encoder.encode(key_i.getSymbol()) + "_print.txt"));
			bf_print.write("<DATE TIME>;<OBJECT NAME>-><ASK PRICE>;<BID PRICE>;<VOLUME>;<COLOR>;\r\n");

			BufferedWriter bf_ohlc = new BufferedWriter( new FileWriter(path.toString() + URLUTF8Encoder.encode(key_i.getSymbol()) + "_ohlc.txt"));
			bf_ohlc.write("<DATE TIME>;<OBJECT NAME>-><OPEN>;<HIGH>;<LOW>;<CLOSE>;<VOLUME>;\r\n");

			BufferedWriter bf_text = new BufferedWriter( new FileWriter(path.toString() + URLUTF8Encoder.encode(key_i.getSymbol()) + "_text.txt"));
			bf_text.write("<DATE TIME>;<OBJECT NAME>->[<PRIORITY>] - SHORT: ( <SHORT TEXT> ) LONG: ( <LONG TEXT> );\r\n");

			BufferedWriter bf_indic = new BufferedWriter( new FileWriter(path.toString() + URLUTF8Encoder.encode(key_i.getSymbol()) + "_indic.txt"));
			bf_indic.write("<DATE TIME>;<OBJECT NAME>->[<ID>;<NAME>;<VALUE>];...;\r\n");

			SymbolProperty prop_i = dobjectsProps_m.get(key_i);
			Map<Long, HtDrawableObject> curObjList = prop_i.dobjects_m;

			// main loop over objects
			
			for (Iterator<Long> it2 = curObjList.keySet().iterator(); it2.hasNext(); ) {
				Long time_i = it2.next();
				HtDrawableObject dobj_i = curObjList.get(time_i);

				StringBuilder entry = new StringBuilder(Utils.formatDateTime(time_i));
				entry.append(";");
				entry.append(dobj_i.getName());
				entry.append("->");
				
				if (dobj_i.getType() == HtDrawableObject.DO_LEVEL1) {
					
					HtDrawableObject.Level1 ldobj_i = dobj_i.getAsLevel1();

					entry.append(HtUtils.formatDouble(ldobj_i.best_ask_price_m, 4));
					entry.append(";");
					entry.append(HtUtils.formatDouble(ldobj_i.best_bid_price_m,4));
					entry.append(";");
					entry.append(HtUtils.formatDouble(ldobj_i.best_ask_volume_m,4));
					entry.append(";");
					entry.append(HtUtils.formatDouble(ldobj_i.best_bid_volume_m,4));
					entry.append(";\r\n");
				
					bf_level1.write(entry.toString());

					
				} else if (dobj_i.getType() == HtDrawableObject.DO_LINE_INDIC) {
					List<HtDrawableObject.LineIndicator> indicList_i = dobj_i.getAsLineIndicator();

					for(int k = 0; k < indicList_i.size(); k++) {
						HtDrawableObject.LineIndicator indic_i = indicList_i.get(k);
						entry.append("[ ");
						entry.append(k);
						entry.append(";");
						entry.append(indic_i.name_m);
						entry.append(";");
						entry.append(HtUtils.formatDouble(indic_i.value_m,4));
						entry.append(";");
						entry.append(" ]");

					}

					entry.append("\r\n");
					bf_indic.write(entry.toString());
					
				} else if (dobj_i.getType() == HtDrawableObject.DO_OHLC) {
					HtDrawableObject.OHLC ohlc_obj = dobj_i.getAsOHLC();
					entry.append(HtUtils.formatDouble(ohlc_obj.open_m,4));
					entry.append(";");
					entry.append(HtUtils.formatDouble(ohlc_obj.high_m,4));
					entry.append(";");
					entry.append(HtUtils.formatDouble(ohlc_obj.low_m,4));
					entry.append(";");
					entry.append(HtUtils.formatDouble(ohlc_obj.close_m,4));
					entry.append(";");
					entry.append(HtUtils.formatDouble(ohlc_obj.volume_m,4));
					entry.append(";\r\n");
					
					bf_ohlc.write(entry.toString());
					
					
				} else if (dobj_i.getType() == HtDrawableObject.DO_PRINT) {
					HtDrawableObject.Print pobj_i = dobj_i.getAsPrint();
					entry.append(HtUtils.formatDouble(pobj_i.ask_m,4));
					entry.append(";");
					entry.append(HtUtils.formatDouble(pobj_i.bid_m,4));
					entry.append(";");
					entry.append(HtUtils.formatDouble(pobj_i.volume_m,4));
					entry.append(";");
					entry.append(RtDataPrintColor.getColorTypeName( pobj_i.color_m ));
					entry.append(";\r\n");
				
					bf_print.write(entry.toString());


				} else if (dobj_i.getType() == HtDrawableObject.DO_TEXT) {
					HtDrawableObject.Text text_obj = dobj_i.getAsText();

					entry.append("[");
					entry.append(AlertPriorities.getAlertpriorityByName(text_obj.priority_m));
					entry.append("] -");
					entry.append(" SHORT: ( ");
					entry.append(text_obj.shortText_m);
					entry.append(" ) LONG: ( ");
					entry.append(text_obj.text_m);
					entry.append(" )");
					entry.append(";\r\n");
				

					bf_text.write(entry.toString());
				}



			}

			bf_level1.close();
			bf_print.close();
			bf_ohlc.close();
			bf_text.close();
			bf_indic.close();


		}

		if (progressor != null) {
			progressor.setPctValue(100);
		}
	}

	public SymbolProperty getSymbolProperty(SymbolKey symbol) throws Exception {
		if (!dobjectsProps_m.containsKey(symbol)) {
			throw new Exception("Non-registered symbol: " + symbol.toString());
		}

		return dobjectsProps_m.get(symbol);
	}

	// searching
	public List<Integer> globaltextSearch(String shortTextPattern, String longTextPattern, HtWorkProgressor progressor)
	{
		List<Integer> result = new ArrayList<Integer>();

		if (progressor != null) {
			progressor.setPctValue(0);
		}

		
		if (shortTextPattern.length()<=0 && longTextPattern.length()<=0) {
			if (progressor != null) {
				progressor.setPctValue(100);
			}

			return result;
		}
		
		int cnt = 0;
		int total = dobjectsProps_m.size();
		for (Iterator<SymbolKey> it = dobjectsProps_m.keySet().iterator(); it.hasNext();)
		{
			SymbolKey key_i = it.next();
			SymbolProperty prop_i = dobjectsProps_m.get(key_i);
			Map<Long, HtDrawableObject> curObjList = prop_i.dobjects_m;

			boolean matched = false;
			for (Iterator<Long> it2 = curObjList.keySet().iterator(); it2.hasNext(); ) {
				Long time_i = it2.next();
				HtDrawableObject dobj_i = curObjList.get(time_i);

				

				if (dobj_i.getType() == HtDrawableObject.DO_TEXT) {
					HtDrawableObject.Text text_obj = dobj_i.getAsText();

					if (shortTextPattern.length() >0) {
						if (text_obj.shortText_m.toString().indexOf(shortTextPattern) >= 0) {
							matched=true;
							break;

						}
						
					}

					if (longTextPattern.length() > 0)	{
						if (text_obj.text_m.toString().indexOf(longTextPattern)>=0) {
							matched=true;
							break;
						}
					}

					
				}
			}

			if (matched) {
					result.add(cnt);
			}

			cnt++;
			if (progressor != null) {
				int pct = (int)(((double)cnt/(double)total)*100.0);
				progressor.setPctValue(pct);
			}
	
		}

		if (progressor != null) {
				progressor.setPctValue(100);
		}

		return result;
	}

	

	// ----------------
	// helpers
}
