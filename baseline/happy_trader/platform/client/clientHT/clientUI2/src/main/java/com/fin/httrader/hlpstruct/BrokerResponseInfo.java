/*
 * BrokerResponseInfo.java
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
public class BrokerResponseInfo extends BrokerResponseBase {

	public BrokerResponseInfo() {
		type_m = ResponseTypes.RT_BrokerResponseInfo;
	}
	// string information
	public StringBuilder news_m = new StringBuilder();
	// the date relevant to information
	public double newsDate_m = -1;
}
