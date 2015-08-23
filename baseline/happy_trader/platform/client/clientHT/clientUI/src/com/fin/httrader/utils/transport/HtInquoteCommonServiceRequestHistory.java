/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.utils.transport;

import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.hlpstruct.HtPair;
import com.fin.httrader.utils.hlpstruct.HtRtData;
import com.fin.httrader.utils.hlpstruct.XmlHandler;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import java.util.ArrayList;
import java.util.List;

/**
 *
 * @author DanilinS
 * this class wraps FLAG_COMMON_SERVICE_REQUEST_HISTORY functionality
 */
public class HtInquoteCommonServiceRequestHistory {
	
	public static String getContext()
	{
		return HtInquoteCommonServiceRequestHistory.class.getSimpleName();
	}

	public static class RequestParameters {
		public long begin_date_m  =-1;
		public long end_date_m = -1;
		public List<HtPair<String, String>> provSymMap = new ArrayList<HtPair<String, String>>();
		public String profile_name_m = null;
	}


	public static RequestParameters parsePacket(HtInquotePacket packet) throws Exception
	{
		if (packet.getFlag() != HtInquotePacket.FLAG_COMMON_SERVICE_REQUEST_HISTORY)
			throw new HtException(getContext(), "parsePacket", "Invalid packet type: " + packet.getFlag());

		RequestParameters rp = new RequestParameters();
		String datas = new String(packet.getData(), "UTF-8");

		XmlParameter param = XmlHandler.deserializeParameterStatic(datas);
		rp.begin_date_m = param.getDate("begin_date");
		rp.end_date_m = param.getDate("end_date");
		rp.profile_name_m = param.getString("profile_name");

		List<XmlParameter> psml = param.getXmlParameterList("provider_symbol_list");
		for(int i = 0; i < psml.size(); i++) {
			XmlParameter entry_i = psml.get(i);
			HtPair<String, String> pair_i = new HtPair<String, String>(entry_i.getString("provider"), entry_i.getString("symbol"));
			rp.provSymMap.add( pair_i );
		}

		return rp;
	}


}
