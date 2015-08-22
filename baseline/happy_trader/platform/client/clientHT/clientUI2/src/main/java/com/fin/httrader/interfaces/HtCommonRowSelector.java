/*
 * HtCommonRowSelector.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.interfaces;

import java.util.HashMap;
import java.util.Iterator;

/**
 *
 * @author Victor_Zoubok
 * This interface describes common dataset that is returned by SQL query
 */
public interface HtCommonRowSelector {

	public interface HtCommonFieldFormatter {

		abstract public String format(CommonRow row, String sval);
	}

	public class FieldDescriptor
	{
		FieldDescriptor()
		{
		}

		FieldDescriptor(HtCommonFieldFormatter formatter, String displayName, String fieldName, int fieldType)
		{
			formatter_m = formatter;

			displayName_m = displayName;

			fieldName_m = fieldName;

			fieldType_m = fieldType;

		}
		
		HtCommonFieldFormatter formatter_m = null;

		HtCommonFieldFormatter typeFormatter_m = null;

		String displayName_m = null;

		String fieldName_m = null;

		int fieldType_m = -1;
	}
	
	// this class incapsulate header
	public class CommonHeader {

		private int size_m = 0;
		// columns
		// map column ID- column name
		private HashMap<String, Integer> columns_m = new HashMap<String, Integer>();

		// reverse map column name - column ID
		//private HashMap<Integer, String> rcolumns_m = new HashMap<Integer, String>();

		private HashMap<Integer, FieldDescriptor> rdispcolumns_m = new HashMap<Integer, FieldDescriptor>();
				
		//private HashMap<Integer, HtCommonFieldFormatter> rformatter_m = new HashMap<Integer, HtCommonFieldFormatter>();

		public CommonHeader()
		{
		}

		public int getSize()
		{
			return size_m;
		}

		// add formatter for the specified column
		public void addFormatter(int id, HtCommonFieldFormatter formatter) {
			if (id > 0 && id <= size_m) {
				if (rdispcolumns_m.containsKey(id)) {
					FieldDescriptor descr = rdispcolumns_m.get(id);
					descr.formatter_m = formatter;
				}
			}
		}

		// add formatter for the specified type
		public void addFormatterForTheType(int typeId, HtCommonFieldFormatter formatter) {

			for(Iterator<Integer> it=rdispcolumns_m.keySet().iterator(); it.hasNext(); ) {
				FieldDescriptor descr_i = rdispcolumns_m.get( it.next() );

				if (descr_i.fieldType_m == typeId)
					descr_i.typeFormatter_m = formatter;
			}

		}


		public int addFieldId(String name, String displayName) {
			if (name != null) {
				columns_m.put(name, ++size_m);
				rdispcolumns_m.put(size_m, new FieldDescriptor(null, displayName, name, -1));
				return size_m;
			}
			return -1;
		}

		public int addFieldId(String name, String displayName, int dataType) {
			if (name != null) {
				columns_m.put(name, ++size_m);
				rdispcolumns_m.put(size_m, new FieldDescriptor(null, displayName, name, dataType));
				return size_m;
			}
			return -1;
		}

		public int getFieldId(String name) {
			if (name != null) {
				if (columns_m.containsKey(name)) {
					return columns_m.get(name).intValue();
				}
			}

			return -1;
		}

		public String getFieldName(int id) {
			if (id > 0 && id <= size_m) {
				if (rdispcolumns_m.containsKey(id)) {
					return rdispcolumns_m.get(id).fieldName_m;
				}
			}

			return null;
		}

		public String getDisplayName(int id) {
			if (id > 0 && id <= size_m) {
				if (rdispcolumns_m.containsKey(id)) {
					return rdispcolumns_m.get(id).displayName_m;
				}
			}

			return null;
		}

		public void clear() {
			columns_m.clear();
			rdispcolumns_m.clear();
		}

		public void create(CommonHeader src) {
			this.size_m = src.size_m;
			clear();

			this.columns_m.putAll(src.columns_m);
			this.rdispcolumns_m.putAll(src.rdispcolumns_m);
		}

		// formatting functions based on the type of values
		public String formatValue(CommonRow row, int id, String sval) {
			if (rdispcolumns_m.containsKey(id)) {
				FieldDescriptor descr_i = rdispcolumns_m.get(id);
				HtCommonFieldFormatter formatterFound = descr_i.formatter_m;

				if (formatterFound != null) {
					return formatterFound.format(row, sval);
				}

				HtCommonFieldFormatter typeFormatterFound = descr_i.typeFormatter_m;
				if (typeFormatterFound != null) {
					return typeFormatterFound.format(row, sval);
				}
			}

			return sval;
		}
	};

	// this Class incapsulate single raw
	public class CommonRow {

		static private final int MAX_COLS = 52;
		private CommonHeader header_m;
		//private int size_m = 0;
		private String[] row_m = new String[MAX_COLS + 1];

		public CommonRow(CommonHeader header) {
			header_m = header;
		}

		public void setHeader(CommonHeader header) {
			header_m = header;
		}

		public CommonHeader getHeader() {
			return header_m;
		}

		public void clear() {
			for (int i = 1; i <= header_m.getSize(); i++) {
				row_m[i] = null;
			}

		}
		// create a copy from source

		public void create(CommonRow src) {
			for (int i = 1; i <= header_m.getSize(); i++) {
				if (src.row_m[i] != null) {
					row_m[i] = src.row_m[i];
				}
			}
		}

		// derived to set different types of data
		public void setValue(int i, String val) {
			if (val != null) {
				row_m[i] = val;
			} else {
				row_m[i] = "";
			}
		}

		public void setValue(int i, double val) {
			row_m[i] = String.valueOf(val);
		}

		public void setValue(int i, long val) {
			row_m[i] = String.valueOf(val);
		}

		public void setValue(int i, int val) {
			row_m[i] = String.valueOf(val);
		}

		public String getValue(int i) {
			return row_m[i];
		}

		// get formatted value
		public String getFormattedValue(int i) {
			return header_m.formatValue(this, i, getValue(i));
		}

	
	}

	//
	abstract public void newHeaderArrived(CommonHeader header) throws Exception;

	abstract public void newRowArrived(CommonRow row) throws Exception;

	abstract public void onSelectionFinish(long cntr) throws Exception;
}
