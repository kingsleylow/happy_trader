/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.externalapi.utils;



/**
 *
 * @author DanilinS
 */

public class HtExtPropertyHolder<T> {

	public T getProperty()
	{
		return p_m;
	}

	public void setProperty(T p)
	{
		p_m = p;
	}


	private T p_m = null;
}
