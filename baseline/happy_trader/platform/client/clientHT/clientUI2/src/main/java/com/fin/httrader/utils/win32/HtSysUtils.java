/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.utils.win32;



/**
 *
 * @author Victor_Zoubok
 */
public class HtSysUtils {
	
	public static native boolean dllIsBrokerLib(String dllFullPath);
	
	public static native boolean dllIsAlgorithmLib(String dllFullPath);
	
	public static native String returnSystemAnalisysString();  
}
