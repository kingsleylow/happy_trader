/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.utils.hlpstruct;

import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtUtils;

/**
 *
 * @author DanilinS
 * This is level1 data provide level 1- best bid and ask prices and volumes
 */
public class HtLevel1Data {

	public double best_bid_price_m = -1;
	public double best_bid_volume_m = -1;
	public double best_ask_price_m = -1;
	public double best_ask_volume_m = -1;
	public double open_interest_m = -1;
	public long time_m = -1;
	public StringBuilder symbol_m = new StringBuilder();
	public StringBuilder provider_m = new StringBuilder();

	// -------------------------------
	public void create(HtLevel1Data src) {

		this.setSymbol(src.getSymbol());
		this.setProvider(src.getProvider());

		this.time_m = src.time_m;
		this.best_bid_price_m = src.best_bid_price_m;
		this.best_ask_price_m = src.best_ask_price_m;
		this.best_bid_volume_m = src.best_bid_volume_m;
		this.best_ask_volume_m = src.best_ask_volume_m;
		this.open_interest_m = src.open_interest_m;

	}

	public HtLevel1Data() {
	}

	public HtLevel1Data(HtLevel1Data src) {
		create(src);
	}

	public boolean isValid() {
		return ((time_m > 0) && symbol_m.length() > 0);
	}

	public void clear() {
		this.time_m = -1;
		this.best_bid_price_m = -1;
		this.best_ask_price_m = -1;
		this.best_bid_volume_m = -1;
		this.best_ask_volume_m = -1;
		this.open_interest_m = -1;

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

	@Override
	public String toString() {
		StringBuilder out = new StringBuilder();
		out.append("<");
		out.append(provider_m);
		out.append("> - ");
		out.append("<");
		out.append(symbol_m);
		out.append("> - ");
		out.append("best bid/volume@ask/volume: ");
		out.append(this.best_bid_price_m);
		out.append("/");
		out.append(this.best_bid_volume_m);
		out.append("@");
		out.append(this.best_ask_price_m);
		out.append("/");
		out.append(this.best_ask_volume_m);
		out.append(" [ ");
		out.append(this.open_interest_m);
		out.append(" ] ");

		out.append(" - ").append(HtDateTimeUtils.time2String_Gmt(time_m));

		return out.toString();

	}

	//
	public static void level1DataToParameter(HtLevel1Data rtdata, XmlParameter paramout) throws Exception {
		paramout.clear();

		paramout.setCommandName("level1_packet");
		paramout.setString("symbol", rtdata.symbol_m.toString());
		paramout.setString("provider", rtdata.provider_m.toString());
		paramout.setDate("time", rtdata.time_m);

		paramout.setDouble("best_bid_price", rtdata.best_bid_price_m);
		paramout.setDouble("best_ask_price", rtdata.best_ask_price_m);

		paramout.setDouble("best_bid_volume", rtdata.best_bid_volume_m);
		paramout.setDouble("best_ask_volume", rtdata.best_ask_volume_m);
		paramout.setDouble("open_interest", rtdata.open_interest_m);
	}

	public static void level1DataFromParameter(XmlParameter paramin, HtLevel1Data rtdata) throws Exception {

		rtdata.setSymbol(paramin.getString("symbol"));
		rtdata.setProvider(paramin.getString("provider"));
		rtdata.time_m = paramin.getDate("time");

		rtdata.best_ask_price_m = paramin.getDouble("best_ask_price");
		rtdata.best_bid_price_m = paramin.getDouble("best_bid_price");
		rtdata.best_ask_volume_m = paramin.getDouble("best_ask_volume");
		rtdata.best_bid_volume_m = paramin.getDouble("best_bid_volume");
		rtdata.open_interest_m = paramin.getDouble("open_interest");

	}
}
