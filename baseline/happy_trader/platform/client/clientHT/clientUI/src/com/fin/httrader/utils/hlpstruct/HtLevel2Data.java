/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.utils.hlpstruct;

import com.fin.httrader.utils.HtDateTimeUtils;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

/**
 *
 * @author Victor_Zoubok
 */
public class HtLevel2Data {

	public static class Htlevel2Entry implements Comparable<HtLevel2Data.Htlevel2Entry> {

		public static final int IS_BID = -1;
		public static final int IS_ASK = 1;
		public static final int IS_DUMMY = 0;

		public Htlevel2Entry(double volume, double price, boolean isSelf, int entryType) {
			volume_m = volume;
			self_m = isSelf;
			entryType_m = entryType;
			price_m = price;
		}

		public Htlevel2Entry(Htlevel2Entry src) {
			volume_m = src.volume_m;
			self_m = src.self_m;
			entryType_m = src.entryType_m;
			price_m = src.price_m;
		}

		public Htlevel2Entry() {
		}

		public void create(Htlevel2Entry src) {
			volume_m = src.volume_m;
			self_m = src.self_m;
			entryType_m = src.entryType_m;
			price_m = src.price_m;
			
		}

		public void clear() {
			entryType_m = IS_DUMMY;
			self_m = false;
			volume_m = -1.0;
			price_m = -1.0;
		}

		public double getPrice() {
			return price_m;
		}

		public int getType() {
			return entryType_m;
		}

		public boolean isSelf() {
			return self_m;
		}

		public double getVolume() {
			return volume_m;
		}
		private double price_m;
		private double volume_m;
		private boolean self_m;
		private int entryType_m = IS_DUMMY;

		@Override
		public int compareTo(Htlevel2Entry o) {
			return new Double(this.price_m).compareTo(new Double(o.price_m));
		}
	}
	// main class
	private StringBuilder symbol_m = new StringBuilder();
	private StringBuilder provider_m = new StringBuilder();
	private long time_m = -1;
	private Htlevel2Entry[] data_m = null;
	private int idxBestBid_m = -1;
	private int idxBestAsk_m = -1;
			

	// ctor
	public HtLevel2Data() {
	}

	public HtLevel2Data(HtLevel2Data src) {
		create(src);
	}

	public Htlevel2Entry[] getData() {
		return data_m;
	}
	
	public Htlevel2Entry getData(int idx) {
		return data_m[idx];
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

	public long getTime() {
		return time_m;
	}

	public void setTime(long time) {
		time_m = time;
	}

	public void init(Htlevel2Entry[] data) {
		data_m = null;
		if (data != null && data.length > 0) {
			copyArray(data);
		}
	}
	
	public void setEntry(Htlevel2Entry entry, int idx)
	{
		//if (data_m != null ) {
		//	if (data_m.length > idx && idx >= 0) {
				data_m[idx] = entry;
		//	}
		//}
	}
	
	public void setEntry(double volume, double price, boolean isSelf, int entryType, int idx)
	{
		//if (data_m != null ) {
		//	if (data_m.length > idx && idx >= 0) {
				data_m[idx] = new Htlevel2Entry(volume, price, isSelf, entryType);
		//	}
		//}
	}
	
	public void init(int numEntries) {
		if (numEntries > 0)
			data_m = new Htlevel2Entry[numEntries];
		else
			data_m = null;
	}
	
	public void init(int numEntries, int idxBestBid, int idxBestAsk) {
		idxBestBid_m = idxBestBid;
		idxBestAsk_m = idxBestAsk;
		
		if (numEntries > 0)
			data_m = new Htlevel2Entry[numEntries];
		else
			data_m = null;
	}
	
	public int getSize()
	{
		if (data_m == null)
			return 0;
		else
			return data_m.length;
	}
	
	public int getBestBidIndex()
	{
	  return idxBestBid_m;
	}
	
	public int getBestAskIndex()
	{
	  return idxBestAsk_m;
	}
	
	public void setBestBidIndex(int idxBestBid)
	{
	  idxBestBid_m = idxBestBid;
	}
	
	public void setBestAskIndex(int idxBestAsk)
	{
	  idxBestAsk_m = idxBestAsk;
	}
	
	public void setBestBidAskIndexes(int idxBestBid, int idxBestAsk)
	{
	  idxBestBid_m = idxBestBid;
	  idxBestAsk_m = idxBestAsk;
	}

	public void create(HtLevel2Data src) {

		if (src.data_m != null && src.data_m.length > 0) {
			copyArray(src.data_m);
		}

		setTime(src.getTime());
		setSymbol(src.getSymbol());
		setProvider(src.getProvider());
		idxBestBid_m = src.idxBestBid_m;
		idxBestAsk_m = src.idxBestAsk_m;


	}
	
	@Override
	public String toString()
	{
		StringBuilder result = new StringBuilder();
		result.append(getProvider()).append(" - ").append(getSymbol()).append(" [ ").append(HtDateTimeUtils.time2String_Gmt(getTime())).append(" ] ->\n");
		
		
		if (data_m != null) {
			for(int  i =0; i < data_m.length; i++) {
				Htlevel2Entry e = getData(i);
				if (e== null) {
					result.append("Entry ").append(i).append(" is null, shouldn't happen!\n");
				}
				else {
					result.append(" price=").append(e.getPrice());
					result.append(" volume=").append(e.getVolume());
					result.append(" type=").append(e.getType());
					result.append(" is self=").append(e.isSelf()).append("\n");
					
				}
			}
		}
		
		return result.toString();
	}

	
	public static void level2DataToParameter(HtLevel2Data level2data, XmlParameter paramout) throws Exception {
		paramout.clear();

		paramout.clear();
		paramout.setCommandName("level2_packet");
		paramout.setString("symbol", level2data.getSymbol());
		paramout.setString("provider", level2data.getProvider());
		paramout.setDate("time", level2data.getTime());


		XmlParameter bids_param = new XmlParameter();
		XmlParameter asks_param = new XmlParameter();
		//

		List<Double> bid_price = new ArrayList<Double>();
		List<Double> bid_volume = new ArrayList<Double>();
		List<Long> bid_self = new ArrayList<Long>();

		List<Double> ask_price = new ArrayList<Double>();
		List<Double> ask_volume = new ArrayList<Double>();
		List<Long> ask_self = new ArrayList<Long>();

		if (level2data.data_m != null) {
			for (int i = 0; i < level2data.data_m.length; i++) {
				Htlevel2Entry e = level2data.data_m[i];
				if (e.getType() == Htlevel2Entry.IS_ASK) {
					ask_price.add(e.getPrice());
					ask_volume.add(e.getVolume());
					ask_self.add((long) (e.isSelf() ? 1 : 0));
				} else if (e.getType() == Htlevel2Entry.IS_BID) {
					bid_price.add(e.getPrice());
					bid_volume.add(e.getVolume());
					bid_self.add((long) (e.isSelf() ? 1 : 0));
				}
			}
		}


		bids_param.setDoubleList("bid_price", bid_price);
		bids_param.setDoubleList("bid_volume", bid_volume);
		bids_param.setIntList("bid_self", bid_self);

		asks_param.setDoubleList("ask_price", ask_price);
		asks_param.setDoubleList("ask_volume", ask_volume);
		asks_param.setIntList("ask_self", ask_self);


		paramout.setXmlParameter("bids", bids_param);
		paramout.setXmlParameter("asks", asks_param);

		//


	}

	public static void level2DataFromParameter(XmlParameter paramin, HtLevel2Data level2data) throws Exception {

		level2data.setSymbol(paramin.getString("symbol"));
		level2data.setProvider(paramin.getString("provider"));

		level2data.setTime(paramin.getDate("time"));

		XmlParameter bids_param = paramin.getXmlParameter("bids");
		XmlParameter asks_param = paramin.getXmlParameter("asks");

		List<Double> bid_price = bids_param.getDoubleList("bid_price");
		List<Double> bid_volume = bids_param.getDoubleList("bid_volume");
		List<Long> bid_self = bids_param.getIntList("bid_self");

		List<Double> ask_price = asks_param.getDoubleList("ask_price");
		List<Double> ask_volume = asks_param.getDoubleList("ask_volume");
		List<Long> ask_self = asks_param.getIntList("ask_self");

		int totalsize = ask_price.size() + bid_price.size();
		if (totalsize > 0) {
			
			level2data.data_m = new Htlevel2Entry[totalsize];
			int k = 0;
			
			for (int i = 0; i < bid_price.size(); i++) {
				Htlevel2Entry e = new Htlevel2Entry(bid_volume.get(i), bid_price.get(i), bid_self.get(i) == 1 ? true : false, Htlevel2Entry.IS_BID);
				level2data.data_m[k++] = e;
			}
			
			for (int i = 0; i < ask_price.size(); i++) {
				Htlevel2Entry e = new Htlevel2Entry(ask_volume.get(i), ask_price.get(i), ask_self.get(i) == 1 ? true : false, Htlevel2Entry.IS_ASK);
				level2data.data_m[k++] = e;
			}
		}



	}
	 

	/**
	 * Helpers
	 */
	private void copyArray(Htlevel2Entry[] data) {

		data_m = new Htlevel2Entry[data.length];
		for (int i = 0; i < data.length; i++) {
			data_m[i] = new Htlevel2Entry(data[i]);
		}

	}
}
