/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.hlpstruct.remotecall;

import com.fin.httrader.utils.hlpstruct.HtRtData;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import com.fin.httrader.utils.xmlparamserial.HtXmlParameterSerialization;
import java.util.ArrayList;
import java.util.List;
import java.util.Vector;

/**
 *
 * @author DanilinS
 */
public class RtDataList_Proxy extends ParamBase<List<HtRtData>> {

	@Override
	public void convertToXmlParameter(XmlParameter xmlparameter) throws Exception {
		xmlparameter.clear();
		xmlparameter.setString("type", "RtDataList_Proxy");

		List<HtRtData> positionList = get();
		List<XmlParameter> xmlParamList = new ArrayList<XmlParameter>();
		for (int i = 0; i < positionList.size(); i++) {
			HtRtData pos_i = positionList.get(i);

			XmlParameter xmlParam_i = new XmlParameter();

			HtRtData.rtDataToParameter(pos_i, xmlParam_i);
	
			xmlParamList.add(xmlParam_i);

		}

		xmlparameter.setXmlParameterList("value", xmlParamList);
	}

	@Override
	public void convertFromXmlparameter(XmlParameter xmlparameter) throws Exception {
		List<HtRtData> posList = new ArrayList<HtRtData>();

		List<XmlParameter> xmlParamList = xmlparameter.getXmlParameterList("value");

		if (xmlParamList != null) {
			for (int i = 0; i < xmlParamList.size(); i++) {
				XmlParameter param_i = xmlParamList.get(i);
				HtRtData pos_i = new HtRtData();

				HtRtData.rtDataFromParameter(param_i, pos_i);

			
				posList.add(pos_i);
			}
		}


		set(posList);
	}
}
