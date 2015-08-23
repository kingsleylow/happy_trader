/*
 * Position.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.hlpstruct;

import com.fin.httrader.hlpstruct.OperationDetailResult;
import com.fin.httrader.hlpstruct.CloseReason;
import com.fin.httrader.configprops.HtSaveAnnotation;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.Uid;
import java.util.Collections;
import java.util.Comparator;
import java.util.TreeMap;
import java.util.Vector;

/**
 *
 * @author Victor_Zoubok
 * This is helper class holding position
 */
public class Position {

	public void Position()
	{
	}
	
	// internal ID
	@HtSaveAnnotation
	public Uid ID_m  = new Uid();
	
	// position broker ID
	@HtSaveAnnotation
	public StringBuilder brokerPositionID_m = new StringBuilder();
	
	// the time we successfully installed the order
	@HtSaveAnnotation
	public double timeOpen_m = -1;
	
	// the time we successfully executed the order
	@HtSaveAnnotation
	public double timeClose_m = -1;
	
	// the time when we closed the order
	@HtSaveAnnotation
	public double avrOpenPrice_m = -1;
	
	// expiration time
	@HtSaveAnnotation
	public double avrClosePrice_m = -1;
	
	// the price of instrument (symbolNum_m / symbolDenom_m) when we installed the order
	@HtSaveAnnotation
	public double volumeImbalance_m = -1;
	
	@HtSaveAnnotation
	public double volume_m = -1;
	
	// the price of instrument (symbolNum_m / symbolDenom_m) when order was executed
	@HtSaveAnnotation
	public int direction_m = TradeDirection.TD_NONE;
	
	// the price of instrument (symbolNum_m / symbolDenom_m) when order was closed
	
	// RT provider
	@HtSaveAnnotation
	public StringBuilder provider_m = new StringBuilder();
	
	// symbol that is unique for the defined broker - in numenator
	@HtSaveAnnotation
	public StringBuilder symbol_m = new StringBuilder();
	
	
	// position state
	// whether it is alive, history or closed
	@HtSaveAnnotation
	public int posState_m = PosState.STATE_DUMMY;
	
	

}
