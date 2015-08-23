/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.hlpstruct;

/**
 *
 * @author DanilinS
 * this is the entry representing info from DB
 */
public class ProviderSymbolInfoEntry {
	public ProviderSymbolInfoEntry(String provider, String symbol, long minDate, long maxDate)
	{
		provider_m.append( provider );
		symbol_m.append(symbol );
		minDate_m = minDate;
		maxDate_m = maxDate;
	}

	public StringBuilder provider_m = new StringBuilder();
	public StringBuilder symbol_m = new StringBuilder();
	public long minDate_m = -1;
	public long maxDate_m = -1;
	
}
