/*
 * HtServletsBase.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.hlpstruct.HtPair;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.Uid;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.Vector;
import java.util.logging.Level;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;


/**
 *
 * @author victor_zoubok
 */
public abstract class HtServletsBase extends HttpServlet {

	private Uid pageUid_m = new Uid();

	protected String getContext() {
		return HtServletsBase.class.getName();
	}

	public String getUniquePageId() throws Exception {
		if (!pageUid_m.isValid()) {
			throw new HtException(getContext(), "getUniquePageId", "Invalid page UID");
		}

		return pageUid_m.toString();
	}

	public void setUniquePageId(String pid) throws Exception {
		pageUid_m.fromString(pid);
		if (!pageUid_m.isValid()) {
			throw new HtException(getContext(), "setUniquePageId", "Invalid page UID");
		}
	}

	public void generateUniquePageId() {
		pageUid_m.generate();
	}
	
	// ------------------------------
	
	protected boolean  initialize_Get(HttpServletRequest req, 
                                   HttpServletResponse res) throws ServletException, IOException
    {
        throw new ServletException("Implemented in child class");
    }
    
    protected boolean  initialize_Post(HttpServletRequest req, 
                                   HttpServletResponse res) throws ServletException, IOException
    {
        throw new ServletException("Implemented in child class");
    }

    
    public boolean dispatchJspCall(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException
    {
        // dispatched call to necessary handler
        // and also return true if we cotinue output
        String methodName = req.getMethod();
        if (HtUtils.nvl(methodName)) {
            throw new ServletException("Invalid method");
        }
        
        if (methodName.equalsIgnoreCase("POST")) {
            return this.initialize_Post(req,res);
        }
        else if (methodName.equalsIgnoreCase("GET")) {
            return this.initialize_Get(req,res);
        }
        else
            throw new ServletException("Invalid method: " + methodName);     
        
    }

	// ------------------------------
	// this do not explecetely establish seesion assuming we have one
	protected HttpSession getCurrentSession(HttpServletRequest req) throws Exception {
		HttpSession sess = req.getSession(true);

		if (sess == null) {
			throw new HtException(getContext(), "getCurrentSession", "Invalid session");
		}

		return sess;
	}

	// ------------------------------
	public void setIntegerSessionValue(String cookie, HttpServletRequest req, String name, int value) throws Exception {
			String sval = String.valueOf(value);
			this.setStringSessionValue(cookie, req, name, sval);
	}
	
	public int getIntegerSessionValue(String cookie, HttpServletRequest req, String name) throws Exception {
			String sval = getStringSessionValue(cookie, req, name);
			return Integer.valueOf(sval);
	}
	
	// ------------------------------
	public void setStringSessionValue(String cookie, HttpServletRequest req, String name, String value) throws Exception {

		if (cookie == null || cookie.length() == 0) {
			throw new HtException(getContext(), "setStringSessionValue", "Empty cookie");
		}

		StringBuilder totalName = new StringBuilder(cookie);
		totalName.append("_");
		totalName.append(name);

		StringBuilder obj = (StringBuilder) getCurrentSession(req).getAttribute(totalName.toString());
		if (obj == null) {
			obj = new StringBuilder();
		}

		obj.setLength(0);
		obj.append(value);

		HttpSession sess = getCurrentSession(req);
		sess.setAttribute(totalName.toString(), obj);

	//HtUtils.log(Level.INFO, getContext(), "setStringSessionValue", "Set up string session value for key: "+ totalName.toString() );
	}

	// ------------------------------
	public String getStringSessionValue(String cookie, HttpServletRequest req, String name) throws Exception {

		if (cookie == null || cookie.length() == 0) {
			throw new HtException(getContext(), "setStringSessionValue", "Empty cookie");
		}

		StringBuilder totalName = new StringBuilder(cookie);
		totalName.append("_");
		totalName.append(name);

		HttpSession sess = getCurrentSession(req);
		StringBuilder obj = (StringBuilder) sess.getAttribute(totalName.toString());
		if (obj == null) {
			return "";
		//throw new HtException(getContext(), "getStringSessionValue", "No string session value for key: " + totalName.toString() );
		}

		return obj.toString();
	}

	// ------------------------------
	public void setObjectSessionValue(String cookie, HttpServletRequest req, String name, Object value) throws Exception {
		if (cookie == null || cookie.length() == 0) {
			throw new HtException(getContext(), "setStringSessionValue", "Empty cookie");
		}

		StringBuilder totalName = new StringBuilder(cookie);
		totalName.append("_");
		totalName.append(name);

		getCurrentSession(req).setAttribute(totalName.toString(), value);
	}

	// ------------------------------
	Object getObjectSessionValue(String cookie, HttpServletRequest req, String name) throws Exception {
		if (cookie == null || cookie.length() == 0) {
			throw new HtException(getContext(), "setStringSessionValue", "Empty cookie");
		}

		StringBuilder totalName = new StringBuilder(cookie);
		totalName.append("_");
		totalName.append(name);

		return getCurrentSession(req).getAttribute(totalName.toString());
	}

	// ------------------------------
	// cleasr only values starting with that cookie
	protected void clearSessionValues(String cookie, HttpServletRequest req) throws Exception {
		if (cookie == null || cookie.length() == 0) {
			throw new HtException(getContext(), "setStringSessionValue", "Empty cookie");
		}

		StringBuilder totalName = new StringBuilder(cookie);
		totalName.append("_");

		HttpSession sess = getCurrentSession(req);

		Vector<String> attribsNames = new Vector<String>();
		Enumeration attribs = sess.getAttributeNames();

		while (attribs.hasMoreElements()) {
			String key_i = (String) attribs.nextElement();
			if (key_i.startsWith(totalName.toString())) {
				attribsNames.add(key_i);
			}
		}



		for (int i = 0; i < attribsNames.size(); i++) {
			sess.removeAttribute(attribsNames.get(i));
		//HtUtils.log(Level.INFO, getContext(), "clearSessionValues", "Cleared session value for key: "+ attribsNames.get(i));

		}
	}

	// ------------------------------
	protected void setRedirectedAfterPost(String cookie, HttpServletRequest req, boolean isTrue) throws Exception {
		if (isTrue) {
			setStringSessionValue(cookie, req, "after_post", "true");
		} else {
			setStringSessionValue(cookie, req, "after_post", "false");
		}
	}

	// ------------------------------
	protected boolean isRedirectedAfterPost(String cookie, HttpServletRequest req) throws Exception {
		String after_post = getStringSessionValue(cookie, req, "after_post");
		if (after_post.equals("true")) {
			return true;
		}

		return false;
	}

	// ------------------------------
	public void wrapParametersToSession(String cookie, HttpServletRequest req) throws Exception {

		Enumeration params = req.getParameterNames();
		while (params.hasMoreElements()) {
			String key = (String) params.nextElement();
			String value = req.getParameter(key);

			setStringSessionValue(cookie, req, "uri_param_" + key, value);
		}
	}

	// ------------------------------
	public void wrapFakedParameterToSession(String cookie, HttpServletRequest req, String key, String value) throws Exception {
		setStringSessionValue(cookie, req, "uri_param_" + key, value);
	}

	// ------------------------------
	public String getStringParameterWrappedToSession(String cookie, HttpServletRequest req, String param_name, boolean noexcept) throws Exception {
		String value = getStringSessionValue(cookie, req, "uri_param_" + (param_name != null ? param_name : ""));

		if (value == null || value.length() <= 0) {
			if (noexcept) {
				return "";
			} else {
				throw new HtException(getContext(), "getStringParameterWrappedToSession", "Invalid URL parameter wrapped to session: \"" + param_name + "\"");
			}
		}

		return value;
	}

	// ------------------------------
	public boolean getBooleanParameterWrappedToSession(String cookie, HttpServletRequest req, String param_name, boolean noexcept) throws Exception {
		String value = getStringSessionValue(cookie, req, "uri_param_" + (param_name != null ? param_name : ""));

		if (value == null || value.length() <= 0) {
			if (noexcept) {
				return false;
			} else {
				throw new HtException(getContext(), "getBooleanParameterWrappedToSession", "Invalid URL parameter wrapped to session: \"" + param_name + "\"");
			}
		}

		return (value.equalsIgnoreCase("true") ? true : false);
	}

	// ------------------------------
	public long getIntParameterWrappedToSession(String cookie, HttpServletRequest req, String param_name, boolean noexcept) throws Exception {

		String value = getStringSessionValue(cookie, req, "uri_param_" + (param_name != null ? param_name : ""));

		if (value == null || value.length() <= 0) {
			if (noexcept) {
				return -1;
			} else {
				throw new HtException(getContext(), "getIntParameterWrappedToSession", "Invalid URL parameter wrapped to session: \"" + param_name + "\"");
			}
		}

		long val = -1;
		try {
			val = Long.valueOf(value);
		} catch (NumberFormatException e) {
			if (noexcept) {
				return -1;
			}
			throw new HtException(getContext(), "getIntParameterWrappedToSession", "Exception: \"" + e.getMessage() + "\"");
		}

		return val;

	}

	// ------------------------------
	public double getDoubleParameterWrappedToSession(String cookie, HttpServletRequest req, String param_name, boolean noexcept) throws Exception {

		String value = getStringSessionValue(cookie, req, "uri_param_" + (param_name != null ? param_name : ""));

		if (value == null || value.length() <= 0) {
			if (noexcept) {
				return -1;
			} else {
				throw new HtException(getContext(), "getDoubleParameterWrappedToSession", "Invalid URL parameter wrapped to session: \"" + param_name + "\"");
			}
		}

		double val = -1;
		try {
			val = Double.valueOf(value);
		} catch (NumberFormatException e) {
			if (noexcept) {
				return -1;
			}
			throw new HtException(getContext(), "getDoubleParameterWrappedToSession", "Exception: \"" + e.getMessage() + "\"");
		}

		return val;

	}

	// ------------------------------
	public Vector<String> getStringListParameterWrappedToSession(String cookie, HttpServletRequest req, String param_name, boolean noexcept) throws Exception {
		Vector<String> result = new Vector<String>();
		String value = getStringSessionValue(cookie, req, "uri_param_" + (param_name != null ? param_name : ""));

		if (value == null || value.length() <= 0) {
			if (noexcept) {
				return result;
			} else {
				throw new HtException(getContext(), "getStringListParameterWrappedToSession", "Invalid URL parameter wrapped to session: \"" + param_name + "\"");
			}
		}

		value += "dummy";
		String[] splitted = value.toString().split(",");

		for (int i = 0; i < splitted.length; i++) {
			result.add(splitted[i]);
		}

		if (result.get(result.size() - 1).equals("dummy")) {
			// remove last element
			result.removeElementAt(result.size() - 1);
		}

		return result;

	}

	// ------------------------------
	public Vector<HtPair<String, String>> getStringPairListParameterWrappedToSession(String cookie, HttpServletRequest req, String param_name, boolean noexcept) throws Exception {
		Vector<HtPair<String, String>> result = new Vector<HtPair<String, String>>();
		String value = getStringSessionValue(cookie, req, "uri_param_" + (param_name != null ? param_name : ""));

		if (value == null || value.length() <= 0) {
			if (noexcept) {
				return result;
			} else {
				throw new HtException(getContext(), "getStringPairListParameterWrappedToSession", "Invalid URL parameter wrapped to session: \"" + param_name + "\"");
			}
		}

		value += "dummy";
		// parse comma separated - name, value, name, value, ...
		String[] splitted = value.toString().split(",");
		for (int i = 0; i < splitted.length / 2; i++) {
			String v1 = splitted[i * 2];
			String v2 = splitted[i * 2 + 1];

			if (v1 != null && v1.length() > 0) {
				result.add(new HtPair<String, String>(v1, v2));
			}
		}

		return result;
	}

	// ------------------------------
	public Set<Map<String, String>> getStringMappedTableParametersWrappedToSession(String cookie, HttpServletRequest req, String param_name, boolean noexcept) throws Exception {
		LinkedHashSet<Map<String, String>> result = new LinkedHashSet<Map<String, String>>();
		Vector<String> raw = getStringListParameterWrappedToSession(cookie, req, param_name, noexcept);

		if (raw.size() > 1) {
			int rows = Integer.valueOf(raw.get(0));
			if (rows < 0) {
				throw new HtException(getContext(), "getStringMappedTableParameters", "Invalid rows number for the table parameter: " + param_name);
			}

			int cols = Integer.valueOf(raw.get(1));
			if (cols < 0) {
				throw new HtException(getContext(), "getStringMappedTableParameters", "Invalid columns number for the table parameter: " + param_name);
			}
			//
			int idx = 2;
			for (int i = 0; i < rows; i++) {
				// column name <-> column value
				LinkedHashMap<String, String> newRow = new LinkedHashMap<String, String>();

				for (int k = 0; k < cols; k++) {

					newRow.put(raw.get(idx), raw.get(idx + 1));
					idx += 2;
				}
				result.add(newRow);
			}
		}

		return result;
	}

	// ------------------------------
	// this is base function to process XML creating get requests
	protected boolean processXmlXGridRequest(String cookie, HttpServletRequest req, HttpServletResponse res)
					throws Exception {
		String operation = getStringParameter(req, "operation", false);

		if (operation.equalsIgnoreCase("get_session_var")) {
			String session_var = getStringParameter(req, "session_var", false);

			res.setHeader("Content-disposition", "attachment; filename=data.xml");
			res.setHeader("Content-Type", "application/octet-stream");
			forceNoCache(res);

			res.getOutputStream().print(getStringSessionValue(cookie, req, session_var));
			res.getOutputStream().close();

			return true;
		}

		return false;
	}

	// ------------------------------
	public String getStringParameter(HttpServletRequest req, String parameter, boolean noexcept) throws Exception {
		String paramValue = req.getParameter(parameter);
		if (paramValue == null) {
			if (noexcept) {
				return "";
			} else {
				throw new HtException(getContext(), "getStringParameter", "Invalid URL parameter: \"" + parameter + "\"");
			}
		}

		return paramValue;
	}
	// ---------------------------
	public Uid getUidParameter(HttpServletRequest req, String parameter, boolean noexcept) throws Exception {
		Uid uid = new Uid();
		String paramValue = req.getParameter(parameter);
		if (paramValue == null) {
			if (noexcept) {
				return uid;
			} else {
				throw new HtException(getContext(), "getStringParameter", "Invalid URL parameter: \"" + parameter + "\"");
			}
		}

		uid.fromString(paramValue);

		return uid;
	}

	// ------------------------------
	public List<HtPair<String, String>> getStringPairListParameter(HttpServletRequest req, String parameter, boolean noexcept) throws Exception {
		ArrayList<HtPair<String, String>> result = new ArrayList<HtPair<String, String>>();

		// this must be comma separeted list
		String param = req.getParameter(parameter);
		if (param == null) {
			if (noexcept) {
				return result;
			} else {
				throw new HtException(getContext(), "getStringPairListParameter", "Invalid URL map parameter: \"" + parameter + "\"");
			}
		}


		param += "dummy";
		// parse comma separated - name, value, name, value, ...
		String[] splitted = param.split(",");
		for (int i = 0; i < splitted.length / 2; i++) {
			String v1 = splitted[i * 2];
			String v2 = splitted[i * 2 + 1];

			if (v1 != null && v1.length() > 0 && v2 != null && v2.length() >= 0) {
				result.add(new HtPair<String, String>(v1, v2));
			}
		}

		return result;
	}

	// ------------------------------
	public long getLongParameter(HttpServletRequest req, String parameter, boolean noexcept) throws Exception {
		String paramValueStr = req.getParameter(parameter);
		if (paramValueStr == null) {
			if (noexcept) {
				return -1;
			} else {
				throw new HtException(getContext(), "getLongParameter", "Invalid URL parameter: \"" + parameter + "\"");
			}
		}

		long val = -1;
		try {
			val = Long.valueOf(paramValueStr);
		} catch (NumberFormatException e) {
			if (noexcept) {
				return -1;
			}
			throw new HtException(getContext(), "getLongParameter", "Exception: \"" + e.getMessage() + "\"");
		}

		return val;
	}

	// ------------------------------
	public int getIntParameter(HttpServletRequest req, String parameter, boolean noexcept) throws Exception {
		String paramValueStr = req.getParameter(parameter);
		if (paramValueStr == null) {
			if (noexcept) {
				return -1;
			} else {
				throw new HtException(getContext(), "getIntParameter", "Invalid URL parameter: \"" + parameter + "\"");
			}
		}

		int val = -1;
		try {
			val = Integer.valueOf(paramValueStr);
		} catch (NumberFormatException e) {
			if (noexcept) {
				return -1;
			}
			throw new HtException(getContext(), "getIntParameter", "Exception: \"" + e.getMessage() + "\"");
		}

		return val;
	}
	
	// ------------------------------
	public boolean getBooleanParameter(HttpServletRequest req, String parameter, boolean noexcept) throws Exception {
		String value = req.getParameter(parameter);

		if (value == null || value.length() <= 0) {
			if (noexcept) {
				return false;
			} else {
				throw new HtException(getContext(), "getBooleanParameter", "Invalid URL parameter: \"" + parameter + "\"");
			}
		}

		return (value.equalsIgnoreCase("true") ? true : false);
	}

	// ---------------------------
	public double getDoubleParameter(HttpServletRequest req, String param_name, boolean noexcept) throws Exception {

		String value = req.getParameter(param_name);

		if (value == null || value.length() <= 0) {
			if (noexcept) {
				return -1;
			} else {
				throw new HtException(getContext(), "getDoubleParameter", "Invalid URL parameter: \"" + param_name + "\"");
			}
		}

		double val = -1;
		try {
			val = Double.valueOf(value);
		} catch (NumberFormatException e) {
			if (noexcept) {
				return -1;
			}
			throw new HtException(getContext(), "getDoubleParameter", "Exception: \"" + e.getMessage() + "\"");
		}

		return val;

	}

	// ------------------------------
	public List<String> getStringListParameter(HttpServletRequest req, String param_name, boolean noexcept) throws Exception {
		ArrayList<String> result = new ArrayList<String>();
		String value = req.getParameter(param_name);

		if (value == null || value.length() <= 0) {
			if (noexcept) {
				return result;
			} else {
				throw new HtException(getContext(), "getStringListParameter", "Invalid URL parameter: \"" + param_name + "\"");
			}
		}

		value += "dummy";
		String[] splitted = value.toString().split(",");

		for (int i = 0; i < splitted.length; i++) {
			result.add(splitted[i]);
		}

		if (result.get(result.size() - 1).equals("dummy")) {
			// remove last element
			result.remove(result.size() - 1);
		}

		return result;

	}
  // ------------------------------
	public List<Integer> getIntListParameter(HttpServletRequest req, String param_name, boolean noexcept) throws Exception {
		ArrayList<Integer> result = new ArrayList<Integer>();
		String value = req.getParameter(param_name);

		if (value == null || value.length() <= 0) {
			if (noexcept) {
				return result;
			} else {
				throw new HtException(getContext(), "getIntListParameter", "Invalid URL parameter: \"" + param_name + "\"");
			}
		}

		value += "dummy";
		String[] splitted = value.toString().split(",");

		for (int i = 0; i < splitted.length; i++) {
			if (!splitted[i].equalsIgnoreCase("dummy"))
				result.add(Integer.valueOf( splitted[i] ));
		}

		return result;

	}

	// ------------------------------
	public Set<Map<String, String>> getStringMappedTableParameters(HttpServletRequest req, String param_name, boolean noexcept) throws Exception {
		LinkedHashSet<Map<String, String>> result = new LinkedHashSet<Map<String, String>>();
		List<String> raw = getStringListParameter(req, param_name, noexcept);

		if (raw.size() > 1) {
			int rows = Integer.valueOf(raw.get(0));
			if (rows <= 0) {
				throw new HtException(getContext(), "getStringMappedTableParameters", "Invalid rows number for the table parameter: " + param_name);
			}

			int cols = Integer.valueOf(raw.get(1));
			if (cols <= 0) {
				throw new HtException(getContext(), "getStringMappedTableParameters", "Invalid columns number for the table parameter: " + param_name);
			}
			//
			int idx = 2;
			for (int i = 0; i < rows; i++) {
				// column name <-> column value
				LinkedHashMap<String, String> newRow = new LinkedHashMap<String, String>();

				for (int k = 0; k < cols; k++) {

					newRow.put(raw.get(idx), raw.get(idx + 1));
					idx += 2;
				}

				result.add(newRow);
			}
		}

		return result;
	}

	// ------------------------------
	protected void forceNoCache(HttpServletResponse response) {
		response.setHeader("Pragma", "No-cache");
		response.setHeader("Cache-Control", "No-cache");
		response.setDateHeader("Expires", 1);
	}

	// ------------------------------
	public void writeHtmlErrorToOutput(HttpServletResponse res, Throwable e) {
		HtWebUtils.writeHtmlErrorToOutput(res, e);
	}

	public String writeHtmlErrorToString(Throwable e) {
		return HtWebUtils.writeHtmlErrorToString(e);
	}

	// ------------------------------
	// restricts length
	public String restrictLength(String inStr, int strLen) {

		if (inStr == null || inStr.length() <= 0) {
			return "";
		}

		int lastInd = -1;
		if (strLen <= 0) {
			lastInd = inStr.length();
		} else {
			if (strLen > inStr.length()) {
				lastInd = inStr.length();
			} else {
				lastInd = strLen;
			}
		}

		StringBuilder out = new StringBuilder();
		out.append(inStr.substring(0, lastInd));

		if (out.length() < inStr.length()) {
			out.append("...");
		}

		return out.toString();
	}

	// ------------------------------
	public String createValidHtml(String inStr) {

		String tmp = inStr.replaceAll("[<]", "&lt;");
		tmp = tmp.replaceAll("[>]", "&gt;");
		//tmp = tmp.replaceAll("[']", "&quot;");
		//tmp = tmp.replaceAll("[&]", "&amp;");
		return tmp;
	}

	// ------------------------------
	// returns parsed time or empty string
	// for UNIX time
	public String parseTimeToString(double unixTime) {
		if (unixTime <= 0) {
			return "";
		}
		return HtDateTimeUtils.time2String_Gmt((long) (unixTime * 1000));
	}
	// ------------------------------

	public void clearErrorXmlInfo(HttpServletRequest req)
	{
		try {
			setStringSessionValue(getUniquePageId(), req, "error_info", "");
		}
		catch(Throwable e2)
		{

		}
	}

	public void initErrorXmlInfo(HttpServletRequest req, Throwable e)
	{

		try {
			setStringSessionValue(getUniquePageId(), req, "error_info", HtWebUtils.createErrorInfoEntryForXGrid(e));
		}
		catch(Throwable e2)
		{

		}
	}
}
