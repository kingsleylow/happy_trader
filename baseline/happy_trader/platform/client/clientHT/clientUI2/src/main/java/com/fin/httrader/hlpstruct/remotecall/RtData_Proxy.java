/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.hlpstruct.remotecall;

import com.fin.httrader.utils.hlpstruct.HtRtData;
import com.fin.httrader.utils.hlpstruct.XmlParameter;

/**
 *
 * @author DanilinS
 */
public class RtData_Proxy extends ParamBase<HtRtData> {
	@Override
	public void convertToXmlParameter(XmlParameter xmlparameter) throws Exception {
		xmlparameter.clear();
		XmlParameter param_rtdata = new XmlParameter();
		xmlparameter.setString("type", "RtData_Proxy");
		HtRtData.rtDataToParameter(get(), param_rtdata);

		xmlparameter.setXmlParameter("value", param_rtdata);
	
	}

	@Override
	public void convertFromXmlparameter(XmlParameter xmlparameter) throws Exception {
		HtRtData rtdata = new HtRtData();
		
		HtRtData.rtDataFromParameter(xmlparameter.getXmlParameter("value"), rtdata);

		set(rtdata);
	}
}
