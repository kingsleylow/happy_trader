/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.mtrunner.client.servlets;

import com.fin.httrader.eventplugins.ht_mt_bridge.helper.HtMtJsonUtils;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.webserver.ajaxutils.AjaxResponse;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response.HtGetPositionHistoryResp;
import com.mtrunner.client.HappyTraderTransport.CachedRequestHelper;
import com.mtrunner.client.HappyTraderTransport.RequestHelper;
import com.mtrunner.client.managers.GateWayServletParamChanger;
import com.mtrunner.client.managers.SettingsManager;
import com.mtrunner.client.userlist.controller.ServerControler;
import java.io.IOException;
import javax.servlet.RequestDispatcher;
import javax.servlet.ServletConfig;
import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.context.support.SpringBeanAutowiringSupport;

import org.apache.http.Header;
import org.apache.http.HttpEntity;
import org.apache.http.HttpEntityEnclosingRequest;
import org.apache.http.HttpHeaders;
import org.apache.http.HttpHost;
import org.apache.http.HttpRequest;
import org.apache.http.HttpResponse;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.AbortableHttpRequest;
import org.apache.http.client.params.ClientPNames;
import org.apache.http.client.params.CookiePolicy;
import org.apache.http.client.utils.URIUtils;
import org.apache.http.entity.InputStreamEntity;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.impl.conn.tsccm.ThreadSafeClientConnManager;
import org.apache.http.message.BasicHeader;
import org.apache.http.message.BasicHttpEntityEnclosingRequest;
import org.apache.http.message.BasicHttpRequest;
import org.apache.http.message.HeaderGroup;
import org.apache.http.params.BasicHttpParams;
import org.apache.http.params.HttpParams;
import org.apache.http.util.EntityUtils;

import javax.servlet.ServletException;
import javax.servlet.http.Cookie;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.Closeable;
import java.io.IOException;
import java.io.OutputStream;
import java.lang.reflect.Constructor;
import java.net.HttpCookie;
import java.net.URI;
import java.util.BitSet;
import java.util.Enumeration;
import java.util.Formatter;
import java.util.Iterator;
import java.util.List;  
import org.apache.log4j.Logger;
import org.json.simple.JSONObject;
import org.json.simple.JSONValue;


// this is just a proxy servlet
@WebServlet(name = "HappyTraderProxyGateServlet")
public class HappyTraderProxyGateServlet extends HttpServlet {
	
	public static interface ParamSubstitutor
	{
		public String changeStringParameter(String commandName, String paramName); 
	};
	
	
	
	static Logger log_m = Logger.getLogger(HappyTraderProxyGateServlet.class.getName());
	
	public static final int GENERAL_ERROR_REPLY_CODE=-1;
	
	
	@Override
  public void init() throws ServletException {
	}
	
	@Override
	protected void doGet(HttpServletRequest req, HttpServletResponse resp) throws ServletException, IOException 
	{
		throw new ServletException("Request is not allowed");
	}
	
	@Override
	protected void doPost(HttpServletRequest req, HttpServletResponse resp) throws ServletException, IOException 
	{
		do_service(req, resp);
	}
	/**
	 *	helpers
	 */
	protected void do_service(HttpServletRequest servletRequest, HttpServletResponse servletResponse)
	 throws ServletException, IOException 
	{
		
		
		
		// num page, company name and account ID
		
		servletResponse.setContentType("text/xml; charset=UTF-8");
		String outData = null;
		
		// this shall be a proxy just chnaging 2 fields
		
		try {
			
			
		
			String request_data = servletRequest.getParameter("request_data");
			log_m.debug("Happy trader proxy servlet request: " + request_data);
			
			if (!HtUtils.nvl(request_data)) {
				JSONObject root_json = (JSONObject) JSONValue.parseWithException(request_data);
				
			
				// parse parameters
				for(Iterator<String> it = root_json.keySet().iterator(); it.hasNext(); ) {
					String key = it.next();
					Object value = root_json.get(key);
					if (value instanceof String) {
						
						String value_s = (String)value;
						
							if (value_s.equalsIgnoreCase(SettingsManager.CHANGE_CONSTANT)) {
								String commandName = HtMtJsonUtils.<String>getJsonField(root_json, "COMMAND_NAME");
								if (!HtUtils.nvl(commandName)) {
									String newValue_s = GateWayServletParamChanger.getInstance().changeStringParameter(commandName, key);
									if (newValue_s != null)
										root_json.put(key, newValue_s);
								}
							}
						
					}
				
					
				}
				
				RequestHelper rh = new RequestHelper();
				
				// change credentials
				rh.addCredentials(root_json, false);
				String new_request_data = root_json.toString();
				outData = rh.makeRequestToHappyTraderServlet( new_request_data);
				
			}
			
	
		}
		catch(Exception e )
		{
			//ajax_resp.setError(GENERAL_ERROR_REPLY_CODE, e.toString());
			log_m.error("Error happened on request: " + e.getMessage());
			outData = AjaxResponse.serializeToString("", e.getMessage(), GENERAL_ERROR_REPLY_CODE);
		}
	
		servletResponse.getOutputStream().print(outData);
		
	}
}
