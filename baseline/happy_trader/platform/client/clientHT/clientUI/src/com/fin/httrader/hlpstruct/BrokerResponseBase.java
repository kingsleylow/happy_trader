/*
 * BrokerResponseBase.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.hlpstruct;


import com.fin.httrader.hlpstruct.BrokerErrorInfo;
import com.fin.httrader.configprops.HtSaveAnnotation;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.Uid;
import com.fin.httrader.utils.hlpstruct.XmlParameter;

/**
 *
 * @author Victor_Zoubok
 */
public class BrokerResponseBase {

	// broker string response
	@HtSaveAnnotation
	public StringBuilder brokerComment_m = new StringBuilder();

	// native ID
	@HtSaveAnnotation
	public Uid ID_m = new Uid();

	// this is parent id - e.i. if a response was a result of order execution this is
	@HtSaveAnnotation
	public Uid parentID_m = new Uid();

	// result of broker operation
	@HtSaveAnnotation
	public int resultCode_m = OperationResult.PT_DUMMY;

	// this is detailed operation result
	@HtSaveAnnotation
	public int resultDetailCode_m = OperationDetailResult.ODR_DUMMY;

	// response type
	@HtSaveAnnotation
	public int type_m = ResponseTypes.RT_BrokerResponseDummy;

	// internal broker code
	@HtSaveAnnotation
	public int brokerInternalCode_m = -1;
	
	// broker issue time
	@HtSaveAnnotation
	public double brokerIssueTime_m = -1;
	
	// context in Java layer is only a string
	@HtSaveAnnotation
	public StringBuilder context_m = new StringBuilder();

	// this returns the correct object basing on the type_m field
	public static BrokerResponseBase instantiate(XmlParameter xmlparameter) throws Exception {
		int type = (int) xmlparameter.getInt("type_m");
		if (type == ResponseTypes.RT_BrokerErrorInfo) {
			return new BrokerErrorInfo();
		} else if (type == ResponseTypes.RT_BrokerResponseEquity) {
			return new BrokerResponseEquity();
		} else if (type == ResponseTypes.RT_BrokerResponseHistory) {
			return new BrokerResponseHistory();
		} else if (type == ResponseTypes.RT_BrokerResponseInfo) {
			return new BrokerResponseInfo();
		} else if (type == ResponseTypes.RT_BrokerResponseOrder) {
			return new BrokerResponseOrder();
		} else if (type == ResponseTypes.RT_BrokerResponseTick) {
			return new BrokerResponseTick();
		} else if (type == ResponseTypes.RT_BrokerResponseMetaInfo) {
			return new BrokerResponseMetaInfo();
		} else if (type == ResponseTypes.RT_BrokerResponseDelayedOrder) {
			return new BrokerResponseDelayedOrder();
		} else if (type == ResponseTypes.RT_BrokerResponseClosePos) {
			return new BrokerResponseClosePos();
		} else if (type == ResponseTypes.RT_BrokerResponsePositionOperation) {
			return new BrokerResponsePositionOperation();
		}
		else {
			throw new HtException("BrokerResponseBase", "instantiate", "Invalid parameter type: " + type);
		}
	}
}



