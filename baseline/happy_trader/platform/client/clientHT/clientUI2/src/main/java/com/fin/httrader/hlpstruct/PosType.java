/*
 * PosType.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.hlpstruct;

import com.fin.httrader.utils.HtEnumeratorDescriptor;
import com.fin.httrader.utils.HtException;
import java.util.Arrays;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
import java.util.TreeSet;

/**
 *
 * @author Victor_Zoubok
 */
public class PosType {

	// open
	public static final int POS_DUMMY = 0;
	public static final int POS_BUY = 1; // alive
	public static final int POS_SELL = 2; // alive
	public static final int POS_LIMIT_BUY = 3; // pending
	public static final int POS_LIMIT_SELL = 4; // pending
	public static final int POS_STOP_BUY = 5; // pending
	public static final int POS_STOP_SELL = 6; // pending
	public static final int POS_STOP_LIMIT_BUY = 7;
	public static final int POS_STOP_LIMIT_SELL = 8;

	// close
	public static final int 	POS_CLOSE_LONG = 9;
	public static final int 	POS_CLOSE_SHORT = 10;
	public static final int 	POS_CLOSE_LIMIT_LONG = 11;
	public static final int 	POS_CLOSE_LIMIT_SHORT = 12;
	public static final int 	POS_CLOSE_STOP_LONG = 13;
	public static final int 	POS_CLOSE_STOP_SHORT = 14;
	public static final int 	POS_CLOSE_STOP_LIMIT_LONG = 15;
	public static final int 	POS_CLOSE_STOP_LIMIT_SHORT = 16;
	public static final int 	POS_CANCELLED = 17;


	private static HtEnumeratorDescriptor descriptor_m = new HtEnumeratorDescriptor("com.fin.httrader.hlpstruct.PosType");

	// ----------------------
	

	
	// -------------------------------------------

	public static String getPosTypeName(int pt) {

		return descriptor_m.getNameById_StrictValue(pt);
	
	
	}

	public static int getPosTypeId(String posTypeName) {
		return descriptor_m.getIdByName_StrictValue(posTypeName, POS_DUMMY);
		
	}

	public static Set<Integer> getAllPosTypes() {
		return descriptor_m.getIdSet();
	}
	
	public static Map<Integer, String> getAllPosTypesNamesMap() {
		return descriptor_m.getNameMap();
	}



	
}



