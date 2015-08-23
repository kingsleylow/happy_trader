/*
 * IntList_Proxy.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package com.fin.httrader.hlpstruct.remotecall;

import com.fin.httrader.utils.hlpstruct.XmlParameter;
import java.util.ArrayList;
import java.util.List;
import java.util.Vector;

/**
 *
 * @author Victor_Zoubok
 */
public class IntList_Proxy extends ParamBase< List<Long> > {
    
	@Override
     public void convertToXmlParameter(XmlParameter xmlparameter) throws Exception {
	    xmlparameter.clear();
	    xmlparameter.setString( "type", "IntList_Proxy" );
	    xmlparameter.setIntList( "value", get() );
	
	}

	@Override
	public void convertFromXmlparameter(XmlParameter xmlparameter) throws Exception {
	     List<Long> values = new ArrayList<Long>();
	     values.addAll(xmlparameter.getIntList("value"));
	     
	     set(values);
	}
    
}
