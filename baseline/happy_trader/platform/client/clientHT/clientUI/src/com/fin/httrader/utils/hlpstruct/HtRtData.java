/*
 * HtRtData.java
 *
 * Created on December 3, 2006, 5:10 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.utils.hlpstruct;

import com.fin.httrader.configprops.HtSaveAnnotation;
import com.fin.httrader.hlpstruct.RtDataOperation;
import com.fin.httrader.hlpstruct.RtDataPrintColor;
import com.fin.httrader.managers.RtDatabaseManager;
import com.fin.httrader.utils.*;

/**
 *
 * @author Administrator
 */
public class HtRtData {

	// constants for sync events
	public static final int SYNC_EVENT_PROVIDER_START =					 1;
	public static final int SYNC_EVENT_PROVIDER_FINISH =				 2;
	public static final int SYNC_EVENT_PROVIDER_TRANSMIT_ERROR = 3;
	public static final int SYNC_EVENT_CUSTOM_MESSAGE = 4;


	// types of RT data
	static public final int RT_Type = 0;
	static public final int HST_Type = 1;
	static public final int HST_Dummy = 2;
					

	public static final int MAX_SYMB_LENGTH = 32;



	// -------------------------------
	@HtSaveAnnotation
	public double bid_m = -1;
	
	@HtSaveAnnotation
	public double ask_m = -1;
	
	@HtSaveAnnotation
	public int color_m = RtDataPrintColor.CT_NA; // id last deal done with bid or ask price
	
	@HtSaveAnnotation
	public int operation_m  = RtDataOperation.OT_NA;
	// EXPECTED to bid & ask OHCL

	// ask
	@HtSaveAnnotation
	public double open_m = -1;
	
	@HtSaveAnnotation
	public double high_m = -1;
	
	@HtSaveAnnotation
	public double low_m = -1;
	
	@HtSaveAnnotation
	public double close_m = -1;

	// bid
	@HtSaveAnnotation
	public double open2_m = -1;
	
	@HtSaveAnnotation
	public double high2_m = -1;
	
	@HtSaveAnnotation
	public double low2_m = -1;
	
	@HtSaveAnnotation
	public double close2_m = -1;
	
	@HtSaveAnnotation
	public double volume_m = -1;
	
	@HtSaveAnnotation
	public int type_m = HST_Dummy;
	
	@HtSaveAnnotation
	public StringBuilder symbol_m = new StringBuilder();
	
	@HtSaveAnnotation
	public StringBuilder provider_m = new StringBuilder();
	
	@HtSaveAnnotation
	public long time_m = -1;
	
	@HtSaveAnnotation
	public int aflag_m = RtDatabaseManager.HST_DATA_CLOSED;
	
	// tag for any purpuse
	//@HtSaveAnnotation
	//public long tag_m = -1;

	public void create(HtRtData src) {


		this.setSymbol(src.getSymbol());
		this.setProvider(src.getProvider());
		this.volume_m = src.volume_m;
		this.type_m = src.type_m;
		this.time_m = src.time_m;
		this.aflag_m = src.aflag_m;
		this.ask_m = src.ask_m;
		this.bid_m = src.bid_m;
		this.color_m = src.color_m;
		this.operation_m = src.operation_m;

		this.open_m = src.open_m;
		this.high_m = src.high_m;
		this.low_m = src.low_m;
		this.close_m = src.close_m;

		this.open2_m = src.open2_m;
		this.high2_m = src.high2_m;
		this.low2_m = src.low2_m;
		this.close2_m = src.close2_m;
	}

	public int getType() {
		return type_m;
	}

	/** Creates a new instance of HtRtData */
	public HtRtData() {
	}

	public HtRtData(int type) {
		type_m = type;
	}
	// copy ctor

	public HtRtData(HtRtData src) {
		create(src);
	}

	public boolean isValid() {
		return ((time_m > 0) && (type_m >= 0) && symbol_m.length() > 0);
	}

	public HtRtData(String provider, String symbol, long time, double bid, double ask, double volume, int color, int operation) {
		bid_m = bid;
		ask_m = ask;
		color_m = color;
		operation_m = operation;
		volume_m = volume;
		time_m = time;

		provider_m.setLength(0);
	
		symbol_m.append(symbol);
		provider_m.append( provider );

		type_m = RT_Type;
	}

	public HtRtData(String provider, String symbol, long time, double open, double high, double low, double close, double volume) {
		open_m = open;
		high_m = high;
		low_m = low;
		close_m = close;

		open2_m = -1;
		high2_m = -1;
		low2_m = -1;
		close2_m = -1;

		volume_m = volume;

		symbol_m.append(symbol);
		provider_m.append( provider );

		type_m = HST_Type;
	}

	public HtRtData(String provider, String symbol, long time, double open, double high, double low, double close,
					double open2, double high2, double low2, double close2,
					double volume) {
		open_m = open;
		high_m = high;
		low_m = low;
		close_m = close;


		open2_m = open2;
		high2_m = high2;
		low2_m = low2;
		close2_m = close2;

		volume_m = volume;

		symbol_m.append(symbol);
		provider_m.append( provider );
		
		type_m = HST_Type;
	}

	// this ctor to adopt to newRowArrived :)
	public HtRtData(
			 long ttime,
       String tsymbol,
       String tprovider,
       int ttype,
       int aflag,
       double tbid,
       double task,
       double topen,
       double thigh,
       double tlow,
       double tclose,
       double topen2,
       double thigh2,
       double tlow2,
       double tclose2,
       double tvolume,
       long tid,
			 int color,
			 int operation
		)
	{
		open_m = topen;
		high_m = thigh;
		low_m = tlow;
		close_m = tclose;

		open2_m = topen2;
		high2_m = thigh2;
		low2_m = tlow2;
		close2_m = tclose2;
		volume_m = tvolume;

		symbol_m.append(tsymbol);
		provider_m.append( tprovider );
		this.ask_m = task;
		this.bid_m = tbid;
		this.type_m = ttype;
		this.time_m = ttime;
		this.operation_m = operation;
		this.color_m = color;
		this.aflag_m = aflag;

	}

	public void clear() {
		bid_m = -1;
		ask_m = -1;
		color_m = RtDataPrintColor.CT_NA;
		operation_m = RtDataOperation.OT_NA;

		open_m = -1;
		high_m = -1;
		low_m = -1;
		close_m = -1;

		open2_m = -1;
		high2_m = -1;
		low2_m = -1;
		close2_m = -1;

		volume_m = -1;
		type_m = -1;
		time_m = -1;
		symbol_m.setLength(0);
		provider_m.setLength(0);
	}

	public void setSymbol(String symbol) {
		symbol_m.setLength(0);
		symbol_m.append(symbol);
	}

	public String getSymbol() {
		return symbol_m.toString();
	}

	public void setProvider(String provider) {
		provider_m.setLength(0);
		provider_m.append(provider);
	}

	public String getProvider() {
		return provider_m.toString();
	}

	public String toString_HistoryBidType()
	{
		StringBuilder out = new StringBuilder();
		out.append(HtDateTimeUtils.time2SimpleString_Gmt(time_m));
		out.append(" - ");
		out.append(HtUtils.formatPriceValue(open2_m, 4, false));
		out.append(" ");
		out.append(HtUtils.formatPriceValue(high2_m, 4, false));
		out.append(" ");
		out.append(HtUtils.formatPriceValue(low2_m, 4, false));
		out.append(" ");
		out.append(HtUtils.formatPriceValue(close2_m, 4, false));

		return out.toString();
	}

	@Override
	public String toString() {
		StringBuilder out = new StringBuilder();
		out.append("<").append(provider_m).append("> - ");
		out.append("<").append(symbol_m).append("> - ");
		if (type_m == RT_Type) {
			out.append("RT");
		} else if (type_m == HST_Type) {
			out.append("HIST");
		} else {
			out.append("N/A");
		}

		out.append(" - ").append(HtDateTimeUtils.time2String_Gmt(time_m)).append(" - ");

		return out.toString();
	}

	//
	public static void rtDataToParameter(HtRtData rtdata, XmlParameter paramout) throws Exception {
		paramout.clear();

		paramout.clear();
		paramout.setCommandName("rt_packet");
		paramout.setString("symbol", rtdata.symbol_m.toString());
		paramout.setString("provider", rtdata.provider_m.toString());
		paramout.setDate("time", rtdata.time_m);
		paramout.setInt("type", rtdata.type_m);
		paramout.setInt("aflag", rtdata.aflag_m);

		paramout.setDouble("volume", rtdata.volume_m);
		if (rtdata.type_m == HtRtData.RT_Type) {
			paramout.setDouble("bid", rtdata.bid_m);
			paramout.setDouble("ask", rtdata.ask_m);
			paramout.setInt("color", rtdata.color_m);
			paramout.setInt("operation", rtdata.operation_m);

		} else if (rtdata.type_m == HtRtData.HST_Type) {
			paramout.setDouble("open", rtdata.open_m);
			paramout.setDouble("high", rtdata.high_m);
			paramout.setDouble("low", rtdata.low_m);
			paramout.setDouble("close", rtdata.close_m);

			paramout.setDouble("open2", rtdata.open2_m);
			paramout.setDouble("high2", rtdata.high2_m);
			paramout.setDouble("low2", rtdata.low2_m);
			paramout.setDouble("close2", rtdata.close2_m);

		} else {
			//throw new HtException("HtRtData", "rtDataToParameter", "Invalid ticker type: " + rtdata.toString());
		}
	}

	public static void rtDataFromParameter(XmlParameter paramin, HtRtData rtdata) throws Exception {

		rtdata.setSymbol(paramin.getString("symbol"));
		rtdata.setProvider(paramin.getString("provider"));

		rtdata.type_m = (int) paramin.getInt("type");
		rtdata.time_m = paramin.getDate("time");
		rtdata.volume_m = paramin.getDouble("volume");
		rtdata.aflag_m = (int) paramin.getInt("aflag");

		if (rtdata.type_m == HtRtData.RT_Type) {
			rtdata.bid_m = paramin.getDouble("bid");
			rtdata.ask_m = paramin.getDouble("ask");
			rtdata.color_m = (int) paramin.getInt("color");
			rtdata.operation_m = (int) paramin.getInt("operation");
		} else if (rtdata.type_m == HtRtData.HST_Type) {
			rtdata.open_m = paramin.getDouble("open");
			rtdata.high_m = paramin.getDouble("high");
			rtdata.low_m = paramin.getDouble("low");
			rtdata.close_m = paramin.getDouble("close");

			rtdata.open2_m = paramin.getDouble("open2");
			rtdata.high2_m = paramin.getDouble("high2");
			rtdata.low2_m = paramin.getDouble("low2");
			rtdata.close2_m = paramin.getDouble("close2");

		} else {
			//throw new HtException("HtRtData", "rtDataFromParameter", "Invalid ticker type: " + rtdata.toString());
		}
	}

	// previous versions
	public static HtRtData deserializeFromBinary(byte[] buffer) throws Exception
	{
		HtRtData rtdata = new HtRtData();

		if (buffer == null || buffer.length != 4+8*12+2*4+2*MAX_SYMB_LENGTH)
			throw new HtException("HtRtData", "deserializeFromBinary", "Invalid buffer length");

		rtdata.color_m = (int)buffer[0];
		rtdata.type_m = (int)buffer[1];
		rtdata.operation_m = (int)buffer[2];
		rtdata.aflag_m = (int)buffer[3];

		rtdata.time_m = (long)HtMathUtils.readDoubleValueFromBytes(buffer, 4);

		rtdata.open_m = HtMathUtils.readDoubleValueFromBytes(buffer, 4+8);
		rtdata.high_m = HtMathUtils.readDoubleValueFromBytes(buffer, 4+8*2);
		rtdata.low_m = HtMathUtils.readDoubleValueFromBytes(buffer, 4+8*3);
		rtdata.close_m = HtMathUtils.readDoubleValueFromBytes(buffer, 4+8*4);

		rtdata.open2_m = HtMathUtils.readDoubleValueFromBytes(buffer, 4+8*5);
		rtdata.high2_m = HtMathUtils.readDoubleValueFromBytes(buffer, 4+8*6);
		rtdata.low2_m = HtMathUtils.readDoubleValueFromBytes(buffer, 4+8*7);
		rtdata.close2_m = HtMathUtils.readDoubleValueFromBytes(buffer, 4+8*8);

		rtdata.volume_m = HtMathUtils.readDoubleValueFromBytes(buffer, 4+8*9);
		rtdata.bid_m = HtMathUtils.readDoubleValueFromBytes(buffer, 4+8*10);
		rtdata.ask_m = HtMathUtils.readDoubleValueFromBytes(buffer, 4+8*11);

		int provider_lenth = HtMathUtils.convertByteToInt4(buffer, 4+8*12);
		int symbol_lenth = HtMathUtils.convertByteToInt4(buffer, 4+8*12+4);

		byte[] provider_b = new byte[provider_lenth];
		byte[] symbol_b = new byte[symbol_lenth];

		
		System.arraycopy(buffer, 4+8*12+2*4, provider_b, 0, provider_lenth);
		System.arraycopy(buffer, 4+8*12+2*4+MAX_SYMB_LENGTH, symbol_b, 0, symbol_lenth);

		rtdata.setProvider(new String(provider_b, "ISO-8859-1"));
		rtdata.setSymbol(new String(symbol_b, "ISO-8859-1"));
		
		return rtdata;
	}

	// previous versions
	public static byte[] serializeToBinary(HtRtData rtdata) throws Exception
	{
		// this wraps HtRtData
		// time

		byte[] buffer = new byte[4+8*12+2*4+2*MAX_SYMB_LENGTH];

		buffer[0] = (byte)rtdata.color_m;
		buffer[1] = (byte)rtdata.type_m;
		buffer[2] = (byte)rtdata.operation_m;
		buffer[3] = (byte)rtdata.aflag_m;

		

		System.arraycopy(HtMathUtils.readBytesValueFromDouble( (double)rtdata.time_m ), 0, buffer, 4, 8);
		System.arraycopy(HtMathUtils.readBytesValueFromDouble(rtdata.open_m), 0, buffer, 4+8, 8);
		System.arraycopy(HtMathUtils.readBytesValueFromDouble(rtdata.high_m), 0, buffer, 4+8*2, 8);
		System.arraycopy(HtMathUtils.readBytesValueFromDouble(rtdata.low_m), 0, buffer, 4+8*3, 8);
		System.arraycopy(HtMathUtils.readBytesValueFromDouble(rtdata.close_m), 0, buffer, 4+8*4, 8);

		System.arraycopy(HtMathUtils.readBytesValueFromDouble(rtdata.open2_m), 0, buffer, 4+8*5, 8);
		System.arraycopy(HtMathUtils.readBytesValueFromDouble(rtdata.high2_m), 0, buffer, 4+8*6, 8);
		System.arraycopy(HtMathUtils.readBytesValueFromDouble(rtdata.low2_m), 0, buffer, 4+8*7, 8);
		System.arraycopy(HtMathUtils.readBytesValueFromDouble(rtdata.close2_m), 0, buffer, 4+8*8, 8);

		System.arraycopy(HtMathUtils.readBytesValueFromDouble(rtdata.volume_m), 0, buffer, 4+8*9, 8);

		System.arraycopy(HtMathUtils.readBytesValueFromDouble(rtdata.bid_m), 0, buffer, 4+8*10, 8);
		System.arraycopy(HtMathUtils.readBytesValueFromDouble(rtdata.ask_m), 0, buffer, 4+8*11, 8);

		byte[] provider_b = rtdata.getProvider().getBytes("ISO-8859-1");
		byte[] symbol_b = rtdata.getSymbol().getBytes("ISO-8859-1");

		int provider_lenth = provider_b.length > MAX_SYMB_LENGTH ? MAX_SYMB_LENGTH : provider_b.length;
		int symbol_length = symbol_b.length > MAX_SYMB_LENGTH ? MAX_SYMB_LENGTH : symbol_b.length;

		byte[] provider_lenth_b = HtMathUtils.convertIntToByte4(provider_lenth);
		byte[] symbol_length_b  = HtMathUtils.convertIntToByte4(symbol_length);

		System.arraycopy(provider_lenth_b, 0, buffer, 4+8*12, 4);
		System.arraycopy(symbol_length_b, 0, buffer, 4+8*12+4, 4);

		System.arraycopy(provider_b, 0, buffer, 4+8*12+2*4, provider_lenth);
		System.arraycopy(symbol_b, 0, buffer, 4+8*12+2*4+MAX_SYMB_LENGTH, symbol_length);


		return buffer;
	}
	
}
