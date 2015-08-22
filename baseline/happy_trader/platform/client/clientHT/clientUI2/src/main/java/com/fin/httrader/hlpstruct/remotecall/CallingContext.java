/*
 * CallingContext.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.hlpstruct.remotecall;

import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import java.util.HashMap;
import java.util.Iterator;

/**
 *
 * @author Victor_Zoubok
 */
public class CallingContext {

	public static final String CONTEXT_DEFAULT_ENCODING = "windows-1251";
	private StringBuilder callName_m = new StringBuilder();
	
	// additional string 
	private StringBuilder callCustomName_m = new StringBuilder();
	private StringBuilder resultReason_m = new StringBuilder();
	private int resultCode_m = 0;
	private HashMap<String, ParamBase> parameterPtr_m = new HashMap();

	/** Creates a new instance of CallingContext */
	public CallingContext() {
	}

	public CallingContext(String callName) {
		setCallName(callName);
	}

	public CallingContext(String callName, String callCustomName) {
		setCallName(callName);
		setCallCustomName(callCustomName);
	}

	public String getCallName()
	{
		return callName_m.toString();
	}

	public String getCallCustomName()
	{
		return callCustomName_m.toString();
	}

	public void setCallName(String callName) {
		callName_m.setLength(0);
		callName_m.append(callName);
	}

	public void setCallCustomName(String callCustomName) {
		callCustomName_m.setLength(0);
		callCustomName_m.append(callCustomName);
	}


	public int getResultCode() {
		return resultCode_m;
	}

	public String getResultReason() {
		return resultReason_m.toString();
	}

	public <T extends ParamBase> T getParameter(String pname) throws Exception {
		if (parameterPtr_m.containsKey(pname)) {
			return (T) parameterPtr_m.get(pname);
		}

		throw new HtException(getClass().getSimpleName(), "getParameter", "Invalid parameter: \"" + pname + "\"");
	}

	public <T extends ParamBase> T setParameter(String pname, ParamBase<?> param) {
		parameterPtr_m.put(pname, param);
		return (T) param;
	}

	// convert to & from XML parameter
	public static void convertCallingContextFromXmlParameter(CallingContext context, XmlParameter xmlparameter) throws Exception {
		// function name
		if (xmlparameter == null)
				throw new HtException("CallingContext", "convertCallingContextFromXmlParameter", "NULL XML content");
			
		if (!xmlparameter.getCommandName().equals("function_call_wrap")) {
			throw new HtException("CallingContext", "convertCallingContextFromXmlParameter", "Invalid XML content");
		}

		context.callName_m.setLength(0);
		context.callName_m.append(xmlparameter.getString("function"));

		context.callCustomName_m.setLength(0);
		context.callCustomName_m.append(xmlparameter.getString("function_custom"));

		// result
		context.resultCode_m = (int) xmlparameter.getInt("status_code");
		context.resultReason_m.setLength(0);
		context.resultReason_m.append( HtUtils.unhexlifyString( xmlparameter.getString("status_reason"), CONTEXT_DEFAULT_ENCODING ));

		// return if invalid
		if (context.resultCode_m != 0) {
			return;
		}

		// parameters
		XmlParameter xmlParams = xmlparameter.getXmlParameter("parameters");
		for (Iterator<String> it = xmlParams.getKeys().iterator(); it.hasNext();) {
			String pname = it.next();
			XmlParameter param_i = xmlParams.getXmlParameter(pname);

			ParamBase proxyInst = ParamBase.instantiate(param_i.getString("type"));
			proxyInst.convertFromXmlparameter(param_i);

			proxyInst.setParamName(pname);
			context.setParameter(pname, proxyInst);

		}

	}

	public static void convertCallingContextToXmlParameter(CallingContext context, XmlParameter xmlparameter) throws Exception {
		xmlparameter.clear();
		xmlparameter.setCommandName("function_call_wrap");
		xmlparameter.setString("function", context.getCallName());
		xmlparameter.setString("function_custom", context.getCallCustomName());

		XmlParameter xmlParams = new XmlParameter();
		for (Iterator<String> it = context.parameterPtr_m.keySet().iterator(); it.hasNext();) {
			String pname = it.next();
			ParamBase param = context.parameterPtr_m.get(pname);

			XmlParameter param_i = new XmlParameter();
			param.convertToXmlParameter(param_i);

			xmlParams.setXmlParameter(pname, param_i);

		}

		
		
		
		
		xmlparameter.setXmlParameter("parameters", xmlParams);

				
		xmlparameter.setInt("status_code", context.getResultCode());
		xmlparameter.setString("status_reason", HtUtils.hexlifyString( context.getResultReason(), CONTEXT_DEFAULT_ENCODING));

	}
}