/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.utils.win32;

import com.fin.httrader.utils.hlpstruct.*;


/**
 *
 * @author DanilinS
 * This class works with HtRtData, Htlevel1Data and HtLevel2Data serializations to binary format
 */
public class QuoteSerializer {

	// rt data
	public static native void serializeRtDataToBinary(HtRtData rtdata, byte[] buffer);
	public static native void deserializeRtDataFromBinary(HtRtData rtdata, byte[] buffer);
	public static native int getHtRtDataSize();

	//level1 data
	public static native void serializeLevel1ToBinary(HtLevel1Data level1data, byte[] buffer);
	public static native void deserializeLevel1FromBinary(HtLevel1Data level1data, byte[] buffer);
	public static native int getLevel1DataSize();
	
	public static native byte[] serializeLevel2ToBinary(HtLevel2Data level2data);
	public static native void deserializeLevel2FromBinary(HtLevel2Data level2data, byte[] buffer);
	//public static native int getLevel2DataSize(HtLevel2Data level2data);


}
