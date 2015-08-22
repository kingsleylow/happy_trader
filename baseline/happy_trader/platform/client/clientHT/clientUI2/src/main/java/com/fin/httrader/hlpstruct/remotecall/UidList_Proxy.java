/*
 * UidList_Proxy.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.hlpstruct.remotecall;

import com.fin.httrader.utils.Uid;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import java.util.ArrayList;
import java.util.List;
import java.util.Vector;

/**
 *
 * @author Victor_Zoubok
 */
public class UidList_Proxy extends ParamBase<List<Uid>> {

	@Override
	public void convertToXmlParameter(XmlParameter xmlparameter) throws Exception {
		xmlparameter.clear();
		xmlparameter.setString("type", "UidList_Proxy");
		xmlparameter.setUidList("value", get());

	}

	@Override
	public void convertFromXmlparameter(XmlParameter xmlparameter) throws Exception {
		List<Uid> uidlist = new ArrayList<Uid>();
		uidlist.addAll(xmlparameter.getUidList("value"));


		set(uidlist);
	}
}
