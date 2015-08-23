/*
 * Uid_Proxy.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.hlpstruct.remotecall;

import com.fin.httrader.utils.Uid;
import com.fin.httrader.utils.hlpstruct.XmlParameter;

/**
 *
 * @author Victor_Zoubok
 */
public class Uid_Proxy extends ParamBase<Uid> {

	@Override
	public void convertToXmlParameter(XmlParameter xmlparameter) throws Exception {
		xmlparameter.clear();
		xmlparameter.setString("type", "Uid_Proxy");
		xmlparameter.setUid("value", get());
	}

	@Override
	public void convertFromXmlparameter(XmlParameter xmlparameter) throws Exception {
		set(new Uid(xmlparameter.getUid("value")));
	}
}
