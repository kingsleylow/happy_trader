/*
 * StringList_Proxy.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package com.fin.httrader.hlpstruct.remotecall;

import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import java.util.ArrayList;
import java.util.List;
import java.util.Vector;

/**
 *
 * @author Victor_Zoubok
 */
public class StringList_Proxy  extends ParamBase< List<String> > {
	@Override
	public void convertToXmlParameter(XmlParameter xmlparameter) throws Exception {
	    xmlparameter.clear();
	    xmlparameter.setString( "type", "StringList_Proxy" );
		List<String> dataArray = get();
		for(int i = 0; i < dataArray.size(); i++) {
		  dataArray.set(i, HtUtils.hexlifyString(dataArray.get(i),CallingContext.CONTEXT_DEFAULT_ENCODING));
		}
		
	    xmlparameter.setStringList( "value", dataArray );
	}

	@Override
	public void convertFromXmlparameter(XmlParameter xmlparameter) throws Exception {
	    List<String> dataArray = new ArrayList<String>();
	    dataArray.addAll(xmlparameter.getStringList("value"));
		for(int i = 0; i < dataArray.size(); i++) {
		  dataArray.set(i, HtUtils.unhexlifyString( dataArray.get(i), CallingContext.CONTEXT_DEFAULT_ENCODING));
		}
	  
	    
	    set( dataArray );
	}
    
   
}
