/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.utils.hlpstruct;

import com.fin.httrader.utils.HtUtils;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;


/**
 *
 * @author DanilinS
 */
public class HtDrawableObject {
	// nested classes

	
	public static class LineIndicator {

		public LineIndicator()
		{
		}

		public LineIndicator(String name, double value, int color)
		{
			name_m.append(name);
			value_m = value;
			color_m = color;
		}
		
		public StringBuilder name_m = new StringBuilder();
		public double value_m = -1;
		public int color_m = -1;
	}

	public static class Print {

		public double bid_m = -1;
		public double ask_m = -1;
		public double volume_m = -1;
		public int color_m = -1; // this is color showing ask deal, bead deal or in the spread deal
		public int operation_m = -1;
	}

	public static class OHLC {

		public double open_m = -1;
		public double high_m = -1;
		public double low_m = -1;
		public double close_m = -1;
		public double volume_m = -1;
	}

	public static class Text {

		public StringBuilder shortText_m = new StringBuilder(); // up to 32 symbols
		public StringBuilder text_m = new StringBuilder(); // up to 8K symbols
		public int priority_m = 0; // get from alert priority
	}

	public static class Level1 {

		public double best_bid_price_m = -1;
		public double best_bid_volume_m = -1;
		public double best_ask_price_m = -1;
		public double best_ask_volume_m = -1;
	}
	// types
	public static final int MAX_INDIC = 7;
	//
	public static final int DO_DUMMY = 0;
	public static final int DO_LINE_INDIC = 1;
	public static final int DO_PRINT = 2;
	public static final int DO_OHLC = 3;
	public static final int DO_TEXT = 4;
	public static final int DO_LEVEL1 = 5;
	// -------------
	// members
	private int type_m = DO_DUMMY;
	private long time_m = -1;
	private StringBuilder symbol_m = new StringBuilder();
	private StringBuilder provider_m = new StringBuilder();
	private StringBuilder name_m = new StringBuilder();
	private Object data_m = null;
	// this is run name
	private StringBuilder runName_m = new StringBuilder();
	private static Integer[] allTypes_m = new Integer[]{DO_LINE_INDIC, DO_PRINT, DO_OHLC, DO_TEXT, DO_LEVEL1};
	private static Map<Integer, String> allTypesMap_m = new HashMap<Integer, String>();
	static {
		for(int i = 0; i < allTypes_m.length; i++) {
			allTypesMap_m.put(allTypes_m[i], getDrawableObjectName(allTypes_m[i]));
		}
	}

	public String toSmallDebugString()
	{
		StringBuilder out = new StringBuilder("Drawable entry: [ ");
		out.append( runName_m );
		out.append(	" ] [ ");
		out.append( name_m );
		out.append(	" ] [ ");
		out.append( provider_m );
		out.append(	" ] [ ");
		out.append( symbol_m );
		out.append(	" ] [ ");
		out.append(this.getDrawableObjectName(type_m));
		out.append(" ]" );
		

		return out.toString();
	}

	public static Integer[] getAllTypes() {
		return allTypes_m;
	}

	public static Map<Integer, String> getAllTypesMap() {
		return allTypesMap_m;
	}

	public static String getDrawableObjectName(int dot) {
		switch (dot) {
			case DO_LINE_INDIC:
				return "DO_LINE";
			case DO_PRINT:
				return "DO_PRINT";
			case DO_OHLC:
				return "DO_OHLC";
			case DO_TEXT:
				return "DO_TEXT";
			case DO_LEVEL1:
				return "DO_LEVEL1";
			default:
				return "";
		}

	}

	//
	public HtDrawableObject() {
	}

	public boolean isValid() {
		if (runName_m.length() > 0 && provider_m.length() > 0 && time_m > 0) {
			return true;
		} else {
			return false;
		}
	}

	public void clear() {
		data_m = null;
		type_m = DO_DUMMY;
		time_m = -1;

		symbol_m.setLength(0);
		provider_m.setLength(0);
		runName_m.setLength(0);
	}

	public void create(HtDrawableObject src) {

		this.setSymbol(src.getSymbol());
		this.setProvider(src.getProvider());
		this.setType(src.getType());
		this.setTime(src.getTime());
		this.setRunName(src.getRunName());


		switch (type_m) {
			case DO_LINE_INDIC: {

				((List<LineIndicator>) data_m).clear();
				((List<LineIndicator>) data_m).addAll(src.getAsLineIndicator());

				break;
			}
			case DO_PRINT: {
				Print data = (Print) data_m;
				data.ask_m = src.getAsPrint().ask_m;
				data.bid_m = src.getAsPrint().bid_m;
				data.color_m = src.getAsPrint().color_m;
				data.operation_m = src.getAsPrint().operation_m;
				data.volume_m = src.getAsPrint().volume_m;

				break;
			}
			case DO_OHLC: {
				OHLC data = (OHLC) data_m;

				data.open_m = src.getAsOHLC().open_m;
				data.high_m = src.getAsOHLC().high_m;
				data.low_m = src.getAsOHLC().low_m;
				data.close_m = src.getAsOHLC().close_m;
				data.volume_m = src.getAsOHLC().volume_m;

				break;
			}
			case DO_TEXT: {
				Text tdata = (Text) data_m;
				tdata.priority_m = src.getAsText().priority_m;
				tdata.shortText_m.setLength(0);
				tdata.shortText_m.append(src.getAsText().shortText_m.toString());
				tdata.text_m.setLength(0);
				tdata.text_m.append(src.getAsText().text_m.toString());

				break;
			}
			case DO_LEVEL1: {
				Level1 tdata = (Level1) data_m;
				tdata.best_ask_price_m = src.getAsLevel1().best_ask_price_m;
				tdata.best_ask_volume_m = src.getAsLevel1().best_ask_volume_m;
				tdata.best_bid_price_m = src.getAsLevel1().best_bid_price_m;
				tdata.best_bid_volume_m = src.getAsLevel1().best_bid_volume_m;

				break;
			}
		}
	}

	public int getType() {
		return type_m;
	}

	public String getSymbol() {
		return symbol_m.toString();
	}

	public String getProvider() {
		return provider_m.toString();
	}

	public void setSymbol(String symbol) {
		symbol_m.setLength(0);
		symbol_m.append(symbol);
	}

	public void setProvider(String provider) {
		provider_m.setLength(0);
		provider_m.append(provider);
	}

	public void setRunName(String runName)
	{
		runName_m.setLength(0);
		runName_m.append( runName );
	}

	public String getRunName()
	{
		return runName_m.toString();
	}

	public long getTime() {
		return time_m;
	}

	public void setTime(long timeval) {
		time_m = timeval;
	}

	public String getName() {
		return name_m.toString();
	}

	public void setName(String name) {
		name_m.setLength(0);
		name_m.append(name);
	}

	public List<LineIndicator> getAsLineIndicator(boolean setType) {
		if (setType) {
			setType(DO_LINE_INDIC);
		}
		return (List<LineIndicator>) data_m;
	}

	public List<LineIndicator> getAsLineIndicator() {

		return (List<LineIndicator>) data_m;
	}

	public Print getAsPrint(boolean setType) {
		if (setType) {
			setType(DO_PRINT);
		}
		return (Print) data_m;
	}

	public Print getAsPrint() {

		return (Print) data_m;
	}

	public OHLC getAsOHLC(boolean setType) {
		if (setType) {
			setType(DO_OHLC);
		}
		return (OHLC) data_m;
	}

	public OHLC getAsOHLC() {

		return (OHLC) data_m;
	}

	public Text getAsText(boolean setType) {
		if (setType) {
			setType(DO_TEXT);
		}
		return (Text) data_m;
	}

	public Text getAsText() {

		return (Text) data_m;
	}

	public Level1 getAsLevel1(boolean setType) {
		if (setType) {
			setType(DO_LEVEL1);
		}
		return (Level1) data_m;
	}

	public Level1 getAsLevel1() {

		return (Level1) data_m;
	}

	// ---------------------------------
	// helpers
	private void setType(int type) {
		// New type?
		//
		if (type == type_m) // No; bail out
		{
			return;
		}

		type_m = type;

		switch (type_m) {
			case DO_LINE_INDIC: {
				data_m = new ArrayList<LineIndicator>();
				break;
			}
			case DO_PRINT: {
				data_m = new Print();
				break;
			}
			case DO_OHLC: {
				data_m = new OHLC();
				break;
			}
			case DO_TEXT: {
				data_m = new Text();
				break;
			}
			case DO_LEVEL1: {
				data_m = new Level1();
				break;
			}

		}
	}

	// -----------------------------
	public static void drawableObjectToParameter(HtDrawableObject dobj, XmlParameter paramout) throws Exception {
		paramout.clear();
		paramout.setCommandName("drawobj");
		paramout.setInt("type", dobj.getType());
		paramout.setDate("time", dobj.getTime());
		paramout.setString("symbol", dobj.getSymbol());
		paramout.setString("provider", dobj.getProvider());
		paramout.setString("name", dobj.getName());
		paramout.setString("run_name", dobj.getRunName());


		if (dobj.getType() == HtDrawableObject.DO_LINE_INDIC) {
			List<LineIndicator> dobj_list = dobj.getAsLineIndicator();
			ArrayList<XmlParameter> xmlparam = new ArrayList<XmlParameter>();

			for (int i = 0; i < dobj_list.size(); i++) {
				LineIndicator indic_i = dobj_list.get(i);
				XmlParameter indic_xml_i = new XmlParameter();

				indic_xml_i.setDouble("val", indic_i.value_m);
				indic_xml_i.setInt("color", indic_i.color_m);
				indic_xml_i.setString("name", indic_i.name_m.toString());

				xmlparam.add(indic_xml_i);


			}


			paramout.setXmlParameterList("line_indic", xmlparam);


		} else if (dobj.getType() == HtDrawableObject.DO_PRINT) {
			paramout.setDouble("print_bid", dobj.getAsPrint().bid_m);
			paramout.setDouble("print_ask", dobj.getAsPrint().ask_m);
			paramout.setDouble("print_volume", dobj.getAsPrint().volume_m);
			paramout.setInt("print_color", dobj.getAsPrint().color_m);
			paramout.setInt("print_operation", dobj.getAsPrint().operation_m);
		} else if (dobj.getType() == HtDrawableObject.DO_OHLC) {
			paramout.setDouble("ohlc_open", dobj.getAsOHLC().open_m);
			paramout.setDouble("ohlc_high", dobj.getAsOHLC().high_m);
			paramout.setDouble("ohlc_low", dobj.getAsOHLC().low_m);
			paramout.setDouble("ohlc_close", dobj.getAsOHLC().close_m);
			paramout.setDouble("ohlc_volume", dobj.getAsOHLC().volume_m);
		} else if (dobj.getType() == HtDrawableObject.DO_TEXT) {

			
			paramout.setString("short_text", HtUtils.wrapToCDataTags(dobj.getAsText().shortText_m.toString()));
			paramout.setString("text", HtUtils.wrapToCDataTags(dobj.getAsText().text_m.toString()));
			
			paramout.setInt("priority", dobj.getAsText().priority_m);

		} else if (dobj.getType() == HtDrawableObject.DO_LEVEL1) {
			paramout.setDouble("level1_best_bid_price", dobj.getAsLevel1().best_bid_price_m);
			paramout.setDouble("level1_best_bid_volume", dobj.getAsLevel1().best_bid_volume_m);
			paramout.setDouble("level1_best_ask_price", dobj.getAsLevel1().best_ask_price_m);
			paramout.setDouble("level1_best_ask_volume", dobj.getAsLevel1().best_ask_volume_m);

		} else {
			assert (false);
		}



	}

	public static void drawableObjectFromParameter(XmlParameter paramin, HtDrawableObject dobj) throws Exception {
		dobj.clear();
		dobj.setTime(paramin.getDate("time"));
		dobj.setSymbol(paramin.getString("symbol"));
		dobj.setProvider(paramin.getString("provider"));
		dobj.setName(paramin.getString("name"));
		dobj.setRunName(paramin.getString("run_name"));

		int type = (int) paramin.getInt("type");

		if (type == HtDrawableObject.DO_LINE_INDIC) {

			List<LineIndicator> lineindic = dobj.getAsLineIndicator(true);
			lineindic.clear();

			List<XmlParameter> xmlparamlist = paramin.getXmlParameterList("line_indic");
			for (int i = 0; i < xmlparamlist.size(); i++) {
				XmlParameter xmlparam_i = xmlparamlist.get(i);

				HtDrawableObject.LineIndicator entry_i = new HtDrawableObject.LineIndicator();
				entry_i.name_m.append(xmlparam_i.getString("name"));
				entry_i.color_m = (int) xmlparam_i.getInt("color");
				entry_i.value_m = xmlparam_i.getDouble("val");

				lineindic.add(entry_i);
			}




		} else if (type == HtDrawableObject.DO_PRINT) {
			dobj.getAsPrint(true).ask_m = paramin.getDouble("print_ask");
			dobj.getAsPrint().bid_m = paramin.getDouble("print_bid");
			dobj.getAsPrint().volume_m = paramin.getDouble("print_volume");
			dobj.getAsPrint().color_m = (int) paramin.getInt("print_color");
			dobj.getAsPrint().operation_m = (int) paramin.getInt("print_operation");
		} else if (type == HtDrawableObject.DO_OHLC) {
			dobj.getAsOHLC(true).open_m = paramin.getDouble("ohlc_open");
			dobj.getAsOHLC().high_m = paramin.getDouble("ohlc_high");
			dobj.getAsOHLC().low_m = paramin.getDouble("ohlc_low");
			dobj.getAsOHLC().close_m = paramin.getDouble("ohlc_close");
			dobj.getAsOHLC().volume_m = paramin.getDouble("ohlc_volume");
		} else if (type == HtDrawableObject.DO_TEXT) {
			dobj.getAsText(true).priority_m = (int) paramin.getInt("priority");

			dobj.getAsText().shortText_m.setLength(0);
			dobj.getAsText().shortText_m.append(paramin.getString("short_text"));

			dobj.getAsText().text_m.setLength(0);
			dobj.getAsText().text_m.append(paramin.getString("text"));

		} else if (type == HtDrawableObject.DO_LEVEL1) {
			dobj.getAsLevel1(true).best_ask_price_m = paramin.getDouble("level1_best_ask_price");
			dobj.getAsLevel1().best_bid_price_m = paramin.getDouble("level1_best_bid_price");
			dobj.getAsLevel1().best_ask_volume_m = paramin.getDouble("level1_best_ask_volume");
			dobj.getAsLevel1().best_bid_volume_m = paramin.getDouble("level1_best_bid_volume");
		} else {
			assert (false);
		}


	}

	public static void createXmlEventDrawableObject(HtDrawableObject dobject, XmlEvent event) {
		event.clear();


		event.getAsDrawableObject(true).create(dobject);
		event.setEventType(XmlEvent.ET_DrawableObject);
	}
}
