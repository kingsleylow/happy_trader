/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.mtrunner.client.HappyTraderTransport;

import com.fin.httrader.eventplugins.ht_mt_bridge.helper.HtMtJsonUtils;
import com.fin.httrader.eventplugins.ht_mt_bridge.persistent.MainPersistManager;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.request.HtGetAccountHistoryReq;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.request.HtGetPositionHistoryReq;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response.HtGetAccountHistoryResp;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response.HtGetPositionHistoryResp;
import static com.mtrunner.client.HappyTraderTransport.CachedRequestHelper.log_m;
import static com.mtrunner.client.HappyTraderTransport.RequestHelper.MD5;
import com.mtrunner.client.managers.SettingsManager;
import com.mtrunner.client.utils.GeneralUtils;
import java.io.ByteArrayInputStream;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.Constructor;
import java.nio.charset.StandardCharsets;
import java.security.MessageDigest;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import javax.servlet.http.HttpServletResponse;
import org.apache.commons.lang3.StringEscapeUtils;


import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.NameValuePair;

import org.apache.http.client.HttpClient;
import org.apache.http.client.entity.UrlEncodedFormEntity;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.client.params.ClientPNames;
import org.apache.http.client.params.CookiePolicy;
import org.apache.http.entity.ContentType;
import org.apache.http.entity.InputStreamEntity;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.impl.conn.tsccm.ThreadSafeClientConnManager;
import org.apache.http.message.BasicNameValuePair;
import org.apache.http.params.BasicHttpParams;
import org.apache.http.params.HttpParams;
import org.apache.http.util.EntityUtils;
import org.json.simple.JSONObject;
import org.json.simple.JSONValue;
import org.springframework.beans.factory.annotation.Autowired;


/**
 *
 * @author Administrator
 */
public class RequestHelper {
	
	String url_m;
	String user_m;
	String password_m;
	
	
	//private SettingsManager settings_m;

	public RequestHelper()
	{
		url_m = SettingsManager.getInstance().getHappyTraderUrl();
		user_m = SettingsManager.getInstance().getHappyTraderUser();
		password_m = SettingsManager.getInstance().getHappyTraderPassword();
		
	}
	
	
	public String accountHistoryRequest( String companyName, String accId) throws Exception
	{
		String result =  makeRequestToHappyTraderServlet( getAccontHistoryRequest(user_m, password_m, companyName, accId));
		return result;
	};
	
	public String positionHistoryRequest(String companyName, String accId, String serverName, int typeOfPos) throws Exception
	{
		String result =  makeRequestToHappyTraderServlet( getPositionHistoryRequest(companyName, accId, serverName, typeOfPos, -1, -1));
		return result;
		
	};
	
	
	
	public HtGetPositionHistoryResp parsePositionHistoryRequest(String pos_data) throws Exception
	{
	
		String data_s = getData((JSONObject) JSONValue.parseWithException(pos_data));
		HtGetPositionHistoryResp r = new HtGetPositionHistoryResp(HtGetPositionHistoryReq.COMMAND_NAME);
		r.fromJson((JSONObject) JSONValue.parseWithException(data_s));
		
		return r;
	}
	
	public HtGetAccountHistoryResp parseAccountHistoryRequest(String pos_data) throws Exception
	{
		
		String data_s = getData((JSONObject) JSONValue.parseWithException(pos_data));
		HtGetAccountHistoryResp accr = new HtGetAccountHistoryResp(HtGetAccountHistoryReq.COMMAND_NAME);
		accr.fromJson((JSONObject) JSONValue.parseWithException(data_s));
		
		return accr;
	}
	
	public String getData(JSONObject upper_level_response) throws Exception
	{
		
		long code = HtMtJsonUtils.<Long>getJsonField(upper_level_response, "code");
		if (code != 0)
			throw new Exception("Error on getting data from happy trader srevlet: " + HtMtJsonUtils.<String>getJsonField(upper_level_response, "reason"));
						
		String custom_data_s = HtMtJsonUtils.<String>getJsonField(upper_level_response, "custom_data");
		if (custom_data_s == null || custom_data_s.length()<=0)
			throw new Exception("Invalid custom data");
		
		return custom_data_s;
		
	}
	
	
	
	public String getUser()
	{
		return user_m;
	}
	public String getUrl()
	{
		return url_m;
	}
	public String getPassword()
	{
		return password_m;
	}
	
	public void addCredentials(JSONObject jo, boolean is_simple_answer)
	{
		jo.put("HUSR", MD5(getUser()));
		jo.put("HPW", MD5(getPassword()));
		jo.put("SIMPLE_ANSWER", is_simple_answer);
	}
	
	public String makeRequestToHappyTraderServlet( String data) throws Exception
	{
		HttpParams hcParams = new BasicHttpParams();
    hcParams.setParameter(ClientPNames.COOKIE_POLICY, CookiePolicy.IGNORE_COOKIES);
		
		HttpClient client = createHttpClient(hcParams);
		HttpResponse clientResponse = null;
		String out = null;
		HttpEntity entity = null;
		try {
			HttpPost httppost = new HttpPost(url_m); 

			List<NameValuePair> params = new ArrayList<NameValuePair>();
			params.add(new BasicNameValuePair("request_data", data));
			httppost.setEntity(new UrlEncodedFormEntity(params,"UTF-8"));
			httppost.setHeader("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8");
    
			clientResponse = client.execute(httppost);
			
			int statusCode = clientResponse.getStatusLine().getStatusCode();
			if (statusCode == HttpServletResponse.SC_OK ) {
				entity = clientResponse.getEntity();
				out = GeneralUtils.convertInputStreamToString(entity.getContent(), null);
			}
		}
		finally
		{
			 consumeQuietly(entity);
		}
		
		return out;
 
	}
	
	public static String MD5(String md5) {
		try {
			java.security.MessageDigest md = java.security.MessageDigest.getInstance("MD5");
			byte[] array = md.digest(md5.getBytes());
			StringBuilder sb = new StringBuilder();
			for (int i = 0; i < array.length; ++i) {
				sb.append(Integer.toHexString((array[i] & 0xFF) | 0x100).substring(1, 3));
			}
			return sb.toString();
		} catch (java.security.NoSuchAlgorithmException e) {
		}
		return null;
	}
	
	
	// ----------
	// Helpers
	
	
	protected String getAccontHistoryRequest(String user, String password, String companyName, String accId) throws Exception {
		
		JSONObject jo = new JSONObject();
		jo.put("COMMAND_NAME", "GET_ACCOUNT_HISTORY_REQ");
		jo.put("COMPANY_NAME", companyName);
		jo.put("ACCOUNT_NAME", accId);
		addCredentials( jo, true);
		
	
		return jo.toString();
	}
	
	protected String getPositionHistoryRequest(
			String companyName, 
			String accId, 
			String serverName,
			int typeOfPos,
			long open_pos_filter_time,
			long close_pos_filter_time
	) throws Exception {
		
		
		JSONObject jo = new JSONObject();
		jo.put("COMMAND_NAME", "GET_POSITION_HISTORY_REQ");
		jo.put("COMPANY_NAME", companyName);
		jo.put("SERVER_NAME", serverName);
		jo.put("ACCOUNT_NAME", accId);
		jo.put("OPEN_POS_FILTER_TIME", open_pos_filter_time);
		jo.put("CLOSE_POS_FILTER_TIME", close_pos_filter_time);
		jo.put("INCLUDE_CHANGE_DETAILS", false); // no chnage details
		// only history
		
		
		jo.put("TYPE_OF_POS_TO_SELECT", typeOfPos);
		
		addCredentials( jo, true);
		
	
		return jo.toString();
	}
	
	
	

	
	protected void consumeQuietly(HttpEntity entity) {
    if (entity == null)
			return;
		
		try {
      EntityUtils.consume(entity);
    } catch (IOException e) {//ignore
     
    }
  }
	
	
	
	protected HttpClient createHttpClient(HttpParams hcParams) {
    try {
      //as of HttpComponents v4.2, this class is better since it uses System
      // Properties:
      Class clientClazz = Class.forName("org.apache.http.impl.client.SystemDefaultHttpClient");
      Constructor constructor = clientClazz.getConstructor(HttpParams.class);
      return (HttpClient) constructor.newInstance(hcParams);
    } catch (ClassNotFoundException e) {
      //no problem; use v4.1 below
    } catch (Exception e) {
      throw new RuntimeException(e);
    }
		
		
    //Fallback on using older client:
    return new DefaultHttpClient(new ThreadSafeClientConnManager(), hcParams);
	}
}
