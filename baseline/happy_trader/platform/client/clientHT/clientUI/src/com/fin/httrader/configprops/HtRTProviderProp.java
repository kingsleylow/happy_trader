/*
 * HtRTProviderProp.java
 *
 * 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.configprops;


import java.util.LinkedHashMap;
import java.util.Map;

/**
 *
 * @author Victor_Zoubok
 */
public class HtRTProviderProp extends HtBaseProp {

	/**
	 * Creates a new instance of HtRTProviderProp
	 */
	public HtRTProviderProp() {
		type_m = HtBaseProp.ID_CONFIGURE_RT_PROV;
	}
	

	public Map<String, String> getParameters()
	{
		return initParams_m;
	}
	
	public String getProviderClass()
	{
		return providerClass_m.toString();
	}
	
	public void setProviderClass(String provClass)
	{
		providerClass_m.setLength(0);
		providerClass_m.append(provClass);
	} 
	
	public int getHourShift()
	{
		return hourShift_m;
	}
	
	public int getSignDigits()
	{
		return signDig_m;
	}
	
	public boolean isAlien()
	{
		return isAlien_m;
	}
	
	public void setHourShift(int hourShift)
	{
		hourShift_m = hourShift;
	}
	
	public void setSignDigits(int signDigits)
	{
		signDig_m = signDigits;
	}
	
	public void setAlien(boolean isAlien)
	{
		isAlien_m = isAlien;
	}
	
	@HtSaveAnnotation
	private LinkedHashMap<String, String> initParams_m = new LinkedHashMap<String, String>();
	
	@HtSaveAnnotation
	private StringBuilder providerClass_m = new StringBuilder();
	
	@HtSaveAnnotation
	private int signDig_m = -1;
	
	@HtSaveAnnotation
	private int hourShift_m = 0;
	
	@HtSaveAnnotation
	private boolean isAlien_m = false; // no Java - just name is necessary

	
}
