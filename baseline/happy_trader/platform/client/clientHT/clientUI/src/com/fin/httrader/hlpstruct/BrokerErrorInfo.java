/*
 * BrokerErrorInfo.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.hlpstruct;

/**
 *
 * @author Victor_Zoubok
 */
public class BrokerErrorInfo extends BrokerResponseBase {

	BrokerErrorInfo() {
		type_m = ResponseTypes.RT_BrokerErrorInfo;
	}
	// string error info
	public StringBuilder error_m = new StringBuilder();
	// releval error time
	public double errorDate_m = -1;
	// error code
	public int errorCode_m = -1;
	// if relevant to some kind of order
	public StringBuilder brokerPositionID_m = new StringBuilder();
}
