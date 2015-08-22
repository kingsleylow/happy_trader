/*
 * Order_Proxy.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package com.fin.httrader.hlpstruct.remotecall;

import com.fin.httrader.hlpstruct.Order;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import com.fin.httrader.utils.xmlparamserial.HtXmlParameterSerialization;

/**
 *
 * @author Victor_Zoubok
 */
public class Order_Proxy extends ParamBase<Order> {
    
	@Override
    public void convertToXmlParameter(XmlParameter xmlparameter) throws Exception {
	    xmlparameter.clear();
	    xmlparameter.setString( "type", "Order_Proxy" );
	    
	    XmlParameter internal = new XmlParameter();
	    HtXmlParameterSerialization.serializeObjectToXmlParameter(get(), internal);
	    
	    xmlparameter.setXmlParameter("value", internal );
	}

	@Override
	public void convertFromXmlparameter(XmlParameter xmlparameter) throws Exception {
	    Order order = new Order();
	    XmlParameter internal = xmlparameter.getXmlParameter("value");
	    HtXmlParameterSerialization.deserializeObjectFromXmlParameter(order, internal);
	    
	    set(  order );
	}
    
}
