/*
 * String_Proxy.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package com.fin.httrader.hlpstruct.remotecall;

import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.hlpstruct.XmlParameter;

/**
 *
 * @author Victor_Zoubok
 */
public class String_Proxy extends ParamBase<String> {
	@Override
	public void convertToXmlParameter(XmlParameter xmlparameter) throws Exception {
	    xmlparameter.clear();
	    xmlparameter.setString( "type", "String_Proxy" );
		
		
	    xmlparameter.setString("value", HtUtils.hexlifyString(get(), CallingContext.CONTEXT_DEFAULT_ENCODING));
	}

	@Override
	public void convertFromXmlparameter(XmlParameter xmlparameter) throws Exception {
	    set(  HtUtils.unhexlifyString( xmlparameter.getString("value" ), CallingContext.CONTEXT_DEFAULT_ENCODING));
	}
    
       
}
