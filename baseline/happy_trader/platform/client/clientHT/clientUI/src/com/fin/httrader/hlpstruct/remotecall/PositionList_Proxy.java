/*
 * PositionList_Proxy.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.hlpstruct.remotecall;

import com.fin.httrader.hlpstruct.Position;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import com.fin.httrader.utils.xmlparamserial.HtXmlParameterSerialization;
import java.util.ArrayList;
import java.util.List;
import java.util.Vector;

/**
 *
 * @author Victor_Zoubok
 */
public class PositionList_Proxy extends ParamBase<List<Position>> {

	@Override
	public void convertToXmlParameter(XmlParameter xmlparameter) throws Exception {
		xmlparameter.clear();
		xmlparameter.setString("type", "PositionList_Proxy");

		List<Position> positionList = get();
		List<XmlParameter> xmlParamList = new ArrayList<XmlParameter>();
		for (int i = 0; i < positionList.size(); i++) {
			Position pos_i = positionList.get(i);

			XmlParameter xmlParam_i = new XmlParameter();

			HtXmlParameterSerialization.serializeObjectToXmlParameter(pos_i, xmlParam_i);


			xmlParamList.add(xmlParam_i);

		}

		xmlparameter.setXmlParameterList("value", xmlParamList);
	}

	@Override
	public void convertFromXmlparameter(XmlParameter xmlparameter) throws Exception {
		List<Position> posList = new ArrayList<Position>();

		List<XmlParameter> xmlParamList = xmlparameter.getXmlParameterList("value");

		if (xmlParamList != null) {
			for (int i = 0; i < xmlParamList.size(); i++) {
				XmlParameter param_i = xmlParamList.get(i);
				Position pos_i = new Position();

				HtXmlParameterSerialization.deserializeObjectFromXmlParameter(pos_i, param_i);

				posList.add(pos_i);
			}
		}


		set(posList);
	}
}
