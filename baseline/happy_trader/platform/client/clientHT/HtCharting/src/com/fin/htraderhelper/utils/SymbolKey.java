/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.htraderhelper.utils;

import java.io.Serializable;

/**
 *
 * @author DanilinS
 * This class is intneded to be a key
 */
public class SymbolKey implements Serializable, Comparable<SymbolKey> {

	public StringBuilder provider_m = new StringBuilder();
	public StringBuilder symbol_m = new StringBuilder();
	public StringBuilder runName_m = new StringBuilder();
	public StringBuilder name_m = new StringBuilder();

	// --------------------

	public SymbolKey(String runName, String name, String provider, String symbol)
	{
		runName_m.append(runName);
		symbol_m.append(symbol);
		provider_m.append(provider);
		name_m.append(name);
	}

	public String toString() {
		StringBuilder thisS = new StringBuilder(runName_m);
		thisS.append(" ");
		thisS.append(name_m);
		thisS.append(" ");
		thisS.append(provider_m);
		thisS.append(" ");
		thisS.append(symbol_m);

		return thisS.toString();
	}

	public int compareTo(SymbolKey o) {
		return this.toString().compareTo(o.toString());
	}

	public boolean equals(Object obj) {
		SymbolKey o = (SymbolKey) obj;
		return this.toString().equals(o.toString());
	}

	public int hashCode() {
		return this.toString().hashCode();
	}

	public String getName()
	{
		return name_m.toString();
	}

	public String getRunName()
	{
		return runName_m.toString();
	}

	public String getSymbol()
	{
		return symbol_m.toString();
	}

	public String getProvider()
	{
		return provider_m.toString();
	}

}
