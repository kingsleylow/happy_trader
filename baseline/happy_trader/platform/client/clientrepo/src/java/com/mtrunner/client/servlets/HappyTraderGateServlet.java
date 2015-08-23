/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.mtrunner.client.servlets;

import com.fin.httrader.eventplugins.ht_mt_bridge.persistent.MainPersistManager;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.webserver.ajaxutils.AjaxResponse;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response.HtGetPositionHistoryResp;
import com.mtrunner.client.HappyTraderTransport.CachedRequestHelper;
import com.mtrunner.client.managers.SettingsManager;
import java.io.IOException;
import java.util.Calendar;
import java.util.Date;
import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import org.apache.commons.lang3.time.DateUtils;
import org.json.simple.JSONObject;
import org.springframework.beans.factory.annotation.Autowired;

/**
 *
 * @author Administrator
 */
@WebServlet(name = "HappyTraderGateServlet")
public class HappyTraderGateServlet extends HttpServlet {
	
	public static final int GENERAL_ERROR_REPLY_CODE=-1;
	//public static final int NO_DATA_REPLY_CODE=-101;
	
	//@Autowired
	//private SettingsManager settings_m;
	
	
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
		AjaxResponse ajax_resp = new AjaxResponse();
		servletResponse.setContentType("text/xml; charset=UTF-8");
		
		CachedRequestHelper rh = new CachedRequestHelper(	);
		try {
			String company_name = servletRequest.getParameter("company_name");
			String account_id = servletRequest.getParameter("account_id");
			String server_name_s = servletRequest.getParameter("server_name");
			String num_page_s = servletRequest.getParameter("num_page");
			String page_size_s = servletRequest.getParameter("page_size");
			
			boolean isHistory = Boolean.parseBoolean(servletRequest.getParameter("is_history"));
			
			int num_page = HtUtils.parseInt(num_page_s);
			int page_size = HtUtils.parseInt(page_size_s);
			
			
			if (HtUtils.nvl(company_name) || HtUtils.nvl(account_id) || HtUtils.nvl(num_page_s) ) {
				throw new Exception("Invalid parameters");
			}
			
			HtGetPositionHistoryResp r;
			
			if (isHistory) {
				if (num_page >=1)
					r = rh.positionHistoryRequestHistoryPositions(company_name, account_id, server_name_s, num_page, page_size );
				else
					// all
					r = rh.positionHistoryRequestHistoryPositions(company_name, account_id,server_name_s );
			}
			else{
			
				if (num_page >=1)
					r = rh.positionHistoryRequestOpenPositions(company_name,account_id, server_name_s, num_page, page_size );
				else
					// all 
					r = rh.positionHistoryRequestOpenPositions(company_name,account_id, server_name_s);
			}
			
			
			JSONObject obj = r.toJson();
			ajax_resp.setCustomData(obj.toString());
			
		
			
		}
		catch(Exception e )
		{
			ajax_resp.setError(GENERAL_ERROR_REPLY_CODE, e.toString());
		}
		finally
		{
		}
		
		String data = ajax_resp.serializeToXmlString();
		servletResponse.getOutputStream().print(data);
		
	}
}
