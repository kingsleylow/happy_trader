/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.externalapi;

import com.fin.httrader.externalapi.HtExtException;

/**
 *
 * @author Victor_Zoubok
 */
public abstract class HtExtBaseLogger {
	
	// to be derived
	
	
	public abstract void addStringEntry(String data) throws HtExtException;
	
	public abstract void addErrorEntry(String data) throws HtExtException;
	
}
