/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.webserver;

import com.fin.httrader.hlpstruct.remotecall.CallingContext;
import com.fin.httrader.utils.HtBase64;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.HtStringByteConverter;
import com.fin.httrader.utils.HtUtils;
import com.google.gson.Gson;
import java.io.IOException;
import java.io.OutputStream;
import java.util.Calendar;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.logging.Level;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author DanilinS
 * this class wraps ajax calling from the side of servlet
 */
public class HtAjaxCaller {


	private StringBuilder statusString_m = new StringBuilder();
	private int statusCode_m = -1;
	private StringBuilder dataString_m = new StringBuilder();


	public HtAjaxCaller()
	{
	}

	public void setError(int status, String errorMessage)
	{
		statusCode_m = status;
		statusString_m.setLength(0);
		statusString_m.append(errorMessage);

	}

	public void setOk()
	{
		statusCode_m = 0;
		statusString_m.setLength(0);
		statusString_m.append("Ok");
	}

	public void setData(String data)
	{
		dataString_m.setLength(0);
		dataString_m.append( data );
	}

	/*
	public String serializeToXml()
	{
		StringBuilder out = new StringBuilder();
		out.append("<?xml version=\"1.0\" encoding=\"windows-1251\"?>");
		out.append("<ajax_call>");

		out.append("<status_code>");
		out.append(statusCode_m);
		out.append("</status_code>");

		out.append("<status_string>");
		out.append(statusString_m);
		out.append("</status_string>");

		out.append("<data_string>");
		out.append(HtUtils.wrapToCDataTags(dataString_m.toString()));
		out.append("</data_string>");

		out.append("</ajax_call>");


		return out.toString();
	}
	*/

	public void serializeToXml(HttpServletResponse res) throws IOException
	{

		res.setCharacterEncoding("UTF-8");
		res.getOutputStream().print("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
		res.getOutputStream().print("<ajax_call>");

		res.getOutputStream().print("<status_code>");
		res.getOutputStream().print(statusCode_m);
		res.getOutputStream().print("</status_code>");

		res.getOutputStream().print("<status_string>");
		
		// we are going to hexlify all these bytes
		res.getOutputStream().print(HtBase64.encode(HtStringByteConverter.encodeUTF8(statusString_m.toString())));
		res.getOutputStream().print("</status_string>");

		res.getOutputStream().print("<data_string>");
		res.getOutputStream().print(HtBase64.encode(HtStringByteConverter.encodeUTF8(dataString_m.toString())));
		res.getOutputStream().print("</data_string>");

		res.getOutputStream().print("</ajax_call>");
		
	}

	
}
