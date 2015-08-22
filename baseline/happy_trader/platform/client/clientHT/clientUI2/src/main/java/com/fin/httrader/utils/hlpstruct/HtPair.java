/*
 * HtPair.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.utils.hlpstruct;

/**
 */
public class HtPair<T1, T2> implements Comparable<HtPair> {

	public HtPair(T1 first, T2 second) {
		this.first = first;
		this.second = second;
	}

	public HtPair() {
		first = null;
		second = null;
	}

	@Override
	public int compareTo(HtPair o) {
		return this.toString().compareTo(o.toString());
	}

	@Override
	public String toString()
	{
		StringBuilder b = new StringBuilder(String.valueOf(first));
		b.append(String.valueOf(second));
		return b.toString();
	}
	
	public T1 first = null;
	public T2 second = null;

	
}
