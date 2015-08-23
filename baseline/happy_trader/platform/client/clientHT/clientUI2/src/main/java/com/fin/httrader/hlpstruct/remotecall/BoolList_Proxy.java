/*
 * BoolList_Proxy.java
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
public class BoolList_Proxy extends ParamBase<List<Boolean>> {

	@Override
	public void convertToXmlParameter(XmlParameter xmlparameter) throws Exception {
		xmlparameter.clear();
		xmlparameter.setString("type", "BoolList_Proxy");

		List<Boolean> tmp = get();
		List<Long> data = new ArrayList<Long>();
		for (int i = 0; i < tmp.size(); i++) {
			data.add(Long.valueOf(tmp.get(i) ? 1 : 0));
		}

		xmlparameter.setIntList("value", data);

	}

	@Override
	public void convertFromXmlparameter(XmlParameter xmlparameter) throws Exception {
		List<Boolean> blist = new ArrayList<Boolean>();

		List<Long> data = xmlparameter.getIntList("value");
		for (int i = 0; i < data.size(); i++) {
			blist.add(Boolean.valueOf(data.get(i) == 1 ? true : false));
		}

		set(blist);

	}
};
