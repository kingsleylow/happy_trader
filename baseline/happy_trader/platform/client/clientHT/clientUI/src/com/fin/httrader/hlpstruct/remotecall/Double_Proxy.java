/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.hlpstruct.remotecall;

import com.fin.httrader.utils.hlpstruct.XmlParameter;

/**
 *
 * @author DanilinS
 */
public class Double_Proxy extends ParamBase<Double> {
	@Override
	public void convertToXmlParameter(XmlParameter xmlparameter) throws Exception {
	    xmlparameter.clear();
	    xmlparameter.setString( "type", "Double_Proxy" );
	    xmlparameter.setDouble("value", get() );
	}

	@Override
	public void convertFromXmlparameter(XmlParameter xmlparameter) throws Exception {
	    set( Double.valueOf( xmlparameter.getDouble("value" ) ) );
	}
}
