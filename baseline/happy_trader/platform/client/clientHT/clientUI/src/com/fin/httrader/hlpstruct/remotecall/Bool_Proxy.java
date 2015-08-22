/*
 * Bool_Proxy.java
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
public class Bool_Proxy extends ParamBase<Boolean>{
    
	@Override
    public void convertToXmlParameter(XmlParameter xmlparameter) throws Exception {
	 xmlparameter.clear();
	 xmlparameter.setString( "type", "Bool_Proxy" );
	 xmlparameter.setInt("value", get()? 1:0 );
    }
    
	@Override
    public void convertFromXmlparameter(XmlParameter xmlparameter) throws Exception {
	 set( Boolean.valueOf( xmlparameter.getInt("value" ) == 1 ? true:false ) );
    }
    
}
