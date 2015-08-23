/*
 * Int_Proxy.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package com.fin.httrader.hlpstruct.remotecall;

import com.fin.httrader.utils.hlpstruct.XmlParameter;

/**
 *
 * @author Victor_Zoubok
 */
public class Int_Proxy extends ParamBase<Long> {
    
	@Override
	public void convertToXmlParameter(XmlParameter xmlparameter) throws Exception {
	    xmlparameter.clear();
	    xmlparameter.setString( "type", "Int_Proxy" );
	    xmlparameter.setInt("value", get() );
	}

	@Override
	public void convertFromXmlparameter(XmlParameter xmlparameter) throws Exception {
	    set( Long.valueOf( xmlparameter.getInt("value" ) ) );
	}
    
}
