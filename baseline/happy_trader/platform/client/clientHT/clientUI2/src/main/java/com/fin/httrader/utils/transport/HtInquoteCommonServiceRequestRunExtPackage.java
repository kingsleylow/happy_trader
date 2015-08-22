/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.utils.transport;

import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.Uid;
import com.fin.httrader.utils.hlpstruct.XmlHandler;
import com.fin.httrader.utils.hlpstruct.XmlParameter;

/**
 *
 * @author Victor_Zoubok
 * this is to run external jar package
 */
public class HtInquoteCommonServiceRequestRunExtPackage {
	public static String getContext()
	{
		return HtInquoteCommonServiceRequestRunExtPackage.class.getSimpleName();
	}

	public static class RequestParameters {
		public String packageName_m = null;
		public boolean returnImmedaitely_m = true;
	}

	public static RequestParameters parsePacket(HtInquotePacket packet) throws Exception
	{
		if (packet.getFlag() != HtInquotePacket.FLAG_COMMON_SERVICE_REQUEST_EXT_PACKAGE_RUN)
			throw new HtException(getContext(), "parsePacket", "Invalid packet type: " + packet.getFlag());

		RequestParameters rp = new RequestParameters();
		String datas = new String(packet.getData(), "UTF-8");

		XmlParameter param = XmlHandler.deserializeParameterStatic(datas);
		rp.packageName_m = param.getString("package_name");
		rp.returnImmedaitely_m = (param.getInt("return_immedaitely") == 1 ? true: false);
		
		return rp;
	}

}
